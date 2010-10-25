using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using ZedGraph;

namespace UDPClient
{
    public partial class Form1 : Form
    {
        bool _bStart;        
        ArrayList _Workers, _Th;
        LineItem _myCurve;
        int _iSec;
        public Form1()
        {
            InitializeComponent();
            _bStart = false;
            _Workers = new ArrayList();
            _Th = new ArrayList();
        }

       

        private void butStartStop_Click(object sender, EventArgs e)
        {
            _bStart = !_bStart;
            int i;
            
            if (_bStart)
            {
                _iSec = 0;
                butStartStop.Text = "Stop";
                byte[] byA = new byte[13];
                byte[] byV = new byte[1024];
                byA[0] = byV[0] = 11;
                int ii = 0;
                listBox1.Items.Clear();
                for (i = 0; i < Convert.ToInt32(txtThread.Text); i++)
                {                       
                    Worker workV = new Worker();                    
                    _Workers.Add(workV);
                    if (ii == 0 && cmbCmd.SelectedIndex == 0)
                        byV[0] = 13;
                    else if (cmbCmd.SelectedIndex == 1)
                        byV[0] = 13;
                    else if (cmbCmd.SelectedIndex == 2)
                        byV[0] = 11;
                    else if (ii != 0 && cmbCmd.SelectedIndex == 0)
                        byV[0] = 11;
                    workV.Init(txtServer.Text, 9051, byV, 66, ii, true, cmbMode.SelectedIndex); 
                    listBox1.Items.Add(ii.ToString() + "_Video");                    
                    Thread thV = new Thread(workV.DoWork);
                    _Th.Add(thV);
                    thV.Start();
                    workV.StartStop(true);

                    ii++;
                    if (cmbMode.SelectedIndex != 1)
                    {
                        Worker workA = new Worker();
                        _Workers.Add(workA);
                        workA.Init(txtServer.Text, 9052, byA, 18, ii, false, cmbMode.SelectedIndex); 
                        listBox1.Items.Add(ii.ToString() + "_Audio");
                        ii++;
                        Thread thA = new Thread(workA.DoWork);
                        _Th.Add(thA);
                        thA.Start();
                        workA.StartStop(true);
                    }
                }
            }
            else
            {
                butStartStop.Text = "Start";
                for (i = 0; i < _Workers.Count; i++)
                {
                    Worker w = (Worker)_Workers[i];
                    w.RequestStop();
                    Thread t = (Thread)_Th[i];
                    //t.Join();
                }
                _Workers.Clear();
                _Th.Clear();

            }
            timer1.Enabled = _bStart;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (_bStart)
                butStartStop_Click(null, null);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            float iTotTX = 0, iTotRX = 0;
            int i;
            for (i = 0; i < _Workers.Count; i++)
            {
                Worker w = (Worker)_Workers[i];
                iTotTX += w.GetTX();
                iTotRX += w.GetRX();
            }
            if (iTotTX > 0)
                iTotTX /= 1000;
            if (iTotRX > 0)
                iTotRX /= 1000;     
            lblTx.Text = iTotTX.ToString();
            lblRx.Text = iTotRX.ToString();            
            _iSec++;
            lblTime.Text = _iSec.ToString();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            cmbMode.SelectedIndex = 0;
            cmbCmd.SelectedIndex = 0;
            CreateGraph(zedGraphControl1);
        }
        private void CreateGraph(ZedGraphControl zgc)
        {
            GraphPane myPane = zgc.GraphPane;

            // Set the titles and axis labels
            myPane.Title.Text = "Performance Answer Times";
            myPane.XAxis.Title.Text = "Frames";
            myPane.YAxis.Title.Text = "Time answers (ms)";

            // Make up some data points from the Sine function
            PointPairList list = new PointPairList();
            list.Add(0, 0);

            // Generate a blue curve with circle symbols, and "My Curve 2" in the legend
            _myCurve = myPane.AddCurve("My Curve", list, Color.Blue,
                                    SymbolType.Circle);
            
            // Fill the area under the curve with a white-red gradient at 45 degrees
           // _myCurve.Line.Fill = new Fill(Color.White, Color.Red, 45F);
            // Make the symbols opaque by filling them with white
            //_myCurve.Symbol.Fill = new Fill(Color.White);

            // Fill the axis background with a color gradient
            myPane.Chart.Fill = new Fill(Color.White, Color.LightGoldenrodYellow, 45F);

            // Fill the pane background with a color gradient
            myPane.Fill = new Fill(Color.White, Color.FromArgb(220, 220, 255), 45F);

            // Calculate the Axis Scale Ranges
            zgc.AxisChange();
        }      
        void SetAxis(int iIDThread){

            FileStream f = File.Open(iIDThread.ToString() + ".bin", FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            BinaryReader b = new BinaryReader(f);
            int iVal;

            _myCurve.Clear();
            byte[] byArray = new byte[4];
            int iFrame = 0;
            try{
                while (true)
                {
                    iVal = b.ReadInt32();
                    PointPair p = new PointPair(iFrame, iVal);
                    iFrame++;
                    _myCurve.AddPoint(p);
                }
            }
            catch (Exception) { }
            zedGraphControl1.AxisChange();
            zedGraphControl1.Refresh();
            b.Close();
            f.Close();
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string curItem = listBox1.SelectedItem.ToString();
            int index = listBox1.FindString(curItem);
            if (index >= 0)
                SetAxis(index);
        }

    }

    public class Worker
    {
        UdpClient client;
        bool _bSend;
        byte[] _byData;
        int _iSleep, _iRX, _iTX;
        int _iPort, _iIDThread;
        FileStream _fs;
        bool _bVideo;
        int _iCnt, _iMode, _iCmd;
        EndPoint _ep;
        Socket _sock;

        public int GetRX() {
            int itmp = _iRX;
            _iRX = 0;
            return itmp;            
        }
        public int GetTX() {
            int itmp = _iTX;
            _iTX = 0; 
            return itmp;            
        }

        public void Init(String sServer, int iPort, byte []byData, int iSleep, int iIDThread, bool bVideo, int iMode)
        {            
            _iMode = iMode;
            _iPort = iPort;
            _bVideo = bVideo;
            _iIDThread = iIDThread;
            if (iMode==1){
                _sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                _sock.ReceiveTimeout =2000;
                IPEndPoint iep = new IPEndPoint(IPAddress.Any, _iPort);
                _sock.Bind(iep);
                _ep = (EndPoint)iep;                
            }
            else{
                client = new UdpClient(sServer, _iPort);
                client.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 2000);
            }
            

            _bSend = false;
            _byData = new byte[byData.Length];
            Array.Copy(byData, _byData, byData.Length);
            _iSleep = iSleep;            
        }
        public void StartStop(bool bStart)
        {
            if (bStart)
            {   _iRX = 0; 
                _iTX = 0;
            }
            _bSend = bStart;
            _iCnt = 0;
        }

        // This method will be called when the thread is started.
        public void DoWork()
        {            
            byte[] data = new byte[20000];
            IPEndPoint iep = new IPEndPoint(IPAddress.Any, _iPort);
            _fs = File.Open(_iIDThread.ToString() + ".bin", FileMode.Create, FileAccess.Write, FileShare.ReadWrite);
            BinaryWriter b = new BinaryWriter(_fs);
            int iT1, iT2=0, iTS;
            while (!_shouldStop)
            {
                if (_bSend)
                {
                    iT1 = Environment.TickCount;
                    if (_iMode==0 || _iMode==2){
                        iTS = Environment.TickCount;
                        _byData[1] = (byte)(iTS & 0x000000FF);
                        _byData[2] = (byte)((iTS >> 8) & 0x000000FF);
                        _byData[3] = (byte)((iTS >> 16) & 0x000000FF);
                        _byData[4] = (byte)((iTS >> 24) & 0x000000FF);
                        _byData[5] = (byte)(_iCnt & 0x000000FF);
                        _byData[6] = (byte)((_iCnt >> 8) & 0x000000FF);
                        _byData[7] = (byte)((_iCnt >> 16) & 0x000000FF);
                        _byData[8] = (byte)((_iCnt >> 24) & 0x000000FF);
                        client.Send(_byData, _byData.Length);
                        _iTX += _byData.Length;
                    }
                    

                    try{
                        if (_iMode == 2)
                        {
                            data = client.Receive(ref iep);                            
                        }
                        else if (_iMode==1){
                            _sock.ReceiveFrom(data, ref _ep);                            
                        }
                        if (_iMode == 1 || _iMode == 2)
                        {
                            iT2 = data[4]; iT2 <<= 8;
                            iT2 |= data[3]; iT2 <<= 8;
                            iT2 |= data[2]; iT2 <<= 8;
                            iT2 |= data[1]; iT2 <<= 8;
                            iT2 = Environment.TickCount - iT2;
                            _iRX += data.Length;
                        }                        
                        
                    }
                    catch (Exception)
                    {
                        iT2 = 0;
                        Console.WriteLine("Lost Packet");
                    }
                    b.Write(iT2);
                    if (iT2<_iSleep && (_iMode==0 || _iMode==2))
                        Thread.Sleep(_iSleep-iT2);
                    _iCnt++;
                    //Console.WriteLine("Counter:"+_iCnt.ToString());
                }
            }
            if (_sock!=null)
                _sock.Close();
            b.Close();
            _fs.Close();
            StartStop(false);
            Console.WriteLine("worker thread: terminating gracefully.");
        }
        public void RequestStop()
        {
            _shouldStop = true;
        }
        // Volatile is used as hint to the compiler that this data
        // member will be accessed by multiple threads.
        private volatile bool _shouldStop;
    }

}

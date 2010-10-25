using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net.Sockets;
using System.Net;
using System.Collections;

namespace VideoCallServer
{
    public partial class Form1 : Form
    {
        TCPServer _sockServer;
        public UDPServer _udpServer;
        ArrayList _listUsr;
        int _iBytesRx;
        public Form1()
        {
            InitializeComponent();
            _listUsr = new ArrayList();
        }

        public String GetUsrLst(String sIP)
        {
            try
            {
                int i; String sRet = "";

                for (i = 0; i < _listUsr.Count; i++)
                {
                    User usr = (User)_listUsr[i];
                    if (usr.GetIP()!=sIP)
                        sRet += usr.GetUser() + "|";
                }
                return sRet;                
            }
            catch (Exception)
            {
            }
            return "";
        }

        delegate void UpdateUserListCallback(string sUsr, String sIP, string sPortVideo, string sPortAudio, bool bAdd, Socket sck);
        public void UpdateUserList(string sUsr, String sIP, string sPortVideo, string sPortAudio, bool bAdd, Socket sck)
        {
            try
            {   
                if (!lstUsers.InvokeRequired)
                {
                    // remove if exist
                    int i;
                    for (i = 0; i < _listUsr.Count; i++)
                    {
                        User tmp = (User)_listUsr[i];
                        if (tmp.GetUser() == sUsr)
                        {
                            _listUsr.Remove(tmp);
                            lstUsers.Items.Remove(sUsr);
                        }
                    }
                    if (bAdd){
                        User usr = new User(sUsr, sIP, sck);
                        usr.SetIepAudio(Convert.ToInt32(sPortAudio));
                        usr.SetIepVideo(Convert.ToInt32(sPortVideo));
                        _listUsr.Add(usr);
                        lstUsers.Items.Add(sUsr);                        
                    }                    
                }
                else
                {
                    UpdateUserListCallback d = new UpdateUserListCallback(UpdateUserList);
                    Invoke(d, new object[] { sUsr, sIP, sPortVideo, sPortAudio, bAdd, sck });
                }
            }
            catch (Exception)
            {
            }
        }
        public User GetUserFromIPNoPort(String sIP)
        {
            string sAddr = sIP.Substring(0,sIP.LastIndexOf(':'));
            User usr = null;
            try
            {
                int i;
                for (i = 0; i < _listUsr.Count && usr == null; i++)
                {
                    User tmp = (User)_listUsr[i];
                    if (tmp.GetOnlyIP() == sAddr)
                        usr = tmp;
                }
            }
            catch (Exception)
            {
            }
            return usr;
        }
        public User GetUserFromIP(String sIP)
        {
            User usr = null;
            try
            {
                int i;
                for (i = 0; i < _listUsr.Count && usr == null; i++)
                {
                    User tmp = (User)_listUsr[i];
                    if (tmp.GetIP() == sIP)
                        usr = tmp;
                }
            }
            catch (Exception)
            {
            }
            return usr;
        }
        public User GetUser(String sUser)
        {
            User usr=null;
            try
            {
                int i;
                for (i = 0; i < _listUsr.Count && usr==null; i++)
                {                        
                    User tmp =(User)_listUsr[i];
                    if (tmp.GetUser()==sUser)
                        usr = tmp;
                }                   
            }
            catch (Exception)
            {
            }
            return usr;
        }

        public bool HeartBeatUser(String sIP)
        {
            bool bFound = false;
            try
            {
                int i;
                
                for (i = 0; i < _listUsr.Count && !bFound; i++){
                    User tmp = (User)_listUsr[i];
                    if (tmp.GetIP()==sIP){
                        tmp.SetHearBeat(true);                        
                        bFound=true;
                    }
                }                
            }
            catch (Exception)
            {
            }
            return bFound;
        }
        
        delegate void RemoveUserDeadCallback();
        public void RemoveUserDead()
        {
            try
            {
                if (!lstUsers.InvokeRequired)
                {
                    int i;
                    for (i = 0; i < _listUsr.Count; i++)
                    {
                        User tmp = (User)_listUsr[i];
                        if (tmp.GetIEPConvVideo() == null && tmp.GetIEPConvAudio() == null)
                        {
                            if (!tmp.GetHearBeat())
                            {
                                lstUsers.Items.Remove(tmp.GetUser());
                                _listUsr.Remove(tmp);
                                tmp = null;
                            }
                            else
                                tmp.SetHearBeat(false);
                        }
                    }
                }
                else
                {
                    RemoveUserDeadCallback d = new RemoveUserDeadCallback(RemoveUserDead);
                    Invoke(d, new object[] {  });
                }
            }
            catch (Exception)
            {
            }
        }

        delegate void AddLogCallback(string Message, string sFolder);
        public void AddLog(string Message, string sFolder)
        {   try
            {   if (!lstLog.InvokeRequired)
                {   DateTime dateTime = DateTime.Now;
                    Message = dateTime.ToShortDateString() + " " + dateTime.Hour + ":" + dateTime.Minute + ":" + dateTime.Second + " " + Message;
                    this.lstLog.Items.Insert(0, Message);
                    if (sFolder != "")
                    {
                        TextWriter tr = new StreamWriter(sFolder + "\\log.txt", true);
                        tr.WriteLine(Message);
                        tr.Close();
                    }
                }
                else
                {   AddLogCallback d = new AddLogCallback(AddLog);
                    Invoke(d, new object[] { Message, sFolder });
                }
            }
            catch (Exception)
            {
            }
        }
        
        public void UpdateBytesRX(int iBytes)
        {
            _iBytesRx += iBytes;                 
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // init ip socket server
            //IPAddress ip = IPAddress.Parse("192.168.2.3");
            _sockServer = new TCPServer(8082);            
            _udpServer = new UDPServer();
            

            _udpServer.StartServer();
            _sockServer.StartServer();
            timer1.Enabled = true;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            _udpServer.StopServer();
            _sockServer.StopServer();
        }

        private void butClearLog_Click(object sender, EventArgs e)
        {
            lstLog.Items.Clear();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Text = "Video Call Server (" + _sockServer.m_socketListenersList.Count.ToString() + ")";
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            RemoveUserDead();
        }

        private void timer3_Tick(object sender, EventArgs e)
        {
            if (_iBytesRx>0)
            {
                lblRx.Text = (_iBytesRx / 1000).ToString();
                _iBytesRx = 0;
            }
            else
                lblRx.Text = "0";
        }
    }
}

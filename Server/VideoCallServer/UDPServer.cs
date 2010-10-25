using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Windows.Forms;
using System.IO;

namespace VideoCallServer
{
    public class UDPServer
    {
        private const int CMD_LOGIN         = 0;
        private const int CMD_LOGOUT        = 1;
        private const int CMD_CALL          = 2;
        private const int CMD_CALL_ACK      = 3;
        private const int CMD_CALL_YESNO    = 4;
        private const int CMD_FRM_VIDEO     = 5;
        private const int CMD_FRM_AUDIO     = 6;
        private const int CMD_GET_USR_LST   = 7;
        private const int CMD_CLOSE_CONV    = 8;
        private const int CMD_CLOSECONV_ACK = 9;
        private const int CMD_HEARTBEAT     = 10;
        private const int CMD_ECHO          = 11;
        private const int CMD_BIND_USER     = 12;

        private const int ACK = 6;
        private const int NACK = 21;
        

        Thread _UdpThreadCmds, _UdpThreadAudio, _UdpThreadVideo;
        bool _bStopServer;
        int _iPortAudio, _iPortVideo, _iPortCmd;
        public UDPServer()
        {
            _iPortAudio = 9052;
            _iPortVideo = 9051;
            _iPortCmd   = 9050;
        }
        public void StartServer()
        {
            try
            {
                //Starting the UDP Server thread.
                _bStopServer = false;
                /*_UdpThreadCmds = new Thread(new ThreadStart(UDPThreadCmd));
                _UdpThreadCmds.Start();*/
                _UdpThreadAudio = new Thread(new ThreadStart(UDPThreadAudio));
                _UdpThreadAudio.Start();
                _UdpThreadVideo = new Thread(new ThreadStart(UDPThreadVideo));
                _UdpThreadVideo.Start();
            }
            catch (Exception e)
            {
                Console.WriteLine("An UDP Exception has occurred!" + e.ToString());
            }
        }
        /// <summary>
        /// UDP Server Thread for audio frames
        /// </summary>
        private void UDPThreadAudio()
        {
            Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            sock.ReceiveBufferSize = 10000000;
            sock.ReceiveTimeout = 900;
            IPEndPoint iep = new IPEndPoint(IPAddress.Any, _iPortAudio);
            sock.Bind(iep);
            EndPoint ep = (EndPoint)iep;

            int iRead;
            byte[] data = new byte[10000];
            IPEndPoint iepConv;
            System.Text.Encoding enc;
            enc = System.Text.Encoding.ASCII;
            while (!_bStopServer)
            {
                try
                {
                    iRead = sock.ReceiveFrom(data, ref ep);
                    if (iRead > 0)
                    {
                        //Console.WriteLine("Thread Audio:" + iRead.ToString());
                        /*int iCnt = data[2];
                        iCnt <<= 8;
                        iCnt |= data[1];*/
                        switch (data[0])                            
                        {
                            case CMD_ECHO:
                                sock.SendTo(data, iRead, SocketFlags.None, ep);
                                Program.MainForm.UpdateBytesRX(iRead);
                                break;
                            case CMD_FRM_AUDIO:
                                User usr = Program.MainForm.GetUserFromIPNoPort(ep.ToString());
                                //Console.WriteLine(usr.GetUser() + " > Audio ");
                                iepConv = usr.GetIEPConvAudio();
                                if (iepConv != null)
                                    sock.SendTo(data, iRead, SocketFlags.None, iepConv);
                                break;
                            case CMD_BIND_USER:
                                data[iRead] = 0;
                                string sTmp = enc.GetString(data, 1, iRead - 1);
                                User usrReceiver = Program.MainForm.GetUser(sTmp);
                                User usrCaller = Program.MainForm.GetUserFromIPNoPort(ep.ToString());
                                if (usrReceiver != null)
                                {
                                    usrCaller.SetIepAudio(ep.ToString());
                                    usrCaller.SetIEPConvAudio(usrReceiver.GetIEPAudio());
                                    String sCal = usrCaller.GetIEPVideo().Address + ":" + usrCaller.GetIEPVideo().Port;
                                    String sDst = usrReceiver.GetIEPVideo().Address + ":" + usrReceiver.GetIEPVideo().Port;
                                    Program.MainForm.AddLog("AUDIO BINDING :" + usrCaller.GetUser() + " " + sCal + "-> " + usrReceiver.GetUser() + " " + sDst, "");

                                    data[1] = ACK;
                                    sock.SendTo(data, 2, SocketFlags.None, ep);
                                }
                                else
                                {
                                    Program.MainForm.AddLog(usrCaller.GetUser() + " CANNOT AUDIO BIND ", "");
                                    data[1] = NACK;
                                    sock.SendTo(data, 2, SocketFlags.None, ep);
                                }
                                break;
                        }
                    }
                    

                }
                catch (Exception)
                {
                }
            }
            sock.Close();
        }

        /// <summary>
        /// UDP Server Thread for video frames
        /// </summary>
        private void UDPThreadVideo()
        {
            Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            sock.ReceiveBufferSize = 10000000;
            sock.ReceiveTimeout = 900;
            IPEndPoint iep = new IPEndPoint(IPAddress.Any, _iPortVideo);
            sock.Bind(iep);
            EndPoint ep = (EndPoint)iep;

            int iRead;
            byte[] data = new byte[10000];
            IPEndPoint iepConv;
            System.Text.Encoding enc;
            enc = System.Text.Encoding.ASCII;
            while (!_bStopServer)
            {
                try
                {
                    iRead = sock.ReceiveFrom(data, ref ep);
                    if (iRead > 0)
                    {
                        /*int iCnt = data[2];
                        iCnt <<= 8;
                        iCnt |= data[1];*/
                        //Console.WriteLine("Thread Video:" + iRead.ToString());
                        switch (data[0])
                        {
                            case CMD_ECHO:
                                sock.SendTo(data, iRead, SocketFlags.None, ep);
                                Program.MainForm.UpdateBytesRX(iRead);
                                break;
                            case CMD_FRM_VIDEO:
                                User usr = Program.MainForm.GetUserFromIPNoPort(ep.ToString());                                
                                iepConv = usr.GetIEPConvVideo();
                                if (iepConv != null)
                                {
                                    //Console.WriteLine(usr.GetUser() + " > Video ");
                                    sock.SendTo(data, iRead, SocketFlags.None, iepConv);                                    
                                }
                                else
                                    Console.Write(usr.GetUser() + " NOT BINDED");
                                break;
                            case CMD_BIND_USER:
                                data[iRead] = 0;
                                string sTmp = enc.GetString(data, 1, iRead - 1);
                                User usrReceiver = Program.MainForm.GetUser(sTmp);
                                User usrCaller = Program.MainForm.GetUserFromIPNoPort(ep.ToString());
                                if (usrReceiver != null)
                                {
                                    usrCaller.SetIepVideo(ep.ToString());
                                    usrCaller.SetIEPConvVideo(usrReceiver.GetIEPVideo());
                                    String sCal = usrCaller.GetIEPVideo().Address + ":"+ usrCaller.GetIEPVideo().Port;
                                    String sDst = usrReceiver.GetIEPVideo().Address + ":" + usrReceiver.GetIEPVideo().Port;
                                    Program.MainForm.AddLog("VIDEO BINDING curr:"+ ep.ToString() +" --" + usrCaller.GetUser() + " " +sCal + "-> " + usrReceiver.GetUser() + " " +sDst, "");
                                    data[1] = ACK;
                                    sock.SendTo(data, 2, SocketFlags.None, ep);
                                }
                                else
                                {
                                    Program.MainForm.AddLog(usrCaller.GetUser() + " CANNOT VIDEO BIND ", "");
                                    data[1] = NACK;
                                    sock.SendTo(data, 2, SocketFlags.None, ep);
                                }
                                break;
                        }
                    }
                    
                    
                }
                catch (Exception)
                {
                }
            }
            sock.Close();
        }
        /// <summary>
        /// UDP Server Thread for commands
        /// </summary>
        private void UDPThreadCmd()
        {
            Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            sock.ReceiveTimeout = 1000;
            IPEndPoint iep = new IPEndPoint(IPAddress.Any, _iPortCmd);
            sock.Bind(iep);
            EndPoint ep = (EndPoint)iep;            

            int iRead;
            byte[] data = new byte[10000];
            String sTmp;
            System.Text.Encoding enc;
            enc = System.Text.Encoding.ASCII;
            User usrReceiver, usrCaller;
            
            int iCnt = 0;
            while (!_bStopServer)
            {
                try
                {
                    iRead = sock.ReceiveFrom(data, ref ep);
                    Thread.Sleep(100);
                    
                    if (iRead > 0)
                    {
                        Console.WriteLine("THREAD CMD RX:{0} {1}", data[0], ep.ToString());
                        
                        switch (data[0])
                        {   
                            case CMD_ECHO :
                                sock.SendTo(data, iRead, SocketFlags.None, ep);
                                break;
                            case CMD_LOGIN:
                                data[iRead] = 0;
                                sTmp = enc.GetString(data, 1, iRead - 1);
                                string[] sinfo = sTmp.Split(';');
                                Console.WriteLine("CMD_LOGIN " + sinfo[0]);
                                Program.MainForm.UpdateUserList(sinfo[0], ep.ToString(), sinfo[1], sinfo[2], true, null);
                                Program.MainForm.AddLog("Login:" + sinfo[0], "");
                                data[1] = ACK;
                                sock.SendTo(data, 2, SocketFlags.None, ep);                                
                                break;
                            case CMD_LOGOUT:                                
                                data[iRead] = 0;                                
                                sTmp = enc.GetString(data, 1, iRead - 1);
                                Console.WriteLine("CMD_LOGOUT " + sTmp);
                                Program.MainForm.UpdateUserList(sTmp, ep.ToString(), null, null, false, null);
                                Program.MainForm.AddLog("Logout:" + sTmp, "");
                                data[1] = ACK;
                                sock.SendTo(data, 2, SocketFlags.None, ep);
                                break;
                            case CMD_GET_USR_LST:
                                sTmp = Program.MainForm.GetUsrLst(ep.ToString());
                                int iSize = sTmp.Length;
                                if (iSize > 0)
                                {
                                    Byte[] byUsr = enc.GetBytes(sTmp);
                                    Array.Copy(byUsr, 0, data, 1, iSize);
                                }
                                sock.SendTo(data, iSize+1, SocketFlags.None, ep);
                                break;
                            case CMD_HEARTBEAT:
                                iCnt = data[2];
                                iCnt <<= 8;
                                iCnt |= data[1];

                                if (Program.MainForm.HeartBeatUser(ep.ToString()))
                                {
                                    data[3] = ACK;
                                    Console.WriteLine("CMD_HEARTBEAT ACK:"+iCnt.ToString());
                                }
                                else
                                {
                                    Console.WriteLine("CMD_HEARTBEAT NACK"+iCnt.ToString());
                                    data[3] = NACK;
                                }
                                sock.SendTo(data, 4, SocketFlags.None, ep);
                                break;
                            
                            case CMD_CLOSECONV_ACK:
                                data[iRead] = 0;
                                sTmp = enc.GetString(data, 2, iRead - 2);
                                usrReceiver = Program.MainForm.GetUser(sTmp);
                                usrCaller = Program.MainForm.GetUserFromIP(ep.ToString());
                                usrCaller.SetIEPConvVideo(null);
                                usrCaller.SetIEPConvAudio(null);
                                if (usrReceiver != null)
                                {
                                    string sUsrCaller = usrCaller.GetUser();
                                    Program.MainForm.AddLog(sUsrCaller + " ACK CLOSE CONV -> " + sTmp, "");
                                    data[1] = ACK;
                                    Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 2, sUsrCaller.Length);
                                    sock.SendTo(data, sUsrCaller.Length + 2, SocketFlags.None, usrReceiver.GetIEPCmd());
                                }
                                else
                                {
                                    Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T ANSWER CMD_CLOSECONV_ACK -> " + sTmp, "");
                                }
                                break;

                            case CMD_CLOSE_CONV:
                                data[iRead] = 0;
                                sTmp = enc.GetString(data, 1, iRead - 1);
                                usrReceiver = Program.MainForm.GetUser(sTmp);
                                usrCaller = Program.MainForm.GetUserFromIP(ep.ToString());
                                usrCaller.SetIEPConvVideo(null);
                                usrCaller.SetIEPConvAudio(null);
                                if (usrReceiver != null)
                                {
                                    string sUsrCaller = usrCaller.GetUser();
                                    Program.MainForm.AddLog(sUsrCaller + " CLOSE CONVERSATION -> " + sTmp, "");
                                    Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 1, sUsrCaller.Length);
                                    sock.SendTo(data, sUsrCaller.Length + 1, SocketFlags.None, usrReceiver.GetIEPCmd());
                                }
                                else
                                {
                                    Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T CLOSE -> " + sTmp, "");
                                    // send nack
                                    data[0] = CMD_CLOSECONV_ACK;
                                    data[1] = ACK;
                                    Array.Copy(enc.GetBytes(sTmp), 0, data, 2, sTmp.Length);
                                    sock.SendTo(data, sTmp.Length + 2, SocketFlags.None, ep);
                                }
                                break;

                            case CMD_CALL_YESNO:
                                data[iRead] = 0;
                                sTmp = enc.GetString(data, 2, iRead - 2);
                                usrReceiver = Program.MainForm.GetUser(sTmp);
                                usrCaller = Program.MainForm.GetUserFromIP(ep.ToString());
                                if (usrReceiver != null)
                                {
                                    string sUsrCaller = usrCaller.GetUser();
                                    if (data[1] == ACK)
                                    {
                                        Program.MainForm.AddLog(sUsrCaller + " ACCEPT CONV -> " + sTmp, "");
                                        //usrCaller.SetIEPConv(usrReceiver.GetIEP());
                                    }
                                    else
                                    {
                                        usrCaller.SetIEPConvVideo(null);
                                        usrCaller.SetIEPConvAudio(null);
                                        Program.MainForm.AddLog(sUsrCaller + " REJECT CONV -> " + sTmp, "");
                                    }
                                    Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 2, sUsrCaller.Length);
                                    sock.SendTo(data, sUsrCaller.Length + 2, SocketFlags.None, usrReceiver.GetIEPCmd());
                                }
                                else
                                {
                                    Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T ANSWER CMD_CALL_YESNO -> " + sTmp, "");
                                    // send reject cmd
                                    data[0] = CMD_CALL_YESNO;
                                    data[1] = 0;
                                    Array.Copy(enc.GetBytes(sTmp), 0, data, 2, sTmp.Length);
                                    sock.SendTo(data, sTmp.Length + 2, SocketFlags.None, ep);
                                }
                                break;
                            case CMD_CALL_ACK:
                                data[iRead] = 0;
                                sTmp = enc.GetString(data, 2, iRead - 2);
                                usrReceiver = Program.MainForm.GetUser(sTmp);
                                usrCaller = Program.MainForm.GetUserFromIP(ep.ToString());
                                if (usrReceiver != null)
                                {
                                    string sUsrCaller = usrCaller.GetUser();
                                    Program.MainForm.AddLog(sUsrCaller + " ACK CALLING -> " + sTmp, "");
                                    data[1] = ACK;
                                    Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 2, sUsrCaller.Length);
                                    sock.SendTo(data, sUsrCaller.Length + 2, SocketFlags.None, usrReceiver.GetIEPCmd());
                                }
                                else
                                {
                                    Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T ANSWER CMD_CALL_ACK -> " + sTmp, "");
                                }
                                break;
                            case CMD_CALL:
                                data[iRead] = 0;
                                sTmp = enc.GetString(data, 1, iRead - 1);
                                usrReceiver = Program.MainForm.GetUser(sTmp);
                                usrCaller = Program.MainForm.GetUserFromIP(ep.ToString());                                
                                if (usrReceiver != null)
                                {
                                    string sUsrCaller = usrCaller.GetUser();
                                    Program.MainForm.AddLog(sUsrCaller + " CALLING -> " + sTmp, "");
                                    Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 1, sUsrCaller.Length);
                                    sock.SendTo(data, sUsrCaller.Length + 1, SocketFlags.None, usrReceiver.GetIEPCmd());
                                }
                                else
                                {
                                    Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T CALL -> " + sTmp, "");
                                    // send nack
                                    data[0] = CMD_CALL_ACK;
                                    data[1] = NACK;
                                    Array.Copy(enc.GetBytes(sTmp), 0, data, 2, sTmp.Length);
                                    sock.SendTo(data, sTmp.Length + 2, SocketFlags.None, ep);
                                }
                                break;
                        } 
                    }
                }
                catch (Exception)
                {
                }
            }
            sock.Close();
        }
        
        /// <summary>
        /// Method that stops the TCP/IP Server.
        /// </summary>
        public void StopServer()
        {
            _bStopServer = true;
            Thread.Sleep(1000);
            /*if (_UdpThreadCmds.IsAlive)
                _UdpThreadCmds.Abort();*/
            if (_UdpThreadAudio.IsAlive)
                _UdpThreadAudio.Abort();
            if (_UdpThreadVideo.IsAlive)
                _UdpThreadVideo.Abort();
        }
    }
}

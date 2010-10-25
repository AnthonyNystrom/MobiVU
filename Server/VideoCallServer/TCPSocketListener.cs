using System;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace VideoCallServer
{
    /// <summary>
    /// TCPSocketListener is the direct socket comunication server with client. into is implemented the protocol.
    /// </summary>
    public class TCPSocketListener
    {
        /// <summary>
        /// Variables that are accessed by other classes indirectly.
        /// </summary>
        public Socket m_clientSocket = null;
        /// <summary>
        /// Working Variables.
        /// </summary>
        private DateTime m_lastReceiveDateTime;
        private DateTime m_currentReceiveDateTime;
        private bool m_stopClient = false;
        private Thread m_clientListenerThreadPhone = null;
        private bool m_markedForDeletion = false;
        private System.Threading.Timer _t;
        private int _iStatus = 0;
        private int _iSizePack = 0;
        private int _iCurrentIdx = 0;
        private bool _bHalfSize = false, _bConvOn=false;
        private Byte _byHalfSize;

        private const int SIZE_BUFFER = 1024;
        private Byte[] _byBuffer = new Byte[SIZE_BUFFER];
        private Byte[] _byBufferRx = new Byte[SIZE_BUFFER];
        private Byte[] _byBufferTx = new Byte[SIZE_BUFFER];
        private TCPServer m_Server;

        private const int CMD_LOGIN = 0;
        private const int CMD_LOGOUT = 1;
        private const int CMD_CALL = 2;
        private const int CMD_CALL_ACK = 3;
        private const int CMD_CALL_YESNO = 4;
        private const int CMD_FRM_VIDEO = 5;
        private const int CMD_FRM_AUDIO = 6;
        private const int CMD_GET_USR_LST = 7;
        private const int CMD_CLOSE_CONV = 8;
        private const int CMD_CLOSECONV_ACK = 9;
        private const int CMD_HEARTBEAT = 10;
        private const int CMD_ECHO = 11;
        private const int CMD_BIND_USER = 12;

        private const int TIMEOUT_CONNECTION = 20000;

        private const int ACK = 6;
        private const int NACK = 21;

        /// <summary>
        /// Client Socket Listener Constructor.
        /// </summary>
        /// <param name="clientSocket">Phone connection</param>
        public TCPSocketListener(Socket clientSocket, TCPServer server)
        {
            m_Server = server;
            m_clientSocket = clientSocket;

        }

        /// <summary>
        /// Client SocketListener Destructor.
        /// </summary>
        ~TCPSocketListener()
        {
            StopSocketListener();
        }


        /// <summary>
        /// Method that starts SocketListener Thread.
        /// </summary>
        public bool StartSocketListener()
        {
            if (m_clientSocket != null)
            {
                // create thread to data listen from phone
                m_clientListenerThreadPhone = new Thread(new ThreadStart(SocketListenerThreadPhone));
                m_clientListenerThreadPhone.Start();

                return true;
            }
            else
                return false;
        }
        

        /// <summary>
        /// Thread method that does the communication to the client phone. This 
        /// thread tries to receive from phone and forward to proxy
        /// </summary>
        private void SocketListenerThreadPhone()
        {
            int size = 0;
            int iT1 = Environment.TickCount;

            m_lastReceiveDateTime = DateTime.Now;
            m_currentReceiveDateTime = DateTime.Now;
            _t = new System.Threading.Timer(new TimerCallback(CheckTimeOut), null, TIMEOUT_CONNECTION, TIMEOUT_CONNECTION);

            while (!m_stopClient)
            {
                try
                {
                    size = m_clientSocket.Receive(_byBufferRx, (int)0, (int)SIZE_BUFFER/2, SocketFlags.None);
                    if (size > 0)
                    {   m_lastReceiveDateTime = DateTime.Now;
                       // Console.WriteLine("Rx {0}", size);
                        Parser1(_byBufferRx, size);
                    }
                    else
                    {
                        

                        m_stopClient = true;
                        m_markedForDeletion = true;
                    }
                    
                }
                catch (Exception Ex)
                {
                    RemoveUsrReference();
                    m_stopClient = true;
                    m_markedForDeletion = true;
                }                
            }
            _t.Dispose();
        }

        void SendPacket(int iCmd, byte[] byBuff, int iLenBuff, Socket sck)
        {
            if (iLenBuff>0)
                Array.Copy(byBuff, 0, _byBufferTx, 3, iLenBuff);
            iLenBuff++;
            _byBufferTx[0] = (byte)(iLenBuff & 0x00FF);
            _byBufferTx[1] = (byte)((iLenBuff >> 8) & 0x00FF);
            _byBufferTx[2] = (byte)iCmd;
            sck.Send(_byBufferTx, iLenBuff + 2, SocketFlags.None);
        }


        void Parser2(Byte[] data, int iLen)
        {
            try
            {
                System.Text.Encoding enc;
                enc = System.Text.Encoding.ASCII;
                string sTmp;
                User usrReceiver, usrCaller;
                switch (data[0])
                {
                    case CMD_ECHO:
                        SendPacket(CMD_ECHO, data, iLen-1, m_clientSocket);
                        break;
                    case CMD_LOGIN:
                        data[iLen] = 0;
                        sTmp = enc.GetString(data, 1, iLen - 1);
                        string[] sinfo = sTmp.Split(';');
                        Console.WriteLine("CMD_LOGIN " + sinfo[0]);
                        Program.MainForm.UpdateUserList(sinfo[0], m_clientSocket.RemoteEndPoint.ToString(), sinfo[1], sinfo[2], true, m_clientSocket);
                        Program.MainForm.AddLog("Login:" + sinfo[0] + ":"+ sinfo[1] + ":"+sinfo[2], "");
                        data[0] = ACK;
                        SendPacket(CMD_LOGIN, data, 1, m_clientSocket);
                        break;
                    case CMD_LOGOUT:
                        data[iLen] = 0;
                        sTmp = enc.GetString(data, 1, iLen - 1);
                        Console.WriteLine("CMD_LOGOUT " + sTmp);
                        Program.MainForm.UpdateUserList(sTmp, m_clientSocket.RemoteEndPoint.ToString(), null, null, false, m_clientSocket);
                        Program.MainForm.AddLog("Logout:" + sTmp, "");
                        data[0] = ACK;
                        SendPacket(CMD_LOGOUT, data, 1, m_clientSocket);
                        break;
                    case CMD_GET_USR_LST:
                        sTmp = Program.MainForm.GetUsrLst(m_clientSocket.RemoteEndPoint.ToString());
                        int iSize = sTmp.Length;
                        if (iSize > 0)
                        {
                            Byte[] byUsr = enc.GetBytes(sTmp);
                            Array.Copy(byUsr, 0, data, 0, iSize);
                        }
                        SendPacket(CMD_GET_USR_LST, data, iSize, m_clientSocket);
                        break;
                    case CMD_HEARTBEAT:
                        _bConvOn = false;
                        int iCnt = data[2];
                        iCnt <<= 8;
                        iCnt |= data[1];                        
                        data[0] = data[1];
                        data[1] = data[2];
                        if (Program.MainForm.HeartBeatUser(m_clientSocket.RemoteEndPoint.ToString()))
                        {
                            data[2] = ACK;
                            Console.WriteLine("CMD_HEARTBEAT ACK:" + iCnt.ToString());
                        }
                        else
                        {
                            Console.WriteLine("CMD_HEARTBEAT NACK" + iCnt.ToString());
                            data[2] = NACK;
                        }
                        SendPacket(CMD_HEARTBEAT, data, 3, m_clientSocket);
                        break;

                    case CMD_CLOSECONV_ACK:
                        _bConvOn = false;
                        data[iLen] = 0;
                        sTmp = enc.GetString(data, 2, iLen - 2);
                        usrReceiver = Program.MainForm.GetUser(sTmp);
                        usrCaller = Program.MainForm.GetUserFromIP(m_clientSocket.RemoteEndPoint.ToString());
                        usrCaller.SetIEPConvVideo(null);
                        usrCaller.SetIEPConvAudio(null);
                        if (usrReceiver != null)
                        {
                            string sUsrCaller = usrCaller.GetUser();
                            Program.MainForm.AddLog(sUsrCaller + " ACK CLOSE CONV -> " + sTmp, "");
                            data[0] = ACK;
                            Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 1, sUsrCaller.Length);                            
                            SendPacket(CMD_CLOSECONV_ACK, data, sUsrCaller.Length + 1, usrReceiver.GetSocket());
                        }
                        else
                        {
                            Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T ANSWER CMD_CLOSECONV_ACK -> " + sTmp, "");
                        }
                        break;

                    case CMD_CLOSE_CONV:
                        _bConvOn = false;
                        data[iLen] = 0;
                        sTmp = enc.GetString(data, 1, iLen - 1);
                        usrReceiver = Program.MainForm.GetUser(sTmp);
                        usrCaller = Program.MainForm.GetUserFromIP(m_clientSocket.RemoteEndPoint.ToString());
                        if (usrCaller != null)
                        {
                            usrCaller.SetIEPConvVideo(null);
                            usrCaller.SetIEPConvAudio(null);
                        }
                        if (usrReceiver != null && usrCaller!=null)
                        {
                            string sUsrCaller = usrCaller.GetUser();
                            Program.MainForm.AddLog(sUsrCaller + " CLOSE CONVERSATION -> " + sTmp, "");
                            Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 0, sUsrCaller.Length);                            
                            SendPacket(CMD_CLOSE_CONV, data, sUsrCaller.Length, usrReceiver.GetSocket());
                        }
                        else
                        {
                            Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T CLOSE -> " + sTmp, "");
                            // send nack
                            data[0] = ACK;
                            Array.Copy(enc.GetBytes(sTmp), 0, data, 1, sTmp.Length);                            
                            SendPacket(CMD_CLOSECONV_ACK, data, sTmp.Length+1, m_clientSocket);
                        }
                        break;

                    case CMD_CALL_YESNO:
                        data[iLen] = 0;
                        sTmp = enc.GetString(data, 2, iLen - 2);
                        usrReceiver = Program.MainForm.GetUser(sTmp);
                        usrCaller = Program.MainForm.GetUserFromIP(m_clientSocket.RemoteEndPoint.ToString());
                        if (usrReceiver != null)
                        {
                            string sUsrCaller = usrCaller.GetUser();
                            if (data[1] == ACK)
                            {
                                Program.MainForm.AddLog(sUsrCaller + " ACCEPT CONV -> " + sTmp, "");
                                _bConvOn = true;
                            }
                            else
                            {
                                usrCaller.SetIEPConvVideo(null);
                                usrCaller.SetIEPConvAudio(null);
                                Program.MainForm.AddLog(sUsrCaller + " REJECT CONV -> " + sTmp, "");
                            }
                            data[0]=data[1];
                            Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 1, sUsrCaller.Length);                            
                            SendPacket(CMD_CALL_YESNO, data, sUsrCaller.Length +1, usrReceiver.GetSocket());
                        }
                        else
                        {
                            Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T ANSWER CMD_CALL_YESNO -> " + sTmp, "");
                            // send reject cmd
                            data[0] = 0;                            
                            Array.Copy(enc.GetBytes(sTmp), 0, data, 1, sTmp.Length);
                            SendPacket(CMD_CALL_YESNO, data, sTmp.Length +1, m_clientSocket);
                        }
                        break;
                    case CMD_CALL_ACK:
                        data[iLen] = 0;
                        sTmp = enc.GetString(data, 2, iLen - 2);
                        usrReceiver = Program.MainForm.GetUser(sTmp);
                        usrCaller = Program.MainForm.GetUserFromIP(m_clientSocket.RemoteEndPoint.ToString());
                        if (usrReceiver != null)
                        {
                            string sUsrCaller = usrCaller.GetUser();
                            Program.MainForm.AddLog(sUsrCaller + " ACK CALLING -> " + sTmp, "");
                            data[0] = ACK;
                            Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 1, sUsrCaller.Length);
                            SendPacket(CMD_CALL_ACK, data, sUsrCaller.Length +1, usrReceiver.GetSocket());                            
                        }
                        else
                        {
                            Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T ANSWER CMD_CALL_ACK -> " + sTmp, "");
                        }
                        break;
                    case CMD_CALL:
                        data[iLen] = 0;
                        sTmp = enc.GetString(data, 1, iLen - 1);
                        usrReceiver = Program.MainForm.GetUser(sTmp);
                        usrCaller = Program.MainForm.GetUserFromIP(m_clientSocket.RemoteEndPoint.ToString());
                        if (usrReceiver != null)
                        {
                            string sUsrCaller = usrCaller.GetUser();
                            Program.MainForm.AddLog(sUsrCaller + " CALLING -> " + sTmp, "");
                            Array.Copy(enc.GetBytes(sUsrCaller), 0, data, 0, sUsrCaller.Length);                            
                            SendPacket(CMD_CALL, data, sUsrCaller.Length, usrReceiver.GetSocket());
                        }
                        else
                        {
                            Program.MainForm.AddLog(usrCaller.GetUser() + " CAN'T CALL -> " + sTmp, "");
                            // send nack
                            data[0] = NACK;
                           
                            Array.Copy(enc.GetBytes(sTmp), 0, data, 1, sTmp.Length);
                            SendPacket(CMD_CALL_ACK, data, sTmp.Length+1, m_clientSocket);
                        }
                        break;
                }
            }
            catch (Exception Ex)
            {
                Program.MainForm.AddLog(String.Format("Parser2:EXCEPTION:{0}", Ex.Message), "");
                throw (Ex);
            }
        }

        void Parser1(Byte[] byBuf, int iLen)
        {
            try{
                int iOffset=0;                
                if (_iCurrentIdx + iLen > SIZE_BUFFER)
                {
                    Console.WriteLine("ERROR Parser1:FULL BUFFER {0}:{1}", _iCurrentIdx, iLen);
                    _iCurrentIdx = 0;
                    Program.MainForm.AddLog("ERROR Parser1:FULL BUFFER", "");
                    return;
                }

                if (_bHalfSize)
                {
                    _bHalfSize = false;
                    Array.Copy(byBuf, 0, _byBuffer, 1, iLen);
                    _byBuffer[0] = _byHalfSize;
                    iLen++;
                    Array.Copy(_byBuffer, byBuf, iLen);
                }

                if (_iStatus == 0 && iLen >= 2)   // first packet cannot less 4
                {                    
                    _iSizePack = byBuf[1] & 0x1F;
                    _iSizePack <<= 8;
                    _iSizePack |= byBuf[0];
                    
                    if (_iSizePack < 0)
                    {   m_stopClient = true;
                        m_markedForDeletion = true;
                        Program.MainForm.AddLog("ERROR Parser1:Bad Size", "");
                        return;
                    }
                    else
                    {   _iStatus++;
                        iLen -= 2;
                        iOffset = 2;
                        _iCurrentIdx = 0;
                    }
                }
                else if (_iStatus == 0 && iLen < 2 && !_bHalfSize)
                {   /*m_stopClient = true;
                    m_markedForDeletion = true;*/
                    Program.MainForm.AddLog("Parser1 HALF Size", "");
                    _bHalfSize = true;
                    _byHalfSize = byBuf[0];
                }

                if (_iStatus!=0)
                {   
                    switch (_iStatus)
                    {
                        case 1:
                            int iSize2Copy = iLen;
                            if (iSize2Copy>0){
                                if (_iCurrentIdx + iLen > _iSizePack)
                                    iSize2Copy = _iSizePack - _iCurrentIdx;
                                Array.Copy(byBuf, iOffset, _byBuffer, _iCurrentIdx, iSize2Copy);
                                _iCurrentIdx += iSize2Copy;
                                if (_iCurrentIdx >= _iSizePack)
                                {
                                    
                                    Parser2(_byBuffer, _iSizePack);
                                    _iStatus     = 0;
                                    _iCurrentIdx = 0;
                                    _iSizePack   = 0;
                                    if (iSize2Copy != iLen)
                                    {
                                        iLen -= iSize2Copy;
                                        Array.Copy(byBuf, iSize2Copy + iOffset, _byBuffer, 0, iLen);
                                        Array.Copy(_byBuffer, 0, byBuf, 0, iLen);
                                        Parser1(byBuf, iLen);
                                    }
                                }
                            }
                            break;
                    }
                }
            }
            catch (Exception Ex)
            {
                Program.MainForm.AddLog(String.Format("Parser1:EXCEPTION:{0}", Ex.Message), "");
                throw (Ex);
            }

        }


        /// <summary>
        /// Method that stops Client SocketListening Thread.
        /// </summary>
        public void StopSocketListener()
        {
            if (m_clientSocket != null)
            {
                m_stopClient = true;
                m_clientSocket.Close();
                // Wait for one second for the the thread to stop.
                m_clientListenerThreadPhone.Join(1000);

                // If still alive; Get rid of the thread.
                if (m_clientListenerThreadPhone.IsAlive)
                    m_clientListenerThreadPhone.Abort();

                m_clientListenerThreadPhone = null;
                m_clientSocket = null;
                m_markedForDeletion = true;
            }
        }

        /// <summary>
        /// Method that returns the state of this object i.e. whether this
        /// object is marked for deletion or not.
        /// </summary>
        /// <returns></returns>
        public bool IsMarkedForDeletion()
        {            
            return m_markedForDeletion;
        }
        void RemoveUsrReference()
        {
            try{
                User usr = Program.MainForm.GetUserFromIP(m_clientSocket.RemoteEndPoint.ToString());
                if (usr != null)
                {
                    usr.SetIEPConvVideo(null);
                    usr.SetIEPConvAudio(null);
                }
            }
            catch (Exception Ex){
            }
        }
        /// <summary>
        /// Method that checks whether there are any client calls for the
        /// last x seconds or not. If not this client SocketListener will
        /// be closed.
        /// </summary>
        /// <param name="o"></param>
        private void CheckTimeOut(object o)
        {

            if (m_lastReceiveDateTime.Equals(m_currentReceiveDateTime) && _bConvOn==false)
            {
                RemoveUsrReference();
                Program.MainForm.AddLog("TIMEOUT. CLOSE THE CONNECTION", "");
                StopSocketListener();
                _t.Dispose();
            }
            else
            {
                m_lastReceiveDateTime = m_currentReceiveDateTime;
            }
        }    
    }

}

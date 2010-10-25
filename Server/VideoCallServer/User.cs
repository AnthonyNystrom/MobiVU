using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace VideoCallServer
{
    public class User
    {
        string _sUserName, _sIP;
        bool _bHearBeat;
        IPEndPoint _iepCmd, _iepVideo, _iepAudio, _iepConvVideo, _iepConvAudio;
        int _iPort;
        Socket _sck;

        public User(string sUsr, string sIP, Socket sck)
        {
            string[] stmp = sIP.Split(':');
            _bHearBeat  = true;
            _sUserName  = sUsr;
            _sIP        = stmp[0];
            _iPort      = Convert.ToInt32(stmp[1]);
            _iepCmd     = new IPEndPoint(IPAddress.Parse(_sIP), _iPort);
            _sck        = sck;
            _iepVideo   = null;
            _iepAudio   = null;
            _iepConvVideo = null;
            _iepConvAudio = null;
        }
        public void SetIepVideo(String sIP)
        {
            string[] stmp = sIP.Split(':');
            int iPort = Convert.ToInt32(stmp[1]);
            _iepVideo = new IPEndPoint(IPAddress.Parse(stmp[0]), iPort);
        }
        public void SetIepVideo(int iPort)
        {
            _iepVideo = new IPEndPoint(IPAddress.Parse(_sIP), iPort);
        }
        public void SetIepAudio(String sIP)
        {
            string[] stmp = sIP.Split(':');
            int iPort = Convert.ToInt32(stmp[1]);
            _iepAudio = new IPEndPoint(IPAddress.Parse(stmp[0]), iPort);
        }
        public void SetIepAudio(int iPort)
        {
            _iepAudio = new IPEndPoint(IPAddress.Parse(_sIP), iPort);
        }
        public void SetHearBeat(bool bHearBeat)
        {
            _bHearBeat = bHearBeat;
        }
        public string GetUser()
        {
            return _sUserName;
        }
        public string GetIP()
        {
            return _iepCmd.ToString();
        }
        public string GetOnlyIP()
        {
            return _sIP;
        }
        public bool GetHearBeat()
        {
            return _bHearBeat;
        }
        public IPEndPoint GetIEPCmd()
        {
            return _iepCmd;
        }
        public IPEndPoint GetIEPVideo()
        {
            return _iepVideo;
        }
        public IPEndPoint GetIEPAudio()
        {
            return _iepAudio;
        }
        public void SetIEPConvVideo(IPEndPoint iep)
        {
            _iepConvVideo = iep;
        }
        public void SetIEPConvAudio(IPEndPoint iep)
        {
            _iepConvAudio = iep;
        }
        public IPEndPoint GetIEPConvVideo()
        {
            return _iepConvVideo;
        }
        public IPEndPoint GetIEPConvAudio()
        {
            return _iepConvAudio;
        }
        public Socket GetSocket()
        {
            return _sck;
        }
    }
}

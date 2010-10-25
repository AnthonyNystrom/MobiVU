package com.mobivu;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.*;
import java.net.DatagramPacket; 
import java.net.DatagramSocket; 
import java.net.InetAddress; 
import java.net.SocketException;


import android.os.Looper;
import android.os.SystemClock;
import android.util.Log;

public class NetworkEngine implements Runnable {
	
	private String _sIP;
	private Socket _sc;
	private OutputStream _out;
	private InputStream _insc;
	private Thread _th=null;
	private boolean _bLoop;
	private int _iEngineStatus;
	private byte []_byBufRX;
	private byte []_byBufRX2;
	private byte []_byBuf2Send;
	private int _iLen2Send;
	private int _cmd;
	private DatagramSocket _socket;
	private DatagramPacket _datagram;
	private InetAddress _addr;
	private int _icnttest=0;
	FileOutputStream _inputread, _inputread1;
	int _iSizeRx2;
	
	private static final int ENGINE_NET_STATUS_DISCONNECTED =0;
	private static final int ENGINE_NET_STATUS_CONNECTED    =1;
	private static final int ENGINE_NET_STATUS_TX_CMD       =2;
	private static final int ENGINE_NET_STATUS_RX_CMD       =3;

	public static final int CMD_NONE       = -1;
	public static final int CMD_LOGIN      = 0;
	public static final int CMD_LOGOUT     = 1;
	public static final int CMD_CALL       = 2;
	public static final int CMD_FRM_VIDEO  = 3;
	public static final int CMD_FRM_AUDIO  = 4;
	public static final int CMD_USERS_LIST = 5;
	public static final int CMD_RING       = 6;
	public static final int CMD_CLOSE_CONV = 7;
	public static final int CMD_RECORDER   = 11;
	
	public static final int BAD_RESULT = 0;
	public static final int GOOD_RESULT = 1;
	private EventNetwork _eventNetwork;
	
	NetworkEngine(EventNetwork eventNetwork){
		_eventNetwork = eventNetwork;
	}
	
	void Init(String sIP){
		_sIP           = sIP;
		_byBuf2Send    = new byte[20000];
		_byBufRX       = new byte[20000];
		_byBufRX2      = new byte[20000];
		_iSizeRx2      = 0;
		_iLen2Send     = 0;
		_bLoop         = true;
		_cmd           = CMD_NONE;
		_iEngineStatus = ENGINE_NET_STATUS_DISCONNECTED;
		
		try {
			_socket = new DatagramSocket();
			_socket.setSoTimeout(2000);			
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		_datagram = new DatagramPacket(new byte[1],1);
		
		try {
			_addr =  InetAddress.getByName(sIP);
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		
		// prepare file to write amr
		File fo = new File("/sdcard/input.dat");
		
		if (fo.exists())
			fo.delete();
		File fo1 = new File("/sdcard/input1.dat");
		if (fo1.exists())
			fo1.delete();
		try {
			_inputread = new FileOutputStream(fo);
			_inputread1= new FileOutputStream(fo1);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		_th = new Thread(this);
		_th.start();
	}
	public void Release(){
		try {
			_inputread .close();
			_inputread1.close();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		_bLoop=false;
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		_th.stop();
		_byBufRX    = null;
		_byBuf2Send = null;
	}

	public void run() {
		Looper.prepare();
		int iRead, iSizePack;
		int iCmdRx;
		boolean bReadDataGood=false;
		
		
		while (_bLoop){
			switch (_iEngineStatus){
			case ENGINE_NET_STATUS_DISCONNECTED:
				if (!ConnectSocket())
					CloseSocket();
				break;
			case ENGINE_NET_STATUS_CONNECTED:
				break;
			case ENGINE_NET_STATUS_TX_CMD:
				try {
					WriteData(_byBuf2Send, 0, _iLen2Send);
					_iEngineStatus++;
				} catch (IOException e) {
					Log.i("NetworkEngine::ENGINE_NET_STATUS_TX_CMD", "error transmission");
					e.printStackTrace();
					CloseSocket();
					_iEngineStatus = ENGINE_NET_STATUS_DISCONNECTED;
				}
				break;
			case ENGINE_NET_STATUS_RX_CMD:
				iRead =0;
				try {
					/*iRead = ReadData(_byBufRX, 0, 3);
					
					iSizePack = _byBufRX[1] & 0x000000FF;
	                iSizePack <<= 8;
	                iSizePack |= (_byBufRX[0] & 0x000000FF);
	                //Engine.AddLog("iRead:"+iRead+"iSizePack:"+iSizePack);
	                if (iSizePack>0)
	                {	iRead = ReadData(_byBufRX, 3, iSizePack-1);
	                	
						//Engine.AddLog("iRead:"+iRead);
						//_eventNetwork.CmdResult(CMD_FRM_AUDIO, _byBufRX, iSizePack-1, GOOD_RESULT);
					}*/
					iRead = ReadData(_byBufRX, 0, _byBufRX.length);
					if (iRead>0)
						_eventNetwork.CmdResult(CMD_FRM_VIDEO, _byBufRX, iRead, GOOD_RESULT);
					
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				break;/*
				long l1 = SystemClock.uptimeMillis();

				iRead=0;
				try {
					// read header					
					
					iRead = ReadData(_byBufRX, 0, 3);
					if (iRead>0)
						_icnttest++;
					if (iRead==3){
		    			//iSizePack = _byBufRX[1] & 0x0000001F;
						iSizePack = _byBufRX[1] & 0x000000FF;
		                iSizePack <<= 8;
		                iSizePack |= (_byBufRX[0] & 0x000000FF);
		                iCmdRx =_byBufRX[2];		                
		                
	                    if (iSizePack >_byBufRX.length)
	                    	Engine.AddLog("Size Packet too big");
	                    else{
	                    	_byBufRX[0]=(byte) 0xFF;
	                    	iRead=ReadData(_byBufRX, 3, iSizePack-1);
	                    	Engine.AddLog("iSizePack:"+iSizePack+ " Counter:"+_icnttest+"Read:"+iRead);
	                    	bReadDataGood = true;
	                    	switch (_cmd){	                    	
	                    	case CMD_USERS_LIST:
	                    	case CMD_LOGOUT:
	                    	case CMD_LOGIN:
	                    		if ((_cmd==iCmdRx || iCmdRx==CMD_CLOSE_CONV) && iRead==iSizePack-1){
	                    			_eventNetwork.CmdResult(iCmdRx, _byBufRX, iSizePack, GOOD_RESULT);
	                    		}	                    		
	                    		else{
	                    			Engine.AddLog("doesn't match cmd");
	                    			_eventNetwork.CmdResult(_cmd, _byBufRX, iSizePack, BAD_RESULT);
	                    		}
	                    		_cmd = CMD_NONE;
	                    		break;
	                    	default:
	                    		if (_cmd!=CMD_NONE && iCmdRx!=CMD_RING && iCmdRx!=CMD_FRM_VIDEO && iCmdRx!=CMD_FRM_AUDIO && iCmdRx!=CMD_CLOSE_CONV  && iCmdRx!=CMD_CALL){
	                    			Engine.AddLog("BAD RESULT1");
	                    			_eventNetwork.CmdResult(_cmd, _byBufRX, 0, BAD_RESULT);
	                    			_cmd=CMD_NONE;
	                    		}
	                    		else if ((iCmdRx==CMD_RING || iCmdRx==CMD_FRM_VIDEO || iCmdRx==CMD_FRM_AUDIO || iCmdRx==CMD_CLOSE_CONV || iCmdRx==CMD_CALL) && iRead==iSizePack-1){
	                    			_eventNetwork.CmdResult(iCmdRx, _byBufRX, iSizePack-1, GOOD_RESULT);
	                    		}
	                    		else{
	                    			Engine.AddLog("BAD RESULT2");
	                    			_eventNetwork.CmdResult(_cmd, _byBufRX, 0, BAD_RESULT);
	                    			_cmd=CMD_NONE;
	                    		}
	                    	}
	                    }
					}
					else if (iRead>0)
					{	
						Engine.AddLog("DATA LESS OF 3:"+iRead);
					}
				} catch (IOException e) {
					e.printStackTrace();
					Engine.AddLog("exception to read socket");
					if (_cmd!=CMD_NONE){
						_eventNetwork.CmdResult(_cmd, _byBufRX, 0, BAD_RESULT);
            			_cmd=CMD_NONE;
            		}
				}
				if (!bReadDataGood && iRead>0){
					Engine.AddLog("!bReadDataGood && iRead>0");
					CloseSocket();
					_iEngineStatus = ENGINE_NET_STATUS_DISCONNECTED;
				}
				Engine.AddLog("Time ReadData:"+(SystemClock.uptimeMillis()-l1));

				break;*/
			}
		}
	}
	private void CloseSocket(){
		/*if (_out!=null){
			try{
		    	_out.close();
		    	_sc.close();
		    	_sc   = null;
		    	_out  = null;
		    	_insc = null;
			}
			catch(IOException ex){
			}
	    	_eventNetwork.Connected(false);
		
		}*/
		_socket.disconnect();
	}
	
	private boolean ConnectSocket(){
		/*boolean bret=false;
		try{
			_sc = new Socket(_sIP, 8082); 
			_sc.setSoTimeout(5000);
	    	_out  = _sc.getOutputStream();
	    	_insc = _sc.getInputStream();
	    	bret=true;
	    	_iEngineStatus = ENGINE_NET_STATUS_CONNECTED;
	    	_eventNetwork.Connected(true);
		}
		catch(IOException e){
			e.printStackTrace();
			_eventNetwork.Connected(false);
		}*/
		
		_socket.connect(_addr, 9050);
		boolean bret=_socket.isConnected();
		_eventNetwork.Connected(bret);
		_iEngineStatus = ENGINE_NET_STATUS_CONNECTED;
		return bret;
	}
	
	public void SendCommand(int iCmd, byte []byData, int iSizeData){		
		if (iCmd==CMD_FRM_AUDIO || iCmd==CMD_FRM_VIDEO){
		
			if (byData!=null){
				System.arraycopy(byData, 0, _byBuf2Send, 3, iSizeData);
				SetCheckSum(_byBuf2Send, iSizeData+1, (byte)iCmd);
				_iLen2Send = iSizeData+3;
			}		else{
				SetCheckSum(_byBuf2Send, 1, (byte)iCmd);
				_iLen2Send = 3;
			}
			try {
					WriteData(_byBuf2Send, 0, _iLen2Send);				
			} catch (IOException e) {
				Log.i("SendCommand", "Failed");
				e.printStackTrace();
			}
			
			_cmd = iCmd;
			//_iEngineStatus = ENGINE_NET_STATUS_TX_CMD;
			_iEngineStatus = ENGINE_NET_STATUS_RX_CMD;
		}
	}
	
	private void SetCheckSum(byte []buf, int iLen, byte cmd){
    	/*short iChkSum= (short) (((iLen & 0x1FFF) % 0x07) ^ 0x05);
    	buf[0] = (byte) (iLen & 0x000000FF);
    	buf[1] = (byte) ((iLen>>8) & 0x00000FF);
    	buf[1] = (byte) ((iChkSum<<5) | buf[1]);
    	buf[2] = cmd;*/
		buf[0] = (byte) (iLen & 0x000000FF);
		buf[1] = (byte) ((iLen>>8) & 0x00000FF);
		buf[2] = (byte)_icnttest;
		_icnttest++;
    }
	
	private int ReadData(byte []byBuff, int iIdx, int iSize) throws IOException{
		/*int iRet=0;
		if (_insc.available()>0)
			iRet=_insc.read(byBuff, iIdx, iSize);
		return iRet;*/
				
		/*_socket.receive(_datagram2Read);		
		int iRet=_datagram2Read.getLength();
		Engine.AddLog("datagram2Read.getLength():"+_datagram2Read.getLength());*/
		
		DatagramPacket dm = new DatagramPacket(byBuff, iSize);
		_socket.receive(dm);
		int iRet = dm.getLength();
		if (iRet>0)
			_inputread.write(byBuff,2,1);
		return iRet;
		
	}
	
	private void WriteData(byte []byBuff, int iIdx, int iSize) throws IOException{
		//_out.write(byBuff, iIdx, iSize);
		//_out.flush();
		
		_datagram.setData(byBuff, iIdx, iSize);
		_datagram.setLength(iSize);
		_datagram.setAddress(_addr);
		_datagram.setPort(9050);
		try {
			_socket.send(_datagram);
			
		} catch (IOException e) {
			Log.i("WriteData", "Failed");
			e.printStackTrace();
		}
	}

}

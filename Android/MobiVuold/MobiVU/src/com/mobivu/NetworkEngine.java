package com.mobivu;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.*;

import android.os.Looper;
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
	private byte []_byBuf2Send;
	private int _iLen2Send;
	private int _cmd;
	
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
		_byBufRX       = new byte[100000];
		_iLen2Send     = 0;
		_bLoop         = true;
		_cmd           = CMD_NONE;
		_iEngineStatus = ENGINE_NET_STATUS_DISCONNECTED;
		
		_th = new Thread(this);
		_th.start();
	}
	public void Release(){
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
			try {
				Thread.sleep(5);
			} catch (InterruptedException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			
			switch (_iEngineStatus){
			case ENGINE_NET_STATUS_DISCONNECTED:
				if (!ConnectSocket())
					CloseSocket();
				break;
			case ENGINE_NET_STATUS_CONNECTED:
				break;
			case ENGINE_NET_STATUS_TX_CMD:
				try {
					_out.write(_byBuf2Send, 0, _iLen2Send);
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
					// read header
					if (_insc.available()>0)
					/*while (_insc.available()>0)
					{*/	//Log.i("Buffer Socket:", ""+_insc.available());
						iRead=_insc.read(_byBufRX, 0, 3);
					
					if (iRead==3){
		    			iSizePack = _byBufRX[1] & 0x0000001F;
		                iSizePack <<= 8;
		                iSizePack |= (_byBufRX[0] & 0x000000FF);
	                    
	                    if (iSizePack >_byBufRX.length)
	                    	Log.i("NetworkEngine::ENGINE_NET_STATUS_RX_CMD", "data size packet too large");
	                    else{
	                    	iRead=_insc.read(_byBufRX, 3, iSizePack-1);
	                    	bReadDataGood = true;
	                    	iCmdRx =_byBufRX[2] & 0x0000000F; 
	                    	switch (_cmd){
	                    	/*case CMD_CLOSE_CONV:
	                    	case CMD_CALL:*/
	                    	case CMD_USERS_LIST:
	                    	case CMD_LOGOUT:
	                    	case CMD_LOGIN:
	                    		if ((_cmd==iCmdRx || iCmdRx==CMD_CLOSE_CONV) && iRead==iSizePack-1){
	                    			_eventNetwork.CmdResult(iCmdRx, _byBufRX, iSizePack, GOOD_RESULT);
	                    		}	                    		
	                    		else{
	                    			Log.i("NetworkEngine::ENGINE_NET_STATUS_RX_CMD", "doesn't match cmd");
	                    			_eventNetwork.CmdResult(_cmd, _byBufRX, iSizePack, BAD_RESULT);
	                    		}
	                    		_cmd = CMD_NONE;
	                    		break;
	                    	default:
	                    		if (_cmd!=CMD_NONE && iCmdRx!=CMD_RING && iCmdRx!=CMD_FRM_VIDEO && iCmdRx!=CMD_FRM_AUDIO && iCmdRx!=CMD_CLOSE_CONV  && iCmdRx!=CMD_CALL){
	                    			Log.i("BAD_RESULT", "BAD_RESULT1");
	                    			_eventNetwork.CmdResult(_cmd, _byBufRX, 0, BAD_RESULT);
	                    			_cmd=CMD_NONE;
	                    		}
	                    		else if ((iCmdRx==CMD_RING || iCmdRx==CMD_FRM_VIDEO || iCmdRx==CMD_FRM_AUDIO || iCmdRx==CMD_CLOSE_CONV || iCmdRx==CMD_CALL) && iRead==iSizePack-1){
	                    			_eventNetwork.CmdResult(iCmdRx, _byBufRX, iSizePack, GOOD_RESULT);
	                    		}
	                    		else{
	                    			Log.i("BAD_RESULT", "BAD_RESULT2:"+iRead+"-"+iSizePack+"-"+iCmdRx);
	                    			_eventNetwork.CmdResult(_cmd, _byBufRX, 0, BAD_RESULT);
	                    			_cmd=CMD_NONE;
	                    		}
	                    	}
	                    }
					}
					else if (iRead>0)
					{	/*Log.i("NetworkEngine::ENGINE_NET_STATUS_RX_CMD", "error data len read socket");
						if (_cmd!=CMD_NONE){
							_eventNetwork.CmdResult(_cmd, _byBufRX, 0, BAD_RESULT);
	            			_cmd=CMD_NONE;
	            		}*/
						Log.i("ERRRR", "DATA LESS OF 3.................................");
					}
				} catch (IOException e) {
					e.printStackTrace();
					Log.i("NetworkEngine::ENGINE_NET_STATUS_RX_CMD", "exception to read socket");
					if (_cmd!=CMD_NONE){
						_eventNetwork.CmdResult(_cmd, _byBufRX, 0, BAD_RESULT);
            			_cmd=CMD_NONE;
            		}
				}
				if (!bReadDataGood && iRead>0){
					CloseSocket();
					_iEngineStatus = ENGINE_NET_STATUS_DISCONNECTED;
				}
				/*else if (_iEngineStatus==ENGINE_NET_STATUS_RX_CMD)
					_iEngineStatus = ENGINE_NET_STATUS_CONNECTED;*/
				break;
			}
		}
	}
	private void CloseSocket(){
		if (_out!=null){
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
		
		}
	}
	
	private boolean ConnectSocket(){
		boolean bret=false;
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
		}
		return bret;
	}
	
	public void SendCommand(int iCmd, byte []byData, int iSizeData){
		if (byData!=null){
			System.arraycopy(byData, 0, _byBuf2Send, 3, iSizeData);
			SetCheckSum(_byBuf2Send, iSizeData+1, (byte)iCmd);
			_iLen2Send = iSizeData+3;
		}
		else{
			SetCheckSum(_byBuf2Send, 1, (byte)iCmd);
			_iLen2Send = 3;
		}
		try {
				_out.write(_byBuf2Send, 0, _iLen2Send);
				_out.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		_cmd = iCmd;
		//_iEngineStatus = ENGINE_NET_STATUS_TX_CMD;
		_iEngineStatus = ENGINE_NET_STATUS_RX_CMD;
		
	}
	
	private void SetCheckSum(byte []buf, int iLen, byte cmd){
    	short iChkSum= (short) (((iLen & 0x1FFF) % 0x07) ^ 0x05);
    	buf[0] = (byte) (iLen & 0x00FF);
    	buf[1] = (byte) ((iLen>>8) & 0x0FF);
    	buf[1] = (byte) ((iChkSum<<5) | buf[1]);
    	buf[2] = cmd;
    	
    }

}

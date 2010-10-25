package com.mobivu;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

import android.os.SystemClock;
import android.util.Log;

public class Engine implements Runnable {
	
	private String _sIP, _sNickName, _sUser2Call, _sUserConv, _sCaller;
	private boolean _bLoop, _bLogoutAndExitLoop;
	private Thread _th=null;
	private byte []_buffer;
	private int _iStatus;
	private long _lTmHearBeat, _lTmRinging, _lTmr;
	private MobiVu _win;
	private AV _netAudio;
	private AV _netVideo;
	private Codecs _codec;
	private Socket _sc;
	private OutputStream _out;
	private InputStream _insc;
	
	public static final int TIME_HEAR_BEAT = 3000;
	public static final int TIMEOUT        = 5000;
	public static final int TIME_RINGING   = 15000;
	
	public static final int STATUS_LOGOUT        = -1;
	public static final int STATUS_DISCONNECTED  = 0;
	public static final int STATUS_IDLE  		 = 1;
	public static final int STATUS_CALLING       = 2;
	public static final int STATUS_RINGING       = 3;
	public static final int STATUS_CONVERSATION  = 4;
	public static final int STATUS_USERLIST      = 5;
	public static final int STATUS_WAIT_YESNO    = 6;
	public static final int STATUS_TEST          = 7;
	public static final int STATUS_REJECT_INCOMING_CALL = 8;
	public static final int STATUS_REJECT_OUTGOING_CALL = 9;
	public static final int STATUS_CLOSE_CONVERSATION   = 10;
	public static final int STATUS_CLOSING_CONVERSATION = 11;
	public static final int STATUS_ACCEPT_INCOMING_CALL = 12;
	public static final int STATUS_BINDING       = 13;
	
	
	public static final int CMD_NONE       = -1;
	public static final int CMD_LOGIN      = 0;
	public static final int CMD_LOGOUT     = 1;
	public static final int CMD_CALL       = 2;
	public static final int CMD_CALL_ACK   = 3;
	public static final int CMD_CALL_YESNO = 4;
	public static final int CMD_FRM_VIDEO  = 5;
	public static final int CMD_FRM_AUDIO  = 6;
	public static final int CMD_GET_USR_LST= 7;
	public static final int CMD_CLOSE_CONV = 8;
	public static final int CMD_CLOSECONV_ACK=9;
	public static final int CMD_HEARTBEAT  = 10;
	public static final int CMD_ECHO       = 11;
	public static final int CMD_BIND_USER  = 12;
	
	public static final int ACK  = 6;
	public static final int NACK = 21;
	
	public static final String TIMEOUT_MSG = "TIMEOUT CONNECTION PROBLEM";
	
	
	public void Init(String sIP, String sNikName, MobiVu win){
		_win 		= win;
		_sNickName  = sNikName;
		_sIP        = sIP;
		
		_codec = new Codecs();
		_codec.Init();
		
		_netVideo = new AV();
        _netAudio = new AV();
        _netVideo.Init(_sIP, _sNickName, 9051, _codec, _win, true);
        _netAudio.Init(_sIP, _sNickName, 9052, _codec, _win, false);        
		
		_buffer   = new byte[2048];		
		_iStatus 	= STATUS_DISCONNECTED;		
		
		_bLoop = true;
		_bLogoutAndExitLoop = false;
		_th = new Thread(this);		
		_th.start();
	}
	public void Release(){
		if (_codec!=null)
			_codec.Release();
		if (_netVideo!=null)
			_netVideo.Release();
		if (_netAudio!=null)
			_netAudio.Release();
		if (_th!=null){
			_bLogoutAndExitLoop = true;
			_iStatus = STATUS_LOGOUT;
			try {Thread.sleep(500);} catch (InterruptedException e) {}
			_bLoop=false;
			try {Thread.sleep(200);} catch (InterruptedException e) {}
			_th.stop();
		}
		CloseSocket();
	}

	@Override
	public void run() {
		int iRead;
		_lTmr = 0;
		int iCnt=0, iTent = 0;
		
		while(_bLoop){
			try {
				
				switch (_iStatus){
				case STATUS_TEST:
					if (_lTmr==0){
						_buffer[3] = (byte) (iCnt & 0x000000FF);
						_buffer[4] = (byte) ((iCnt>>8) & 0x000000FF);
						PrepareBuffer(_buffer, 2, CMD_ECHO);
						WriteSocket(_buffer, 0, 5);
						_lTmr = SystemClock.uptimeMillis();
						//Log.i("ECHO", ""+iCnt);
						iCnt++;
					}
					else if ((SystemClock.uptimeMillis()-_lTmr)>TIMEOUT){
						// timeout.
						_lTmr = 0;
						_iStatus = STATUS_LOGOUT;
						Log.i("timeout echo", "-");
					}					
					break;
				case STATUS_LOGOUT:					
					_win.SetStatus(_iStatus, "");
					
					if (_lTmr==0){						
						System.arraycopy(_sNickName.getBytes(), 0, _buffer, 3, _sNickName.length());
						PrepareBuffer(_buffer, _sNickName.length(), CMD_LOGOUT);
						WriteSocket(_buffer, 0, _sNickName.length()+3);
						_lTmr = SystemClock.uptimeMillis();
					}
					else if ((SystemClock.uptimeMillis()-_lTmr)>TIMEOUT){
						if (_bLogoutAndExitLoop)
							_bLoop=false;
						{				
							_lTmr = 0;
							CloseSocket();
							_iStatus = STATUS_DISCONNECTED;
						}
					}
					break;
				case STATUS_DISCONNECTED:					
					_win.SetStatus(_iStatus, "");
					if (ConnectSocket()){
						// send cmd login
						if (_lTmr==0){
							Log.i("Login cmd", "-");
							String sMsgPck =  _sNickName+";"+_netVideo.GetLocalPort()+";"+_netAudio.GetLocalPort();
							System.arraycopy(sMsgPck.getBytes(), 0, _buffer, 3, sMsgPck.length());
							PrepareBuffer(_buffer, sMsgPck.length(), CMD_LOGIN);
							WriteSocket(_buffer, 0, sMsgPck.length()+3);
							_lTmr = SystemClock.uptimeMillis();
						}
						else if ((SystemClock.uptimeMillis()-_lTmr)>TIMEOUT){
							// timeout. Retry
							_lTmr = 0;							
						}
					}					
					break;
				case STATUS_IDLE:				
					_win.SetStatus(_iStatus, "");
					if (_lTmr==0){
						if ((SystemClock.uptimeMillis()-_lTmHearBeat)>TIME_HEAR_BEAT){
							
							_buffer[3] = (byte) (iCnt & 0x000000FF);
							_buffer[4] = (byte) ((iCnt>>8) & 0x000000FF);
							PrepareBuffer(_buffer, 2, CMD_HEARTBEAT);
							WriteSocket(_buffer, 0, 5);
							_lTmr = _lTmHearBeat = SystemClock.uptimeMillis();
							iCnt++;
							Log.i("Heartbeat", ""+iCnt);
						}
					}
					else if ((SystemClock.uptimeMillis()-_lTmr)>TIMEOUT){
						// timeout.
						_lTmr = 0;
						_iStatus = STATUS_LOGOUT;
						Log.i("timeout heartbeat", "-");
					}
					break;					
				case STATUS_USERLIST:
					if (_lTmr==0){
						PrepareBuffer(_buffer, 0, CMD_GET_USR_LST);
						WriteSocket(_buffer, 0, 3);
						_lTmr = SystemClock.uptimeMillis();
					}
					else if ((SystemClock.uptimeMillis()-_lTmr)>TIMEOUT){
						_lTmr = 0;
						_iStatus = STATUS_LOGOUT;
						Log.i("timeout get user list", "-");
						_win.ShowToastMessage(TIMEOUT_MSG);
					}
					break;
				
				case STATUS_CALLING:
					_win.SetStatus(_iStatus, "");
					if (_lTmr==0){
						System.arraycopy(_sUser2Call.getBytes(), 0, _buffer, 3, _sUser2Call.length());
						PrepareBuffer(_buffer, _sUser2Call.length(), CMD_CALL);
						WriteSocket(_buffer, 0, _sUser2Call.length()+3);
						_lTmr = SystemClock.uptimeMillis();
					}
					else if ((SystemClock.uptimeMillis()-_lTmr)>TIMEOUT){
						_lTmr = 0;
						_win.ShowOutgoingCall(false);
						_win.ShowToastMessage(TIMEOUT_MSG);
						_iStatus = STATUS_LOGOUT;
						Log.i("timeout calling", "-");
					}
					break;
				case STATUS_WAIT_YESNO:
					_win.SetStatus(_iStatus, "");
					if ((SystemClock.uptimeMillis()-_lTmr)>TIME_RINGING){
						_lTmr = 0;
						_win.ShowOutgoingCall(false);
						_win.ShowToastMessage("USER DON'T ANSWER");
						_iStatus = STATUS_LOGOUT;
						Log.i("timeout wait yesno", "-");
					}
					break;
				case STATUS_RINGING:
					_win.SetStatus(_iStatus, _sCaller);
					if ((SystemClock.uptimeMillis()-_lTmRinging)>TIME_RINGING){
						_win.ShowIncomingCall(false);
						_lTmr = 0;
						_iStatus = STATUS_IDLE;
						_buffer[3]=NACK;
						System.arraycopy(_sCaller.getBytes(), 0, _buffer, 4, _sCaller.length());
						PrepareBuffer(_buffer, _sCaller.length()+1, CMD_CALL_YESNO);
						WriteSocket(_buffer, 0, _sCaller.length()+4);
					}
					break;
				case STATUS_ACCEPT_INCOMING_CALL:					
					_win.SetStatus(_iStatus, "");
					_lTmr = 0;
					_iStatus = STATUS_BINDING;					
					_buffer[3]=ACK;
					System.arraycopy(_sCaller.getBytes(), 0, _buffer, 4, _sCaller.length());
					PrepareBuffer(_buffer, _sCaller.length()+1, CMD_CALL_YESNO);
					WriteSocket(_buffer, 0, _sCaller.length()+4);
					break;
				case STATUS_REJECT_INCOMING_CALL:
					_win.SetStatus(_iStatus, "");
					_lTmr = 0;
					_iStatus = STATUS_IDLE;
					_buffer[3]=NACK;
					System.arraycopy(_sCaller.getBytes(), 0, _buffer, 4, _sCaller.length());
					PrepareBuffer(_buffer, _sCaller.length()+1, CMD_CALL_YESNO);
					WriteSocket(_buffer, 0, _sCaller.length()+4);
					break;
				case STATUS_REJECT_OUTGOING_CALL:
					_win.SetStatus(_iStatus, "");
					_lTmr = 0;
					_iStatus = STATUS_IDLE;
					_buffer[3]=NACK;
					System.arraycopy(_sUser2Call.getBytes(), 0, _buffer, 4, _sUser2Call.length());
					PrepareBuffer(_buffer, _sUser2Call.length()+1, CMD_CALL_YESNO);
					WriteSocket(_buffer, 0, _sUser2Call.length()+4);
					break;
				case STATUS_BINDING:
					if (_lTmr==0){
						Bind();
						_lTmr = SystemClock.uptimeMillis();
					}
					else if (_netAudio.IsBinded() && _netVideo.IsBinded()){
						_netAudio.StartRec();
						_win.SetStatus(_iStatus, _sUserConv);
						_iStatus = STATUS_CONVERSATION;
					}
					else if ((SystemClock.uptimeMillis()-_lTmr)>TIMEOUT){
						_win.ShowToastMessage("Impossibile estabilish the conversation");
						_iStatus = STATUS_CLOSE_CONVERSATION;
					}
					break;
				case STATUS_CONVERSATION:
					_win.SetStatus(_iStatus, "");
					break;
				case STATUS_CLOSE_CONVERSATION:
					_win.SetStatus(_iStatus, "");
					_netAudio.StopRec();
					_iStatus = STATUS_CLOSING_CONVERSATION;
					System.arraycopy(_sUserConv.getBytes(), 0, _buffer, 3, _sUserConv.length());
					PrepareBuffer(_buffer, _sUserConv.length(), CMD_CLOSE_CONV);
					WriteSocket(_buffer, 0, _sUserConv.length()+3);
					_lTmr = SystemClock.uptimeMillis();
					iTent = 1;
					break;
				case STATUS_CLOSING_CONVERSATION:
					_win.SetStatus(_iStatus, "");
					if ((SystemClock.uptimeMillis()-_lTmr)>TIMEOUT && iTent>=2){
						_lTmr = 0;
						_iStatus = STATUS_IDLE;
					}
					else{
						// try again to send close conv cmd, last time						
						System.arraycopy(_sUserConv.getBytes(), 0, _buffer, 3, _sUserConv.length());
						PrepareBuffer(_buffer, _sUserConv.length(), CMD_CLOSE_CONV);
						WriteSocket(_buffer, 0, _sUserConv.length()+3);
						_lTmr = SystemClock.uptimeMillis();
						iTent++;
					}
					break;				
				}
				
				iRead = ReadSocket(_buffer, _buffer.length);
				
				if (iRead>0){
					Log.i("Answer", iRead+"-"+_buffer[2]+"-"+_iStatus);
					_lTmr = 0;
					switch (_buffer[2])
					{
					case CMD_CLOSE_CONV:
						// send ack
						_netAudio.StopRec();
						_win.SetStatus(STATUS_CLOSE_CONVERSATION, "");
						_buffer[3] = (byte)ACK;
						System.arraycopy(_sUserConv.getBytes(), 0, _buffer, 4, _sUserConv.length());
						PrepareBuffer(_buffer, _sUserConv.length()+1, CMD_CLOSECONV_ACK);
						WriteSocket(_buffer, 0, _sUserConv.length()+4);
						_lTmr = 0;
						_iStatus = STATUS_IDLE;
						break;
					case CMD_CLOSECONV_ACK:
						_lTmr = 0;
						_iStatus = STATUS_IDLE;
						break;
					case CMD_ECHO:
						int iTmp = _buffer[4] & 0x000000FF;
						iTmp <<= 8;
						iTmp |= (_buffer[3] & 0x000000FF);
						Log.i("ECHO READ", ""+iTmp);
						break;
					case CMD_GET_USR_LST:
						String sUsers = new String(_buffer,3,iRead-3);						
						_win.SetStatus(_iStatus, sUsers);
						_iStatus = STATUS_IDLE;
						break;
					case CMD_LOGOUT:
						if (_bLogoutAndExitLoop)
							_bLoop=false;
						else{
							CloseSocket();
							_iStatus = STATUS_DISCONNECTED;
						}
						break;
					case CMD_LOGIN:
						//_iStatus = STATUS_TEST;
						if (_buffer[3]==ACK){
							_iStatus = STATUS_IDLE;						
						}
						else{
							CloseSocket();
							_iStatus = STATUS_DISCONNECTED;
						}
						break;
					case CMD_HEARTBEAT:
						if (_buffer[5]!=ACK){
							Log.i("NAK HEARTBEAT", "-");
							CloseSocket();
							_iStatus = STATUS_DISCONNECTED;							
						}		
						else{
							/*iTmp = _buffer[2] & 0x000000FF;
							iTmp <<= 8;
							iTmp |= (_buffer[1] & 0x000000FF);
							Log.i("Answer HB", ""+iTmp);*/
						}
						break;
					case CMD_CALL_ACK:
						if (_buffer[3]==ACK){
							// ok Wait yes or no
							_lTmr = SystemClock.uptimeMillis();
							_iStatus = STATUS_WAIT_YESNO;
						}
						else{
							Log.i("NAK on answer calling", "-");
							// Answer NAK. Hide calling window
							_iStatus = STATUS_IDLE;
							_win.ShowOutgoingCall(false);
							_win.ShowToastMessage("User not available");
						}
						break;
					case CMD_CALL:
						_iStatus  = STATUS_RINGING; 
						_sCaller = new String(_buffer,3,iRead-3);
						_sUserConv = _sCaller;
						Log.i(_sCaller + " Calling", "-");
						// send ack						
						_buffer[3] = (byte)ACK;
						System.arraycopy(_sCaller.getBytes(), 0, _buffer, 4, _sCaller.length());
						PrepareBuffer(_buffer, _sCaller.length()+1, CMD_CALL_ACK);
						WriteSocket(_buffer, 0, _sCaller.length()+4);																
						_lTmRinging = SystemClock.uptimeMillis();
						break;
					
					case CMD_CALL_YESNO:
						String sUsr = new String(_buffer, 4, iRead-4);
						if (_iStatus != STATUS_WAIT_YESNO && _iStatus != STATUS_RINGING){	// too late or rejected from caller
							_buffer[3] = (byte)NACK;
							Log.i("Answer YES NO too late", "Status:"+_iStatus);
						}
						else{
							if (_buffer[3]==ACK)	// Accept
							{	
								if (_iStatus==STATUS_WAIT_YESNO){
									_lTmr=0;
									_iStatus = STATUS_BINDING;
								}
								_win.SetStatus(STATUS_ACCEPT_INCOMING_CALL, "");// only for hide outgoing window
								Log.i("Answer YES NO ACK", "-");
							}
							else					// Rejected
							{	_buffer[3] = (byte)NACK;
								Log.i("Answer YES NO NACK", "-");
								_iStatus = STATUS_IDLE;
								_win.ShowOutgoingCall(false);
								_win.ShowIncomingCall(false);
								_win.ShowToastMessage("USER HAS REJECTED THE CALL");
							}							
							WriteSocket(_buffer, 0, sUsr.length()+4);
							
						}
						break;
					}
				}				
			} catch (IOException e) {
				//CloseSocket();
				//Log.i("Timeout", ""+_iStatus);
				//e.printStackTrace();
			}
		}

	}	
	void CloseSocket(){
		if (_out!=null){
			try{
		    	_out.close();
		    	_sc.close();
		    	_sc   = null;
		    	_out  = null;
		    	_insc = null;
		    	_iStatus = STATUS_DISCONNECTED;
			}
			catch(IOException ex){
				
			}
		}
	}
	boolean ConnectSocket(){
		boolean bret=false;
		try{
			if (_sc!=null && _sc.isConnected())
				return true;
			_sc = new Socket();			
            SocketAddress adr = new InetSocketAddress(_sIP,8082); 
            _sc.connect(adr, 3000);
            _sc.setSoTimeout(2000);
			_out = _sc.getOutputStream();
			_insc  = _sc.getInputStream();			
			bret=true;
		}
		catch(IOException ex){
			Log.i("ConnectSocket", "Fail to connect:"+_sIP);
		}
		return bret;
	}
	private void PrepareBuffer(byte []byBuff, int iSize, int iCmd){
		iSize++;
		byBuff[0] = (byte) (iSize & 0x000000FF);
		byBuff[1] = (byte) ((iSize>>8) & 0x000000FF);
		byBuff[2] = (byte) iCmd;
	}
	private int ReadSocket(byte []byBuff, int iSize) throws IOException{
		if (_insc!=null)
			return _insc.read(byBuff, 0, iSize);
		else
			return 0;
	}	
	private void WriteSocket(byte []byBuff, int iIdx, int iSize) {
		if (_out!=null)
			try {
				_out.write(byBuff, iIdx, iSize);
			} catch (IOException e) {
				CloseSocket();				
			}
	}
	public void GetUsersOnline(){
		_iStatus = STATUS_USERLIST;
	}
	public void Call(String sUser2Call){
		_sUserConv = _sUser2Call = sUser2Call;
		_iStatus = STATUS_CALLING;
	}
	public void RejectOutgoingCall(){		
		_iStatus = STATUS_REJECT_OUTGOING_CALL;		
	}
	public void RejectIncomingCall(){
		_iStatus = STATUS_REJECT_INCOMING_CALL;
	}
	public void AcceptConveration(){
		_iStatus = STATUS_ACCEPT_INCOMING_CALL;
	}
	public void CloseConversation(){
		_iStatus = STATUS_CLOSE_CONVERSATION;
	}
	public void SendFrameVideo(byte []byFrame){
		byte []byH263 =_codec.EncodeVideo(byFrame, 0);		
		int iSize = _codec.GetSizeBufferEncodedVideo();
		if (iSize>0)
			_netVideo.SendCmd(Engine.CMD_FRM_VIDEO, byH263, iSize);
	}
	public void Bind(){
		// bind socket audio video
		byte []byBuff = new byte[_sUserConv.length()+1];
		System.arraycopy(_sUserConv.getBytes(), 0, byBuff, 1, _sUserConv.length());
		_netAudio.SendCmd(Engine.CMD_BIND_USER, byBuff, _sUserConv.length());
		_netVideo.SendCmd(Engine.CMD_BIND_USER, byBuff, _sUserConv.length());
		_netAudio.SendCmd(Engine.CMD_BIND_USER, byBuff, _sUserConv.length());
		_netVideo.SendCmd(Engine.CMD_BIND_USER, byBuff, _sUserConv.length());
	}

	// iMode 0 = Low Q, 1=Mid, 2=High
	public boolean ReinitCodecQ(int iMode){
		boolean bRet=true;
		_codec.ReleaseEncoderVideo();
		switch (iMode)
		{
		case 0:
			if (_codec.InitEncoderVideo(Codecs.FRAME_WITDH, Codecs.FRAME_HEIGHT, Codecs.BITRATE_LOW ) == 0)
				bRet=false;				
			else
				Log.i("Changed Quality", "LOW");
			break;
		case 1:
			if (_codec.InitEncoderVideo(Codecs.FRAME_WITDH, Codecs.FRAME_HEIGHT, Codecs.BITRATE_MID) == 0)
				bRet=false;
			else
				Log.i("Changed Quality", "MID");
			break;
		case 2:
			if (_codec.InitEncoderVideo(Codecs.FRAME_WITDH, Codecs.FRAME_HEIGHT, Codecs.BITRATE_HIGH) == 0)
				bRet=false;
			else
				Log.i("Changed Quality", "HIGH");
			break;
		}
		return bRet;
	}
	
}

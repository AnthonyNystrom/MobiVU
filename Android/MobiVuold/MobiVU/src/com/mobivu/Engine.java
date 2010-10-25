package com.mobivu;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Array;

import android.graphics.Color;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Looper;
import android.os.SystemClock;
import android.util.Log;

public class Engine implements Runnable, EventNetwork {
	private Codecs _Codecs;
	private Thread _th;
	private String _sNickName, _sUser2Call, _sIPServer;
	private AudioCapture _audioCapture;
	private EventProtocol _eventProtocol;
	private NetworkEngine _netEngine;
	private AudioTrack _audioTrack; 
	private boolean _bLoop, _bConnected, _bAudioCaptureEnabled;
	private int _iEngineStatus, _iCntRing, _iSizeH263TX, _iSizeAmrTX, _icntFrameAmr;
	private long _l1=0, _l2, _l3;
	private byte _byH263TX[], _byH263RX[], _byAmrTX[], _byAmrRX[];	
	
	private static final int ENGINE_STATUS_IDLE           = 0;
	private static final int ENGINE_STATUS_HEART_BEAT     = 1;
	private static final int ENGINE_STATUS_GET_USERS_LIST = 3;
	private static final int ENGINE_STATUS_OUTGOING       = 4 ;
	private static final int ENGINE_STATUS_CONVERSATION   = 5;
	private static final int ENGINE_STATUS_CLOSE_CONVERSATION = 6;
	private static final int ENGINE_STATUS_INCOMING       = 7 ;
	
	int _icntFrameE=0;
	int _iTimeStamp=0;
	
	Engine(EventProtocol eventProtocol){
		_eventProtocol = eventProtocol;
	}
	
	public void Init(String sNickName, String sIPServer){
		_sIPServer 		= sIPServer;
		_sNickName 		= sNickName;
		_bLoop     		= true;
		_bConnected		= false;
		_iEngineStatus 	= ENGINE_STATUS_IDLE;

		_netEngine = new NetworkEngine(this);
		_netEngine.Init(_sIPServer);
		
        _Codecs = new Codecs();
        _Codecs.Init();
        _audioCapture = new AudioCapture(this);
        
        _audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 8000, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT, 10240, AudioTrack.MODE_STREAM);
        _audioTrack.setStereoVolume(_audioTrack.getMaxVolume()/2, _audioTrack.getMaxVolume()/2);
		
		_th = new Thread(this);
		_th.setPriority(Thread.MAX_PRIORITY);
		_th.start();
		
		_byH263TX = new byte[20000];
		_byH263RX = new byte[20000];
		_byAmrTX  = new byte[1024];
		_byAmrRX  = new byte[1024];
		_iSizeH263TX = 0;
		_iSizeAmrTX  = 0;
	}
	public void Release(){
		_bLoop=false;
		_audioCapture.Stop();
		
    	_Codecs.Release();
		
		if (_bConnected){
			byte []byData = _sNickName.getBytes();
			_netEngine.SendCommand(NetworkEngine.CMD_LOGOUT, byData, byData.length);
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		_netEngine.Release();
		
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		_th.stop();
		_byH263TX = null;
		_byH263RX = null;
		_byAmrRX  = null;
		_byAmrTX  = null;
	}
	
	public void run() {
		Looper.prepare();
		
		
		while (_bLoop){
			try {
				Thread.sleep(5);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			//_netEngine.run();			
			if (_bAudioCaptureEnabled){
				_audioCapture.run();
			}
			switch (_iEngineStatus)
			{
			case ENGINE_STATUS_IDLE:
				break;
			case ENGINE_STATUS_HEART_BEAT:
				if (_l1==0 || (SystemClock.uptimeMillis()-_l1)>10000)
				{	byte []byData = _sNickName.getBytes();
					_netEngine.SendCommand(NetworkEngine.CMD_LOGIN, byData, byData.length);
					_l1 = SystemClock.uptimeMillis();
				}
				break;
			case ENGINE_STATUS_GET_USERS_LIST:
				_netEngine.SendCommand(NetworkEngine.CMD_USERS_LIST, null, 0);
				_iEngineStatus=ENGINE_STATUS_IDLE;
				break;
			case ENGINE_STATUS_INCOMING:
				break;
			case ENGINE_STATUS_OUTGOING:
				if (_l1==0 || (SystemClock.uptimeMillis()-_l1)>3000)
				{	if (_iCntRing>=10){
						_netEngine.SendCommand(NetworkEngine.CMD_CLOSE_CONV, null, 0);
					}
					else{
						byte []byData = _sUser2Call.getBytes();
						_netEngine.SendCommand(NetworkEngine.CMD_CALL, byData, byData.length);
						_l1 = SystemClock.uptimeMillis();
						_iCntRing++;
					}
				}
				break;
			case ENGINE_STATUS_CONVERSATION:
				if (_iSizeH263TX>0 && _iSizeH263TX<5000){
					_netEngine.SendCommand(NetworkEngine.CMD_FRM_VIDEO, _byH263TX, _iSizeH263TX);
					Log.i("VIDEO OUT", "SENT FRAME VIDEO");
					_iSizeH263TX=0;					
				}
				if (_iSizeAmrTX>0){
					_netEngine.SendCommand(NetworkEngine.CMD_FRM_AUDIO, _byAmrTX, _iSizeAmrTX);
					_iSizeAmrTX=0;
				}
				break;
			case ENGINE_STATUS_CLOSE_CONVERSATION:
				_netEngine.SendCommand(NetworkEngine.CMD_CLOSE_CONV, null, 0);
				_l1 = SystemClock.uptimeMillis()-8000;
				_iEngineStatus=ENGINE_STATUS_HEART_BEAT; 
				break;
			}
/*			try {
				Thread.sleep(20);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}*/
		}

	}
	// fired when socket is connected or disconnected
	public void Connected(boolean bConnected) {
		_bConnected = bConnected;
		if (_bConnected){
			_iEngineStatus = ENGINE_STATUS_HEART_BEAT;
		}
		else
			_iEngineStatus = ENGINE_STATUS_IDLE;
		
	}

	public void CmdResult(int iCmd, byte[] byData, int iLenData, int iResult) {
		switch (iCmd){
			case NetworkEngine.CMD_FRM_AUDIO:
				if (iResult!=NetworkEngine.BAD_RESULT && (iLenData-2)>0){
					//_l3 = SystemClock.uptimeMillis();
					System.arraycopy(byData, 3, _byAmrRX, 0, iLenData-2);
					byte []byAmr = _Codecs.DecodeAudio(_byAmrRX, iLenData-2);
					_audioTrack.write(byAmr, 0, _Codecs.GetSizeBufferDecodedAudio());					
					_icntFrameAmr++;
					if (_icntFrameAmr>=25){
						_audioTrack.flush();
						_audioTrack.play();
						_icntFrameAmr=0;
					}					
				}
				break;
			case NetworkEngine.CMD_FRM_VIDEO:
				if (iResult!=NetworkEngine.BAD_RESULT){
					Log.i("VIDEO SIZE", ""+(iLenData-2));
					if (iLenData-2>0){
						System.arraycopy(byData, 3, _byH263RX, 0, iLenData-2);
						//_l3 = SystemClock.uptimeMillis();
						int []iRGB = _Codecs.DecodeVideo(_byH263RX, iLenData-2);
						_eventProtocol.ShowFrame(iRGB);
					}
					else
						Log.i("Error size frame video", "Error size frame video...........");
					//Log.i("Time Decoder", ""+(SystemClock.uptimeMillis()-_l3));
				}
				break;
			case NetworkEngine.CMD_RING:
				if (iResult!=NetworkEngine.BAD_RESULT){
					_iEngineStatus = ENGINE_STATUS_INCOMING;
					String value = new String(byData,3,iLenData-1);
					_eventProtocol.IncomingCall(value);
				}
				break;
			case NetworkEngine.CMD_CLOSE_CONV:
				if (iResult!=NetworkEngine.BAD_RESULT){
					CloseConversation();
				}
				break;
			case NetworkEngine.CMD_CALL:
				if (iResult==NetworkEngine.BAD_RESULT)
					_iEngineStatus=ENGINE_STATUS_HEART_BEAT;
				else
				{
					if (iLenData>1){
						if (byData[3]==1){
							_eventProtocol.ConversationStarted();
							_iEngineStatus=ENGINE_STATUS_CONVERSATION;							
							EnableAV(true);							
						}
						else{
							_eventProtocol.RejectedFromHost();
							_iEngineStatus=ENGINE_STATUS_HEART_BEAT;
						}
					}
				}
				break;
			case NetworkEngine.CMD_USERS_LIST:
				if (iResult==NetworkEngine.BAD_RESULT)
					_eventProtocol.UsersList(null);
				else{
					String value = new String(byData,3,iLenData-2);
					value = value.replace("|", "/");
					value = value.replace(_sNickName, "");
					value = value.replace("//", "/");
					if (value.length()>1 && value.charAt(0)=='/')
						value = value.substring(1);
					String []sUsers=null;
					if (value.length()>0)
						sUsers = value.split("/");
					_eventProtocol.UsersList(sUsers);
				}
				_iEngineStatus=ENGINE_STATUS_HEART_BEAT;
				break;
				
			case NetworkEngine.CMD_LOGIN:
				if (iResult==NetworkEngine.BAD_RESULT)
					_eventProtocol.Logged(false);
				else
					_eventProtocol.Logged(true);
				break;
		}
	}
	public void CloseConversation(){
		if (_iEngineStatus == ENGINE_STATUS_CONVERSATION || _iEngineStatus == ENGINE_STATUS_OUTGOING || _iEngineStatus == ENGINE_STATUS_INCOMING){
			EnableAV(false);
			_iEngineStatus=ENGINE_STATUS_CLOSE_CONVERSATION;
			_eventProtocol.ConversationStopped();
		}
	}
	public void Accept(){
		_iEngineStatus=ENGINE_STATUS_CONVERSATION;
		byte []byByff = new byte[1];
		byByff[0]=1;
		_netEngine.SendCommand(NetworkEngine.CMD_RING, byByff, 1);
		EnableAV(true);
		_eventProtocol.ConversationStarted();
	}
	private void EnableAV(boolean bEnable){
		_eventProtocol.EnableCamera(bEnable);
		if (bEnable){
			_bAudioCaptureEnabled = _eventProtocol.IsAudioEnabled();
			if (_bAudioCaptureEnabled)
				_audioCapture.Start();
		}
		else{
			if (_bAudioCaptureEnabled)
				_audioCapture.Stop();
		}
		_icntFrameAmr=0;	
	}
	public void Reject(){
		byte []byByff = new byte[1];
		byByff[0]=0;
		_netEngine.SendCommand(NetworkEngine.CMD_RING, byByff, 1);
		_iEngineStatus=ENGINE_STATUS_HEART_BEAT;
	}
	public void MakeCall(String sUser2Call){
		_l1 = _iCntRing=0;
		_sUser2Call = sUser2Call;
		if (_bConnected)
			_iEngineStatus = ENGINE_STATUS_OUTGOING;
		else
			_iEngineStatus = ENGINE_STATUS_IDLE;
	}

	public void GetUserList(){
		if (_bConnected)
			_iEngineStatus = ENGINE_STATUS_GET_USERS_LIST;
		else
			_eventProtocol.UsersList(null);
	}
	
	public void AudioSignalMax(short value){
		_eventProtocol.SignalMax(value);
	}
	
	public void FrameAudioToEncode(byte byBuff[], int iSizeBuff, int iNBlocks){
		int ifrmsize;
		int itotsize=0;
		byte []byTmp = new byte[AudioCapture.SIZE_FRAME];
		for (int i=0;i<iNBlocks; i++){
			System.arraycopy(byBuff, i*AudioCapture.SIZE_FRAME, byTmp, 0, AudioCapture.SIZE_FRAME);
			 byte []byAmr = _Codecs.EncodeAudio(byTmp, AudioCapture.SIZE_FRAME);
			 ifrmsize = _Codecs.GetSizeBufferEncodedAudio();
			 System.arraycopy(byAmr, 0, _byAmrTX, i*ifrmsize, ifrmsize);
			 itotsize+=ifrmsize;
		}
		_iSizeAmrTX += itotsize;
		
		/*if (_iSizeAmrTX>0){
			Log.i("FrameAudioToEncode", "Size:"+_iSizeAmrTX);
			_netEngine.SendCommand(NetworkEngine.CMD_FRM_AUDIO, _byAmrTX, _iSizeAmrTX);
			_iSizeAmrTX=0;
		}*/
	}
	
	public void FrameVideoToEncode(byte []byFrame)
	{/*
		String sFileName ="/sdcard/video_"+_icntFrameE+"_h263.yuv";
		_icntFrameE++;
		File fo = new File(sFileName);
		FileOutputStream out;
		try {
			out = new FileOutputStream(fo, true);
			try {
				out.write(byFrame);
				out.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		
		return;*/
		//long l1 = SystemClock.uptimeMillis();
		if (_l2!=0)
			_iTimeStamp+=SystemClock.uptimeMillis()-_l2;
		_l2=SystemClock.uptimeMillis();
		byte []byH263 =_Codecs.EncodeVideo(byFrame, _iTimeStamp);		
		int iSize = _Codecs.GetSizeBufferEncodedVideo();
		/*if (iSize>0)
			_netEngine.SendCommand(NetworkEngine.CMD_FRM_VIDEO, byH263, iSize);*/
		
		if (iSize>0 && _iSizeH263TX==0){
			System.arraycopy(byH263, 0, _byH263TX, 0, iSize);
			//Log.i("VIDEO OUT", "POST FRAME VIDEO");
			_iSizeH263TX = iSize;
			//AudioSignalMax((short)0);
		}
		else if(_iSizeH263TX!=0){
			AudioSignalMax((short)1);
			Log.i("OOPPSS", "TOO SLOW THREAD.....................................");
		}
			
	}
	
	public void EnableRecorder(){
		String sStream = "stream.h263";
		_netEngine.SendCommand(NetworkEngine.CMD_RECORDER, sStream.getBytes(), sStream.length());
	}

}

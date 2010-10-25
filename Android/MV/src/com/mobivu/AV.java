package com.mobivu;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.media.MediaRecorder.AudioSource;
import android.os.Looper;
import android.os.ParcelFileDescriptor;
import android.os.SystemClock;
import android.util.Log;


public class AV implements Runnable {
	
	private boolean _bLoop, _bBinded;	
	private String _sIP, _sNickName;
	private Thread _th=null;
	private Socket _sc;
	private DatagramSocket _socket;
	private DatagramPacket _datagramOut, _datagramIn;
	private InetAddress _addr;
	private byte []_bufInUDP;
	private int _iCntV, _iCntA, _iPort;	
	Codecs _codecs;
	MobiVu _win;
	AudioCapture _audioRec=null;
	private AudioTrack _audioTrack;
	
	void Init(String sIP, String sNickName, int iPort, Codecs codec, MobiVu win, boolean bVideo){
		_win   		= win;
		_codecs		= codec;
		_sIP   		= sIP;
		_sNickName 	= sNickName;
		_iPort 		= iPort;
		_bLoop 		= true;
		_bBinded    = false;
		
		if (!bVideo){
			_audioRec = new AudioCapture(_codecs, this);			
			_audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 8000, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT, 10240, AudioTrack.MODE_STREAM);
	        _audioTrack.setStereoVolume(_audioTrack.getMaxVolume()/2, _audioTrack.getMaxVolume()/2);
		}
		ConnectSocketUDP();
		_datagramOut = new DatagramPacket(new byte[1],1);
		_datagramIn  = new DatagramPacket(new byte[1],1);		
		_datagramOut.setAddress(_addr);
		_datagramOut.setPort(_iPort);		

		_bufInUDP  = new byte[2048];
		_iCntV = 0;
		_iCntA = 0;
		                   
		_th = new Thread(this);
		_th.start();
	}
	
	
	public void Release(){
		if (_audioRec!=null)
			_audioRec.Stop();		
		_bLoop=false;
		if (_th!=null){
			try {Thread.sleep(500);} catch (InterruptedException e) {e.printStackTrace();}
			_th.stop();			
			//CloseSocketTCP();
			CloseSocketUDP();
		}
		_bufInUDP = null;	
	}
	
	@Override
	public void run() {
		Looper.prepare();
		int iRead, iTmp, iCntV, iCntA;
		iCntA=iCntV=iTmp=0;
		android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);		
		
		while (_bLoop){
			try{
				
				iRead = ReadUDP(_bufInUDP, _bufInUDP.length);
				if (iRead>0){
					switch (_bufInUDP[0])
					{
					case Engine.CMD_BIND_USER:
						if (_bufInUDP[1]==Engine.ACK)
							_bBinded=true;						
						break;
					case Engine.CMD_FRM_VIDEO:
						iTmp = _bufInUDP[2] & 0x000000FF;
						iTmp <<= 8;
						iTmp |= (_bufInUDP[1] & 0x000000FF);
						if (iTmp>iCntV || iCntV==0){							
							int[]iRGB=_codecs.DecodeVideo(_bufInUDP, iRead);							
							_win.ShowFrame(iRGB);							
						}
						break;
					case Engine.CMD_FRM_AUDIO:
						iTmp = _bufInUDP[2] & 0x000000FF;
						iTmp <<= 8;
						iTmp |= (_bufInUDP[1] & 0x000000FF);
						if (iTmp>iCntA || iCntA==0){
							iCntA = iTmp;
							byte[]byPCM=_codecs.DecodeAudio(_bufInUDP, iRead);							
							_audioTrack.write(byPCM, 0, ((iRead-3)/Codecs.FRAME_SIZE_AMR)*AudioCapture.SIZE_FRAME);
							_audioTrack.play();
							Log.i("Play", (((iRead-3)/Codecs.FRAME_SIZE_AMR)*AudioCapture.SIZE_FRAME)+"");
						}
						break;
					}
				}
				
			} catch (IOException e) {
				//e.printStackTrace();
			}
		}
		//audiorec.stop();
	}

	private void CloseSocketTCP(){
		try {
			_sc.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private boolean ConnectSocketTCP(){
		boolean bret=false;
		try{
			_sc = new Socket(_sIP, 8082); 
			_sc.setSoTimeout(200);
		}
		catch(IOException e){
			e.printStackTrace();
		}
		return bret;
	}
	private void CloseSocketUDP(){
		if (_socket!=null)
			_socket.disconnect();
	}
	private boolean ConnectSocketUDP(){
		try {
			try {_addr =  InetAddress.getByName(_sIP);} catch (UnknownHostException e) {e.printStackTrace();}
			_socket = new DatagramSocket();
			_socket.setSoTimeout(1000);			
		} catch (SocketException e) {
			e.printStackTrace();
		}
		try {_addr =  InetAddress.getByName(_sIP);} catch (UnknownHostException e) {e.printStackTrace();}		
		_socket.connect(_addr, _iPort);
		boolean bret=_socket.isConnected();		
		
		return bret;
	}
	private int ReadUDP(byte []byBuff, int iSize) throws IOException{
		_datagramIn.setData(byBuff, 0, iSize);
		_socket.receive(_datagramIn);
		return _datagramIn.getLength();
	}	
	private void WriteUDP(byte []byBuff, int iIdx, int iSize) throws IOException{
		_datagramOut.setData(byBuff, iIdx, iSize);
		_datagramOut.setLength(iSize);
		try {
			_socket.send(_datagramOut);
		} catch (IOException e) {
			
			e.printStackTrace();
		}
	}
	public void SendCmd(int iCmd, byte []byBuff, int iSize){
		try {			
			//synchronized(this) {
				switch (iCmd){
				case Engine.CMD_FRM_AUDIO:
					_iCntA++;
					byBuff[1] = (byte) (_iCntA & 0x000000FF);
					byBuff[2] = (byte) ((_iCntA>>8) & 0x000000FF);
					iSize+=3;
					break;
				case Engine.CMD_FRM_VIDEO:
					_iCntV++;
					byBuff[1] = (byte) (_iCntV & 0x000000FF);
					byBuff[2] = (byte) ((_iCntV>>8) & 0x000000FF);
					iSize+=2;
					break;
				case Engine.CMD_BIND_USER:
					iSize+=1;
					break;
				}
				byBuff[0] = (byte) iCmd;
				WriteUDP(byBuff, 0, iSize);				
			//}
			//try {Thread.sleep(5);} catch (InterruptedException e) {e.printStackTrace();}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	public void StartRec(){
		_audioRec.Start();
	}
	public void StopRec(){
		_audioRec.Stop();
	}
	public boolean IsBinded(){
		return _bBinded;
	}
	public int GetLocalPort(){
		if (!_socket.isConnected())
			ConnectSocketUDP();
		return _socket.getLocalPort();
	}
}


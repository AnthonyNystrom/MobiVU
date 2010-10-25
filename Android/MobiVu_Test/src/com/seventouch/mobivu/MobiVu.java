package com.seventouch.mobivu;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.lang.*;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Handler;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.os.*;

public class MobiVu extends Activity implements Runnable, View.OnClickListener, Camera.PreviewCallback, SurfaceHolder.Callback {
	Socket _sc;
	OutputStream _out=null;
	InputStream _insc;
	boolean _bEndThread;
	Thread _thread, _threadH263, _threadAudioRec;
	int _iStateMachine=STATE_MACHINE_NONE;
	int _iSizePack, _iUnKnowPkg, _iNVideoFrames, _iNAudioFrames;
	
	static final int STATE_MACHINE_NONE  		= 0;
	static final int STATE_MACHINE_HEART	 	= 1;
	static final int STATE_MACHINE_LOGOUT       = 2;
	static final int STATE_MACHINE_TEST_VIDEO   = 3;
	static final int STATE_MACHINE_TEST_AUDIO   = 4;
	static final int STATE_MACHINE_TEST_AV      = 5;
	
	byte _byBuf[] = new byte[1024*8];
	TextView _txtStatus, _txtVideo, _txtAudio, _txtUnknow;
	Button _butLogin, _butLogout, _butGetVideo, _butRecStart, _butRecStop, _butGetAudio, _butAV;
	ImageView _imgVideo;
	public Bitmap _bmpFrame=null;
	String _sUserName;
	AudioEngine _audioEng;
	ParcelFileDescriptor _pfd=null;
	DecoderH263 _dech263;
	Camera _Camera;
	
	static final int CMD_NULL   		= -1;
	static final int CMD_LOGIN  		= 0;
	static final int CMD_LOGOUT 		= 1;
	static final int CMD_FRAME_VIDEO 	= 3;
	static final int CMD_FRAME_AUDIO 	= 4;
	static final int CMD_TEST_AUDIO 	= 8;
	static final int CMD_TEST_VIDEO 	= 9;	
	static final int CMD_TEST_AV 		= 10;
	static final int CMD_RECORDER   	= 11;

	static final int TIME_HEART_BEATS=30000;
	
	static final int FRAME_WITDH  =176;
	static final int FRAME_HEIGHT =144;

	public native void InitDecoderAmrNb();
	public native void ReleaseDecoderAmrNb();
	public native byte[] DecodeAmrNb(byte []byData);
    public native int InitEncoder();    
    public native void ReleaseEncoder();
    public native int InitDecoder();
    public native void ReleaseDecoder();
    public native void InitLog();
    public native void ReleaseLog();
    public native int[] Decode(byte []byData, int iSizeData);

    static { 
        System.loadLibrary("encdec"); 
    }
	

	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        _txtStatus  = (TextView) findViewById(R.id.TextStatus);
        _txtVideo  = (TextView) findViewById(R.id.TextVideo);
        _txtAudio  = (TextView) findViewById(R.id.TextAudio);
        _txtUnknow  = (TextView) findViewById(R.id.TextUnknow);
        
        _butLogin = (Button)findViewById(R.id.butLogin);
        _butLogin.setOnClickListener(this);
        _butRecStart = (Button)findViewById(R.id.butRecStart);
        _butRecStart.setOnClickListener(this);                
        _butRecStop = (Button)findViewById(R.id.butRecStop);
        _butRecStop.setOnClickListener(this);                
        _butGetAudio = (Button)findViewById(R.id.butGetAudio);
        _butGetAudio.setOnClickListener(this);                        
        _butLogout = (Button)findViewById(R.id.butLogout);
        _butLogout.setOnClickListener(this);
        _butAV = (Button)findViewById(R.id.butAV);
        _butAV.setOnClickListener(this);

        
        _butGetVideo = (Button)findViewById(R.id.butGetVideo);
        _butGetVideo.setOnClickListener(this);
        _imgVideo = (ImageView)findViewById(R.id.ImageView01);
        
        InitLog();
        int iEnc=InitEncoder();
        int iDec=InitDecoder();
        InitDecoderAmrNb();
        //_txtStatus.setText( "Encoder init:"+iEnc+" Decoder Init:"+iDec);
        _txtStatus.setText( "Encoder init:"+iEnc);
        
        _bmpFrame = Bitmap.createBitmap(FRAME_WITDH, FRAME_HEIGHT, Bitmap.Config.ARGB_8888);
        
        SurfaceView surfaceView = (SurfaceView)findViewById(R.id.surface);
        SurfaceHolder surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        Log.i("OnCreate", "Init MainThread");
        _bEndThread=false;
		_thread  = new Thread(this);
		_thread.start();
		
		
		// init camera
        _Camera = Camera.open();
		Camera.Parameters parameters = _Camera.getParameters();
        parameters.setPreviewSize(FRAME_WITDH, FRAME_HEIGHT);
        parameters.setPreviewFormat(PixelFormat.YCbCr_420_SP);
        parameters.setPreviewFrameRate(15);
        _Camera.setParameters(parameters);
        _Camera.setPreviewCallback(this);

        /*
        _audioEng = new AudioEngine();
        _audioEng.Play();
        _threadAudioRec  = new Thread(_audioEng);
		_threadAudioRec.start();
        */
        
		/*_dech263 = new DecoderH263();
		_threadH263 = new Thread(_dech263);
		_threadH263.start();*/
		
		_sUserName = "Android";
    }
    
    
    public void onClick(View v) {
    	if (v.getId()==_butLogin.getId()){
    		if (ConnectSocket())
    		{
    			_txtStatus.setText("Connected");
    			_iStateMachine = STATE_MACHINE_HEART;
    		}
    		else
    			_txtStatus.setText("Connection error");
    	}
    	else if (v.getId()==_butLogout.getId()){
    		CloseSocket();    		
    		_txtStatus.setText("Connection closed");
    	}
    	else if (v.getId()==_butRecStart.getId()){
    		String sStream="AV_PCM";
    		//SendCmd((byte)CMD_RECORDER, sStream.getBytes());
    		//_audioEng.StartRec();
    		_Camera.startPreview();
    	}
    	else if (v.getId()==_butRecStop.getId()){
    		_Camera.stopPreview();
    		_audioEng.StopRec();
    	}
    	else if (v.getId()==_butAV.getId()){
    		_iStateMachine = STATE_MACHINE_TEST_AV;
    		String sStream="t";
    		SendCmd((byte)CMD_TEST_AV, sStream.getBytes());
    	}
    	else if (v.getId()==_butGetAudio.getId()){
    		_iStateMachine = STATE_MACHINE_TEST_AUDIO;
    		String sStream="t";
    		SendCmd((byte)CMD_TEST_AUDIO, sStream.getBytes());
    	}
    	else if (v.getId()==_butGetVideo.getId()){
    		_iStateMachine = STATE_MACHINE_TEST_VIDEO;
    		String sStream="t";
    		SendCmd((byte)CMD_TEST_VIDEO, sStream.getBytes());
    	}
    }
    
    public Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			try{
				switch (msg.what)
				{
				case CMD_LOGIN:
					if (msg.arg1==1)
						_txtStatus.setText("LOGIN COMMAND SUCCESSFUL");
					else
						_txtStatus.setText("LOGIN COMMAND FAILED");
		        	break;
				case CMD_LOGOUT:
					if (msg.arg1==1)
						_txtStatus.setText("LOGOUT COMMAND SUCCEFUL");
					else
						_txtStatus.setText("LOGOUT COMMAND FAILED");
		        	break;
				case CMD_FRAME_VIDEO:
					_txtVideo.setText("Frames Video:"+_iNVideoFrames);
					if (_bmpFrame!=null){
						_imgVideo.setImageBitmap(_bmpFrame);
					}
					break;
				case CMD_FRAME_AUDIO:
					_txtAudio.setText("Frames Audio:"+_iNAudioFrames);
					break;
				case CMD_TEST_VIDEO:
					break;
				case CMD_TEST_AUDIO:
					break;
				case CMD_NULL:
					_txtUnknow.setText("Unknow Pkg:"+_iUnKnowPkg);
					break;
				}
			}
			catch(Exception ex){
				
			}
		}
	};
    	
    
    public void run() {
		Looper.prepare();

		long l1=0;
		
		while (!_bEndThread){
			
			switch (_iStateMachine){
			case STATE_MACHINE_NONE:
				break;
			case STATE_MACHINE_HEART:
				if (l1==0 || SystemClock.uptimeMillis()-l1 > TIME_HEART_BEATS){
					l1=SystemClock.uptimeMillis();
					SendCmd((byte)CMD_LOGIN, _sUserName.getBytes());
					if (Parser((byte)CMD_LOGIN))
						handler.sendMessage(handler.obtainMessage(CMD_LOGIN, 1, 0));					
					else
						handler.sendMessage(handler.obtainMessage(CMD_LOGIN, 0, 0));
				}
				break;
			case STATE_MACHINE_LOGOUT:				
				_iStateMachine = STATE_MACHINE_NONE;
				SendCmd((byte)CMD_LOGOUT, _sUserName.getBytes());
				break;
			case STATE_MACHINE_TEST_VIDEO:
				Parser((byte)CMD_TEST_VIDEO);
				break;
			case STATE_MACHINE_TEST_AUDIO:
				Parser((byte)CMD_TEST_AUDIO);
				break;
			case STATE_MACHINE_TEST_AV:
				Parser((byte)CMD_TEST_AUDIO);
				break;
			}
		}
    }
    
    // return true if the command is received
    // byCmd : command to receive
    boolean Parser(byte byCmd){
    	boolean bRet=false;
		try {
	    	if (_insc!=null){
	    		int iRead;
				iRead = _insc.read(_byBuf, 0, 3);
	    		if (iRead==3){
	    			//Log.i("Parser", "iRead:"+iRead + "available:"+_insc.available());
	    			_iSizePack = _byBuf[1] & 0x0000001F;
	                _iSizePack <<= 8;
	                _iSizePack |= (_byBuf[0] & 0x000000FF);
	                
	                int iCmd =_byBuf[2];
	                //Log.i("Pkg.Size", ""+_iSizePack);
	                _iSizePack--;
	                
	                if (_iSizePack>0){
		                iRead=0;
		                do
		                	iRead +=_insc.read(_byBuf, iRead, _iSizePack-iRead);
		                while (iRead<_iSizePack);
		                
		                //Log.i("Parser", "iRead:"+iRead + "available:"+_insc.available());
	                	bRet=true;
		                switch (iCmd)
		                {
		                case CMD_LOGIN:
		                	break;
		                case CMD_LOGOUT:
		                	break;
		                case CMD_TEST_VIDEO:	                	
		                	break;
		                case CMD_FRAME_VIDEO:
		                	_iNVideoFrames++;
		                	//_dech263.DecodeH263(_byBuf, _iSizePack);
		                	int []iRGB = Decode(_byBuf, _iSizePack);
		                	_bmpFrame.setPixels(iRGB, 0, FRAME_WITDH, 0, 0, FRAME_WITDH, FRAME_HEIGHT);
		                	//Log.i("Parse", "CMD_FRAME_VIDEO:");
		                	handler.sendMessage(handler.obtainMessage(CMD_FRAME_VIDEO, 0, 0));
		                	break;
		                case CMD_FRAME_AUDIO:
		                	//Log.i("Parse", "CMD_FRAME_AUDIO");
		                	/*byte []byPCM = DecodeAmrNb(_byBuf);
		                	_audioRec.WriteAudioTrack(byPCM);*/
		                	_audioEng.WritePlay(_byBuf);
		                	_iNAudioFrames++;
		                	handler.sendMessage(handler.obtainMessage(CMD_FRAME_AUDIO, 0, 0));
		                	break;
		                default:
		                	_iUnKnowPkg++;
		                	Log.i("Parser", "Unknow command");
		                	handler.sendMessage(handler.obtainMessage(CMD_NULL, 0, 0));
		                	break;
		                }
	                }
	    		}
	    		else if (iRead>0)
	    			Log.i("Parser", "Header Broken");
	    			
	    	}
		} catch (IOException e) {
			Log.i("Parser", "IOException:"+e.getMessage());
			e.printStackTrace();
		}
    	return bRet;
    }
    
    int[] Byte2Int(byte[] byBuf){
    	
    	int []iBuf = new int[FRAME_WITDH*FRAME_HEIGHT];
    	byte []byTmp = new byte[4];
    	int i, ii, iby, iAddr;
    	
    	for(i=0, iby=0, iAddr=0;i<FRAME_HEIGHT;i++){
    		for(ii=0;ii<FRAME_WITDH;ii++)
    		{
    			byTmp[0]=(byte)0xFF;		// alpha
    			byTmp[1]=byBuf[iby];
    			byTmp[2]=byBuf[iby+1];
    			byTmp[3]=byBuf[iby+2];
    			
    			
    			iBuf[iAddr] = makeIntFromByte4(byTmp);
    			iby+=3;
    			iAddr++;
    		}
    	}
    	return iBuf;
    }
    
    public static final int makeIntFromByte4(byte[] b) {
		return b[0]<<24 | (b[1]&0xff)<<16 | (b[2]&0xff)<<8 | (b[3]&0xff);
	}
    
    void SendCmd(byte cmd, byte []buf){
    	
    	short iLen=1;
    	if (buf!=null)
    		iLen+=(short)(buf.length);
    	
    	SetCheckSum(_byBuf, iLen, cmd);
    	try {
    		if (iLen>1)
    			System.arraycopy(buf, 0, _byBuf, 3, buf.length);
    		//_insc.reset();
			_out.write(_byBuf, 0, iLen+2);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    
    void SetCheckSum(byte []buf, short iLen, byte cmd){
    	short iChkSum= (short) (((iLen & 0x1FFF) % 0x07) ^ 0x05);
    	buf[0] = (byte) (iLen & 0x00FF);
    	buf[1] = (byte) ((iLen>>8) & 0x0FF);
    	buf[1] = (byte) ((iChkSum<<5) | buf[1]);
    	buf[2] = cmd;
    	
    }
    public void onPreviewFrame (byte[] data, Camera camera){
/*    	if (camera.getParameters().getPreviewFormat()==PixelFormat.YCbCr_420_SP){
    		Log.i("onPreviewFrame", "FORMAT YUV420");
    	}*/
    	_txtVideo.setText("Frames Video:"+_iNVideoFrames);
    	_iNVideoFrames++;
    }
    
    protected void onDestroy() {
		super.onDestroy();
		_bEndThread=true;     
		_dech263._bEndThread=true;
		_dech263=null;
		_audioEng._bEndThread=true;
		_audioEng.Stop();
		_audioEng.StopRec();
		_audioEng=null;
		_Camera.stopPreview();
		_Camera.release();
		
        ReleaseEncoder();
        ReleaseDecoder();
        ReleaseDecoderAmrNb();
        
        ReleaseLog();
		CloseSocket();
	}

    
    void CloseSocket(){
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
		}
	}
    
    public void surfaceCreated(SurfaceHolder holder) {
        // The Surface has been created, acquire the camera and tell it where
        // to draw.
        try {
           _Camera.setPreviewDisplay(holder);
        } catch (IOException exception) {
            _Camera.release();
            _Camera = null;
            // TODO: add more exception handling logic here
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        // Surface will be destroyed when we return, so stop the preview.
        // Because the CameraDevice object is not a shared resource, it's very
        // important to release it when the activity is paused.    	
    	if (_Camera!=null)
    		_Camera.stopPreview();
        _Camera = null;
    }

    
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        // Now that the size is known, set up the camera parameters and begin
        // the preview.
    }
    boolean ConnectSocket(){
		boolean bret=false;
		try{
			_sc = new Socket("69.21.114.135",8082); 
			_sc.setSoTimeout(120000);
			_sc.setReceiveBufferSize(1024*8);
	    	_out = _sc.getOutputStream();
	    	_insc  = _sc.getInputStream();
	    	bret=true;
		}
		catch(IOException ex){
			
		}
		return bret;
	}
    
    
    public class AudioEngine implements Runnable{
    	public boolean _bEndThread=false;
    	AudioRecord _recorder;
    	AudioTrack _audioTrack;
    	short []_iBuffer;
    	byte []_alaw;
    	static final int BUFFER_SIZE = 1024;
    	G711 _g711;
    	
    	AudioEngine(){
    		_recorder = new AudioRecord(MediaRecorder.AudioSource.MIC, 8000, 
    				AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_8BIT, BUFFER_SIZE);
    		_iBuffer = new short[BUFFER_SIZE];
		   _audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 8000, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT, BUFFER_SIZE, AudioTrack.MODE_STREAM);
    	}
    	
    	public void Play(){
    		_audioTrack.play();
    	}
    	public void Stop(){
    		_audioTrack.stop();
    	}
    	public void WritePlay(byte []buff){
    		_audioTrack.write(buff, 0, buff.length);
    	}
    	
    	public void StartRec(){
    		//_recorder.startRecording();
    	}
    	public void StopRec(){
    		_recorder.startRecording();
    	}
    	
    	public void run() {
    		int iRead;
    		while (!_bEndThread) {
    			if (_recorder.getState()==_recorder.RECORDSTATE_RECORDING){
    				iRead = _recorder.read(_iBuffer, 0, BUFFER_SIZE);
    				if (iRead>0){
    					_iNAudioFrames++;
						handler.sendMessage(handler.obtainMessage(CMD_FRAME_AUDIO, 0, 0));
    				
    					Log.i("AudioEngine", "AUDIO CAPTURED:"+iRead);
    				}
    			}
    		}
    	}
    }
    
	public class DecoderH263 implements Runnable{
		public boolean _bEndThread=false;
		ArrayList _FramesH263 = new ArrayList();
		
		void DecodeH263(byte []byBuff, int iSizeBuff){
			byte []buff = new byte[iSizeBuff];
			System.arraycopy(byBuff, 0, buff, 0, iSizeBuff);
			_FramesH263.add(buff);
			//Log.i("DecodeH263", "New frame to decode:"+_FramesH263.size() );
		}
	    public void run() { 
	    	Looper.prepare();
	    	InitDecoder();
	         try { 
	              while (!_bEndThread) {
	            	  if (_FramesH263.size()>0)
	            	  {
	            		  //Log.i("DecodeH263:run", "decoding:"+_FramesH263.size() );
	            		  byte []buff=(byte [])_FramesH263.get(0);
	            		  int []iARGB = Decode(buff, buff.length);
	            		  if (iARGB.length>0){
	            			  _bmpFrame.setPixels(iARGB, 0, FRAME_WITDH, 0, 0, FRAME_WITDH, FRAME_HEIGHT);
		                		handler.sendMessage(handler.obtainMessage(CMD_FRAME_VIDEO, 0, 0));
	            		  }
	            		  _FramesH263.remove(0);
	            	  }
	            	  /**else
	            		  Thread.sleep(5);*/
	              } 
	         } catch (Exception e) { 
	        	 Log.e("Decoder thread", "S: Error", e); 
	         }
	         ReleaseDecoder();
	    } 
	} 
   
}
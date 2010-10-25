package com.mobivu;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder.AudioSource;
import android.os.Looper;
import android.os.SystemClock;
import android.util.Log;

public class AudioCapture implements Runnable {
    AudioRecord _audiorec;
    boolean _bRecordingAudio;
    Thread _th;
    boolean _bExitTh;
    byte []_byAudioBuffer;    
    int _iSizeBuffer;
    int _iTotalBytes;    
    FileOutputStream _out;
    Engine _engine;
    int _iCntFrame, _iCntGood;
    short _iMax;
    int _iSizeFrame;
    public static final int SIZE_FRAME = 320;
    public static final int FRAMES_PER_BLOCK = 25;
    public static final int LEVEL2SEND = 1000;
    
    AudioCapture(Engine engine){
    	_iMax=0;
    	_iCntGood=0;
    	_engine = engine;
        _audiorec = new AudioRecord(AudioSource.MIC, 8000, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT, 1024*8);        
        _bRecordingAudio = false;
        _th = new Thread(this);
        _iSizeBuffer = 10240;
        _byAudioBuffer = new byte[_iSizeBuffer];        
    }

	public void run() {
		if (!_bRecordingAudio)
			return;
		//Looper.prepare();
		int iRead;
		
		//while(!_bExitTh){
			iRead = _audiorec.read(_byAudioBuffer, _iTotalBytes, SIZE_FRAME);
			_iCntFrame++;
			if (_iCntFrame>=FRAMES_PER_BLOCK){
				short i, iVal=0;				
				int iBuff= _iCntFrame*FRAMES_PER_BLOCK;

				for (i=0, iVal=0, _iMax=0; i<iBuff; i+=2)
				{
					iVal = (short) (_byAudioBuffer[i+1] & 0x00FF);
					iVal<<=8;
					iVal |= (short)(_byAudioBuffer[i] & 0x00FF);					
					if (iVal>_iMax)
						_iMax=iVal;
				}
				if (_iMax>=LEVEL2SEND || _iCntGood< (100/FRAMES_PER_BLOCK)) // 2 seconds later
				{	
					//_engine.AudioSignalMax((short)1);
					_engine.FrameAudioToEncode(_byAudioBuffer, iRead, _iCntFrame);
					
					if (_iMax>=LEVEL2SEND)
						_iCntGood=0;
					else
						_iCntGood++;
				}
				else
					_engine.AudioSignalMax((short)0);
					
				_iTotalBytes = 0;
				_iCntFrame   = 0;
			}
			else
			_iTotalBytes += iRead;
			
			/* 
			if (iRead>0){
				try {
					_out.write(_byAudioBuffer, 0, iRead);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			*/
		//}
	}
	void InitFilePCM(){
		File fo = new File("/sdcard/captured.pcm"); 	
		try {
			_out = new FileOutputStream(fo, true);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	void CloseFilePCM(){
		try {
			_out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
		
	public void Start(){
		if (!_bRecordingAudio){
			//InitFilePCM();
			_iCntFrame=0;
			_iTotalBytes = 0;
			_audiorec.startRecording();
			_bRecordingAudio = true;
			_bExitTh=false;			
			//_th.start();
		}
	}
	public int Stop(){
		if (_bRecordingAudio){
			//CloseFilePCM();
			_audiorec.stop();
			_bRecordingAudio = false;
			_bExitTh=true;
			try {Thread.sleep(1000);} catch (InterruptedException e) {e.printStackTrace();}
			//_th.stop();
		}
		return GetBytesRead();
	}
	public int GetBytesRead(){
		return _iTotalBytes;
	}
	
}

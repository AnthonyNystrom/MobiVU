package com.android.opencore;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder.AudioSource;
import android.os.Looper;
import android.util.Log;

public class AudioCapture implements Runnable {
    AudioRecord _audiorec;
    boolean _bRecordingAudio;
    Thread _th;
    boolean _bExitTh;
    byte []_byAudioBuffer;
    int _iSizeBuffer;
    int _iTotalBytes;    
    Codecs _Codecs = null;
    FileOutputStream _out;
    
    AudioCapture(Codecs codecs){
    	_Codecs = codecs;
        _audiorec = new AudioRecord(AudioSource.MIC, 8000, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT, 1024*8);
        _bRecordingAudio = false;
        _th = new Thread(this);
        _byAudioBuffer = new byte[1024];
        _iSizeBuffer = 1024;
    }

	public void run() {
		Looper.prepare();
		int iRead;
		Log.i("run", "Audio Capture");
		while(!_bExitTh){
			iRead = _audiorec.read(_byAudioBuffer, 0, 320);
			_iTotalBytes += iRead; 
			if (iRead>0){
				try {
					_out.write(_byAudioBuffer, 0, iRead);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				//_Codecs.EncodeAudioAMR(_byAudioBuffer, iRead);
			}
		}
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
			InitFilePCM();
			_audiorec.startRecording();
			_bRecordingAudio = true;
			_bExitTh=false;
			_iTotalBytes = 0;
			_th.start();
		}
	}
	public int Stop(){
		if (_bRecordingAudio){
			CloseFilePCM();
			_audiorec.stop();
			_bRecordingAudio = false;
			_bExitTh=true;
			try {Thread.sleep(1000);} catch (InterruptedException e) {e.printStackTrace();}
			_th.stop();
		}
		return GetBytesRead();
	}
	public int GetBytesRead(){
		return _iTotalBytes;
	}
	
}

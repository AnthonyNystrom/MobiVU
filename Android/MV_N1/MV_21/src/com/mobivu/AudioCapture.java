package com.mobivu;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder.AudioSource;
import android.os.Looper;
import android.util.Log;

public class AudioCapture implements Runnable {
    AudioRecord _audiorec;
    boolean _bRecordingAudio, _bLoop;
    byte []_byAudioBuffer;    
    Codecs _codecs;
    AV _engine;
    private Thread _th=null;
    public static final int SIZE_FRAME = 320;
    public static final int FRAMES_PER_BLOCK = 12;
    
    AudioCapture(Codecs codec, AV engine){
    	_engine   = engine;
    	_codecs   = codec;
        _audiorec = new AudioRecord(AudioSource.MIC, 8000, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT, 1024*8);       
        _bRecordingAudio = false;
        _byAudioBuffer = new byte[AudioCapture.SIZE_FRAME*(AudioCapture.FRAMES_PER_BLOCK*2)];
    }
	public void Start(){
		if (!_bRecordingAudio && _engine.AudioEnabled()){
			_bLoop = true;
	        _th = new Thread(this);
			_th.start();
			_audiorec.startRecording();
			_bRecordingAudio = true;
		}
	}
	public void Stop(){
		if (_bRecordingAudio){
			_bLoop=false;
			try {Thread.sleep(500);} catch (InterruptedException e) {e.printStackTrace();}
			_th.stop();
			_audiorec.stop();
			_bRecordingAudio = false;
		}
	}

	@Override
	public void run() {
		Looper.prepare();
		android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_AUDIO);
		int iRead=0, iEncoded;
		while (_bLoop){
			iRead = _audiorec.read(_byAudioBuffer, 0, AudioCapture.SIZE_FRAME*AudioCapture.FRAMES_PER_BLOCK);					
			byte []byAmr = _codecs.EncodeAudio(_byAudioBuffer, iRead);					
			iEncoded = _codecs.GetSizeBufferEncodedAudio();
			if (iEncoded>0)
				_engine.SendCmd(Engine.CMD_FRM_AUDIO, byAmr, iEncoded);
		}
	}	
	
}

package com.seventouch.mobivu;
import java.io.File;
import java.io.IOException;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Environment;
import android.os.ParcelFileDescriptor;

public class AudioRecorder {

  final MediaRecorder recorder = new MediaRecorder();
  final MediaPlayer player = new MediaPlayer();
  AudioTrack _audioTrack;
  final String path;
  boolean _bRecording=false;
  ParcelFileDescriptor _pfd=null;
  public static final int BUFFER_SIZE = 1024;
  
  public void SetFileDes(ParcelFileDescriptor fd){
	  _pfd=fd;
  }
  public boolean IsRecording(){
	  return _bRecording;
  }
  /**
   * Creates a new audio recording at the given path (relative to root of SD card).
   */
  public AudioRecorder(String path) {
    this.path = sanitizePath(path);
  }
  
  public void InitAudioTrack(){
	  _audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 8000, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT, BUFFER_SIZE, AudioTrack.MODE_STREAM);
	  _audioTrack.play();
  }
  public void ReleaseAudioTrack(){
	  _audioTrack.stop();
  }
  public void WriteAudioTrack(byte []byBuff){
	  _audioTrack.write(byBuff, 0, byBuff.length);
  }
  
  private String sanitizePath(String path) {
    if (!path.startsWith("/")) {
      path = "/" + path;
    }
    if (!path.contains(".")) {
      path += ".amr";
    }
    return Environment.getExternalStorageDirectory().getAbsolutePath() + path;
  }

  /**
   * Starts a new recording.
   */
  public void start(boolean bRec) throws IOException {
    String state = android.os.Environment.getExternalStorageState();
    if(!state.equals(android.os.Environment.MEDIA_MOUNTED))  {
        throw new IOException("SD Card is not mounted.  It is " + state + ".");
    }

    // make sure the directory we plan to store the recording in exists
    File directory = new File(path).getParentFile();
    if (!directory.exists() && !directory.mkdirs()) {
      throw new IOException("Path to file could not be created.");
    }
    if (bRec){
	    recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
	    recorder.setOutputFormat(MediaRecorder.OutputFormat.RAW_AMR);
	    recorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
	    recorder.setOutputFile(path);
	    recorder.prepare();
	    recorder.start();
	    _bRecording=true;
    }
    else
    {	if (_pfd!=null)
    		player.setDataSource(_pfd.getFileDescriptor());
    	else
    		player.setDataSource(path);
    	player.prepare();
    	player.start();
    }
  }
  

  /**
   * Stops a recording that has been previously started.
   */
  public void stop() throws IOException {
	_bRecording=false;
    recorder.stop();
    recorder.release();
  }

}

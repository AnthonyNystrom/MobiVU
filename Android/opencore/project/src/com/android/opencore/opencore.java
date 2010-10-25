package com.android.opencore;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import android.app.Activity;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class opencore extends Activity  implements View.OnClickListener, ICodecs{
    
    Button _butDecodeVideo, _butEncodeVideo, _butStartMic, _butStopMic, _butEncodeAudio, _butDecodeAudio;
    int _icntFrameD=1, _icntFrameE=1;
    AudioCapture _audioCapt;
    Codecs _Codecs;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        _butDecodeVideo = (Button)findViewById(R.id.butDecode);
        _butDecodeVideo.setOnClickListener(this);
        _butEncodeVideo = (Button)findViewById(R.id.butEncode);
        _butEncodeVideo.setOnClickListener(this);
        _butStartMic = (Button)findViewById(R.id.butStartMic);
        _butStartMic.setOnClickListener(this);
        _butStopMic = (Button)findViewById(R.id.butStopMic);
        _butStopMic.setOnClickListener(this);
        _butEncodeAudio = (Button)findViewById(R.id.butEncodeAudio);
        _butEncodeAudio.setOnClickListener(this);
        _butDecodeAudio = (Button)findViewById(R.id.butDecodeAudio);
        _butDecodeAudio.setOnClickListener(this);
        
        _Codecs = new Codecs();
        _Codecs.Init();        
        _audioCapt = new AudioCapture(_Codecs);
    }
    protected void onDestroy() {    	
    	_audioCapt.Stop();
    	_Codecs.Release();
    }
    
	public void onClick(View v) {
		if (v.getId()==_butDecodeVideo.getId()){
			DecodeVideo();			
		}		
		else if (v.getId()==_butEncodeVideo.getId()){
			EncodeVideo();
		}
		else if (v.getId()==_butStartMic.getId()){
			_audioCapt.Start();
		}
		else if (v.getId()==_butEncodeAudio.getId()){
			EncodeAudio();
		}
		else if (v.getId()==_butDecodeAudio.getId()){
			DecodeAudio();
		}
		else if (v.getId()==_butStopMic.getId()){
			int iRead = _audioCapt.Stop();
			TextView t = (TextView)findViewById(R.id.textInfo);
			t.setText("Bytes Catturati:"+iRead);
		}
	}
	public void DecodeVideo(){
    	/*_icntFrameD++;
    	if (_icntFrameD>222)
    		_icntFrameD=1;
    	
		try {
			String sFileName1 ="/sdcard/video_"+_icntFrameD+".h263";			
			Log.i("DecodeVideo", sFileName1);
			File f = new File(sFileName1);
			FileInputStream in = new FileInputStream(f);
			if (f.length()<=0)
				return;
			
			String sFileName ="/sdcard/video_"+_icntFrameD+"_h263.yuv";			
			File fo = new File(sFileName); 			
			FileOutputStream out = new FileOutputStream(fo);
			

	    	byte []bybuf=new byte[(int)f.length()*2];
	    	try {
				int iRead = in.read(bybuf,0,(int)f.length());
				
				in.close();	
				if (iRead>0){
					Log.i("DecodeVideo", "Start:"+_icntFrameD+":"+f.length());
					long l1 = SystemClock.uptimeMillis();				
					byte []byYUV = Decode(bybuf, (int)f.length());					
					Log.i("DecodeVideo", ""+(SystemClock.uptimeMillis()-l1));
					out.write(byYUV);
				}
				else{
					Log.i("DecodeVideo", "error to read file:"+sFileName1);
				}
			} catch (IOException e) {
				Log.i("DecodeVideo", e.getMessage());
				e.printStackTrace();
			}
			
		} catch (FileNotFoundException e) {
			Log.i("DecodeVideo", e.getMessage());
			e.printStackTrace();
		}*/
    }
	public void EncodeVideo(){
    	/*_icntFrameE++;
    	if (_icntFrameE>222)
    		_icntFrameE=1;
    	
		try {
			String sFileName ="/sdcard/video_"+_icntFrameE+"_h263.yuv";
			Log.i("EncodeVideo", sFileName);
			File f = new File(sFileName);
			FileInputStream in = new FileInputStream(f);
			
			sFileName ="/sdcard/video_"+_icntFrameE+".h263";
			sFileName ="/sdcard/video"+".h263";
			File fo = new File(sFileName); 	
			if (_icntFrameE==1)
				fo.delete();
			FileOutputStream out = new FileOutputStream(fo, true);
			

	    	byte []bybuf=new byte[38016];
	    	try {
				int iNRead = in.read(bybuf,0,38016);
				if (iNRead!=38016){
					Log.i("EncodeVideo", "Error read yuv file");
				}
				else{
					in.close();				
					Log.i("EncodeVideo", "Start:"+_icntFrameE);
					long l1 = SystemClock.uptimeMillis();
					bybuf=EncodeVideo(bybuf);
					Log.i("EncodeVideo", ""+GetSizeBufferEncodedVideo()+":"+(SystemClock.uptimeMillis()-l1));
					out.write(bybuf, 0, GetSizeBufferEncodedVideo());
				}
				out.close();
			} catch (IOException e) {
				Log.i("EncodeVideo", e.getMessage());
				e.printStackTrace();
			}
			
		} catch (FileNotFoundException e) {
			Log.i("EncodeVideo", e.getMessage());
			e.printStackTrace();
		}*/
    }
	public void AmrEncoded(byte[] byBuff, int iSizeBuff) {
		TextView t = (TextView)findViewById(R.id.textInfo);
		t.setText("AMR encoded:"+iSizeBuff);		
	}
	void DecodeAudio(){
		File f = new File("/sdcard/captured.amr");
		try {
			FileInputStream in = new FileInputStream(f);
			File fo = new File("/sdcard/captured2.pcm");
			if (fo.exists())
				fo.delete();
			FileOutputStream out = new FileOutputStream(fo, true);
			byte []byH = new byte[6];			
			/*try {
				in.read(byH);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}*/
			
			byte []bybuf = new byte[1024];
			try {
				int iNRead=1;
				int iTot=0;
				while (iNRead>0)
				{	iNRead = in.read(bybuf,0,13);
					if (iNRead>0){
						iTot+=iNRead;
						long l1 = SystemClock.uptimeMillis();
						byte []byPcm = _Codecs.DecodeAudio(bybuf, iNRead);
						Log.i("DecodeAudio", "Amr Decoded:"+(SystemClock.uptimeMillis()-l1)+" "+ iTot);
						out.write(byPcm, 0, _Codecs.GetSizeBufferDecodedAudio());
					}
				}
				in.close();
				out.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}	
	void EncodeAudio(){
		File f = new File("/sdcard/captured.pcm");
		try {
			FileInputStream in = new FileInputStream(f);
			File fo = new File("/sdcard/captured.amr");
			if (fo.exists())
				fo.delete();
			FileOutputStream out = new FileOutputStream(fo, true);
			byte []byH = new byte[6];
			byH[0]='#';byH[1]='!';byH[2]='A';byH[3]='M';byH[4]='R';byH[5]=0x0A;
			/*try {
				out.write(byH);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}*/
			
			byte []bybuf = new byte[1024];
			try {
				int iNRead=1;
				while (iNRead>0)
				{	iNRead = in.read(bybuf,0,320);
					if (iNRead>0){
						long l1 = SystemClock.uptimeMillis();
						byte []byAmr = _Codecs.EncodeAudio(bybuf, iNRead);
						Log.i("EncodeAudio", "Amr Encoded:"+(SystemClock.uptimeMillis()-l1)+":"+_Codecs.GetSizeBufferEncodedAudio());
						out.write(byAmr, 0, _Codecs.GetSizeBufferEncodedAudio());
					}
				}
				in.close();
				out.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
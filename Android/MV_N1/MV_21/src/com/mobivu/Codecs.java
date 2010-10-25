package com.mobivu;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.os.SystemClock;
import android.util.Log;

public class Codecs  {
	static {
		System.loadLibrary("opencore");
	}
// NATIVE OPENCORE FUNCTIONS ********************************
	public native int InitOpencore();
	public native void ReleaseOpencore();
	
	public native int InitDecoderVideo(int iW, int iH, int iWRsz, int iHRsz);
	public native void ReleaseDecoderVideo();
	public native int[] DecodeVideo(byte[] byData, int iSize);	
	public native int GetSizeBufferDecodedVideo();
	
	public native int InitEncoderVideo(int iW, int iH, int iQ);
	public native void ReleaseEncoderVideo();
	public native byte[] EncodeVideo(byte[] byData, int iTimeStamp);
	public native int GetSizeBufferEncodedVideo();
	
	public native int InitEncoderAudio();
	public native byte[] EncodeAudio(byte[] byData, int iSizeData);
	public native void ReleaseEncoderAudio();	
	public native int GetSizeBufferEncodedAudio();

	public native int InitDecoderAudio();
	public native byte[] DecodeAudio(byte[] byData, int iSizeData);
	public native void ReleaseDecoderAudio();	
	public native int GetSizeBufferDecodedAudio();
// END NATIVE OPENCORE FUNCTIONS
	
	static final int FRAME_WITDH        = 176;
	static final int FRAME_HEIGHT       = 144;
	//static final int FRAME_WITDH_RSZ    = 264;
	//static final int FRAME_HEIGHT_RSZ   = 216;
	static final int FRAME_WITDH_RSZ    = 440;
	static final int FRAME_HEIGHT_RSZ   = 360;
	static final int FRAME_SIZE_AMR = 13;
	
	static final int BITRATE_LOW = 20000;
	static final int BITRATE_MID = 70000;
	static final int BITRATE_HIGH = 100000;
	
	boolean _bOpencoreInitialized = false;
	FileOutputStream _out;
	boolean _bDeleteFile = true;
	long _l1;
	
	public boolean Init() {
		if (!_bOpencoreInitialized){
			if (InitOpencore() == 0)
				return false;
			if (InitDecoderVideo(FRAME_WITDH, FRAME_HEIGHT, FRAME_WITDH_RSZ, FRAME_HEIGHT_RSZ) == 0)
				return false;
			if (InitEncoderVideo(FRAME_WITDH, FRAME_HEIGHT, BITRATE_MID) == 0)
				return false;
			if (InitEncoderAudio()==0)
				return false;
			if (InitDecoderAudio()==0)
				return false;
			_bOpencoreInitialized = true;
			/*
			File fo = new File("/sdcard/captured.amr"); 	
			try {
				_out = new FileOutputStream(fo, true);
				// write header
				byte []byH = new byte[6];
				byH[0]='#';byH[1]='!';byH[2]='A';byH[3]='M';byH[4]='R';byH[5]=0x0A;
				try {
					_out.write(byH);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}*/

		}
		
		return true;
	}

	public void Release() {
		if (_bOpencoreInitialized){
			/*try {
				_out.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}*/
			ReleaseEncoderAudio();
			ReleaseDecoderAudio();
			ReleaseDecoderVideo();
			ReleaseEncoderVideo();
			ReleaseOpencore();
		}
	}

	public void EncodeAudioAMR(byte[] byData, int iSizeBuff) {
		byte[] byEncoded = EncodeAudio(byData, iSizeBuff);
		/*try {
			_out.write(byEncoded, 0, GetSizeBufferEncodedAudio());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}*/
	}
	

}


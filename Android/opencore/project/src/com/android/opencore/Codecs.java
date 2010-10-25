package com.android.opencore;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class Codecs {
	static {
		System.loadLibrary("opencore");
	}
// NATIVE OPENCORE FUNCTIONS ********************************
	public native int InitOpencore();
	public native void ReleaseOpencore();
	
	public native int InitDecoderVideo(int iW, int iH);
	public native void ReleaseDecoderVideo();
	public native byte[] DecodeVideo(byte[] byData, int iSize);	
	public native int GetSizeBufferDecodedVideo();
	
	public native int InitEncoderVideo(int iW, int iH);
	public native void ReleaseEncoderVideo();
	public native byte[] EncodeVideo(byte[] byData);
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
	
	boolean _bOpencoreInitialized = false;
	FileOutputStream _out;

	public boolean Init() {
		if (!_bOpencoreInitialized){
			if (InitOpencore() == 0)
				return false;
			if (InitDecoderVideo(176, 144) == 0)
				return false;
			if (InitEncoderVideo(176, 144) == 0)
				return false;
			if (InitEncoderAudio()==0)
				return false;
			if (InitDecoderAudio()==0)
				return false;
			_bOpencoreInitialized = true;
			
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
			}

		}
		
		return true;
	}

	public void Release() {
		if (_bOpencoreInitialized){
			try {
				_out.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			ReleaseEncoderVideo();
			ReleaseDecoderVideo();
			ReleaseEncoderAudio();
			ReleaseDecoderAudio();
			ReleaseOpencore();
		}
	}

	public void EncodeAudioAMR(byte[] byData, int iSizeBuff) {
		byte[] byEncoded = EncodeAudio(byData, iSizeBuff);
		try {
			_out.write(byEncoded, 0, GetSizeBufferEncodedAudio());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}


package com.mobivu;

import android.util.AttributeSet;
import android.view.SurfaceView;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.Bitmap.Config;
import android.view.SurfaceHolder;

public class FrameView extends SurfaceView{
	SurfaceHolder _holder;
	Bitmap _bmp;
	private final Matrix _Matrix = new Matrix();
	
	public FrameView(Context context) {
		super(context);
		_holder = getHolder();		
	}
    public FrameView(Context context, AttributeSet attrs) {
        super(context, attrs);
        _holder = getHolder();        
        _Matrix.setScale(1.5f, 1.5f);        
        _bmp = Bitmap.createBitmap(Codecs.FRAME_WITDH, Codecs.FRAME_HEIGHT,  Config.ARGB_8888);
    }

    public FrameView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        _holder = getHolder();
    }	
	public void DrawFrame(int []iBmp){
		Canvas c =_holder.lockCanvas(null);
		//c.drawBitmap(iBmp, 0, Codecs.FRAME_WITDH,0,0,Codecs.FRAME_WITDH,Codecs.FRAME_HEIGHT,false,null);
		c.drawBitmap(iBmp, 0, Codecs.FRAME_WITDH_RSZ,0,0,Codecs.FRAME_WITDH_RSZ,Codecs.FRAME_HEIGHT_RSZ,false,null);
		
		/*_bmp.setPixels(iBmp, 0, Codecs.FRAME_WITDH,0,0,Codecs.FRAME_WITDH,Codecs.FRAME_HEIGHT);
		c.drawBitmap(_bmp, _Matrix, null);*/		
		_holder.unlockCanvasAndPost(c); 
	}
}

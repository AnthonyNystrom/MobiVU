package com.mobivu;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.ViewGroup.LayoutParams;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.AbsoluteLayout;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

public class MobiVu extends Activity implements  Camera.PreviewCallback, SurfaceHolder.Callback, View.OnClickListener, View.OnTouchListener {
	
	private Engine _engine;
	private Camera _Camera;
	private Button _butRejectIncomingCall, _butAccept, _butRejectOutgoingCall, _butH, _butM, _butL;
	private boolean _bPreviewCamera, _bOutgoingShowed, _bIncomingShowed, _bPause;
	private SurfaceView _surfaceCamera;
	private FrameView _imgVideo;	
	private Handler _Handler = new Handler();
	private ImageView _mainButton;
	private String _sNickName, _sIPServer, _sUserCalling, _sToastMessage, _sUserConv;
	int _iCurrentMainBut;
	int []_iBmp;
	private String []_sUsersOnline;
	private TransparentPanel _panelIncoming, _panelOutgoing;
	private TextView _userIncomingCalling, _userOutgoingCalling;
	private Animation _animShow, _animHide;
	private PowerManager.WakeLock _wl;
	private Ringtone _ringtonein=null, _ringtoneout=null;
	
	public static final String LOG_FILE = "/sdcard/mv.log";

	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {    		
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE); 
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN); 
        setContentView(R.layout.main);
                
        _imgVideo     = (FrameView) findViewById(R.id.imgFrame);
        
        _mainButton = (ImageView) findViewById(R.id.imgMainButton);
        _mainButton.setOnClickListener(this);
        _mainButton.setOnTouchListener(this);
        _iCurrentMainBut = R.drawable.greenoffbut;
        
        _surfaceCamera = (SurfaceView)findViewById(R.id.previewCamera);        
        SurfaceHolder surfaceHolder = _surfaceCamera.getHolder();
        surfaceHolder.addCallback(this);
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);                
        _bPreviewCamera=false;
                        
        _bPause   = false;
        _engine   = new Engine();
        
        _bOutgoingShowed = false;
        _panelOutgoing = (TransparentPanel) findViewById(R.id.outgoing_window);
		_panelOutgoing.setVisibility(View.GONE);
		_butRejectOutgoingCall = (Button) findViewById(R.id.rejectioutgoingcall);
		_butRejectOutgoingCall.setOnClickListener(this);
		_butRejectOutgoingCall.setOnTouchListener(this);
		_userOutgoingCalling = (TextView) findViewById(R.id.user_outgoing_call);
		_bIncomingShowed = false;
		_panelIncoming = (TransparentPanel) findViewById(R.id.incoming_window);
		_panelIncoming.setVisibility(View.GONE);
		_butRejectIncomingCall = (Button) findViewById(R.id.rejectincomingcall);
		_butRejectIncomingCall.setOnClickListener(this);
		_butRejectIncomingCall.setOnTouchListener(this);
		_butAccept = (Button) findViewById(R.id.acceptconv);
		_butAccept.setOnClickListener(this);
		_butAccept.setOnTouchListener(this);
		_animShow  = AnimationUtils.loadAnimation( this, R.anim.popup_show);
		_animHide  = AnimationUtils.loadAnimation( this, R.anim.popup_hide);
		_userIncomingCalling = (TextView) findViewById(R.id.user_incoming_call);
		_sUserCalling="";
		
		_butH = (Button) findViewById(R.id.butHighQ);
		_butH.setOnClickListener(this);
		_butM = (Button) findViewById(R.id.butMidQ);
		_butM.setOnClickListener(this);
		_butL = (Button) findViewById(R.id.butLowQ);
		_butL.setOnClickListener(this);
		
		PowerManager pm = (PowerManager) getSystemService(this.POWER_SERVICE);
		_wl = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");
		_wl.acquire();
		
        
        File fo = new File(LOG_FILE);        
        if (fo.exists())
        	fo.delete();
    }
    @Override
    protected void onResume() {
		super.onResume();
		GetSettings();
		if (_sNickName.length()<=0 || _sIPServer.length()<=0)
			ShowToastMessage("Please compile fields settings");			
		else{
			_engine.Init(_sIPServer, _sNickName, this);
			// get ringtone
			String sKey = getResources().getString(R.string.prefringtonein);
			String sUriSipRingtone = PreferenceManager.getDefaultSharedPreferences(this).getString(sKey, "");
			Uri oUriSipRingtone = null;
			if(!TextUtils.isEmpty(sUriSipRingtone))
			{	oUriSipRingtone = Uri.parse(sUriSipRingtone);
				_ringtonein = RingtoneManager.getRingtone(this, oUriSipRingtone);
			}
			sKey = getResources().getString(R.string.prefringtoneout);
			sUriSipRingtone = PreferenceManager.getDefaultSharedPreferences(this).getString(sKey, "");
			oUriSipRingtone = null;
			if(!TextUtils.isEmpty(sUriSipRingtone))
			{	oUriSipRingtone = Uri.parse(sUriSipRingtone);
				_ringtoneout = RingtoneManager.getRingtone(this, oUriSipRingtone);
			}			
		}
    }
	@Override
	protected void onPause() {
		_engine.Release();
		super.onPause();
	}
	@Override
	protected void onDestroy() {
		super.onDestroy();
		_engine.Release();		
		_wl.release();
		if (_ringtoneout!=null)
    		handler.removeCallbacks(RingOut);
		if (_ringtonein!=null)
    		handler.removeCallbacks(RingIn);
	}
	@Override
	public void onPreviewFrame(byte[] byFrame, Camera arg1) {
		if (!_bPause)
			_engine.SendFrameVideo(byFrame);		
	}
	@Override
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		_Camera = Camera.open();
        try {
        	_Camera.setPreviewDisplay(holder);
        	_Camera.setPreviewCallback(this);
    		Camera.Parameters parameters = _Camera.getParameters();
            parameters.setPreviewSize(Codecs.FRAME_WITDH, Codecs.FRAME_HEIGHT);
            //parameters.setPreviewFormat(PixelFormat.YCbCr_420_SP);            
            parameters.setPreviewFrameRate(15);
            parameters.set("orientation", "landscape");
            _Camera.setParameters(parameters);
                    	
        } catch (IOException exception) {
        	_Camera.release();
        	_Camera = null;
        }		
        try {
           _Camera.setPreviewDisplay(holder);
        } catch (IOException exception) {
            _Camera.release();
            _Camera = null;
        }
	}
	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		if (_Camera!=null)
			StartCamera(false);
    	_Camera.release();
        _Camera = null;		
	}
	@Override
	public void onClick(View v) {
		_bPause=true;
		try {Thread.sleep(500);} catch (InterruptedException e) {}
		if (v.getId()==_butL.getId()){
			_engine.ReinitCodecQ(0);			
		}
		else if (v.getId()==_butH.getId()){
			_engine.ReinitCodecQ(2);			
		}
		else if (v.getId()==_butM.getId()){
			_engine.ReinitCodecQ(1);
		}
		_bPause=false;
	}	
	@Override
	public boolean onTouch(View v, MotionEvent event) {
		ImageView img=null;
		Button but=null;
    	int idResUp=0, idResDn=0, idRes=0;
    	if (v.getId()==_mainButton.getId()){
    		img=_mainButton;
    		switch (_iCurrentMainBut){
    		case R.drawable.greenoffbut:
    			idResUp = R.drawable.greenoffbut;
        		idResDn = R.drawable.greenoffbut_p;
    			break;
    		case R.drawable.redbut:
    			idResUp = R.drawable.redbut;
        		idResDn = R.drawable.redbut_p;
    			break;
    		case R.drawable.greenbut:
    			idResUp = R.drawable.greenbut;
        		idResDn = R.drawable.greenbut_p;
    			break;
    		}
    	}
    	else if (v.getId()==_butRejectIncomingCall.getId()){
    		but=_butRejectIncomingCall;
    		idResUp = R.drawable.button_black;
    		idResDn = R.drawable.button_black_p;
    	}
    	else if (v.getId()==_butRejectOutgoingCall.getId()){
    		but=_butRejectOutgoingCall;
    		idResUp = R.drawable.button_black;
    		idResDn = R.drawable.button_black_p;
    	}
    	else if (v.getId()==_butAccept.getId()){
    		but=_butAccept;
    		idResUp = R.drawable.button_black;
    		idResDn = R.drawable.button_black_p;
    	}
        
        switch (event.getAction()) { 
	        case MotionEvent.ACTION_DOWN:
	        	idRes = idResDn;
	        	break;
	        case MotionEvent.ACTION_UP:
	        	idRes = idResUp;
	        	if (v.getId()==_mainButton.getId()){
	        		switch (_iCurrentMainBut){
	        		case R.drawable.greenoffbut:
	        			ShowToastMessage("MobiVu is not logged. Please wait green button");
	        			break;
	        		case R.drawable.redbut:
	        			_engine.CloseConversation();
	        			break;
	        		case R.drawable.greenbut:
	        			_engine.GetUsersOnline();
	        			break;
	        		}
	        	}
	        	else if (v.getId()==_butAccept.getId()){
	    			//StartCamera(true);
	        		MyShowIncomingCall(false);
	        		_engine.AcceptConveration();
	    		}
	    		else if (v.getId()==_butRejectIncomingCall.getId()){
	    			_engine.RejectIncomingCall();
	    		}
	    		else if (v.getId()==_butRejectOutgoingCall.getId()){
	    			_engine.RejectOutgoingCall();	    			
	    		}
	        	break;
	        default:
	        	return true;
        }
        if (img!=null && idRes>=0)
        	img.setImageResource(idRes);
        if (but!=null && idRes>=0)
        	but.setBackgroundResource(idRes); 
        return true; 
	}
	
 
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add("Settings");
		return super.onCreateOptionsMenu(menu);
	}
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		ShowSettings();
		return super.onOptionsItemSelected(item);
	}
	void GetSettings(){
		// check settings data
		Resources res = getResources();
		SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
		_sNickName = sharedPref.getString(res.getString(R.string.prefNikName), "");
		_sIPServer = sharedPref.getString(res.getString(R.string.prefIPServer), "");
	}
	void ShowSettings(){
		Intent intent = new Intent(MobiVu.this, Settings.class);
		startActivity(intent);
	}
	
	private void StartCamera(boolean bEnable){
		
		if (bEnable && !_bPreviewCamera){
			_bPreviewCamera=true;
			LayoutParams params = _surfaceCamera.getLayoutParams();
    		params.height= 140;
    		params.width = 170;
    		_surfaceCamera.setLayoutParams(params);			
    		RelativeLayout lay = (RelativeLayout)findViewById(R.id.bklay);
			lay.setBackgroundResource(R.drawable.bkconv);
    		_bPreviewCamera=true;
			_Camera.startPreview();
			_imgVideo.setVisibility(View.VISIBLE);
			TextView txtusr = (TextView)findViewById(R.id.txtUser);
			txtusr.setText(_sUserConv);
		}
		else if (!bEnable && _bPreviewCamera){			
			_bPreviewCamera=false;			
			RelativeLayout lay = (RelativeLayout)findViewById(R.id.bklay);
			lay.setBackgroundResource(R.drawable.bk);
			LayoutParams params = _surfaceCamera.getLayoutParams();
    		params.height= 0;
    		params.width = 0;
    		_surfaceCamera.setLayoutParams(params);    		
    		_imgVideo.setVisibility(View.GONE);
			_Camera.stopPreview();
			_sUserConv="";
			TextView txtusr = (TextView)findViewById(R.id.txtUser);
			txtusr.setText(_sUserConv);
		}
	}
	private static final int HANDLE_UPDATE_FRAME 		= 1;
	private static final int HANDLE_SHOW_BUTTON  		= 2;
	private static final int HANDLE_SHOW_TOAST_MSG    	= 3;
	private static final int HANDLE_SHOW_USERS_ONLINE  	= 4;
	private static final int HANDLE_SHOW_WIN_OUTGOING  	= 5;
	private static final int HANDLE_CLOSE_WIN_OUTGOING  = 6;
	private static final int HANDLE_SHOW_WIN_INCOMING   = 7;
	private static final int HANDLE_CLOSE_WIN_INCOMING  = 8;
	private static final int HANDLE_START_CAMERA        = 9;
	private static final int HANDLE_STOP_CAMERA         = 10;
	
	
	private Handler handler = new Handler() {
		public void handleMessage(Message msg) {			
			switch (msg.what)
			{
			case HANDLE_START_CAMERA:
				StartCamera(true);
				break;
			case HANDLE_STOP_CAMERA:
				StartCamera(false);
				break;
			case HANDLE_SHOW_WIN_INCOMING:
				MyShowIncomingCall(true);
				break;
			case HANDLE_CLOSE_WIN_INCOMING:
				MyShowIncomingCall(false);
				break;
			case HANDLE_SHOW_USERS_ONLINE:
				showDialog(DIALOG_LIST_USERS);
				break;
			case HANDLE_SHOW_TOAST_MSG:
				Toast.makeText(MobiVu.this, _sToastMessage, Toast.LENGTH_SHORT).show();
				break;
			case HANDLE_SHOW_BUTTON:				
				_mainButton.setImageResource(_iCurrentMainBut);
				break;
			case HANDLE_UPDATE_FRAME:
				if (_bPreviewCamera)
					_imgVideo.DrawFrame(_iBmp);
				break;
			case HANDLE_SHOW_WIN_OUTGOING:
				MyShowOutgoingCall(true);
				break;
			case HANDLE_CLOSE_WIN_OUTGOING:
				MyShowOutgoingCall(false);
				break;
			}
		}
	};
	public void ShowFrame(int[] iRGB) {
		_iBmp = iRGB;
		handler.sendEmptyMessage(HANDLE_UPDATE_FRAME);
	}
	
	public static void MyLog(String sLog){
		File fo = new File(LOG_FILE); 	
		try {
			FileOutputStream out = new FileOutputStream(fo, true);
			try {
				sLog+="\n";
				out.write(sLog.getBytes());
				out.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		} catch (FileNotFoundException e) {			
			e.printStackTrace();
		}
	}
	
	public void ShowToastMessage(String sMsg){
		_sToastMessage = sMsg;
		handler.sendEmptyMessage(HANDLE_SHOW_TOAST_MSG);
	}
	
	public void SetStatus(int iStatus, String sData){
		switch (iStatus){
		case Engine.STATUS_CONVERSATION:
			if (_iCurrentMainBut != R.drawable.redbut){
				_iCurrentMainBut = R.drawable.redbut;				
				handler.sendEmptyMessage(HANDLE_SHOW_BUTTON);
			}
			break;
		case Engine.STATUS_ACCEPT_INCOMING_CALL:
			ShowOutgoingCall(false);
			break;
		case Engine.STATUS_BINDING:
			_sUserConv = sData;
			handler.sendEmptyMessage(HANDLE_START_CAMERA);
			break;
		case Engine.STATUS_CLOSE_CONVERSATION:
			_sUserConv = "";
			handler.sendEmptyMessage(HANDLE_STOP_CAMERA);
			break;
		case Engine.STATUS_REJECT_INCOMING_CALL:			
			ShowIncomingCall(false);
			break;
		case Engine.STATUS_REJECT_OUTGOING_CALL:			
			ShowOutgoingCall(false);
			break;
		case Engine.STATUS_RINGING:
			_sUserCalling = sData;
			ShowIncomingCall(true);
			break;
		case Engine.STATUS_CALLING:
		case Engine.STATUS_WAIT_YESNO:			
			ShowOutgoingCall(true);
			break;
		case Engine.STATUS_LOGOUT:
		case Engine.STATUS_DISCONNECTED:
			if (_iCurrentMainBut != R.drawable.greenoffbut){
				_iCurrentMainBut = R.drawable.greenoffbut;
				handler.sendEmptyMessage(HANDLE_SHOW_BUTTON);
			}
			break;
		case Engine.STATUS_IDLE:
			if (_iCurrentMainBut != R.drawable.greenbut){
				_iCurrentMainBut = R.drawable.greenbut;
				handler.sendEmptyMessage(HANDLE_SHOW_BUTTON);
			}
			break;
		case Engine.STATUS_USERLIST:
			if (sData.length()<=0){
				ShowToastMessage("NOBODY IS ONLINE");						
			}
			else{
				sData = sData.replace("|", "/");
				sData = sData.replace(_sNickName, "");
				sData = sData.replace("//", "/");
				if (sData.length()>1 && sData.charAt(0)=='/')
					sData = sData.substring(1);
				if (sData.length()>0){
					_sUsersOnline = sData.split("/");								
					handler.sendEmptyMessage(HANDLE_SHOW_USERS_ONLINE);
				}
				else
					ShowToastMessage("NOBODY IS ONLINE");				
			}	
			break;
		}
	}
	private static final int DIALOG_LIST_USERS = 1;
	protected void onPrepareDialog(int id, Dialog d){ 
		super.onPrepareDialog(id, d);
		switch (id) {
        case DIALOG_LIST_USERS:
        	break;
		}
	} 
	@Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
        case DIALOG_LIST_USERS:
        	return new AlertDialog.Builder(MobiVu.this)
            .setTitle(R.string.users_online)
            .setItems(_sUsersOnline, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                	_sUserCalling = _sUsersOnline[which];
                	_engine.Call(_sUsersOnline[which]);
                	removeDialog(DIALOG_LIST_USERS);
                }
            })
            .setOnCancelListener(new DialogInterface.OnCancelListener(){
            	public void onCancel (DialogInterface dialog){            		
            		removeDialog(DIALOG_LIST_USERS);
            	}
            })
            .create();
        }
        return null;
	}
	public void ShowOutgoingCall(boolean bShow)
	{	if (bShow)
			handler.sendEmptyMessage(HANDLE_SHOW_WIN_OUTGOING);
		else
			handler.sendEmptyMessage(HANDLE_CLOSE_WIN_OUTGOING);
	}
	public void ShowIncomingCall(boolean bShow)
	{	if (bShow)
			handler.sendEmptyMessage(HANDLE_SHOW_WIN_INCOMING);
		else
			handler.sendEmptyMessage(HANDLE_CLOSE_WIN_INCOMING);
	}
	void MyShowIncomingCall(boolean bShow){
		if (_bPreviewCamera)
			return;
        
		if (!bShow && _bIncomingShowed){
			if (_ringtonein!=null){
				handler.removeCallbacks(RingIn);
				_ringtonein.stop();
			}
			_bIncomingShowed = bShow;
			_panelIncoming.startAnimation( _animHide );
			_butRejectIncomingCall.setEnabled(false);
			_butAccept.setEnabled(false);
			_panelIncoming.setVisibility(View.GONE);
			_mainButton.setEnabled(true);
			
			PowerManager pm = (PowerManager) getSystemService(this.POWER_SERVICE);
			_wl.release();			
			_wl = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK|PowerManager.ACQUIRE_CAUSES_WAKEUP, "My Tag");
			_wl.acquire();
		}
		else if (bShow && !_bIncomingShowed){
			if (_ringtonein!=null)
				RingIn.run();
			_bIncomingShowed = bShow;
			_userIncomingCalling.setText(_sUserCalling);
			_panelIncoming.setVisibility(View.VISIBLE);
			_panelIncoming.startAnimation( _animShow );
			_butAccept.setEnabled(true);
			_butRejectIncomingCall.setEnabled(true);
			_mainButton.setEnabled(false);
			
			PowerManager pm = (PowerManager) getSystemService(this.POWER_SERVICE);
			_wl.release();						
			_wl = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK|PowerManager.ACQUIRE_CAUSES_WAKEUP, "My Tag");
			_wl.acquire();
		}
	}
	private void MyShowOutgoingCall(boolean bShow){
		if (!bShow && _bOutgoingShowed){
			if (_ringtoneout!=null){
				handler.removeCallbacks(RingOut);
				_ringtoneout.stop();
			}
			_bOutgoingShowed=bShow;
			_panelOutgoing.startAnimation( _animHide );
			_panelOutgoing.setVisibility(View.GONE);
			_mainButton.setEnabled(true);
			handler.sendEmptyMessage(HANDLE_SHOW_BUTTON);
			
			PowerManager pm = (PowerManager) getSystemService(this.POWER_SERVICE);
			_wl.release();			
			_wl = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK|PowerManager.ACQUIRE_CAUSES_WAKEUP, "My Tag");
			_wl.acquire();
		}
		else if (bShow && !_bOutgoingShowed){
			if (_ringtoneout!=null)
				RingOut.run();
			_bOutgoingShowed=bShow;
			_userOutgoingCalling.setText(_sUserCalling);
			_panelOutgoing.setVisibility(View.VISIBLE);
			_panelOutgoing.startAnimation( _animShow );			
			_mainButton.setEnabled(false);
			handler.sendEmptyMessage(HANDLE_SHOW_BUTTON);
			
			PowerManager pm = (PowerManager) getSystemService(this.POWER_SERVICE);
			_wl.release();						
			_wl = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK|PowerManager.ACQUIRE_CAUSES_WAKEUP, "My Tag");
			_wl.acquire();
		}		
	}
	private Runnable RingOut = new Runnable() {
        public void run() {
        	if (_ringtoneout!=null){
        		if (!_ringtoneout.isPlaying())
        			_ringtoneout.play();
        		// Force toggle again in a second
        		handler.postDelayed(this, 1000);
        	}
        }
    };
    private Runnable RingIn = new Runnable() {
        public void run() {
        	if (_ringtonein!=null){
        		if (!_ringtonein.isPlaying())
        			_ringtonein.play();
        		handler.postDelayed(this, 1000);
        	}        	
        }
    };
}
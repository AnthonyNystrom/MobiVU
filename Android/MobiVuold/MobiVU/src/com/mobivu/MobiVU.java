package com.mobivu;


import com.mobivu.widget.TransparentPanel;
import java.io.IOException;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.preference.ListPreference;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;

public class MobiVU extends Activity implements  Camera.PreviewCallback, SurfaceHolder.Callback, View.OnFocusChangeListener, View.OnTouchListener, View.OnClickListener, EventProtocol{
    
	private static final int DIALOG_LIST_USERS = 1;
	
	private Engine _Engine;
	private ImageView _imgSettings, _imgRedButton, _imgGreenButton, _imgGreenOffButton;
	private String _sNickName, _sUserCalling, _sIPServer, _sToastMessage, _sDebugInfo;
	private Handler _Handler = new Handler();
	private Camera _Camera;
	private boolean _bPreviewCamera, _bIncomingShowed, _bOutgoingShowed;
	private Bitmap _bmpFrame=null;
	private SurfaceView _surfaceView;
	private String []_sUsersOnline;
	private ImageView _imgVideo;
	private Ringtone _ringtonein=null, _ringtoneout=null;
	private Animation _animShow, _animHide;
	private TransparentPanel _panelIncoming, _panelOutgoing;
	private Button _butRejectIncomingCall, _butAccept, _butRejectOutgoingCall;
	private TextView _userIncomingCalling, _userOutgoingCalling;		
	private PowerManager.WakeLock _wl;
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.main);
        
        _imgSettings  = (ImageView) findViewById(R.id.imgsettings);
        _imgSettings.setOnTouchListener(this);
        _imgSettings.setOnClickListener(this);
        _imgSettings.setOnFocusChangeListener(this);
        _imgRedButton  = (ImageView) findViewById(R.id.imgredbut);
        _imgRedButton.setOnTouchListener(this);
        _imgRedButton.setOnClickListener(this);
        _imgRedButton.setOnFocusChangeListener(this);
        _imgGreenButton  = (ImageView) findViewById(R.id.imgreenbut);
        _imgGreenButton.setOnTouchListener(this);
        _imgGreenButton.setOnClickListener(this);
        _imgGreenButton.setOnFocusChangeListener(this);
        _imgGreenOffButton  = (ImageView) findViewById(R.id.imgreenoffbut);
        _imgGreenOffButton.setOnTouchListener(this);
        _imgGreenOffButton.setOnClickListener(this);
        _imgGreenOffButton.setOnFocusChangeListener(this);
       
        _imgVideo =(ImageView) findViewById(R.id.imgframe);
        
        _surfaceView = (SurfaceView)findViewById(R.id.surface);
        SurfaceHolder surfaceHolder = _surfaceView.getHolder();
        surfaceHolder.addCallback(this);
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);        
        _bPreviewCamera=false;
        
        _Engine = new Engine(this);
        
        GetSettings();
		if (_sNickName.length()<=0 || _sIPServer.length()<=0)
			ShowSettings();
		_bmpFrame = Bitmap.createBitmap(Codecs.FRAME_WITDH, Codecs.FRAME_HEIGHT, Bitmap.Config.ARGB_8888);

		// Setting popup notification incoming outgoing call
		_panelIncoming = (TransparentPanel) findViewById(R.id.incoming_window);
		_panelIncoming.setVisibility(View.GONE);
		_animShow  = AnimationUtils.loadAnimation( this, R.anim.popup_show);
		_animHide  = AnimationUtils.loadAnimation( this, R.anim.popup_hide);
		_butRejectIncomingCall = (Button) findViewById(R.id.rejectincomingcall);
		_butRejectIncomingCall.setOnClickListener(this);
		_butAccept = (Button) findViewById(R.id.acceptconv);
		_butAccept.setOnClickListener(this);
		_userIncomingCalling = (TextView) findViewById(R.id.user_incoming_call);
		_bIncomingShowed=false;
		
		_panelOutgoing = (TransparentPanel) findViewById(R.id.outgoing_window);
		_panelOutgoing.setVisibility(View.GONE);
		_butRejectOutgoingCall = (Button) findViewById(R.id.rejectioutgoingcall);
		_butRejectOutgoingCall.setOnClickListener(this);
		_userOutgoingCalling = (TextView) findViewById(R.id.user_outgoing_call);
		_bOutgoingShowed=false;
		
		// power managment 
		PowerManager pm = (PowerManager) getSystemService(this.POWER_SERVICE);
		_wl = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");		
		_wl.acquire();
    }
    @Override
    protected void onDestroy() {
    	super.onDestroy();    	
    	_wl.release();
    	_Engine.Release();
    	_bmpFrame = null;
    	if (_ringtoneout!=null)
    		handler.removeCallbacks(RingOut);
    	//_Camera.release();
    }
	@Override
    protected void onResume() {
		super.onResume();
		GetSettings();
		if (_sNickName.length()<=0 || _sIPServer.length()<=0)
			Toast.makeText(this, "Please compile fields settings", Toast.LENGTH_SHORT).show();
		else
			_Engine.Init(_sNickName, _sIPServer);
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
	 @Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
        switch (keyCode) {        	
        case KeyEvent.KEYCODE_CALL:
        	StartPreview(true);
        	return true;
        case KeyEvent.KEYCODE_BACK:
        	StartPreview(false);        	
        }
        return super.onKeyDown(keyCode, event);
    }
	
	public void onFocusChange(View v, boolean hasFocus) {
    	ImageView img=null;
    	int idResF=0, idResUF=0;
		
		if (v.getId()==_imgSettings.getId()){
    		img=_imgSettings;
    		idResF = R.drawable.graybut;
    		idResUF = R.drawable.graybut;
    	}
		if (img!=null)
        {	if (hasFocus)
        		img.setImageResource(idResF);
        	else
        		img.setImageResource(idResUF);
        }
		
	}
	public boolean onTouch(View v, MotionEvent event) {
		ImageView img=null;
    	int idResUp=0, idResDn=0, idRes=0;
    	if (v.getId()==_imgSettings.getId()){
    		img=_imgSettings;
    		idResUp = R.drawable.graybut;
    		idResDn = R.drawable.graybut;
    	}
    	else if (v.getId()==_imgGreenButton.getId()){
    		img=_imgGreenButton;
    		idResUp = R.drawable.greenbut;
    		idResDn = R.drawable.greenbut;
    	}
    	else if (v.getId()==_imgRedButton.getId()){
    		img=_imgRedButton;
    		idResUp = R.drawable.redbut;
    		idResDn = R.drawable.redbut;
    	}
    	
        switch (event.getAction()) { 
	        case MotionEvent.ACTION_DOWN:
	        	idRes = idResDn;
	        	break;
	        case MotionEvent.ACTION_UP:
	        	idRes = idResUp;
	        	if (v.getId()==_imgSettings.getId())
	        		ShowSettings();
	        	else if (v.getId()==_imgGreenOffButton.getId())
	        		Toast.makeText(this, "MobiVU is not logged. Wait please green button", Toast.LENGTH_SHORT).show();
	        	else if (v.getId()==_imgGreenButton.getId())
	        		_Engine.GetUserList();
	        	else if (v.getId()==_imgRedButton.getId()){
	        		StartPreview(false);
	    			_Engine.CloseConversation();	        	
	    		}        		
	        	break;
	        default:
	        	return true;
        }
        if (img!=null && idRes>=0)
        	img.setImageResource(idRes);
         
        return true; 
	}
	
	public void onClick(View v) {
		if (v.getId()==_imgSettings.getId())
    		ShowSettings();
		else if (v.getId()==_imgGreenButton.getId())
			_Engine.GetUserList();
		else if (v.getId()==_imgRedButton.getId()){
			StartPreview(false);
			_Engine.CloseConversation();
		}
		else if (v.getId()==_butAccept.getId()){
			_Engine.Accept();
			ShowIncomingCall(false);
		}
		else if (v.getId()==_butRejectIncomingCall.getId()){
			_Engine.Reject();
			ShowIncomingCall(false);
		}
		else if (v.getId()==_butRejectOutgoingCall.getId()){
			_Engine.Reject();
			ShowOutgoingCall(false);			
		}
	}
	
	void ShowOutgoingCall(boolean bShow){
		
		if (!bShow && _bOutgoingShowed){
			if (_ringtoneout!=null)
				handler.removeCallbacks(RingOut);
			_bOutgoingShowed=bShow;
			_panelOutgoing.startAnimation( _animHide );
			_butRejectOutgoingCall.setEnabled(false);
			_panelOutgoing.setVisibility(View.GONE);
			_imgGreenButton.setEnabled(!bShow);
			_imgRedButton.setEnabled(!bShow);
			_imgSettings.setEnabled(!bShow);
		}
		else if (bShow && !_bOutgoingShowed){
			if (_ringtoneout!=null)
				RingOut.run();
			_bOutgoingShowed=bShow;
			_userOutgoingCalling.setText(_sUserCalling);
			_panelOutgoing.setVisibility(View.VISIBLE);
			_panelOutgoing.startAnimation( _animShow );
			_butRejectOutgoingCall.setEnabled(true);
			_imgGreenButton.setEnabled(!bShow);
			_imgRedButton.setEnabled(!bShow);
			_imgSettings.setEnabled(!bShow);
		}
	}
	
	void ShowIncomingCall(boolean bShow){
		if (_bPreviewCamera)
			return;
        
		if (!bShow && _bIncomingShowed){
			if (_ringtonein!=null)
				_ringtonein.stop();
			_bIncomingShowed = bShow;
			_panelIncoming.startAnimation( _animHide );
			_butRejectIncomingCall.setEnabled(false);
			_butAccept.setEnabled(false);
			_panelIncoming.setVisibility(View.GONE);
			_imgGreenButton.setEnabled(!bShow);
			_imgRedButton.setEnabled(!bShow);
			_imgSettings.setEnabled(!bShow);
		}
		else if (bShow && !_bIncomingShowed){
			_bIncomingShowed = bShow;
			_userIncomingCalling.setText(_sUserCalling);
			_panelIncoming.setVisibility(View.VISIBLE);
			_panelIncoming.startAnimation( _animShow );
			_butAccept.setEnabled(true);
			_butRejectIncomingCall.setEnabled(true);
			_imgGreenButton.setEnabled(!bShow);
			_imgRedButton.setEnabled(!bShow);
			_imgSettings.setEnabled(!bShow);			
		}
	}
	
	void GetSettings(){
		// check settings data
		Resources res = getResources();
		SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
		_sNickName = sharedPref.getString(res.getString(R.string.prefNikName), "");
		_sIPServer = sharedPref.getString(res.getString(R.string.prefIPServer), "");
	}
	void ShowSettings(){
		Intent intent = new Intent(MobiVU.this, Settings.class);
		startActivity(intent);
	}
	
		
	public void Logged(boolean bLogged) {
		Message msg = new Message();
		if (bLogged)
			msg.arg1 = 1;
		else
			msg.arg1 = 0;
		msg.what = HANDLE_SHOW_LOGGED;
		handler.sendMessage(msg);
	}
	
	private static final int HANDLE_SHOW_LOGGED      	 = 0;
	private static final int HANDLE_SHOW_USERS_ONLINE  	 = 1;
	private static final int HANDLE_ENABLECAMERA  		 = 2;
	private static final int HANDLE_UPDATE_FRAME         = 3;
	private static final int HANDLE_SHOW_INCOMING        = 4;
	private static final int HANDLE_SHOW_TOAST_MSG       = 5;
	private static final int HANDLE_CLOSE_OUTGOING       = 6;
	private static final int HANDLE_CLOSE_INCOMING       = 7;
	private static final int HANDLE_SHOW_DEBUG_INFO      = 8;
		
	private Handler handler = new Handler() {
		public void handleMessage(Message msg) {			
			switch (msg.what)
			{
			case HANDLE_SHOW_DEBUG_INFO:
				TextView txt = (TextView)findViewById(R.id.txtSignalMax);
				txt.setText(_sDebugInfo);			
				break;
			case HANDLE_CLOSE_INCOMING:
				ShowIncomingCall(false);
				break;
			case HANDLE_CLOSE_OUTGOING:
				ShowOutgoingCall(false);
				break;
			case HANDLE_SHOW_TOAST_MSG:
				Toast.makeText(MobiVU.this, _sToastMessage, Toast.LENGTH_SHORT).show();
				break;
			case HANDLE_SHOW_INCOMING:
				ShowIncomingCall(true);
				break;
			case HANDLE_UPDATE_FRAME:
				_imgVideo.setImageBitmap(_bmpFrame);
				break;
			case HANDLE_ENABLECAMERA:
				if (msg.arg1==1)
					StartPreview(true);
				else
					StartPreview(false);
				break;
			case HANDLE_SHOW_USERS_ONLINE:
				showDialog(DIALOG_LIST_USERS);
				break;
			case HANDLE_SHOW_LOGGED:
				if (msg.arg1==1){
					_imgGreenButton.setVisibility(View.VISIBLE);
					_imgGreenOffButton.setVisibility(View.INVISIBLE);
					_imgRedButton.setVisibility(View.INVISIBLE);
				}
				else{
					_imgGreenOffButton.setVisibility(View.VISIBLE);
					_imgGreenButton.setVisibility(View.INVISIBLE);
					_imgRedButton.setVisibility(View.INVISIBLE);
				}				
				break;
			}
			super.handleMessage(msg);
		}
	};

	public void onPreviewFrame(byte[] byYUV, Camera arg1) {
		// check mode setting video enabled
		if (!IsVideoEnabled())
			return;
		_Engine.FrameVideoToEncode(byYUV);
	}
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
	}
	public void surfaceCreated(SurfaceHolder holder) {
		_Camera = Camera.open();
        try {
        	_Camera.setPreviewDisplay(holder);
    		Camera.Parameters parameters = _Camera.getParameters();
            parameters.setPreviewSize(Codecs.FRAME_WITDH, Codecs.FRAME_HEIGHT);
            parameters.setPreviewFormat(PixelFormat.YCbCr_420_SP);            
            parameters.setPreviewFrameRate(15);
            parameters.set("orientation", "landscape");
            _Camera.setParameters(parameters);
            _Camera.setPreviewCallback(this);        	
        } catch (IOException exception) {
        	_Camera.release();
        	_Camera = null;
            // TODO: add more exception handling logic here
        }		
        try {
           _Camera.setPreviewDisplay(holder);
        } catch (IOException exception) {
            _Camera.release();
            _Camera = null;
            // TODO: add more exception handling logic here
        }
	}
	public void surfaceDestroyed(SurfaceHolder arg0) {
    	if (_Camera!=null)
    		StartPreview(false);
    	_Camera.release();
        _Camera = null;
	}
	private void StartPreview(boolean bStart){
		
		LinearLayout lay = (LinearLayout)findViewById(R.id.mainLayout);
    	if (bStart && !_bPreviewCamera){
    		_bPreviewCamera=true;
    		_imgSettings.setEnabled(!bStart);
    		_imgRedButton.setVisibility(View.VISIBLE);
    		_imgGreenButton.setVisibility(View.INVISIBLE);    		
    		LayoutParams params = _surfaceView.getLayoutParams();
    		params.height= 70;
    		params.width = 88;
    		_surfaceView.setLayoutParams(params);
    		_imgVideo.setVisibility(View.VISIBLE);
    		lay.setBackgroundDrawable(getResources().getDrawable(R.drawable.backconv));
    		_Camera.startPreview();    		
    	}
    	else if (!bStart && _bPreviewCamera){
    		_bPreviewCamera=false;
    		_imgSettings.setEnabled(!bStart);
    		_imgRedButton.setVisibility(View.INVISIBLE);
    		_imgGreenButton.setVisibility(View.VISIBLE);    		
    		_Camera.stopPreview();    		
    		LayoutParams params = _surfaceView.getLayoutParams();
    		params.height= 0;
    		params.width = 0;
    		_surfaceView.setLayoutParams(params);
    		_imgVideo.setVisibility(View.INVISIBLE);
    		lay.setBackgroundDrawable(getResources().getDrawable(R.drawable.backfox));
    	}
    }
	public void UsersList(String[] sUsers) {
		if (sUsers==null){
			_sToastMessage = "NOBODY IS ONLINE";
			handler.sendEmptyMessage(HANDLE_SHOW_TOAST_MSG);			
		}
		else{
			_sUsersOnline = sUsers;
			handler.sendEmptyMessage(HANDLE_SHOW_USERS_ONLINE);
		}
	}
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
        	return new AlertDialog.Builder(MobiVU.this)
            .setTitle(R.string.users_online)
            .setItems(_sUsersOnline, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                	_sUserCalling = _sUsersOnline[which];
                	ShowOutgoingCall(true);                	
                	_Engine.MakeCall(_sUsersOnline[which]);
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
	public void EnableCamera(boolean bEnable) {
		Message msg = new Message();
		if (bEnable)
			msg.arg1 = 1;
		else
			msg.arg1 = 0;
		msg.what = HANDLE_ENABLECAMERA;
		handler.sendMessage(msg);			
	}
	public void IncomingCall(String sUser) {
		if (_ringtonein!=null){
			if (!_ringtonein.isPlaying())
				_ringtonein.play();
		}
		_sUserCalling = sUser;
		handler.sendEmptyMessage(HANDLE_SHOW_INCOMING);
	}
	public void ShowFrame(int[] iRGB) {
		_bmpFrame.setPixels(iRGB, 0, Codecs.FRAME_WITDH, 0, 0, Codecs.FRAME_WITDH, Codecs.FRAME_HEIGHT);		
		handler.sendEmptyMessage(HANDLE_UPDATE_FRAME);
		
	}
	@Override
	public boolean IsAudioEnabled() {
		// check setting mode
		String sKey = getResources().getString(R.string.prefMode);
		String sValMode = PreferenceManager.getDefaultSharedPreferences(this).getString(sKey, "");        
        int iMode = Integer.parseInt(sValMode);
        if (iMode==1) // audio mode only ?
        	return false;
        return true;
	}
	@Override
	public boolean IsVideoEnabled() {
		// check setting mode
		String sKey = getResources().getString(R.string.prefMode);
		String sValMode = PreferenceManager.getDefaultSharedPreferences(this).getString(sKey, "");        
        int iMode = Integer.parseInt(sValMode);
        if (iMode==2) // audio mode only ?
        	return false;
		return true;
	}
	@Override
	public void ConversationStarted() {
		if (_bOutgoingShowed)
			handler.sendEmptyMessage(HANDLE_CLOSE_OUTGOING);		
	}
	@Override
	public void ConversationStopped() {
		try
		{	if (_bOutgoingShowed){			
				handler.sendEmptyMessage(HANDLE_CLOSE_OUTGOING);
				_sToastMessage = "CALL REJECTED";
				handler.sendEmptyMessage(HANDLE_SHOW_TOAST_MSG);							
			}
			if (_bIncomingShowed){			
				handler.sendEmptyMessage(HANDLE_CLOSE_INCOMING);
				_sToastMessage = "CALL REJECTED";
				handler.sendEmptyMessage(HANDLE_SHOW_TOAST_MSG);							
			}
		}
		catch(Exception ex){
			
		}
				
	}
	private Runnable RingOut = new Runnable() {
        public void run() {
        	if (_ringtoneout!=null){
        		if (!_ringtoneout.isPlaying())
        			_ringtoneout.play();
        		// Force toggle again in a second
        		handler.postDelayed(this, 2000);
        	}
        }
    };

	@Override
	public void RejectedFromHost() {
		if (_bOutgoingShowed)
			handler.sendEmptyMessage(HANDLE_CLOSE_OUTGOING);
		else if (_bIncomingShowed)
			handler.sendEmptyMessage(HANDLE_CLOSE_INCOMING);
		_sToastMessage = "CALL REJECTED";
		handler.sendEmptyMessage(HANDLE_SHOW_TOAST_MSG);									
	}
	@Override
	public void SignalMax(short iValMax) {
		_sDebugInfo = "Signal:"+iValMax;
		handler.sendEmptyMessage(HANDLE_SHOW_DEBUG_INFO);
	}
}
	
	
	

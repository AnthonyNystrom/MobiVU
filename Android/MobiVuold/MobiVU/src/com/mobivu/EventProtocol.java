package com.mobivu;

public interface EventProtocol {
	public abstract void Logged(boolean bLogged);
	public abstract void UsersList(String []sUsers);
	public abstract void EnableCamera(boolean bEnable);
	public abstract void IncomingCall(String sUser);
	public abstract void ShowFrame(int []iRGB);
	public abstract boolean IsVideoEnabled();
	public abstract boolean IsAudioEnabled();
	public abstract void RejectedFromHost();
	public abstract void ConversationStarted();
	public abstract void ConversationStopped();
	public abstract void SignalMax(short iValMax);
}

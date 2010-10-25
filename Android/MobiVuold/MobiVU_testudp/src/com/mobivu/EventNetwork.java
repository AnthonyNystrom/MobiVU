package com.mobivu;

public interface EventNetwork {
	public abstract void Connected(boolean bConnected);
	public abstract void CmdResult(int iCmd, byte[]byData, int iLenData, int iResult);
}

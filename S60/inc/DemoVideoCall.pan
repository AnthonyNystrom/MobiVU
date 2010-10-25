/*
 ============================================================================
 Name		: DemoVideoCall.pan
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __DEMOVIDEOCALL_PAN__
#define __DEMOVIDEOCALL_PAN__

/** DemoVideoCall application panic codes */
enum TDemoVideoCallPanics
	{
	EDemoVideoCallUi = 1
	// add further panics here
	};

inline void Panic(TDemoVideoCallPanics aReason)
	{
	_LIT(applicationName, "DemoVideoCall");
	User::Panic(applicationName, aReason);
	}

#endif // __DEMOVIDEOCALL_PAN__

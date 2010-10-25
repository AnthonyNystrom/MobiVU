/*
 ============================================================================
 Name		: DemoVideoCall.cpp
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "DemoVideoCallApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new CDemoVideoCallApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}


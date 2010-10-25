/*
 * Copyright © 2008 Nokia Corporation.
 */

#ifndef __AUDIOSTREAM_PAN__
#define __AUDIOSTREAM_PAN__

#include <e32std.h>
#include "mainwindow.h"
inline void Panic(TInt aPanicCode)
    {
    _LIT(KApplicationName, "AudioStream");
    User::Panic(KApplicationName, aPanicCode);
    }

inline void PanicIfError(TInt aPanicCode)
    {
    if (aPanicCode != KErrNone)
        {        
            iAppUi->LogInfo("PanicIfError:%d", aPanicCode);
        Panic(aPanicCode);
        }
    }

#endif // __AUDIOSTREAM_PAN__


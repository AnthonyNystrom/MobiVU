

#include "TimeOutTimer.h"
#include "TimeOutNotifier.h"



CTimeOutTimer* CTimeOutTimer::NewL(const TInt aPriority, MTimeOutNotifier& aTimeOutNotify)
    {
	CTimeOutTimer* self = CTimeOutTimer::NewLC(aPriority, aTimeOutNotify);
	CleanupStack::Pop(self);
	return self;
    }

	
CTimeOutTimer* CTimeOutTimer::NewLC(const TInt aPriority, MTimeOutNotifier& aTimeOutNotify)
    {
	CTimeOutTimer* self = new (ELeave) CTimeOutTimer(aPriority, aTimeOutNotify);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }


CTimeOutTimer::CTimeOutTimer(const TInt aPriority, MTimeOutNotifier& aTimeOutNotify)
: CTimer(aPriority), iNotify(aTimeOutNotify)
    {
    }


CTimeOutTimer::~CTimeOutTimer()
    {
    }


void CTimeOutTimer::ConstructL()
    {
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
    }

void CTimeOutTimer::RunL()
    {
    _LIT(KPanicSockets, "Sockets");
    // Timer request has completed, so notify the timer's owner
    if (iStatus == KErrNone)
        {
	    iNotify.TimerExpired();
        }
    else
        {
        User::Panic(KPanicSockets, ESocketsBadStatus);
        }
	}




#ifndef __TIMEOUTTIMER_H__
#define __TIMEOUTTIMER_H__

#include <e32base.h>
#include "TimeOutNotifier.h"

class MTimeOutNotifier;

/*! 
  @class CTimeOutTimer
  
  @discussion This class will notify an object after a specified timeout.
  */
class CTimeOutTimer : public CTimer
    {
public:
/*!
  @function NewL
   
  @discussion Create a CTimeOutTimer object
  @param aPriority priority to use for this timer
  @param aTimeOutNotify object to notify of timeout event
  @result a pointer to the created instance of CTimeOutTimer
  */
	static CTimeOutTimer* NewL(const TInt aPriority, MTimeOutNotifier& aTimeOutNotify);

/*!
  @function NewL
   
  @discussion Create a CTimeOutTimer object
  @param aPriority priority to use for this timer
  @param aTimeOutNotify object to notify of timeout event
  @result a pointer to the created instance of CTimeOutTimer
  */
    static CTimeOutTimer* NewLC(const TInt aPriority, MTimeOutNotifier& aTimeOutNotify);

/*!
  @function ~CTimeOutTimer
  
  @discussion Destroy the object and release all memory objects
  */
	~CTimeOutTimer();

protected: // From CTimer
/*!
  @function RunL

  @discussion Invoked when a timeout occurs
  */
    virtual void RunL();

private:
/*!
  @function CTimeOutTimer
  
  @discussion Perform the first phase of two phase construction 
  @param aPriority priority to use for this timer
  @param aTimeOutNotify An observer to notify
  */
	CTimeOutTimer(const TInt aPriority, MTimeOutNotifier& aTimeOutNotify);

/*!
  @function ConstructL
  
  @discussion Perform the second phase construction of a CTimeOutTimer 
  */
    void ConstructL();

private: // Member variables

    /*! @var The observer for this objects events */
	MTimeOutNotifier& iNotify;
    };

#endif // __TIMEOUTTIMER_H__


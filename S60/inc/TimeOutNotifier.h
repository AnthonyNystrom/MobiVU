// TIMEOUTNOTIFIER.H
//
// Copyright (c) 1997-1999 Symbian Ltd.  All rights reserved.
//


#ifndef _TIMEOUTNOTIFIER_H_
#define _TIMEOUTNOTIFIER_H_

/*! 
  @class MTimeOutNotifier
  
  @discussion This class specifies the function to be called when a timeout occurs.
  Used in conjunction with CTimeOutTimer class
  */
class MTimeOutNotifier
    {
public:
/*!
  @function TimerExpired

  @discussion The function to be called when a timeout occurs
  */
    virtual void TimerExpired() = 0;
    };

#endif
    
    

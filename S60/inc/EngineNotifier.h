/* Copyright (c) 2001, Nokia Mobile Phones. All rights reserved */


#ifndef __ENGINENOTIFIER_H__
#define __ENGINENOTIFIER_H__

/*! 
  @class MEngineNotifier
  
  @discussion This class specifies the error notification interface
  */
class MEngineNotifier  
    {
public:
/*!
  @enum TErrorType

  @discussion Error types encountered when reading/writing to a sockets
  @value EDisconnected A disconnected error has been encountered
  @value ETimeOutOnWrite A write operation has failed to complete within a predetermined period
  @value EGeneralReadError A general error has been encountered during a read
  @value EGeneralWriteError A general error has been encountered during a write
  */
    enum TErrorType
        {
        EDisconnected,
        ETimeOutOnWrite,
        EGeneralReadError,
        EGeneralWriteError,
        ECreateBuffFile
        };

/*!
  @function ReportError

  @discussion Report a communication error
  @param aErrorType error type
  @param aErrorCode associated error code
  */
	virtual void ReportError(TErrorType aErrorType, TInt aErrorCode) = 0;

/*!
  @function ResponseReceived

  @discussion Data has been received on the socket and read into a buffer
  @param aBuffer the data buffer
  */
	virtual void ResponseReceived(const TDesC8& aBuffer) = 0;
	virtual void ResponseReceived(const TDesC& aBuffer) = 0;
    };

#endif // __ENGINENOTIFIER_H__

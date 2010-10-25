

#ifndef __SOCKETSENGINE_H__
#define __SOCKETSENGINE_H__

#include <in_sock.h>
#include "TimeOutNotifier.h"
#include "EngineNotifier.h"
//#include "Sockets.hrh"
#include "DemoVideoCallAppUi.h"


class CSocketsReader;
class CSocketsWriter;
class CTimeOutTimer;
/*! 
  @class CSckEngine
  
  @discussion This class is the main engine part of the sockets application.
  It establishes a TCP connection using its server name and port number (performing a DNS lookup
  operation first, if appropriate).
  It creates instances of separate active objects to perform reading from, and writing to, the socket.
  */
class CSckEngine : public CActive, public MTimeOutNotifier, public MEngineNotifier
    {
public: // new methods

/*!
  @function NewL
   
  @discussion Create a CSckEngine object
  @param aConsole console to use for ui output
  @result a pointer to the created instance of CSckEngine
  */
	static CSckEngine* NewL(CDemoVideoCallAppUi *aAppUi);

/*!
  @function NewLC
   
  @discussion Create a CSckEngine object
  @param aConsole console to use for ui output
  @result a pointer to the created instance of CSckEngine
  */
    static CSckEngine* NewLC(CDemoVideoCallAppUi *aAppUi);

/*!
  @function ~CSckEngine
  
  @discussion Destroy the object and release all memory objects
  */
	~CSckEngine();

/*!
  @function ConnectL

  @discussion Initiate connection of socket, using iServerName and iPort
  */
    void ConnectL();

	void ListenL(/*TUint32 aAddr */);
/*!
  @function Disconnect

  @discussion Disconnect socket
  */
    void Disconnect();

/*!
  @function WriteL

  @discussion Write data to socket
  @param aData data to be written
  */
	void WriteL(const TDesC8& aData);

/*!
  @function Read

  @discussion Initiate read of data from socket
  */
	void Read();

/*!
  @function SetServerName

  @discussion Set name of server to connect to
  @param aName new server name
  */
    void SetServerName(const TDesC& aName);

/*!
  @function ServerName

  @discussion Get server name
  @result name of server
  */
    const TDesC& ServerName() const;

/*!
  @function SetPort

  @discussion Set port number to connect to
  @param aPort new port number
  */
    void SetPort(TInt aPort);

/*!
  @function Port

  @discussion Get port number
  @result port number
  */
    TInt Port() const;

/*!
  @function Connected

  @discussion Is socket fully connected?
  @result true if socket is connected
  */
    TBool Connected() const;

public: // from MTimeOutNotifier
/*!
  @function TimerExpired

  @discussion The function to be called when a timeout occurs
  */
	void TimerExpired();

public: // from MEngineNotifier
/*!
  @function ReportError

  @discussion Report a communication error
  @param aErrorType error type
  @param aErrorCode associated error code
  */
	void ReportError(MEngineNotifier::TErrorType aErrorType, TInt aErrorCode);

/*!
  @function ResponseReceived

  @discussion Data has been received on the socket and read into a buffer
  @param aBuffer the data buffer
  */
	void ResponseReceived(const TDesC8& aBuffer);
	void ResponseReceived(const TDesC& aBuffer);	

protected: // from CActive
/*!
  @function DoCancel
   
  @discussion cancel any outstanding operation
  */
	void DoCancel();

/*!
  @function RunL
   
  @discussion called when operation complete
  */
    void RunL();

private: // New methods
/*!
  @function CSckEngine
  
  @discussion Perform the first phase of two phase construction 
  @param aConsole the console to use for ui output
  */
	CSckEngine(CDemoVideoCallAppUi *aAppUi);

/*!
  @function ConstructL
  
  @discussion Perform the second phase construction of a CSckEngine 
  */
    void ConstructL();

/*!
  @function ConnectL
   
  @discussion initiate a connect operation on a socket
  @param aAddr the ip address to connect to
  */
	void ConnectL(TUint32 aAddr);	
	

/*!
  @enum TSocketsEngineState

  @discussion Tracks the state of this object through the connection process
  @value ENotConnected The initial (idle) state
  @value EConnecting A connect request is pending with the socket server
  @value EConnected A connection has been established
  @value ELookingUp A DNS lookup request is pending with the socket server
  */
	enum TSocketsEngineState 
	    {
		ENotConnected,
        EConnecting,
        EConnected,
		ELookingUp,
		EListening
	    };

/*!
  @function ChangeStatus
   
  @discussion handle a change in this object's status
  @param aNewStatus new status
  */
    void ChangeStatus(TSocketsEngineState aNewStatus);


private: // Member variables

    /*! @const The maximum time allowed for a lookup or connect requests to complete */
    static const TInt KTimeOut;

    /*! @const The initial port number displayed to the user */
    static const TInt KDefaultPortNumber;

    /*! @var this object's current status */
	TSocketsEngineState         iEngineStatus;

	/*! @var the listen socket */
	RSocket						iListenSocket;

    /*! @var the actual socket */
	RSocket                     iSocket;

    /*! @var active object to control reads from the socket */
	CSocketsReader*             iSocketsReader;

    /*! @var active object to control writes to the socket */
	CSocketsWriter*             iSocketsWriter;

    /*! @var the socket server */
	RSocketServ                 iSocketServ;

    /*! @var DNS name resolver */
	RHostResolver               iResolver;

    /*! @var The result from the name resolver */
	TNameEntry                  iNameEntry;

    /*! @var The anme record found by the resolver */
	TNameRecord                 iNameRecord;

    /*! @var timer active object */
	CTimeOutTimer*              iTimer;

    /*! @var The address to be used in the connection */
	TInetAddr                   iAddress;

    /*! @var port number to connect to */
    TInt                        iPort;
    CDemoVideoCallAppUi *iAppUi;

    /*! @var server name to connect to */
    TBuf<100>  iServerName;
    
	TInt iMaxPerc;
	TInt iPeriod;
	TInt iTickC1;
	TInt iTickC2;
    };

#endif // __SOCKETSENGINE_H__


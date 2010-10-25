

#include <eikgted.h>
#include "SocketsEngine.h"
#include "TimeOutTimer.h"
#include "SocketsReader.h"
#include "SocketsWriter.h"
#include "Sockets.h"
#include <hal.h>
#include <CommDbConnPref.h>

const TInt CSckEngine::KTimeOut = 60000000; // 30 seconds time-out

_LIT(KDefaultServerName, "127.0.0.1");
const TInt CSckEngine::KDefaultPortNumber = 7;

CSckEngine* CSckEngine::NewL(CDemoVideoCallAppUi *aAppUi)
    {
	CSckEngine* self = CSckEngine::NewLC(aAppUi);
	CleanupStack::Pop(self);
	return self;
    }

	
CSckEngine* CSckEngine::NewLC(CDemoVideoCallAppUi *aAppUi)
    {
	CSckEngine* self = new (ELeave) CSckEngine(aAppUi);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }

CSckEngine::CSckEngine(CDemoVideoCallAppUi *aAppUi)
: CActive(EPriorityStandard),
  iPort(KDefaultPortNumber),
  iAppUi(aAppUi),
  iServerName(KDefaultServerName)
    {
    }


CSckEngine::~CSckEngine()
    {
    Cancel();

    delete iSocketsReader;
    iSocketsReader = NULL;

    delete iSocketsWriter;
    iSocketsWriter = NULL;

    delete iTimer;
    iTimer = NULL;

    iSocketServ.Close();
    }


void CSckEngine::ConstructL()
    {
	ChangeStatus(ENotConnected);

    // Start a timer
	iTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
	CActiveScheduler::Add(this); 

	// Open channel to Socket Server
	User::LeaveIfError(iSocketServ.Connect());

    // Create socket read and write active objects
	iSocketsReader = CSocketsReader::NewL(*this, iSocket);
	iSocketsWriter = CSocketsWriter::NewL(*this, iSocket);
	
	if (HAL::Get(HALData::ESystemTickPeriod, iPeriod) != KErrNone)
		iPeriod = 1000000/64;	
    }

void CSckEngine::ConnectL()
    {    
    iAppUi->LogInfo(_L("CSckEngine::ConnectL():%d"),iEngineStatus);
    // Initiate connection process
    if (iEngineStatus == ENotConnected)
        {
        TInetAddr addr;
        if (addr.Input(iServerName) == KErrNone)
            {            
            // server name is already a valid ip address
            TRAPD(iErr,ConnectL(addr.Address()));            
            }
        else // need to look up name using dns
            {
	        // Initiate DNS
	        User::LeaveIfError(iResolver.Open(iSocketServ, KAfInet, KProtocolInetUdp));
	        // DNS request for name resolution
	        iResolver.GetByName(iServerName, iNameEntry, iStatus);
	        
	        ChangeStatus(ELookingUp);
	        // Request time out
	        iTimer->After(KTimeOut);
	        SetActive();
            }
        }
    }


void CSckEngine::ConnectL(TUint32 aAddr) // <a name="ConnectL32">
    {
    // Initiate attempt to connect to a socket by IP address	
    if (iEngineStatus == ENotConnected)
        {
	        RConnection conn;
	    	TCommDbConnPref prefs;
	    	iAppUi->LogInfo(_L("try to open"));
	    	User::LeaveIfError(iSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp));
			iAppUi->LogInfo(_L("opened"));

		    // Set up address information
		    iAddress.SetPort(iPort);
		    iAddress.SetAddress(aAddr);
		
		    // Initiate socket connection
		    TRAPD(error, iSocket.Connect(iAddress, iStatus));
		    if (error)
		    	iAppUi->LogInfo(_L("Error to connect socket:%d"), error);
		    else{
			    ChangeStatus(EConnecting);
			    
			    // Start a timeout
			    iTimer->After(KTimeOut);
			
			    SetActive();
		    }
    	}
    
    }

const TUint KPortNum = 7000;

void CSckEngine::ListenL(/*TUint32 aAddr */)
	{
	// TInetAddr inet_addr( KInetAddrAny, KPortNum );
	TInetAddr inet_addr( INET_ADDR(127,0,0,1), KPortNum );
	
	User::LeaveIfError( iListenSocket.Open( iSocketServ, KAfInet, KSockStream, /*KUndefinedProtocol*/KProtocolInetTcp) );
	User::LeaveIfError( iSocket.Open( iSocketServ ) );

	// Bind the socket to the required port.
	iListenSocket.Bind( inet_addr );

	// Listen for incoming connections
	iListenSocket.Listen( 1 );
		
	// Accept incoming connections
	// iChannelState = EChannelListening;
	// TBuf<30> buf(KListening);
	// iObserver->NotifyMsg( buf );
	iListenSocket.Accept( iSocket, iStatus );
	
	ChangeStatus( EListening );

	SetActive();
	}

void CSckEngine::Disconnect()
    {
    iAppUi->LogInfo(_L("CSckEngine::Disconnect() start"));
	// cancel all outstanding operations
    // since we are connected, the only possibilities are read and write
    iSocketsReader->Cancel();
    iSocketsWriter->Cancel();
	iSocket.Close();
	//iSocketServ.Close();
	ChangeStatus(ENotConnected);
	iSocketsWriter->CreateFileBuffer();
	iAppUi->LogInfo(_L("CSckEngine::Disconnect() end"));
    }

// from CActive
void CSckEngine::DoCancel()
    {
    iTimer->Cancel();

    // Cancel appropriate request to socket
    switch (iEngineStatus)
        {
        case EConnecting:
            iSocket.CancelConnect();
            iSocket.Close();
            break;
        case ELookingUp:
            // Cancel look up attempt
            iResolver.Cancel();
            iResolver.Close();
            break;
		case EListening:
			iSocket.CancelAccept();
			break;
	    default:
	    	iAppUi->LogInfo(_L("CSckEngine::DoCancel() panic"));
            User::Panic(KPaniCSckEngine, ESocketsBadStatus);
            break;
	    }
    iAppUi->LogInfo(_L("CSckEngine::DoCancel()"));
    ChangeStatus(ENotConnected);
    }

void CSckEngine::WriteL(const TDesC8& aData)
    {
    	// Write data to socket
		if (iEngineStatus == EConnected)
        	iSocketsWriter->IssueWriteL(aData);
    }
	
void CSckEngine::Read()
    {
    // Initiate read of data from socket
	if ((iEngineStatus == EConnected) && (!iSocketsReader->IsActive())) 
        {
        iSocketsReader->Start();
        }
    }

// from CActive
void CSckEngine::RunL()
    {
    // Active object request complete handler.
    // iEngineStatus flags what request was made, so its
    // completion can be handled appropriately
	iTimer->Cancel(); // Cancel TimeOut timer before completion
	iAppUi->LogInfo(_L("CSckEngine::RunL()"));

	switch(iEngineStatus)
	    {
	    case EConnecting:
		    // IP connection request
		    if (iStatus == KErrNone)
	        // Connection completed successfully
	        {
			    ChangeStatus(EConnected);
			    Read(); //Start CSocketsReader Active object
	        }
		    else{
		        iAppUi->LogInfo(_L("CSckEngine::RunL() Connection failed:%d"), iStatus);
		        iAppUi->ShowMessage(_L("Connection failed"), iStatus.Int());
		        Disconnect();
		    }
		    break;

		case EListening:
		    if (iStatus == KErrNone)
		        // Connection completed successfully
		        {
			    ChangeStatus( EConnected );
			    Read(); //Start CSocketsReader Active object
		        }
			break;

	    case ELookingUp:
		    iResolver.Close();
		    if (iStatus == KErrNone)
		        {
    		    // DNS look up successful
			    iNameRecord = iNameEntry();
			    // Extract domain name and IP address from name record
                TBuf<15> ipAddr;
			    TInetAddr::Cast(iNameRecord.iAddr).Output(ipAddr);
			    // And connect to the IP address
			    iAppUi->LogInfo(_L("CSckEngine::RunL() ELookingUp"));
                ChangeStatus(ENotConnected);
			    ConnectL(TInetAddr::Cast(iNameRecord.iAddr).Address());
		        }
		    else
		        {	
                // DNS lookup failed
		        iAppUi->LogInfo(_L("CSckEngine::RunL() DNS lookup failed"));
                ChangeStatus(ENotConnected);
		        }
		    break;
	    default:
	    	iAppUi->LogInfo(_L("CSckEngine::RunL() ESocketsBadStatus"));
            User::Panic(KPaniCSckEngine, ESocketsBadStatus);
            break;

	    };
    }

void CSckEngine::TimerExpired()
    {
	Cancel();
	iAppUi->LogInfo(_L("Timed out connection"));
	Disconnect();
    }
void CSckEngine::ReportError(MEngineNotifier::TErrorType aErrorType, TInt aErrorCode)
    {
    switch (aErrorType)
        {
        case MEngineNotifier::ECreateBuffFile:
	    	Disconnect();
	    	iAppUi->LogInfo(_L("CSckEngine::ReportError ECreateBuffFile"));
	        break;
        case MEngineNotifier::EDisconnected:
	    	Disconnect();
	    	iAppUi->LogInfo(_L("CSckEngine::ReportError Host Disconnected"));
            break;
        case MEngineNotifier::EGeneralReadError:
        	Disconnect();
        	iAppUi->LogInfo(_L("CSckEngine::ReportError EGeneralReadError"));
            break;
        case MEngineNotifier::ETimeOutOnWrite:
        	Disconnect();
        	iAppUi->LogInfo(_L("CSckEngine::ReportError ETimeOutOnWrite"));
            break;
        case MEngineNotifier::EGeneralWriteError:
        	Disconnect();
        	iAppUi->LogInfo(_L("CSckEngine::ReportError EGeneralWriteError"));
            break;
        default:
        	iAppUi->LogInfo(_L("CSckEngine::ReportError ESocketsBadStatus"));
            User::Panic(KPaniCSckEngine, ESocketsBadStatus);
            break;
        }
    }


void CSckEngine::ResponseReceived(const TDesC8& aBuffer)
{
	iAppUi->SocketReceiver(aBuffer);
}

void CSckEngine::ResponseReceived(const TDesC& aBuffer)
    {
    }

void CSckEngine::ChangeStatus(TSocketsEngineState aNewStatus)
    {
    iEngineStatus = aNewStatus;
    // Update the status (and the status display)
    switch (aNewStatus)
        {
        case ENotConnected:
            iAppUi->LogInfo(_L("CSckEngine::ChangeStatus ENotConnected"));
            iAppUi->RefreshStatus();
            break;
        case EConnecting:
        	iAppUi->LogInfo(_L("CSckEngine::ChangeStatus EConnecting"));
            break;
        case EConnected:
        	iAppUi->LogInfo(_L("CSckEngine::ChangeStatus EConnected"));
            iSocketsWriter->CreateFileBuffer();
        	iAppUi->SendCmd(CMD_LOGIN);
            // can start            
            break;
        case ELookingUp:
        	iAppUi->LogInfo(_L("CSckEngine::ChangeStatus ELookingUp"));
            break;
		case EListening:
			iAppUi->LogInfo(_L("CSckEngine::ChangeStatus EListening"));
			break;
        default:
        	iAppUi->LogInfo(_L("CSckEngine::ChangeStatus ESocketsBadStatus"));
            User::Panic(KPaniCSckEngine, ESocketsBadStatus);
            break;
        }
    }

void CSckEngine::SetServerName(const TDesC& aName)
    {
    iServerName.Copy(aName);
    }

const TDesC& CSckEngine::ServerName() const
    {
    return iServerName;
    }

void CSckEngine::SetPort(TInt aPort)
    {
    iPort = aPort;
    }

TInt CSckEngine::Port() const
    {
    return iPort;
    }

TBool CSckEngine::Connected() const
    {
    return (iEngineStatus == EConnected);
    }



#include "Sockets.h"
#include "SocketsReader.h"
#include "EngineNotifier.h"

CSocketsReader* CSocketsReader::NewL(MEngineNotifier& aEngineNotifier, RSocket& aSocket)
    {
	CSocketsReader* self = CSocketsReader::NewLC(aEngineNotifier, aSocket);
	CleanupStack::Pop(self);
	return self;
    }

	
CSocketsReader* CSocketsReader::NewLC(MEngineNotifier& aEngineNotifier, RSocket& aSocket)
    {
	CSocketsReader* self = new (ELeave) CSocketsReader(aEngineNotifier, aSocket);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }


CSocketsReader::CSocketsReader(MEngineNotifier& aEngineNotifier, RSocket& aSocket)
: CActive(EPriorityStandard),
  iSocket(aSocket),
  iEngineNotifier(aEngineNotifier)
    {
    }


CSocketsReader::~CSocketsReader()
    {
    Cancel();
    }


void CSocketsReader::ConstructL()
    {
	CActiveScheduler::Add(this);
    }

void CSocketsReader::DoCancel()
    {
    // Cancel asychronous read request
	iSocket.CancelRead();
    }

#include <eikenv.h>

// The following function is used to display a message.
void MessageBox(const TDesC &aDes)
{
    // Launch Info window with the specified title and message.
    CEikonEnv* iEnv = CEikonEnv::Static();
	iEnv->AlertWin( aDes );
}


void CSocketsReader::RunL()
    {
    // Active object request complete handler
    switch (iStatus.Int())
        {
        case KErrNone:
			{
            iEngineNotifier.ResponseReceived(iBuffer);
            iBuffer.Zero();
		    IssueRead(); // Immediately start another read
            break;
			}
        case KErrDisconnected:
            iEngineNotifier.ReportError(MEngineNotifier::EDisconnected,
                                        iStatus.Int());
            break;
        default:
            iEngineNotifier.ReportError(MEngineNotifier::EGeneralReadError,
                                        iStatus.Int());
            break;
        }	
    }

void CSocketsReader::IssueRead()
    {
    // Initiate a new read from socket into iBuffer
    __ASSERT_ALWAYS(!IsActive(), User::Panic(KPanicSocketsEngineRead, ESocketsBadState));
    TRAPD(iErr,iSocket.RecvOneOrMore(iBuffer, 0, iStatus, iDummyLength));
    if (iErr == KErrNone) 
    	SetActive();
    }

void CSocketsReader::Start()
    {
    // Initiate a new read from socket into iBuffer
    if (!IsActive())
        {
        IssueRead();
        }
    }

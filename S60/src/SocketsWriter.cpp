
#include "SocketsWriter.h"
#include "TimeOutTimer.h"
#include "Sockets.h"
#include "EngineNotifier.h"
#include <utf.h>
#include <pathinfo.h>

const TInt CSocketsWriter::KTimeOut = 30000000; // 30 seconds time-out

CSocketsWriter* CSocketsWriter::NewL(MEngineNotifier& aEngineNotifier, RSocket& aSocket)
    {
	CSocketsWriter* self = CSocketsWriter::NewLC(aEngineNotifier, aSocket);
	CleanupStack::Pop(self);
	return self;
    }

	
CSocketsWriter* CSocketsWriter::NewLC(MEngineNotifier& aEngineNotifier, RSocket& aSocket)
    {
	CSocketsWriter* self = new (ELeave) CSocketsWriter(aEngineNotifier, aSocket);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }


CSocketsWriter::CSocketsWriter(MEngineNotifier& aEngineNotifier, RSocket& aSocket)
: CActive(EPriorityStandard),
  iSocket(aSocket),
  iEngineNotifier(aEngineNotifier)
    {
    }


CSocketsWriter::~CSocketsWriter()
{
    Cancel();
    delete iTimer;
    iTimer = NULL;
    if (fileBuffer){
    	fileBuffer->Close();
    	delete fileBuffer;
    }
}

void CSocketsWriter::DoCancel()
    {	
    // Cancel asychronous write request
	iSocket.CancelWrite();
    iTimer->Cancel();
	iWriteStatus = EWaiting;
    }

void CSocketsWriter::ConstructL()
    {
	CActiveScheduler::Add(this);
	iAppUi = static_cast<CDemoVideoCallAppUi*> (CEikonEnv::Static()->EikAppUi());
	
	iTimeOut = KTimeOut; 
	iTimer = CTimeOutTimer::NewL(CActive::EPriorityUserInput, *this);
	iWriteStatus = EWaiting;
	fileBuffer   = NULL;
	const TDesC& root = PathInfo::PhoneMemoryRootPath();
	m_filenameBuffer.Append(root);
	m_filenameBuffer.Append(_L("buffer.dat"));
	
    }

void CSocketsWriter::TimerExpired()
    {
	Cancel();
	iWriteStatus = EWaiting;
    iEngineNotifier.ReportError(MEngineNotifier::ETimeOutOnWrite, KErrTimedOut);
    }

void CSocketsWriter::DeleteFileBuffer()
{
	if (fileBuffer){
		fileBuffer->Close();
		delete fileBuffer;
	}
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);
	TRAPD(iErr,fs.Delete(m_filenameBuffer));
	fs.Close();
}
void CSocketsWriter::CreateFileBuffer()
{
//iGlobalEngine->LogInfo(_L("Start CreateFileBuffer"));
	if (!fileBuffer){
		fileBuffer = new RFile();
	}
	else
		fileBuffer->Close();
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);
	m_iSizeFile = 0;
	m_iPosRead  = 0;
	TRAPD(iErr, fileBuffer->Replace(fs, m_filenameBuffer, EFileWrite | EFileStream | EFileRead | EFileShareExclusive));
	if (iErr!=KErrNone){
		delete fileBuffer;
		fileBuffer = NULL;
		iEngineNotifier.ReportError(MEngineNotifier::ECreateBuffFile,0xFFFF);
	}
	//iGlobalEngine->LogInfo(_L("OK End CreateFileBuffer"));
}

void CSocketsWriter::RunL()
    {
    iTimer->Cancel();
    

    // Active object request complete handler
	if (iStatus == KErrNone)
	    {
		switch(iWriteStatus)
		    {
		    // Character has been written to socket
		    case ESending:
                SendNextPacket();
			    break;
		    default:
                User::Panic(KPanicSocketsEngineWrite, ESocketsBadStatus);
                break;
		    };
	    }
    else 
	    {
		// Error: pass it up to user interface
        iEngineNotifier.ReportError(MEngineNotifier::EGeneralWriteError, iStatus.Int());
        iWriteStatus = EWaiting;
	    }
    }

void CSocketsWriter::IssueWriteL(const TDesC8& aData)
    {
    if (((iWriteStatus != EWaiting) && (iWriteStatus != ESending)) || (!fileBuffer))
	{   
		iAppUi->LogInfo(_L("Write Exit"));
		if (!fileBuffer)
			iAppUi->LogInfo(_L("NULL File pointer"));
		User::Leave(KErrNotReady);
	}
    //iGlobalEngine->LogInfo(_L("fileBuffer->Write:%d"),aData.Length());
    TRAPD(iErr, fileBuffer->Write(m_iSizeFile+1, aData));
	if (iErr==KErrNone){
	    m_iSizeFile+=aData.Length();
	    if (!IsActive())
	        SendNextPacket();
	    
		}
	else
		iAppUi->LogInfo(_L("ERROR fileBuffer->Write"));
    }

void CSocketsWriter::TestBW()
	{
		TInt i;

		bWaitBuffering = EFalse;
		iBufBW.Zero();
		for (i=0;i<KWriteBufferSizeTest+1;i++){
			if (!(i%1000) && i)
			{	IssueWriteL(iBufBW);
				iBufBW.Zero();
			}
			iBufBW.Append(0);
		}
	}


void CSocketsWriter::SendNextPacket()
    {
    if (m_iPosRead!=m_iSizeFile)
    {
        int i2R=KWriteBufferSize;
        if (i2R>m_iSizeFile-m_iPosRead)
        	i2R = m_iSizeFile-m_iPosRead;
        //iGlobalEngine->LogInfo(_L("fileBuffer->Read:%d"),i2R);
        TRAPD(iErr, fileBuffer->Read(m_iPosRead, iTransferBuffer, i2R));
        if (iErr==KErrNone){
	        iWriteBuffer = iTransferBuffer;
	    	iTransferBuffer.Zero();
	
	    	TRAPD(iErr, iSocket.Write(iWriteBuffer, iStatus)) // Initiate actual write
		    if (iErr == KErrNone) {
		    	//iGlobalEngine->LogInfo(_L("Socket sent:%d"),i2R);

		    	m_iPosRead += iWriteBuffer.Length();
		    	int iBF	    = m_iSizeFile-m_iPosRead;
		    	//iAppUi->BufferingSizeInfo(iBF);	// show buffer size
		    	
			    //User::WaitForRequest(iStatus);
		        // Request timeout
			    iTimer->After(iTimeOut);
			    SetActive();
			    iWriteStatus = ESending;
		    }
		    else{
		    	//User::Panic(KPanicSocketsEngineWrite, iErr);
		    	iEngineNotifier.ReportError(MEngineNotifier::ETimeOutOnWrite, KErrTimedOut);
		    }
        }
    }
    else if (fileBuffer)
	    iWriteStatus = EWaiting;    
    }



/*
 ============================================================================
 Name		: DemoVideoCallAppUi.cpp
 Author	  : 7TouchGroup
 Copyright   : 7TouchGroup
 Description : CDemoVideoCallAppUi implementation
 ============================================================================
 */

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <aknglobalnote.h> 
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>
#include <utf.h>
#include <pathinfo.h> 
#include <stdlib.h>
#include <hal.h>
#include <DemoVideoCall_0xE5EC06EB.rsg>

//#include "DemoVideoCall_0xE5EC06EB.hlp.hrh"
#include "DemoVideoCall.hrh"
#include "DemoVideoCall.pan"
#include "DemoVideoCallApplication.h"
#include "DemoVideoCallAppUi.h"
#include "DemoVideoCallAppView.h"

// Constants
//_LIT(KFileName, "C:\\private\\E5EC06EB\\DemoVideoCall.txt");
_LIT(KRingFileName,"C:\\Data\\Sounds\\incoming.mp3");
//_LIT(KText, "Demo VideoCall");
_LIT(KSettingsFile, "settings.dat");
_LIT(KLogFile, "demovideocall.log");

#define MODE_AUDIOVIDEO 0
#define MODE_AUDIO 		1
#define MODE_VIDEO 		2

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDemoVideoCallAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDemoVideoCallAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);
	
    bufferNewPkt = NULL;
    lenNewPkt    = 0;

	ritorno     = 0;
	_iCntErr    =_iCnt=0;
	lenRitorno  = 0;
	_bClearLog  = ETrue;
	_iCmd       = -1;	
	_iStatus    = STATUS_UNLOGGED;
	_bPLaying   = EFalse;
	_bSpeaking  = EFalse;
	_bSpeakingBoth = EFalse;
	_iStatusKey = 0;
	_iKey       = 0;

	if (HAL::Get(HALData::ESystemTickPeriod, iPeriod) != KErrNone)
		iPeriod = 1000000/64;
	
	// Create view object
	_viewDefault = CDefaultView::NewL();
	AddViewL(_viewDefault);
	SetDefaultViewL(*_viewDefault);

    RFs& fsSession = iEikonEnv->FsSession();
    User::LeaveIfError(fsSession.CreatePrivatePath( EDriveC ) );
    User::LeaveIfError(fsSession.PrivatePath(iSettingsFile));
    iSettingsFile += KSettingsFile;
	iData = CSettingsData::NewL();
    InternalizeSettingsDataL();	
	_viewSettings = CSettingListView::NewL(*iData);
	AddViewL(_viewSettings);	
	
	_iEngine = CEngine::NewL();
	_iEngine->StartL(500000);
	
	_iRingTone = CMdaAudioPlayerUtility::NewFilePlayerL(KRingFileName, *this);
	
	if (iData->UserName().Length()<=0){
		ShowSettings();			
	}
	else
		ConnectSocket();
}
TUint CDemoVideoCallAppUi::GetTick(){
	TUint tick = User::TickCount();
	TInt64 usecs = TInt64(tick)*TInt64(iPeriod);
		#ifndef I64LOW
		#define I64LOW(x) ((x).Low())
		#endif
	TUint msecs = I64LOW(usecs / 1000);
	return msecs;
}

void CDemoVideoCallAppUi::ConnectSocket()
{
	if (_iSck)
	{	_iSck->Disconnect();
		delete _iSck;
	}
	_iSck = CSckEngine::NewL(this);
	//_iSck->SetServerName(_L("192.168.1.4"));
	//_iSck->SetServerName(_L("69.21.114.100"));
	
	TInetAddr::Cast(iData->IpAddress()).Output(_ipAddr);
	LogInfo(_ipAddr);
	_iSck->SetServerName(_ipAddr);
	_iSck->SetPort(8082);
	_iSck->ConnectL(); // Initiate connection
}

// -----------------------------------------------------------------------------
// CDemoVideoCallAppUi::CDemoVideoCallAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDemoVideoCallAppUi::CDemoVideoCallAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CDemoVideoCallAppUi::~CDemoVideoCallAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CDemoVideoCallAppUi::~CDemoVideoCallAppUi()
{		
	if (_viewDefault)
		delete _viewDefault;
	if (_viewSettings)
		delete _viewSettings;
	if (iData)
		delete iData;
	if (_iEngine)
		delete _iEngine;
	CloseConnection();
	if (ritorno)
		delete ritorno;
}

void CDemoVideoCallAppUi::CloseConnection()
{
	if (_iSck){
		_iSck->Disconnect();
		delete _iSck;
	}
}
void CDemoVideoCallAppUi::DisplayFrame(CFbsBitmap* aFrame)
{
	_viewDefault->iAppView->DisplayFrame(aFrame);
}

// -----------------------------------------------------------------------------
// CDemoVideoCallAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CDemoVideoCallAppUi::HandleCommandL(TInt aCommand)
	{
	switch(aCommand)
        {
        case EEikCmdExit:
		case EAknSoftkeyExit:	// only softkey handled at this level			
			if (_iSck && _iSck->Connected())
				SendCmd(CMD_LOGOUT);            
			ExternalizeSettingsDataL();
            Exit();
            break;
        default:
            break;
        }
	}
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CDemoVideoCallAppUi::HandleStatusPaneSizeChange()
	{
	
	}

CArrayFix<TCoeHelpContext>* CDemoVideoCallAppUi::HelpContextL() const
	{/*
#warning "Please see comment about help and UID3..."
	// Note: Help will not work if the application uid3 is not in the
	// protected range.  The default uid3 range for projects created
	// from this template (0xE0000000 - 0xEFFFFFFF) are not in the protected range so that they
	// can be self signed and installed on the device during testing.
	// Once you get your official uid3 from Symbian Ltd. and find/replace
	// all occurrences of uid3 in your project, the context help will
	// work.
	CArrayFixFlat<TCoeHelpContext>* array = new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
	CleanupStack::PushL(array);
	array->AppendL(TCoeHelpContext(KUidDemoVideoCallApp, KGeneral_Information));
	CleanupStack::Pop(array);
	return array;*/
	
	}

void CDemoVideoCallAppUi::LogInfo(char *pBuf){
	TPtrC8 ptr8((const TUint8*)pBuf); 
	
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);
	RFile file;

	TBuf<200> filename;
	const TDesC& root = PathInfo::PhoneMemoryRootPath();

	filename.Append(root);
	filename.Append(KLogFile);
	if (_bClearLog){
		_bClearLog=EFalse;
		fs.Delete(filename);
	}

	TInt err = file.Open(fs, filename, EFileWrite | EFileStreamText);
	if (err == KErrNotFound)
		err = file.Create(fs, filename, EFileWrite | EFileStreamText);
	if (err == KErrNone) {
		CleanupClosePushL(file);
		TInt pos = 0;
		file.Seek(ESeekEnd, pos);
		file.Write(ptr8);
		CleanupStack::PopAndDestroy(&file);
	}
		
	CleanupStack::PopAndDestroy(&fs);
}
void CDemoVideoCallAppUi::LogInfo(TRefByValue<const TDesC> aFormat, ...)
{
	VA_LIST list;
	HBufC* buf = HBufC::NewLC(1000);
	VA_START(list, aFormat);
	buf->Des().FormatList(aFormat, list);
	VA_END(list);
	HBufC8* buf8 = HBufC8::NewLC(1000);
	TPtr8 ptr = buf8->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, *buf);
	
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);
	
	RFile file;

	TBuf<200> filename;
	const TDesC& root = PathInfo::PhoneMemoryRootPath();

	filename.Append(root);
	filename.Append(KLogFile);
	if (_bClearLog){
		_bClearLog=EFalse;
		fs.Delete(filename);
	}

	TInt err = file.Open(fs, filename, EFileWrite | EFileStreamText);
	if (err == KErrNotFound)
		err = file.Create(fs, filename, EFileWrite | EFileStreamText);
	if (err == KErrNone) {
		CleanupClosePushL(file);
	
		HBufC8* outBuf = HBufC8::NewLC(1500);
		TBuf<100> timestamp;
		TTime time;
		time.HomeTime();
		time.FormatL(timestamp, _L("%/0%1%/1%2%/2%3%/3 %:0%J%:1%T%:2%S%:3 "));
		TPtr8 outPtr = outBuf->Des();
		CnvUtfConverter::ConvertFromUnicodeToUtf8(outPtr, timestamp);
		outBuf->Des().Append(*buf8);
		outBuf->Des().Append(_L8("\r\n"));
	
		TInt pos = 0;
		file.Seek(ESeekEnd, pos);
		file.Write(*outBuf);
		CleanupStack::PopAndDestroy(outBuf);
		CleanupStack::PopAndDestroy(&file);
	}
	
	CleanupStack::PopAndDestroy(&fs);
	
	CleanupStack::PopAndDestroy(buf8);
	CleanupStack::PopAndDestroy(buf);
}

void CDemoVideoCallAppUi::InternalizeSettingsDataL()
    {
    RFs& fs = iEikonEnv->FsSession();
    RFileReadStream readStream;
    TInt error = readStream.Open(fs, iSettingsFile, EFileRead);
    TInt internalizationError = KErrNone;
    // if file existed, try to read settings.
    if (error == KErrNone)
        TRAP(internalizationError, iData->LoadL(readStream);)
    readStream.Release();

    // reading failed, settings file might be corrupted.    
    if (internalizationError != KErrNone)
        TRAP(internalizationError, fs.Delete(iSettingsFile));
    }


void CDemoVideoCallAppUi::ExternalizeSettingsDataL()
    {
    RFs& fs = iEikonEnv->FsSession();
    RFileWriteStream writeStream;
    TInt error = writeStream.Open(fs, iSettingsFile, EFileWrite);
    // setting file did not exist, create one.
    if (error != KErrNone)        
        TRAPD(error, writeStream.Create(fs, iSettingsFile, EFileWrite));
            
    writeStream.PushL();
    iData->SaveL(writeStream);    
    writeStream.CommitL();
    writeStream.Pop();
    writeStream.Release();    
    }    

void CDemoVideoCallAppUi::ShowSettings(){
	ActivateLocalViewL(_viewSettings->Id());
}
void CDemoVideoCallAppUi::ShowDefault(){
	ActivateLocalViewL(_viewDefault->Id());
	TBuf<32> ipAddr;
	TInetAddr::Cast(iData->IpAddress()).Output(ipAddr);
	if (_ipAddr.Compare(ipAddr))	// ip changed ?
		ConnectSocket();	
}
void CDemoVideoCallAppUi::ShowMessage(const TDesC& aErrMessage, TInt aErrCode)
{
    _LIT(KNewLine, "\n");

    TRAPD(error,

    HBufC* errorTitleCode = HBufC::NewLC(50);
	errorTitleCode->Des().Append(aErrMessage);
    errorTitleCode->Des().Append(KNewLine);
    if (aErrCode!=0xFFFF && aErrCode!=0)
    	errorTitleCode->Des().AppendNum(aErrCode);

    CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
    globalNote->ShowNoteL(EAknGlobalErrorNote, *errorTitleCode); 
    CleanupStack::PopAndDestroy(globalNote);
    CleanupStack::PopAndDestroy(errorTitleCode);
    )
}


void *CDemoVideoCallAppUi::xmalloc(int size) {
	void *mem = malloc(size);
   if (mem == NULL) {
	   LogInfo(_L("ERROR: xmalloc %d"), size);
      Exit();
   }
   return mem;
}


/* free */
void CDemoVideoCallAppUi::xfree(void *mem) {
	if (mem != NULL) 
		free(mem);   
}

bool CDemoVideoCallAppUi::GiveMeAPacket(unsigned char *pbufInput, unsigned short iSizeInput, unsigned char **pBuffOutput, unsigned short *piSizeOutput)
{
    static unsigned short pktDataLen; // Lunghezza del pacchetto in arrivo
    unsigned short newSize;

    unsigned short pktTail = *piSizeOutput; // Lunghezza del buffer di uscita prima dell'elaborazione di questo flusso in ingresso

unsigned char *pbufInput2=NULL;

    // Devo recuperare gli avanzi del flusso precedente
    if ( iSizeInput == 0) {
        // Copio la roba rimasta dal vecchio flusso come nuova roba in ingresso
        if ( lenNewPkt ) {
            pbufInput2=(unsigned char*)xmalloc(lenNewPkt);
            memcpy(pbufInput2, bufferNewPkt, lenNewPkt);
            iSizeInput=lenNewPkt;
            *piSizeOutput = 0;
pbufInput=pbufInput2;

            lenNewPkt=0;
            xfree(bufferNewPkt);bufferNewPkt=NULL;
        } else {
            // Nel buffer precedente non mi Ë rimasto niente del nuovo pacchetto
            pktDataLen=0;

            return false;
        }
    }

    // Verifico se Ë l'inizio di un nuovo pacchetto
    if (*piSizeOutput == 0) {
        // Verifico che in ingresso abbia almeno 2 bytes
        if (iSizeInput < 2) {
            // Troppi pochi dati in ingresso per essere analizzati
            pktDataLen = 0;
            newSize=iSizeInput;// Che √® 1

            *piSizeOutput = newSize;
            *pBuffOutput=(unsigned char*)xmalloc(newSize);
            memcpy(*pBuffOutput, pbufInput, newSize); // non metto nel buffer la prima parte del byte di lunghezza il secondo byte arriver√†
        } else {
            unsigned char *inizioPtr = pbufInput;
            do {
                // Memorizzo la lunghezza del pacchetto in arrivo
                pktDataLen = checkPktLen( *(unsigned short *)inizioPtr );

                // Sono disallineato, inizia a cercare il prossimo pacchetto
                if ( !pktDataLen ) {
                    inizioPtr++;
                    iSizeInput--;

                    // In questo flusso no trovo nulla di interessante quindi butto via tutto
                    if ( iSizeInput < 2) {
//                        printf("!!! AHIA !!!\n");
if (pbufInput2 != NULL) { xfree(pbufInput2); pbufInput2=NULL;}
                        return false;
                    }
                }
            } while (!pktDataLen);
            pbufInput = inizioPtr;


            // Calcolo lo spazio che devo allocare per questo pacchetto
            if ( (iSizeInput - 2) > pktDataLen ) newSize=pktDataLen;
            else newSize=(iSizeInput - 2);

            // Assegno il nuovo puntatore e la lunghezza dei dati contenuti
            *piSizeOutput = newSize;
            *pBuffOutput=(unsigned char*)xmalloc(newSize);
            memcpy(*pBuffOutput, pbufInput+2, newSize); // non metto nel buffer i primi 2 bytes
        }
    } else {
        if ( !pktDataLen ) {
            unsigned short skip=1;

            // Compongo la lunghezza del pacchetto prendendone un p√≤ qua e la
            pktDataLen = checkPktLen( (*pbufInput << 8 ) + (**pBuffOutput) );

            // Sono disallineato
            if (!pktDataLen) {
                unsigned char *inizioPtr = pbufInput;

                do {
                    // Memorizzo la lunghezza del pacchetto in arrivo
                    pktDataLen = checkPktLen( *(unsigned short *)inizioPtr );

                    // Sono disallineato, inizia a cercare il prossimo pacchetto
                    if ( !pktDataLen ) {
                        inizioPtr++;
                        iSizeInput--;

                        // In questo flusso no trovo nulla di interessante quindi butto via tutto
                        if ( iSizeInput < 2) {
if (pbufInput2 != NULL) { xfree(pbufInput2); pbufInput2=NULL;}
                            return false;
                        }
                    }
//                    printf("!!! AHIA AZZO !!!\n");
                } while (!pktDataLen);
                pbufInput = inizioPtr;
                skip=2;
            }


            // Calcolo lo spazio che devo allocare per questo pacchetto
            if ( (iSizeInput - skip) > pktDataLen ) newSize=pktDataLen; // Il pacchetto finisce qui
            else newSize=(iSizeInput - skip); // Tutto quelli in ingresso fa parte del pacchetto

            unsigned char *appBuff;
            // Creo il nuovo blocco
            appBuff=(unsigned char*)xmalloc(newSize);

            // Libero la memoria che non mi serve pi˘ (il primo byte dei 2 che mi servivono come inizio pacchetto)
            xfree(*pBuffOutput);*pBuffOutput=NULL;

            // Gli accodo quella appena arrivata
            memcpy(appBuff, pbufInput+skip, newSize);

            // Assegno il nuovo puntatore e la lunghezza dei dati contenuti
            *piSizeOutput = newSize;
            *pBuffOutput = appBuff;

            //printf("in uscita:%d %c\n", *piSizeOutput, **pBuffOutput );
        } else {
            // Ho gi‡ pezzi del pacchetto nel buffer
            unsigned char *appBuff;

            // Calcolo lo spazio che devo allocare per questo pacchetto
            if ( (*piSizeOutput + iSizeInput) > pktDataLen ) newSize=pktDataLen; // Il pacchetto finisce qui
            else newSize=(*piSizeOutput + iSizeInput); // Tutto quelli in ingresso fa parte del pacchetto

            // Creo il nuovo blocco
            appBuff=(unsigned char*)xmalloc(newSize);

            // Copio la parte che gi‡ avevo del pacchetto
            memcpy(appBuff, *pBuffOutput, *piSizeOutput);

            // Libero la memoria che non mi serve pi˘
            xfree(*pBuffOutput);*pBuffOutput=NULL;

            // Gli accodo quella appena arrivata
            memcpy(appBuff + *piSizeOutput, pbufInput, newSize - *piSizeOutput);

            // Assegno il nuovo puntatore e la lunghezza dei dati contenuti
            *piSizeOutput = newSize;
            *pBuffOutput = appBuff;

            //printf("in uscita:%d %c\n", *piSizeOutput, **pBuffOutput );
        }
    }

    // Il pacchetto √® completo o forse c'√® anche del nuovo
    if ( newSize == pktDataLen ) {
        // Verifico e salvo l'eventuale nuovo pacchetto arrivato
        if ( newSize - pktTail == iSizeInput ) {
            if (lenNewPkt>0) {xfree(bufferNewPkt);bufferNewPkt=NULL;}
            //xfree(bufferNewPkt);
            lenNewPkt=0;
        } else {
            // Salvo il nuovo pacchetto in un buffer di appoggio
            lenNewPkt = iSizeInput - (newSize - pktTail);

            // Se Ë un nuovo pacchetto tolgo i 2 bytes di header
            if (!pktTail) lenNewPkt-=2;

            // Creo il nuovo blocco
            bufferNewPkt=(unsigned char*)xmalloc( lenNewPkt );

            // Copio la parte che √® rimasta fuori dal pacchetto
            memcpy(bufferNewPkt, pbufInput + (iSizeInput - lenNewPkt), lenNewPkt);
        }
if (pbufInput2 != NULL) { xfree(pbufInput2); pbufInput2=NULL;}
        pktDataLen=0;
        return true;
    } else {
if (pbufInput2 != NULL) { xfree(pbufInput2); pbufInput2=NULL;}
        // Il pacchetto non √® ancora finito
        return false;
    }
}

void CDemoVideoCallAppUi::CheckPackets(unsigned char *entrata, unsigned short lunghezza)
{
	bool result=false;
	TBuf<64> info;
	
	do {		
		TRAPD(errore, result=GiveMeAPacket(entrata, lunghezza, &ritorno, &lenRitorno));
		if (errore != KErrNone)
			LogInfo(_L(" GiveMeAPacket error:%d"), errore);
		
		/*info.Format(_L("ok: %d ko:%d"), _iCnt, _iCntErr);
		if (_viewDefault->iAppView)
			_viewDefault->iAppView->SetInfo(info);*/
		if (result) {
			int iCmd = ritorno[0] & 0x0F;
			if (lenRitorno>4000)
				LogInfo(_L("mmmm STRANGE lenRitorno>4000 : %d cmd:%d"), lenRitorno, iCmd);
			TPtrC8 p; 
			
			if ((iCmd==_iCmd && _iCmd!=CMD_NULL && _iStatus!=STATUS_TALKING) ||
				(iCmd==CMD_CLOSE_CONV && _iStatus!=STATUS_TALKING) ||
			   (_iStatus==STATUS_TALKING && (iCmd==CMD_FRM_VIDEO || iCmd==CMD_FRM_AUDIO || iCmd==CMD_CLOSE_CONV || iCmd==CMD_SETCODEC)) )
			{
				switch (iCmd){
					default:
						LogInfo(_L("COMANDO SCONOSCIUTO : %d cmd:%d"), lenRitorno, iCmd);
						break;
						
					case CMD_SETCODEC:
						LogInfo(_L("CMD_SETCODEC : %d "), ritorno[1]);
						if (ritorno[1]==CODEC_ID_H263 || ritorno[1]==CODEC_ID_MPEG4)
							_viewDefault->iAppView->InitFFmpeg(ritorno[1]);
						break;
					case CMD_RECORDER:
						_iCmd=CMD_NULL;
						if (lenRitorno>1)
							ShowMessage(_L("Recorder Enabled"), 0);
						else
							ShowMessage(_L("Recorder Disabled"), 0);
						break;
					case CMD_LOGIN:
						_iCmd=CMD_NULL;
						_iStatus=STATUS_LOGGED;
						RefreshStatus();
						break;
					case CMD_LOGOUT:
						_iCmd=CMD_NULL;
						_iStatus=STATUS_UNLOGGED;
						RefreshStatus();
						break;
					case CMD_CALL:
						_iCmd=CMD_NULL;
						if (ritorno[1]==1){
							LogInfo(_L("CALL ACCEPTED"));
							Accept(EFalse);
						}
						else{
							LogInfo(_L("CALL REJECTED"));
							Reject(EFalse);
						}
						break;
					case CMD_GET_USR_LST:
						_iCmd=CMD_NULL;					
						if (lenRitorno<_UsersLogged.MaxLength()){
							_UsersLogged.Copy(&ritorno[1], lenRitorno-1);
							_iEngine->ShowUsersLogged();
						}
						break;
					case CMD_CLOSE_CONV:						
						if (lenRitorno==1)	// filtro anti sfiga
							Reject(EFalse);
						break;
					case CMD_FRM_VIDEO:						
						p.Set(&ritorno[1], lenRitorno-1);
						if (_viewDefault->iAppView)
							_viewDefault->iAppView->DecodeAndShow(p, !_bSpeaking);		
						break;
					case CMD_FRM_AUDIO:
						if (!_bSpeaking && _viewDefault){
							_bSpeakingBoth=EFalse;
							p.Set(&ritorno[1], lenRitorno-1);
							//LogInfo(_L("frm audio size:%d"), lenRitorno-1);
							if (_viewDefault->_iCamera->iAudioEngine->CanPlay())
								_viewDefault->_iCamera->iAudioEngine->Play(p);
							else
								_viewDefault->_iCamera->iAudioEngine->OpenForPlay();							
						}
						else if (_bSpeaking)
							_bSpeakingBoth=ETrue;
							
						break;					
				}
			}
			else if (_iStatus!=STATUS_TALKING){
				if (iCmd==CMD_RING && _iStatus!=STATUS_INCOMING){
					_iEngine->StopLogin();
					LogInfo(_L("Incoming call _iStatus:%d"), _iStatus);
					if (lenRitorno<_User2Call.MaxLength())
					{	_User2Call.Copy(&ritorno[1], lenRitorno-1);
						_sUser2Call.Copy(_User2Call);
					}
					ShowInComing();
				}
				if (iCmd==CMD_RING)
					_iEngine->ResetRings();
			}
		
			lenRitorno=0;
			delete ritorno;
			ritorno=NULL;
			lunghezza=0;
		}
		else
		{	/*info.Format(_L("size %d - %d"), lenRitorno, _iCnt);
			if (_viewDefault->iAppView){
				_viewDefault->iAppView->SetInfo(info);
				_viewDefault->iAppView->DrawNow();
			}*/
		}
	} while (result);
}


// In caso di errore di sinc riporta 0 altrimenti riporta la lunghezza del pacchetto
unsigned short CDemoVideoCallAppUi::checkPktLen(unsigned short dataLenIn)
{
// Ricavo len e cks
   unsigned short cleanLength = dataLenIn & 0x1FFF;
   unsigned short cksLength = (cleanLength % 0x07) ^ 0x05;


//    return (cleanLength | ( cksLength << 13)) ;
   if ( dataLenIn == (cleanLength | ( cksLength << 13) ) ) return cleanLength;
   else {
//        printf("DISALLINEATO: %d [%x]\n", dataLenIn, dataLenIn);
// printf("DISALLINEATO: dataLenIn=%d cleanLength=%d cksLength=%d\n", dataLenIn, cleanLength, cksLength);

       return 0;
   }
}


// receive data from socket
void CDemoVideoCallAppUi::SocketReceiver(const TDesC8& aBuffer)
{	
	_bFlagRxData=true;
	TUint8* buffer = const_cast<TUint8*>(aBuffer.Ptr());
	TRAPD(error, CheckPackets(buffer, aBuffer.Length()));	
	if (error!=KErrNone)
		LogInfo(_L("errore CheckPackets:%d"), error);		
}
void CDemoVideoCallAppUi::SendCmd(int iCmd)
{
	TBuf<32> buf; 
	
	switch (iCmd){
		case CMD_RECORDER:
			SendCmd(iCmd, buf);
			break;
		case CMD_LOGOUT:
		case CMD_LOGIN:
			SendCmd(iCmd, iData->UserName());
			break;
		case CMD_CALL:
			SendCmd(iCmd, _sUser2Call);
			break;
		case CMD_GET_USR_LST:
			SendCmd(iCmd, buf);
			break;
		case CMD_RING:
			break;
		case CMD_CLOSE_CONV:
			SendCmd(iCmd, buf);
			break;
		case CMD_SETCODEC:			
			if (_viewDefault->_iCameraSelected == ECameraF){
				LogInfo(_L("Send Codec used:%d"), CODEC_ID_H263);
				buf.Append(CODEC_ID_H263);
			}				
			else{
				LogInfo(_L("Send Codec used:%d"), CODEC_ID_MPEG4);
				buf.Append(CODEC_ID_MPEG4);
			}
			SendCmd(iCmd, buf);
			break;
	}
}
void CDemoVideoCallAppUi::SendCmd(int iCmd, const TDesC& aBuffer)
{
	if (_iSck && !_iSck->Connected()){
		_iSck->ConnectL();
		return;
	}
	
	LogInfo(_L("SendCmd:%d"), iCmd);
	
	TUint8 byHeader[5];
	TInt iLen = aBuffer.Length()+1;
	switch (iCmd){
		case CMD_RECORDER:
			_BufferTX.Zero();
			byHeader[2] = iCmd; 
			SetCheckSum(byHeader, iLen, iCmd);
			_BufferTX.Append(byHeader, 3);				
			if (aBuffer.Length()>0)
				_BufferTX.Append(aBuffer);
			_iSck->WriteL(_BufferTX);
			_iCmd=iCmd;			
			break;
		case CMD_LOGOUT:
		case CMD_LOGIN:
			if (aBuffer.Length()>0){
				_BufferTX.Zero();
				byHeader[2] = iCmd; 
				SetCheckSum(byHeader, iLen, iCmd);
				_BufferTX.Append(byHeader, 3);				
				_BufferTX.Append(aBuffer);
				_iSck->WriteL(_BufferTX);
				_iCmd=iCmd;
			}
			break;
		case CMD_CALL:
			_BufferTX.Zero();
			byHeader[0] = iLen & 0x000000FF;
			byHeader[1] = (iLen>>8) & 0x000000FF;
			byHeader[2] = iCmd; 
			SetCheckSum(byHeader, iLen, (TUint8)iCmd);
			_BufferTX.Append(byHeader, 3);
			_BufferTX.Append(aBuffer);
			_iSck->WriteL(_BufferTX);
			_iCmd=iCmd;
			break;
		case CMD_GET_USR_LST:
			_BufferTX.Zero();
			iLen=1;
			byHeader[0] = iLen;
			byHeader[1] = 0;
			byHeader[2] = iCmd; 
			SetCheckSum(byHeader, iLen, (TUint8)iCmd);
			_BufferTX.Append(byHeader, 3);
			_iSck->WriteL(_BufferTX);
			_iCmd=iCmd;
			break;
		case CMD_RING:
			_BufferTX.Zero();
			iLen=2;
			byHeader[0] = iLen;
			byHeader[1] = 0;
			byHeader[2] = CMD_RING;
			SetCheckSum(byHeader, iLen, (TUint8)iCmd);
			byHeader[3] = aBuffer[0];
			_BufferTX.Append(byHeader, 4);
			_iSck->WriteL(_BufferTX);			
			break;
		case CMD_CLOSE_CONV:
			_BufferTX.Zero();
			iLen=1;
			byHeader[0] = iLen;
			byHeader[1] = 0;
			byHeader[2] = CMD_CLOSE_CONV;
			SetCheckSum(byHeader, iLen, (TUint8)iCmd);
			_BufferTX.Append(byHeader, 3);
			_iSck->WriteL(_BufferTX);			
			break;
		case CMD_SETCODEC:
			if (aBuffer.Length()>0){
				_BufferTX.Zero();
				byHeader[2] = iCmd; 
				SetCheckSum(byHeader, iLen, iCmd);
				_BufferTX.Append(byHeader, 3);				
				_BufferTX.Append(aBuffer);
				_iSck->WriteL(_BufferTX);
				_iCmd=iCmd;
			}
			break;
	}
}

void CDemoVideoCallAppUi::SendAudioFrame(TDesC8& aBuffer){
	if (_iStatus != STATUS_TALKING)
		return;
	
	TUint8 byHeader[3];
	_BufferTX.Zero();
	TInt iLen = aBuffer.Length()+1;
	byHeader[0] = iLen & 0x000000FF;
	byHeader[1] = (iLen>>8) & 0x000000FF;
	byHeader[2] = CMD_FRM_AUDIO;
	SetCheckSum(byHeader, iLen, CMD_FRM_AUDIO);
	_BufferTX.Append(byHeader, 3);
	_BufferTX.Append(aBuffer);
	_iSck->WriteL(_BufferTX);
}
void CDemoVideoCallAppUi::SendVideoFrame(TDesC8& aBuffer){

	if (_iStatus == STATUS_INCOMING || _iStatus == STATUS_OUTCOMING)
		return;
	else if (_iStatus == STATUS_LOGGED){
		LogInfo(_L("CDemoVideoCallAppUi::SendVideoFrame and STATUS_LOGGED"));		
		return;
	}
	TUint8 byHeader[3];
	_BufferTX.Zero();
	TInt iLen = aBuffer.Length()+1;
	byHeader[0] = iLen & 0x000000FF;
	byHeader[1] = (iLen>>8) & 0x000000FF;
	byHeader[2] = CMD_FRM_VIDEO;
	SetCheckSum(byHeader, iLen, CMD_FRM_VIDEO);
	_BufferTX.Append(byHeader, 3);
	_BufferTX.Append(aBuffer);
	_iSck->WriteL(_BufferTX);
	//LogInfo(_L("Frame Size:%d"), iLen);
}
void CDemoVideoCallAppUi::RefreshStatus(){
	if (_iSck && !_iSck->Connected())
		_iStatus=STATUS_UNLOGGED;
	if (_viewDefault)
		_viewDefault->RefreshDisplay();
}

void CDemoVideoCallAppUi::GetUsersLogged(){
	SendCmd(CMD_GET_USR_LST);
}

void CDemoVideoCallAppUi::ShowUsersLogged(){
	TInt selectedOption( 0 );
	TBuf<64> user;
	_BufferTX.Copy(_UsersLogged);
	
	LogInfo(_L("CDemoVideoCallAppUi::ShowUsersLogged:%d"), _BufferTX.Length());

	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(100);
	CleanupStack::PushL(array);

	TInt iSep;
	do
	{	iSep = _BufferTX.Locate('|');
		if (iSep>=0){
			user.Copy(_BufferTX.Left(iSep));
			_BufferTX = _BufferTX.Mid(iSep+1);
		}
		else
			user.Copy(_BufferTX);
		if (user.Length()>0 && user.Compare(iData->UserName()))
			array->AppendL(user);
	}while (iSep>=0);
	
	if (array->Count()>0){
		CAknListQueryDialog* dlg = new(ELeave) CAknListQueryDialog( &selectedOption );
		dlg->PrepareLC(R_UPDATE_QUERY_DIALOG);
		dlg->SetItemTextArray(array);
		dlg->SetOwnershipType(ELbmDoesNotOwnItemArray);
		TInt iRet = dlg->RunLD();
		
		if (iRet==EAknSoftkeyOk){
			GetUser(selectedOption);
			_iEngine->MakeCall();
			StartCamera(ETrue);
		}
	}
	else
		ShowMessage(_L("Not users available to call"), 0);
	
	CleanupStack::PopAndDestroy(); //array
}
void CDemoVideoCallAppUi::GetUser(TInt iIdxUsr){
	TBuf<64> user;
	TInt iCnt=0, iSep;
	_BufferTX.Copy(_UsersLogged);
	_User2Call.Zero();
	LogInfo(_L("CDemoVideoCallAppUi::GetUser"));
	do
	{
		iSep = _BufferTX.Locate('|');
		if (iSep>=0){
			user.Copy(_BufferTX.Left(iSep));
			_BufferTX = _BufferTX.Mid(iSep+1);
		}
		else
			user.Copy(_BufferTX);
		if (user.Compare(iData->UserName())){
			if (iCnt==iIdxUsr){
				_User2Call.Copy(user);
				_sUser2Call.Copy(_User2Call);
			}
			iCnt++;
		}
	}while (iSep>=0 && _User2Call.Length()<=0);
}

void CDemoVideoCallAppUi::MakeCall(){
	SendCmd(CMD_CALL);
}
void CDemoVideoCallAppUi::ShowOutComing(){
	_iStatus=STATUS_OUTCOMING;
	if (_viewDefault)
		_viewDefault->RefreshDisplay();
	CEikButtonGroupContainer* iCba = CEikButtonGroupContainer::Current();
	iCba->SetCommandSetL(R_OUTCOMING_CBA_BUTTON);
	iCba->DrawNow();
}
void CDemoVideoCallAppUi::ShowInComing(){
	PlayRing();
	if (_iStatus!=STATUS_INCOMING){
		_iStatus=STATUS_INCOMING;
		LogInfo(_L("CDemoVideoCallAppUi::ShowInComing()"));
		_iEngine->InComingCall();
	}
	if (_viewDefault)
		_viewDefault->RefreshDisplay();
	CEikButtonGroupContainer* iCba = CEikButtonGroupContainer::Current();
	iCba->SetCommandSetL(R_INCOMING_CBA_BUTTON);
	iCba->DrawNow();	
}

void CDemoVideoCallAppUi::Accept(TBool bSendCmd){	
	TBuf<10> buf;	
	_iStatus=STATUS_TALKING;
	LogInfo(_L("CDemoVideoCallAppUi::Accept"));
	SendCmd(CMD_SETCODEC);

	if (_iRingTone)_iRingTone->Stop();
	if (bSendCmd){
		buf.Append(0);
		buf[0]=1;
		SendCmd(CMD_RING, buf);
	}
	if (_viewDefault)
		_viewDefault->RefreshDisplay();
	CEikButtonGroupContainer* iCba = CEikButtonGroupContainer::Current();
	iCba->SetCommandSetL(R_CLOSECONVERSATION_CBA_BUTTON);
	iCba->DrawNow();	
}
void CDemoVideoCallAppUi::Reject(TBool bSendCmd)
{	
	TBuf<10> buf;
	buf.Append(0);
	buf[0]=0;
	LogInfo(_L("CDemoVideoCallAppUi::Reject"));
	switch(_iStatus)
	{
		case STATUS_OUTCOMING:
			_iStatus=STATUS_LOGGED;
			_iEngine->StopCall();
			StartCamera(EFalse);
			SendCmd(CMD_RING, buf);
			if (_iRingTone)_iRingTone->Stop();
			break;
		case STATUS_INCOMING:	
			_iStatus=STATUS_LOGGED;			
			_iEngine->StopCall();
			StartCamera(EFalse);
			SendCmd(CMD_RING, buf);
			if (_iRingTone)_iRingTone->Stop();
			break;
		case STATUS_TALKING:
			_iStatus=STATUS_LOGGED;
			_iEngine->StopCall();
			StartCamera(EFalse);
			if (bSendCmd)
				SendCmd(CMD_CLOSE_CONV);						
			break;
	}	
	
	if (_viewDefault)
		_viewDefault->RefreshDisplay();
	CEikButtonGroupContainer* iCba = CEikButtonGroupContainer::Current();
	iCba->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
	iCba->DrawNow();
}

void CDemoVideoCallAppUi::MapcInitComplete(TInt aError,const TTimeIntervalMicroSeconds &/*aDuration*/)
{
    if(aError==KErrNone)
    	_iRingTone->SetVolume(_iRingTone->MaxVolume()/2);
}

void CDemoVideoCallAppUi::MapcPlayComplete(TInt /*aError*/)
{
	_bPLaying=EFalse;
}
void CDemoVideoCallAppUi::PlayRing(){
	if(!_bPLaying && _iRingTone){
		_bPLaying=ETrue;
		_iRingTone->Play();
	}
}
void CDemoVideoCallAppUi::StartCamera(TBool bEnable){
	if (iData->Mode()==MODE_AUDIOVIDEO || iData->Mode()==MODE_VIDEO){
		LogInfo(_L("CDemoVideoCallAppUi::StartCamera:%d"), bEnable);
		if (_viewDefault)
			_viewDefault->StartCamera(bEnable);
	}
}

void CDemoVideoCallAppUi::SetCheckSum(TUint8 byHeader[], unsigned short iLen, TUint8 cmd)
{
	unsigned short iChkSum = ((iLen & 0x1FFF)% 0x07) ^ 0x05;
	byHeader[0] = iLen & 0x00FF;
	byHeader[1] = (iLen>>8) & 0x00FF;				
	byHeader[1] = (iChkSum<<5)|byHeader[1];
	byHeader[2] = cmd;
}

void CDemoVideoCallAppUi::BufferingSizeInfo(int iBuffSize)
{
	if (_viewDefault->iAppView)
		_viewDefault->iAppView->Buffering(iBuffSize);
}

/** 
 * Override of the HandleKeyEventL virtual function
 * @return EKeyWasConsumed if event was handled, EKeyWasNotConsumed if not
 * @param aKeyEvent 
 * @param aType 
 */
TKeyResponse CDemoVideoCallAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse iRet=EKeyWasNotConsumed;
	
	if (aType==EEventKey)
		_iKey=aKeyEvent.iCode;
	
	switch (_iKey){
	case EKeyUpArrow:
		if (aType==EEventKeyUp && _iStatus==STATUS_TALKING && iData->Mode()==MODE_AUDIOVIDEO){
			_viewDefault->_iCamera->IncreaseBitRate();			
		}
		break;
	case EKeyDownArrow:
		if (aType==EEventKeyUp && _iStatus==STATUS_TALKING && iData->Mode()==MODE_AUDIOVIDEO){
			_viewDefault->_iCamera->DecreaseBitRate();			
		}
		break;
	case EKeyDevice3:
		if (_iStatus==STATUS_TALKING && (iData->Mode()==MODE_AUDIOVIDEO || iData->Mode()==MODE_AUDIO)){
			//LogInfo(_L("_bSpeaking %d"), _bSpeaking);
			if (_iStatusKey==EEventKeyDown && aType!=EEventKeyUp && _viewDefault){
			_viewDefault->_iCamera->iAudioEngine->Stop();
				_viewDefault->_iCamera->iAudioEngine->Record();
				_bSpeaking=true;
			}
			else if (aType==EEventKeyUp &&  _viewDefault){
				_viewDefault->_iCamera->iAudioEngine->Stop();
				_iKey=0;
				_bSpeaking=false;
			}			
			if (_viewDefault->iAppView)
				_viewDefault->iAppView->DrawNow();			
			iRet=EKeyWasConsumed;
		}
		break;	
	}
	if (aType==EEventKeyDown || EEventKeyUp){
		_iStatusKey=aType;
	}
	if (_iKey==EKeyDevice3 && aType==EEventKeyUp)
		_bSpeakingBoth=EFalse;			

	return iRet;
}


	
// End of File



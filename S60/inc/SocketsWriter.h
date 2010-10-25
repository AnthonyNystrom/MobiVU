

#ifndef __SOCKETSWRITER_H__
#define __SOCKETSWRITER_H__

#include <in_sock.h>
#include "TimeOutNotifier.h"
#include <f32file.h>
#include "DemoVideoCallAppUi.h"
class CTimeOutTimer;
class MEngineNotifier;
class CDemoVideoCallAppUi;
/*! 
  @class CSocketsWriter
  
  @discussion This class handles writing data to the socket.
  Data to be written is accumulated in iTransferBuffer, and is then transferred
  to iWriteBuffer for the actual write to the socket.
  */
class CSocketsWriter : public CActive, public MTimeOutNotifier
    {
public:
/*!
  @function NewL
   
  @discussion Create a CSocketsWriter object
  @param aEngineNotifier An observer for status reporting
  @param aSocket socket to write to
  @result a pointer to the created instance of CSocketsWriter
  */
    static CSocketsWriter* NewL(MEngineNotifier& aEngineNotifier, RSocket& aSocket);

/*!
  @function NewLC
   
  @discussion Create a CSocketsWriter object
  @param aEngineNotifier An observer for status reporting
  @param aSocket socket to write to
  @result a pointer to the created instance of CSocketsWriter
  */
	static CSocketsWriter* NewLC(MEngineNotifier& aEngineNotifier, RSocket& aSocket);

/*!
  @function ~CSocketsWriter
  
  @discussion Destroy the object and release all memory objects
  */
	~CSocketsWriter();

/*!
  @function IssueWrite

  @discussion Write the data to the socket (buffered)
  @param aData the data to be written
  */
    void IssueWriteL(const TDesC8& aData);

public: // From MTimeOutNotifier
/*!
  @function TimerExpired
  
  @discussion Handle a timeout event
  */
	void TimerExpired(); 
	void TestBW();
	void CreateFileBuffer();
	void DeleteFileBuffer();

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

private:
/*!
  @function CSocketsWriter
  
  @discussion Perform the first phase of two phase construction 
  @param aEngineNotifier An observer for status reporting
  @param aSocket socket to read from
  */
	CSocketsWriter(MEngineNotifier& aEngineNotifier, RSocket& aSocket);

/*!
  @function ConstructL
  
  @discussion Perform the second phase construction of a CSocketsWriter 
  */
	void ConstructL();

/*!
  @function SendNextPacket

  @discussion Handle a 'write buffer empty' situation.
  */    
    void SendNextPacket();

private: // Member variables

    /*! @const The size of the write buffer in bytes */
    enum {KWriteBufferSize = 5000 };
    enum {KWriteBufferSizeTest = 10000 };

    /*! @const The maximum time allowed for a write to complete */
    static const TInt KTimeOut;
 
    /*!
      @enum TWriteState

      @discussion Records whether a write request is pending
      @value ESending A write request is pending with the socket server
      @value EWaiting The idle state for this object
      */
	enum TWriteState 
	    {
		ESending, 
        EWaiting
	    };

    /*! @var The socket to write to */
    RSocket&                iSocket;

    /*! @var An observer for status reporting */
	MEngineNotifier&        iEngineNotifier;

    /*! @var Accumulate data to send in here */
	TBuf8<KWriteBufferSize> iTransferBuffer;
	TBuf8<KWriteBufferSize> iTransferBuffer2;
    TBuf8<KWriteBufferSizeTest> iBufBW;


    /*! @var Holds data currently being sent to socket */
	TBuf8<KWriteBufferSize> iWriteBuffer;
	TBool bWaitBuffering;

    /*! @var A timer used to cancel an outstanding write after a predefined timeout */
	CTimeOutTimer*          iTimer;

    /*! @var The timeout to use */
	TInt                    iTimeOut;

	CDemoVideoCallAppUi *iAppUi;
    /*! @var The state of this active object */
	TWriteState             iWriteStatus;
	RFile 					*fileBuffer;
	TBuf<200> 				m_filenameBuffer;
	int						m_iSizeFile;
	int						m_iPosRead;
    };

#endif // __SOCKETSWRITER_H__


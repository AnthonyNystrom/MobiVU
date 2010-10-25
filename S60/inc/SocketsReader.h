

#ifndef __SOCKETSREADER_H__
#define __SOCKETSREADER_H__

#include <in_sock.h>

class MEngineNotifier;

/*! 
  @class CSocketsReader
  
  @discussion This class handles reading data from the socket.
  In this implementation, any data read is simply displayed as text on the console.
  */
class CSocketsReader : public CActive
    {
public:
/*!
  @function NewL
   
  @discussion Create a CSocketsReader object
  @param aEngineNotifier An observer for status reporting
  @param aSocket socket to read from
  @result a pointer to the created instance of CSocketsReader
  */
	static CSocketsReader* NewL(MEngineNotifier& aEngineNotifier, RSocket& aSocket);

/*!
  @function NewLC
   
  @discussion Create a CSocketsReader object
  @param aEngineNotifier An observer for status reporting
  @param aSocket socket to read from
  @result a pointer to the created instance of CSocketsReader
  */
    static CSocketsReader* NewLC(MEngineNotifier& aEngineNotifier, RSocket& aSocket);

/*!
  @function ~CSocketsReader
  
  @discussion Destroy the object and release all memory objects
  */
	~CSocketsReader();

/*!
  @function Start

  @discussion Initiate a read from socket
  */
	void Start();

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
  @function CSocketsReader
  
  @discussion Perform the first phase of two phase construction 
  @param aEngineNotifier An observer for status reporting
  @param aSocket socket to read from
  */
	CSocketsReader(MEngineNotifier& aEngineNotifier, RSocket& aSocket);

/*!
  @function ConstructL
  
  @discussion Perform the second phase construction of a CSocketsReader 
  */
	void ConstructL();

/*!
  @function IssueRead

  @discussion Initiate a read from socket
  */
	void IssueRead();

private: // Member variables

    /*! @const The size of the read buffer in bytes */
    enum {KReadBufferSize = 100000};

    /*! @var socket to read data from */
    RSocket&                iSocket;

    /*! @var An observer for status reporting */
	MEngineNotifier&        iEngineNotifier;

    /*! @var buffer for receiving data */
	TBuf8<KReadBufferSize>  iBuffer;

    /*! @var dummy - length of data read is written here */
    TSockXfrLength          iDummyLength;
    };

#endif // __SOCKETSREADER_H__


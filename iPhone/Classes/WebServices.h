//
//  WebServices.h
//  Pikling
//
//  Created by acerone on 4/30/09.
//  Copyright 2009 7touch Group. All rights reserved.
//

#import <Foundation/Foundation.h>


#define kM2MServerIp @"69.21.114.135"
#define kM2MServerPort 8082
#define kMaxPacketSize 4096

 enum {
	 kWSIdleState = 0xFF,
	 kWSLogin     =  0,
	 kWSLogout    =  1,
	 kWSCall      =  2,
	 kWSRing      =  3
 };
 typedef uint8_t WSCommand;


@protocol WebServicesDelegate;


@interface WebServices : NSObject {
	id <WebServicesDelegate> delegate;

	NSOutputStream *oStream;
	NSInputStream *iStream;

	uint byteIndex;
	NSData *txData; // Buffer contenente il pacchetto da trasmettere (TX)
	NSMutableData *receivedPacket; // Buffer contenente il pacchetto in arrivo (RX)
	NSMutableData *bufferNewPkt; // Buffer contenente gli avanzi di un flusso


//	NSData *header; // Dimensione dell'immagine da spedire
//	NSString *richiesta;

	bool isConnected; // Indica se ho aperto lo streaming verso il server
//	WSCommand commandPending; // Indica se ho dei comandi pendenti di cui sto apesttando la risposta
	
	// Variabili di gestione delle MAS
//	WSMasEvent uploadImageMAS;	// MAS per l'upload dell'immagine da OCRizzare
//	WSMasEvent uploadRequestMAS;	// MAS per la richiesta di invio risposta a email
	BOOL errorAlreadySent;		// Flag per per limitare la generazione continua di errori di comunicazione
	BOOL streamError;
}

@property (nonatomic, assign) NSInputStream * iStream;
@property (nonatomic, assign) NSOutputStream * oStream;
@property(nonatomic, assign) id <WebServicesDelegate> delegate;
//@property(nonatomic, retain) NSData *header;
@property(nonatomic, retain) NSData *txData;

- (NSData *)receiveData ;

@end


@protocol WebServicesDelegate
 @required
    -(void) WebServicesDidFinish;
    -(void) processIncominPacket:(NSData *)receivedPacket;

 @optional
    -(void) streamEventErrorOccurred:(NSString *)errorString;
    -(void) connectionStatusDidChanged:(bool)connected;
    -(void) hasSpaceAvailable:(NSStream *)stream;
@end

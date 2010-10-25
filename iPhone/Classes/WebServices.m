//
//  WebServices.m
//  Pikling
//
//  Created by acerone on 4/30/09.
//  Copyright 2009 7touch Group. All rights reserved.
//

#import "WebServices.h"
#import "M2M___ProtocolAppDelegate.h"

unsigned char *ritorno=nil;
uint16_t lenRitorno=0;

void *xmalloc(int size) {                                                                                
	void *mem = malloc(size);                                                                             
	if (mem == NULL) {                                                                                    
		fprintf(stderr, "ERROR: xmalloc %d\n", size);                                                      
		exit(1);                                                                                           
	}                                                                                                     
	return mem;                                                                                           
}       

int16_t checkPktLen(uint16_t dataLenIn, unsigned char *comando)                                                                                            
{                                                                                                                                                           
	// Ricavo len e cks                                                                                                                                     
    uint16_t cleanLength = dataLenIn & 0x1FFF;                                                                                                              
    unsigned char cksLength = (cleanLength % 0x07) ^ 0x05;                                                                                                  
	
    if ( dataLenIn == (cleanLength | ( cksLength << 13) ) ) {                                                                                               
//		printf("%04x %02x [%04x] [%02x] \n", dataLenIn, *comando, dataLenIn & 0x1ff, *comando & 0x0F);                                                              
        return cleanLength;                                                                                                                                 
    } else {                                                                                                                                                
        return 0;                                                                                                                                           
    }                                                                                                                                                       
}                                                                                                                                                           


bool GiveMeAPacket(unsigned char *pbufInput, uint16_t iSizeInput, unsigned char **pBuffOutput, uint16_t *piSizeOutput)
{
    static unsigned char *bufferNewPkt;
    static uint16_t lenNewPkt;
    static uint16_t pktDataLen; // Lunghezza del pacchetto in arrivo
    uint16_t newSize;

    uint16_t pktTail = *piSizeOutput; // Lunghezza del buffer di uscita prima dell'elaborazione di questo flusso in ingresso

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
            free(bufferNewPkt);
            bufferNewPkt=NULL;
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
                pktDataLen = checkPktLen( *(uint16_t *)inizioPtr, inizioPtr+2 );

                // Sono disallineato, inizia a cercare il prossimo pacchetto
                if ( !pktDataLen ) {
                    inizioPtr++;
                    iSizeInput--;

                    // In questo flusso no trovo nulla di interessante quindi butto via tutto
                    if ( iSizeInput < 2) {
//                        printf("!!! AHIA !!!\n");
			if (pbufInput2 != NULL) { free(pbufInput2); }
			pbufInput2=NULL;
			
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
            uint16_t skip=1;

            // Compongo la lunghezza del pacchetto prendendone un p√≤ qua e la
            pktDataLen = checkPktLen( (*pbufInput << 8 ) + (**pBuffOutput), *pBuffOutput+1 );

            // Sono disallineato
            if (!pktDataLen) {
                unsigned char *inizioPtr = pbufInput;

                do {
                    // Memorizzo la lunghezza del pacchetto in arrivo
                    pktDataLen = checkPktLen( *(uint16_t *)inizioPtr, inizioPtr+2 );

                    // Sono disallineato, inizia a cercare il prossimo pacchetto
                    if ( !pktDataLen ) {
                        inizioPtr++;
                        iSizeInput--;

                        // In questo flusso no trovo nulla di interessante quindi butto via tutto
                        if ( iSizeInput <= 2) {
			    if (pbufInput2 != NULL) { free(pbufInput2); }
			    pbufInput2=NULL;
			    
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
            free(*pBuffOutput);
	    *pBuffOutput=NULL;

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
            free(*pBuffOutput);
	    *pBuffOutput=NULL;

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
            if (lenNewPkt>0) free(bufferNewPkt);
            bufferNewPkt=NULL;
//            free(bufferNewPkt);
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
	
	if (pbufInput2 != NULL) { free(pbufInput2); }
	
        pktDataLen=0;
        return true;
    } else {
	if (pbufInput2 != NULL) { free(pbufInput2); }
        // Il pacchetto non √® ancora finito
        return false;
    }
}



@implementation WebServices

@synthesize iStream, oStream, delegate;
@synthesize txData;

//@synthesize header;


//
// Apro la connessione con il server
//
- (void)openConnection
{
    streamError = NO;
//  commandPending = kWSIdleState;
    isConnected = NO;

    bufferNewPkt=nil;
    receivedPacket=nil;

    // Attivo la connessione con il server
    NSHost* host;
    host = [NSHost hostWithAddress:kM2MServerIp];
    if( host ) {
        [NSStream getStreamsToHost:host port:kM2MServerPort inputStream:&iStream outputStream:&oStream] ;
        if(self.iStream && self.oStream) {
            self.iStream.delegate = self;
            [self.iStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
            [self.iStream open];

            self.oStream.delegate = self;
            [self.oStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
            [self.oStream open];
        }
    }

//  [UIApplication sharedApplication].networkActivityIndicatorVisible = YES;

    // Inizializzo le MAS

    // Azzero il flag che vincola la generazione dei messaggi di errore
    // Al primo errore segnalo l'anomalia e chiuso la connessione
    errorAlreadySent=NO;
}


//
// Chiudo la connessione col server
//
- (void) closeConnection
{
    // Chiudo gli stream
    [self.iStream close];
//  [self.iStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
//  [self.iStream release];

    [self.oStream close];
//  [self.oStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
//  [self.oStream release];

    isConnected=NO;

//  [UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
}


//
// Genero il segnale di errore e chiudo la connessione con il server
//
- (void) errorOccuredWithString:(NSString *)errorDescription
{
    if (!errorAlreadySent) {
        errorAlreadySent=YES;

        // Segnalo l'errore
        if ([self _isValidDelegateForSelector:@selector(streamEventErrorOccurred:)]) [delegate streamEventErrorOccurred:errorDescription];

        // Chiudo la connessione con il server
        [self closeConnection];
    }
}


- (NSData *)receiveData {
    int len = 0;
    size_t lengthByte = kMaxPacketSize;
    NSMutableData *retBlob = nil;

//  retBlob = [NSMutableData dataWithLength:lengthByte];
    retBlob = [[NSMutableData alloc] initWithLength:lengthByte];

    len = [self.iStream read:(uint8_t *)[retBlob mutableBytes] maxLength:lengthByte];
    //NSLog(@"Stream read: [%d bytes] %@", len, [retBlob description]);

    if (len > 0) [retBlob setLength:len];
    else retBlob = nil;

    return retBlob;
}


- (void)sendCommand:(WSCommand)cmd withData:(NSData *)_data
{
    // Se non sono connesso non faccio niente
    if ( !isConnected ) {
//  if ( [self.oStream streamStatus] != kCFStreamStatusOpen ) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Attention" message:@"iPhone is disconnected from server" delegate:self cancelButtonTitle:@"OK" otherButtonTitles: nil];
        [alert show];
        [alert release];
        return;
    }

    // Voglio trasmettere un comando e ho ancora dei pacchetti pendenti
/*  if ( commandPending != kWSIdleState ) {
        NSLog(@"sendCommand: invio comando con un comando pendente [%d]", commandPending);
        M2M___ProtocolAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
        [_delegate playSound:@"/Pikling Tap -8db.mp3" withDelta:-0.03f];
    }
*/
        uint8_t *readBytes = (uint8_t *)[_data bytes];
        uint16_t data_len = (uint16_t)[_data length]+1;
#ifdef NEW_CHECKSUM
    // Calcolo la checksum
    uint8_t cks = ((data_len & 0x1FFF) % 7) ^ 0x5);
    uint16_t lenECks = (( data_len & 0x1FFF) | (( cks & 0x70 ) << 9));
    uint8_t comandoECks = (cmd & 0x0F) | ((cks & 0x0F) << 4);
#else
    // Calcolo la checksum
    uint8_t cks = (((data_len & 0x1FFF) % 7) ^ 0x5A);
    uint16_t lenECks = (( data_len & 0x1FFF) | ( cks << 13));
    uint8_t comandoECks = cmd;
#endif
    // Verifico di non esagerare con il buffer ||| rivederla
    unsigned int len = ((data_len >= kMaxPacketSize) ? kMaxPacketSize : data_len);

    uint8_t buf[len+3];

    // Metto l'intestazione (lunghezza pacchetto, comando e relativa cks)
    buf[0] = lenECks & 0xFF;
    buf[1] = (lenECks>>8) & 0xFF;
    buf[2] = comandoECks;

    // Accodo i dati
    (void)memcpy(buf+3, readBytes, len);

    len = [self.oStream write:(const uint8_t *)buf maxLength:len+2];
    //    byteIndex += len;
    NSString *dettagli=[[[NSString alloc] initWithData:_data encoding:NSUTF8StringEncoding] autorelease];
//NSLog(@"sendData command:%d length:%d  %@", cmd, len, dettagli );
//NSLog(@"sendData command:%d length:%d  %@", cmd, len, [_data description]);

    // Verifico di aver trasmesso tutto il pacchetto in modo da poter distruggere txData
    if (len == data_len+2) txData=nil;
    else NSLog(@"sendCommand: not all data are sent %d != %d", len, data_len+2);

    // Tendo traccia del pacchetto che ho inviato
//  commandPending = cmd;
}


- (BOOL) _isValidDelegateForSelector:(SEL)selector
{
    return ((delegate != nil) && [delegate respondsToSelector:selector]);
}

/*
//
// Verifico il pacchetto, riporto YES se e' completo e metto il pacchetto in outData
//
-(bool) GiveMeAPacket:(NSData *)inData withData:(NSMutableData**)outData
{
   static uint16_t lunghezzaPacchetto;

//    static uint16_t lenNewPkt;

    NSData *_inData=nil;

NSLog(@"GiveMeAPacket:: dentro con IN %d e OUT %d", [inData length], [*outData length]);
// Controllo di sicurezza correttezza operazioni
if ((bufferNewPkt != nil) && (inData != nil)) {
    NSLog(@"!!! ERRORE !!!!!! in=%d e buf=%d bytes", [inData length], [bufferNewPkt length]);
}

    // Devo recuperare gli avanzi del flusso precedente
//  if ( ![inData length] ) {
    if ( inData==nil ) {
        NSLog(@"GiveMeAPacket:: entrati 0 bytes");

        // Devo recuperare gli avanzi del flusso precedente
            // Copio la roba rimasta dal vecchio flusso come nuova roba in ingresso
//          if ( [bufferNewPkt length] ) {
            if ( bufferNewPkt != nil  ) {
NSLog(@"Prendo roba dal buffer [%d bytes]", [bufferNewPkt length]);
                _inData = [[[NSData alloc] initWithData:bufferNewPkt] autorelease];

        	// Pulisco il buffer degli avanzi di flusso
                [bufferNewPkt release];
                bufferNewPkt=nil;
            } else {
NSLog(@"buffer vuoto");
                // Nel buffer precedente non mi ‚àö√£ rimasto niente del nuovo pacchetto
            	lunghezzaPacchetto=0;
        	*outData = nil;

                return false;
            }
        } else {
//NSLog(@"0 %@", [inData description]);
            // Ho dati in entrata quindi analizzo quelli
    	    _inData = [[[NSData alloc] initWithData:inData] autorelease];
if ( [_inData length] < 2) {
NSLog(@"!!!!! POCHI DATI IN INGRESSO %d", [_inData length]);
// Al momento butto via sta roba ||| poi la dovrÚ gestire
return false;
}
        }
NSLog(@"1 %d", [_inData length]);

    // E' l'inizio di un nuovo pacchetto e non ho idea di quanto sara' lungo il prossimo
    if ( (![*outData length]) && (!lunghezzaPacchetto) ) {
//      NSLog(@"2a");
    		uint8_t packetOffset=0;

		// Estraggo le info per il calcolo della checksum e della lunghezza del pacchetto
		uint8_t *readBytes = (uint8_t *)[_inData bytes];

        // Nei dati in ingresso sono presenti anche i 2 byte che indicano la lunghezza del pacchetto
        if ( !lunghezzaPacchetto ) {
            uint16_t tempValue= *readBytes | ( *(readBytes+1) << 8 ) ;
            //NSLog(@"%@ [%x]", [_inData description], tempValue);

            // Controllo la checksum e la lunghezza
            //lunghezzaPacchetto = [self checkPktLen:tempValue withCommand:(readBytes + 2)];
            lunghezzaPacchetto = [self checkPktLen:tempValue withCommand:nil];

if (lunghezzaPacchetto < 2) {
	if ([_inData length] > 2)NSLog(@"!!!!!!!!!!!!! SONO DISALLINEATO !!!!!!!  tempValue:%d [0x%x] comando %d", tempValue, tempValue, *(readBytes+2));
	else NSLog(@"!!!!!!!!!!!!! SONO DISALLINEATO !!!!!!!  tempValue:%d [ox%x]", tempValue, tempValue);
	
lunghezzaPacchetto=0;
// Al momento butto via sta roba ||| poi la dovrÚ gestire
return false;
}
            packetOffset = 2;
        }

NSLog(@"2");
        // Pacchetto perfetto - nessun avanzo fa gestire
        if  (packetOffset + lunghezzaPacchetto == [_inData length]) {
            // Copio i dati in uscita
            NSMutableData *uscita = [[NSMutableData alloc] initWithBytes:(readBytes+packetOffset) length:(NSUInteger)(lunghezzaPacchetto) ];
            *outData = uscita;
			lunghezzaPacchetto=0;
            //NSLog(@"GiveMeAPacket:: OK con dati: %d [%d]", [uscita length], lunghezzaPacchetto);
NSLog(@"3");
            return YES;
        } else {
            // Il pacchetto e' bello lungo quindi salvo tutto
            if  ( packetOffset + lunghezzaPacchetto > [_inData length]) {
                NSLog(@"GiveMeAPacket:: PACCHETTO LUNGO: %d != %d nel buffer", lunghezzaPacchetto+packetOffset, [_inData length]);
                // Copio i dati in uscita
                NSMutableData *uscita = [[NSMutableData alloc] initWithBytes:([_inData bytes]+packetOffset) length:[_inData length]-packetOffset ];
                *outData = uscita;

                // Il pacchetto NON √ã ancora completo
                return NO;
            } else {
NSLog(@"GiveMeAPacket:: PACCHETTO CORTO: %d != %d nel buffer", lunghezzaPacchetto+2, [_inData length]);
                if ([_inData length] >= 2) {
                    // Il pacchetto e' corto quindi salvo gli avanzi
                    // Copio i dati in uscita
                    NSMutableData *uscita = [[NSMutableData alloc] initWithBytes:(readBytes+packetOffset) length:(NSUInteger)(lunghezzaPacchetto) ];
                    *outData = uscita;

                    // Salvo gli avanzi
                    bufferNewPkt = [[NSData alloc] initWithBytes:(readBytes+packetOffset+lunghezzaPacchetto) length:([_inData length]-lunghezzaPacchetto-packetOffset) ];
                    NSLog(@"PACCHETTO CORTO exit: %d bytes, nel buffer [%d] '%@'", [*outData length], [bufferNewPkt length], [*outData description]);

					lunghezzaPacchetto=0;
					
                    // Il pacchetto √ã pronto per essere parserato
                    return YES;
                } else {
                    NSLog(@"CASO SFIGATISSIMO  len in:%d out:%d buffer:%d", [_inData length], [*outData length], [bufferNewPkt length]);
                    // Il pacchetto √ã pronto per essere parserato

                    return NO;// |||| Da rivedere
                }
            }
        }
    } else {
NSLog(@"GiveMeAPacket:: outData non era vuoto ||| TODO");
        // Estraggo le info per il calcolo della checksum e della lunghezza del pacchetto
        uint8_t *readBytes = (uint8_t *)[_inData bytes];


        // Verifico se con la roba in ingresso riesco a completare sto pacchetto
        if ( [*outData length] + [_inData length] >= lunghezzaPacchetto) {
            // Completo il pacchetto
            NSData *finale=[[[NSData alloc] initWithBytes:readBytes length:(lunghezzaPacchetto - [*outData length])] autorelease];
            [*outData appendData:finale];

            // Salvo l'eventuale resto
            if ( [_inData length] > [finale length] ) {
            // Salvo gli avanzi
            bufferNewPkt = [[NSData alloc] initWithBytes:(readBytes+[finale length]) length:([_inData length]-[finale length])];
            }
			
			lunghezzaPacchetto=0;
			
            // Il pacchetto √ã pronto per essere parserato
            return YES;
        } else {
            // Non √ã ancora sufficiente

            // Copio i dati in uscita
            [*outData appendData:_inData];

            // Il pacchetto NON √ã ancora completo
            return NO;
        }
    }
}
*/

#undef NEW_CHECKSUM
-(uint16_t) checkPktLen:(uint16_t)dataLenIn withCommand:(uint8_t *)comando
{
#ifdef NEW_CHECKSUM
    // Ricavo len e cks
    uint16_t cleanLength = dataLenIn & 0x1FFF;
    uint8_t cksLength = ((cleanLength % 128) ^ 0x5A) & 0x7F; // Checksum ricavata
    uint8_t cksReceived = (((dataLenIn >> 9) & 0x70) | ((*comando >> 4) & 0x0F)); // Checksum ricevuta

NSLog(@"%x %x [%x %x %x]", dataLenIn, *comando, cleanLength, cksLength, cksReceived );
    if ( cksLength != cksReceived ) {
        //wrong++;
        return 0;
    }
//    if ( !(*comando & 7) ) { wrong++; return 0; }

    if (cleanLength) {
//good++;
    return cleanLength;
    } else {
//wrong++   ;
    return 0;
    }
#else
    // Con questo metodo il dato: comando non viene usato e vale nil
    uint8_t cleanLength = dataLenIn & 0x1FFF;
    uint8_t cksLength = ((cleanLength % 7) ^ 0x05); // Checksum ricavata

    if (dataLenIn == (cleanLength | (cksLength << 13))) return cleanLength;
    else return  0;
#endif
}

@end


@implementation WebServices (NSStreamDelegate)
- (void)stream:(NSStream *)stream handleEvent:(NSStreamEvent)eventCode
{
//NSLog(@"NSStreamEvent::handleEvent codice:%d", eventCode);
    switch(eventCode) {
        case NSStreamEventHasSpaceAvailable: // TX
//          NSLog(@"NSStreamEventHasSpaceAvailable (TX)");
            // Genero un segnale per avvisare cheho spazio nel buffer di trasmissione
            if ([self _isValidDelegateForSelector:@selector(hasSpaceAvailable:)]) [delegate hasSpaceAvailable:stream];
            break;

        case NSStreamEventHasBytesAvailable: // RX
        {
            if(stream == self.iStream) {
//              NSLog(@"NSStreamEventHasBytesAvailable (RX)");

                bool result;
                NSData *inData=[self receiveData]; // Ritorna nil se non ho dati
//              NSLog(@"presenti:  %d", [inData length]);

//              if ( ![inData length] ) return;
                do {
//                    result=[self GiveMeAPacket:inData withData:&receivedPacket];
    		    result = GiveMeAPacket([inData bytes], [inData length], &ritorno, &lenRitorno);
                    if ( result ) {
//                        NSLog(@"lunghezza pacchetto:  %d", [receivedPacket length]);
//                        uint8_t *readBytes = (uint8_t *)[receivedPacket bytes];

NSData *receivedPacket=[[[NSData alloc] initWithBytes:ritorno length:lenRitorno] autorelease];

                        // Segnalo l'arrivo di un pacchetto
                        [self.delegate processIncominPacket:receivedPacket];

			// Pulisco il buffer dei dati ricevuti
        		lenRitorno=0;
        		free(ritorno);
        		ritorno=nil;
 //       		lunghezza=0;

//NSLog(@"distruggo receivedPacket");
//                        [receivedPacket release];
//                        receivedPacket=nil;
//NSLog(@"fatto");

                        // Libero la classe dal pacchetto pendente
//|||                       commandPending = kWSIdleState;
                    }

                    // Svuoto i dati presenti nel buffer in entrata
                    [inData release];
                    inData=nil;
                } while (result);
            }
            break;
        }

        case NSStreamEventErrorOccurred:
        {
            streamError=YES;
            NSError *theError = [stream streamError];
            NSLog(@"stream NSStreamEventErrorOccurred [ERR]: %@", stream);
            NSLog(@"stream status: %d ERR", [stream streamStatus], [stream streamError]);

            // Genero un segnale per avvisare che sono collegato
            [self errorOccuredWithString:[theError localizedDescription]];
            break;
        }
        case NSStreamEventOpenCompleted:
            isConnected=YES;

            // Genero un segnale per avvisare che sono collegato
            if ([self _isValidDelegateForSelector:@selector(connectionStatusDidChanged:)]) [delegate connectionStatusDidChanged:isConnected];
            break ;
        default:
            NSLog(@"handleEvent::default");
            break;
    }
}
@end

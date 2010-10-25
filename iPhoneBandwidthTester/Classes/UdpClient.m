//
//  UdpClient.m
//  Mobivu_Tester
//
//  Created by Alex Mantovani on 11/03/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

#import "UdpClient.h"

#include <arpa/inet.h>
#include <netinet/in.h> //for sockaddr_in
#include <sys/socket.h> //for socket(), AF_INET

#import "Mobivu_TesterAppDelegate.h"
#import "GraphView.h"

@implementation UdpClient

@synthesize good, wrong, tot, lost, minLatency, avgLatency, maxLatency, avgFrequency;

-(void)initConnection:(NSString*)address onPort:(int)_port withLen:(int16_t) _buflen
{	
	minLatency = [[NSDate date] timeIntervalSince1970];
	avgLatency = 0.0;
	maxLatency = 0.0;
	
	lastPacketReceived=-1;
	
	good = wrong = lost = tot = 0;
	
	buflen =_buflen;
	
	if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) fprintf(stderr, "socket() failed\n");
	
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(_port);
	if (inet_aton([address cStringUsingEncoding:NSUTF8StringEncoding], &si_other.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	
	// Loop per Ricezione
	[NSThread detachNewThreadSelector:@selector(receiveDataLoop) 
							 toTarget:self 
						   withObject:nil];
		
}


//
// Invia un pacchetto di test delle performance
//
-(void)sendData
{
	static int progressivo;
	
	int slen=sizeof(si_other);
	
	char buf[buflen];

	// Comando di echo/speed test
	buf[0] = 14;
	
	// Infilo nel pacchetto progressivo e ora
	*((int*)&buf[2]) = progressivo++;
	*((NSTimeInterval*)&buf[6]) = [[NSDate date] timeIntervalSince1970];
	
	if (sendto(sock, buf, buflen, 0, (const struct sockaddr*)&si_other, slen)==-1) fprintf(stderr, "sendto() failed\n");
}


//
// Loop per la ricezione dei pacchetti
//
-(void)receiveDataLoop
{	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	char buf[buflen];
	int slen=sizeof(si_other);
	
	int lastProgressive = -1;
	
	while (YES) {
		recvfrom(sock, buf, buflen, 0, (struct sockaddr *)&si_other, (socklen_t*)&slen);
		
		if (buf[0] == 14) {
			// Ricavo i dati
			int progressivo = *((int*)&buf[2]);
			
			// Esamino il progressivo
			if ( (lastProgressive+1) == progressivo) { 
				good++; 
				lastProgressive = progressivo; 
			} else if (lastProgressive > progressivo) {
				wrong++; 
			} else { 
				lost += (progressivo - lastProgressive);
				lastProgressive = progressivo;
			}
			tot++;
			
			NSArray *dati = [[NSArray alloc] initWithObjects:[NSNumber numberWithDouble:*((NSTimeInterval*)&buf[6])], nil];
			[self performSelectorOnMainThread:@selector(aggiungi:) 
								   withObject:dati 
								waitUntilDone:YES];
			[dati release];
		} else {
			// Pacchetto assurdo (sicuramente è successo del casino
			wrong++; 
		}

	} // while
		
	[pool release];
}

-(void)aggiungi:(id)dati
{
	// Calcolo la latenza istantanea
	latency = fabs([ [NSDate dateWithTimeIntervalSince1970: [[(NSArray *)dati objectAtIndex:0] doubleValue] ] timeIntervalSinceNow]);// TODO: Sistemare
	
	// Esamino i tempi di viaggio del pacchetto
	if (latency > maxLatency) maxLatency=latency;
	else if (latency < minLatency) minLatency=latency;
	avgLatency = ((avgLatency * (tot-1)) + latency) / tot; 

	// Esamino i tempi che intercorrono tra un pacchetto e l'altro
	if (lastPacketReceived >= 0)	{
		NSTimeInterval distanceBetween = fabs([ [NSDate dateWithTimeIntervalSince1970:lastPacketReceived] timeIntervalSinceNow]);// TODO: Sistemare
		avgFrequency = ((avgFrequency * (tot-1)) + distanceBetween) / tot; 
	}
	lastPacketReceived = [[NSDate date] timeIntervalSince1970];
	
	
	Mobivu_TesterAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
	[[_delegate.flipsideViewController realtimeGraph] addX:avgLatency*1000.0 y:maxLatency*1000.0 z:latency*1000.0];
}

@end

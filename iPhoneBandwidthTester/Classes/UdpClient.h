//
//  UdpClient.h
//  Mobivu_Tester
//
//  Created by Alex Mantovani on 11/03/10.
//  Copyright 2010 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#include <arpa/inet.h>
#include <netinet/in.h> //for sockaddr_in
#include <sys/socket.h> //for socket(), AF_INET


@interface UdpClient : NSObject {
	int good;
	int wrong;
	int lost;
	int tot;
	
	NSTimeInterval minLatency;
	NSTimeInterval avgLatency;
	NSTimeInterval maxLatency;
	NSTimeInterval avgFrequency;

	NSTimeInterval latency;
	NSTimeInterval lastPacketReceived;

	int16_t buflen;
}

@property(nonatomic,assign) int good;
@property(nonatomic,assign) int wrong;
@property(nonatomic,assign) int lost;
@property(nonatomic,assign) int tot;
@property(nonatomic,assign) NSTimeInterval minLatency;
@property(nonatomic,assign) NSTimeInterval avgLatency;
@property(nonatomic,assign) NSTimeInterval maxLatency;
@property(nonatomic,assign) NSTimeInterval avgFrequency;


struct sockaddr_in si_other;
int sock;


-(void)initConnection:(NSString*)address onPort:(int)_port withLen:(int16_t) _buflen;
-(void)sendData;


@end

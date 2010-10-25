//
//  RecordLayer.m
//  M2M - Protocol
//
//  Created by Alex Mantovani on 05/10/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "RecordLayer.h"
#import "M2M___ProtocolAppDelegate.h"

// per AMR
#include "typedef.h"
#include "interf_enc.h"

NSMutableData *flusso;
NSMutableData *flussoPCM;

// Derive the Buffer Size. I punt with the max buffer size. 
void DeriveBufferSize ( AudioQueueRef audioQueue, AudioStreamBasicDescription ASBDescription, Float64 seconds, UInt32 *outBufferSize ) 
{ 
	static const int maxBufferSize = 0x50000;
	int maxPacketSize = ASBDescription.mBytesPerPacket; 
	if (maxPacketSize == 0) { 
		UInt32 maxVBRPacketSize = sizeof(maxPacketSize); 
		AudioQueueGetProperty ( audioQueue, kAudioConverterPropertyMaximumOutputPacketSize, &maxPacketSize, &maxVBRPacketSize ); 
	} 
	Float64 numBytesForTime = ASBDescription.mSampleRate * maxPacketSize * seconds; 
	*outBufferSize =  (UInt32) ((numBytesForTime < maxBufferSize) ? numBytesForTime : maxBufferSize); 
} 

// Forward Declaration to Input Buffer Handler 
static void HandleInputBuffer ( void *aqData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer, const AudioTimeStamp *inStartTime, UInt32 inNumPackets, const AudioStreamPacketDescription *inPacketDesc ); 

// Assign the file metadata 
OSStatus SetMagicCookieForFile ( AudioQueueRef inQueue, AudioFileID inFile )
{
	OSStatus result = noErr; 
	UInt32 cookieSize; 
	if ( AudioQueueGetPropertySize ( inQueue, kAudioQueueProperty_MagicCookie,  &cookieSize ) == noErr) { 
		char* magicCookie = (char *) malloc (cookieSize); 
		if ( AudioQueueGetProperty ( inQueue, kAudioQueueProperty_MagicCookie, magicCookie, &cookieSize ) == noErr ) 
		    result = AudioFileSetProperty (  inFile, kAudioFilePropertyMagicCookieData,  cookieSize, magicCookie); 
		free (magicCookie); 
	} 
	return result; 
} 

// Write out current packets 
static void HandleInputBuffer ( void *aqData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer, const AudioTimeStamp *inStartTime, UInt32 inNumPackets, const AudioStreamPacketDescription *inPacketDesc) 
{ 
	RecordState *pAqData = (RecordState *) aqData; 

	if (inNumPackets == 0 && pAqData->dataFormat.mBytesPerPacket != 0) inNumPackets = inBuffer->mAudioDataByteSize / pAqData->dataFormat.mBytesPerPacket; 

	
	static int byte_counter;
	int *enstate;
	enum Mode req_mode = MR475;
//	enum Mode req_mode = MRDTX; // Niente
//	enum Mode req_mode = MR515; // Qualcosa arriva
//	enum Mode req_mode = MR59;//MERDA
//	enum Mode req_mode = MR122;
	unsigned char serial_data[32];
	int dtx = 0;
	
	enstate = (int *)Encoder_Interface_init(dtx);


NSLog(@"HandleInputBuffer %d #packets: %d datasize=%d", pAqData->currentPacket, inNumPackets ,inBuffer->mAudioDataByteSize);
	UInt32 packetParsered=0;
	while (packetParsered < inNumPackets) {
		byte_counter = Encoder_Interface_Encode(enstate, req_mode, (Word16 *)inBuffer->mAudioData+packetParsered, serial_data, 0);
		packetParsered += 160;
		
		// Generare un segnale e togliere sta roba da qui
		NSData *buffer=[[[NSData alloc] initWithBytes:serial_data length:byte_counter] autorelease];
NSLog(@"Invio pacchetto audio di %d bytes", [buffer length]);
		M2M___ProtocolAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
		WebServices *ws=_delegate.webServices;
		[ws sendCommand:4 withData:buffer ];//CMD_AUDIOFRAME

// Salvo i dati nel file
//NSData *buffer=[[[NSData alloc] initWithBytes:serial_data length:byte_counter] autorelease];
[flusso appendData:buffer];
	}
	NSData *bufferPCM=[[[NSData alloc] initWithBytes:inBuffer->mAudioData length:inBuffer->mAudioDataByteSize] autorelease];
	[flussoPCM appendData:bufferPCM];
	
	Encoder_Interface_exit(enstate);

//	if ( AudioFileWritePackets ( pAqData->audioFile, NO, inBuffer->mAudioDataByteSize, inPacketDesc, pAqData->currentPacket, &inNumPackets, inBuffer->mAudioData ) == noErr) { 
		pAqData->currentPacket += inNumPackets; 
		if (pAqData->recording == 0) return; 
		AudioQueueEnqueueBuffer ( pAqData->queue, inBuffer, 0, NULL); 
//	} 
} 

// Write the buffers out 
void AudioInputCallback( void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer, const AudioTimeStamp *inStartTime, UInt32 inNumberPacketDescriptions, const AudioStreamPacketDescription *inPacketDescs) 
{
NSLog(@"AudioInputCallback");
	RecordState* recordState = (RecordState*)inUserData; 
	if(!recordState->recording)  { printf("Not recording, returning\n"); return; } 
	
	printf("Writing buffer %d\n", recordState->currentPacket); 
//	OSStatus status = AudioFileWritePackets(recordState->audioFile, NO, inBuffer->mAudioDataByteSize, inPacketDescs, recordState->currentPacket, &inNumberPacketDescriptions, inBuffer->mAudioData); 
OSStatus status = 0;
	if (status == 0) { // Success 
		recordState->currentPacket += inNumberPacketDescriptions; 
	} 
	AudioQueueEnqueueBuffer(recordState->queue, inBuffer, 0, NULL); 
} 

// For use with the file naming, which is automatic 
@interface NSDate (extended) 
-(NSDate *) dateWithCalendarFormat:(NSString *)format timeZone: (NSTimeZone *) timeZone; 
@end 

@implementation RecordLayer
// Initialize the recorder 
- (id) init 
{ 
	self = [super init]; 
	recordState.recording = NO; 
	return self; 
} 

// Set up the recording format as low-quality mono AIFF 
- (void)setupAudioFormat:(AudioStreamBasicDescription*)format 
{ 
	format->mSampleRate = 8000.0; 
	format->mFormatID = kAudioFormatLinearPCM; 
	format->mFormatFlags =  kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
	format->mChannelsPerFrame = 1; // mono 
	format->mBitsPerChannel = 16; 
	format->mFramesPerPacket = 1; 
	format->mBytesPerPacket = 2; 
	format->mBytesPerFrame = 2; // not used, apparently required 
	format->mReserved = 0; 
} 

// Begin recording 
- (void) startRecording: (NSString *) filePath 
{ 
	[self setupAudioFormat:&recordState.dataFormat];
	
	fileURL =  CFURLCreateFromFileSystemRepresentation (NULL, (const UInt8 *) [filePath UTF8String], [filePath length], NO); 
	recordState.currentPacket = 0; 
	OSStatus status; 
	status = AudioQueueNewInput(&recordState.dataFormat, HandleInputBuffer, &recordState, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &recordState.queue); 
	if (status != 0) { 
		printf("Could not establish new queue\n"); 
		return; 
	} 
/*
	status = AudioFileCreateWithURL(fileURL, kAudioFileAIFFType, &recordState.dataFormat, kAudioFileFlags_EraseFile, &recordState.audioFile); 
	if (status != 0) { 
		printf("Could not create file to record audio\n"); 
		return; 
	} 
*/
	DeriveBufferSize ( recordState.queue,  recordState.dataFormat, 0.5, &recordState.bufferByteSize ); 
	// printf("0Buffer size: %d\n", recordState.bufferByteSize); 
	for(int i = 0; i < NUM_BUFFERS; i++) { 
		status = AudioQueueAllocateBuffer(recordState.queue,  recordState.bufferByteSize, &recordState.buffers[i]); 
		if (status) { 
			printf("Error allocating buffer %d\n", i); 
			return; 
		} 
		status = AudioQueueEnqueueBuffer(recordState.queue, recordState.buffers[i], 0, NULL); 
		if (status) { 
			printf("Error enqueuing buffer %d\n", i); 
			return; 
		} 
	} 
/*	status = SetMagicCookieForFile(recordState.queue, recordState.audioFile); 
	if (status != 0) { 
		printf("Magic cookie failed\n"); 
		return; 
	} 
*/	status = AudioQueueStart(recordState.queue, NULL); 
	if (status != 0) { 
		printf("Could not start Audio Queue\n"); 
		return; 
	} 
	recordState.currentPacket = 0; 
	recordState.recording = YES; 
	
	NSLog(@"startRecording quit");

	return; 
} 

#define DOCSFOLDER [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"]

// There’s generally about a one-second delay before the buffers fully empty 
- (void) reallyStopRecording 
{ 
	AudioQueueFlush(recordState.queue); 
	AudioQueueStop(recordState.queue, NO); 
	recordState.recording = NO; 
//	SetMagicCookieForFile(recordState.queue, recordState.audioFile); 
	for(int i = 0; i < NUM_BUFFERS; i++) { 
		AudioQueueFreeBuffer(recordState.queue, recordState.buffers[i]); 
	} 
	AudioQueueDispose(recordState.queue, YES); 
//	AudioFileClose(recordState.audioFile); 
	
	// Salvo il file amr di debug
	NSString *plistPath = [DOCSFOLDER stringByAppendingPathComponent:@"analyze.amr"]; 
	[flusso writeToFile:plistPath atomically:YES];
	NSString *plistPathPCM = [DOCSFOLDER stringByAppendingPathComponent:@"analyze.pcm"]; 
	[flussoPCM writeToFile:plistPathPCM atomically:YES];
	NSLog(@"Saving stream in %@ [%d bytes]\n",plistPath, [flusso length]); 
} 

// Stop the recording after waiting just a second 
- (void) stopRecording 
{ 
	[self performSelector:@selector(reallyStopRecording) withObject:NULL afterDelay:1.0f]; 
}

// Automatically create a file in Documents and start/stop recording to it 
- (void) toggleRecording 
{ 
	// Create a new dated file 
	NSDate *now = [NSDate dateWithTimeIntervalSinceNow:0]; 
	NSString *caldate = [[now dateWithCalendarFormat:@"%b_%d_%H_%M_%S" timeZone:nil] description]; // no :’s. 
	NSString *filePath = [NSString stringWithFormat:@"%@/analyze.aiff", DOCUMENTS_FOLDER]; 
	if( !recordState.recording ) { 
		printf("Starting recording\n"); 
		[self startRecording: filePath]; 
		flusso=[[NSMutableData alloc] initWithBytes:"#!AMR\x0A" length:6];
		flussoPCM=[[NSMutableData alloc] initWithLength:0];
	} else { 
		[self stopRecording]; 
		printf("Stopping recording \n"); 
	} 
} 

- (BOOL) isRecording 
{ 
	return recordState.recording; 
} 

@end

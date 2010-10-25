//
//  MainViewController.m
//  M2M - Protocol
//
//  Created by Alex Mantovani on 24/09/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "MainViewController.h"
#import "MainView.h"
#import "M2M___ProtocolAppDelegate.h"

#import "CoreAudio/CoreAudioTypes.h"

#import "RecordLayer.h"
#import "interf_dec.h"

#define CMD_LOGIN 0
#define CMD_LOGOUT 1
#define CMD_AUDIOFRAME 4
#define CMD_USERLIST 5
#define CMD_CALL 6 


@implementation MainViewController

@synthesize statusLed;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}



 // Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
 - (void)viewDidLoad {
	 self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"bgdark.png"]];
	 alreadyAsked=0xFF;
	 ricevutiCnt=0;
	 goodCnt=0;

	NSString *newAudioFile = [[NSBundle mainBundle] pathForResource:@"MGJ"  ofType:@"amr"];
	 talkData = [[NSData alloc] initWithContentsOfFile:newAudioFile];
	 NSLog(@"Caricati %d", [talkData length]);
	 
	 rec=[[RecordLayer alloc] init];
	 
	 [super viewDidLoad];
	 
 }
 


/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */


- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
}


- (IBAction)showInfo {    
	
	FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	controller.delegate = self;
	
	controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
	[self presentModalViewController:controller animated:YES];
	
	[controller release];
}



/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[talkData release];
	
    [super dealloc];
}


- (IBAction)doLogin;
{
	NSString *username=@"iPhone";
	
	NSLog(@"Eseguo il login");
	M2M___ProtocolAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
	WebServices *ws=_delegate.webServices;
	[ws sendCommand:CMD_LOGIN withData:[username dataUsingEncoding:NSUTF8StringEncoding ] ];

	alreadyAsked=0xFF;
}


- (IBAction)doLogout;
{
	NSString *username=@"iPhone";
	
	NSLog(@"Eseguo il logout");
	M2M___ProtocolAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
	WebServices *ws=_delegate.webServices;
	[ws sendCommand:CMD_LOGOUT withData:[username dataUsingEncoding:NSUTF8StringEncoding ] ];

	alreadyAsked=0xFF;
}

- (IBAction)doUserList
{
	NSString *buffer=@"";
	NSLog(@"doUserList");
	M2M___ProtocolAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
	WebServices *ws=_delegate.webServices;
	[ws sendCommand:CMD_USERLIST withData:[buffer dataUsingEncoding:NSUTF8StringEncoding ]  ];
}

- (IBAction)doCallUser
{
	NSString *buffer=@"Alex";
	NSLog(@"call user %@", buffer);
	M2M___ProtocolAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
	WebServices *ws=_delegate.webServices;
	[ws sendCommand:2 withData:[buffer dataUsingEncoding:NSUTF8StringEncoding ]  ];
}

-(void) answerToCallRequest:(uint8_t)esito
{
	NSData *buffer=[[[NSData alloc] initWithBytes:&esito length:1] autorelease];
	NSLog(@"answer to call with %@", [buffer description]);
	M2M___ProtocolAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
	WebServices *ws=_delegate.webServices;
	[ws sendCommand:CMD_CALL withData:buffer ];
}


#pragma mark -
#pragma mark WebServices Delegate

-(void)connectionStatusDidChanged:(bool)connected
{
	if (connected) {
		[statusLed setImage:[UIImage imageNamed:@"greenLed.png"]];
		statusLabel.text=@"Connesso";
		[statusLabel setTextColor:[UIColor greenColor]];
	} else  { 
		[statusLed setImage:[UIImage imageNamed:@"redLed.png"]];
		statusLabel.text=@"Non connesso";
		[statusLabel setTextColor:[UIColor redColor]];
	}
	alreadyAsked=0xFF;
}


//
// Perser per i pacchetti ricevuti
//
-(void) processIncominPacket:(NSData *)receivedPacket
{
	// Aggiorno le statistiche a video
	[receivedLabel setText:[[[NSString alloc] initWithFormat:@"%d", 	++ricevutiCnt] autorelease]];
	[goodLabel setText:[[[NSString alloc] initWithFormat:@"%d", ++goodCnt] autorelease]];
	
	uint8_t comando=*(uint8_t *)[receivedPacket bytes];
	
//	NSString *risultato=[[[NSString alloc] initWithData:receivedPacket+1 encoding:NSASCIIStringEncoding] autorelease];
	NSString *risultato=[[[NSString alloc] initWithBytes:(uint8_t *)[receivedPacket bytes]+1 length:[receivedPacket length]-1 encoding:NSASCIIStringEncoding] autorelease];
	
//    NSLog(@"Ricevuto comando %d di lunghezza '%@'", comando, risultato);
	UIAlertView *alert;
	// Analizzo il pacchetto di risposta
	switch (comando) {
		case CMD_LOGIN: // Login
			NSLog(@"Ricevuto comando di LOGIN");
			[statusLabel setText:@"Logged in"];
			[statusLed setImage:[UIImage imageNamed:@"greenLed.png"]];
			break;
		case CMD_LOGOUT: // Logout
			NSLog(@"Ricevuto comando di LOGOUT");
			[statusLabel setText:@"Logged out"];
			[statusLed setImage:[UIImage imageNamed:@"redLed.png"]];
			break;
		case CMD_AUDIOFRAME: // Frame audio
			NSLog(@"Ricevuto frame AUDIO di lunghezza %d", [receivedPacket length]-1);
			[statusLabel setText:[NSString stringWithFormat:@"Ricevuto pacchetto audio (len: %d)", [risultato length]]];
//||| Riproduco i dati ricevuti
			short synth[160];
			int * destate;
			
			destate = (int *)Decoder_Interface_init();
			Decoder_Interface_Decode(destate, (uint8_t *)[receivedPacket bytes]+1, synth, 0);
// Riprodurre synt			fwrite( synth, sizeof (short), 160, file_speech );
			
			Decoder_Interface_exit(destate);
	
			break;
		case CMD_USERLIST: // User List
			NSLog(@"Ricevuto USER LIST");
			listItems = [risultato componentsSeparatedByString:@"|"];
			[statusLabel setText:[NSString stringWithFormat:@"Logged in %d users", [listItems count]-2]];
			NSLog(@"'%@'", [listItems objectAtIndex:1]);
			break;
		case  CMD_CALL: // Chiamata in entrata
			if (alreadyAsked==0xFF) {
				alert = [[UIAlertView alloc] initWithTitle:@"Incoming call" message:[NSString stringWithFormat:@"%@ calling", risultato] 
														delegate:self cancelButtonTitle:NSLocalizedString(@"Rifiuta",@"")
														otherButtonTitles:NSLocalizedString(@"Accetta",@""), nil];
			
				[alert show];
				[alert release];
			} else {
				[self answerToCallRequest:alreadyAsked];
			}
			break;
		default:
			NSLog(@"non ancora gestito comando:%d", comando);
			break;
	}
}


-(void) hasSpaceAvailable:(NSStream *)stream
{
	static int counter;
	wrongLabel.text=[NSString stringWithFormat:@"%d",++counter];
	
	if (sendAudio) [self doSendAudioFrame];
}

#pragma mark - UIAlertViewDelegate

- (void)alertView:(UIAlertView *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	alreadyAsked=buttonIndex; 
	[self answerToCallRequest:alreadyAsked];
}

/*
- (IBAction)sendAudio
{
	NSLog(@"talking");

	sendAudio=YES;
	bufferIndice=6;
	
	[self doSendAudioFrame];
}
*/

- (IBAction)sendAudio
{
	// Record/stop dal microfono
	[rec toggleRecording];
}

- (IBAction)doSendAudioFrame;
{
	uint8_t lenFrame = *(uint8_t*)[talkData bytes]+bufferIndice;
	if ((lenFrame & 0x70)==0) lenFrame = 14; 
	else if ((lenFrame & 0x70)==0x40) lenFrame = 18; 
	NSData *buffer=[[[NSData alloc] initWithBytes:(uint8_t *)[talkData bytes]+bufferIndice length:lenFrame] autorelease];
	bufferIndice=bufferIndice+lenFrame;
	
	NSLog(@"Invio pacchetto audio di %d bytes", [buffer length]);
	M2M___ProtocolAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
	WebServices *ws=_delegate.webServices;
	[ws sendCommand:CMD_AUDIOFRAME withData:buffer ];
	
	if (bufferIndice > 3100) sendAudio=NO;
}


/*
// TEST
-(void)playForLearn
{
	AudioStreamBasicDescription desc;
	desc.mSampleRate = 8000;
//	desc.mFormatID = kAudioFormatLinearPCM;
	desc.mFormatID = kAudioFormatAMR;
	desc.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger  | kAudioFormatFlagIsPacked;
	desc.mBytesPerPacket = 12;
	desc.mFramesPerPacket = 1;
	desc.mBytesPerFrame = 12;
	desc.mChannelsPerFrame = 1;
	desc.mBitsPerChannel = 16;
	
	// single thread
	int status = AudioQueueNewOutput(&desc, AQBufferCallback, self, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &outAQ);
	if (status) [NSException raise:@"AudioOutputError" format:@"AudioQueueSetParameter failed: %d", status];

	UInt32 bufferBytes  = FRAMES * desc.mBytesPerFrame;
	for (i = 0; i < BUFFERS; i++) {
		status = AudioQueueAllocateBuffer(outAQ, bufferBytes, &mBuffers[i]);
		if (status) [NSException raise:@"AudioOutputError" format:@"AudioQueueAllocateBuffer failed: %d", status];
		
		AQBufferCallback(self, outAQ, mBuffers[i]);
	}
	
	status = AudioQueueStart(outAQ, NULL);
	if (status) [NSException raise:@"AudioOutputError" format:@"AudioQueueStart failed: %d", status];
}

static void AQBufferCallback(void *aqData, AudioQueueRef queue, AudioQueueBufferRef buffer) {
	AudioOutput *dev = (AudioOutput*)aqData;
	AudioSample *coreAudioBuffer = (AudioSample*) buffer->mAudioData;
	buffer->mAudioDataByteSize = sizeof(AudioSample) * 2 * FRAMES;
    [dev fillBuffer:coreAudioBuffer frames:FRAMES];
	AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
}

*/
@end

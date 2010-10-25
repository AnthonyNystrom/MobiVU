//
//  MainViewController.m
//  Mobivu_Tester
//
//  Created by Alex Mantovani on 11/03/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

#import "MainViewController.h"
#import "Mobivu_TesterAppDelegate.h"

@implementation MainViewController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	[super viewDidLoad];

	[self.view setBackgroundColor:[[UIColor alloc] initWithPatternImage:[UIImage imageNamed:@"Pattern1.png"]]];
	
	Mobivu_TesterAppDelegate * _delegate = [UIApplication sharedApplication].delegate;
	
	// Informazioni generali
	[info1Label setText:[NSString stringWithFormat:@"UDP packet length (%d bytes)", 
						 [[_delegate.networkSetup objectForKey:@"UDP Packet Length"] intValue]]];
	[info2Label setText:[NSString stringWithFormat:@"Server @ %@:%d", 
						 [_delegate.networkSetup objectForKey:@"Server IP Address"], 
						 [[_delegate.networkSetup objectForKey:@"UDP Port"] intValue]]];
	
	// Attivo il client
	udpClient=[[UdpClient alloc] init];
	[udpClient initConnection:[_delegate.networkSetup objectForKey:@"Server IP Address"] 
					   onPort:[[_delegate.networkSetup objectForKey:@"UDP Port"] intValue]
					  withLen:[[_delegate.networkSetup objectForKey:@"UDP Packet Length"] intValue] ];
	
	startTime = [[NSDate alloc] init];
	
	// Attivo un timer per sparare i pacchetti a raffica
	[NSTimer scheduledTimerWithTimeInterval:0.05 
									 target:self 
								   selector:@selector(sendPacket:) 
								   userInfo:nil 
									repeats:YES];

	// Attivo un timer per sparare i pacchetti a raffica
	[NSTimer scheduledTimerWithTimeInterval:0.5 
									 target:self 
								   selector:@selector(updateScreen:) 
								   userInfo:nil 
									repeats:YES];
}



- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
}


- (IBAction)showInfo {
	Mobivu_TesterAppDelegate * _delegate = [UIApplication sharedApplication].delegate;

	_delegate.flipsideViewController.delegate = self;
	
	_delegate.flipsideViewController.modalTransitionStyle = UIModalTransitionStyleCrossDissolve;
	[self presentModalViewController:_delegate.flipsideViewController animated:YES];
	
}


- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc. that aren't in use.
}


- (void)viewDidUnload {
	startTime = nil;
	udpClient = nil;
}


- (void)dealloc {
    [super dealloc];
	
	[startTime release];
	[udpClient release];

}

#pragma mark -
#pragma mark Chiamate periodiche

//
// Chiamata attraverso un timer per eseguire l'invio di un pacchetto di TEST
//
-(void)sendPacket:(NSTimer*)timer 
{	
	// Invio del pacchetto al server UDP
	udpClient.sendData;
}


//
// Aggiornamento sul video delle statistiche
//
-(void)updateScreen:(NSTimer*)timer 
{	
	[goodLabel setText:[NSString stringWithFormat:@"%d", [udpClient good]]];
	[wrongLabel setText:[NSString stringWithFormat:@"%d", [udpClient wrong]]];
	[totalLabel setText:[NSString stringWithFormat:@"%d", [udpClient tot]]];
	[lostLabel setText:[NSString stringWithFormat:@"%d", [udpClient lost]]];
	
	[minLatencyLabel setText:[NSString stringWithFormat:@"%.5f", [udpClient minLatency]]];
	[avgLatencyLabel setText:[NSString stringWithFormat:@"%.5f", [udpClient avgLatency]]];
	[maxLatencyLabel setText:[NSString stringWithFormat:@"%.5f", [udpClient maxLatency]]];
	
	[avgFrequencyLabel setText:[NSString stringWithFormat:@"%.5f", [udpClient avgFrequency]]];
	
	
	[durationLabel setText:[NSString stringWithFormat:@"%d", abs(rint([startTime timeIntervalSinceNow])) ]];
}


@end

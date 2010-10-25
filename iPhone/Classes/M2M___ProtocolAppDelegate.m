//
//  M2M___ProtocolAppDelegate.m
//  M2M - Protocol
//
//  Created by Alex Mantovani on 24/09/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "M2M___ProtocolAppDelegate.h"
#import "MainViewController.h"

#import <AVFoundation/AVFoundation.h>

@implementation M2M___ProtocolAppDelegate


@synthesize window;
@synthesize mainViewController;
@synthesize webServices;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	MainViewController *aController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
	self.mainViewController = aController;
	[aController release];
	
    mainViewController.view.frame = [UIScreen mainScreen].applicationFrame;
	[window addSubview:[mainViewController view]];
    [window makeKeyAndVisible];
	
	NSLog(@"Mi connetto al server");
	webServices = [[WebServices alloc] init];
	webServices.delegate=mainViewController;

	[webServices openConnection];
}


- (void)dealloc {
	[webServices closeConnection];
	[webServices release];

    [mainViewController release];
    [window release];
    [super dealloc];
}

#pragma mark -
#pragma mark PlaySound

-(IBAction) playSound:(NSString*)fileName withDelta:(const float)delta
{
	NSURL *url = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@%@", [[NSBundle mainBundle] resourcePath], fileName]];
	
	NSError *error;
	AVAudioPlayer *audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
	
	float	volumeValue = 0.5f;
//	float	volumeValue = [[[NSUserDefaults standardUserDefaults] objectForKey:@"volumeSounds"] floatValue] / 10.0f;
	
	[audioPlayer setVolume:volumeValue+delta];
	audioPlayer.numberOfLoops = 0;
	[audioPlayer setMeteringEnabled:YES];
	
	if (audioPlayer == nil) NSLog([error description]);
	else [audioPlayer play];
}


@end

//
//  Mobivu_TesterAppDelegate.m
//  Mobivu_Tester
//
//  Created by Alex Mantovani on 11/03/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

#import "Mobivu_TesterAppDelegate.h"
#import "MainViewController.h"

@implementation Mobivu_TesterAppDelegate


@synthesize window;
@synthesize mainViewController;
@synthesize flipsideViewController;
@synthesize networkSetup;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    
	networkSetup = [[NSDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"NetworkSetup" ofType:@"plist"]];

	MainViewController *aController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
	self.mainViewController = aController;
	[aController release];
	
    mainViewController.view.frame = [UIScreen mainScreen].applicationFrame;
	[window addSubview:[mainViewController view]];
    [window makeKeyAndVisible];
	
	
	flipsideViewController = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];

	return YES;
}


- (void)dealloc {
    [mainViewController release];
	[flipsideViewController release];
	[networkSetup release];

    [window release];
    [super dealloc];
}

@end

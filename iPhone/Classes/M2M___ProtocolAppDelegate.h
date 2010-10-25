//
//  M2M___ProtocolAppDelegate.h
//  M2M - Protocol
//
//  Created by Alex Mantovani on 24/09/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//
#import "WebServices.h"

@class MainViewController;

@interface M2M___ProtocolAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    MainViewController *mainViewController;
	
	WebServices *webServices;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) MainViewController *mainViewController;
@property (nonatomic, retain) WebServices *webServices;

-(IBAction) playSound:(NSString*)fileName withDelta:(const float)delta;

@end


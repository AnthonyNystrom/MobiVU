//
//  Mobivu_TesterAppDelegate.h
//  Mobivu_Tester
//
//  Created by Alex Mantovani on 11/03/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MainViewController;
@class FlipsideViewController;

@interface Mobivu_TesterAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    MainViewController *mainViewController;
	FlipsideViewController *flipsideViewController;
	
	NSDictionary *networkSetup;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) MainViewController *mainViewController;
@property (nonatomic, retain) FlipsideViewController *flipsideViewController;
@property (nonatomic, retain) NSDictionary *networkSetup;

@end


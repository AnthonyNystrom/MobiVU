//
//  MainViewController.h
//  M2M - Protocol
//
//  Created by Alex Mantovani on 24/09/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "FlipsideViewController.h"
#import "WebServices.h"
#import "RecordLayer.h"

@interface MainViewController : UIViewController <FlipsideViewControllerDelegate, WebServicesDelegate, UIAlertViewDelegate> {
	IBOutlet UILabel *Debug;
	IBOutlet UILabel *statusLabel;

	IBOutlet UILabel *receivedLabel;
	IBOutlet UILabel *goodLabel;
	IBOutlet UILabel *wrongLabel;
	NSInteger ricevutiCnt;
	NSInteger goodCnt;

	IBOutlet UIImageView *statusLed;	
	uint8_t alreadyAsked;
	NSArray *listItems;
	
	NSData *talkData;
	int bufferIndice;
	bool sendAudio;
	
	
	RecordLayer *rec;
}

@property (nonatomic, retain) IBOutlet UIImageView *statusLed;

- (IBAction)showInfo;


- (IBAction)doLogin;
- (IBAction)doLogout;
- (IBAction)doUserList;
- (IBAction)doCallUser;

- (IBAction)sendAudio;


@end

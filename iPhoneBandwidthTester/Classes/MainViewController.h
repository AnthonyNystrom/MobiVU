//
//  MainViewController.h
//  Mobivu_Tester
//
//  Created by Alex Mantovani on 11/03/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

#import "FlipsideViewController.h"
#import "UdpClient.h"

@interface MainViewController : UIViewController <FlipsideViewControllerDelegate> {
	IBOutlet UILabel *durationLabel;

	IBOutlet UILabel *goodLabel;
	IBOutlet UILabel *totalLabel;
	IBOutlet UILabel *wrongLabel;
	IBOutlet UILabel *lostLabel;

	IBOutlet UILabel *minLatencyLabel;
	IBOutlet UILabel *maxLatencyLabel;
	IBOutlet UILabel *avgLatencyLabel;
	IBOutlet UILabel *avgFrequencyLabel;

	IBOutlet UILabel *info1Label;
	IBOutlet UILabel *info2Label;

	UdpClient *udpClient;	// Client UDP per il controllo dei dati
	NSDate *startTime;		// Ora di inizio del TEST
}

- (IBAction)showInfo;

@end

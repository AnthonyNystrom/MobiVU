//
//  FlipsideViewController.h
//  Mobivu_Tester
//
//  Created by Alex Mantovani on 11/03/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GraphView.h"

@protocol FlipsideViewControllerDelegate;


@interface FlipsideViewController : UIViewController {
	id <FlipsideViewControllerDelegate> delegate;
	GraphView *realtimeGraph;
}
@property(nonatomic, retain) IBOutlet GraphView *realtimeGraph;

@property (nonatomic, assign) id <FlipsideViewControllerDelegate> delegate;
- (IBAction)done;

@end


@protocol FlipsideViewControllerDelegate
- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller;
@end


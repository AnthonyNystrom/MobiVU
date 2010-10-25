#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@class GraphViewSegment;
@class GraphTextView;
@interface GraphView : UIView
{
	NSMutableArray *segments;
	GraphViewSegment *current; // weak reference
	GraphTextView *text; // weak reference
}

-(void)addValue:(NSTimeInterval)x;

@end

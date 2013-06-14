#import <Cocoa/Cocoa.h>

@interface TrackBarView : NSView

//Add colors
- (void)addColorColumn: (NSMutableArray *)colors;

//Offscreen data buffer
@property (atomic) CGContextRef offscreenContext;
@property (atomic) CGLayerRef offscreenLayer;

@property (atomic) float seekTime;          //Where the offset is drawn
@property (atomic) float recordOffsetTime;  //What offset we are rocrding from

@property (atomic) BOOL scrollToLatest;     //Scroll to end

@end

#import "TrackWaveView.h"

#define kTrackBufferSize 600000
#define kTrackElementWidth 1 //Size of each discrete track data
#define kTrackElementHeight 20.0f
#define TIME_TO_OFFSCREEN_OFFSET(time) (time*kTrackElementWidth)
#define OFFSCREEN_OFFSET_TO_TIME(offset) (offset/kTrackElementWidth)

@implementation TrackWaveView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.scrollToLatest = YES;
        
        NSTimer *drawTimer = [NSTimer timerWithTimeInterval:0.01f target:self selector:@selector(redraw) userInfo:nil repeats:YES];
        [[NSRunLoop mainRunLoop] addTimer:drawTimer forMode:NSDefaultRunLoopMode];

    }
    
    return self;
}

- (void)redraw {
    [self setNeedsDisplay:YES];
}

- (void)firstDraw {
    //Setup offscreen buffer
    self.offscreenLayer = CGLayerCreateWithContext([self.window.graphicsContext graphicsPort], CGSizeMake(kTrackBufferSize, self.frame.size.height), nil);
    self.offscreenContext = CGLayerGetContext(self.offscreenLayer);
}

- (void)addWaveColumn: (NSMutableArray *)values {
    //Keeps track of last values
    static NSMutableArray *lastValues;
    
    static BOOL isFirst = YES;
    if (isFirst == YES) {
        isFirst = NO;
        lastValues = values;
        return;
    }
    
    if (self.offscreenContext == 0)
        return;
    int index = 0;
    for (NSNumber *value in values) {
        CGContextSetRGBStrokeColor(self.offscreenContext, 0.0f, [value floatValue], 0.0f, 1.0f);
        CGContextSetRGBFillColor(self.offscreenContext, 0.0f, 0.0f, 0.0f, 1.0f);

        NSNumber *lastValue = [lastValues objectAtIndex:index];
        CGContextFillRect(self.offscreenContext, CGRectMake(TIME_TO_OFFSCREEN_OFFSET(self.recordOffsetTime), kTrackElementHeight*[value floatValue] + index*kTrackElementHeight, 1, 1));
//        CGContextMoveToPoint(self.offscreenContext, TIME_TO_OFFSCREEN_OFFSET(self.recordOffsetTime-1), kTrackElementHeight*[lastValue floatValue] + index*kTrackElementHeight);
//        CGContextAddLineToPoint(self.offscreenContext, TIME_TO_OFFSCREEN_OFFSET(self.recordOffsetTime), kTrackElementHeight*[value floatValue] + index*kTrackElementHeight);
//        
//        CGContextStrokePath(self.offscreenContext);
        
        ++index;
    }
    
    self.recordOffsetTime += 1.0f;
    lastValues = values;
}

- (void)drawTrack {
    //Draw offscreen layer to main layer at some offset position
    float offscreenOffset = TIME_TO_OFFSCREEN_OFFSET(self.seekTime);
    CGContextDrawLayerAtPoint(self.window.graphicsContext.graphicsPort, CGPointMake(offscreenOffset, 0.0f), self.offscreenLayer);
}

- (void)drawRect:(NSRect)dirtyRect
{
    static BOOL isFirst = YES;
    if (isFirst) {
        isFirst = NO;
        [self firstDraw];
    }
    
    if (self.scrollToLatest) {
        if (TIME_TO_OFFSCREEN_OFFSET(self.recordOffsetTime) + TIME_TO_OFFSCREEN_OFFSET(self.seekTime) > self.frame.size.width) {
            self.seekTime = -self.recordOffsetTime;
        }
    }
    
    [self drawTrack];
}

@end

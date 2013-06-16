#import "TrackDataView.h"

#define kTrackBufferSize 600000
#define kTrackElementWidth 5 //Size of each discrete track data
#define kTrackElementHeight 20.0f
#define TIME_TO_OFFSCREEN_OFFSET(time) (time*kTrackElementWidth)
#define OFFSCREEN_OFFSET_TO_TIME(offset) (offset/kTrackElementWidth)

@implementation TrackDataView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
    }
    
    self.scrollToLatest = YES;
    
    NSTimer *drawTimer = [NSTimer timerWithTimeInterval:0.01f target:self selector:@selector(redraw) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:drawTimer forMode:NSDefaultRunLoopMode];
    
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

- (void)addColorColumn: (NSMutableArray *)colors {
    if (self.offscreenContext == 0)
        return;
    int index = 0;
    for (NSColor *color in colors) {
        CGContextSetRGBFillColor(self.offscreenContext, [color redComponent], [color greenComponent], [color blueComponent], [color alphaComponent]);
        
        CGContextFillRect(self.offscreenContext, CGRectMake(TIME_TO_OFFSCREEN_OFFSET(self.recordOffsetTime), index*kTrackElementHeight, kTrackElementWidth, kTrackElementHeight));
        
        CGContextSetRGBStrokeColor(self.offscreenContext, 0.0f, 0.0f, 0.0f, 1.0f);
        CGContextStrokeRect(self.offscreenContext, CGRectMake(TIME_TO_OFFSCREEN_OFFSET(self.recordOffsetTime), index*kTrackElementHeight, kTrackElementWidth, kTrackElementHeight));
        
        ++index;
    }
    
    self.recordOffsetTime += 1.0f;
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
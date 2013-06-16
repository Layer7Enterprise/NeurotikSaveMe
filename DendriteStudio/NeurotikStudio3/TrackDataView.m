#import "TrackDataView.h"

#define kTrackBufferSize 600000
#define kTrackElementWidth 5 //Size of each discrete track data
#define kTrackElementHeight 20
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

- (void)addBarColumn: (NSMutableArray *)values {
    if (self.offscreenContext == 0)
        return;
    int index = 0;
    
    if ([values count] == 0)
        return;
    
    CGContextClearRect(self.offscreenContext, CGRectMake(0.0f, 0.0f, self.frame.size.width, self.frame.size.height));
    
    CGContextSelectFont(self.offscreenContext, "Monaco", 1.0f, kCGEncodingMacRoman);
    CGContextSetTextDrawingMode(self.offscreenContext, kCGTextFill);
    CGContextSetTextMatrix(self.offscreenContext, CGAffineTransformMakeScale(13.0f, 13.0f));
    
    for (NSNumber *value in values) {
        if ([value floatValue] > 0.0f) {
            CGContextSetRGBFillColor(self.offscreenContext, 0.0f, 1.0f, 0.0f, 1.0f);
        } else {
            CGContextSetRGBFillColor(self.offscreenContext, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        
        float value2 = [value floatValue];
        if (value2 < 0)
            value2 = -value2;
        
         CGContextFillRect(self.offscreenContext, CGRectMake(0, index*kTrackElementHeight, value2*self.frame.size.width*0.8f, kTrackElementHeight));
        
        CGContextSetLineWidth(self.offscreenContext, 3.0f);
        CGContextSetRGBStrokeColor(self.offscreenContext, 0.0f, 0.0f, 0.0f, 1.0f);
        CGContextStrokeRect(self.offscreenContext, CGRectMake(0, index*kTrackElementHeight, value2*self.frame.size.width*0.8f, kTrackElementHeight));
        
        CGContextSetRGBFillColor(self.offscreenContext, 0.0f, 0.0f, 0.0f, 1.0f);
        CGContextShowTextAtPoint(self.offscreenContext, 20.0f, index*kTrackElementHeight + 5, [[value stringValue] UTF8String], [[value stringValue] length]);
        
        
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
    
    [self drawTrack];
}

@end
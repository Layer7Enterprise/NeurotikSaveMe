#import "TrackInfoView.h"

#define LABEL_HEIGHT 20.0

@implementation TrackInfoView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
    }
    
    return self;
}

- (void)drawName: (NSString *)name atPosition: (CGPoint) pos {
    CGContextRef context = [[[self window] graphicsContext] graphicsPort];
    CGContextSetRGBFillColor(context, 0.3f, 0.3f, 0.3f, 1.0f);
    
    CGContextFillRect(context, CGRectMake(pos.x, pos.y, self.frame.size.width, LABEL_HEIGHT));
    
    CGContextSelectFont(context, "Monaco", 1.0f, kCGEncodingMacRoman);
    CGContextSetTextDrawingMode(context, kCGTextFill);
    
    int index = 0;
    for (NSString *name in self.neuronNames) {
        //Draw rectangle
        CGContextSetRGBFillColor(context, index % 2 == 0 ? 1.0f : 0.0f, 1.0f, 0.0f, 1.0f); //Stiple pattern
        CGContextFillRect(context, CGRectMake(0.0f, index*LABEL_HEIGHT, self.frame.size.width, LABEL_HEIGHT));
        
        //Draw text
        CGContextSetRGBFillColor(context, 0.0f, 0.0f, 0.0f, 1.0f);
        CGContextShowTextAtPoint(context, LABEL_HEIGHT, index*LABEL_HEIGHT + 4, [name UTF8String], [name length]);
        ++index;
    }

}

- (void)drawNames {
    float offset = 0.0f;
    
    for (NSString *name in self.neuronNames) {
        [self drawName:name atPosition:CGPointMake(0.0f, offset)];
        offset += LABEL_HEIGHT;
    }
}

- (void)drawRect:(NSRect)dirtyRect
{
    [self drawNames];
}

@end

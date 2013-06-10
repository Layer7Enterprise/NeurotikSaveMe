//
//  MainView.m
//  NeurotikStudio
//
//  Created by Seo Townsend on 6/3/13.
//  Copyright (c) 1013 Layer 7. All rights reserved.
//

#import "MainView.h"

#define BUFFER_SIZE 100000

@implementation MainView

-(void)awakeFromNib {
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(handleAnimationTimer:) userInfo:nil repeats:YES];
}

-(void)handleAnimationTimer: (NSTimer *) timer {
    NSMutableArray *colorArray = [[NSMutableArray alloc] init];
    for (int i = 0; i < 5; ++i) {
        int color = rand() % 3;
        
        if (color == 0)
            [colorArray addObject:[NSColor redColor]];
        else if (color == 1)
            [colorArray addObject:[NSColor greenColor]];
        else if (color == 2)
            [colorArray addObject:[NSColor blueColor]];
    }
    
    [self addColorColumn:colorArray];
    self.xPos += 10;
    
    dispatch_async(dispatch_get_current_queue(), ^{
        [self setNeedsDisplay:YES];
    });
}

-(void)pushColorColumn: (NSArray *)colors atPosition: (int)position {
    CGContextSetRGBStrokeColor(self.offscreenContext, 0.0, 0.0, 0.0, 1.0);
    
    int yp = 0;
    for (NSColor *color in colors) {
        CGContextSetRGBFillColor(self.offscreenContext, [color redComponent], [color greenComponent], [color blueComponent], 1.0);
        
        CGContextSetRGBFillColor(self.offscreenContext, [color redComponent], [color greenComponent], [color blueComponent], 1.0);
        
        CGContextFillRect(self.offscreenContext, CGRectMake(position, yp, 10, 20));
        CGContextStrokeRect(self.offscreenContext, CGRectMake(position, yp, 10, 20));
        
        yp += 20;
    }
}

-(void)addColorColumn: (NSMutableArray *)colors {
    static int x = BUFFER_SIZE - 10;
    
    [self pushColorColumn:colors atPosition:x];
    
    x -= 10;
}

-(void)drawText: (NSString *)text atRow: (int)row {
    CGContextSelectFont(self.mainContext, "Arial", 100.0f, kCGEncodingMacRoman);
    CGContextSetRGBFillColor(self.mainContext, 1.0f, 1.0f, 0.0f, 1.0f);
    CGContextShowTextAtPoint(self.mainContext, -100.0f, row, [text UTF8String], [text length]);
}

-(void)firstDraw {
    self.mainContext = [[NSGraphicsContext currentContext] graphicsPort];
    
    //Create offscreen context
    self.offscreenLayer = CGLayerCreateWithContext(self.mainContext, CGSizeMake(BUFFER_SIZE, 800), nil);
    self.offscreenContext = CGLayerGetContext(self.offscreenLayer);
    
    self.xPos = -BUFFER_SIZE;
}

-(void)drawRect:(NSRect)dirtyRect {
    static BOOL isFirst = NO;
    
    if (!isFirst) {
        isFirst = YES;
        
        [self firstDraw];
    }
    
    CGContextDrawLayerAtPoint(self.mainContext, CGPointMake(self.xPos + 100, 0), self.offscreenLayer);
    CGContextSetRGBStrokeColor(self.mainContext, 0.0f, 0.0f, 0.0f, 1.0f);
}

@end

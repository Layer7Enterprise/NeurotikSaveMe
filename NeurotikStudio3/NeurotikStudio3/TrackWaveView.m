//
//  TrackWaveView.m
//  NeurotikStudio3
//
//  Created by Seo Townsend on 6/10/13.
//  Copyright (c) 2013 Seo Townsend. All rights reserved.
//

#import "TrackWaveView.h"

@implementation TrackWaveView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{

    CGContextRef ref = [[[self window] graphicsContext] graphicsPort];
    
    CGContextSetRGBFillColor(ref, 1.f, 0.0f, 0.0f, 1.0f);
    CGContextFillRect(ref, CGRectMake(0.0f, 0.0f, 100.0f, 100.0f));
}

@end

//
//  TrackWaveView.h
//  NeurotikStudio3
//
//  Created by Seo Townsend on 6/10/13.
//  Copyright (c) 2013 Seo Townsend. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface TrackWaveView : NSView

//Add colors
- (void)addWaveColumn: (NSMutableArray *)colors;

//Offscreen data buffer
@property (atomic) CGContextRef offscreenContext;
@property (atomic) CGLayerRef offscreenLayer;

@property (atomic) float seekTime;          //Where the offset is drawn
@property (atomic) float recordOffsetTime;  //What offset we are rocrding from

@property (atomic) BOOL scrollToLatest;     //Scroll to end

@end

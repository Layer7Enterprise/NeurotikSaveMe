//
//  MainView.h
//  NeurotikStudio
//
//  Created by Seo Townsend on 6/3/13.
//  Copyright (c) 2013 Layer 7. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MainView : NSView

@property (retain, nonatomic) NSTimer *animationTimer;
@property CGContextRef offscreenContext;
@property CGContextRef mainContext;
@property CGLayerRef offscreenLayer;
@property (atomic) int xPos;

@end

//
//  TrackInfoView.h
//  NeurotikStudio3
//
//  Created by Seo Townsend on 6/9/13.
//  Copyright (c) 2013 Seo Townsend. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface TrackInfoView : NSView

@property (nonatomic) NSMutableArray *neuronNames;
@property (atomic) NSNumber *number;

-(void)drawNumber: (NSNumber *)number;

@end

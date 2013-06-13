//
//  FullTrackView.h
//  NeurotikStudio3
//
//  Created by Seo Townsend on 6/9/13.
//  Copyright (c) 2013 Seo Townsend. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "TrackInfoView.h"
#import "TrackDataView.h"
#import "TrackWaveView.h"

@interface FullTrackView : NSView

@property (nonatomic) TrackInfoView *trackInfoView;
@property (nonatomic) TrackDataView *trackDataView;
@property (nonatomic) TrackWaveView *trackWaveView;

@end

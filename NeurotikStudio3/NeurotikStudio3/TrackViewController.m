//
//  TrackViewController.m
//  NeurotikStudio3
//
//  Created by Seo Townsend on 6/9/13.
//  Copyright (c) 2013 Seo Townsend. All rights reserved.
//

#import "TrackViewController.h"

static TrackViewController *theTrackViewController;

void onDrop() {
    NSLog(@"Dropped packets detected...");
}

//Running colors, pushed on a negative change in idx
static NSMutableArray *colors;
static NSMutableArray *names;
static BOOL hasSetNames = NO;

void onData(struct NeuronDebugNetworkOutput_t output) {
    static int lastIdx = -1;
    
    //Push
    if (lastIdx > output.idx) {
        @autoreleasepool {
            if (!hasSetNames) {
                hasSetNames = YES;
                [[[theTrackViewController fullTrackView] trackInfoView] setNeuronNames:names];
                [[[theTrackViewController fullTrackView] trackInfoView] setNeedsDisplay:YES];
            }
            
            if ([theTrackViewController isRecording])
                [theTrackViewController addColorColumn:colors];
        }
        colors = [NSMutableArray new];
    }

    lastIdx = output.idx;
    
    if (!hasSetNames) {
        @autoreleasepool {
            [names addObject:[NSString stringWithFormat:@"%s", output.name]];
        }
    }
    
    if (output.inh > 0) {
        [colors addObject:[NSColor redColor]];
    } else if (output.lastSpikeTime == output.globalTime) {
        [colors addObject:[NSColor greenColor]];
    } else {
        [colors addObject:[NSColor brownColor]];
    }
}

@implementation TrackViewController

-(void)awakeFromNib {
    theTrackViewController = self;
    colors = [NSMutableArray new];
    names = [NSMutableArray new];
    
    NUDebugClientSetDropped(onDrop);
    NUDebugClientSetCallback(onData);
    NUDebugClientBegin("127.0.0.1");
}

- (void)addColorColumn: (NSMutableArray *)colors {
    [[[self fullTrackView] trackDataView] addColorColumn:colors];
    
    [self.trackTimeView setStringValue:[NSString stringWithFormat:@"%f", -self.fullTrackView.trackDataView.seekTime]];
}

- (IBAction)recordClick:(id)sender {
    if (self.isRecording) {
        self.isRecording = NO;
        [self.fullTrackView.trackDataView setScrollToLatest:NO];
    } else {
        self.isRecording = YES;
        [self.fullTrackView.trackDataView setScrollToLatest:YES];
    }
}

- (IBAction)gotoClick:(id)sender {
    int value = [[self.gotoField stringValue] intValue];
    [self.fullTrackView.trackDataView setSeekTime:-value];
    [self.trackTimeView setStringValue:[NSString stringWithFormat:@"%f", -self.fullTrackView.trackDataView.seekTime]];
}

- (IBAction)left:(id)sender {
    self.fullTrackView.trackDataView.seekTime += 10;
    [self.trackTimeView setStringValue:[NSString stringWithFormat:@"%f", -self.fullTrackView.trackDataView.seekTime]];
}

- (IBAction)right:(id)sender {
    self.fullTrackView.trackDataView.seekTime -= 10;
    [self.trackTimeView setStringValue:[NSString stringWithFormat:@"%f", -self.fullTrackView.trackDataView.seekTime]];
}

- (IBAction)changeTrackType:(id)sender {
    NSSegmentedControl *control = sender;
    
    //Discrete
    if ([control selectedSegment] == 0) {
        [self.fullTrackView.trackDataView setHidden:NO];
        [self.fullTrackView.trackWaveView setHidden:YES];
    } else {
        [self.fullTrackView.trackDataView setHidden:YES];
        [self.fullTrackView.trackWaveView setHidden:NO];
    }
}
@end

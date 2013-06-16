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
static NSMutableArray *values;
static BOOL hasSetNames = NO;

void onData(struct NeuronDebugNetworkOutput_t output) {
    static int lastIdx = -1;
    static int finalIdx = -1;
    
    static BOOL hasReset = NO;
    if (!hasReset) {
        if (lastIdx > output.idx) {
            hasReset = YES;
            lastIdx = output.idx;
            return;
        } else {
            lastIdx = output.idx;
            return;
        }
    }
    
    if (!hasSetNames) {
        @autoreleasepool {
            [names addObject:[NSString stringWithFormat:@"%s", output.name]];
        }
    }
    
    //Normalize voltages [-65.0V -> 40.0] => [0 -> 1.0]
    float rawVoltage = output.V;
    rawVoltage += 70.0f;
    rawVoltage /= 110.0f;
    NSNumber *value = [[NSNumber alloc] initWithFloat:rawVoltage];
    [values addObject:value];
    
    if (output.inh > 0) {
        [colors addObject:[NSColor redColor]];
    } else if (output.lastSpikeTime == output.globalTime) {
        [colors addObject:[NSColor yellowColor]];
    } else {
        [colors addObject:[NSColor brownColor]];
    }
    
    //Push
    if (lastIdx > output.idx) {
        @autoreleasepool {
            if (!hasSetNames) {
                hasSetNames = YES;
                [[[theTrackViewController fullTrackView] trackInfoView] setNeuronNames:names];
                [[[theTrackViewController fullTrackView] trackInfoView] setNeedsDisplay:YES];
            }
            
            if ([theTrackViewController isRecording]) {
                [theTrackViewController addColorColumn:colors];
                [theTrackViewController addWaveColumn:values];
            }
        }
        colors = [NSMutableArray new];
        values = [NSMutableArray new];
    }
    
    lastIdx = output.idx;
}

@implementation TrackViewController

-(void)awakeFromNib {
    theTrackViewController = self;
    colors = [NSMutableArray new];
    names = [NSMutableArray new];
    values = [NSMutableArray new];
    
    NUDebugClientSetDropped(onDrop);
    NUDebugClientSetCallback(onData);
    NUDebugClientBegin("127.0.0.1");
}

- (void)addColorColumn: (NSMutableArray *)colors {
    [[[self fullTrackView] trackDataView] addColorColumn:colors];

    [self.trackTimeView setStringValue:[NSString stringWithFormat:@"%f", -self.fullTrackView.trackDataView.seekTime]];
}

- (void)addWaveColumn: (NSMutableArray *)values {
    [[[self fullTrackView] trackWaveView] addWaveColumn:values];
}

- (IBAction)recordClick:(id)sender {
    if (self.isRecording) {
        self.isRecording = NO;
        [self.fullTrackView.trackDataView setScrollToLatest:NO];
        [self.fullTrackView.trackWaveView setScrollToLatest:NO];
    } else {
        self.isRecording = YES;
        [self.fullTrackView.trackDataView setScrollToLatest:YES];
        [self.fullTrackView.trackWaveView setScrollToLatest:YES];
    }
}

- (IBAction)gotoClick:(id)sender {
    int value = [[self.gotoField stringValue] intValue];
    [self.fullTrackView.trackDataView setSeekTime:-value];
    [self.fullTrackView.trackWaveView setSeekTime:-value];
    [self.trackTimeView setStringValue:[NSString stringWithFormat:@"%f", -self.fullTrackView.trackDataView.seekTime]];
}

- (IBAction)left:(id)sender {
    self.fullTrackView.trackDataView.seekTime += 10;
    self.fullTrackView.trackWaveView.seekTime += 10;
    [self.trackTimeView setStringValue:[NSString stringWithFormat:@"%f", -self.fullTrackView.trackDataView.seekTime]];
}

- (IBAction)right:(id)sender {
    self.fullTrackView.trackDataView.seekTime -= 10;
    self.fullTrackView.trackWaveView.seekTime -= 10;
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

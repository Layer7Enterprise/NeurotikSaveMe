//
//  TrackViewController.h
//  NeurotikStudio3
//
//  Created by Seo Townsend on 6/9/13.
//  Copyright (c) 2013 Seo Townsend. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "FullTrackView.h"
#import "DebugClient.h"

@interface TrackViewController : NSViewController

@property (weak) IBOutlet NSTextField *trackTimeView;
@property (weak) IBOutlet FullTrackView *fullTrackView;

@property (nonatomic) int currentTime;

- (void)addColorColumn: (NSMutableArray *)colors;
- (void)addWaveColumn: (NSMutableArray *)values;

- (IBAction)recordClick:(id)sender;
@property (atomic) BOOL isRecording;

- (IBAction)gotoClick:(id)sender;
@property (weak) IBOutlet NSTextField *gotoField;

- (void)keyDown:(NSEvent *)theEvent;
- (IBAction)left:(id)sender;
- (IBAction)right:(id)sender;

@end

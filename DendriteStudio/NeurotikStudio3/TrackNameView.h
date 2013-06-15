//
//  TrackNameView.h
//  DendriteStudio
//
//  Created by Seo Townsend on 6/14/13.
//  Copyright (c) 2013 Seo Townsend. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TrackDataView.h"
#import "TrackInfoView.h"

@interface TrackNameView : NSObject
@property (weak) IBOutlet NSPopUpButton *selectNameBox;
@property (weak) IBOutlet NSTextField *selectedName;
@property (nonatomic) TrackDataView *trackDataView;
@property (nonatomic) TrackInfoView *trackInfoView;
@property (weak) IBOutlet NSView *mainView;

- (IBAction)onNameSelected:(id)sender;
- (void)addName: (NSString *)name;

@end

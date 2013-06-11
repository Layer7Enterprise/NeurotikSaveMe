#import "FullTrackView.h"

#define kTrackInfoSize 150.0f

@implementation FullTrackView

-(void)awakeFromNib {
    self.trackInfoView = [TrackInfoView new];
    self.trackDataView = [TrackDataView new];
    self.trackWaveView = [TrackWaveView new];

    //Add track info
    NSRect trackInfoRect = self.frame;
    trackInfoRect.size.width = kTrackInfoSize;
    [self.trackInfoView setFrame: trackInfoRect];
    [self addSubview:self.trackInfoView];
    
    //Add track data
    NSRect trackDataRect = self.frame;
    trackDataRect.size.width -= kTrackInfoSize; //Rest of view is data
    trackDataRect.origin.x += kTrackInfoSize;
    [self.trackDataView setFrame: trackDataRect];
    [self addSubview:self.trackDataView];
    
    //Add track data
    NSRect trackWaveRect = self.frame;
    trackWaveRect.size.width -= kTrackInfoSize; //Rest of view is data
    trackWaveRect.origin.x += kTrackInfoSize;
    [self.trackWaveView setFrame: trackWaveRect];
    [self.trackWaveView setHidden:YES];
    [self addSubview:self.trackWaveView];
}

-(void)drawRect:(NSRect)dirtyRect {
    
}

@end
#import "FullTrackView.h"

#define kTrackInfoSize 150.0f

@implementation FullTrackView

-(void)awakeFromNib {
    self.trackInfoView = [TrackInfoView new];
    self.trackDataView = [TrackDataView new];

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
}

-(void)drawRect:(NSRect)dirtyRect {
    
}

@end
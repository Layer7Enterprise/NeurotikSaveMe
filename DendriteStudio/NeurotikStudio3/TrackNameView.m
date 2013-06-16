#define kTrackInfoSize 150

#import "TrackNameView.h"
#import "DebugClient.h"
#import "TrackWaveView.h"
#import "TrackInfoView.h"

static TrackNameView *theTrackNameView;
static NSMutableArray *dendriteNames;
static NSMutableArray *names;
static NSMutableArray *values;
pthread_mutex_t mutex;

void onNData(struct NeuronDebugNetworkOutput_t output) {
    static int lastIdx = -1;
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

    
    static BOOL hasSetNames = NO;
    if (!hasSetNames) {
        @autoreleasepool {
            [names addObject:[NSString stringWithFormat:@"%s", output.name]];
            [theTrackNameView addName:[NSString stringWithFormat:@"%s", output.name]];
        }
    }

    if (lastIdx > output.idx) {
        hasSetNames = YES;
        return;
    }
    
    lastIdx = output.idx;
}

static BOOL hasStartedOver = NO;
static BOOL hasSetNames = NO;
void onDData(struct NeuronDendriteDebugNetwork_t info) {
    if (!hasStartedOver) {
        if (info.isFirst) {
            hasStartedOver = YES;
            return;
        } else {
            return;
        }
    }
    
    @autoreleasepool {
        //At this point w've gotten everytihng
        if (!hasSetNames) {
            [dendriteNames addObject:[NSString stringWithFormat:@"%s", info.name]];
            
            if (info.isFirst) {
                [[theTrackNameView trackInfoView] setNeuronNames:dendriteNames];
                [[theTrackNameView trackInfoView] setNeedsDisplay:YES];
                hasSetNames = YES;
            }
        } else {
            [values addObject:[NSNumber numberWithFloat:info.weight / NEURON_TH]];
            
            @autoreleasepool {
            if (info.isFirst) {
                [theTrackNameView.trackDataView addBarColumn:values];
                values = [NSMutableArray new];
            }
        }
        }
    }
}

@implementation TrackNameView

- (IBAction)onNameSelected:(id)sender {
    [self.selectedName setStringValue:[self.selectNameBox titleOfSelectedItem]];
    NUDebugClientDendriteTrack([[self.selectNameBox titleOfSelectedItem] UTF8String]);
    
    [dendriteNames removeAllObjects];
    [[self trackInfoView] setNeuronNames:dendriteNames];
    [[self trackInfoView] setNeedsDisplay:YES];
    
    hasSetNames = NO;
    hasStartedOver = NO;
}

- (void)addName: (NSString *)name {
    [[self selectNameBox] addItemWithTitle:name];
}

-(void)awakeFromNib {
    NUDebugClientDendriteBegin("127.0.0..1");
    NUDebugClientBegin("127.0.0.1");
    NUDebugClientDendriteSetCallback(onDData);
    NUDebugClientSetCallback(onNData);
    
    dendriteNames = [[NSMutableArray alloc] init];
    names = [NSMutableArray new];
    values = [NSMutableArray new];
    
    pthread_mutex_init(&mutex, NULL);
    
    theTrackNameView = self;
    
    self.trackInfoView = [TrackInfoView new];
    self.trackDataView = [TrackDataView new];
    
    //Add track info
    NSRect trackInfoRect = self.mainView.frame;
    trackInfoRect.size.width = kTrackInfoSize;
    [self.trackInfoView setFrame: trackInfoRect];
    [self.mainView addSubview:self.trackInfoView];
    
    //Add track data
    NSRect trackDataRect = self.mainView.frame;
    trackDataRect.size.width -= kTrackInfoSize; //Rest of view is data
    trackDataRect.origin.x += kTrackInfoSize;
    [self.trackDataView setFrame: trackDataRect];
    [self.mainView addSubview:self.trackDataView];
}

@end

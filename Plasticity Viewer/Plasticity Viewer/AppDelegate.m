#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    self.debugManager = [DebugManager new];
    self.debugManager.delegate = self;
    [self.debugManager begin];
}

#pragma mark NSMenu (neuron select) delegate
-(void)menu:(NSMenu *)menu willHighlightItem:(NSMenuItem *)item {
    NSLog(@"Changed");
}

#pragma mark DebugManager delegates
-(void)neuronListHasUpdated:(NeuronInfo *)neuron {
    NSString *selectedName = [[self.selectNeuron selectedItem] title];
    NSString *title = [NSString stringWithFormat: @"%@ %d", neuron.name, neuron.idx];
    
    if ([selectedName isEqualToString:title]) {
        [self.plasticity setStringValue:[[NSNumber numberWithFloat:neuron.plasticity] stringValue]];
    }
}

-(void)neuronListHasAdded:(NeuronInfo *)neuron {
    NSString *name = neuron.name;
    int idx = neuron.idx;
    NSString *title = [NSString stringWithFormat: @"%@ %d", name, idx];
    
    [self.selectNeuron addItemWithTitle:title];
}

@end

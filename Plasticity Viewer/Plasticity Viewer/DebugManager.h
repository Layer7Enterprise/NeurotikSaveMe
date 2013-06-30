#import <Foundation/Foundation.h>
#import <arpa/inet.h>
#import "Params.h"

@interface NeuronInfo : NSObject 

@property int idx;
@property NSString *name;
@property float plasticity;

-(NeuronInfo *)initWithName: (NSString *)name andIdx: (int)idx;

@end

@protocol DebugManagerDelegate <NSObject>

//The list has added a neuron
-(void)neuronListHasAdded: (NeuronInfo *)neuron;

//The list has updated values
-(void)neuronListHasUpdated: (NeuronInfo *)list;

@end

@interface DebugManager : NSObject

@property int socket;
@property struct sockaddr_in addr;
@property (atomic) NSMutableArray *neuronList;
@property id<DebugManagerDelegate> delegate;

-(void)begin;

@end

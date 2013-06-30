#import "DebugManager.h"

#define DEBUG_PORT 3002

@implementation NeuronInfo

-(NeuronInfo *)initWithName: (NSString *)name andIdx: (int)idx {
    self.name = name;
    self.idx = idx;
    self.plasticity = 1.0f;
    
    return self;
}

@end

@implementation DebugManager

-(DebugManager *)init {
    self.neuronList = [NSMutableArray new];
    
    return self;
}

//Find all the possible neuron names and idx, etc.
-(void)addNeuronInfoIfRequired: (struct NeuronDebugNetworkOutput_t *)params {
    for (NeuronInfo *info in self.neuronList) {
        if (info.idx == params->idx) {
            info.plasticity = params->plasticity;
            [self.delegate neuronListHasUpdated:info];
            return;
        }
    }
    
    NeuronInfo *neuronInfo = [[NeuronInfo alloc] initWithName:[NSString stringWithFormat: @"%s", params->name] andIdx:params->idx];
    [self.neuronList addObject:neuronInfo];
    
    [self.delegate neuronListHasAdded:neuronInfo];
}

-(void)begin {
    self.socket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DEBUG_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    int res = bind(self.socket, (struct sockaddr *)&addr, sizeof(addr));
    if (res < 0) {
        [NSException raise:@"Socket Erorr" format:@"Could not bind %s", strerror(errno)];
    }
    
    dispatch_async(dispatch_queue_create("debugQueue", NULL), ^{
        while (1) {
            char buffer[1000];
            ssize_t len = recv(self.socket, buffer, sizeof(buffer), 0);
            if (len < 0) {
                [NSException raise:@"Socket Erorr" format:@"Could not recv on socket"];
                return;
            } else if (len != sizeof(struct NeuronDebugNetworkOutput_t)) {
                [NSException raise:@"Socket Erorr" format:@"Was not correct length"];
                return;
            }
            
            struct NeuronDebugNetworkOutput_t *params = (struct NeuronDebugNetworkOutput_t *)buffer;
            
            [self addNeuronInfoIfRequired:params];
        }
    });
}

@end

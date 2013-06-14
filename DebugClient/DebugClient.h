#ifndef DEBUG_CLIENT_H_
#define DEBUG_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include "../Core/Params.h"

//Start recieving
void NUDebugClientBegin(const char ip[]);

//Set callback for recieving data
void NUDebugClientSetCallback(void (*)(struct NeuronDebugNetworkOutput_t output));

//Set callback for a dropped packet
void NUDebugClientSetDropped(void (*)());

//Dendrite tracking

//Set the name to record
void NUDebugClientDendriteTrack(const char *name);

//Start recieving dentrite information
void NUDebugClientDendriteBegin(const char ip[]);

//Set callback for receiving data
void NUDebugClientDendriteSetCallback(void (*)(struct NeuronDDebugNetwork_t output));

#endif

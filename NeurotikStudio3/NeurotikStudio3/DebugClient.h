#ifndef DEBUG_CLIENT_H_
#define DEBUG_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include "Params.h"

void NUDebugClientBegin(const char ip[]);

//Set callback for recieving data
void NUDebugClientSetCallback(void (*)(struct NeuronDebugNetworkOutput_t output));

//Set callback for a dropped packet
void NUDebugClientSetDropped(void (*)());

#endif

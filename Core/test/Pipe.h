#ifndef PIPE_H_
#define PIPE_H_

#include <list>
#include <queue>
#include <sys/stat.h>
#include <semaphore.h>
#include "errno.h"

#define MAX_UNIT_MESSAGE_SIZE 1000

#define NUM_UNIT_MESSAGE_TYPES 1
enum UnitMessageType_t {
  UMCoreInputImpulseOnTimer = 0,  //The core current input when the millisecond timer goes off
};

struct UnitMessageInfo_t {
  UnitMessageType_t type;
  unsigned char msg[MAX_UNIT_MESSAGE_SIZE];
  int len;
};

void BeginUnitPipe();

//Send a message to unit
void SendUnitMessage(UnitMessageType_t type, const unsigned char *msg, int len);

//Get one unit message, return number of messages left
UnitMessageInfo_t GetUnitMessage(UnitMessageType_t type);

#endif

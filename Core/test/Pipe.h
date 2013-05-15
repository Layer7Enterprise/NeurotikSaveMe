#ifndef PIPE_H_
#define PIPE_H_

#include <queue>
#include <sys/stat.h>
#include <semaphore.h>
#include "errno.h"

#define MAX_UNIT_MESSAGE_SIZE 1000

enum UnitMessage_t {
  UMCoreInputImpulseOnTimer  //The core current input when the millisecond timer goes off
};

struct UnitMessageInfo_t {
  UnitMessage_t type;
  unsigned char msg[MAX_UNIT_MESSAGE_SIZE];
  int len;
};

void BeginUnitPipe();

void SendToUnits(UnitMessage_t type, const unsigned char *msg, int len);

//Get one unit message, return number of messages left
void GetUnitMessage(UnitMessage_t *message);

#endif

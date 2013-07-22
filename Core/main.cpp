#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <sys/time.h>
#include <event.h>
#include "Net.h"
#include "Schema.h"
#include "Params.h"
#include "GetNet.h"
#include "CoreManager.h"
#include "Timer.h"

/*#define SCHEMA_FILE "config/schema.txt"*/
/*#define NET_FILE "config/net.txt"*/
#define NET_IP "127.0.0.1"
#define NET_PORT 3000  //Input = 3000, Output = 3001

//#include "test/Units.h"
//

int main() {
  SetupTimer();

  Schema_t schema;
  Params_t params;

  //Load schema (Port, name, etc)
  GetSchema("config/schema.txt", &schema);

  //Load params (net list, large chunk of memory, etc)
  GetNet("config/net.txt", schema, &params);

  //Setup network receiving
  NetRcvBegin(NET_IP, NET_PORT, CoreOnImpulse);
  NetSendBegin(NET_IP, NET_PORT+1);

  //Setup debug
  NetSendBeginDebug(NET_IP, NET_PORT+2);
  NetControlBegin(NET_IP, NET_PORT+3, &params);

  NetDendriteDebugBegin(NET_IP, NET_PORT+4);

  CoreBegin(&params);
  puts(">Core online!");

  HandleEvents();  

  return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <sys/time.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <dispatch/dispatch.h>
#include "Net.h"
#include "Schema.h"
#include "Params.h"
#include "GetNet.h"
#include "Timer.h"

#define SCHEMA_FILE "config/schema.txt"
#define NET_FILE "config/net.txt"

#include "test/Units.h"

void Call() {
}

int main()
{
  OnMillisecond(Call);

  while (1) {
#ifdef OSX
    dispatch_main();
#endif
  }
}

/*std::vector<int> *nextInput = NULL;*/
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//void callback(const char *buffer) {
  //int len = sizeof(buffer);

  //if (!nextInput) {
    //nextInput = new std::vector<int>();
    //nextInput->resize(len);
    //for (int i = 0; i < nextInput->size(); ++i)
      //(*nextInput)[i] = 0;
  //}

  //pthread_mutex_lock(&mutex);
    //for (int i = 0; i < strlen(buffer); ++i) {
      //if (buffer[i] == '1')
        //(*nextInput)[i] |= 1;
    //}
  //pthread_mutex_unlock(&mutex);
//}

//void read() {
  //pthread_mutex_lock(&mutex);
    //for (int i = 0; i < nextInput->size(); ++i) {
      //printf("%d", (*nextInput)[i]);
    //}
    //printf("\n");
  //pthread_mutex_unlock(&mutex);
//}

//int main() {
  ////RunUnits();

  ////Load schema (Port, name, etc)
  //Schema_t schema;
  //GetSchema(SCHEMA_FILE, &schema);

  ////Load params (net list, large chunk of memory, etc)
  //Params_t params;
  //GetNet(NET_FILE, schema, &params);

  //[>NetSendBegin("224.4.5.6", 3000);<]
  ////for (int i = 0; i < 100; ++i) 
    //[>NetSend("test", 4);<]
  
  //NetRcvBegin("224.4.5.6", 3001, callback);

  //while (true);

  //return 0;
/*}*/
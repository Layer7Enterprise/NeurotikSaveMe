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
#include "CoreManager.h"
#include "Timer.h"

#define SCHEMA_FILE "config/schema.txt"
#define NET_FILE "config/net.txt"

#include "test/Units.h"

static sem_t *isDone;
void done() {
  alarm(0);
  sem_post(isDone);
}

void handler(int) {}

#define UNIT_ASYNC_MAX_WAIT 4
typedef void (*Done_t)();
void it(const char *str, void (^testFunction)(Done_t)) {
  //Start timeout
  alarm(UNIT_ASYNC_MAX_WAIT);
  signal(SIGALRM, handler);
  
  //Call the function
  testFunction(done);

  //What happened?  Did we get a semaphore or did an alarm get called
  int res = sem_wait(isDone);
  if (res < 0) {
    fprintf(stderr, "Unit test failed.  Done was not called in %i seconds\n", UNIT_ASYNC_MAX_WAIT);
    exit(EXIT_FAILURE);
  }

}

void SetupUnitAsync() {
  srand(time(NULL));

  char randomName[20];
  randomName[19] = 0;
  for (int i = 0; i < sizeof(randomName)-1; ++i)
    randomName[i] = 'A'+rand()%20;

  isDone = sem_open(randomName, O_CREAT, O_RDWR, 0);

  if (!isDone) {
    fprintf(stderr, "Could not create semaphore for async done semaphore.  Error code %d\n", errno);
    exit(EXIT_FAILURE);
  }

}

int main () {
  SetupUnitAsync();
  printf("hey0\n");

  
  it("is awesome", ^(Done_t done) {
      printf("yo\n");
      done();
  });

  it("is also awesome", ^(Done_t done) {
      printf("yo\n");
      done();
  });
  
  return 0;
}


/*std::vector<int> *nextInput = NULL;*/
//pthread_mutex_t nextInputMutex = PTHREAD_MUTEX_INITIALIZER;
//void callback(const char *buffer) {
  //int len = sizeof(buffer);

  //if (!nextInput) {
    //nextInput = new std::vector<int>();
    //nextInput->resize(len);
    //for (int i = 0; i < nextInput->size(); ++i)
      //(*nextInput)[i] = 0;
  //}

  //pthread_mutex_lock(&nextInputMutex);
    //for (int i = 0; i < strlen(buffer); ++i) {
      //if (buffer[i] == '1')
        //(*nextInput)[i] |= 1;
    //}
  //pthread_mutex_unlock(&nextInputMutex);
/*}*/


/*int main() {*/
  //RunUnits();

  ////Load schema (Port, name, etc)
  //Schema_t schema;
  //GetSchema(SCHEMA_FILE, &schema);

  ////Load params (net list, large chunk of memory, etc)
  //Params_t params;
  //GetNet(NET_FILE, schema, &params);

  //CoreBegin(&params);

  //[>NetSendBegin("224.4.5.6", 3000);<]
  ////for (int i = 0; i < 100; ++i) 
    //[>NetSend("test", 4);<]
  
  //NetRcvBegin("224.4.5.6", 3001, CoreOnImpulse);

  //while (true) {
//#ifdef OSX
    //dispatch_main();
//#endif
  //}

  //return 0;
/*}*/

#include "CoreManager.h"

//Debug?
#include <CCup.h>

//Inbound segment (INTEGER BASED!)
pthread_mutex_t runningSegmentLock = PTHREAD_MUTEX_INITIALIZER;
static unsigned char *runningSegment = NULL;

//Global params (Check main for loading)
static Params_t *params = NULL;

void CoreTick() {
  pthread_mutex_lock(&runningSegmentLock);

#ifdef CCUP
  //Is segment blank?
  int isBlank = 1;
  for (int i = 0; i < NET_INPUT_LEN(params); ++i) {
    if (runningSegment[i] == 1) {
      isBlank = 0;
      break;
    }
  }

  CCSend("CoreTickInput", (const char *)runningSegment, NET_INPUT_LEN(params));

  if (!isBlank)
    CCSend("CoreTickInputNotBlank", (const char *)runningSegment, NET_INPUT_LEN(params));
#endif

  //Load into core
  //CoreLoad
  

  //Get data from core and send it out
  NetSend("00", 2);

  //Reset running
  memset(runningSegment, 0, NET_INPUT_LEN(params));

  pthread_mutex_unlock(&runningSegmentLock);
}

void CoreBegin(Params_t *p) {
  params = p;

  //Start the core processor
  OnMillisecond(CoreTick);

  //Allocate running segment
  runningSegment = new unsigned char[NET_INPUT_LEN(params)];
  memset(runningSegment, 0, NET_INPUT_LEN(params));

  //Create a mutex
  pthread_mutex_init(&runningSegmentLock, NULL);

  //Set segment to all 0's
  for (int i = 0; i < NET_INPUT_LEN(params); ++i)
    runningSegment[i] = 0;
}

void CoreOnImpulse(const unsigned char *impulse, int len) {
  CCSend("CoreOnImpulseStart", (const char *)impulse, len);

  //Confirm that the size of the impulse vector is right
  if (len != NET_INPUT_LEN(params)) {
    fprintf(stderr, "Impulse size was not the same as the params length %d != %d\n", NET_INPUT_LEN(params), len);
    exit(EXIT_FAILURE);
  }

  //OR the running segment
  pthread_mutex_lock(&runningSegmentLock);
  for (int i = 0; i < len; ++i) {
    if (impulse[i] == 1) {
      runningSegment[i] |= 1;
    } else if (impulse[i] == 0) {
      //Do nothing
    } else {
      fprintf(stderr, "Impulse input was not an 1 or 0.  it was %d\n", impulse[i]);
      exit(EXIT_FAILURE);
    }
  }
  pthread_mutex_unlock(&runningSegmentLock);
  CCSend("CoreOnImpulseEnd", (const char *)runningSegment, len);
}

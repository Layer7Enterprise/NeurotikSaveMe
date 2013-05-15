#include "CoreManager.h"

//Inbound segment (INTEGER BASED!)
pthread_mutex_t runningSegmentLock = PTHREAD_MUTEX_INITIALIZER;
static unsigned char *runningSegment = NULL;

//Global params (Check main for loading)
static Params_t *params = NULL;

void CoreTick() {
  pthread_mutex_lock(&runningSegmentLock);

  SendUnitMessage(UMCoreInputImpulseOnTimer, runningSegment, NET_INPUT_LEN(params));

  pthread_mutex_unlock(&runningSegmentLock);
}

void CoreBegin(Params_t *p) {
  params = p;

  //Start the core processor
  OnMillisecond(CoreTick);

  //Allocate running segment
  runningSegment = new unsigned char[NET_INPUT_LEN(params)];
  memset(runningSegment, 0, NET_INPUT_LEN(params));
}


void CoreOnImpulse(const char *impulse) {
  //Confirm that the size of the impulse vector is right
  int len = sizeof(impulse);
  if (len != NET_INPUT_LEN(params)) {
    fprintf(stderr, "Impulse size was not the same as the params length %d != %d\n", NET_INPUT_LEN(params), len);
    exit(EXIT_FAILURE);
  }

  //OR the running segment (NOTE: impulse is an ASCII string, segment is integer)
  pthread_mutex_lock(&runningSegmentLock);
  for (int i = 0; i < len; ++i) {
    if (impulse[i] == '1') {
      runningSegment[i+params->networkInStartPos] |= 1;
    } else if (impulse[i] == '0') {
      //Do nothing
    } else {
      fprintf(stderr, "Impulse input was not an ASCII 1 or 0.  it was %c\n", impulse[i]);
      exit(EXIT_FAILURE);
    }
  }
  pthread_mutex_unlock(&runningSegmentLock);
}

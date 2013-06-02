#include "CoreManager.h"

//Debug?
#include <CCup.h>

//Inbound segment (INTEGER BASED!)
pthread_mutex_t runningSegmentLock = PTHREAD_MUTEX_INITIALIZER;
static unsigned char *runningSegment = NULL;
static char *outputSegment = NULL;

//Global params (Check main for loading)
static Params_t *params = NULL;

void CoreTick() {
  static int id = 0;
  ++id;
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

  if (!isBlank) {
    CCSend("CoreTickInputNotBlank", (const char *)runningSegment, NET_INPUT_LEN(params));
  }
#endif

  for (int idx = 0; idx < params->NN; ++idx)
    CoreTick(idx, params);

  //Get data from core and send it out
  for (int i = 0; i < NET_OUTPUT_LEN(params); ++i) {
    outputSegment[i] = '0';

    if (i < NET_INPUT_LEN(params))
      outputSegment[i] = runningSegment[i] ? '1' : '0';
  }

  if (!isBlank) {
    //Add a count (5 ASCII bytes) to the outgoing to track dropped packets
    static int byteCount = 0;
    static char *outputSegmentWithCount = new char[NET_OUTPUT_LEN(params)+5];

    //Make sure you sprintf first because it generates the new line
    sprintf((char *)outputSegmentWithCount, "%05d", byteCount);
    memcpy(outputSegmentWithCount+5, outputSegment, NET_OUTPUT_LEN(params));

    int outCount = NET_OUTPUT_LEN(params)+5;
    NetSend((char *)outputSegmentWithCount, NET_OUTPUT_LEN(params)+5);
    ++byteCount;
    byteCount = byteCount % 99999;  //No more than 5 digits
  }

  //Reset running
  memset(runningSegment, 0, NET_INPUT_LEN(params));

  //Update global clock
  ++params->globalTime;

  pthread_mutex_unlock(&runningSegmentLock);
}

void CoreBegin(Params_t *p) {
  params = p;

  //Start the core processor
  OnMillisecond(CoreTick);

  //Allocate running segment
  runningSegment = new unsigned char[NET_INPUT_LEN(params)];
  memset(runningSegment, 0, NET_INPUT_LEN(params));

  outputSegment = new char[NET_OUTPUT_LEN(params)];

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

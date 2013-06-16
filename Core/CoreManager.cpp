#include "CoreManager.h"

//Debug?
//#include <CCup.h>

//Inbound segment (INTEGER BASED!)
pthread_mutex_t runningSegmentLock = PTHREAD_MUTEX_INITIALIZER;
static unsigned char *runningSegment = NULL;
static char *outputSegment = NULL;

//Global params (Check main for loading)
static Params_t *params = NULL;

void CoreTick() {
  pthread_mutex_lock(&runningSegmentLock);

  //Is segment blank?
  int isBlank = 1;
  for (int i = 0; i < NET_INPUT_LEN(params); ++i) {
    if (runningSegment[i] == 1) {
      isBlank = 0;
      break;
    }
  }

#ifdef CCUP
  CCSend("CoreTickInput", (const char *)runningSegment, NET_INPUT_LEN(params));

  if (!isBlank) {
    CCSend("CoreTickInputNotBlank", (const char *)runningSegment, NET_INPUT_LEN(params));
  }
#endif

  //Copy running segment into the core (With the right I)
  for (int i = 0; i < NET_INPUT_LEN(params); ++i) {
    if (runningSegment[i] == 1)
      params->nI[i+params->networkInStartPos] = NEURON_TH;
  }

  //########################################
  //Execute core
  for (int idx = 0; idx < params->NN; ++idx)
    CoreTick(idx, params);
  //########################################

  //########################################
  //Dump debug information
  for (auto i = params->neuronDebugLocations->begin(); i != params->neuronDebugLocations->end(); ++i) {
    //For detecting dropped packets
    static int debugCount = 0;

    int idx = *i;
    NeuronDebugNetworkOutput_t debugOutput;

    //Basic params
    debugOutput.count = debugCount;
    debugOutput.globalTime = params->globalTime;
    debugOutput.V = params->nV[idx];
    debugOutput.U = params->nU[idx];

    std::string name = params->neuronLocationToName->operator[](idx);
    strcpy(debugOutput.name, name.c_str());

    debugOutput.idx = idx;
    debugOutput.type = params->nType[idx];
    debugOutput.lastSpikeTime = params->nLastSpikeTime[idx];
    debugOutput.inh = params->nInh[idx];

    //Dendrite potentiation
    NetSendDebug((char *)&debugOutput, sizeof(NeuronDebugNetworkOutput_t));

    ++debugCount;
  }

  if (params->debugDendriteIdx != -1 && params->globalTime % 40 == 0) {
    for (int i = 0; i < params->ND; ++i) {
      //Skip non-connected
      int dIndex = params->debugDendriteIdx*params->ND + i;

      int subIndex = -1;
      if (params->dConnections[dIndex] < 0)
        break;
      else
        subIndex = params->dConnections[dIndex];

      NeuronDendriteDebugNetwork_t debugOutput;
      strcpy(debugOutput.name, params->neuronLocationToName->operator[](subIndex).c_str());
      debugOutput.weight = params->dWeights[dIndex];
      debugOutput.idx = subIndex;

      //Is this the first neuron in the set?
      debugOutput.isFirst = (i == 0);

      NetDendriteDebugSend(&debugOutput, sizeof(NeuronDendriteDebugNetwork_t));
    }
  }
  //########################################

  //Get data from core and send it out
  int outputIsBlank = 1;
  for (int i = 0; i < NET_OUTPUT_LEN(params); ++i) {
    int isSpike = params->nLastSpikeTime[i+params->networkOutStartPos] == params->globalTime;
    outputSegment[i] = isSpike ? '1' : '0'; 

    if (isSpike)
      outputIsBlank = 0;
  }

  if (!outputIsBlank) {
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
#ifdef CCUP
  CCSend("CoreOnImpulseStart", (const char *)impulse, len);
#endif

  //Confirm that the size of the impulse vector is right
  if (len != NET_INPUT_LEN(params)) {
    fprintf(stderr, "Impulse size was not the same as the params length %d != %d\n", NET_INPUT_LEN(params), len);
    return;
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
#ifdef CCUP
  CCSend("CoreOnImpulseEnd", (const char *)runningSegment, len);
#endif
}

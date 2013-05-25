#include "Core.h"

#include <CCup.h>

//Test the initial params
void TestInitialParams(int idx, int NN, int ND, int globalTime, const int *dConnection, const int *dDelay, float *dWeight, int *dLastSpikeTime, long long *dSpikeQue, float v, float u, float I, int lastSpikeTime, int inh, int inhibitoryTime, int ib, NeuronType_t type) {
#ifdef CCUP
  NeuronSnapshot_t initialParamsSnapshot;
  initialParamsSnapshot.idx = idx;
  initialParamsSnapshot.NN = NN;
  initialParamsSnapshot.ND = ND;
  initialParamsSnapshot.globalTime = globalTime;
  initialParamsSnapshot.dConnection = dConnection;
  initialParamsSnapshot.dDelay = dDelay;
  initialParamsSnapshot.dWeight = dWeight;
  initialParamsSnapshot.dLastSpikeTime = dLastSpikeTime;
  initialParamsSnapshot.dSpikeQue = dSpikeQue;
  initialParamsSnapshot.v = v;
  initialParamsSnapshot.u = u;
  initialParamsSnapshot.I = I;
  initialParamsSnapshot.lastSpikeTime = lastSpikeTime;
  initialParamsSnapshot.inh = inh;
  initialParamsSnapshot.ib = ib;
  initialParamsSnapshot.inhibitoryTime = inhibitoryTime;
  initialParamsSnapshot.type = type;

  CCSend("CoreInitialParams", (char *)&initialParamsSnapshot, sizeof(NeuronSnapshot_t));
#endif
}

//1 Millisecond
void CoreTick(int idx, Params_t *params) {
   //Global
   static int &globalTime = params->globalTime;
   static int NN = params->NN;
   static int ND = params->ND;

   //My dendrites
   const int *dConnection = params->dConnections + idx*params->ND;
   const int *dDelay = params->dDelays + idx*params->ND;
   float *dWeight = params->dWeights + idx*params->ND;
   int *dLastSpikeTime = params->dLastSpikeTimes + idx*params->ND;
   long long *dSpikeQue = params->dSpikeQues + idx*params->ND;

   //My internal params
   float &v = *(params->nV + idx);
   float &u = *(params->nU + idx);
   float &I = *(params->nI + idx);
   int &lastSpikeTime = *(params->nLastSpikeTime + idx);
   int &inh = *(params->nInh + idx);
   int inhibitoryTime = *(params->nInhibitoryTime + idx);
   int ib = *(params->nIb + idx);
   NeuronType_t type = *(params->nType + idx);

   if (globalTime == 0)
   TestInitialParams(idx, NN, ND, globalTime, dConnection, dDelay, dWeight, dLastSpikeTime, dSpikeQue, v, u, I, lastSpikeTime, inh, inhibitoryTime, ib, type);
}

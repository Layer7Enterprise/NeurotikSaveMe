#include "Core.h"

//Enable testing
#include <CCup.h>

#include "Test.h"

//Only run if it's the first round
#define only_first if (globalTime == 0)

//Called every millisecond
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

   //Send an initial snapshot (Only when CCUp is included)
   only_first SendSnapshot("CoreInitialParams", idx, NN, ND, globalTime, dConnection, dDelay, dWeight, dLastSpikeTime, dSpikeQue, v, u, I, lastSpikeTime, inh, inhibitoryTime, ib, type);

   //Dendrites
   for (int i = 0; i < ND; ++i) {
     //Is this an active dendrite
     if (dConnection[i] < 0) {
       static int falseValue = 0;
       only_first CCSend("CoreDendriteIsActive", 0);

       break;
     } else {
       only_first CCSend("CoreDendriteIsActive", 1);
     }
   }
}

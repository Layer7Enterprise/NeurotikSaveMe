#include "Core.h"

//Enable testing
#include <CCup.h>

#include "Test.h"

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
   if (globalTime == 0)
     SendSnapshot("CoreInitialParams", idx, NN, ND, globalTime, dConnection, dDelay, dWeight, dLastSpikeTime, dSpikeQue, v, u, I, lastSpikeTime, inh, inhibitoryTime, ib, type);

   //Updates and Checks
   //############################################################
   //Intrinsically burst
   /*if ((globalTime % ib) == 0)*/
     /*I = 24.0f;*/
   
   //Dendrites
   //############################################################
   static bool shouldNotifyOfActiveDendrites = true;
   for (int i = 0; i < ND; ++i) {
     //Is this an active dendrite
     if (dConnection[i] < 0) {
       //Send the running dendrites to the unit test
       //if (shouldNotifyOfActiveDendrites) CCSend("CoreDendriteIsActive", 1, sizeof(int));

       break;
     } else {
     }
   }

   shouldNotifyOfActiveDendrites = false;
}

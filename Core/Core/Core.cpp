#include "Core.h"

//Enable testing
#include <CCup.h>

#include "Test.h"

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
   if (ib && (globalTime % ib == 0)) {
     I = 24.0f;
   }

  //Dendrites
  for (int i = 0; i < ND; ++i) {
    //Is this an active dendrite
    if (dConnection[i] < 0) {
      only_first CCSend("CoreDendriteIsActive", false);
       break;
     } else {
       only_first CCSend("CoreDendriteIsActive", true);

       //We got a message from the axon
       if (params->nLastSpikeTime[dConnection[i]] == globalTime-1) {
       //Schedule the dendrite to fire in the near future (64bit bit que)
         dSpikeQue[i] |= (long long)1 << (dDelay[i]-1);
       }

      //This dendrite is supposed to fire now
      if (dSpikeQue[i] & 1) {
        dLastSpikeTime[i] = globalTime;
         
        //Only for GLU class neurons
        if (params->nType[dConnection[i]] & GLU) {
          I += dWeight[i];
          //Neg pot
          if (!(type & GABA) && !(type & NO_LRN)) {
            int deltaTime = globalTime - lastSpikeTime; //Previous Neuron's spike
            float delta = 0.05*exp(-deltaTime / NEURON_T0);

            if (deltaTime < NEURON_T0)
              dWeight[i] -= delta;

            if (dWeight[i] < 0.0f)
              dWeight[i] = 0.0f;
          }
        }

        //Only for prior neurons that are GABA
        if (params->nType[dConnection[i]] & GABA) {
          dLastSpikeTime[i] = globalTime;
          inh = params->nInhibitoryTime[dConnection[i]];

          //If i'm gaba or glutamagenic
          if (type & GLU) {
            v = NEURON_GLU_C;	
          } else if (type & GABA) {
            v = NEURON_GABA_C;
          }

          I = 0;
        }
      }

      //Move que
      dSpikeQue[i] >>= 1;
    }
  }

#pragma region Core Update

  //Reduce the switching dynamics to tighten the neuron up (Logistic equation)
  //22 is related to how the threshold is about 20, we want to make sure we're in
  //the range when we hit ~22
  I *= 1 / (1 + exp(100.0f*(22.0f - I)));

  if (I > NEURON_TH*1.5f)
    I = NEURON_TH*1.5f;

  //Update neuron
  if (inh <= 0) {
    v += 0.5f*(0.04f*v*v + 5.0f*v + 140.0f - u + I);
    v += 0.5f*(0.04f*v*v + 5.0f*v + 140.0f - u + I);
  }

  if (type & GLU) {
    u += NEURON_GLU_A*(NEURON_GLU_B*v - u);
  } else if (type & GABA) {
    u += NEURON_GABA_A*(NEURON_GABA_B*v - u);
  }

  //Start to reset INH
  --inh;
#pragma endregion

#pragma region NeuronFire
  
  if (v > 30.0f) {
    //Reset V and U
    if (type & GLU) {
      v = NEURON_GLU_C;
      u += NEURON_GLU_D;
    } else if (type & GABA) {
      v = NEURON_GABA_C;
      u += NEURON_GABA_D;
    }

    //Pos Pot
    if (!(type & GABA) && !(type & NO_LRN)) {
      for (int i = 0; i < ND; ++i) {
        if (dConnection[i] < 0)
          break;

        //Skip signal
        if (i == 0 && (type & GLU_SIGNAL))
          continue;

        int deltaTime = globalTime - dLastSpikeTime[i];

        float delta = 0.0f;
        if (deltaTime > 0) {
          if (deltaTime < NEURON_T0)
            delta = 0.05*exp(-deltaTime / NEURON_T0);
        }

        dWeight[i] += delta;

        if (dWeight[i] > NEURON_TH)
          dWeight[i] = NEURON_TH;
      }
    }

    lastSpikeTime = globalTime;
  }

#pragma endregion
}

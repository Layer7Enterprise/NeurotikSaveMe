#include "Core.h"

//Enable testing
//#include <CCup.h>

#include "Test.h"

#define only_first if (globalTime == 0)

//Called every millisecond
void CoreTick(int idx, Params_t *params) {
#pragma region Get Info
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
#ifdef CCUP
   only_first SendSnapshot("CoreInitialParams", idx, NN, ND, globalTime, dConnection, dDelay, dWeight, dLastSpikeTime, dSpikeQue, v, u, I, lastSpikeTime, inh, inhibitoryTime, ib, type);
#endif
#pragma endregion

#pragma region Periodic
   if (ib > 0 && (globalTime % ib == 0)) {
     I = 25.0f;
   }
#pragma endregion

#pragma region Dendrites
  //Dendrites
  for (int i = 0; i < ND; ++i) {
    //Is this an active dendrite
    if (dConnection[i] < 0) {
#ifdef CCUP
      only_first CCSend("CoreDendriteIsActive", false);
#endif
       break;
     } else {
#ifdef CCUP
       only_first CCSend("CoreDendriteIsActive", true);
#endif

       //We got a message from the axon
       if (params->nLastSpikeTime[dConnection[i]] == globalTime-1) {
       //Schedule the dendrite to fire in the near future (64bit bit que)
         dSpikeQue[i] |= (long long)1 << (dDelay[i]-1);
       }

      //This dendrite is supposed to fire now
      if (dSpikeQue[i] & 1) {
        dLastSpikeTime[i] = globalTime;
         
        //Only for GLU class neurons that fired to us
        if (params->nType[dConnection[i]] & GLU) {
          I += dWeight[i];

          //Neg pot (The dendrites!)
          //##############################

          //Avoid depot the signal leading neuron

          //Don't potentiate dendrites if this is a gaba or no_lrn neuron
          int isFirstSignal = type & GLU_SIGNAL && i == 0;
          if (!(type & GABA) && !(type & NO_LRN) && !isFirstSignal) {
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

      dSpikeQue[i] >>= 1;
    }
  }
#pragma endregion

#pragma region Core Update
  //Reduce the switching dynamics to tighten the neuron up (Logistic equation)
  //22 is related to how the threshold is about 20, we want to make sure we're in
  //the range when we hit ~22
  I *= 1 / (1 + exp(100.0f*(22.0f - I)));

  //Ceiling current
  if (I > NEURON_TH*1.5f)
    I = NEURON_TH*1.5f;

  //Update neuron
  v += 0.5f*(0.04f*v*v + 5.0f*v + 140.0f - u + I);
  v += 0.5f*(0.04f*v*v + 5.0f*v + 140.0f - u + I);


  if (type & GLU) {
    u += NEURON_GLU_A*(NEURON_GLU_B*v - u);
  } else if (type & GABA) {
    u += NEURON_GABA_A*(NEURON_GABA_B*v - u);
  }
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

    if (inh <= 0) {
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
  }
#pragma endregion

#pragma region Normalize
//Neuron fired more than than T0/2 ago and hasen't since
if (globalTime == lastSpikeTime && (type & GLU)) {
   float sigma = 0;
   float count = 0;

  for (int i = 0; i < ND; ++i) {
    if (dConnection[i] < 0)
      break;

     //SKIP?
     if (i == 0 && (type & GLU_SIGNAL))
       continue;

     if (type & NO_LRN)
       continue;

     if (params->nType[dConnection[i]] & GABA)
       continue;

    //Are we (not) in this hit region?
    if (lastSpikeTime > dLastSpikeTime[i] && lastSpikeTime - dLastSpikeTime[i] < NEURON_T0/2) {
      sigma += dWeight[i];
      ++count;
    }
  }

  //Round 2, normalize all that fired
  for (int i = 0; i < ND; ++i) {
    if (dConnection[i] < 0)
      break;

    if (i == 0 && (type & GLU_SIGNAL))
       continue;

    if (type & NO_LRN)
      continue;

    if (params->nType[dConnection[i]] & GABA)
       continue;

    //All that fired (Look at the previous when we set -1000 to show that they didn't fire)
    if (lastSpikeTime > dLastSpikeTime[i] && (lastSpikeTime - dLastSpikeTime[i] < NEURON_T0/2)) {
      //sigma += 0.00001f;
      sigma += 0.0001f;
      float dwdt = dWeight[i] * (1.00f*NEURON_TH / sigma - 1);
      dWeight[i] = dwdt*0.02 + dWeight[i];
    }
  }
}

#pragma endregion

#pragma region Updates
  //Update params
  --inh;
  I = 0;
#pragma endregion
}

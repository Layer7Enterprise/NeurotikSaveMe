#include "Core.h"
#include <string.h>

//Figure out which dendrites (of this neuron) fired recently (within period T0 [-t0/2 > lastSpikeTime < t0/2] ago
//where -t0/2 means half a period before this neuron fired first (lastSpikeTime)
#define DendriteFiredALongTimeAgo(i) (abs(dLastSpikeTime[i] - lastSpikeTime) > NEURON_T0/2)

#ifdef NEURON_DEBUG
__global__ void IGNeurotikCore(const int NN, const int ND, const int globalTime,
					float *pDopa, float *pOldDopa, int *pDeath, const int *dConnections, float *dWeights, const int *dDelays, int *dLastSpikeTimes, long long *dSpikeQues,
					const unsigned char *nTypes,
					float *nVs, float *nUs, float *nIs, int *nLastSpikeTime, int *nInh, float *nPlasticity, const int *nInhibitoryTimes, const float *nMaximumG, const int *nIb, float *debug) {
						
#else
__global__ void IGNeurotikCore(const int NN, const int ND, const int globalTime,
							   const int *dConnections, float *dWeights, const int *dDelays, int *dLastSpikeTimes, long long *dSpikeQues,
							   const unsigned char *nTypes,
							   float *nVs, float *nUs, float *nIs, float *nDopas, int *nLastSpikeTime, int *nInh, float *nPlasticity, const int *nInhibitoryTimes, const float *nMaximumG, const int *nIb) {
#endif
								   int idx = blockIdx.x * blockDim.x + threadIdx.x;
								   if (idx >= NN)
									   return;
  								  if (idx == 0) {
									for (int i = 0; i < 10; ++i)
									debug[i] = 0;
								 }
#pragma region Info
								   /*
								   ##################################################
								   Pull My info
								   ##################################################
								   */
								   //My dendrites
								   float &dopa = *pDopa;
								   float &oldDopa = *pOldDopa;
								   int &death = *pDeath;
								   const int *dConnection = dConnections + idx*ND;
								   const int *dDelay = dDelays + idx*ND;
								   float *dWeight = dWeights + idx*ND;
								   int *dLastSpikeTime = dLastSpikeTimes + idx*ND;
								   long long *dSpikeQue = dSpikeQues + idx*ND;

								   //Const params
								   const unsigned char type = *(nTypes + idx);

								   //My internal params
								   float &v = *(nVs + idx);
								   float &u = *(nUs + idx);
								   float &I = *(nIs + idx);
								   int &lastSpikeTime = *(nLastSpikeTime + idx);
								   int &inh = *(nInh + idx);
								   float &plasticity = *(nPlasticity + idx);
								   float maximumG = *(nMaximumG + idx);
								   int ib = *(nIb + idx);

#pragma region UPDATES and CHECKS
								   //Check if we should intrisically burst
								   if (ib)
									   if (globalTime % ib == 0)
										   I = 24.0f;

								   //Looks like we've gone down, start killing neurons
								   if (oldDopa > dopa)
									   death = true;
#pragma endregion

#pragma endregion

#pragma region Dendrites
								   //Process dendrites
								   for (int i = 0; i < ND; ++i) {
									   //Skip
									   if (dConnection[i] < 0)
										   break;

	
									   //Incomming signal from axon of a neuron
									   if (nLastSpikeTime[dConnection[i]] == globalTime-1) {
										   //Schedule the dendrite to fire in the near future
										   dSpikeQue[i] |= (long long)1 << (dDelay[i]-1);
									   }

									   //This dendrite is supposed to fire now
									   if (dSpikeQue[i] & 1) {
										   dLastSpikeTime[i] = globalTime;
										   

										   if (nTypes[dConnection[i]] & GLU) {
											   I += dWeight[i];
											   //Neg pot
											   if (!(type & GABA) && !(type & NO_LRN)) {

													   int deltaTime = globalTime - lastSpikeTime; //Previous Neuron's spike
													   float delta = 0.05*exp(-deltaTime / NEURON_T0);
												   delta *= plasticity; 
												   if (deltaTime < NEURON_T0 && !death)
													   dWeight[i] -= delta;

												   if (dWeight[i] < 0.0f)
													   dWeight[i] = 0.0f;
											   }
										   }
									   }

									   //This dendrites
									   if ((dSpikeQue[i] & 1) && nTypes[dConnection[i]] & GABA) {
										   dLastSpikeTime[i] = globalTime;
										   inh = nInhibitoryTimes[dConnection[i]];
										   if (type & GLU) {
											   v = NEURON_GLU_C;	

										   } else if (type & GABA) {
											   v = NEURON_GABA_C;
										   }

										   I = 0;
									   }

									   //Move que
									   dSpikeQue[i] >>= 1;
								   }	

#pragma endregion 

#pragma region Core Update
								   if ((type & GLU_SIGNAL))
									   I *= dopa / 100.0f;

								   if (type & GABA) 
									   I *= 2;

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

								   --inh;
#pragma endregion

#pragma region NeuronFire
								   if (type & GLU) {
									   u += NEURON_GLU_A*(NEURON_GLU_B*v - u);
								   } else if (type & GABA) {
									   u += NEURON_GABA_A*(NEURON_GABA_B*v - u);
								   }

								   if (v > 30.0f) {
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
												   if (deltaTime < NEURON_T0 && !death)

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

#pragma region Normalize
   //Neuron fired more than than T0/2 ago and hasen't since
   if (globalTime == lastSpikeTime && type & GLU & !death) {
	   float sigma = 0;
	   float count = 0;
		
		for (int i = 0; i < ND; ++i) {
			if (dConnection[i] < 0)
				break;

			//SKIP
		   if (i == 0 && (type & GLU_SIGNAL))
			   continue;

		   if (type & NO_LRN)
			   continue;

		   if (nTypes[dConnection[i]] & GABA)
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

			if (nTypes[dConnection[i]] & GABA)
			   continue;

			//All that fired (Look at the previous when we set -1000 to show that they didn't fire)
			if (lastSpikeTime > dLastSpikeTime[i] && (lastSpikeTime - dLastSpikeTime[i] < NEURON_T0/2)) {
				sigma += 0.00001f;
				float dwdt = dWeight[i] * (1.00f*NEURON_TH / sigma - 1);
				dWeight[i] = dwdt*0.05 + dWeight[i];
			}
		}
	}
#pragma endregion

#pragma region Updates
    //Decay current
	I = 0;

	//Update dopamine history
    oldDopa = dopa;
#pragma endregion

	return;
}

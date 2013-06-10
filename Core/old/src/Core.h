#ifndef CORE_H_
#define CORE_H_

#include <math.h>
#include <stdio.h>

#define NEURON_DEBUG

#define NEURON_POS_POT 0.0001f //Positive potentiation percentage
#define NEURON_NEG_POT NEURON_POS_POT //Negative potentiation percentage
#define NEURON_T0 20.0f  //exp(t/t0)

#define NEURON_TH 22.1f //Dopa calculations are made off this threshold:
#define NEURON_S 1       //Number of dendrites for sensetivity
#define NEURON_LEARN_RATE .1

enum NEURON_TYPES { GLU = 1, GABA = 2, DOPA = 4, NO_LRN = 8, GLU_SIGNAL = 16};

//Params
#define NEURON_GLU_A 0.1f
#define NEURON_GLU_B 0.2f
#define NEURON_GLU_C -65.0f
#define NEURON_GLU_D 2

#define NEURON_GABA_A 0.1f
#define NEURON_GABA_B 0.2f
#define NEURON_GABA_C -65.0f 
#define NEURON_GABA_D 2

/*
##################################################
IGNeurotikCore
	Primary simulator kernel function

	idx - Current operating index
	NN - Number of neurons
	ND - Number of dendrites available per neuron
	globalTime - Current time interval (mil3iseconds)

	dConnections - Connection matrix
	dWeights - Weights matrix
	dDelays - Delay matrix
	dLastSpikeTimes - Last time the dendrite spiked
	dSpikeQues - 64-bit bit-shifted spike que
	
	nType - Neuron type

	nVs - Neuron voltage
	nUs - Neuron differential
	nIs - Neuron input current
	nGabas - Gabagenic content surrounding membrane
	nDopas - Dopergenic activity around neuron
	nLastSpikeTime - Last time this neuron spiked
	nInh - Current inhibitory ms till finished, set all to 0 at start
	nPlasticity - Current ability to learn [0.0 -> 1.0]
	nInhibitoryTimes - How long this neuron will affect others (GABA only)
	nMaxmimuG - Maximum conductance of each input for this neuron
	nIb - Period of intrisnic bursting neurons (1 would be input every tick, i.e. modulo clock)
	
##################################################
*/
#ifdef NEURON_DEBUG
__global__ void IGNeurotikCore(const int NN, const int ND, const int globalTime, 
					float *pDopa, float *pOldDopa, int *pDeath, const int *dConnections, float *dWeights, const int *dDelays, int *dLastSpikeTimes, long long *dSpikeQues,
					const unsigned char *nTypes,
					float *nVs, float *nUs, float *nIs, int *nLastSpikeTime, int *nInh, float *nPlasticity, const int *nInhibitoryTimes, const float *nMaximumG, const int *nIb, float *debug);
#else
__global__ void IGNeurotikCore(const int NN, const int ND, const int globalTime, 
					const int *dConnections, float *dWeights, const int *dDelays, int *dLastSpikeTimes, long long *dSpikeQues,
					const unsigned char *nTypes,
					float *nVs, float *nUs, float *nIs, float *nDopas, int *nLastSpikeTime, int *nInh, float *nPlasticity, const int *nInhibitoryTimes, const float *nMaximumG, const int *nIb);
#endif
#endif

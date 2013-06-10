#ifndef PARAMS_H_
#define PARAMS_H_

enum NeuronType_t {
  GLU = 1,
  GABA = 2,
  NO_LRN = 8
};

//Params
#define NEURON_GLU_A 0.1f
#define NEURON_GLU_B 0.2f
#define NEURON_GLU_C -65.0f
#define NEURON_GLU_D 2
#define NEURON_GLU_V -65.0f

#define NEURON_GABA_A 0.1f
#define NEURON_GABA_B 0.2f
#define NEURON_GABA_C -65.0f 
#define NEURON_GABA_D 2
#define NEURON_GABA_V -65.0f

#define NEURON_T0 20.0f  //exp(t/t0)

#define NET_INPUT_LEN(X) ((X)->networkInEndPos - (X)->networkInStartPos)
#define NET_OUTPUT_LEN(X) ((X)->networkOutEndPos - (X)->networkOutStartPos)

//Debug output networking structs
struct NeuronDebugNetworkOutput_t {
  //Count for tracking dropped packets
  int count;

  //Index
  int idx;

  //Neuron type
  int type;
  
  //Name
  char name[100];

  //Global time
  int globalTime;

  //Voltage and U of diff equations
  float V;
  float U;

  //For tracking when spikes actually occur
  int lastSpikeTime;

  //For tracking how long inhibitory are currently active
  int inh;
};

#endif

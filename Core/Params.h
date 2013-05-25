#ifndef PARAMS_H_
#define PARAMS_H_

#include <map>

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

#define NET_INPUT_LEN(X) ((X)->networkInEndPos - (X)->networkInStartPos)
#define NET_OUTPUT_LEN(X) ((X)->networkOutEndPos - (X)->networkOutStartPos)

struct Params_t {
  //Server
  int NN;                //Number of neurons
  int ND;                //Number of dendrites
  int globalTime;        //Current running time
  std::map<std::string, int> *neuronNameToLocation;
  std::map<int, std::string> *neuronLocationToName;

  //Network in/out
  int networkInStartPos;
  int networkInEndPos;
  int networkOutStartPos;
  int networkOutEndPos;

  //Neurons
  int *nInhibitoryTime;      //How long to inhibit for?
  int *nLastSpikeTime;       //Last time this neuron fired
  float *nV;                 //Voltage of this neuron
  float *nU;                 //U param of this nuron
  float *nI;                 //Instantenous current
  int *nInh;                 //Inhibitor is active
  int *nIb;                  //Intrinsict burst time
  NeuronType_t *nType;       //What kind of neuron?

  //Dendrites
  int *dConnections;     //Connection matrix
  float *dWeights;       //Weight matrix
  int *dDelays;          //Delay matrix
  int *dLastSpikeTimes;  //Last spike time
  long long *dSpikeQues;        //Spike que
};

struct NeuronSnapshot_t {
    int idx;
    int NN;
    int ND;
    int globalTime;
    const int *dConnection;
    const int *dDelay;
    float *dWeight;
    int *dLastSpikeTime;
    long long *dSpikeQue;
    float v;
    float u;
    float I;
    int lastSpikeTime;
    int inh;
    int ib;
    NeuronType_t type;
};

#endif

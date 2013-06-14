#ifndef PARAMS_H_
#define PARAMS_H_

#ifdef __cplusplus
#include <map>
#include <vector>
#endif

enum NeuronType_t {
  GLU = 1,
  GABA = 2,
  NO_LRN = 8,
  GLU_SIGNAL = 16,
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
#define NEURON_TH 22.1f  //Threshold calculations are made off this

#define NET_INPUT_LEN(X) ((X)->networkInEndPos - (X)->networkInStartPos)
#define NET_OUTPUT_LEN(X) ((X)->networkOutEndPos - (X)->networkOutStartPos)

struct Params_t {
  //Server
  int NN;                //Number of neurons
  int ND;                //Number of dendrites
  int globalTime;        //Current running time

#ifdef __cplusplus
  std::map<std::string, int> *neuronNameToLocation;
  std::map<int, std::string> *neuronLocationToName;
  std::vector<int> *neuronDebugLocations;
#endif

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
  enum NeuronType_t *nType;       //What kind of neuron?

  //Dendrites
  int *dConnections;     //Connection matrix
  float *dWeights;       //Weight matrix
  int *dDelays;          //Delay matrix
  int *dLastSpikeTimes;  //Last spike time
  long long *dSpikeQues;        //Spike que

  //Debug assistant
  int debugDendriteIdx;
};

enum NeuronDebugControlCodes_t {
  kNDControlTrackDendrite,  //Tell the engine to track one neuron's dendrites
  kNDControlGetNeuronsPriorOnDendrites,  //Get a list of the neuron names for the dendrites
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
    int inhibitoryTime;
    int inh;
    int ib;
    enum NeuronType_t type;
};

//Debug output networking structs
struct NeuronDebugNetworkOutput_t {
  //Debug output count (For detecting dropped packets)
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

//Debug dendrites packet
struct NeuronDDebugNetwork_t {
  //Number of dendrites
  int count;

  //Name of dendrites
  char dendrite[0][0];
};

#endif

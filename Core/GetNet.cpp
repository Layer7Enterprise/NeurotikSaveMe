#include "GetNet.h"

void GetNet(const char *filename, Schema_t schema, Params_t *params) {
  FILE *f = fopen(filename, "r");
	if (!f) {
		fprintf(stderr, "Couldn't open network file (%s)\n", filename);
		exit(EXIT_FAILURE);
	}

	printf(">Loaded network file (%s)\n", filename);

  int NN = schema.NN;
  int ND = schema.ND;

  //Server
  params->NN = NN;
  params->ND = ND;
  params->globalTime = 0;
  params->neuronNameToLocation = new std::map<std::string, int>();
  params->neuronLocationToName = new std::map<int, std::string>();

  //Initalize neurons
  params->nInhibitoryTime = new int[NN];
  params->nLastSpikeTime = new int[NN];
  params->nV = new float[NN];
  params->nU = new float[NN];
  params->nI = new float[NN];
  params->nInh = new int[NN];
  params->nIb = new int[NN];
  params->nType = new NeuronType_t[NN];

  //Initialize Dendrites
  params->dConnections = new int[NN*ND];
  params->dWeights = new float[NN*ND];
  params->dDelays = new int[NN*ND];
  params->dLastSpikeTimes = new int[NN*ND];
  params->dSpikeQues = new long long[NN*ND];

  for (int i = 0; i < NN*ND; ++i) {
    params->dConnections[i] = -1;
  }

  //Parse schema
  enum ParseState_t { 
    PS_NEURON,
    PS_CONNECTION,
    PS_NETWORKIN,
    PS_NETWORKOUT,
    PS_ENDNETWORKIN,
    PS_ENDNETWORKOUT,
  } parseState;

  parseState = PS_NEURON;

  char trash[100];
  while (!feof(f)) {
    char header[100];
    fscanf(f, "%s", header);

    //Get state
    if (!strcmp(header, "Neuron"))
      parseState = PS_NEURON;
    else if (!strcmp(header, "Connection"))
      parseState = PS_CONNECTION;
    else if (!strcmp(header, "NetworkIn"))
      parseState = PS_NETWORKIN;
    else if (!strcmp(header, "NetworkOut"))
      parseState = PS_NETWORKOUT;
    else if (!strcmp(header, "EndNetworkIn"))
      parseState = PS_ENDNETWORKIN;
    else if (!strcmp(header, "EndNetworkOut"))
      parseState = PS_ENDNETWORKOUT;
    else {
      fprintf(stderr, "Error, invalid parse state '%s'...\n", header);
      exit(EXIT_FAILURE);
    }
    
    //Make sure we didn't hit the EOF
    if (feof(f))
      break;

    static int openPosition = 0;
    switch (parseState) {
      case PS_NEURON:
        char neuronName[100];
        NeuronType_t neuronType;
        int neuronInhibitoryTime;
        float neuronMaximumG;
        int neuronIb;
        fscanf(f, "%s", neuronName);
        fscanf(f, "%s %d", trash, (int *)&neuronType);
        fscanf(f, "%s %d", trash, &neuronInhibitoryTime);
        fscanf(f, "%s %d", trash, &neuronIb);
        fscanf(f, "%s %s", trash, trash);

        if (strcmp(trash, "End")) {
          fprintf(stderr, "Error, last neuron comment wasn't an End");
          getchar();
          exit(EXIT_FAILURE);
        }

        (*params->neuronNameToLocation)[neuronName] = openPosition;
        (*params->neuronLocationToName)[openPosition] = neuronName;

        if (neuronType & GLU)
          params->nV[openPosition] = NEURON_GLU_V;
        else if (neuronType & GABA)
          params->nV[openPosition] = NEURON_GABA_V;

        params->nI[openPosition] = 0.0f;
        params->nLastSpikeTime[openPosition] = -1000;
        params->nInh[openPosition] = -1;
        params->nInhibitoryTime[openPosition] = neuronInhibitoryTime;
        params->nIb[openPosition] = neuronIb;

        params->nType[openPosition] = neuronType;

        if (neuronType & GLU)
          params->nU[openPosition] = NEURON_GLU_B * NEURON_GLU_C;
        else if (neuronType & GABA) {
          params->nU[openPosition] = NEURON_GABA_B * NEURON_GABA_C;
        }

        ++openPosition;

        break;
      case PS_CONNECTION:
        char from[100];
        char to[100];
        float weight;
        int delay;
        fscanf(f, "%s %s", trash, from);
        fscanf(f, "%s %s", trash, to);
        fscanf(f, "%s %f", trash, &weight);
        fscanf(f, "%s %d", trash, &delay);
        fscanf(f, "%s %s", trash, trash);

        static int fromNode;
        static int toNode;
        fromNode = (*params->neuronNameToLocation)[from];
        toNode = (*params->neuronNameToLocation)[to];

        if ((*params->neuronNameToLocation).find(from) == (*params->neuronNameToLocation).end()) {
          fprintf(stderr, "Could not find the mapping from name to location for %s\n", from);
          exit(EXIT_FAILURE);
        }

        if ((*params->neuronNameToLocation).find(to) == (*params->neuronNameToLocation).end()) {
          fprintf(stderr, "Could not find the mapping from name to location for %s\n", to);
          exit(EXIT_FAILURE);
        }

        //Get an open connection
        static int pos;
        pos = -1; //Position might not get set
        for (int i = 0; i < ND; ++i) {
          printf("%d -> %d\n", fromNode, toNode);
          pos = toNode*ND + i;
          if (params->dConnections[pos] == -1) {
            params->dConnections[pos] = fromNode;
            params->dWeights[pos] = weight;
            params->dDelays[pos] = delay;
            params->dSpikeQues[pos] = 0;
            params->dLastSpikeTimes[pos] = -1000;
            break;
          }
        }
        
        if (pos == -1) {
          fprintf(stderr, "Did not have enough dendrites!\n");
          exit(EXIT_FAILURE);
        }

        break;
      case PS_NETWORKIN:
        params->networkInStartPos = openPosition;
        break;
      case PS_NETWORKOUT:
        params->networkOutStartPos = openPosition;
        break;
      case PS_ENDNETWORKIN:
        params->networkInEndPos = openPosition;
        break;
      case PS_ENDNETWORKOUT:
        params->networkOutEndPos = openPosition;
        break;
      default:
        fprintf(stderr, "Invalid parse state reached in case");
        exit(EXIT_FAILURE);
        break;
    };
  }
}

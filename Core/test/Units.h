#ifndef UNITS_H_
#define UNITS_H_

#include <CCup.h>

//Test the loading of the network files
void TestGetNet() {
  //Load schema (Port, name, etc)
  Schema_t schema;
  GetSchema("test/config/schema.txt", &schema);

  //Load params (net list, large chunk of memory, etc)
  Params_t params;
  GetNet("test/config/net.txt", schema, &params);

  It("has the right network start position", function() {
    IsEqual(params.networkInStartPos, 1);
  });

  It("has the right network in end position", function() {
    IsEqual(params.networkInEndPos, 3);
  });

  It("has the right network out start position", function() {
    IsEqual(params.networkOutStartPos, 2);
  });

  It("has the right network out end position", function() {
    IsEqual(params.networkOutEndPos, 4);
  });

  It("has the right NN", function() {
    IsEqual(params.NN, 4);
  });

  It("has the right dendrites", function() {
    IsEqual(params.ND, 10);
  });

  It("has the right initial global time", function() {
    IsEqual(params.globalTime, 0);
  });

  It("has the right inhibitory times", function() {
    IsEqual(params.nInhibitoryTime[0], 0);
    IsEqual(params.nInhibitoryTime[1], 0);
    IsEqual(params.nInhibitoryTime[2], 20);
    IsEqual(params.nInhibitoryTime[3], 0);
  });
  
  It("has the right initial spike times", function() {
    IsEqual(params.nLastSpikeTime[0], -1000);
    IsEqual(params.nLastSpikeTime[1], -1000);
    IsEqual(params.nLastSpikeTime[2], -1000);
    IsEqual(params.nLastSpikeTime[3], -1000);
  });
  
  It("has the right initial voltage", function() {
    IsEqual(params.nV[0], NEURON_GLU_V);
    IsEqual(params.nV[1], NEURON_GABA_V);
    IsEqual(params.nV[2], NEURON_GLU_V);
    IsEqual(params.nV[3], NEURON_GLU_V);
  });
  
  It("has the right initial current", function() {
    IsEqual(params.nI[0], 0);
    IsEqual(params.nI[1], 0);
    IsEqual(params.nI[2], 0);
    IsEqual(params.nI[3], 0);
  });
  
  It("has the right initial U", function() {
    IsEqual(params.nU[0], NEURON_GLU_B * NEURON_GLU_C);
    IsEqual(params.nU[1], NEURON_GABA_B * NEURON_GABA_C);
    IsEqual(params.nU[2], NEURON_GLU_B * NEURON_GLU_C);
    IsEqual(params.nU[3], NEURON_GLU_B * NEURON_GLU_C);
  });
  
  It("has the right initial inh", function() {
    IsEqual(params.nInh[0], -1);
    IsEqual(params.nInh[1], -1);
    IsEqual(params.nInh[2], -1);
    IsEqual(params.nInh[3], -1);
  });
  
  It("has the right intrinsic bursting", function() {
    IsEqual(params.nIb[0], 0);
    IsEqual(params.nIb[1], 0);
    IsEqual(params.nIb[2], 10);
    IsEqual(params.nIb[3], 0);
  });
  
  It("has the right types", function() {
    IsEqual(params.nType[0], GLU);
    IsEqual(params.nType[1], GABA);
    IsEqual(params.nType[2], GLU | NO_LRN);
    IsEqual(params.nType[3], GLU);
  });
  
  It("has the right connections", function() {
    IsEqual(params.dConnections[0*10+0], 0);
    IsEqual(params.dConnections[2*10+0], 1);
    IsEqual(params.dConnections[3*10+0], 3);
    IsEqual(params.dConnections[0*10+1], 2);
    IsEqual(params.dConnections[2*10+1], -1);
    IsEqual(params.dConnections[3*10+1], -1);
  });
  
  It("has the right initial weights", function() {
    IsEqual(params.dWeights[0*10+0], 11.05);
    IsEqual(params.dWeights[2*10+0], 20.0);
    IsEqual(params.dWeights[3*10+0], 1.0);
    IsEqual(params.dWeights[0*10+1], 2.0);
  });
  
  It("has the right initial delays", function() {
    IsEqual(params.dDelays[0*10+0], 1);
    IsEqual(params.dDelays[2*10+0], 5);
    IsEqual(params.dDelays[3*10+0], 20);
    IsEqual(params.dDelays[0*10+1], 20);
  });
  
  It("has the right initial last spike times", function() {
    IsEqual(params.dLastSpikeTimes[0*10+0], -1000);
    IsEqual(params.dLastSpikeTimes[2*10+0], -1000);
    IsEqual(params.dLastSpikeTimes[3*10+0], -1000);
    IsEqual(params.dLastSpikeTimes[0*10+1], -1000);
  });
  
  It("has the right initial spike ques", function() {
    IsEqual(params.dSpikeQues[0*10+0], 0);
    IsEqual(params.dSpikeQues[2*10+0], 0);
    IsEqual(params.dSpikeQues[3*10+0], 0);
    IsEqual(params.dSpikeQues[0*10+1], 0);
  });
}

void TestCoreManager() {
  //Load schema (Port, name, etc)
  Schema_t schema;
  GetSchema("test/config/schema.txt", &schema);

  //Load params (net list, large chunk of memory, etc)
  Params_t params;
  GetNet("test/config/net.txt", schema, &params);

  CoreBegin(&params);

  int input_len = NET_INPUT_LEN(&params);
  int output_len = NET_OUTPUT_LEN(&params);

  It("Gets the right network input sizes", function() {
    IsEqual(input_len, 2);
  });

  It("Gets the right network output sizes", function() {
    IsEqual(output_len, 2);
  });

  It("Can receive one piece of network data", _function() {
    const char data[] = "101010";
    char command[200];
    sprintf(command, "ruby ./test/utility/send_data.rb %s", data);
    system(command);
    puts(command);
  });
}

void RunUnits() {
  CCup(function() {
    Describe("TestGetNet", function() {
      TestGetNet();
    });

    Describe("TestCoreManager", function() {
      TestCoreManager();
    });
  });

  dispatch_main();
}

#endif

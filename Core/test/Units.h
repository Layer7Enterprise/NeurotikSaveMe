#ifndef UNITS_H_
#define UNITS_H_

#include <string.h>
#include <stdlib.h>

static char currentDescribe[200];
void describe(const char *message) {
  strcpy(currentDescribe, message);
}

static char currentIt[200];
static int numTests = 0;
void it(const char *message) {
  strcpy(currentIt, message);
  printf("\n\t+ %s %s ", currentDescribe, currentIt);
  ++numTests ;
}

void shouldEqual(float a, float b) {
  printf("✔");
  if (a != b) {
    fprintf(stderr, "\n*****************************************************\n");
    fprintf(stderr, "˟ %s %s Test Failed! %f != %f\n", currentDescribe, currentIt, a, b);
    fprintf(stderr, "*****************************************************\n\n");
    exit(EXIT_FAILURE);
  }
}

void TestGetNet() {
  describe("GetNet");

  //Load schema (Port, name, etc)
  Schema_t schema;
  GetSchema("test/config/schema.txt", &schema);

  //Load params (net list, large chunk of memory, etc)
  Params_t params;
  GetNet("test/config/net.txt", schema, &params);

  it("has the right network in start position");
  shouldEqual(params.networkInStartPos, 1);

  it("has the right network in end position");
  shouldEqual(params.networkInEndPos, 3);

  it("has the right network out start position");
  shouldEqual(params.networkOutStartPos, 2);

  it("has the right network out end position");
  shouldEqual(params.networkOutEndPos, 4);

  it("has the right NN");
  shouldEqual(params.NN, 4);

  it("has the right dendrites");
  shouldEqual(params.ND, 10);

  it("has the right initial global time");
  shouldEqual(params.globalTime, 0);

  it("has the right inhibitory times");
  shouldEqual(params.nInhibitoryTime[0], 0);
  shouldEqual(params.nInhibitoryTime[1], 0);
  shouldEqual(params.nInhibitoryTime[2], 20);
  shouldEqual(params.nInhibitoryTime[3], 0);

  it("has the right initial spike times");
  shouldEqual(params.nLastSpikeTime[0], -1000);
  shouldEqual(params.nLastSpikeTime[1], -1000);
  shouldEqual(params.nLastSpikeTime[2], -1000);
  shouldEqual(params.nLastSpikeTime[3], -1000);

  it("has the right initial voltage");
  shouldEqual(params.nV[0], NEURON_GLU_V);
  shouldEqual(params.nV[1], NEURON_GABA_V);
  shouldEqual(params.nV[2], NEURON_GLU_V);
  shouldEqual(params.nV[3], NEURON_GLU_V);

  it("has the right initial current");
  shouldEqual(params.nI[0], 0);
  shouldEqual(params.nI[1], 0);
  shouldEqual(params.nI[2], 0);
  shouldEqual(params.nI[3], 0);

  it("has the right initial U");
  shouldEqual(params.nU[0], NEURON_GLU_B * NEURON_GLU_C);
  shouldEqual(params.nU[1], NEURON_GABA_B * NEURON_GABA_C);
  shouldEqual(params.nU[2], NEURON_GLU_B * NEURON_GLU_C);
  shouldEqual(params.nU[3], NEURON_GLU_B * NEURON_GLU_C);

  it("has the right initial inh");
  shouldEqual(params.nInh[0], -1);
  shouldEqual(params.nInh[1], -1);
  shouldEqual(params.nInh[2], -1);
  shouldEqual(params.nInh[3], -1);

  it("has the right intrinsic bursting");
  shouldEqual(params.nIb[0], 0);
  shouldEqual(params.nIb[1], 0);
  shouldEqual(params.nIb[2], 10);
  shouldEqual(params.nIb[3], 0);

  it("has the right types");
  shouldEqual(params.nType[0], GLU);
  shouldEqual(params.nType[1], GABA);
  shouldEqual(params.nType[2], GLU | NO_LRN);
  shouldEqual(params.nType[3], GLU);

  it("has the right connections");
  shouldEqual(params.dConnections[0*10+0], 0);
  shouldEqual(params.dConnections[2*10+0], 1);
  shouldEqual(params.dConnections[3*10+0], 3);
  shouldEqual(params.dConnections[0*10+1], 2);
  shouldEqual(params.dConnections[2*10+1], -1);
  shouldEqual(params.dConnections[3*10+1], -1);

  it("has the right initial weights");
  shouldEqual(params.dWeights[0*10+0], 11.05);
  shouldEqual(params.dWeights[2*10+0], 20.0);
  shouldEqual(params.dWeights[3*10+0], 1.0);
  shouldEqual(params.dWeights[0*10+1], 2.0);

  it("has the right initial delays");
  shouldEqual(params.dDelays[0*10+0], 1);
  shouldEqual(params.dDelays[2*10+0], 5);
  shouldEqual(params.dDelays[3*10+0], 20);
  shouldEqual(params.dDelays[0*10+1], 20);

  it("has the right initial last spike times");
  shouldEqual(params.dLastSpikeTimes[0*10+0], -1000);
  shouldEqual(params.dLastSpikeTimes[2*10+0], -1000);
  shouldEqual(params.dLastSpikeTimes[3*10+0], -1000);
  shouldEqual(params.dLastSpikeTimes[0*10+1], -1000);

  it("has the right initial spike ques");
  shouldEqual(params.dSpikeQues[0*10+0], 0);
  shouldEqual(params.dSpikeQues[2*10+0], 0);
  shouldEqual(params.dSpikeQues[3*10+0], 0);
  shouldEqual(params.dSpikeQues[0*10+1], 0);

}

void RunUnits() {
  printf("Running unit tests\n");
  printf("##################################################\n");

  TestGetNet();

  printf("\nPassed %d unit tests!\n", numTests);
  printf("##################################################\n\n");
  exit(0);
}

#endif
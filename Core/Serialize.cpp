#include "Serialize.h"

void SerializeDebugOutput(NeuronDebugNetworkOutput_t *input, char *outputString) {
  sprintf(outputString, "{\"type\": %d, \"idx\": %d, \"globalTime\": %d, \"v\": %f, \"u\": %f, \"lastSpikeTime\": %d, \"inh\": %d}", input->type, input->idx, input->globalTime, input->V, input->U, input->lastSpikeTime, input->inh);
}

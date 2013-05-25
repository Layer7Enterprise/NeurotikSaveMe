#ifndef CORE_TEST_H_
#define CORE_TEST_H_

#include <string>

void SendSnapshot(std::string name, int idx, int NN, int ND, int globalTime, const int *dConnection, const int *dDelay, float *dWeight, int *dLastSpikeTime, long long *dSpikeQue, float v, float u, float I, int lastSpikeTime, int inh, int inhibitoryTime, int ib, NeuronType_t type) {
  #ifdef CCUP
  NeuronSnapshot_t snapshot;
  snapshot.idx = idx;
  snapshot.NN = NN;
  snapshot.ND = ND;
  snapshot.globalTime = globalTime;
  snapshot.dConnection = dConnection;
  snapshot.dDelay = dDelay;
  snapshot.dWeight = dWeight;
  snapshot.dLastSpikeTime = dLastSpikeTime;
  snapshot.dSpikeQue = dSpikeQue;
  snapshot.v = v;
  snapshot.u = u;
  snapshot.I = I;
  snapshot.lastSpikeTime = lastSpikeTime;
  snapshot.inh = inh;
  snapshot.ib = ib;
  snapshot.inhibitoryTime = inhibitoryTime;
  snapshot.type = type;

  CCSend(name, (char *)&snapshot, sizeof(NeuronSnapshot_t));
#endif

}

#endif

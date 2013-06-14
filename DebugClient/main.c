#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "DebugClient.h"
#include "../Core/Params.h"

//Regular info, not dendrite
void onData(struct NeuronDebugNetworkOutput_t output) {
}

void onDrop() {
  puts("Dropped!");
}

//Dendrite related
void onDData(struct NeuronDDebugNetwork_t output) {
}

int main() {
  NUDebugClientBegin("127.0.0.1");
  NUDebugClientSetCallback(onData);
  NUDebugClientSetDropped(onDrop);

  NUDebugDClientTrack("Hello world!");

  NUDebugDClientBegin("127.0.0.1");
  NUDebugDClientSetCallback(onDData);

  while (1)
    sleep(1);

  return 0;
}

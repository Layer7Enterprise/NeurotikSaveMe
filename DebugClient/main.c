#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "DebugClient.h"
#include "Params.h"

void onData(struct NeuronDebugNetworkOutput_t output) {
}

void onDrop() {
  puts("Dropped!");
}

int main() {
  NUDebugClientBegin("127.0.0.1");
  NUDebugClientSetCallback(onData);
  NUDebugClientSetDropped(onDrop);

  while (1)
    sleep(1);

  return 0;
}

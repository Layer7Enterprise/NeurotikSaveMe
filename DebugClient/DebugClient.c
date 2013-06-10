#include "DebugClient.h"

#define DEBUG_PORT 3002

//Networking
static int sock;
static struct sockaddr_in addr;

//Callback for recieving data
static void (*onData)(struct NeuronDebugNetworkOutput_t output);

//Callback for dropping packets error
static void (*onDrop)();

void *_RcvThread(void *threadInput) {
  while (1) {
    char buffer[1000];
    socklen_t sockLen = sizeof(struct sockaddr);
    int num = recv(sock, buffer, sizeof(buffer), 0);
    if (num < 0) {
      perror("NUDebugClient: Could not recieve from the socket");
      exit(EXIT_FAILURE);
    }

    struct NeuronDebugNetworkOutput_t output;
    memcpy(&output, buffer, sizeof(struct NeuronDebugNetworkOutput_t));

    //Detect dropped packets
    static int lastCount = -1;
    if (lastCount != -1) {
      if (output.count - lastCount != 1) {
        onDrop();
      }
    }
    lastCount = output.count;

    onData(output);
  }

  return NULL;
}

void NUDebugClientBegin(const char ip[]) {
  puts("NUDebugClient listeninng...");
  //Setup socket
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("NUDebugClient: Could not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(DEBUG_PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
  
  pthread_t rcv_thread;
  pthread_create(&rcv_thread, NULL, _RcvThread, NULL);
}

void NUDebugClientSetCallback(void (*callback)(struct NeuronDebugNetworkOutput_t output)) {
  onData = callback;
}

void NUDebugClientSetDropped(void (*callback)()) {
  onDrop = callback;
}

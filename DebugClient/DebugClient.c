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

//##############################
//Dendrite related
//##############################

//Networking
static int dendriteRcvSock;
static struct sockaddr_in dendriteRcvAddr;

//Callback for recieving data
static void (*onDendriteData)(struct NeuronDendriteDebugNetwork_t output);

void NUDebugClientDendriteTrack(const char *name) {
  //Setup a socket
  int sockTrack = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("NUDebugClient: Could not create socket track socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in trackAddr;

  memset(&trackAddr, 0, sizeof(struct sockaddr_in));
  trackAddr.sin_family = AF_INET;
  trackAddr.sin_port = htons(DEBUG_PORT+1);
  trackAddr.sin_addr.s_addr = INADDR_ANY;

  int res = connect(sockTrack, (struct sockaddr *)&trackAddr, sizeof(struct sockaddr_in));
  if (res < 0) {
    perror("Could not connect to setup client track...");
    exit(EXIT_FAILURE);
  }

  char buffer[100];
  strcpy(buffer+1, name);
  buffer[0] = kNDControlTrackDendrite;  //Track control code

  int bufferLen = 1 + strlen(name) + sizeof('\0');

  //Send the name off
  sendto(sockTrack, buffer, bufferLen, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
}

void *_DRcvThread(void *threadInput) {
  while (1) {
    char buffer[1000];
    socklen_t sockLen = sizeof(struct sockaddr);
    int num = recv(dendriteRcvSock, buffer, sizeof(buffer), 0);
    if (num < 0) {
      perror("NUDDebugClient: Could not recieve from the socket");
      exit(EXIT_FAILURE);
    }

    struct NeuronDendriteDebugNetwork_t output;
    memcpy(&output, buffer, sizeof(struct NeuronDendriteDebugNetwork_t));

    onDendriteData(output);
  }

  return NULL;
}

void NUDebugClientDendriteBegin(const char ip[]) {
  puts("NUDDebugClient listeninng...");

  //Setup socket
  dendriteRcvSock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("NUDDebugClient: Could not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&dendriteRcvAddr, 0, sizeof(struct sockaddr_in));
  dendriteRcvAddr.sin_family = AF_INET;
  dendriteRcvAddr.sin_port = htons(DEBUG_PORT+2);
  dendriteRcvAddr.sin_addr.s_addr = INADDR_ANY;

  bind(dendriteRcvSock, (struct sockaddr *)&dendriteRcvAddr, sizeof(struct sockaddr));
  
  pthread_t d_rcv_thread;
  pthread_create(&d_rcv_thread, NULL, _DRcvThread, NULL);
}

//Set callback for receiving data
void NUDebugClientDendriteSetCallback(void (*callback)(struct NeuronDendriteDebugNetwork_t output)) {
  onDendriteData = callback;
}


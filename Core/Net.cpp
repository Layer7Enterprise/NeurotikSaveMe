#include "Net.h"

//Sending Stuff
//##############################
static int netSendSocket;
static sockaddr_in netSendAddr;
void NetSendBegin(const char *ip, int port) {
  //Create a new socket
  netSendSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if (netSendSocket < 0) {
    perror("Could not create send socket");
    exit(EXIT_FAILURE);
  }

  //Setup address
  memset(&netSendAddr, 0, sizeof(sockaddr_in));
  netSendAddr.sin_family = AF_INET;
  netSendAddr.sin_addr.s_addr = inet_addr(ip);
  netSendAddr.sin_port = htons(port);
}

void NetSend(const char *message, int len) {
  int res = sendto(netSendSocket, message, len, 0, (sockaddr *)&netSendAddr, sizeof(sockaddr_in));

  if (res < 0) {
    perror("Could not send data");
    exit(EXIT_FAILURE);
  }
}

//Receiving Stuff (Primary)
//##############################
static int netRcvSocket;
static sockaddr_in netRcvAddr;
static void (*netRcvCallback)(const char *);
static pthread_t netRcvThread;

void *_NetRcvThread(void *) {
  while (true) {
    socklen_t addrlen = sizeof(sockaddr_in);

    const int MAXLEN = 100;
    static char buffer[MAXLEN];

    int nBytes = recvfrom(netRcvSocket, buffer, MAXLEN, 0, (sockaddr *)&netRcvAddr, &addrlen);

    if (nBytes < 0) {
      perror("Tried to recvfrom but failed");
      exit(EXIT_FAILURE);
    }

    if (!buffer) {
      fprintf(stderr, "Net receive callback was not set");
      exit(EXIT_FAILURE);
    }

    netRcvCallback(buffer);
  }
}

void NetRcvBegin(const char *ip, int port, void (*callback )(const char *)) {
  //Save callback
  netRcvCallback = callback;

  //Setup socket
  netRcvSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if (netRcvSocket < 0) {
    perror("Could not create receive socket");
    exit(EXIT_FAILURE);
  }

  //Setup address
  memset(&netRcvAddr, 0, sizeof(sockaddr_in));
  netRcvAddr.sin_family = AF_INET;
  netRcvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  netRcvAddr.sin_port = htons(port);

  //Bind
  int res = bind(netRcvSocket, (sockaddr *)&netRcvAddr, sizeof(sockaddr_in));
  if (res < 0) {
    perror("Could not bind receive socket");
    exit(EXIT_FAILURE);
  }

  //Join Multicast group
  ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(ip);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  res = setsockopt(netRcvSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(ip_mreq));
  if (res < 0) {
    perror("Could not join IP Multicast Group");
    exit(EXIT_FAILURE);
  }

  //Start threaded server
  pthread_create(&netRcvThread, NULL, _NetRcvThread, NULL);
}

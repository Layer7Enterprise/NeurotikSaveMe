#include "Net.h"

//Sending Stuff
//##############################
int netSendSocket;
sockaddr_in netSendAddr;
void NetSendBegin(const char *ip, int port) {
  //Create a new socket
  netSendSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if (netSendSocket < 0) {
    fprintf(stderr, "Could not create send socket");
    exit(EXIT_FAILURE);
  }

  //Setup address
  memset(&netSendAddr, 0, sizeof(sockaddr_in));
  netSendAddr.sin_family=AF_INET;
  netSendAddr.sin_addr.s_addr = inet_addr(ip);
  netSendAddr.sin_port = htons(port);
}

void NetSend(const char *message, int len) {
  int res = sendto(netSendSocket, message, len, 0, (sockaddr *)&netSendAddr, sizeof(sockaddr_in));

  if (res < 0) {
    fprintf(stderr, "Could not send data");
    exit(EXIT_FAILURE);
  }
}

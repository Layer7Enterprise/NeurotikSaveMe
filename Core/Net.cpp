#include "Net.h"

//Debug?
//#include <CCup.h>

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

//Sending Debug Info
//############################################################
static int netSendSocketDebug;
static sockaddr_in netSendAddrDebug;
void NetSendBeginDebug(const char *ip, int port) {
  //Create a new socket
  netSendSocketDebug = socket(AF_INET, SOCK_DGRAM, 0);
  if (netSendSocketDebug < 0) {
    perror("Could not create send debug socket");
    exit(EXIT_FAILURE);
  }

  //Setup address
  memset(&netSendAddrDebug, 0, sizeof(sockaddr_in));
  netSendAddrDebug.sin_family = AF_INET;
  netSendAddrDebug.sin_addr.s_addr = inet_addr(ip);
  netSendAddrDebug.sin_port = htons(port);
}

void NetSendDebug(const char *message, int len) {
  int res = sendto(netSendSocketDebug, message, len, 0, (sockaddr *)&netSendAddrDebug, sizeof(sockaddr_in));

  if (res < 0) {
    perror("Could not send debug data");
    exit(EXIT_FAILURE);
  }
}

//Receiving Stuff (Primary)
//##############################
static int netRcvSocket;
static sockaddr_in netRcvAddr;
static void (*netRcvCallback)(const unsigned char *, int len);
static pthread_t netRcvThread;

void *_NetRcvThread(void *) {
  while (true) {
    socklen_t addrlen = sizeof(sockaddr_in);

    const int MAXLEN = 100;
    static char _buffer[MAXLEN];
    char *buffer = _buffer;
    memset(_buffer, 0, sizeof(buffer));

    int nBytes = recvfrom(netRcvSocket, buffer, MAXLEN, 0, (sockaddr *)&netRcvAddr, &addrlen);

    //Offset counter
    nBytes = nBytes - 5;
    buffer += 5;

    static char counter[6];
    memcpy(counter, _buffer, sizeof(counter)-1);
    counter[sizeof(counter)-1] = '\0';
    int byteCount = atoi(counter); //What number byte
    static int lastByteCount = -1;  //Keep track of this below
    static int droppedPacketsEstimate = 0;

    if (lastByteCount != -1) {
      if (byteCount - lastByteCount > 1) {
        printf("\nWarning: Byte count was out of order. Went from %d -> %d\n", lastByteCount, byteCount);
        droppedPacketsEstimate += byteCount - lastByteCount - 1;
#ifdef CCUP
        CCSend("NetDroppedPacketsEstimate", (char *)&droppedPacketsEstimate, sizeof(droppedPacketsEstimate));
#endif
      }
    }

#ifdef CCUP
    CCSend("NetGotSomething", buffer, nBytes);
    CCSend("NetCounter", (char *)&byteCount, sizeof(byteCount));
#endif

    if (nBytes < 0) {
      perror("Tried to recvfrom but failed");
      exit(EXIT_FAILURE);
    }

    if (!buffer) {
      fprintf(stderr, "Net receive callback was not set");
      exit(EXIT_FAILURE);
    }

    //Perform the conversion to binary '10' => [1, 0]
    static unsigned char integerVersion[MAXLEN];
    for (int i = 0; i < nBytes; ++i) {
      integerVersion[i] = buffer[i] == '1';
    }

#ifdef CCUP
    //Integer version
    CCSend("NetSendToCallback", (char *)integerVersion, nBytes);
#endif

    lastByteCount = byteCount;
    netRcvCallback(integerVersion, nBytes);
  }
}

void NetRcvBegin(const char *ip, int port, void (*callback )(const unsigned char *, int len)) {
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

  //Join Multicast group DID NOT WORK BECAUSE OF DUPLICATES
  /*ip_mreq mreq;*/
  //mreq.imr_multiaddr.s_addr = inet_addr(ip);
  //mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  //res = setsockopt(netRcvSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(ip_mreq));
  //if (res < 0) {
    //perror("Could not join IP Multicast Group");
    //exit(EXIT_FAILURE);
  /*}*/

  //Start threaded server
  pthread_create(&netRcvThread, NULL, _NetRcvThread, NULL);
}

//Control port (TCP)
//########################################
static pthread_t netControlThread;
static int netControlSocket = -1;
static sockaddr_in netControlAddr;

void *_NetControlThread(void *a) {
  while (1) {
    listen(netControlSocket, 1024);

    static unsigned char buffer[200];
    recv(netControlSocket, buffer, sizeof(buffer), 0);

    //Process command
    switch (buffer[0]) {
      case 0:
        break;
    }
  }
}

void NetControlBegin(const char *ip, int port) {
  //Setup socket
  netControlSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (netControlSocket < 0) {
    perror("Could not create control socket");
    exit(EXIT_FAILURE);
  }

  //Setup address
  memset(&netControlAddr, 0, sizeof(sockaddr_in));
  netControlAddr.sin_family = AF_INET;
  netControlAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  netControlAddr.sin_port = htons(port);

  //Bind
  int res = bind(netControlSocket, (sockaddr *)&netControlAddr, sizeof(sockaddr_in));
  if (res < 0) {
    perror("Could not bind receive socket");
    exit(EXIT_FAILURE);
  }

  //Start threaded server
  pthread_create(&netControlThread, NULL, _NetControlThread, NULL);
}

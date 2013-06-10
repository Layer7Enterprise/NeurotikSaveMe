#ifndef NET_H_
#define NET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

//Setup UDP Multicast sending
void NetSendBegin(const char *ip, int port);
void NetSendBeginDebug(const char *ip, int port);

//Send UDP
void NetSend(const char *message, int len);
void NetSendDebug(const char *message, int len);

//Setup UDP Multicast receiver
void NetRcvBegin(const char *ip, int port, void (*callback)(const unsigned char *, int));

//Setup TCP Control port
void NetControlBegin(const char *ip, int port, void (*callback)(const unsigned char *, int));

#endif

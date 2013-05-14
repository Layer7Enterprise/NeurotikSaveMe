#ifndef NET_H_
#define NET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Setup UDP Multcast sending
void NetSendBegin(const char *ip, int port);

//Send UDP multicast
void NetSend(const char *message, int len);

#endif

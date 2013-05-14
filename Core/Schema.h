#ifndef SCHEMA_H_
#define SCHEMA_H_

#include <stdlib.h>
#include <stdio.h>

struct Schema_t {
	char name[100];        //Name of this server

	int NN;				         //Maximum number of neurons
	int ND;                //Maximum number of dendrites

	int port;			         //Base server port
};

void GetSchema(const char *filename, Schema_t *schema);

#endif

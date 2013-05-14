#ifndef GET_ENV_H_
#define GET_ENV_H_

#include <stdlib.h>
#include <stdio.h>

struct NeurotikEnv_t {
	char name[100];        //Name of this server
	char schemaFile[100];  //schema file name

	int NN;				   //Maximum number of neurons
	int ND;                //Maximum number of dendrites

	int port;			   //Base server port
	char host[100];		   //Server hostname

	int debug;             //Neuron to debug
};

/*
##################################################
IGGetEnv
	Get Environmental information from file

	filename - File to load
	env - Load file data into enviornment
##################################################
*/
void IGGetEnv(char *filename, NeurotikEnv_t *env);

#endif
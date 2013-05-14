#include "GetEnv.h"

void IGGetEnv(char *filename, NeurotikEnv_t *env) {
	FILE *f = fopen(filename, "r");

	if (!f) {
		fprintf(stderr, "Couldn't open environment file (%s)", filename);
		getchar();
		exit(EXIT_FAILURE);
	}

	char trash[100];
	fscanf(f, "%s %s", trash, env->name);
	fscanf(f, "%s %s", trash, env->schemaFile);
	fscanf(f, "%s %d", trash, &env->NN);
	fscanf(f, "%s %d", trash, &env->ND);
	fscanf(f, "%s %d", trash, &env->port);
	fscanf(f, "%s %s", trash, &env->host);
	fscanf(f, "%s %d", trash, &env->debug);

	printf(">Loaded environment file (%s)\n", filename);
	printf("	Name: %s\n", env->name);
	printf("	SchemaFile: %s\n", env->schemaFile);
	printf("	# Neurons: %d\n", env->NN);
	printf("	# Dendrites: %d\n", env->ND);
	printf("	Server Port: %d\n", env->port);
	printf("	Host Name: %s\n", env->host);
	printf("	Debug Neuron: %d\n", env->debug);

	fclose(f);
}
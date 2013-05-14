#include "Schema.h"

void GetSchema(const char *filename, Schema_t *schema) {
  FILE *f = fopen(filename, "r");

	if (!f) {
		fprintf(stderr, "Couldn't open schema file (%s)\n", filename);
		exit(EXIT_FAILURE);
	}

	char trash[100];
	fscanf(f, "%s %s", trash, schema->name);
	fscanf(f, "%s %d", trash, &schema->NN);
	fscanf(f, "%s %d", trash, &schema->ND);
	fscanf(f, "%s %d", trash, &schema->port);

	printf(">Loaded schema file (%s)\n", filename);
	printf("	Name: %s\n", schema->name);
	printf("	# Neurons: %d\n", schema->NN);
	printf("	# Dendrites: %d\n", schema->ND);
	printf("	Server Port: %d\n", schema->port);
}

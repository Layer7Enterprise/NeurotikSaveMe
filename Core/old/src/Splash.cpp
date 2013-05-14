#include "Splash.h"

void IGNeurotikSplash() {
	FILE *f = fopen("splash.txt", "r");

	if (!f) {
		fprintf(stderr, "Couldn't open splash.txt...\n");
		getchar();
		exit(EXIT_FAILURE);
	}

	const int SIZE = 8000;
	char buffer[SIZE];
	while (fgets(buffer, SIZE, f))
		fputs(buffer, stdout);

	printf("\n");
	printf("Built @ " __DATE__ " " __TIME__);
	printf("\n\n");
}
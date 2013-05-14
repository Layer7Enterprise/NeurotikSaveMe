#include "JSON.h"

//Convert float array into json compatible float array (Managed pointer)
static char *jsonBuffer = 0;
static int jsonBufferSize = 0;
char * FloatsToJson(const char *title, std::vector<std::string> names, int N, float *floats) {
	const int size = (N*9 + 2)+(N*100);
	if (jsonBufferSize < size) {
		delete [] jsonBuffer;
		jsonBuffer = new char[size];
		jsonBufferSize = size;
	}
	jsonBuffer[0] = 0;
	sprintf(jsonBuffer, "{\"name\":\"%s\",", title);
	sprintf(jsonBuffer+strlen(jsonBuffer), "\"data\":");
	sprintf(jsonBuffer+strlen(jsonBuffer), "[");
	for (int i = 0; i < N; ++i) {
		if (i == N-1)
			sprintf(jsonBuffer+strlen(jsonBuffer), "{\"name\":\"%s\", \"value\":%.5f}", names[i], floats[i]);
		else
			sprintf(jsonBuffer+strlen(jsonBuffer), "{\"name\":\"%s\", \"value\":%.5f}, ", names[i], floats[i]);
	}
	sprintf(jsonBuffer+strlen(jsonBuffer), "]}");

	return jsonBuffer;
}
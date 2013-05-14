#ifndef JSON_H_
#define JSON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

//Convert float array into json compatible float array (Managed pointer)
char * FloatsToJson(const char *title, std::vector<std::string> names, int N, float *floats);

#endif
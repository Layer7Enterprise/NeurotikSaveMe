#ifndef DEBUG_FILE_H_
#define DEBUG_FILE_H_

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

//This is really for plotting
class DebugFile {
public:
	DebugFile(std::string fileName);

	//Dump the file data
	void Dump();

	//Insert a value into the file
	void operator<<(float v);

	//Start a new line entry 
	void Next();
	std::string fileName;
	std::stringstream fileContents;
};

#endif
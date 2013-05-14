#include "DebugFile.h"

DebugFile::DebugFile(std::string fileName) 
	: fileName(fileName) {
}

//Dump the file data
void DebugFile::Dump() {
	std::ofstream out(fileName);
	out << fileContents.str();
}

//Insert a value into the file
void DebugFile::operator<<(float v) {
	fileContents << v << " ";
}

//Start a new line entry 
void DebugFile::Next() {
	fileContents << '\n';
}
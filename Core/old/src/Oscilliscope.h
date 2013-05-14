#ifndef OSCILLISCOPE_H_
#define OSCILLISCOPE_H_

#include <vector>
#include <opencv/cv.h>
#include <opencv/highgui.h>

class Oscilliscope {
public:
	Oscilliscope(const char *name, int width, int height, float *var, float minY, float maxY);
	~Oscilliscope();

	//Run
	void Tick();

	//Clear graph
	void Reset();
private:
	const int width;
	const int height;
	char *name;
	float *var;
	float minY;
	float maxY;
	IplImage *graph;

	//Where are we in the graph?
	int pos;

	//Last graph position (For drawing lines)
	int oldVal;
};

#endif
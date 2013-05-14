#include "Oscilliscope.h"

Oscilliscope::Oscilliscope(const char *name, int width, int height, float *var, float minY, float maxY)
	: width(width), height(height), var(var), minY(minY), maxY(maxY) {

	graph = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	Reset();

	this->name = new char[100];
	strcpy(this->name, name);
	cvShowImage(this->name, graph);
}

Oscilliscope::~Oscilliscope() {
	delete [] name;

	cvReleaseImage(&graph);
}

void Oscilliscope::Tick() {
	if (pos >= width)
		Reset();

	//Rescale point
	float val = *var;

	val = val - minY;
	val = val / (maxY - minY);

	int dVal = val * height;


	if (dVal > 0 && dVal < height) {
		//graph->imageData[dVal*width + pos] = 0xFF;
		cvLine(graph, cvPoint(pos, height-oldVal), cvPoint(pos, height-dVal), cvScalar(0xFF));
		oldVal = dVal;
	}

	++pos;

	cvShowImage(name, graph);
}

void Oscilliscope::Reset() {
	pos = 0;
	memset(graph->imageData, 0, width*height*graph->nChannels);
	oldVal = 0;
}
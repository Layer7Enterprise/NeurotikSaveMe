#include "Graph.h"

Graph::Graph(int width, int height, unsigned char *dataToRead, int xOffset, int yOffset) 
    : dataToRead(dataToRead), time(0), width(width), height(height) {
    buffer = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    memset(buffer->imageData, 0, width*height);
    cvShowImage("Graph", buffer);

	//Re-align window and remove border
	if (xOffset != 0 && yOffset != 0) {
		cvMoveWindow("Graph", xOffset, yOffset);
		HWND handle = FindWindow(0, L"Graph");
		SetWindowLong(handle, GWL_STYLE, 0);
		ShowWindow(handle, SW_SHOW);
	}
}

Graph::~Graph() {
    delete [] buffer;
}

void Graph::Tick() {
    //Copy from data to read
    for (int i = 0; i < height; ++i)
		if (dataToRead[i] == 1) {
			(buffer->imageData + i*buffer->widthStep)[time *buffer->nChannels + 1] = (dataToRead[i] > 0)*0xFF/((i%2)+1);
			(buffer->imageData + i*buffer->widthStep)[time *buffer->nChannels + 2] = (dataToRead[i] > 0)*0xFF/((i%4)+1);
			(buffer->imageData + i*buffer->widthStep)[time *buffer->nChannels + 3] = (dataToRead[i] > 0)*0xFF/((i%8)+1);
		} else if (dataToRead[i] == 2) {
			(buffer->imageData + i*buffer->widthStep)[time *buffer->nChannels + 2] = (dataToRead[i] > 0)*0xFF*(i%3);
		}

    cvShowImage("Graph", buffer);

    ++time;

    if (time >= width)
        Reset();
}

void Graph::Reset() {
	memset(buffer->imageData, 0, width*height*buffer->nChannels);
    time = 0;
}

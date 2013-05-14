#ifndef GRAPH_H_
#define GRAPH_H_

#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string.h>

class Graph {
public:
    //Create a graph, dataToRead should be same size as height
    Graph(int width, int height, unsigned char *dataToRead, int xOffset = 0, int yOffset = 0);
    ~Graph();

    void Tick();
    void Reset();
private:
    unsigned char *dataToRead;
    IplImage *buffer;
    int time;

    int width, height;
};

#endif

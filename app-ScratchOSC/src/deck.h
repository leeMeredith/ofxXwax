#pragma once

#include "ofMain.h"
#include "includes.h"

class deck {
public:
	void setup();
	void update();
	void draw(float newX, float newY);
    void audioInputListener(float* input, int audioBuffersize);
	
    float x;
    float y;
    
    ofxXwax xwax;
	
	bool serialReady;
	unsigned char serialThreshold;
	bool overThreshold;
	
	string recordFormat, recordSide, serialPort, audioInterface;
	unsigned int audioSamplerate, audioBuffersize;
	    
    graphicVinyl graphVinyl;
    graphicCurve graphCurve;
    
	deque<float> absolutePosition, relativePosition, faderPosition;
	
	unsigned int audioFrame;
    
    vector<float> input;
    int nChannels;

};

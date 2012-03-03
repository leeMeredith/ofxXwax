#pragma once

#include "ofMain.h"
#include "ofxXwax.h"
#include "ofxXmlSettings.h"
#include "drawVinyl.h"
#include "drawCurve.h"
#include "drawAudioInput.h"
#include "ofxOsc.h"
#include "ofxSoundStream.h"


class ofApp : public ofBaseApp {
public:
	void loadSettings();
	void setup();
	void update();
	void draw();
	
    ofxXwax xwaxLeft; 
    ofxXwax xwaxRight;
	
    ofxOscSender osc;
	ofSerial serial;
	
	bool serialReady;
	unsigned char serialThreshold;
	bool overThreshold;
	
	string recordFormat, recordSide, oscHost, serialPort, audioInterface;
	unsigned int oscPort, oscSubdivide, audioSamplerate, audioBuffersize;
	float oscRate;
	bool oscPitch, oscRelative, oscDegrees;
	
    
    drawVinyl drawVinyl_0;
    drawAudioInput drawAudioInput_0;
    
    drawVinyl drawVinyl_1;
    drawAudioInput drawAudioInput_1;
    
	
	vector<float> middleAudioBuffer, frontAudioBuffer;
	ofMutex audioMutex;
	
    vector<float> middleAudioBuffer_1, frontAudioBuffer_1;
	ofMutex audioMutex_1;
    
    drawCurve drawCurve_0;
    drawCurve drawCurve_1;
    
	deque<float> absolutePosition, relativePosition, faderPosition;
    deque<float> absolutePosition_1, relativePosition_1, faderPosition_1;
	
	unsigned int audioFrame;
    
    
    //soundStream---------_
    vector<float> inputLeft, inputRight;
    int nChannels;
    void audioInputListener (ofxAudioEventArgs &args);
	
    ofxSoundStream c1;
    //----soundStream-----_
};

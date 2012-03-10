#pragma once

#include "ofMain.h"
#include "includes.h"


class ofApp : public ofBaseApp {
public:
	void loadSettings();
	void setup();
	void update();
	void draw();
	
    ofxOscSender osc;
	ofSerial serial;
	graphicCurve drawFader;
    
	bool serialReady;
	unsigned char serialThreshold;
	bool overThreshold;
	
	string recordFormat, recordSide, oscHost, serialPort, audioInterface;
	unsigned int oscPort, oscSubdivide, audioSamplerate, audioBuffersize;
	float oscRate;
	bool oscPitch, oscRelative, oscDegrees;
    unsigned int audioFrame;
    
    deque<float> faderPosition;
    
    //deck-----------_
    int totalDecks;
    float cellWidth, cellHeight, incrementCellHeight;
    vector<deck> decks;
    //------deck-----_
    
    //soundStream---------_
    vector< vector<float> > inputs;
    int nChannels;
    void audioInputListener (ofxAudioEventArgs &args);
    ofxSoundStream c1;
    //----soundStream-----_
    
    //drawAudioInput----------_
    vector<graphicAudioInput> graphicAudioInputs;
    vector < vector<float> > middleAudioBuffers;
    ofMutex audioMutex;
    //-----drawAudioInput-----_
    
};

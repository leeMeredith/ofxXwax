#include "ofApp.h"

static const int maxData = 512;

void ofApp::loadSettings() {
	ofxXmlSettings xml;
	xml.loadFile("settings.xml");
	
	xml.pushTag("record");
	recordFormat = xml.getValue("format", "serato_2a");
	recordSide = xml.getValue("side", "right");
	xml.popTag();
	
	xml.pushTag("osc");
	oscHost = xml.getValue("host", "");
	oscPort = xml.getValue("port", 0);
	oscSubdivide = xml.getValue("subdivide", 8);
	oscPitch = xml.getValue("pitch", 0);
	oscRelative = xml.getValue("relative", 0);
	oscDegrees = xml.getValue("degrees", 0);
	xml.popTag();
	
	xml.pushTag("audio");
	audioSamplerate = xml.getValue("samplerate", 44100);
	audioBuffersize = xml.getValue("buffersize", 128);
    audioInterface = xml.getValue("interface", "");//soundStream audio interface by name
    totalDecks = xml.getValue("decks", 1);
	xml.popTag();
	
	xml.pushTag("serial");
	serialPort = xml.getValue("port", "");
	serialThreshold = xml.getValue("threshold", 0);
	xml.popTag();
}

void ofApp::setup() {
	ofSetWindowTitle("ScratchOSC");
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetCircleResolution(50);
	ofSetLineWidth(2);

	loadSettings();//load xml
    
    //fader--------_
    serialReady = serial.setup(serialPort, 115200);
    //----fader----_
    
	audioFrame = 0;
    
    nChannels = totalDecks * 2;
    
    //soundStream---------_
	c1.listDevices();
	c1.setDeviceIdByName(audioInterface);//"M-Audio, Inc.: M-Audio Conectiv"
	c1.setup(0, nChannels, this, 44100, 256, 4);//(int nOutputChannels, int nInputChannels, ofBaseApp *OFSA, unsigned int sampleRate, unsigned int bufferSize, unsigned int nBuffers)
	ofAddListener(c1.audioReceivedEvent, this, &ofApp::audioInputListener);
    //----soundStream-----_
    
    //inputs-----------_
    incrementCellHeight = 150;
    inputs.resize(nChannels);
    middleAudioBuffers.resize(nChannels);
    for(int c = 0; c < nChannels; c++) {
        inputs[c].resize(audioBuffersize * 2); // 2 for stereo
        middleAudioBuffers[c].resize(audioBuffersize * 2);
    }
    //------inputs-----_
    
    for (int i = 0; i < totalDecks; i++) {
        //deck-----------_
        deck myDeck;
        myDeck.audioSamplerate = audioSamplerate;
        myDeck.audioBuffersize = audioBuffersize;
        myDeck.audioInterface = audioInterface;
        myDeck.nChannels = nChannels;
        myDeck.recordFormat = recordFormat;
        myDeck.recordSide = recordSide;
        myDeck.serialPort = serialPort;
        myDeck.serialThreshold = serialThreshold;
        myDeck.setup();
        decks.push_back(myDeck);
        //------deck-----_
        
        //graphicAudioInputs----------_
        graphicAudioInput myGraphicAudioInput;
        myGraphicAudioInput.setup(audioBuffersize);    
        myGraphicAudioInput.audioBuffersize = audioBuffersize;
        graphicAudioInputs.push_back(myGraphicAudioInput);
        //-----graphicAudioInputs-----_
    }
}  

void ofApp::update() {
    for (int i = 0; i < totalDecks; i++) {
        //deck-----------_
        decks[i].update();
        decks[i].audioInputListener(&inputs[i][0], audioBuffersize);
        //------deck-----_
        //graphicAudioInputs----------_
        audioMutex.lock();
        graphicAudioInputs[i].frontAudioBuffer = middleAudioBuffers[i];
        audioMutex.unlock();
        //-----graphicAudioInputs-----_
    }
}

void ofApp::draw() {
	ofBackground(0);
	ofNoFill();
   
    for (int i = 0; i < totalDecks; i++) {
        if (i == 0) {
            cellHeight = 0; 
        }else{
            cellHeight = cellHeight + incrementCellHeight;
        }
        //deck-----------_
        decks[i].draw(0,cellHeight);
        //------deck-----_
        //graphicAudioInputs----------_
        graphicAudioInputs[i].draw(10, cellHeight+10, 128);
        //-----graphicAudioInputs-----_
    }
    //fader--------_
    ofPushMatrix();
    ofSetColor(255);
    drawFader.draw(faderPosition, 128, 0, 255);
	ofPopMatrix();
    //----fader----_
}

static float adder = 0;

//--------------------------------------------------------------
void ofApp::audioInputListener(ofxAudioEventArgs &args){	

    //inputs-----------_
    // samples are "interleaved"    
    int sample = 0;
    for(int i = 0; i < args.bufferSize; i++) {
        int k = i * 2; // 2 for stereo
        for(int c = 0; c < nChannels; c++) {
            inputs[c][k + 0] = args.buffer[sample++];
            inputs[c][k + 1] = args.buffer[sample++];
        }
    }
    //------inputs-----_
    
    //drawudioInput----------_
	for (int i = 0; i < totalDecks; i++) {
        audioMutex.lock();
        middleAudioBuffers[i].assign(&inputs[i][0], &inputs[i][0] + args.bufferSize * 2); 
        audioMutex.unlock();
    }
    //-----drawudioInput-----_
    
    //fader--------_
	if(serialReady) {
		unsigned char faderData;
		int nRead;
		while ((nRead = serial.readBytes(&faderData, 1)) > 0) {
			bool prevOverThreshold = overThreshold;
			overThreshold = faderData > serialThreshold;
			if(prevOverThreshold != overThreshold) {
				ofxOscMessage msg;
				msg.setAddress("/scratch/mixer/fader");
				msg.addFloatArg(overThreshold ? 1 : 0);
				osc.sendMessage(msg);
			}
			faderPosition.push_back(faderData);
            if(faderPosition.size() > maxData) {
                faderPosition.pop_front();
            }
		}
	}
    //----fader----_
}

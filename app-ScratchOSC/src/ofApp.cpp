#include "ofApp.h"

static const ofColor cyanPrint = ofColor::fromHex(0x00abec);
static const ofColor magentaPrint = ofColor::fromHex(0xec008c);
static const ofColor yellowPrint = ofColor::fromHex(0xffee00);
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
	
	loadSettings();
	oscRate = (audioSamplerate / audioBuffersize) / oscSubdivide;
    
    xwaxLeft.setup(audioSamplerate, audioBuffersize, recordFormat);
    xwaxRight.setup(audioSamplerate, audioBuffersize, recordFormat);
    nChannels = 4;
	
    ofSoundStreamSetup(0, 2, audioSamplerate, audioBuffersize, 1);
	
	osc.setup(oscHost, oscPort);
	
	serialReady = serial.setup(serialPort, 115200);
	
	middleAudioBuffer.resize(audioBuffersize * 2);
	frontAudioBuffer.resize(audioBuffersize * 2);
    
    middleAudioBuffer_1.resize(audioBuffersize * 2);
	frontAudioBuffer_1.resize(audioBuffersize * 2);
    
    drawAudioInput_0.setup(audioBuffersize);    
    drawAudioInput_0.audioBuffersize = audioBuffersize;
    
    drawAudioInput_1.setup(audioBuffersize);    
    drawAudioInput_1.audioBuffersize = audioBuffersize;
    
	audioFrame = 0;
    
    inputLeft.resize(audioBuffersize);
    inputRight.resize(audioBuffersize);
 
    
    //soundStream---------_
	c1.listDevices();
	c1.setDeviceIdByName(audioInterface);//"M-Audio, Inc.: M-Audio Conectiv"
	c1.setup(0, 4, this, 44100, 256, 4);
	
	ofAddListener(c1.audioReceivedEvent, this, &ofApp::audioInputListener);
    //----soundStream-----_
    
}  

void ofApp::update() {
	audioMutex.lock();
	frontAudioBuffer = middleAudioBuffer;
    drawAudioInput_0.frontAudioBuffer = middleAudioBuffer;
    frontAudioBuffer_1 = middleAudioBuffer_1;
    drawAudioInput_1.frontAudioBuffer = middleAudioBuffer_1;
	audioMutex.unlock();
    
    drawVinyl_0.rotateAbsolute = ofxXwax::millisToDegrees(xwaxLeft.getAbsolute());
    drawVinyl_0.rotateRelative = ofxXwax::millisToDegrees(xwaxLeft.getRelative());
    
    drawVinyl_1.rotateAbsolute = ofxXwax::millisToDegrees(xwaxRight.getAbsolute());
    drawVinyl_1.rotateRelative = ofxXwax::millisToDegrees(xwaxRight.getRelative());
}

void ofApp::draw() {
	ofBackground(0);
	ofNoFill();
	ofPushMatrix();
	ofTranslate(10, 10);
    drawAudioInput_0.draw(0, 0, 128);
    drawAudioInput_1.draw(0, 200, 128);
    drawVinyl_0.draw(128, 0, 128);
    drawVinyl_1.draw(128, 200, 128);
	ofTranslate(256, 0);
	ofSetColor(yellowPrint);
    drawCurve_0.draw(absolutePosition, 128);
	ofSetColor(magentaPrint);
    drawCurve_0.draw(relativePosition, 128);
    ofTranslate(0, 200);
	ofSetColor(yellowPrint);
    drawCurve_1.draw(absolutePosition_1, 128);
	ofSetColor(magentaPrint);
    drawCurve_1.draw(relativePosition_1, 128);
    
	ofSetColor(255);
    drawCurve_0.draw(faderPosition, 128, 0, 255);
    drawCurve_1.draw(faderPosition_1, 128, 200, 255);
	ofPopMatrix();
	
	ofSetColor(255);
	
	ofPushMatrix();
	ofDrawBitmapString("pitch: " + ofToString(xwax.getPitch(), 2) + "x", 0, 20);
	ofTranslate(10, ofGetHeight() - 140);
	ofDrawBitmapString("pitch: " + ofToString(xwaxLeft.getPitch(), 2) + "x", 0, 20);
	ofDrawBitmapString("velocity: " + ofToString(xwaxLeft.getVelocity(), 2) + " ms", 0, 40);
	ofDrawBitmapString("relative: " + ofToString(xwaxLeft.getRelative(), 0) + " ms", 0, 60);
	ofDrawBitmapString("absolute: " + ofToString(xwaxLeft.getAbsolute(), 0) + " ms", 0, 80);
	ofDrawBitmapString("absolute valid: " + ofToString(xwaxLeft.isAbsoluteValid() ? "yes" : "no"), 0, 100);
	ofPopMatrix();
    
    ofPushMatrix();
	ofTranslate(206, ofGetHeight() - 140);
	ofDrawBitmapString("pitch: " + ofToString(xwaxRight.getPitch(), 2) + "x", 0, 20);
	ofDrawBitmapString("velocity: " + ofToString(xwaxRight.getVelocity(), 2) + " ms", 0, 40);
	ofDrawBitmapString("relative: " + ofToString(xwaxRight.getRelative(), 0) + " ms", 0, 60);
	ofDrawBitmapString("absolute: " + ofToString(xwaxRight.getAbsolute(), 0) + " ms", 0, 80);
	ofDrawBitmapString("absolute valid: " + ofToString(xwaxRight.isAbsoluteValid() ? "yes" : "no"), 0, 100);
	ofPopMatrix();
	
	ofPushMatrix();
	ofTranslate(406, ofGetHeight() - 140);
	ofDrawBitmapString("format: " + xwaxLeft.getFormat() + " (" + recordSide +")", 0, 20);
	ofDrawBitmapString("osc: " + oscHost + ":" + ofToString(oscPort), 0, 40);
	ofDrawBitmapString("subdivide: " + ofToString(oscSubdivide) + " (" + ofToString(oscRate, 1) + "Hz)", 0, 60);
	ofDrawBitmapString("audio: " + ofToString(audioSamplerate) + "Hz, " + ofToString(audioBuffersize) + " samples", 0, 80);
	ofDrawBitmapString("serial: " + ofToString(serialPort), 0, 100);
    ofDrawBitmapString("audioIn: " + audioInterface, 0, 120);
	ofPopMatrix();
	ofPushMatrix();
    
    //soundStream---------_
	// draw the left:
    ofPushMatrix();
    ofTranslate(10, 160);
	ofSetColor(55,55,55);
	for (int i = 0; i < 256; i++){
		ofLine(0+i,0,0+i,0+inputLeft[i*2+0]*100.0f);
	}
	ofSetColor(155,55,55);
	for (int i = 0; i < 256; i++){
		ofLine(300+i,0,300+i,0+inputLeft[i*2+1]*100.0f);
	}
	ofPopMatrix();
    
    // draw the right:
    ofPushMatrix();
    ofTranslate(10, 360);
	ofSetColor(55,55,55);
	for (int i = 0; i < 256; i++){
		ofLine(0+i,0,0+i,0+inputRight[i*2+0]*100.0f);
	}
	ofSetColor(155,55,55);
	for (int i = 0; i < 256; i++){
		ofLine(300+i,0,300+i,0+inputRight[i*2+1]*100.0f);
	}
    ofPopMatrix();
    //----soundStream-----_
}

static float adder = 0;

//--------------------------------------------------------------
void ofApp::audioInputListener(ofxAudioEventArgs &args){	
	// samples are "interleaved"
    for(int i = 0; i < args.bufferSize; i++) {
        int j = i * nChannels; // nChannels = 4
        int k = i * 2; // 2 for stereo
        inputLeft[k + 0] =  args.buffer[j + 0];
        inputLeft[k + 1] =  args.buffer[j + 1];
        inputRight[k + 0] = args.buffer[j + 2];
        inputRight[k + 1] = args.buffer[j + 3];
    }
    
    xwaxLeft.update(&inputLeft[0]);
    xwaxRight.update(&inputRight[0]);
	
    //xwaxLeft--------_
	audioMutex.lock();
    middleAudioBuffer.assign(&inputLeft[0], &inputLeft[0] + args.bufferSize * 2); 
	audioMutex.unlock();
	
	absolutePosition.push_back(xwaxLeft.getAbsolute());
	relativePosition.push_back(xwaxLeft.getRelative());
	if(absolutePosition.size() > maxData) {
		absolutePosition.pop_front();
	}
	if(relativePosition.size() > maxData) {
		relativePosition.pop_front();
	}
    
    //xwaxRight-------_
    audioMutex.lock();
    middleAudioBuffer_1.assign(&inputRight[0], &inputRight[0] + args.bufferSize * 2); 
	audioMutex.unlock();
	
	absolutePosition_1.push_back(xwaxRight.getAbsolute());
	relativePosition_1.push_back(xwaxRight.getRelative());
	if(absolutePosition_1.size() > maxData) {
		absolutePosition_1.pop_front();
	}
	if(relativePosition_1.size() > maxData) {
		relativePosition_1.pop_front();
	}

    
	if(audioFrame % oscSubdivide == 0) {
		float position = oscRelative ? xwaxLeft.getRelative() : xwaxLeft.getAbsolute();
		ofxOscMessage msg;
		msg.setAddress("/scratch/record/" + recordSide);
		msg.addFloatArg(position);
		osc.sendMessage(msg);
		if(oscPitch) {
			ofxOscMessage msg;
			msg.setAddress("/scratch/record/" + recordSide + "/pitch");
			msg.addFloatArg(xwaxLeft.getPitch());
			osc.sendMessage(msg);
		}
		if(oscDegrees) {
			ofxOscMessage msg;
			msg.setAddress("/scratch/record/" + recordSide + "/degrees");
			msg.addFloatArg(fmodf(xwaxLeft.millisToDegrees(position), 360));
			osc.sendMessage(msg);
		}
	}
	audioFrame++;
	
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
}

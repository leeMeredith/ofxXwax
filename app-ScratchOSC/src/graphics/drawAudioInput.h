#pragma once

#include "ofMain.h"

class drawAudioInput{
public:
    void setup(unsigned int newAudioBuffersize);
	void draw(float x, float y, float scale);
    unsigned int audioBuffersize;
    vector<float>frontAudioBuffer;
};

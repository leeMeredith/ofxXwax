#include "ofApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 950, 400, OF_WINDOW);
	ofRunApp(new ofApp());
}

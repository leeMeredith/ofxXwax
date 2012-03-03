#include "ofApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 768, 556, OF_WINDOW);
	ofRunApp(new ofApp());
}

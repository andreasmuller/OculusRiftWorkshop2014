#pragma once
#include <math.h>
#include "ofMain.h"
#include "ofxOculusRift.h"
#include "ofxGui.h"
//#include "ofxOpenCv.h"
//#include "ofxKinect.h"

#include "Utils/Cameras/ofxWalkingFirstPersonCameraOculus.h"

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();
		void exit();
		
		void drawScene();
	
		void drawFloor();
		
		void keyPressed(int key);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
		ofxOculusRift		oculusRift;
	
		ofxWalkingFirstPersonCameraOculus camera;
		//ofxFirstPersonCamera camera;
	
		ofTrueTypeFont		fontSmall;
		ofTrueTypeFont		fontLarge;
		
		ofxPanel			gui;
		bool				drawGui;
};

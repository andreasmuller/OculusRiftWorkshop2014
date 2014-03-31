#pragma once
#include <math.h>
#include "ofMain.h"
#include "ofxOculusRift.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxAssimpModelLoader.h"
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
	
		void drawPointCloud();
		void drawFloor();
		
		void keyPressed(int key);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
		float triangleArea( ofVec3f _p0, ofVec3f _p1, ofVec3f _p2 );
	
		ofxOculusRift		oculusRift;
	
		ofxWalkingFirstPersonCameraOculus camera;
		//ofxFirstPersonCamera camera;
	
		ofxAssimpModelLoader model;
	
		ofTrueTypeFont		fontSmall;
		ofTrueTypeFont		fontLarge;
	
		float				prevUpdateTime;
	
		ofLight				light1;
		ofLight				light2;
		
		ofxPanel			gui;
		bool				drawGui;
};

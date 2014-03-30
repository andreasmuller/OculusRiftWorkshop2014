#pragma once
#include <math.h>
#include "ofMain.h"
#include "ofxOculusRift.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

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
	
		ofParameter<int>	kinectMeshStep;
		ofParameter<float>	triangleMaxArea;
	
	
		ofxKinect			kinect;
		vector<float>		pixelDistance;
		vector<bool>		pixelIsValid;
	
		ofxWalkingFirstPersonCameraOculus camera;
		//ofxFirstPersonCamera camera;
	
		int					nearThreshold;
		int					farThreshold;
		
		int					angle;
	
		ofTrueTypeFont		fontSmall;
		ofTrueTypeFont		fontLarge;
		
		ofxPanel			gui;
		bool				drawGui;
};

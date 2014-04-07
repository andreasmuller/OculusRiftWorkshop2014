#pragma once

#include "ofMain.h"
#include "ofxOculusRift.h"
#include "Utils/Cameras/ofxWalkingFirstPersonCameraOculus.h"

// Lots in this example comes from the great demo by James George:
// https://github.com/obviousjim/ofxOculusRift/blob/master/example-OculusRiftRendering/src/testApp.h

// test from iOS app

// -----------------------------------------
class DemoSphere
{
	public:
	
		ofColor color;
		ofVec3f pos;
		float radius;
		int myID;
		int spacePartitioningIndexX;
		int spacePartitioningIndexY;
		float mouseDist;
		bool isMouseOvered;
};

// -----------------------------------------
class ofApp : public ofBaseApp
{
  public:
	
	void setup();
	void update();
	void draw();
	
	void drawScene();
	
	void updateLinesMesh();
	void initializeSpacePartitioning();
	void updateSpacePartitioning();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxOculusRift		oculusRift;

	ofLight				light;
	//ofCamera			cam;
	ofxWalkingFirstPersonCameraOculus camera;
	
	bool showOverlay;
	bool predictive;
	vector<DemoSphere> demos;
	float gridSize;
	
	float mouseAffectionRadius;
	float lineConnectionMaxDistance;
	float lineConnectionMaxDistanceMin;
	float lineConnectionMaxDistanceMax;
	ofMesh linesMesh;
    
    ofVec3f cursorRift;
    ofVec3f demoRift;
	
	ofVec2f lastMouse;
	ofVec2f rotationSpeed;
	
	ofTrueTypeFont meshFont;
	
	vector< vector< vector< DemoSphere* > > > spacePartitioningGrid;
	int					spacePartitioningResX;
	int					spacePartitioningResY;
	float				spacePartitioningGridWidth;
	float				spacePartitioningGridHeight;
};

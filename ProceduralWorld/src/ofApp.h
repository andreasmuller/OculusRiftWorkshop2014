#pragma once

#include "ofMain.h"
#include "ofxGui.h"
//#include "ofxAutoReloadedShader.h"
#include "ofMeshExt.h"
#include "Cameras/ofxWalkingFirstPersonCamera.h"

class ofApp : public ofBaseApp
{
	public:
	
		void setup();
		void update();
		void draw();
		
		void drawScene( ofCamera* _cam );
	
		void setTerrainCentrePosition( ofVec3f _pos );
	
		float getHeightmapValue( int _x, int _y );
		float getTerrainHeightAtWorldSpaceCoordinate( float _x, float _y );
		ofVec3f getTerrainNormalAtWorldSpaceCoordinate( float _x, float _y );
	
		void generateTerrainMesh();
		void generateGrassMesh();
		void generateFlyersMesh();
	
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
		ofParameter<ofVec3f> dimension;
	
		ofParameter<bool> doGravity;
	
		ofParameter<float> stalkWidth;
		ofParameter<float> stalkHeight;
		ofParameter<float> swayingMaxAngle;

		ofParameter<float> grassNoisePlacementThreshold;
	
		ofParameter<int> grassLayingMethod;
		ofParameter<float> grassPlacementMaxHeight;
		ofParameter<float> grassPlacementNoiseSpaceDiv;
	
		ofParameter<float> grassSwayingNoiseSpaceDiv;
		ofParameter<float> grassSwayingNoiseMag;
		ofParameter<float> grassSwayingTimeScale;

		ofParameter<float> flyerTimeScale;
		ofParameter<float> flyerTimeVariation;

		ofParameter<float> flyerSwarmOffsetNoiseSpaceDiv;
		ofParameter<float> flyerSwarmOffsetMagnitude;
		ofParameter<float> flyerPlacementNoiseSpaceDiv;
		ofParameter<float> flyerPlacementMaxHeight;
		ofParameter<float> flyerPlacementNoiseThreshold;
		ofParameter<float> flyerMinHeight;
	
		ofParameter<int> flyerTailLength;
		ofParameter<float> flyerTailStepTimeOffset;
		ofParameter<float> flyerMovementNoiseSpaceDivider;
		ofParameter<ofVec3f> flyerMovementMagnitude;
	
		ofParameter<int> heightmapRes;
		vector<float> heightmap;
	
		ofFloatColor currentMainColor;
	
	
		ofMesh terrainMesh;
		ofMesh grassMesh;
		ofMeshExt flyersMesh;
		//ofMesh flyersMesh;
	
		ofVec3f terrainPosition;
		ofVec3f terrainMeshRenderingPosition; // we keep a shifted copy of the position
		int noisePositionX;
		int noisePositionZ;
		ofVec3f noiseStartPos;

	
		ofImage streakTexture;
	
		//ofxAutoReloadedShader grassShader;
		ofShader grassShader;
		//ofxAutoReloadedShader flyersShader;
		ofShader flyersShader;	

		bool isWalking;
		ofEasyCam godCamera;
		ofxWalkingFirstPersonCamera walkingCamera;
		ofCamera* camera;
	
		bool debugDrawNormals;
		ofMesh debugNormalsMesh;

		//vector<ofPoint> points;
	
		ofTrueTypeFont fontSmall;
	
		ofxPanel gui;
		bool showGui;
};

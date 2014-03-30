#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetFrameRate(999);
	ofSetVerticalSync( true );
		
	oculusRift.setup();
}

//--------------------------------------------------------------
void testApp::update()
{

}

//--------------------------------------------------------------
void testApp::draw()
{
	ofEnableDepthTest();

		ofPushMatrix();

			ofTranslate( ofPoint( ofGetWidth()*0.5f, ofGetHeight()*0.5f, 0) ); // move to the middle

			ofMultMatrix( oculusRift.getOrientationMat() ); // grab the orientation from the rift headset and rotate

			ofSetColor( 255, 0, 0 ); // big red box
			ofBox( 200 );
	
			ofTranslate( 0.0f, 0.0f, 100.0f ); // small blue box so we can see orientation
			ofSetColor( 0, 0, 255 );
			ofBox( 100 );

		ofPopMatrix();
		
	ofDisableDepthTest();
}

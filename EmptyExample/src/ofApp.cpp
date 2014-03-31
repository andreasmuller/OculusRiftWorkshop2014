#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	fontSmall.loadFont("Fonts/DIN.otf", 8);
	fontLarge.loadFont("Fonts/DIN.otf", 20);
	
	gui.setup( "Settings", "Settings/Main.xml" );

	drawGui = true;
	
	//enable mouse;
	camera.begin();
	camera.end();
	camera.setGlobalPosition( 0, 100, 500 ); // let's start by stepping back a little bit and put ourselves a few units up from the floor

	oculusRift.baseCamera = &camera;
	oculusRift.setup();
	
	if( oculusRift.isSetup() )
	{
		ofHideCursor();
	}
	
}

//--------------------------------------------------------------
void ofApp::update()
{
	
	camera.setHeadsetOrientation( oculusRift.getOrientationQuat() );
	camera.update();
	
	ofBackground(100, 100, 100);
	
	camera.update();

}

//--------------------------------------------------------------
void ofApp::draw() {
	
	ofSetColor(255, 255, 255);

	if(oculusRift.isSetup())
	{
        
		ofEnableDepthTest();
		
			oculusRift.beginLeftEye();
				drawScene();
			oculusRift.endLeftEye();
			
			oculusRift.beginRightEye();
				drawScene();
			oculusRift.endRightEye();
						
		ofDisableDepthTest();
		
		oculusRift.draw();
	}
	else
	{
		camera.begin();
			drawScene();
		camera.end();
		
		ofSetColor(255, 255, 255);
		fontSmall.drawString( ofToString( ofGetFrameRate(), 2), ofGetWidth()-30, ofGetHeight()-8 );
	}
	
	if( drawGui )
	{
		gui.draw();
	}
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
	drawFloor();
	
	ofSetColor( ofColor::white );
	
	ofPushMatrix();


	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawFloor()
{
	ofSetColor(20, 20, 20);
	ofPushMatrix();
		ofRotate(90, 1, 0, 0);
		ofDrawPlane( 1600.0f, 1600.0f );
	ofPopMatrix();
	
	ofDisableDepthTest();
		ofSetColor(40, 40, 40);
		ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			ofDrawGridPlane(800.0f, 10.0f, false );
		ofPopMatrix();
	ofEnableDepthTest();
}

//--------------------------------------------------------------
void ofApp::exit() 
{
	
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key)
{
	switch (key)
	{
		case OF_KEY_TAB:
			drawGui = !drawGui;
			break;
			
		case ' ':
			break;
						
		case 'f':
			ofToggleFullscreen();
			break;

		case 'p':
			ofShowCursor();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{	
}

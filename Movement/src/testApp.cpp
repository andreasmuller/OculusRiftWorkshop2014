#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	ofBackground(0);
	ofSetLogLevel( OF_LOG_VERBOSE );
	ofSetVerticalSync( true );

	//ofToggleFullscreen();
	
	showOverlay = false;
	predictive = true;
	
	ofHideCursor();
	
	oculusRift.baseCamera = &cam;
	oculusRift.setup();
	
	gridSize = 150.0f;
	
	//enable mouse;
	cam.begin();
	cam.end();
	cam.setGlobalPosition( 0, 100, 500 ); // let's start by stepping back a little bit and put ourselves a few units up from the floor
	
	lastMouse = ofVec2f( ofGetMouseX(), ofGetMouseY() );
	

}


//--------------------------------------------------------------
void testApp::update()
{
	
	// Update mouse values
	ofVec2f mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
	ofVec2f mouseVel = mouse - lastMouse;
	lastMouse = mouse;
	
	// Movement
	ofVec3f forward =  ofVec3f(0,0,1) * oculusRift.getOrientationQuat() * cam.getOrientationQuat();
	ofVec3f sideways = ofVec3f(1,0,0) * oculusRift.getOrientationQuat() * cam.getOrientationQuat();
	
	float movementSpeed = 2.0f;
	if( ofGetKeyPressed('w') ) cam.setPosition( cam.getPosition() + (-forward * movementSpeed) );
	if( ofGetKeyPressed('s') ) cam.setPosition( cam.getPosition() + ( forward * movementSpeed) );
	
	if( ofGetKeyPressed('a') ) cam.setPosition( cam.getPosition() + (-sideways * movementSpeed) );
	if( ofGetKeyPressed('d') ) cam.setPosition( cam.getPosition() + ( sideways * movementSpeed) );
	
	// Rotation
	float rotationDrag = 0.8f;
	float rotationInertia = 0.2f;
	float mouseSensitivity = 0.3f;
	
	// Apply draw to rotation and stop when it gets very close to zero
	rotationSpeed *= rotationDrag;
	if( rotationSpeed.length() < 0.00000001f ) { rotationSpeed = ofVec3f(0,0,0); }

	// When we hold down the right mouse button, calculate some rotation in the Y axis.
	if( ofGetMousePressed( OF_MOUSE_BUTTON_RIGHT) )
	{
		rotationSpeed.y = ofLerp( rotationSpeed.y,  mouseVel.x * mouseSensitivity, rotationInertia );
	}

	ofQuaternion tmpRotY( rotationSpeed.y, ofVec3f(0,1,0));
	cam.setOrientation( cam.getOrientationQuat() * tmpRotY );
	
}


//--------------------------------------------------------------
void testApp::draw()
{

	if(oculusRift.isSetup())
	{
		if(showOverlay)
		{
			oculusRift.beginOverlay(-230, 320,240);
			ofRectangle overlayRect = oculusRift.getOverlayRectangle();
			
			ofPushStyle();
			ofEnableAlphaBlending();
			ofFill();
			ofSetColor(255, 40, 10, 200);
			
			ofRect(overlayRect);
			
			ofSetColor(255,255);
			ofFill();
			ofDrawBitmapString("ofxOculusRift by\nAndreas Muller\nJames George\nJason Walters\nElie Zananiri\nFPS:"+ofToString(ofGetFrameRate())+"\nPredictive Tracking " + (oculusRift.getUsePredictiveOrientation() ? "YES" : "NO"), 40, 40);
			
			ofPopStyle();
			oculusRift.endOverlay();
		}
        
		glEnable(GL_DEPTH_TEST);
		oculusRift.beginLeftEye();
		drawScene();
		oculusRift.endLeftEye();
		
		oculusRift.beginRightEye();
		drawScene();
		oculusRift.endRightEye();
		
		oculusRift.draw();
		
		glDisable(GL_DEPTH_TEST);
	}
	else{
		cam.begin();
		drawScene();
		cam.end();
	}
	
}

//--------------------------------------------------------------
void testApp::drawScene()
{
	ofEnableDepthTest();
		
		// Draw the floor
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
	
		//ofDrawPlane( 1000.0f, 1000.0f );

		ofSetColor( ofColor::white );

		
		ofNoFill();
		// Billboard and draw the mouse
		if(oculusRift.isSetup())
		{
			ofPushMatrix();
				ofSetColor(255, 0, 0);
				oculusRift.multBillboardMatrix();
				ofCircle(0,0,.5);
			ofPopMatrix();
		}
		
		ofSetColor( ofColor::white );
		ofFill();
	
	ofDisableDepthTest();
	
}
//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	if( key == 'f' )
	{
		//gotta toggle full screen for it to be right
		ofToggleFullscreen();
	}
	
	if(key == 's'){
		oculusRift.reloadShader();
	}
	
	if(key == 'l'){
		oculusRift.lockView = !oculusRift.lockView;
	}
	
	if(key == 'o'){
		showOverlay = !showOverlay;
	}
	if(key == 'r'){
		oculusRift.reset();
		
	}
	if(key == 'h'){
		ofHideCursor();
	}
	if(key == 'H'){
		ofShowCursor();
	}
	
	if(key == 'p'){
		predictive = !predictive;
		oculusRift.setUsePredictedOrientation(predictive);
	}
}

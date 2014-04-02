#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	fontSmall.loadFont("Fonts/DIN.otf", 8);
	fontLarge.loadFont("Fonts/DIN.otf", 20);
	
	gui.setup( "Settings", "Settings/Main.xml" );

	// add any gui parameters here

	drawGui = false;
	
	
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
	
	model.setScaleNomalization( false );
    model.loadModel("Models/AstroBoy/astroBoy_walk.dae", true);
    model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
    model.playAllAnimations();

	light1.setPosition( ofVec3f(-250, 300, 0) );
	light1.setDiffuseColor( ofColor( 200, 0, 0) );
	light1.enable();
	
	light2.setPosition( ofVec3f( 250, 300, 0) );
	light2.setDiffuseColor( ofColor( 0, 0, 200 ) );
	light2.enable();
	
	ofSetGlobalAmbientColor( ofColor( 10, 10, 10 ) );
	ofSetSmoothLighting( true );
	
	prevUpdateTime = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::update()
{
	camera.setHeadsetOrientation( oculusRift.getOrientationQuat() );
	camera.update();
	
	ofBackground(100, 100, 100);
	
	model.update();
	
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
		fontSmall.drawString( ofToString( ofGetFrameRate(), 2), ofGetWidth()-20, ofGetHeight() - 20 );
	}
	
	if( drawGui )
	{
		gui.draw();
	}
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
	float time = ofGetElapsedTimef();
	float timeDelta = prevUpdateTime - time;
	prevUpdateTime = time;
	
	float timeScale = 0.05f;
	
	time *= timeScale;
	timeDelta *= timeScale;
	
	drawFloor();
	
	ofEnableLighting();
	
	ofSetColor( ofColor::white );
	
	// draw a couple of the models that we loaded, randommizing a walk on the stage for them
	float wanderingMagnitude = 600.0f;
	float scale = 10.0f;
	
	int numWalkers = 10;
	ofSeedRandom( 1024 ); // make sure we get the same results from random every time
	for( int i = 0; i < numWalkers; i++ )
	{
		ofVec3f noisePos(0,0,0);
		noisePos.x = ofRandom( -100, 100 );
		noisePos.z = ofRandom( -100, 100 );
		
		noisePos.x = 31; // uncomment these two lines to get a conga line
		noisePos.z = 70;
		
		ofPushMatrix();
		
			float walkerTime = time + ((0.08f) * i);
		
			ofVec3f pos(0,0,0);
			pos.x = ofSignedNoise( noisePos.x, walkerTime ) * wanderingMagnitude;
			pos.z = ofSignedNoise( noisePos.z, walkerTime ) * wanderingMagnitude;
		
			ofVec3f prevPos(0,0,0);
			prevPos.x = ofSignedNoise( noisePos.x, walkerTime - timeDelta ) * wanderingMagnitude;
			prevPos.z = ofSignedNoise( noisePos.z, walkerTime - timeDelta ) * wanderingMagnitude;
		
			ofVec3f tmpDirection = (pos - prevPos).getNormalized();
		
			ofTranslate( pos );
			ofRotateY( 270 - ofRadToDeg(atan2f( tmpDirection.z, tmpDirection.x ))  );
		
			// Todo: drive the animation of the walker so that it advances based on the distance traveled, would help offset the animation as well
		
			ofScale( scale, -scale, scale );
			model.drawFaces();
		
		ofPopMatrix();
	}
	
	ofSeedRandom(); // seeds random with the time again so that random results are.. random.
	
	ofDisableLighting();
	
	light1.draw();
	light2.draw();
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

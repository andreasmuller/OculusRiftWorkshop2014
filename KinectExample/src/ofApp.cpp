#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	fontSmall.loadFont("Fonts/DIN.otf", 8);
	fontLarge.loadFont("Fonts/DIN.otf", 20);
	
	gui.setup( "Settings", "Settings/Main.xml" );
	gui.add( kinectMeshStep.set("Kinect Mesh Res", 1, 1, 20));

	gui.add( triangleMaxArea.set("Triangle Max Area", 400, 0, 800));

	drawGui = true;
	
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
	
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
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
	
	kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew())
	{
		
	}
	
	//camera.setHeadsetOrientation( oculusRift.getOrientationQuat() );
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
		fontSmall.drawString( ofToString( ofGetFrameRate(), 2), ofGetWidth()-30, ofGetHeight()-6 );
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
		ofTranslate(ofVec3f(0,140,0));
		ofScale( 0.1f,0.1f,0.1f );
		drawPointCloud();
	ofPopMatrix();
	
}

//--------------------------------------------------------------
void ofApp::drawPointCloud()
{
	if( !kinect.isConnected() )
	{
		return;
	}
	
	int w = 640;
	int h = 480;
	
	ofMesh mesh;
		
	// Let's mark all valid pixels
	pixelIsValid.clear();
	pixelDistance.clear();
	for(int y = 0; y < h; y++ )
	{
		for(int x = 0; x < w; x++ )
		{
			mesh.addTexCoord( ofVec2f(x,y) );
			mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			
			pixelDistance.push_back( kinect.getDistanceAt(x, y) );
			pixelIsValid.push_back( pixelDistance.back() > 0 );
		}
	}
	
	int step = kinectMeshStep;
	
	triangleMaxArea.setMax( 4000 * step ); // this is a bit arbitrary, todo: figure out a good value for this
		
	vector<ofVec3f>& vertices = mesh.getVertices();
	vector<int> vertexIndexLookup;
	
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
		
	// Let's add a triangle when we have 3 valid points in our grid and the triangle is small enough
	int currVertexIndex = 0;
	for(int y = 0; y < h-step-step; y += step)
	{
		for(int x = 0; x < w-step-step; x += step)
		{
			//if(kinect.getDistanceAt(x, y) > 0) { mesh.addIndex( ((y*step) * w) + (x*step) ); } // Points
			
			int tmpIndex			=        y * w + x;
			int tmpIndexRight		= tmpIndex + step;
			int tmpIndexBottom		= (y+step) * w + x;
			int tmpIndexBottomRight = tmpIndexBottom + step;
				
			// Triangle 1
			if( pixelIsValid[tmpIndex]		 &&
			    pixelIsValid[tmpIndexBottom] &&
			    pixelIsValid[tmpIndexRight] )
			{
				float tmpArea = triangleArea( vertices.at(tmpIndex), vertices.at(tmpIndexBottom), vertices.at(tmpIndexRight) );
				
				if( tmpArea < triangleMaxArea  )
				{
					mesh.addTriangle(tmpIndex, tmpIndexBottom, tmpIndexRight );
				}
			}

			// Triangle 2
			if( pixelIsValid[tmpIndexRight]	 &&
			    pixelIsValid[tmpIndexBottom] &&
			    pixelIsValid[tmpIndexBottomRight] )
			{
				float tmpArea = triangleArea( vertices.at(tmpIndexRight), vertices.at(tmpIndexBottom), vertices.at(tmpIndexBottomRight) );
				
				if( tmpArea < triangleMaxArea  )
				{
					mesh.addTriangle(tmpIndexRight, tmpIndexBottom, tmpIndexBottomRight );
				}
			}
		}
	}
		
	//glPointSize(3); // make the points bigger if we are drawing points
	ofPushMatrix();
		// the projected points are 'upside down' and 'backwards'
		ofScale(1, -1, -1);
		ofEnableDepthTest();
			kinect.getTextureReference().bind();
				mesh.draw();
			kinect.getTextureReference().unbind();
		ofDisableDepthTest();
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
float ofApp::triangleArea( ofVec3f _p0, ofVec3f _p1, ofVec3f _p2 )
{
	/*
	ofVec3f s = (_p0+_p1+_p2)*0.5f;
	float tmp = s.length()*(s-_p0).length()*(s-_p1).length()*(s-_p2).length();
	return sqrtf( tmp );
	 */
	
	float a = (_p1 - _p0).length();
	float b = (_p2 - _p1).length();
	float c = (_p2 - _p0).length();
	
	float s = (a+b+c)*0.5f;
	float tmpArea = sqrt(s*(s-a)*(s-b)*(s-c));
	return tmpArea;
}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
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

		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;

		case 'p':
			ofShowCursor();
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{}

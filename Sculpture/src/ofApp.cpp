#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofBackground(0);
	ofSetLogLevel( OF_LOG_VERBOSE );
	ofSetVerticalSync( true );

	meshFont.loadFont("Fonts/DIN.otf", 10, true, true, true, 0.0, 96 );
	
	//ofToggleFullscreen();
	
	showOverlay = false;
	predictive = true;
	
	oculusRift.baseCamera = &camera;
	oculusRift.setup();
	
	if( oculusRift.isSetup() )
	{
		ofHideCursor();
	}
	
	gridSize = 150.0f;
	
	// Setup some data for us to see
	for(int i = 0; i < 500; i++)
	{
		DemoSphere d;
		d.color = ofColor(ofRandom(255),
						  ofRandom(255),
						  ofRandom(255));
		
		d.pos = ofVec3f(ofRandom(-gridSize, gridSize),0,ofRandom(-gridSize,gridSize));
		
		d.myID = i;
		
		d.radius = 1.0f; //ofRandom(5.0, 10.0);
        d.mouseDist = 0.0f;
        d.isMouseOvered = false;
        
		demos.push_back(d);
	}
	
	//enable mouse;
	camera.begin();
	camera.end();
	camera.setGlobalPosition( 0, 100, 500 ); // let's start by stepping back a little bit and put ourselves a few units up from the floor
	
	lastMouse = ofVec2f( ofGetMouseX(), ofGetMouseY() );
	
	lineConnectionMaxDistanceMin = 20.0f;
	lineConnectionMaxDistanceMax = 80.0f;
	lineConnectionMaxDistance = 0.0f;
	mouseAffectionRadius = 300.0f;
	
	initializeSpacePartitioning();
}


//--------------------------------------------------------------
void ofApp::update()
{
	camera.setHeadsetOrientation( oculusRift.getOrientationQuat() );
	camera.update();
	
	// Update mouse values
	ofVec2f mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
	ofVec2f mouseVel = mouse - lastMouse;
	lastMouse = mouse;
	

	// Update scene
	float perlinTime = ofGetElapsedTimef()/50.0;
	float perlinSpaceDivider = 100.0f;
	for(int i = 0; i < demos.size(); i++)
	{
		float tmpDistFromCentre = ofMap( ofVec2f(demos[i].pos.x, demos[i].pos.z).length(), 0.0f, gridSize, 1.0f, 0.0f, true );
		
		float tmpNoise = ofSignedNoise( perlinTime * ofMap(tmpDistFromCentre, 0.0f, 1.0f, 0.9999999f, 1.0f),
									    demos[i].pos.x/perlinSpaceDivider,
									    demos[i].pos.z/perlinSpaceDivider );
		
		demos[i].pos.y = ofMap( tmpNoise, -1.0f, 1.0, 0.0f, 800.0f );
		float tmpFrac = ofMap(tmpDistFromCentre, 0.0f, 1.0f, 0.3f, 1.0f);
		tmpFrac = 1.0f - cosf( tmpFrac * (PI * 0.5f));
		demos[i].pos.y *= tmpFrac;
	}
    
	
	// Hit test against the mouse in Rift space
    if(oculusRift.isSetup())
	{
        for(int i = 0; i < demos.size(); i++)
		{
			demos[i].mouseDist = oculusRift.distanceFromMouse( demos[i].pos );
            demos[i].isMouseOvered = (demos[i].mouseDist < 50);
        }
    }
	else // or in normal camera space
	{
		for(int i = 0; i < demos.size(); i++)
		{
			demos[i].mouseDist = ofVec2f(ofGetMouseX(),ofGetMouseY()).distance( camera.worldToScreen( demos[i].pos ));
			demos[i].isMouseOvered = (demos[i].mouseDist < 50);
		}
	}
	
	updateLinesMesh();
}


//--------------------------------------------------------------
void ofApp::draw()
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
	else
	{
		camera.begin();
			drawScene();
		camera.end();
	}
	
}

//--------------------------------------------------------------
void ofApp::drawScene()
{
	ofEnableDepthTest();
		
		// Draw the floor
		ofSetColor(20, 20, 20);
		ofPushMatrix();
			ofRotate(90,	1, 0, 0);
			ofDrawPlane( 1600.0f, 1600.0f );
		ofPopMatrix();

		ofDisableDepthTest();
		ofSetColor(40, 40, 40);
		ofPushMatrix();
			ofRotate(90,	0, 0, -1);
			ofDrawGridPlane(800.0f, 10.0f, false );
		ofPopMatrix();
		ofEnableDepthTest();
	
		//ofDrawPlane( 1000.0f, 1000.0f );

		ofSetColor( ofColor::white );
		linesMesh.draw();
		
		// Draw the spheres
		for(int i = 0; i < demos.size(); i++)
		{
			ofPushMatrix();
				ofTranslate(demos[i].pos);
				ofSetColor(demos[i].isMouseOvered ? ofColor::white.getLerped(ofColor::red, sin(ofGetElapsedTimef()*10.0)*.5+.5) : demos[i].color); // pulse red to white when it's 'mouseovered'
				//ofSetColor(demos[i].color);
				ofSphere(demos[i].radius);
				//ofSphere(demos[i].isMouseOvered ? demos[i].radius * 10.0f: demos[i].radius ); // draw the sphere larger when it's 'mouseovered'
			ofPopMatrix();
		}
		
		// Draw the text and an indicator for the distance form the mouse we are using to reveal or hide the sculpture
	
		ofSetColor( ofColor::white );
		meshFont.drawStringAsShapes("Lines Max Connection Distance: " + ofToString(lineConnectionMaxDistance, 1), -450, 170 );
		ofPushMatrix();
			ofTranslate( ofVec3f(-450, 150, 0 ) );
			ofNoFill();
				ofRect( 0,0, 250, 10 );
			ofFill();
				ofRect( 0,0, 250 * ofMap( lineConnectionMaxDistance, lineConnectionMaxDistanceMin, lineConnectionMaxDistanceMax, 0.0f, 1.0f), 10 );
		ofPopMatrix();
	
		meshFont.drawStringAsShapes("Mouse Affection Radius: " + ofToString(mouseAffectionRadius, 1), -450, 130 );
	
	
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
void ofApp::updateLinesMesh()
{
	float currTime = ofGetElapsedTimef();
	
	updateSpacePartitioning();
	
	// Now we update the line mesh, to do this we check each particle against every other particle, if they are
	// within a certain distance we draw a line between them. As this quickly becoems a pretty insane amount
	// of checks, we use our space partitioning scheme to optimize it all a little bit.
	
	linesMesh.clear();
	linesMesh.setMode( OF_PRIMITIVE_LINES );
	
	ofFloatColor scratchColor;
	scratchColor.set( 1.0f, 1.0f, 1.0f );
	
	lineConnectionMaxDistance =  ofMap( cosf( currTime / 7.0f ) , -1.0f, 1.0f, lineConnectionMaxDistanceMin, lineConnectionMaxDistanceMax); //   ofGetMouseY() / 10.0f;
	
	// how many slots do we need to check on each side?
	int spacePartitioningIndexDistanceX = ceil(lineConnectionMaxDistance / spacePartitioningGridWidth);
	int spacePartitioningIndexDistanceY = ceil(lineConnectionMaxDistance / spacePartitioningGridHeight);
	
	for( unsigned int particleIndex = 0; particleIndex < demos.size(); particleIndex++ )
	{
		DemoSphere* tmpParticle = &demos.at(particleIndex);
		
		/*
		if( tmpParticle->isMouseOvered )
		{
			continue; // skip to the next particle in the loop
		}*/
		
		// the particle knows where it is in the space partitioning grid, figure out which indices to loop between based
		// on how many slots the maximum line distance  can cover, then do a bounds check.
		int startIndexX = tmpParticle->spacePartitioningIndexX - spacePartitioningIndexDistanceX;
		if( startIndexX < 0 ) { startIndexX = 0; } if( startIndexX >= spacePartitioningResX ) { startIndexX = spacePartitioningResX-1;}
		
		int endIndexX   = tmpParticle->spacePartitioningIndexX + spacePartitioningIndexDistanceX;
		if( endIndexX < 0 ) { endIndexX = 0; } if( endIndexX >= spacePartitioningResX ) { endIndexX = spacePartitioningResX-1;}
		
		int startIndexY = tmpParticle->spacePartitioningIndexY - spacePartitioningIndexDistanceY;
		if( startIndexY < 0 ) { startIndexY = 0; } if( startIndexY >= spacePartitioningResY ) { startIndexY = spacePartitioningResY-1;}
		
		int endIndexY   = tmpParticle->spacePartitioningIndexY + spacePartitioningIndexDistanceY;
		if( endIndexY < 0 ) { endIndexY = 0; } if( endIndexY >= spacePartitioningResY ) { endIndexY = spacePartitioningResY-1;}
		
		for( int y = startIndexY; y < endIndexY; y++ )
		{
			for( int x = startIndexX; x < endIndexX; x++ )
			{
				for( unsigned int i = 0; i < spacePartitioningGrid.at(y).at(x).size(); i++ )
				{
					DemoSphere* tmpOtherParticle = spacePartitioningGrid.at(y).at(x).at(i);
									
					if( tmpOtherParticle->isMouseOvered )
					{
						continue; // skip to the next particle in the loop
					}
					
					if( tmpParticle->myID != tmpOtherParticle->myID )
					{
						ofVec3f diff = tmpParticle->pos - tmpOtherParticle->pos;
						float diffLength = diff.length();
						
						float tmpLineConnectionMaxDistance = lineConnectionMaxDistance;
						
						// lets make the allowed maximum distance smaller the closer we are to the mouse
						
//						tmpLineConnectionMaxDistance *= ofMap( demos[i].mouseDist, mouseAffectionRadius, 0.0f, 0.0f, 1.0f, true ); // the mouse reveals the mesh
						tmpLineConnectionMaxDistance *= ofMap( demos[i].mouseDist, mouseAffectionRadius, 0.0f, 1.0f, 0.0f, true ); // mouse takes away the mesh
						
						if( diffLength < tmpLineConnectionMaxDistance )
						{
							scratchColor.a =  1.0f - (diffLength / tmpLineConnectionMaxDistance);
							
							linesMesh.addVertex( tmpParticle->pos );
							//linesMesh.addColor( scratchColor );
							linesMesh.addColor( ofColor( tmpParticle->color, (int)(scratchColor.a * 255)) );
							
							linesMesh.addVertex( tmpOtherParticle->pos );
							//linesMesh.addColor( scratchColor );
							linesMesh.addColor( ofColor( tmpOtherParticle->color, (int)(scratchColor.a * 255)) );
							
							linesMesh.addIndex( linesMesh.getNumVertices() - 2 );
							linesMesh.addIndex( linesMesh.getNumVertices() - 1 );
						}
					}
				}
			}
		}
	}
	
}


//--------------------------------------------------------------
void ofApp::updateSpacePartitioning()
{
	// clear the space partitioning lists
	for( int y = 0; y < spacePartitioningResY; y++ )
	{
		for( int x = 0; x < spacePartitioningResX; x++ )
		{
			spacePartitioningGrid.at(y).at(x).clear();
		}
	}
	
	// add particles into the space partitioning grid
	for(int i = 0; i < demos.size(); i++)
	{
		DemoSphere* tmpSphere = &demos.at(i);
		
		int tmpIndexX = tmpSphere->pos.x / spacePartitioningGridWidth;
		int tmpIndexY = tmpSphere->pos.z / spacePartitioningGridHeight;
		
		if( tmpIndexX < 0 )  tmpIndexX = 0;
		if( tmpIndexX >= spacePartitioningResX ) tmpIndexX = spacePartitioningResX-1;
		
		if( tmpIndexY < 0 )  tmpIndexY = 0;
		if( tmpIndexY >= spacePartitioningResY ) tmpIndexY = spacePartitioningResY-1;
		
		tmpSphere->spacePartitioningIndexX = tmpIndexX;
		tmpSphere->spacePartitioningIndexY = tmpIndexY;
		
		spacePartitioningGrid.at(tmpIndexY).at(tmpIndexX).push_back( tmpSphere );
	}
	
}

//--------------------------------------------------------------
void ofApp::initializeSpacePartitioning()
{
	// Initialize storage we will use to optimize particle-to-particle distance checks
	spacePartitioningResX = 30;
	spacePartitioningResY = 30;
	
	spacePartitioningGridWidth = gridSize * 2.0f;
	spacePartitioningGridHeight= gridSize * 2.0f;
	
	for( int y = 0; y < spacePartitioningResY; y++ )
	{
		spacePartitioningGrid.push_back( vector< vector< DemoSphere* > >() );
		for( int x = 0; x < spacePartitioningResX; x++ )
		{
			spacePartitioningGrid.at(y).push_back( vector< DemoSphere* >() );
		}
	}
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
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

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
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

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}

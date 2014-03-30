#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetLogLevel( OF_LOG_VERBOSE );
		
	ofSetSmoothLighting(true);
	
	fontSmall.loadFont("Fonts/DIN.otf", 8);

	ofxGuiSetDefaultWidth(300);

	string settingsPath = "Settings/WorldSettings.xml";
	gui.setup("Main", settingsPath );
	
	gui.add( doGravity.set("Gravity", true ) );
	
	gui.add( dimension.set("Dimension", ofVec3f(5500,800,5500),		ofVec3f(100,0,100), ofVec3f(8000,2000,8000) ) );
	gui.add( heightmapRes.set("Heightmap Res", 190,		10, 500 ) );
	
	gui.add( grassLayingMethod.set("Gras Laying Method", 0,		0, 3 ) );
	gui.add( grassPlacementMaxHeight.set("Placement Max Height", 1.0,	-1.0f, 2.0f ) );
	gui.add( grassPlacementNoiseSpaceDiv.set("Placement Noise Space Div", 680,	1.0f, 4000.0f ) );
	gui.add( grassNoisePlacementThreshold.set("Placement Noise Threshold", 0.8,	0.0f, 1.0f ) );

	gui.add( grassSwayingTimeScale.set("Swaying Time Scale", 2.0,	0.01f, 40.0f ) );
	gui.add( grassSwayingNoiseSpaceDiv.set("Swaying Noise Space Div", 60.0,	0.001f, 2000.0f ) );
	gui.add( grassSwayingNoiseMag.set("Swaying Noise Mag", 1.0,	0.1f, 10.0f ) );
	
	gui.add( swayingMaxAngle.set("swayingMaxAngle", 0.04f,	0.001f, 1.0f ) );
	gui.add( stalkWidth.set("stalkWidth", 60.0f,	0.01f, 100.0f ) );
	gui.add( stalkHeight.set("stalkHeight", 260.0f,	0.01f, 400.0f ) );

	gui.add( flyerTimeScale.set("Flyer Time Scale", 0.18f, 0.0f, 4.0f ) );
	
	gui.add( flyerTailLength.set("Flyer Tail Length", 5,	2, 20 ) );
	gui.add( flyerTailStepTimeOffset.set("Flyer Tail Time", 0.02f,	0.001f, 0.2f ) );

	gui.add( flyerSwarmOffsetNoiseSpaceDiv.set("Flyer Swarm Offset Noise Spave Div", 5.0f,	0.01f, 500.0f ) );

	gui.add( flyerSwarmOffsetMagnitude.set("Flyer Swarm Offset Magnitude", 0.1f,	0.0f, 3.0f ) );

	gui.add( flyerMinHeight.set("Flyer Min Height", 400.0f,	0.0f, 1000.0f ) );
	gui.add( flyerTimeVariation.set("Flyer Time Variation", 0.4f,	0.0f, 5.0f ) );
	
	gui.add( flyerPlacementMaxHeight.set("Flyer Placement Max Height", 0.8,	0.0f, 1.0f ) );
	gui.add( flyerPlacementNoiseSpaceDiv.set("Flyer Placement Noise Space Div", 100.0,	0.1f, 8000.0f ) );
	gui.add( flyerPlacementNoiseThreshold.set("Flyer Placement Noise Thresh", 0.2,	0.0f, 1.0f ) );

	gui.add( flyerMovementNoiseSpaceDivider.set("Flyer Movement Space Divider", 850.0f,	0.01f, 2000.0f ) );
	gui.add( flyerMovementMagnitude.set("Flyer Movement Mag", ofVec3f(200,100,200),	ofVec3f(0,0,0), ofVec3f(2000,1000,2000) ) );
	
	gui.loadFromFile( settingsPath );
	
	showGui = true;
	
	grassShader.setGeometryInputType(GL_LINES);
	//grassShader.setGeometryOutputType(GL_LINE_STRIP );
	grassShader.setGeometryOutputType(GL_TRIANGLE_STRIP );
	grassShader.setGeometryOutputCount(10);
    grassShader.load("Shaders/Grass.vert", "Shaders/Grass.frag", "Shaders/Grass.geom");
	
	//flyersShader.setGeometryInputType(GL_LINES);
	flyersShader.setGeometryInputType(GL_LINES_ADJACENCY);
	//flyersShader.setGeometryOutputType(GL_LINES );
	flyersShader.setGeometryOutputType(GL_TRIANGLE_STRIP );
	//flyersShader.setGeometryOutputType(GL_TRIANGLE_STRIP );
	flyersShader.setGeometryOutputCount(10);
    flyersShader.load("Shaders/Flyers.vert", "Shaders/Flyers.frag", "Shaders/Flyers.geom");
	
	streakTexture.loadImage("Textures/StreakRound.png");
	
	
	godCamera.setNearClip(1.0f);
	godCamera.setFarClip(4096.0f);
	godCamera.setAutoDistance( false );
	godCamera.orbit( 100, -20, 500, ofVec3f(0,600,0) );
//	godCamera.setPosition(ofVec3f(0,600,0));
	
	walkingCamera.setEyeHeight(200.0f);
	
//	isWalking = false;
//	camera = &godCamera;
	
	isWalking = true;
	camera = &walkingCamera;
	
	noisePositionX = 0;
	noisePositionZ = 0;
	
}

//--------------------------------------------------------------
void ofApp::update()
{

	// lets allow teleporting to see what the world will look like faraway without haing to walk all the way over there
	float teleportSpeed = 3000.0f;
	ofVec3f teleportOffset(0,0,0);
	if( ofGetKeyPressed( OF_KEY_UP ) )		{ teleportOffset.x += teleportSpeed; }
	if( ofGetKeyPressed( OF_KEY_DOWN ) )	{ teleportOffset.x -= teleportSpeed; }
	if( ofGetKeyPressed( OF_KEY_LEFT ) )	{ teleportOffset.z += teleportSpeed; }
	if( ofGetKeyPressed( OF_KEY_RIGHT ) )	{ teleportOffset.z -= teleportSpeed; }
	if( teleportOffset.length() > 0.0f )
	{
		walkingCamera.setPosition( walkingCamera.getPosition() + teleportOffset );
		walkingCamera.update();
		
		godCamera.setPosition( walkingCamera.getPosition() );
	}
	
	
	if( doGravity ) { walkingCamera.setGravity( -12.0f ); }
	else { walkingCamera.setGravity( 0.0f ); }
		
	walkingCamera.setFarClip( max(dimension.get().x, dimension.get().z) );
	
#if defined( USE_OCULUS_RIFT )
	camera.setHeadsetOrientation( oculusRift.getOrientationQuat() );
#endif
	walkingCamera.update();
	
	setTerrainCentrePosition( ofVec3f(walkingCamera.getPosition().x, 0, walkingCamera.getPosition().z) );
	walkingCamera.setGroundLevelY( getTerrainHeightAtWorldSpaceCoordinate( camera->getPosition().x, camera->getPosition().z ) );
	
	
	// all these generator functions are perfect for parallelising
	generateTerrainMesh();
	generateGrassMesh();
	generateFlyersMesh();
}

//--------------------------------------------------------------
void ofApp::setTerrainCentrePosition( ofVec3f _position )
{
	float stepX = dimension.get().x / (float)(heightmapRes - 1);
	float stepZ = dimension.get().z / (float)(heightmapRes - 1);

	int tmpNextPosX = _position.x / stepX;
	int tmpNextPosZ = _position.z / stepZ;

	int tmpOffsetX = (noisePositionX - tmpNextPosX);
	int tmpOffsetZ = (noisePositionZ - tmpNextPosZ);

	noisePositionX -= tmpOffsetX;
	noisePositionZ -= tmpOffsetZ;
	
	terrainPosition = _position;

	// round the position that we get in to a multiple of the distance between vertices, this cuts down on visual glitches as we move
	terrainMeshRenderingPosition = terrainPosition;
	terrainMeshRenderingPosition.x -= fmod( terrainMeshRenderingPosition.x, stepX );
	terrainMeshRenderingPosition.z -= fmod( terrainMeshRenderingPosition.z, stepZ );
}


//--------------------------------------------------------------
void ofApp::draw()
{
	drawScene( camera );
	
	ofSetColor( ofColor::white );
	string camName = "(g)"; if( isWalking ) { camName = "(w)"; }
	
	fontSmall.drawString( "fps: " + ofToString( ofGetFrameRate(), 2) + " " + camName,  10, ofGetHeight() - 10 );
	
	if( showGui )
	{
		gui.draw();
	}
}


//--------------------------------------------------------------
void ofApp::drawScene( ofCamera* _cam )
{
	ofBackgroundGradient( ofColor(50,50,50), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
	
	ofLight tmpLight;
	tmpLight.setPointLight();
	
	ofEnableLighting();
	ofEnableDepthTest();
	_cam->begin();
		
		//ofTranslate( ofVec3f(0,-300,0) );

		ofPushMatrix();
			tmpLight.setPosition( ofVec3f(0,1000,0) );
			tmpLight.enable();
		
			//ofSetColor( ofColor::grey );
			ofFloatColor groundColor;
			groundColor.setHsb(fmodf( ofNoise(noiseStartPos.x / 50000.0f, noiseStartPos.y / 50000.0f) + 0.08f, 1.0f), 0.3, 0.5f );
			ofSetColor( groundColor );
	
			ofPushMatrix();
				//ofTranslate( terrainMeshRenderingPosition );
				terrainMesh.draw();
				if( debugDrawNormals )
				{
					ofSetColor( ofColor::blue );
					debugNormalsMesh.draw();
				}
			ofPopMatrix();
			tmpLight.draw();
		ofPopMatrix();
		
		ofPushMatrix();
	
			//ofTranslate( ofVec3f(0,30,0) );
	
			grassShader.begin();
		
				grassShader.setUniform1f("timeSecs", ofGetElapsedTimef() );
	
				grassShader.setUniform3f( "cameraWorldPos", _cam->getGlobalPosition().x, _cam->getGlobalPosition().y, _cam->getGlobalPosition().z );
				//grassShader.setUniform3f( "cameraWorldPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z );

				grassShader.setUniformMatrix4f( "cameraMatrix", _cam->getGlobalTransformMatrix() );
				grassShader.setUniformMatrix4f( "cameraInverseMatrix", _cam->getGlobalTransformMatrix().getInverse() );

				grassShader.setUniform1f("stalkHalfWidth", stalkWidth / 2.0f );
				grassShader.setUniform1f("stalkHeight", stalkHeight );

				grassShader.setUniform1f("stalkSwayingMaxAngle", swayingMaxAngle );

				grassShader.setUniform1f("grassSwayingTimeScale", grassSwayingTimeScale );
				grassShader.setUniform1f("grassSwayingNoiseSpaceDiv", grassSwayingNoiseSpaceDiv );
				grassShader.setUniform1f("grassSwayingNoiseMag", grassSwayingNoiseMag );
	
				grassShader.setUniform3f("noiseGenerationPos", noiseStartPos.x, noiseStartPos.y, noiseStartPos.z );
	
				// set thickness of ribbons
				//grassShader.setUniform1f("thickness", 20);
				
				// make light direction slowly rotate
				//grassShader.setUniform3f("lightDir", sin(ofGetElapsedTimef()/10), cos(ofGetElapsedTimef()/10), 0);

				//mesh.draw();
				//for(unsigned int i=1; i<points.size(); i++) {
				//	ofLine(points[i-1], points[i]);
				//}
	
				//ofMesh tmpMesh;
				//tmpMesh.setMode( OF_PRIMITIVE_POINTS );
				//for(unsigned int i=0; i<points.size(); i++) { tmpMesh.addVertex( points.at(i) ); }
				//tmpMesh.draw();
		
				ofFloatColor grassColor;
				grassColor.setHsb( 	ofNoise(noiseStartPos.x / 50000.0f, noiseStartPos.y / 50000.0f), 0.7, 0.3f );
	
				ofSetColor( grassColor );
	
				grassMesh.draw();
	
			grassShader.end();
	
			//grassMesh.draw();
	
			ofSetColor( ofColor::white );
	
			flyersShader.begin();
			
				flyersShader.setUniform3f( "cameraWorldPos", _cam->getGlobalPosition().x, _cam->getGlobalPosition().y, _cam->getGlobalPosition().z );
				//flyersShader.setUniform3f( "cameraWorldPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z );
			
				flyersShader.setUniformMatrix4f( "cameraMatrix", _cam->getModelViewMatrix() );
				flyersShader.setUniformMatrix4f( "cameraInverseMatrix", ofMatrix4x4::getInverseOf(_cam->getModelViewMatrix()) );
		
				flyersShader.setUniformMatrix4f( "cameraPerspectiveMatrix", _cam->getProjectionMatrix().getPtr() );
	
				flyersMesh.draw();
	
			flyersShader.end();

			//flyersMesh.drawVertices();
	
		ofPopMatrix();
	
	_cam->end();
	
	ofDisableDepthTest();
	ofDisableLighting();
	
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	if( key == 'r' )
	{
		grassShader.load("Shaders/Grass.vert", "Shaders/Grass.frag", "Shaders/Grass.geom");
	}
	else if( key == OF_KEY_TAB )
	{
		showGui = !showGui;
	}
	else if( key == 'c' )
	{
		isWalking = !isWalking;
		if( isWalking )
		{
			walkingCamera.enableAutoUpdate();
			camera = &walkingCamera;
		}
		else
		{
			walkingCamera.disableAutoUpdate();
			//godCamera.resetTransform();
			//godCamera.lookAt( walkingCamera.getPosition() );
			godCamera.setPosition( walkingCamera.getPosition() );
			godCamera.orbit( 100, -20, 100 );
			camera = &godCamera;
		}
	}
	else if( key == 'f' )
	{
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::generateTerrainMesh()
{
	if( heightmap.size() != heightmapRes*heightmapRes )
	{
		heightmap.resize(heightmapRes*heightmapRes);
		for( int i = 0; i < heightmap.size(); i++ ) heightmap[i] = 0;
	}
	
	float currTime = ofGetElapsedTimef();
	
	int res = heightmapRes;
	ofVec3f step = dimension.get() / (float)(res-1);
	
	//ofVec3f startPos( noisePositionX * step.x, 0 , noisePositionZ * step.z);
	ofVec3f startPos( noisePositionX * step.x, 0 , noisePositionZ * step.z);
	
	
	// Update heightmap
	
	float perlinSpaceDivider = 8000.0f;
	
	int octaves			= 5;
	
	int tmpIndex = 0;
	for( int indexZ = 0; indexZ < res; indexZ++ )
	{
		for( int indexX = 0; indexX < res; indexX++ )
		{
			float frequency		= 0.7f;
			float persistence	= 0.8f;
			float amplitude		= 0.6f;
			
			//ofVec2f perlinLookup( terrainMeshRenderingPosition.x + (indexX * step.x), terrainMeshRenderingPosition.z + (indexZ * step.z) );
			ofVec2f perlinLookup( startPos.x + (indexX * step.x), startPos.z + (indexZ * step.z) );
			perlinLookup /= perlinSpaceDivider;
			
			float tmpHeight = 0.0f;
			for(int k = 0; k < octaves; k++)
			{
				tmpHeight += ofNoise(perlinLookup.x, perlinLookup.y) * amplitude;
				amplitude *= persistence;
				frequency *= 2.0f;
			}
			
			heightmap[tmpIndex] = tmpHeight;
			tmpIndex++;
		}
	}
	
	// Fill mesh with terrain vertices
	
	terrainMesh.clear();
	terrainMesh.setMode( OF_PRIMITIVE_TRIANGLES );
	
	
	//float startX = dimension.x * -0.5f;
	//float startZ = dimension.z * -0.5f;
	
	ofVec3f meshStartPos = terrainMeshRenderingPosition + (dimension.get() * -0.5f);
	//ofVec3f meshStartPos = (dimension.get() * -0.5f);
	
	tmpIndex = 0;
	for( int indexZ = 0; indexZ < res; indexZ++ )
	{
		for( int indexX = 0; indexX < res; indexX++ )
		{
			/*
			 ofVec3f tmpPos( startPos.x + (indexX * step.x),
			 heightmap[tmpIndex] * dimension.get().y,
			 startPos.z + (indexZ * step.z) );
			 */
			ofVec3f tmpPos( meshStartPos.x + (indexX * step.x),
						   heightmap[tmpIndex] * dimension.get().y,
						   meshStartPos.z + (indexZ * step.z) );
			
			terrainMesh.addVertex( tmpPos );
			terrainMesh.addNormal( ofVec3f(0,0,0) );
			tmpIndex++;
		}
	}
	
	// Compute normals
	
	vector<ofVec3f>& normals = terrainMesh.getNormals();
	
	float fracX = 0.0f;
	float fracY = 0.0f;
	
	for( int indexY = 1; indexY < res-1; indexY++ )
	{
		int tmpIndex = indexY * res;
		
		for( int indexX = 1; indexX < res-1; indexX++ )
		{
			float scale = 0.1f;
			float z0 = getHeightmapValue( indexX, indexY ); //paV[ 0 ].m_Z;
			
			float Az = ( indexX + 1 < res	) ? ( getHeightmapValue( indexX + 1, 	indexY ) )		: z0;
			float Bz = ( indexY + 1 < res	) ? ( getHeightmapValue( indexX, 		indexY + 1 ) )	: z0;
			float Cz = ( indexX - 1 >= 0			) ? ( getHeightmapValue( indexX - 1, 	indexY ) )		: z0;
			float Dz = ( indexY - 1 >= 0			) ? ( getHeightmapValue( indexX, 		indexY - 1)  )	: z0;
			
			normals[tmpIndex] = ofVec3f( Cz - Az, 2.f * scale, Dz - Bz ).getNormalized();
			
			tmpIndex++;
		}
	}
	
	// Add indices to draw triangles with our mesh
	
	for( int indexY = 0; indexY < res-1; indexY++ )
	{
		int tmpIndex = indexY * res;
		
		for( int indexX = 0; indexX < res-1; indexX++ )
		{
			terrainMesh.addTriangle( tmpIndex, tmpIndex + 1, tmpIndex + res + 1 );
			terrainMesh.addTriangle( tmpIndex, tmpIndex + res + 1, tmpIndex + res );
			
			tmpIndex++;
		}
	}
	
	// If we want to have a look at the normals
	
	debugDrawNormals = false;
	
	if( debugDrawNormals )
	{
		debugNormalsMesh.clear();
		debugNormalsMesh.setMode( OF_PRIMITIVE_LINES );
		for( unsigned int i = 0; i < terrainMesh.getNumNormals(); i++ )
		{
			ofVec3f tmpPos = terrainMesh.getVertex(i);
			debugNormalsMesh.addVertex( tmpPos );
			debugNormalsMesh.addVertex( tmpPos + (terrainMesh.getNormal(i) * 10.0f));
		}
	}
	
}

/*
 
 We do some random height calculations, etc in the geometry shader, this is based on the position of the vertex
 The vertex is coming into the geometry shader in it's non quantized position?
 This makes any calculation we base on that change as we move.
 
 Or something along those lines.
 
 */


//--------------------------------------------------------------
void ofApp::generateGrassMesh()
{
	// populate the grass mesh
	grassMesh.clear();
	grassMesh.setMode( OF_PRIMITIVE_LINES );
	
	ofVec3f* terrainVertices = terrainMesh.getVerticesPointer();
	ofVec3f* terrainNormals = terrainMesh.getNormalsPointer();
	
	if( grassLayingMethod == 0 )
	{
		for( int i = 0; i < terrainMesh.getNumVertices(); i++ )
		{
			//ofVec3f noisePos = /*noiseStartPos +*/ terrainVertices[i];
			//ofVec3f tmpVertexPos = terrainMeshRenderingPosition + terrainVertices[i];
			ofVec3f tmpVertexPos = terrainVertices[i];
			//if( (i % heightmapRes) == 0 ) cout << tmpVertexPos << endl;
			
			float stalkHeightFraction = 1.0f;
			
			float noise = ofNoise( tmpVertexPos.x / grassPlacementNoiseSpaceDiv, tmpVertexPos.z / grassPlacementNoiseSpaceDiv );
			
			//noise *= ofMap( terrainVertices[i].y, dimension.get().y * 0.3f, dimension.get().y * 0.7f, 1.0f, 0.0f, true ); // this fades it out toward the higher edge
			
			// this looks great even at grassNoisePlacementThreshold = 1.0 as we still get a nice sweeping variation in height
			if( noise < grassNoisePlacementThreshold && tmpVertexPos.y < dimension.get().y * grassPlacementMaxHeight ) { // stops grass growing high, but grass stops suddenly there
				
				stalkHeightFraction = ofMap( noise, 0.0f, grassNoisePlacementThreshold,  1.0f, 0.1f, true );//noise / 0.5f;
				stalkHeightFraction *= ofMap( tmpVertexPos.y, dimension.get().y * (grassPlacementMaxHeight-0.2f), dimension.get().y * (grassPlacementMaxHeight),  1.0f, 0.1f, true );
				
				grassMesh.addVertex( tmpVertexPos );
				grassMesh.addVertex( tmpVertexPos + (terrainNormals[i] * stalkHeightFraction) ); // we normalize in the shader
			}
		}
		
	}
	if( grassLayingMethod == 1 )
	{
		for( int i = 0; i < terrainMesh.getNumVertices(); i++ )
		{
			//ofVec3f noisePos = /*noiseStartPos +*/ terrainVertices[i];
			ofVec3f tmpVertexPos = terrainMeshRenderingPosition + terrainVertices[i];
			
			float stalkHeightFraction = 1.0f;
			
			float noise = ofNoise( tmpVertexPos.x / grassPlacementNoiseSpaceDiv, tmpVertexPos.z / grassPlacementNoiseSpaceDiv );
			
			noise *= ofMap( tmpVertexPos.y, dimension.get().y * 0.3f, dimension.get().y * 0.7f, 1.0f, 0.0f, true ); // this fades it out toward the higher edge
			
			if( noise > grassNoisePlacementThreshold  ) {
				//if( noise < 0.5f && terrainVertices[i].y < dimension.get().y * 0.9f ) { // stops grass growing high, but grass stops suddenly there
				stalkHeightFraction = ofMap( noise, grassNoisePlacementThreshold, 1.0f,  0.1f, 1.0f, true );//noise / 0.5f;
				
				grassMesh.addVertex( tmpVertexPos );
				grassMesh.addVertex( tmpVertexPos + (terrainNormals[i] * stalkHeightFraction) ); // we normalize in the shader
			}
		}
	}
	else if( grassLayingMethod == 2 )
	{
		for( int i = 0; i < terrainMesh.getNumVertices(); i++ )
		{
			ofVec3f tmpVertexPos = terrainMeshRenderingPosition + terrainVertices[i];
			
			float stalkHeightFraction = 1.0f;
			if( tmpVertexPos.y < dimension.get().y * grassPlacementMaxHeight ) // looks like planted grass spots
			{
				stalkHeightFraction = ofMap( tmpVertexPos.y, dimension.get().y * (grassPlacementMaxHeight-0.2f), dimension.get().y * (grassPlacementMaxHeight),  1.0f, 0.1f, true );//noise / 0.5f;
				
				grassMesh.addVertex( tmpVertexPos );
				grassMesh.addVertex( tmpVertexPos + (terrainNormals[i] * stalkHeightFraction) ); // we normalize in the shader
			}
		}
	}
	else if( grassLayingMethod == 3 )
	{
		for( int i = 0; i < terrainMesh.getNumVertices(); i++ )
		{
			ofVec3f tmpVertexPos = terrainMeshRenderingPosition + terrainVertices[i];
			
			float stalkHeightFraction = 1.0f;
			if( terrainVertices[i].y > dimension.get().y * grassPlacementMaxHeight ) // mountains stick out
			{
				stalkHeightFraction = ofMap( terrainVertices[i].y, dimension.get().y * (grassPlacementMaxHeight), dimension.get().y * (grassPlacementMaxHeight+0.2f),  0.1f, 1.0f, true );//noise / 0.5f;
				
				grassMesh.addVertex( tmpVertexPos );
				grassMesh.addVertex( tmpVertexPos + (terrainNormals[i] * stalkHeightFraction) ); // we normalize in the shader
			}
		}
	}
	
}

//--------------------------------------------------------------
void ofApp::generateFlyersMesh()
{
	bool doLineAdjacency = true;
	
	flyersMesh.clear();
	
	if( doLineAdjacency )	{ flyersMesh.setOverrideGLDrawMode( GL_LINES_ADJACENCY ); }
	else					{ flyersMesh.setMode( OF_PRIMITIVE_LINES ); }
	
	float flyerTimeVariationHalf = flyerTimeVariation * 0.5f;
	
	ofVec3f* terrainVertices = terrainMesh.getVerticesPointer();
	ofVec3f* terrainNormals = terrainMesh.getNormalsPointer();
	
	float movementNoiseTime = ofGetElapsedTimef() * flyerTimeScale;
	float movementTailTimeStep = flyerTailStepTimeOffset;
	
	vector<ofFloatColor> colorLookup;
	vector<ofVec2f> tailTexCoordLookup;
	
	for( int i = 0; i < flyerTailLength; i++ )
	{
		float frac = 0.0f;
		if( doLineAdjacency)	{ frac = ofMap( i, 1, flyerTailLength-2, 0.0f, 1.0f, true ); }
		else					{ frac = i / (float)(flyerTailLength-1); }
		
		colorLookup.push_back( ofFloatColor(ofFloatColor::white,frac) );
		tailTexCoordLookup.push_back( ofVec2f(frac,0.0f) );
	}
	
	vector< ofVec3f > tailPosLookup;
	vector< ofFloatColor > tailColorLookup;
	
	for( int i = 0; i < terrainMesh.getNumVertices(); i++ )
	{
		//ofVec3f noisePos = noiseStartPos + terrainVertices[i];
		ofVec3f vertexPos = terrainVertices[i];
		
		ofVec2f placementNoisePos = ofVec2f(vertexPos.x,vertexPos.z) / flyerPlacementNoiseSpaceDiv;
		
		float noise = ofNoise( placementNoisePos.x, placementNoisePos.y );
		
		if( noise < flyerPlacementNoiseThreshold &&
		   terrainVertices[i].y < dimension.get().y * flyerPlacementMaxHeight )
		{
			ofVec3f movementNoisePos = vertexPos / (flyerMovementNoiseSpaceDivider);
			movementNoisePos *= ofMap( ofNoise(movementNoisePos.z, movementNoisePos.x), 0.0f, 1.0f,  0.9f, 1.1f );
			
			ofVec3f tmpFlyerOffsetNoisePos = vertexPos / flyerSwarmOffsetNoiseSpaceDiv;
//tmpFlyerOffsetNoisePos = ofVec3f(0,0,0);
			
			ofVec3f* fp = &tmpFlyerOffsetNoisePos;
			float ft = movementNoiseTime + ( ofSignedNoise( fp->x, fp->z ) * flyerTimeVariationHalf );
			//ft *= ofMap( ofSignedNoise( fp->z, fp->x ), -1.0f, 1.0f,		0.8f, 1.2f );
//ft = 0.0f;
			// each flyer gets an offset within the swarm
			ofVec3f flyerCentrePos = terrainVertices[i];
			
			flyerCentrePos.y += flyerMinHeight;
	

			ofVec3f offsetFromCentre = ofVec3f( ofSignedNoise( fp->x, fp->z ) + ofSignedNoise( fp->y * 10.1f, fp->x * 10.1f ),
											    ofSignedNoise( fp->y, fp->z ) + ofSignedNoise( fp->z * 10.1f, fp->x * 10.1f ) ,
											    ofSignedNoise( fp->z, fp->x ) + ofSignedNoise( fp->y ) );

/*			ofVec3f offsetFromCentre = ofVec3f( ofSignedNoise( fp->x, fp->z ) ,
												ofSignedNoise( fp->y, fp->z ) ,
												ofSignedNoise( fp->z, fp->x )  );
*/
			offsetFromCentre *= flyerSwarmOffsetMagnitude;
			
			movementNoisePos += offsetFromCentre;
			
			ofVec3f tmpMag = ofVec3f( ofMap( ofSignedNoise(movementNoisePos.x,movementNoisePos.z), -1.0f, 1.0, 0.9f, 1.1f ),
									  ofMap( ofSignedNoise(movementNoisePos.z,movementNoisePos.x), -1.0f, 1.0, 0.9f, 1.1f ),
									  ofMap( ofSignedNoise(movementNoisePos.y),				       -1.0f, 1.0, 0.9f, 1.1f ) );
			
			ofVec3f flyerMagnitude = flyerMovementMagnitude.get();// * tmpMag;
			
			//flyerMagnitude *= ofMap( ofSignedNoise(movementNoisePos.x,movementNoisePos.z), -1.0f, 1.0, 0.9f, 1.1f );
			
			float tailVertexTime = ft; //movementNoiseTime;
			
			//ofFloatColor grassColor;
			//grassColor.setHsb( 	ofNoise(noiseStartPos.x / 50000.0f, noiseStartPos.y / 50000.0f), 0.7, 0.3f );
			
			ofFloatColor flyerColor = ofFloatColor::fromHsb( fmodf( ofNoise(vertexPos.x / 50000.0f, vertexPos.z / 50000.0f) + 0.5f, 1.0), 0.7, 0.5f );
			
			float tailLengthMinusOnef = flyerTailLength-1;
			
			tailPosLookup.clear();
			tailColorLookup.clear();
			
			for( int j = 0; j < flyerTailLength; j++ )
			{
				float tailFrac = j / tailLengthMinusOnef;
				
				ofVec3f* p = &movementNoisePos;
				
				
				ofVec3f tailVertexOffset( ofSignedNoise( p->x, p->z,		tailVertexTime ),
										  ofSignedNoise( p->z, p->x,		tailVertexTime ),
										  ofSignedNoise( tailVertexTime,    p->z, p->x ) );
				
				tailVertexOffset *= flyerMagnitude;
				tailVertexOffset += ofVec3f(0,flyerMovementMagnitude.get().y * 1.05f,0);
				
				//flyersMesh.addVertex( terrainVertices[i] + tmpOffset );
				//flyersMesh.addTexCoord( texCoordLookup.at(j) );
				//flyersMesh.addColor( colorLookup.at(j) );			// we could also generate new colours for each flyer
				
				// make the tail fade? or do that with texture
				flyerColor.a = tailFrac;
				//flyersMesh.addColor( flyerColor );
				
				tailPosLookup.push_back( flyerCentrePos + tailVertexOffset );
				tailColorLookup.push_back( flyerColor );
				
				//if( j > 0 )
				//{
				//	flyersMesh.addIndex( flyersMesh.getNumVertices() - 2);
				//	flyersMesh.addIndex( flyersMesh.getNumVertices() - 1 );
				//}
				
				tailVertexTime += movementTailTimeStep;
			}
			
			for( int j = 0; j < flyerTailLength; j++ )
			{
				flyersMesh.addVertex( tailPosLookup.at(j) );
				flyersMesh.addTexCoord( tailTexCoordLookup.at(j) );
				flyersMesh.addColor( tailColorLookup.at(j) );
				
				if( doLineAdjacency )
				{
					// makes GL_LINES_ADJACENCY
					if( j > 2 )
					{
						int tmpVertexAmount = flyersMesh.getNumVertices();
						flyersMesh.addIndex( tmpVertexAmount - 4 );
						flyersMesh.addIndex( tmpVertexAmount - 3 );
						flyersMesh.addIndex( tmpVertexAmount - 2 );
						flyersMesh.addIndex( tmpVertexAmount - 1 );
					}
				}
				else
				{
					// makes GL_LINES
					if( j > 0 )
					{
						flyersMesh.addIndex( flyersMesh.getNumVertices() - 2);
						flyersMesh.addIndex( flyersMesh.getNumVertices() - 1 );
					}
				}
			}

			
		
/*
			flyersMesh.clear();
			flyersMesh.setMode( OF_PRIMITIVE_POINTS );
						
			float lineHeightHalf = 10.0f / 2.0f;
			
			for( int j = 0; j < flyerTailLength-1; j++ )
			{
				ofVec3f p0 = tailPosLookup.at(j);
				ofVec3f p1 = tailPosLookup.at(j+1);
				
				ofVec3f toEye = camera->getPosition() - p0;
				ofVec3f forward = p0 - p1;
				
				ofVec3f up = toEye.getCrossed( forward );
				up.normalize();
				
				up *= lineHeightHalf;
				
				flyersMesh.addVertex( p0 );
				//flyersMesh.addVertex( ofVec3f( ofRandom(-200, 200), ofRandom(-0, 2000), ofRandom(-200, 200) ) );
				flyersMesh.addColor( ofFloatColor::white );
				//flyersMesh.addTexCoord( tailTexCoordLookup.at(j) );
				
//				flyersMesh.addVertex( p0 + up );
//				flyersMesh.addColor( tailColorLookup.at(j) );
				//flyersMesh.addTexCoord( tailTexCoordLookup.at(j) + ofVec2f(0,1));
			}
*/
 
 
		}
		
		/*
		 // this looks great even at grassNoisePlacementThreshold = 1.0 as we still get a nice sweeping variation in height
		 if( noise < grassNoisePlacementThreshold && terrainVertices[i].y < dimension.get().y * grassPlacementMaxHeight ) { // stops grass growing high, but grass stops suddenly there
		 
		 
		 stalkHeightFraction = ofMap( noise, 0.0f, grassNoisePlacementThreshold,  1.0f, 0.1f, true );//noise / 0.5f;
		 stalkHeightFraction *= ofMap( terrainVertices[i].y, dimension.get().y * (grassPlacementMaxHeight-0.2f), dimension.get().y * (grassPlacementMaxHeight),  1.0f, 0.1f, true );
		 
		 grassMesh.addVertex( terrainVertices[i] );
		 grassMesh.addVertex( terrainVertices[i] + (terrainNormals[i] * stalkHeightFraction) ); // we normalize in the shader
		 }
		 */
	}
}

//--------------------------------------------------------------
float ofApp::getHeightmapValue( int _x, int _y )
{
	if( _x < 0 ) _x = 0;
	if( _x > heightmapRes ) _x = heightmapRes;
	
	if( _y < 0 ) _y = 0;
	if( _y > heightmapRes ) _y = heightmapRes;
	
	return heightmap[ (_y * heightmapRes) + _x ];
}

// ---------------------------------------------------------------------------------------------------
//
float ofApp::getTerrainHeightAtWorldSpaceCoordinate( float _x, float _y )
{
	if( heightmap.size() == 0 ) 
	{
		return 0.0f;
	}

	_x -= terrainMeshRenderingPosition.x;
	_y -= terrainMeshRenderingPosition.z;
	
	_x = ofMap( _x, dimension.get().x * -0.5, dimension.get().x * 0.5, 0, heightmapRes-1, true );
	_y = ofMap( _y, dimension.get().z * -0.5, dimension.get().z * 0.5, 0, heightmapRes-1, true );
	
	int indexX = _x;
	int indexY = _y;
	
	float fX = _x - indexX;
	float fY = _y - indexY;
	
	float A = getHeightmapValue( indexX, 		indexY 		);
	float B = getHeightmapValue( indexX + 1, 	indexY 		);
	float C = getHeightmapValue( indexX, 		indexY + 1 	);
	float D = getHeightmapValue( indexX + 1, 	indexY + 1	);
	
	return ((A + (B -A) * fX) + ((C + (D - C) * fX) - (A + (B - A) * fX)) * fY) * dimension.get().y;
}

// ---------------------------------------------------------------------------------------------------
//
ofVec3f ofApp::getTerrainNormalAtWorldSpaceCoordinate( float _x, float _y )
{
	_x -= terrainMeshRenderingPosition.x;
	_y -= terrainMeshRenderingPosition.z;
	
	_x = ofMap( _x, dimension.get().x * -0.5, dimension.get().x * 0.5, 0, heightmapRes-1, true );
	_y = ofMap( _y, dimension.get().z * -0.5, dimension.get().z * 0.5, 0, heightmapRes-1, true );
	
	int indexX = _x;
	int indexY = _y;
	
	// Todo: pulling scale out of my ass
	float scale = 0.1f; //dimension.y / ( dimension.x / resolutionX ); //dimension.x; // / dimension.y; //hf.GetXYScale();
	float z0 = getHeightmapValue( indexX, indexY ); //paV[ 0 ].m_Z;
	
	float Az = ( indexX + 1 < heightmapRes	) ? ( getHeightmapValue( indexX + 1, 	indexY ) )		: z0;
	float Bz = ( indexY + 1 < heightmapRes	) ? ( getHeightmapValue( indexX, 		indexY + 1 ) )	: z0;
	float Cz = ( indexX - 1 >= 0			) ? ( getHeightmapValue( indexX - 1, 	indexY ) )		: z0;
	float Dz = ( indexY - 1 >= 0			) ? ( getHeightmapValue( indexX, 		indexY - 1)  )	: z0;
	
	return ofVec3f( Cz - Az, 2.f * scale, Dz - Bz ).getNormalized();
}



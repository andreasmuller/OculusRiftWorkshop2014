//
//  Created by Andreas Müller on 23/02/2014.
//
//

#pragma once

#include "ofMesh.h"

class ofMeshExt : public ofMesh
{
	public:
		
		//----------------------------------------------------------
		ofMeshExt()
		{
			hasOverrideGLDrawMode = false;
		}

		//----------------------------------------------------------
		void setOverrideGLDrawMode( GLuint _drawMode )
		{
			overrideGLDrawMode = _drawMode;
			hasOverrideGLDrawMode = true;
		}
	
		//--------------------------------------------------------------
		void setMode(ofPrimitiveMode m)
		{
			hasOverrideGLDrawMode = false;
			ofMesh::setMode(m);
		}
	
		//----------------------------------------------------------
		void draw( bool useColors = true, bool useTextures = true, bool useNormals = true )
		{			
			if(getNumVertices()){
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), &getVerticesPointer()->x);
			}
			if(getNumNormals() && useNormals){
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT, sizeof(ofVec3f), &getNormalsPointer()->x);
			}
			if(getNumColors() && useColors){
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(4,GL_FLOAT, sizeof(ofFloatColor), &getColorsPointer()->r);
			}
			
			if(getNumTexCoords() && useTextures){
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, sizeof(ofVec2f), &getTexCoordsPointer()->x);
			}
			
			GLuint drawMode;
			if( hasOverrideGLDrawMode ) { drawMode = overrideGLDrawMode; }
			else { drawMode = ofGetGLPrimitiveMode(getMode()); }
			
			if(getNumIndices()){
	#ifdef TARGET_OPENGLES
				glDrawElements(drawMode, getNumIndices(),GL_UNSIGNED_SHORT,getIndexPointer());
	#else
				glDrawElements(drawMode, getNumIndices(),GL_UNSIGNED_INT,getIndexPointer());
	#endif
			}else{
				glDrawArrays(drawMode, 0, getNumVertices());
			}
			
			if(getNumColors() && useColors){
				glDisableClientState(GL_COLOR_ARRAY);
			}
			if(getNumNormals() && useNormals){
				glDisableClientState(GL_NORMAL_ARRAY);
			}
			if(getNumTexCoords() && useTextures){
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
		}
	
	private:
	
		bool hasOverrideGLDrawMode;
		GLuint overrideGLDrawMode;
	
	
};
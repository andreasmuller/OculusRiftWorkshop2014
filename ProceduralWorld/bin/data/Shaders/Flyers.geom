
#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform vec3 cameraWorldPos;

uniform mat4 cameraMatrix;
uniform mat4 cameraInverseMatrix;
uniform mat4 cameraPerspectiveMatrix;

//varying vec2 texCoord;

//-------------------------------------------------------------------------------------------------------------------------------------
//
void main()
{
	
	vec4 p0 = gl_PositionIn[0];
	vec4 p1 = gl_PositionIn[1];
	vec4 p2 = gl_PositionIn[2];
	vec4 p3 = gl_PositionIn[3];
	
	vec4 lineNormal = p2 - p1;
	float normalOrigLength = length(lineNormal);
	lineNormal = lineNormal / normalOrigLength; // keep this as a vec4
	
	//vec4 normalPrevLine = normalize(p1 - p0);
	//vec4 normalCurrLine = normalize(p2 - p1);
	
	//screen-aligned axes
//	vec3 right = normalize(vec3(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0]));
//	vec3 up    = normalize(vec3(gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1]));
	
	vec3 X = vec3( gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0] );
	vec3 Y = vec3( gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1] );
    vec3 Z = vec3( gl_ModelViewMatrix[0][2], gl_ModelViewMatrix[1][2], gl_ModelViewMatrix[2][2] );
	
	vec4 vertexPosCameraSpace = (gl_ModelViewMatrix * p1);
	//vec4 vertexPosCameraSpace = gl_ModelViewMatrix * (p0);
	vec3 eyeDirectionCameraSpace = -vertexPosCameraSpace.xyz;
		
	// calculate the normal here as well so that we can light it, this is important to get the right look
//	vec3 up = normalize( cross( eyeDirectionCameraSpace, lineNormal.xyz) );

	//vec3 upPrevLine = normalize( cross( cameraWorldPos - (gl_ModelViewMatrix * p1).xyz, normalPrevLine.xyz) );
	//vec3 upCurrLine = normalize( cross( cameraWorldPos - (gl_ModelViewMatrix * p2).xyz, normalCurrLine.xyz) );

	vec3 toEyePrevLine = normalize( cameraWorldPos - (gl_ModelViewMatrix * p1).xyz );
	vec3 toEyeCurrLine = normalize( cameraWorldPos - (gl_ModelViewMatrix * p2).xyz );
	//vec3 toEyePrevLine = vec3( 0.0, 0.0, -1.0 );
	//vec3 toEyeCurrLine = vec3( 0.0, 0.0, -1.0 );
	
	vec3 forwardPrevLine = normalize((gl_ModelViewMatrix*p1) - (gl_ModelViewMatrix*p0)).xyz;
	vec3 forwardCurrLine = normalize((gl_ModelViewMatrix*p2) - (gl_ModelViewMatrix*p1)).xyz;
	//vec3 forwardPrevLine = normalize((p1) - (p0)).xyz;
	//vec3 forwardCurrLine = normalize((p2) - (p1)).xyz;
	
	vec3 upPrevLine = cross( toEyePrevLine, forwardPrevLine );
	vec3 upCurrLine = cross( toEyeCurrLine, forwardCurrLine );
	
	/*
	 ofVec3f p0 = p->positionHistory.at(j);
	 ofVec3f p1 = p->positionHistory.at(j+1);
	 
	 ofVec3f toEye = eyePos - p0;
	 ofVec3f forward = p0 - p1;
	 
	 ofVec3f up = toEye.getCrossed( forward );
	 up.normalize();
	 */
	
	float lineHeightHalf = 3.0;
	
	vec3 up    = Y; //vec3(0.0,1.0,0.0);
	
	gl_Position = gl_ModelViewProjectionMatrix * vec4((p1.xyz - upPrevLine * lineHeightHalf), 1.0);
	//gl_Position = (cameraPerspectiveMatrix * cameraMatrix) * vec4((p1.xyz - upPrevLine * lineHeightHalf), 1.0);
	//gl_Position = cameraPerspectiveMatrix * vec4((p1.xyz + upPrevLine * lineHeightHalf), 1.0);
	//gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[0];
	gl_FrontColor = gl_FrontColorIn[1];
	EmitVertex();

	gl_Position = gl_ModelViewProjectionMatrix * vec4((p1.xyz + upPrevLine * lineHeightHalf), 1.0);
	//gl_Position = (cameraPerspectiveMatrix * cameraMatrix) * vec4((p1.xyz + upPrevLine * lineHeightHalf), 1.0);
	//gl_Position = cameraPerspectiveMatrix * vec4((p1.xyz + upPrevLine * lineHeightHalf), 1.0);
	//gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[1];
	gl_FrontColor = gl_FrontColorIn[1];
	EmitVertex();

	gl_Position = gl_ModelViewProjectionMatrix * vec4((p2.xyz - upCurrLine * lineHeightHalf), 1.0);
	//gl_Position = (cameraPerspectiveMatrix * cameraMatrix) * vec4((p2.xyz - upCurrLine * lineHeightHalf), 1.0);
	//gl_Position = cameraPerspectiveMatrix * vec4((p2.xyz - upCurrLine * lineHeightHalf), 1.0);
	//gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[2];
	gl_FrontColor = gl_FrontColorIn[2];
	EmitVertex();

	gl_Position = gl_ModelViewProjectionMatrix * vec4((p2.xyz + upCurrLine * lineHeightHalf), 1.0);
	//gl_Position = (cameraPerspectiveMatrix * cameraMatrix) * vec4((p2.xyz + upCurrLine * lineHeightHalf), 1.0);
	//gl_Position = cameraPerspectiveMatrix * vec4((p2.xyz + upCurrLine * lineHeightHalf), 1.0);
	//gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[3];
	gl_FrontColor = gl_FrontColorIn[2];
	EmitVertex();
		

}

/*
 vec1 = eyePos - tmpPositions[index1];
 vec2 = tmpPositions[index1] - tmpPositions[index2];
 
 //			vec1.Normalize();
 //			vec2.Normalize();
 
 vec.CrossProduct( vec1, vec2 );
 vec.Normalize();
 
 vec.Scale( 0.2f );
 
 // update this
 vertices[tmp]   = tmpPositions[j] - vec;
 vertices[tmp+1] = tmpPositions[j] + vec;
 */

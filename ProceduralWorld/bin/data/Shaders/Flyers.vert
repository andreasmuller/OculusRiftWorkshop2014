#version 120

//varying vec3 normal;

uniform mat4 cameraMatrix;
uniform mat4 cameraInverseMatrix;
uniform mat4 cameraPerspectiveMatrix;

void main()
{
	gl_FrontColor =  gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_Vertex;
}

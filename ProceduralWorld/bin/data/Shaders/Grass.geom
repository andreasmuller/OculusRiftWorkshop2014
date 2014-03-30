
#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform float timeSecs;

uniform vec3 cameraWorldPos;

uniform mat4 cameraMatrix;
uniform mat4 cameraInverseMatrix;

uniform vec3 noiseGenerationPos;

uniform float stalkHalfWidth;
uniform float stalkHeight;
uniform float stalkSwayingMaxAngle;

uniform float grassSwayingNoiseSpaceDiv;
uniform float grassSwayingNoiseMag;
uniform float grassSwayingTimeScale;

//-------------------------------------------------------------------------------------------------------------------------------------
//
float rand(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453); }
float map( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin); }
float mapClamped( float value, float inputMin, float inputMax, float outputMin, float outputMax ) { return clamp( ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin),    outputMin, outputMax ); }
float stepInOut( float _edge1, float _edge2, float _val ) { return step(_edge1, _val) - step(_edge2,_val); }
float smoothStepInOut( float _low0, float _high0, float _high1, float _low1, float _t ) { return smoothstep( _low0, _high0, _t ) * (1.0 - smoothstep( _high1, _low1, _t )); }
vec3  mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2  mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3  permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

//-------------------------------------------------------------------------------------------------------------------------------------
//
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

//-------------------------------------------------------------------------------------------------------------------------------------
//
float snoise(vec2 v)
{
	const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
						0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,  // -1.0 + 2.0 * C.x
						0.024390243902439); // 1.0 / 41.0
	// First corner
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);
	
	// Other corners
	vec2 i1;
	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
	//i1.y = 1.0 - i1.x;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	// x0 = x0 - 0.0 + 0.0 * C.xx ;
	// x1 = x0 - i1 + 1.0 * C.xx ;
	// x2 = x0 - 1.0 + 2.0 * C.xx ;
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	
	// Permutations
	i = mod289(i); // Avoid truncation effects in permutation
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
					 + i.x + vec3(0.0, i1.x, 1.0 ));
	
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;
	
	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
	
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	
	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
	
	// Compute final noise value at P
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}



//-------------------------------------------------------------------------------------------------------------------------------------
//
void main()
{
	
	vec4 color = gl_FrontColorIn[0];
	
	vec4 p0 = gl_PositionIn[0];
	vec4 p1 = gl_PositionIn[1];
	vec4 normal = p1 - p0;
	float normalOrigLength = length(normal);
	normal = normal / normalOrigLength; // keep this as a vec4
	
	
	//screen-aligned axes
	vec3 right = normalize(vec3(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0]));
	vec3 up    = normalize(vec3(gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1]));


	//vec3 right = normalize(vec3( gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0] )); // X
    //vec3 up = normalize(vec3( gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1] )); // Y
    //vec3 Z = normalize(vec3( gl_ModelViewMatrix[0][2], gl_ModelViewMatrix[1][2], gl_ModelViewMatrix[2][2] )); // Z
	
	
	//vec3 planeNormal =  (cameraInverseMatrix * p0).xyz - cameraWorldPos;
	//	planeNormal.y = 0.0f;
	//planeNormal = normalize(planeNormal);
	//vec3 upVector = vec3(0.0f, 1.0f, 0.0f);
	//vec3 rightVector = normalize(cross(planeNormal, upVector));
	//rightVector = rightVector * halfWidth;
	//	upVector = vec3(0, 1, 0); // The billboard is straight up and down
	
	
	/*
	up   = vec3(gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1]);
    up   = normalize(up);
    left = vec3(gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0]);
    left = normalize(left);

	// orientation (billboard)
	vec3 X = vec3( gl_ModelViewMatrix[0][0], gl_ModelViewMatrix[1][0], gl_ModelViewMatrix[2][0] );
    vec3 Y = vec3( gl_ModelViewMatrix[0][1], gl_ModelViewMatrix[1][1], gl_ModelViewMatrix[2][1] );
    vec3 Z = vec3( gl_ModelViewMatrix[0][2], gl_ModelViewMatrix[1][2], gl_ModelViewMatrix[2][2] );    
	*/
	
    //vec3 up = vec3(0.0, 1.0, 0.0);
    //vec3 right = cross( normalize((gl_ModelViewMatrix * p0).xyz), up );
    //vec3 right = cross(normalize( (cameraMatrix * p0).xyz), up);
	
	float maxTimeDifferenceBetweenStalks = 3.0;
	float stalkHeightMinFrac = 0.6;
	
	//vec2 randReadPos = noiseGenerationPos.xz + p0.xz;
	//vec2 randReadPos = (cameraMatrix * p0).xz;
	//vec2 randReadPos = vec2(0.0,0.0);
	vec2 randReadPos = p0.xz;
	
	float tmpRand = rand(randReadPos);

	//float stalkLength = stalkHeight * map( tmpRand, 0.0, 1.0, stalkHeightMinFrac, 1.0 ); // map( sin(timeSecs + timeOffset ), -1.0, 1.0, 30.0, 100.0 );
	float stalkLength = stalkHeight * normalOrigLength; // map( sin(timeSecs + timeOffset ), -1.0, 1.0, 30.0, 100.0 );
	
//stalkLength *= normalOrigLength;
	
	float bendMaxAngle = stalkSwayingMaxAngle;
	
	float scaledTime = timeSecs * grassSwayingTimeScale; // otpimize this, send in scaled time, unless we use it anywhere else?

	float tmpNoiseVal = snoise(p0.xz/grassSwayingNoiseSpaceDiv);
	
	stalkLength = stalkHeight * normalOrigLength * map( tmpNoiseVal, 0.0, 1.0, stalkHeightMinFrac, 1.0 );
	
	
	
	//float timeOffset = 0.0;
	//float timeOffset = length(p0.xz);// * map( sin(timeSecs + timeOffset ), -1.0, 1.0, -2.0, 2.0 );
	float timeOffset = map( (tmpNoiseVal * grassSwayingNoiseMag), -1.0, 1.0, -maxTimeDifferenceBetweenStalks, maxTimeDifferenceBetweenStalks);  //length(p0.xz) * map( sin(timeSecs + timeOffset ), -1.0, 1.0, -2.0, 2.0 );
	//float timeOffset = map( sin(scaledTime + (snoise(p0.xz/grassSwayingNoiseSpaceDiv) * grassSwayingNoiseMag)), -1.0, 1.0, -maxTimeDifferenceBetweenStalks, maxTimeDifferenceBetweenStalks);  //length(p0.xz) * map( sin(timeSecs + timeOffset ), -1.0, 1.0, -2.0, 2.0 );
	//float timeOffset = map( tmpRand, 0.0, 1.0, -maxTimeDifferenceBetweenStalks, maxTimeDifferenceBetweenStalks );
	
	scaledTime += timeOffset;

	float swayingAngleRad1 = map( sin(scaledTime), -1.0, 1.0, -bendMaxAngle, bendMaxAngle); //sin(scaledTime + (snoise(p0.xz/perlingSpaceDivisor) * perlingMagnitude))
	float swayingAngleRad2 = map( cos(scaledTime), -1.0, 1.0, -bendMaxAngle, bendMaxAngle); //sin(scaledTime + (snoise(p0.xz/perlingSpaceDivisor) * perlingMagnitude))
	mat4 swayinMat = rotationMatrix( up, swayingAngleRad1 ) * rotationMatrix( right, swayingAngleRad2 );// * rotationMatrix( right, swayingAngleRad );
			
	float camDist = length((gl_ModelViewMatrix * p0).xyz);
	
	//if( camDist > 500.0 ) { color = vec4(0,1,0,1); }
	float distanceFrac = mapClamped( camDist, 300.0, 2000.0,  0.0, 1.0 );
	
	// TEMP
	const int vertexOutAmount = 5;
	float vertexOutAmountf = 5.0;
	
	//distanceFrac = floor(distanceFrac / (1.0/vertexOutAmountf)) * (1.0/vertexOutAmountf);
	distanceFrac = 1.0 - distanceFrac;
	
	//color = vec4(distanceFrac,1.0-distanceFrac,1.0-distanceFrac,1.0);
 	
	//color.r = map( sin(scaledTime), -1.0, 1.0, 0.0, 1.0 );
	//color.g = map( cos(scaledTime/2.0), -1.0, 1.0, 0.0, 1.0 );
	//color.b = 1.0 - color.g; // this could look incredible, try replacing it with a palette texture.
	
	//float colOffset = (timeOffset+maxTimeDifferenceBetweenStalks) / (maxTimeDifferenceBetweenStalks * 2.0);
	//color.r -= (colOffset / 10.0);
	//color.g += (colOffset / 10.0);
	
	float randomMovementMagnitude = 0.0;
	
	vec3 sideVector = right * stalkHalfWidth;
	
	vec4 tmpNormal = normal;
	//vec4 tmpPos = p0;
	vec4 tmpPos = p0 + vec4( (right*tmpRand*randomMovementMagnitude) + (up*tmpRand*randomMovementMagnitude), 1.0);
	//vec4 tmpPos = p0 + vec4( (right*tmpRand*5.0) + (right*tmpRand*5.0), 1.0);
	
	int vertexAmountByDistance = vertexOutAmount; //int(distanceFrac * vertexOutAmountf) + 2;//;
	float vertexAmountByDistancef = float(vertexAmountByDistance);
	
	float stepMagnitude = stalkLength / (vertexAmountByDistancef - 1.0);
	
	for( int i = 0; i < vertexAmountByDistance; i++ )
	{
		float tmpFrac = float(i) / (vertexOutAmountf-1); //float((vertexOutAmount-1));
		//tmpFrac = 1.0 - tmpFrac;
		
		float darkenAmount = smoothstep( 0.4, 1.0, tmpFrac ) ;
		darkenAmount = map( darkenAmount, 0.0, 1.0,		0.3, 1.0 );
		vec4 darkenColor = vec4( darkenAmount, darkenAmount, darkenAmount, 1.0 );
		
		//gl_Position = gl_ModelViewProjectionMatrix * vec4(((tmpPos.xyz - sideVector)), 1.0);
		gl_Position = gl_ModelViewProjectionMatrix * vec4(((tmpPos.xyz - sideVector * (1.0-tmpFrac))), 1.0);
		//gl_TexCoord[0] =
		//gl_Normal = tmpNormal; // is this correct?
		gl_FrontColor = color * darkenColor;
		EmitVertex();
		
		//gl_Position = gl_ModelViewProjectionMatrix * vec4(((tmpPos.xyz + sideVector)), 1.0);
		gl_Position = gl_ModelViewProjectionMatrix * vec4(((tmpPos.xyz + sideVector * (1.0-tmpFrac))), 1.0);
		//gl_Normal = tmpNormal; // is this correct?
		gl_FrontColor = color * darkenColor;
		EmitVertex();
		
		//tmpNormal = normalize( swayinMat * tmpNormal );
		tmpNormal = tmpNormal * swayinMat;
		tmpPos += tmpNormal * stepMagnitude;
	}
}
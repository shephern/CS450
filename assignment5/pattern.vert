#version 120

varying vec2  	vST;		// texture coords
uniform float uA, uB;

const float PI = 	3.14159265;
const float AMP = 	0.2;		// amplitude
const float W = 	2.;		// frequency

void
main( )
{
	vST = gl_MultiTexCoord0.st;
	vec3 vert = gl_Vertex.xyz;
	if( vert.x > 1.)
		vert.x += 3.*(vert.x-1.)*uA;
	else if( vert.x < -1.)
		vert.x += 3.*(vert.x+1.)*uA;

	if( vert.y > 1.)
		vert.y += 3.*(vert.y-1.)*uA;
	else if( vert.y < -1.)
		vert.y += 3.*(vert.y+1.)*uA;

	if( vert.z > 1.)
		vert.z += 3.*(vert.z-1.)*uA;
	else if( vert.z < -1.)
		vert.z += 3.*(vert.z+1.)*uA;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(vert, 1.);
}

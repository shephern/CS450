#version 120

varying vec2  	vST;		// texture coords
uniform vec3 uColor;
uniform float uS0, uT0;
uniform float uC, uD;

const float PI = 	3.14159265;

void
main( )
{

	float lon = .15;
	vec3 myColor = uColor;
	if( vST.t > (.6+lon*uC) && vST.t < (.8 -lon*uC) || vST.t > (.2+lon*uC) && vST.t < (.4 -lon*uC))
	{
		myColor = vec3( 1., 1., 0. );
	}

	if( vST.s < .1 -.25*uC || vST.s > (.2+.1*uC) && vST.s < (.3 -.1*uC) || vST.s > (.45+.1*uC) && vST.s < (.55 -.1*uC) || vST.s > (.7+.1*uC) && vST.s < (.8 -.1*uC) || vST.s > (.95+.1*uC))
	{
		myColor = vec3( 1., 1., 0. );
	}


	if( vST.s < .1 -.25*uC || vST.s > (.2+.1*uC) && vST.s < (.3 -.1*uC) || vST.s > (.45+.1*uC) && vST.s < (.55 -.1*uC) || vST.s > (.7+.1*uC) && vST.s < (.8 -.1*uC) || vST.s > (.95+.1*uC)){
		if( vST.t > (.6+lon*uC) && vST.t < (.8 -lon*uC) || vST.t > (.2+lon*uC) && vST.t < (.4 -lon*uC))
		{
			myColor = vec3( 1., 0., 1. );
		}
	}
	gl_FragColor = vec4( myColor,  1. );
}

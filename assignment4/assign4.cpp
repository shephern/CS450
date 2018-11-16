#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "GLUT/glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "Assignment 4: Nathan Shepherd" };
const char *GLUITITLE   = { "Lighting and Shading" };

float Time;
#define MS_PER_CYCLE	10000;

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };


// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0.3, 0.3, 0.3, 1. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] =
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

struct point {
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

float White[ ] = { 1.,1.,1.,1. };


int		NumLngs, NumLats;
struct point *	Pts;

// Bmp definitions
int	ReadInt( FILE * );
short	ReadShort( FILE * );
unsigned char * BmpToTexture( char *, int *, int *);

struct bmfh
{
	short bfType;
	int bfSize;
	short bfReserved1;
	short bfReserved2;
	int bfOffBits;
} FileHeader;

struct bmih
{
	int biSize;
	int biWidth;
	int biHeight;
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
} InfoHeader;

const int birgb = { 0 };

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
GLuint sphere;
GLuint lighthouseList;
GLuint RockList;
GLuint containerList;
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

bool enableLight0 = true;
bool enableLight1 = true;
bool enableLight2 = true;
bool freezeAnimation = false;
// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void  DoTextureMenu( int );
void  DoModeMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );

float * Array3(float, float, float);
float * MulArray3(float, float [3]);

void SetMaterial( float, float, float, float);
void SetPointLight(int, float, float, float, float, float, float);
void SetSpotLight(int, float, float, float, float, float, float, float, float, float);

//sphere defs
struct point * PtsPointer( int, int );
void DrawPoint( struct point *);
void MjbSphere( float , int , int );


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

	InitLists( );


	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );


	// setup all the user interface stuff:

	InitMenus( );


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );


	// this is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:
	int ms = glutGet( GLUT_ELAPSED_TIME );
	ms %= MS_PER_CYCLE;
	Time = (float)ms / (float)MS_PER_CYCLE;
	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( DepthBufferOn != 0 )
		glEnable( GL_DEPTH_TEST );
	else
		glDisable( GL_DEPTH_TEST );

	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// glLightModelfv( GL_LIGHT_MODEL_AMBIENT, MulArray3( .2, White ) );
	// glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

	// Give us some light
	// Point light
	SetSpotLight(GL_LIGHT0, .18*cos(4.*M_PI*Time), 1.8, .18*sin(4.*M_PI*Time),
			2.*cos(4.*M_PI*Time), 1.8, 2.*sin(4.*M_PI*Time),
			.5, .5, .5
	);

	SetPointLight(GL_LIGHT1, 0., 3., 2., .5, 0., 0.);

	SetPointLight(GL_LIGHT2, 1., .2, -.3,
			.1, .1, .5
	);
	// set the eye position, look-at position, and up-vector:
	glEnable( GL_NORMALIZE );
	gluLookAt( 0., 2., 5.,     0., 0., 0.,     0., 1., 0. );

	glEnable( GL_LIGHTING );

	if(enableLight0)
		glEnable( GL_LIGHT0 );
	else
		glDisable( GL_LIGHT0 );

	if(enableLight1)
		glEnable( GL_LIGHT1 );
	else
		glDisable( GL_LIGHT1 );

	if(enableLight2)
		glEnable( GL_LIGHT2 );
	else
		glDisable( GL_LIGHT2 );
	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );

	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );


	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable( GL_NORMALIZE );

	glEnable( GL_TEXTURE_2D );

	// draw the current object:

	//glCallList( BoxList );

	// glCallList( sphere );
	float r = 0.08;
	int slices = 30;
	int stacks = 30;

	glPushMatrix();
	glColor3f(1., 1., 1.);
	glTranslatef( .18*cos(4.*M_PI*Time), 1.8, .18*sin(4.*M_PI*Time) );
	MjbSphere(r, slices, stacks);
	glPopMatrix();

	glPushMatrix();
	glColor3f(.1, .1, .5);
	glTranslatef( 1., .2, -.3 );
	MjbSphere(r, slices, stacks);
	glPopMatrix();

	glPushMatrix();
	glColor3f( .5, 0., 0.);
	glTranslatef( 0., 3., 2. );
	MjbSphere(r, slices, stacks);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0., 0., -6.);
	glutSolidCube(4.);
	glPopMatrix();

  glCallList( lighthouseList );
	glCallList( RockList );
	// glCallList( containerList );

	float timestand;
	if(Time < .5)
		timestand = Time;
	else{
		timestand = .5 - (Time - .5);
	}

	glColor3f(1., 1., 1.);
	SetMaterial(1., 1., 1., 0.);

	glTranslatef(0., 1.9, 0. );
	glRotatef(35.*timestand, .3*timestand, 0., .3*timestand);
	glRotatef(90.0, 1.0, 0., 0.);
	glShadeModel( GL_SMOOTH );
	glutSolidTorus(.1, 1., 40.0, 50.0);

	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.,   0., 1., 0. );
			glCallList( sphere );
		glPopMatrix( );
	}
	glDisable( GL_TEXTURE_2D );

	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100., 0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	// glColor3f( 1., 1., 1. );
	// DoRasterString( 5., 5., 0., "Text That Doesn't" );


	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{

	// float r = 1.;
	// int slices = 30;
	// int stacks = 30;
	//
	//
	// sphere = glGenLists( 1 );
	// glNewList( sphere, GL_COMPILE );
	// 	MjbSphere(r, slices, stacks);
	// glEndList( );

	int offset = -3;
	int segments = 13;
	float radius = 1.f;
	float slope = .07f;
	float dang = 2. * M_PI / (float)(segments - 1);
	float ang = 0.;
	float h1,h2;

	float depth = 5.;
	float height = 4.;
	float leng = 8.;

	glutSetWindow( MainWindow );


	containerList = glGenLists( 1 );
	glNewList( containerList, GL_COMPILE );
		glShadeModel( GL_FLAT );
		glBegin( GL_QUADS );
			SetMaterial(1, 1, 1, 0.);
			glNormal3f(0., 0., 0.);
			glVertex3f(-leng/2., -height/2., -depth/2.);
			glVertex3f(-leng/2., -height/2., depth/2.);
			glVertex3f(leng/2., -height/2., depth/2.);
			glVertex3f(leng/2., -height/2., -depth/2.);
		glEnd();
	glEndList();

 	lighthouseList = glGenLists( 1 );
	glNewList( lighthouseList, GL_COMPILE );
		glShadeModel( GL_FLAT );
		glBegin( GL_TRIANGLES );
		//Base section
		// SetMaterial(1., 0., 0., 0.);
		// glBegin(GL_TRIANGLE_FAN);
		// 	glVertex3f(0, offset, 0);
		// 	for ( int i = 0; i < segments; i++ )
		// 	{
		// 		glVertex3f( (radius)*cos(ang), offset, (radius)*sin(ang) );
		// 		ang += dang;
		// 	}
		// glEnd( );
		//
		// ang = 0.;
		// for ( int i = 0; i < 8; i += 1)
		// {
		// 	if(i%2 == 0){
		// 		SetMaterial( 1., 0., 0., 0. );
		// 	} else {
		// 		SetMaterial( .2, .3, .2, 0. );
		// 	}
		//
		// 	ang = 0.;
		// 	h1 = float(i) / 2 + offset;
		// 	h2 = float(i+1) / 2 + offset;
		//
		// 	glBegin(GL_TRIANGLE_STRIP);
		// 		for ( int j = 0; j < segments; j++ )
		// 		{
		// 			glVertex3f( radius*cos(ang), h1, radius*sin(ang) );
		// 			glVertex3f( (radius - slope)*cos(ang), h2, (radius - slope)*sin(ang) );
		// 			ang += dang;
		// 		}
		// 		radius -= slope;
		// 	glEnd( );
		// }
		//
		// //Light cover
		// glColor3f(1., 1., 1.);
		// SetMaterial(1., 1., 1., 1.);
		// glBegin(GL_TRIANGLE_STRIP);
		// 	for ( int i = 0; i < segments; i++ )
		// 	{
		// 		glVertex3f( radius*cos(ang), 4 + offset, radius*sin(ang) );
		// 		glVertex3f( (radius)*cos(ang), 4.7f + offset, (radius)*sin(ang) );
		//
		// 		ang += dang;
		// 	}
		// 	ang = 0.;
		// glEnd( );
		//
		// //Roof section
		// glColor3f(1., 0., 0.);
		// SetMaterial(1., 0., 0., .2);
		// for (int j = 0; j < 2; j++){
		// 	glBegin(GL_TRIANGLE_FAN);
		// 		if(j == 0){
		// 			glVertex3f(0, 4.7 + offset, 0);
		// 		} else {
		// 			glVertex3f(0, 5 + offset, 0);
		// 		}
		// 		for ( int i = 0; i < segments; i++ )
		// 		{
		// 			glVertex3f( (radius+.2)*cos(ang), 4.7 + offset, (radius+.2)*sin(ang) );
		// 			ang += dang;
		// 		}
		// 	glEnd( );
		// }
		// red
		SetMaterial( 1, 0, 0, 0.);
			glNormal3f( -0.000000024894101, -1.000000000000000, 0.000000180328769 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( -0.000000000296557, -0.000000057845746, -0.320779353380203);
				glVertex3f( 0.160389676690102, -0.000000054088630, -0.277803003787994);

			glNormal3f( 0.258336901664734, 0.061050239950418, -0.964123964309692 );
				glVertex3f( -0.000000001221039, 1.299711942672729, -0.204872012138367);
				glVertex3f( -0.000000001403581, 1.559654235839844, -0.188411951065063);
				glVertex3f( 0.094205856323242, 1.559654235839844, -0.163169503211975);

			glNormal3f( 0.000000012447047, -1.000000000000000, 0.000000201887687 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( 0.160389676690102, -0.000000054088630, -0.277803003787994);
				glVertex3f( 0.277803063392639, -0.000000028922869, -0.160389646887779);

			glNormal3f( 0.705787479877472, 0.061050020158291, -0.705788195133209 );
				glVertex3f( 0.094205856323242, 1.559654235839844, -0.163169503211975);
				glVertex3f( 0.163169324398041, 1.559654235839844, -0.094206109642982);
				glVertex3f( 0.177424147725105, 1.299711942672729, -0.102436065673828);

			glNormal3f( 0.000000000000000, -1.000000000000000, 0.000000180328783 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( 0.277803063392639, -0.000000028922869, -0.160389646887779);
				glVertex3f( 0.320779353380203, 0.000000000000003, 0.000000018173509);

			glNormal3f( 0.964124143123627, 0.061050150543451, -0.258336335420609 );
				glVertex3f( 0.177424147725105, 1.299711942672729, -0.102436065673828);
				glVertex3f( 0.163169324398041, 1.559654235839844, -0.094206109642982);
				glVertex3f( 0.188411712646484, 1.559654235839844, -0.000000255558575);

			glNormal3f( 0.000000000000000, -1.000000000000000, 0.000000180328769 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( 0.320779353380203, 0.000000000000003, 0.000000018173509);
				glVertex3f( 0.277803063392639, 0.000000028922873, 0.160389676690102);

			glNormal3f( 0.964124143123627, 0.061050273478031, 0.258336335420609 );
				glVertex3f( 0.204871773719788, 1.299711942672729, -0.000000232833713);
				glVertex3f( 0.188411712646484, 1.559654235839844, -0.000000255558575);
				glVertex3f( 0.163169324398041, 1.559654235839844, 0.094205603003502);

			glNormal3f( -0.000000012447045, -1.000000000000000, 0.000000201887673 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( 0.277803063392639, 0.000000028922873, 0.160389676690102);
				glVertex3f( 0.160389676690102, 0.000000054088638, 0.277803063392639);

			glNormal3f( 0.705788373947144, 0.061050444841385, 0.705787301063538 );
				glVertex3f( 0.177424147725105, 1.299711942672729, 0.102435708045959);
				glVertex3f( 0.163169324398041, 1.559654235839844, 0.094205603003502);
				glVertex3f( 0.094205886125565, 1.559654235839844, 0.163169145584106);

			glNormal3f( 0.000000024894087, -1.000000000000000, 0.000000180328783 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( 0.160389676690102, 0.000000054088638, 0.277803063392639);
				glVertex3f( 0.000000048139775, 0.000000057845746, 0.320779353380203);

			glNormal3f( 0.258335769176483, 0.061050143092871, 0.964124202728271 );
				glVertex3f( 0.102435901761055, 1.299711942672729, 0.177423954010010);
				glVertex3f( 0.094205886125565, 1.559654235839844, 0.163169145584106);
				glVertex3f( 0.000000027045798, 1.559654235839844, 0.188411474227905);

			glNormal3f( -0.000000024894089, -1.000000000000000, 0.000000180328783 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( 0.000000048139775, 0.000000057845746, 0.320779353380203);
				glVertex3f( -0.160389587283134, 0.000000054088638, 0.277803063392639);

			glNormal3f( -0.258335798978806, 0.061050254851580, 0.964124262332916 );
				glVertex3f( 0.000000029713739, 1.299711942672729, 0.204871535301208);
				glVertex3f( 0.000000027045798, 1.559654235839844, 0.188411474227905);
				glVertex3f( -0.094205811619759, 1.559654235839844, 0.163169145584106);

			glNormal3f( 0.000000012447100, -1.000000000000000, 0.000000201887715 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( -0.160389587283134, 0.000000054088638, 0.277803063392639);
				glVertex3f( -0.277803003787994, 0.000000028922880, 0.160389766097069);

			glNormal3f( -0.705787479877472, 0.061050191521645, 0.705788075923920 );
				glVertex3f( -0.102435842156410, 1.299711942672729, 0.177423954010010);
				glVertex3f( -0.094205811619759, 1.559654235839844, 0.163169145584106);
				glVertex3f( -0.163169294595718, 1.559654235839844, 0.094205722212791);

			glNormal3f( -0.000000000000009, -1.000000000000000, 0.000000180328698 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( -0.277803003787994, 0.000000028922880, 0.160389766097069);
				glVertex3f( -0.320779353380203, 0.000000000000028, 0.000000153286052);

			glNormal3f( -0.964124023914337, 0.061050131917000, 0.258336633443832 );
				glVertex3f( -0.177424103021622, 1.299711942672729, 0.102435708045959);
				glVertex3f( -0.163169294595718, 1.559654235839844, 0.094205722212791);
				glVertex3f( -0.188411712646484, 1.559654235839844, -0.000000136349300);

			glNormal3f( 0.000000000000000, -1.000000000000000, 0.000000180328868 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( -0.320779353380203, 0.000000000000028, 0.000000153286052);
				glVertex3f( -0.277803182601929, -0.000000028922859, -0.160389512777328);

			glNormal3f( -0.964124202728271, 0.061050180345774, -0.258335590362549 );
				glVertex3f( -0.204871758818626, 1.299711942672729, -0.000000113624424);
				glVertex3f( -0.188411712646484, 1.559654235839844, -0.000000136349300);
				glVertex3f( -0.163169398903847, 1.559654235839844, -0.094205990433693);

			glNormal3f( -0.000000012446989, -1.000000000000000, 0.000000201887715 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( -0.277803182601929, -0.000000028922859, -0.160389512777328);
				glVertex3f( -0.160389855504036, -0.000000054088630, -0.277802944183350);

			glNormal3f( -0.705788314342499, 0.061050038784742, -0.705787360668182 );
				glVertex3f( -0.177424222230911, 1.299711942672729, -0.102435946464539);
				glVertex3f( -0.163169398903847, 1.559654235839844, -0.094205990433693);
				glVertex3f( -0.094205982983112, 1.559654235839844, -0.163169503211975);

			glNormal3f( 0.000000024894138, -1.000000000000000, 0.000000180328783 );
				glVertex3f( 0.000000000000000, 0.000000000000000, 0.000000000000000);
				glVertex3f( -0.160389855504036, -0.000000054088630, -0.277802944183350);
				glVertex3f( -0.000000000296557, -0.000000057845746, -0.320779353380203);

			glNormal3f( -0.258336573839188, 0.061050176620483, -0.964124023914337 );
				glVertex3f( -0.102436020970345, 1.299711942672729, -0.177424311637878);
				glVertex3f( -0.094205982983112, 1.559654235839844, -0.163169503211975);
				glVertex3f( -0.000000001403581, 1.559654235839844, -0.188411951065063);

			glNormal3f( 0.062806360423565, -0.970110118389130, -0.234396383166313 );
				glVertex3f( -0.000000000574626, 1.908206582069397, -0.201212823390961);
				glVertex3f( -0.000000000155385, 1.924097537994385, -0.266981661319733);
				glVertex3f( 0.133490711450577, 1.924097537994385, -0.231212913990021);

			glNormal3f( 0.171590745449066, -0.970109641551971, -0.171591892838478 );
				glVertex3f( 0.100606277585030, 1.908206462860107, -0.174255549907684);
				glVertex3f( 0.133490711450577, 1.924097537994385, -0.231212913990021);
				glVertex3f( 0.231212630867958, 1.924097418785095, -0.133490979671478);

			glNormal3f( 0.234396293759346, -0.970110118389130, -0.062806397676468 );
				glVertex3f( 0.201212555170059, 1.908206462860107, -0.000000286030001);
				glVertex3f( 0.174255162477493, 1.908206462860107, -0.100606486201286);
				glVertex3f( 0.231212630867958, 1.924097418785095, -0.133490979671478);

			glNormal3f( 0.234397917985916, -0.970109701156616, 0.062806867063046 );
				glVertex3f( 0.201212555170059, 1.908206462860107, -0.000000286030001);
				glVertex3f( 0.266981422901154, 1.924097537994385, -0.000000287419255);
				glVertex3f( 0.231212630867958, 1.924097537994385, 0.133490383625031);

			glNormal3f( 0.171591371297836, -0.970109641551971, 0.171591550111771 );
				glVertex3f( 0.174255162477493, 1.908206462860107, 0.100606039166451);
				glVertex3f( 0.231212630867958, 1.924097537994385, 0.133490383625031);
				glVertex3f( 0.133490711450577, 1.924097537994385, 0.231212198734283);

			glNormal3f( 0.062807038426399, -0.970109641551971, 0.234398841857910 );
				glVertex3f( 0.100606292486191, 1.908206462860107, 0.174254953861237);
				glVertex3f( 0.133490711450577, 1.924097537994385, 0.231212198734283);
				glVertex3f( 0.000000040157680, 1.924097537994385, 0.266980946063995);

			glNormal3f( -0.062806934118271, -0.970109641551971, 0.234398439526558 );
				glVertex3f( 0.000000029807616, 1.908206462860107, 0.201212227344513);
				glVertex3f( 0.000000040157680, 1.924097537994385, 0.266980946063995);
				glVertex3f( -0.133490607142448, 1.924097537994385, 0.231212198734283);

			glNormal3f( -0.171591758728027, -0.970109522342682, 0.171591758728027 );
				glVertex3f( -0.174255132675171, 1.908206462860107, 0.100606039166451);
				glVertex3f( -0.100606217980385, 1.908206462860107, 0.174254953861237);
				glVertex3f( -0.133490607142448, 1.924097537994385, 0.231212198734283);

			glNormal3f( -0.234398260712624, -0.970109641551971, 0.062806919217110 );
				glVertex3f( -0.201212525367737, 1.908206462860107, -0.000000166820712);
				glVertex3f( -0.174255132675171, 1.908206462860107, 0.100606039166451);
				glVertex3f( -0.231212556362152, 1.924097537994385, 0.133490383625031);

			glNormal3f( -0.234398037195206, -0.970109701156616, -0.062806539237499 );
				glVertex3f( -0.174255236983299, 1.908206462860107, -0.100606486201286);
				glVertex3f( -0.201212525367737, 1.908206462860107, -0.000000166820712);
				glVertex3f( -0.266981363296509, 1.924097537994385, -0.000000168209951);

			glNormal3f( -0.171590983867645, -0.970109641551971, -0.171592056751251 );
				glVertex3f( -0.174255236983299, 1.908206462860107, -0.100606486201286);
				glVertex3f( -0.231212705373764, 1.924097537994385, -0.133490860462189);
				glVertex3f( -0.133490830659866, 1.924097657203674, -0.231212794780731);

			glNormal3f( -0.062806293368340, -0.970109105110168, -0.234400644898415 );
				glVertex3f( -0.000000000574626, 1.908206582069397, -0.201212823390961);
				glVertex3f( -0.100606389343739, 1.908206462860107, -0.174255430698395);
				glVertex3f( -0.133490830659866, 1.924097657203674, -0.231212794780731);

			glNormal3f( 0.133132234215736, 0.857560455799103, -0.496856003999710 );
				glVertex3f( 0.133490711450577, 1.924097537994385, -0.231212913990021);
				glVertex3f( -0.000000000155385, 1.924097537994385, -0.266981661319733);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( 0.363724559545517, 0.857560396194458, -0.363723427057266 );
				glVertex3f( 0.231212630867958, 1.924097418785095, -0.133490979671478);
				glVertex3f( 0.133490711450577, 1.924097537994385, -0.231212913990021);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( 0.496855944395065, 0.857560276985168, -0.133133172988892 );
				glVertex3f( 0.266981422901154, 1.924097537994385, -0.000000287419255);
				glVertex3f( 0.231212630867958, 1.924097418785095, -0.133490979671478);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( 0.496856153011322, 0.857560396194458, 0.133132487535477 );
				glVertex3f( 0.231212630867958, 1.924097537994385, 0.133490383625031);
				glVertex3f( 0.266981422901154, 1.924097537994385, -0.000000287419255);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( 0.363723933696747, 0.857560336589813, 0.363724291324615 );
				glVertex3f( 0.133490711450577, 1.924097537994385, 0.231212198734283);
				glVertex3f( 0.231212630867958, 1.924097537994385, 0.133490383625031);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( 0.133132383227348, 0.857560098171234, 0.496856421232224 );
				glVertex3f( 0.000000040157680, 1.924097537994385, 0.266980946063995);
				glVertex3f( 0.133490711450577, 1.924097537994385, 0.231212198734283);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( -0.133132413029671, 0.857560098171234, 0.496856480836868 );
				glVertex3f( -0.133490607142448, 1.924097537994385, 0.231212198734283);
				glVertex3f( 0.000000040157680, 1.924097537994385, 0.266980946063995);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( -0.363723933696747, 0.857560217380524, 0.363724410533905 );
				glVertex3f( -0.231212556362152, 1.924097537994385, 0.133490383625031);
				glVertex3f( -0.133490607142448, 1.924097537994385, 0.231212198734283);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( -0.496856093406677, 0.857560276985168, 0.133132651448250 );
				glVertex3f( -0.266981363296509, 1.924097537994385, -0.000000168209951);
				glVertex3f( -0.231212556362152, 1.924097537994385, 0.133490383625031);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( -0.496856153011322, 0.857560396194458, -0.133131951093674 );
				glVertex3f( -0.231212705373764, 1.924097537994385, -0.133490860462189);
				glVertex3f( -0.266981363296509, 1.924097537994385, -0.000000168209951);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( -0.363724410533905, 0.857560515403748, -0.363723129034042 );
				glVertex3f( -0.133490830659866, 1.924097657203674, -0.231212794780731);
				glVertex3f( -0.231212705373764, 1.924097537994385, -0.133490860462189);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( -0.133131787180901, 0.857560455799103, -0.496856003999710 );
				glVertex3f( -0.000000000155385, 1.924097537994385, -0.266981661319733);
				glVertex3f( -0.133490830659866, 1.924097657203674, -0.231212794780731);
				glVertex3f( -0.000000001512950, 2.078782081604004, -0.000000181732901);

			glNormal3f( 0.257686674594879, 0.093441843986511, -0.961699664592743 );
				glVertex3f( -0.000000000501916, 0.259942412376404, -0.294943302869797);
				glVertex3f( -0.000000000693916, 0.519884824752808, -0.269686460494995);
				glVertex3f( 0.134843185544014, 0.519884705543518, -0.233555346727371);

			glNormal3f( 0.257977426052094, 0.080573745071888, -0.962785363197327 );
				glVertex3f( -0.000000000874416, 0.779827117919922, -0.245857000350952);
				glVertex3f( -0.000000001047811, 1.039769649505615, -0.224102884531021);
				glVertex3f( 0.112051382660866, 1.039769530296326, -0.194078832864761);

			glNormal3f( 0.704013049602509, 0.093441843986511, -0.704013049602509 );
				glVertex3f( 0.147471606731415, 0.259942352771759, -0.255428403615952);
				glVertex3f( 0.134843185544014, 0.519884705543518, -0.233555346727371);
				glVertex3f( 0.233555242419243, 0.519884705543518, -0.134843289852142);

			glNormal3f( 0.704807877540588, 0.080573841929436, -0.704807519912720 );
				glVertex3f( 0.122928455471992, 0.779827117919922, -0.212918460369110);
				glVertex3f( 0.112051382660866, 1.039769530296326, -0.194078832864761);
				glVertex3f( 0.194078654050827, 1.039769530296326, -0.112051516771317);

			glNormal3f( 0.961699664592743, 0.093441843986511, -0.257686555385590 );
				glVertex3f( 0.233555242419243, 0.519884705543518, -0.134843289852142);
				glVertex3f( 0.269686371088028, 0.519884705543518, -0.000000045449763);
				glVertex3f( 0.294943213462830, 0.259942382574081, -0.000000022724885);

			glNormal3f( 0.962785184383392, 0.080573923885822, -0.257977694272995 );
				glVertex3f( 0.194078654050827, 1.039769530296326, -0.112051516771317);
				glVertex3f( 0.224102765321732, 1.039769530296326, -0.000000090899540);
				glVertex3f( 0.245856910943985, 0.779827117919922, -0.000000127779288);

			glNormal3f( 0.961699545383453, 0.093441888689995, 0.257686704397202 );
				glVertex3f( 0.294943213462830, 0.259942382574081, -0.000000022724885);
				glVertex3f( 0.269686371088028, 0.519884705543518, -0.000000045449763);
				glVertex3f( 0.233555242419243, 0.519884705543518, 0.134843111038208);

			glNormal3f( 0.962785243988037, 0.080573849380016, 0.257977843284607 );
				glVertex3f( 0.245856910943985, 0.779827117919922, -0.000000127779288);
				glVertex3f( 0.224102765321732, 1.039769530296326, -0.000000090899540);
				glVertex3f( 0.194078654050827, 1.039769530296326, 0.112051278352737);

			glNormal3f( 0.704012989997864, 0.093441896140575, 0.704012989997864 );
				glVertex3f( 0.233555242419243, 0.519884705543518, 0.134843111038208);
				glVertex3f( 0.134843215346336, 0.519884705543518, 0.233555138111115);
				glVertex3f( 0.147471621632576, 0.259942412376404, 0.255428224802017);

			glNormal3f( 0.704807341098785, 0.080574072897434, 0.704807996749878 );
				glVertex3f( 0.212918341159821, 0.779827117919922, 0.122928380966187);
				glVertex3f( 0.194078654050827, 1.039769530296326, 0.112051278352737);
				glVertex3f( 0.112051382660866, 1.039769530296326, 0.194078475236893);

			glNormal3f( 0.257686793804169, 0.093441940844059, 0.961699545383453 );
				glVertex3f( 0.147471621632576, 0.259942412376404, 0.255428224802017);
				glVertex3f( 0.134843215346336, 0.519884705543518, 0.233555138111115);
				glVertex3f( 0.000000040027594, 0.519884705543518, 0.269686281681061);

			glNormal3f( 0.257977366447449, 0.080574281513691, 0.962785243988037 );
				glVertex3f( 0.122928470373154, 0.779827117919922, 0.212918221950531);
				glVertex3f( 0.112051382660866, 1.039769530296326, 0.194078475236893);
				glVertex3f( 0.000000032790762, 1.039769530296326, 0.224102526903152);

			glNormal3f( -0.257686406373978, 0.093441881239414, 0.961699724197388 );
				glVertex3f( 0.000000044033275, 0.259942412376404, 0.294943124055862);
				glVertex3f( 0.000000040027594, 0.519884705543518, 0.269686281681061);
				glVertex3f( -0.134843140840530, 0.519884705543518, 0.233555197715759);

			glNormal3f( -0.257977366447449, 0.080574430525303, 0.962785243988037 );
				glVertex3f( 0.000000036248949, 0.779827117919922, 0.245856821537018);
				glVertex3f( 0.000000032790762, 1.039769530296326, 0.224102526903152);
				glVertex3f( -0.112051323056221, 1.039769530296326, 0.194078475236893);

			glNormal3f( -0.704012751579285, 0.093441896140575, 0.704013168811798 );
				glVertex3f( -0.147471547126770, 0.259942412376404, 0.255428284406662);
				glVertex3f( -0.134843140840530, 0.519884705543518, 0.233555197715759);
				glVertex3f( -0.233555227518082, 0.519884705543518, 0.134843170642853);

			glNormal3f( -0.704807281494141, 0.080574221909046, 0.704808175563812 );
				glVertex3f( -0.122928410768509, 0.779827117919922, 0.212918221950531);
				glVertex3f( -0.112051323056221, 1.039769530296326, 0.194078475236893);
				glVertex3f( -0.194078624248505, 1.039769530296326, 0.112051278352737);

			glNormal3f( -0.961699604988098, 0.093441806733608, 0.257686972618103 );
				glVertex3f( -0.255428254604340, 0.259942382574081, 0.147471666336060);
				glVertex3f( -0.233555227518082, 0.519884705543518, 0.134843170642853);
				glVertex3f( -0.269686371088028, 0.519884705543518, 0.000000073759523);

			glNormal3f( -0.962785124778748, 0.080573968589306, 0.257977813482285 );
				glVertex3f( -0.212918281555176, 0.779827117919922, 0.122928380966187);
				glVertex3f( -0.194078624248505, 1.039769530296326, 0.112051278352737);
				glVertex3f( -0.224102735519409, 1.039769530296326, -0.000000090899540);

			glNormal3f( -0.961699783802032, 0.093441851437092, -0.257686078548431 );
				glVertex3f( -0.294943213462830, 0.259942382574081, 0.000000096484399);
				glVertex3f( -0.269686371088028, 0.519884705543518, 0.000000073759523);
				glVertex3f( -0.233555331826210, 0.519884705543518, -0.134843111038208);

			glNormal3f( -0.962785422801971, 0.080573871731758, -0.257976889610291 );
				glVertex3f( -0.245856896042824, 0.779827117919922, -0.000000008570005);
				glVertex3f( -0.224102735519409, 1.039769530296326, -0.000000090899540);
				glVertex3f( -0.194078758358955, 1.039769530296326, -0.112051397562027);

			glNormal3f( -0.704013645648956, 0.093441806733608, -0.704012453556061 );
				glVertex3f( -0.255428403615952, 0.259942412376404, -0.147471487522125);
				glVertex3f( -0.233555331826210, 0.519884705543518, -0.134843111038208);
				glVertex3f( -0.134843349456787, 0.519884705543518, -0.233555257320404);

			glNormal3f( -0.704808294773102, 0.080573782324791, -0.704807162284851 );
				glVertex3f( -0.212918385863304, 0.779827117919922, -0.122928440570831);
				glVertex3f( -0.194078758358955, 1.039769530296326, -0.112051397562027);
				glVertex3f( -0.112051516771317, 1.039769530296326, -0.194078773260117);

			glNormal3f( -0.257686942815781, 0.093441821634769, -0.961699485778809 );
				glVertex3f( -0.134843349456787, 0.519884705543518, -0.233555257320404);
				glVertex3f( -0.000000000693916, 0.519884824752808, -0.269686460494995);
				glVertex3f( -0.000000000501916, 0.259942412376404, -0.294943302869797);

			glNormal3f( -0.257977634668350, 0.080573745071888, -0.962785303592682 );
				glVertex3f( -0.112051516771317, 1.039769530296326, -0.194078773260117);
				glVertex3f( -0.000000001047811, 1.039769649505615, -0.224102884531021);
				glVertex3f( -0.000000000874416, 0.779827117919922, -0.245857000350952);

			glNormal3f( 0.258337020874023, 0.061050180345774, -0.964123904705048 );
				glVertex3f( 0.102435886859894, 1.299711942672729, -0.177424311637878);
				glVertex3f( -0.000000001221039, 1.299711942672729, -0.204872012138367);
				glVertex3f( 0.094205856323242, 1.559654235839844, -0.163169503211975);

			glNormal3f( 0.705787718296051, 0.061050184071064, -0.705787837505341 );
				glVertex3f( 0.102435886859894, 1.299711942672729, -0.177424311637878);
				glVertex3f( 0.094205856323242, 1.559654235839844, -0.163169503211975);
				glVertex3f( 0.177424147725105, 1.299711942672729, -0.102436065673828);

			glNormal3f( 0.964124023914337, 0.061050217598677, -0.258336514234543 );
				glVertex3f( 0.204871773719788, 1.299711942672729, -0.000000232833713);
				glVertex3f( 0.177424147725105, 1.299711942672729, -0.102436065673828);
				glVertex3f( 0.188411712646484, 1.559654235839844, -0.000000255558575);

			glNormal3f( 0.964124023914337, 0.061050288379192, 0.258336246013641 );
				glVertex3f( 0.177424147725105, 1.299711942672729, 0.102435708045959);
				glVertex3f( 0.204871773719788, 1.299711942672729, -0.000000232833713);
				glVertex3f( 0.163169324398041, 1.559654235839844, 0.094205603003502);

			glNormal3f( 0.705787837505341, 0.061050150543451, 0.705787837505341 );
				glVertex3f( 0.102435901761055, 1.299711942672729, 0.177423954010010);
				glVertex3f( 0.177424147725105, 1.299711942672729, 0.102435708045959);
				glVertex3f( 0.094205886125565, 1.559654235839844, 0.163169145584106);

			glNormal3f( 0.258336037397385, 0.061050251126289, 0.964124143123627 );
				glVertex3f( 0.000000029713739, 1.299711942672729, 0.204871535301208);
				glVertex3f( 0.102435901761055, 1.299711942672729, 0.177423954010010);
				glVertex3f( 0.000000027045798, 1.559654235839844, 0.188411474227905);

			glNormal3f( -0.258336037397385, 0.061050154268742, 0.964124143123627 );
				glVertex3f( -0.102435842156410, 1.299711942672729, 0.177423954010010);
				glVertex3f( 0.000000029713739, 1.299711942672729, 0.204871535301208);
				glVertex3f( -0.094205811619759, 1.559654235839844, 0.163169145584106);

			glNormal3f( -0.705787718296051, 0.061050072312355, 0.705787837505341 );
				glVertex3f( -0.177424103021622, 1.299711942672729, 0.102435708045959);
				glVertex3f( -0.102435842156410, 1.299711942672729, 0.177423954010010);
				glVertex3f( -0.163169294595718, 1.559654235839844, 0.094205722212791);

			glNormal3f( -0.964123964309692, 0.061050206422806, 0.258336812257767 );
				glVertex3f( -0.204871758818626, 1.299711942672729, -0.000000113624424);
				glVertex3f( -0.177424103021622, 1.299711942672729, 0.102435708045959);
				glVertex3f( -0.188411712646484, 1.559654235839844, -0.000000136349300);

			glNormal3f( -0.964124202728271, 0.061050131917000, -0.258335709571838 );
				glVertex3f( -0.177424222230911, 1.299711942672729, -0.102435946464539);
				glVertex3f( -0.204871758818626, 1.299711942672729, -0.000000113624424);
				glVertex3f( -0.163169398903847, 1.559654235839844, -0.094205990433693);

			glNormal3f( -0.705788552761078, 0.061050187796354, -0.705787062644958 );
				glVertex3f( -0.102436020970345, 1.299711942672729, -0.177424311637878);
				glVertex3f( -0.177424222230911, 1.299711942672729, -0.102435946464539);
				glVertex3f( -0.094205982983112, 1.559654235839844, -0.163169503211975);

			glNormal3f( -0.258336693048477, 0.061050236225128, -0.964123964309692 );
				glVertex3f( -0.000000001221039, 1.299711942672729, -0.204872012138367);
				glVertex3f( -0.102436020970345, 1.299711942672729, -0.177424311637878);
				glVertex3f( -0.000000001403581, 1.559654235839844, -0.188411951065063);

			glNormal3f( 0.062805667519569, -0.970109522342682, -0.234399080276489 );
				glVertex3f( 0.100606277585030, 1.908206462860107, -0.174255549907684);
				glVertex3f( -0.000000000574626, 1.908206582069397, -0.201212823390961);
				glVertex3f( 0.133490711450577, 1.924097537994385, -0.231212913990021);

			glNormal3f( 0.171590134501457, -0.970110237598419, -0.171589702367783 );
				glVertex3f( 0.174255162477493, 1.908206462860107, -0.100606486201286);
				glVertex3f( 0.100606277585030, 1.908206462860107, -0.174255549907684);
				glVertex3f( 0.231212630867958, 1.924097418785095, -0.133490979671478);

			glNormal3f( 0.234397947788239, -0.970109820365906, -0.062806002795696 );
				glVertex3f( 0.266981422901154, 1.924097537994385, -0.000000287419255);
				glVertex3f( 0.201212555170059, 1.908206462860107, -0.000000286030001);
				glVertex3f( 0.231212630867958, 1.924097418785095, -0.133490979671478);

			glNormal3f( 0.234398156404495, -0.970109701156616, 0.062806814908981 );
				glVertex3f( 0.174255162477493, 1.908206462860107, 0.100606039166451);
				glVertex3f( 0.201212555170059, 1.908206462860107, -0.000000286030001);
				glVertex3f( 0.231212630867958, 1.924097537994385, 0.133490383625031);

			glNormal3f( 0.171591788530350, -0.970109581947327, 0.171591684222221 );
				glVertex3f( 0.100606292486191, 1.908206462860107, 0.174254953861237);
				glVertex3f( 0.174255162477493, 1.908206462860107, 0.100606039166451);
				glVertex3f( 0.133490711450577, 1.924097537994385, 0.231212198734283);

			glNormal3f( 0.062806658446789, -0.970109701156616, 0.234398439526558 );
				glVertex3f( 0.000000029807616, 1.908206462860107, 0.201212227344513);
				glVertex3f( 0.100606292486191, 1.908206462860107, 0.174254953861237);
				glVertex3f( 0.000000040157680, 1.924097537994385, 0.266980946063995);

			glNormal3f( -0.062806800007820, -0.970109462738037, 0.234398946166039 );
				glVertex3f( -0.100606217980385, 1.908206462860107, 0.174254953861237);
				glVertex3f( 0.000000029807616, 1.908206462860107, 0.201212227344513);
				glVertex3f( -0.133490607142448, 1.924097537994385, 0.231212198734283);

			glNormal3f( -0.171591416001320, -0.970109581947327, 0.171591654419899 );
				glVertex3f( -0.231212556362152, 1.924097537994385, 0.133490383625031);
				glVertex3f( -0.174255132675171, 1.908206462860107, 0.100606039166451);
				glVertex3f( -0.133490607142448, 1.924097537994385, 0.231212198734283);

			glNormal3f( -0.234398022294044, -0.970109641551971, 0.062806971371174 );
				glVertex3f( -0.266981363296509, 1.924097537994385, -0.000000168209951);
				glVertex3f( -0.201212525367737, 1.908206462860107, -0.000000166820712);
				glVertex3f( -0.231212556362152, 1.924097537994385, 0.133490383625031);

			glNormal3f( -0.234398171305656, -0.970109641551971, -0.062806688249111 );
				glVertex3f( -0.231212705373764, 1.924097537994385, -0.133490860462189);
				glVertex3f( -0.174255236983299, 1.908206462860107, -0.100606486201286);
				glVertex3f( -0.266981363296509, 1.924097537994385, -0.000000168209951);

			glNormal3f( -0.171592786908150, -0.970109224319458, -0.171592563390732 );
				glVertex3f( -0.100606389343739, 1.908206462860107, -0.174255430698395);
				glVertex3f( -0.174255236983299, 1.908206462860107, -0.100606486201286);
				glVertex3f( -0.133490830659866, 1.924097657203674, -0.231212794780731);

			glNormal3f( -0.062807373702526, -0.970110177993774, -0.234396383166313 );
				glVertex3f( -0.000000000155385, 1.924097537994385, -0.266981661319733);
				glVertex3f( -0.000000000574626, 1.908206582069397, -0.201212823390961);
				glVertex3f( -0.133490830659866, 1.924097657203674, -0.231212794780731);

			glNormal3f( 0.257686674594879, 0.093441821634769, -0.961699604988098 );
				glVertex3f( 0.147471606731415, 0.259942352771759, -0.255428403615952);
				glVertex3f( -0.000000000501916, 0.259942412376404, -0.294943302869797);
				glVertex3f( 0.134843185544014, 0.519884705543518, -0.233555346727371);

			glNormal3f( 0.257977217435837, 0.080573819577694, -0.962785303592682 );
				glVertex3f( 0.122928455471992, 0.779827117919922, -0.212918460369110);
				glVertex3f( -0.000000000874416, 0.779827117919922, -0.245857000350952);
				glVertex3f( 0.112051382660866, 1.039769530296326, -0.194078832864761);

			glNormal3f( 0.704013109207153, 0.093441732227802, -0.704012870788574 );
				glVertex3f( 0.255428314208984, 0.259942382574081, -0.147471651434898);
				glVertex3f( 0.147471606731415, 0.259942352771759, -0.255428403615952);
				glVertex3f( 0.233555242419243, 0.519884705543518, -0.134843289852142);

			glNormal3f( 0.704807817935944, 0.080573946237564, -0.704807698726654 );
				glVertex3f( 0.212918341159821, 0.779827117919922, -0.122928559780121);
				glVertex3f( 0.122928455471992, 0.779827117919922, -0.212918460369110);
				glVertex3f( 0.194078654050827, 1.039769530296326, -0.112051516771317);

			glNormal3f( 0.961699604988098, 0.093441814184189, -0.257686585187912 );
				glVertex3f( 0.255428314208984, 0.259942382574081, -0.147471651434898);
				glVertex3f( 0.233555242419243, 0.519884705543518, -0.134843289852142);
				glVertex3f( 0.294943213462830, 0.259942382574081, -0.000000022724885);

			glNormal3f( 0.962785184383392, 0.080574005842209, -0.257977455854416 );
				glVertex3f( 0.212918341159821, 0.779827117919922, -0.122928559780121);
				glVertex3f( 0.194078654050827, 1.039769530296326, -0.112051516771317);
				glVertex3f( 0.245856910943985, 0.779827117919922, -0.000000127779288);

			glNormal3f( 0.961699664592743, 0.093441925942898, 0.257686644792557 );
				glVertex3f( 0.255428314208984, 0.259942382574081, 0.147471576929092);
				glVertex3f( 0.294943213462830, 0.259942382574081, -0.000000022724885);
				glVertex3f( 0.233555242419243, 0.519884705543518, 0.134843111038208);

			glNormal3f( 0.962785303592682, 0.080574080348015, 0.257977336645126 );
				glVertex3f( 0.212918341159821, 0.779827117919922, 0.122928380966187);
				glVertex3f( 0.245856910943985, 0.779827117919922, -0.000000127779288);
				glVertex3f( 0.194078654050827, 1.039769530296326, 0.112051278352737);

			glNormal3f( 0.704012930393219, 0.093442007899284, 0.704013168811798 );
				glVertex3f( 0.255428314208984, 0.259942382574081, 0.147471576929092);
				glVertex3f( 0.233555242419243, 0.519884705543518, 0.134843111038208);
				glVertex3f( 0.147471621632576, 0.259942412376404, 0.255428224802017);

			glNormal3f( 0.704807579517365, 0.080574207007885, 0.704807817935944 );
				glVertex3f( 0.122928470373154, 0.779827117919922, 0.212918221950531);
				glVertex3f( 0.212918341159821, 0.779827117919922, 0.122928380966187);
				glVertex3f( 0.112051382660866, 1.039769530296326, 0.194078475236893);

			glNormal3f( 0.257686674594879, 0.093441866338253, 0.961699604988098 );
				glVertex3f( 0.000000044033275, 0.259942412376404, 0.294943124055862);
				glVertex3f( 0.147471621632576, 0.259942412376404, 0.255428224802017);
				glVertex3f( 0.000000040027594, 0.519884705543518, 0.269686281681061);

			glNormal3f( 0.257977694272995, 0.080574430525303, 0.962785124778748 );
				glVertex3f( 0.000000036248949, 0.779827117919922, 0.245856821537018);
				glVertex3f( 0.122928470373154, 0.779827117919922, 0.212918221950531);
				glVertex3f( 0.000000032790762, 1.039769530296326, 0.224102526903152);

			glNormal3f( -0.257686287164688, 0.093441918492317, 0.961699664592743 );
				glVertex3f( -0.147471547126770, 0.259942412376404, 0.255428284406662);
				glVertex3f( 0.000000044033275, 0.259942412376404, 0.294943124055862);
				glVertex3f( -0.134843140840530, 0.519884705543518, 0.233555197715759);

			glNormal3f( -0.257977694272995, 0.080574296414852, 0.962785243988037 );
				glVertex3f( -0.122928410768509, 0.779827117919922, 0.212918221950531);
				glVertex3f( 0.000000036248949, 0.779827117919922, 0.245856821537018);
				glVertex3f( -0.112051323056221, 1.039769530296326, 0.194078475236893);

			glNormal3f( -0.704012751579285, 0.093441963195801, 0.704013288021088 );
				glVertex3f( -0.255428254604340, 0.259942382574081, 0.147471666336060);
				glVertex3f( -0.147471547126770, 0.259942412376404, 0.255428284406662);
				glVertex3f( -0.233555227518082, 0.519884705543518, 0.134843170642853);

			glNormal3f( -0.704807579517365, 0.080573998391628, 0.704807817935944 );
				glVertex3f( -0.212918281555176, 0.779827117919922, 0.122928380966187);
				glVertex3f( -0.122928410768509, 0.779827117919922, 0.212918221950531);
				glVertex3f( -0.194078624248505, 1.039769530296326, 0.112051278352737);

			glNormal3f( -0.961699545383453, 0.093441866338253, 0.257687032222748 );
				glVertex3f( -0.294943213462830, 0.259942382574081, 0.000000096484399);
				glVertex3f( -0.255428254604340, 0.259942382574081, 0.147471666336060);
				glVertex3f( -0.269686371088028, 0.519884705543518, 0.000000073759523);

			glNormal3f( -0.962785065174103, 0.080574005842209, 0.257977873086929 );
				glVertex3f( -0.245856896042824, 0.779827117919922, -0.000000008570005);
				glVertex3f( -0.212918281555176, 0.779827117919922, 0.122928380966187);
				glVertex3f( -0.224102735519409, 1.039769530296326, -0.000000090899540);

			glNormal3f( -0.961699783802032, 0.093441843986511, -0.257686138153076 );
				glVertex3f( -0.255428403615952, 0.259942412376404, -0.147471487522125);
				glVertex3f( -0.294943213462830, 0.259942382574081, 0.000000096484399);
				glVertex3f( -0.233555331826210, 0.519884705543518, -0.134843111038208);

			glNormal3f( -0.962785303592682, 0.080573782324791, -0.257977038621902 );
				glVertex3f( -0.212918385863304, 0.779827117919922, -0.122928440570831);
				glVertex3f( -0.245856896042824, 0.779827117919922, -0.000000008570005);
				glVertex3f( -0.194078758358955, 1.039769530296326, -0.112051397562027);

			glNormal3f( -0.704013466835022, 0.093441709876060, -0.704012572765350 );
				glVertex3f( -0.147471785545349, 0.259942322969437, -0.255428254604340);
				glVertex3f( -0.255428403615952, 0.259942412376404, -0.147471487522125);
				glVertex3f( -0.134843349456787, 0.519884705543518, -0.233555257320404);

			glNormal3f( -0.704808115959167, 0.080573715269566, -0.704807221889496 );
				glVertex3f( -0.122928604483604, 0.779827117919922, -0.212918341159821);
				glVertex3f( -0.212918385863304, 0.779827117919922, -0.122928440570831);
				glVertex3f( -0.112051516771317, 1.039769530296326, -0.194078773260117);

			glNormal3f( -0.257687330245972, 0.093441620469093, -0.961699545383453 );
				glVertex3f( -0.147471785545349, 0.259942322969437, -0.255428254604340);
				glVertex3f( -0.134843349456787, 0.519884705543518, -0.233555257320404);
				glVertex3f( -0.000000000501916, 0.259942412376404, -0.294943302869797);

			glNormal3f( -0.257977813482285, 0.080573625862598, -0.962785124778748 );
				glVertex3f( -0.122928604483604, 0.779827117919922, -0.212918341159821);
				glVertex3f( -0.112051516771317, 1.039769530296326, -0.194078773260117);
				glVertex3f( -0.000000000874416, 0.779827117919922, -0.245857000350952);

		// green
		SetMaterial( 0, 1, 0, 0.);
			glNormal3f( 0.257634520530701, 0.095565333962440, -0.961504936218262 );
				glVertex3f( -0.000000000296557, -0.000000057845746, -0.320779353380203);
				glVertex3f( -0.000000000501916, 0.259942412376404, -0.294943302869797);
				glVertex3f( 0.147471606731415, 0.259942352771759, -0.255428403615952);

			glNormal3f( 0.257810026407242, 0.088203355669975, -0.962161242961884 );
				glVertex3f( -0.000000000693916, 0.519884824752808, -0.269686460494995);
				glVertex3f( -0.000000000874416, 0.779827117919922, -0.245857000350952);
				glVertex3f( 0.122928455471992, 0.779827117919922, -0.212918460369110);

			glNormal3f( 0.258161485195160, 0.071278676390648, -0.963468730449677 );
				glVertex3f( -0.000000001047811, 1.039769649505615, -0.224102884531021);
				glVertex3f( -0.000000001221039, 1.299711942672729, -0.204872012138367);
				glVertex3f( 0.102435886859894, 1.299711942672729, -0.177424311637878);

			glNormal3f( 0.703870594501495, 0.095565274357796, -0.703870296478271 );
				glVertex3f( 0.160389676690102, -0.000000054088630, -0.277803003787994);
				glVertex3f( 0.147471606731415, 0.259942352771759, -0.255428403615952);
				glVertex3f( 0.255428314208984, 0.259942382574081, -0.147471651434898);

			glNormal3f( 0.704350829124451, 0.088203214108944, -0.704350709915161 );
				glVertex3f( 0.134843185544014, 0.519884705543518, -0.233555346727371);
				glVertex3f( 0.122928455471992, 0.779827117919922, -0.212918460369110);
				glVertex3f( 0.212918341159821, 0.779827117919922, -0.122928559780121);

			glNormal3f( 0.705308139324188, 0.071279093623161, -0.705308258533478 );
				glVertex3f( 0.112051382660866, 1.039769530296326, -0.194078832864761);
				glVertex3f( 0.102435886859894, 1.299711942672729, -0.177424311637878);
				glVertex3f( 0.177424147725105, 1.299711942672729, -0.102436065673828);

			glNormal3f( 0.961504876613617, 0.095565594732761, -0.257634460926056 );
				glVertex3f( 0.277803063392639, -0.000000028922869, -0.160389646887779);
				glVertex3f( 0.255428314208984, 0.259942382574081, -0.147471651434898);
				glVertex3f( 0.294943213462830, 0.259942382574081, -0.000000022724885);

			glNormal3f( 0.962161183357239, 0.088203243911266, -0.257810264825821 );
				glVertex3f( 0.233555242419243, 0.519884705543518, -0.134843289852142);
				glVertex3f( 0.212918341159821, 0.779827117919922, -0.122928559780121);
				glVertex3f( 0.245856910943985, 0.779827117919922, -0.000000127779288);

			glNormal3f( 0.963468849658966, 0.071278989315033, -0.258160948753357 );
				glVertex3f( 0.194078654050827, 1.039769530296326, -0.112051516771317);
				glVertex3f( 0.177424147725105, 1.299711942672729, -0.102436065673828);
				glVertex3f( 0.204871773719788, 1.299711942672729, -0.000000232833713);

			glNormal3f( 0.961504936218262, 0.095565736293793, 0.257634460926056 );
				glVertex3f( 0.320779353380203, 0.000000000000003, 0.000000018173509);
				glVertex3f( 0.294943213462830, 0.259942382574081, -0.000000022724885);
				glVertex3f( 0.255428314208984, 0.259942382574081, 0.147471576929092);

			glNormal3f( 0.962161123752594, 0.088203385472298, 0.257810086011887 );
				glVertex3f( 0.269686371088028, 0.519884705543518, -0.000000045449763);
				glVertex3f( 0.245856910943985, 0.779827117919922, -0.000000127779288);
				glVertex3f( 0.212918341159821, 0.779827117919922, 0.122928380966187);

			glNormal3f( 0.963468849658966, 0.071279093623161, 0.258160680532455 );
				glVertex3f( 0.204871773719788, 1.299711942672729, -0.000000232833713);
				glVertex3f( 0.177424147725105, 1.299711942672729, 0.102435708045959);
				glVertex3f( 0.194078654050827, 1.039769530296326, 0.112051278352737);

			glNormal3f( 0.703870296478271, 0.095565780997276, 0.703870594501495 );
				glVertex3f( 0.277803063392639, 0.000000028922873, 0.160389676690102);
				glVertex3f( 0.255428314208984, 0.259942382574081, 0.147471576929092);
				glVertex3f( 0.147471621632576, 0.259942412376404, 0.255428224802017);

			glNormal3f( 0.704350709915161, 0.088203348219395, 0.704350948333740 );
				glVertex3f( 0.212918341159821, 0.779827117919922, 0.122928380966187);
				glVertex3f( 0.122928470373154, 0.779827117919922, 0.212918221950531);
				glVertex3f( 0.134843215346336, 0.519884705543518, 0.233555138111115);

			glNormal3f( 0.705308139324188, 0.071279242634773, 0.705308139324188 );
				glVertex3f( 0.194078654050827, 1.039769530296326, 0.112051278352737);
				glVertex3f( 0.177424147725105, 1.299711942672729, 0.102435708045959);
				glVertex3f( 0.102435901761055, 1.299711942672729, 0.177423954010010);

			glNormal3f( 0.257634520530701, 0.095566011965275, 0.961504936218262 );
				glVertex3f( 0.160389676690102, 0.000000054088638, 0.277803063392639);
				glVertex3f( 0.147471621632576, 0.259942412376404, 0.255428224802017);
				glVertex3f( 0.000000044033275, 0.259942412376404, 0.294943124055862);

			glNormal3f( 0.257810473442078, 0.088203310966492, 0.962161064147949 );
				glVertex3f( 0.134843215346336, 0.519884705543518, 0.233555138111115);
				glVertex3f( 0.122928470373154, 0.779827117919922, 0.212918221950531);
				glVertex3f( 0.000000036248949, 0.779827117919922, 0.245856821537018);

			glNormal3f( 0.258160471916199, 0.071279063820839, 0.963468968868256 );
				glVertex3f( 0.112051382660866, 1.039769530296326, 0.194078475236893);
				glVertex3f( 0.102435901761055, 1.299711942672729, 0.177423954010010);
				glVertex3f( 0.000000029713739, 1.299711942672729, 0.204871535301208);

			glNormal3f( -0.257634133100510, 0.095566041767597, 0.961504995822906 );
				glVertex3f( 0.000000048139775, 0.000000057845746, 0.320779353380203);
				glVertex3f( 0.000000044033275, 0.259942412376404, 0.294943124055862);
				glVertex3f( -0.147471547126770, 0.259942412376404, 0.255428284406662);

			glNormal3f( -0.257810473442078, 0.088203296065331, 0.962161123752594 );
				glVertex3f( 0.000000040027594, 0.519884705543518, 0.269686281681061);
				glVertex3f( 0.000000036248949, 0.779827117919922, 0.245856821537018);
				glVertex3f( -0.122928410768509, 0.779827117919922, 0.212918221950531);

			glNormal3f( -0.258160471916199, 0.071279108524323, 0.963468968868256 );
				glVertex3f( 0.000000032790762, 1.039769530296326, 0.224102526903152);
				glVertex3f( 0.000000029713739, 1.299711942672729, 0.204871535301208);
				glVertex3f( -0.102435842156410, 1.299711942672729, 0.177423954010010);

			glNormal3f( -0.703870117664337, 0.095565706491470, 0.703870713710785 );
				glVertex3f( -0.160389587283134, 0.000000054088638, 0.277803063392639);
				glVertex3f( -0.147471547126770, 0.259942412376404, 0.255428284406662);
				glVertex3f( -0.255428254604340, 0.259942382574081, 0.147471666336060);

			glNormal3f( -0.704350709915161, 0.088203467428684, 0.704350948333740 );
				glVertex3f( -0.134843140840530, 0.519884705543518, 0.233555197715759);
				glVertex3f( -0.122928410768509, 0.779827117919922, 0.212918221950531);
				glVertex3f( -0.212918281555176, 0.779827117919922, 0.122928380966187);

			glNormal3f( -0.705308139324188, 0.071279048919678, 0.705308258533478 );
				glVertex3f( -0.112051323056221, 1.039769530296326, 0.194078475236893);
				glVertex3f( -0.102435842156410, 1.299711942672729, 0.177423954010010);
				glVertex3f( -0.177424103021622, 1.299711942672729, 0.102435708045959);

			glNormal3f( -0.961504817008972, 0.095565743744373, 0.257634878158569 );
				glVertex3f( -0.255428254604340, 0.259942382574081, 0.147471666336060);
				glVertex3f( -0.294943213462830, 0.259942382574081, 0.000000096484399);
				glVertex3f( -0.320779353380203, 0.000000000000028, 0.000000153286052);

			glNormal3f( -0.962161004543304, 0.088203430175781, 0.257810652256012 );
				glVertex3f( -0.212918281555176, 0.779827117919922, 0.122928380966187);
				glVertex3f( -0.245856896042824, 0.779827117919922, -0.000000008570005);
				glVertex3f( -0.269686371088028, 0.519884705543518, 0.000000073759523);

			glNormal3f( -0.963468730449677, 0.071279168128967, 0.258161246776581 );
				glVertex3f( -0.194078624248505, 1.039769530296326, 0.112051278352737);
				glVertex3f( -0.177424103021622, 1.299711942672729, 0.102435708045959);
				glVertex3f( -0.204871758818626, 1.299711942672729, -0.000000113624424);

			glNormal3f( -0.961504995822906, 0.095565654337406, -0.257633954286575 );
				glVertex3f( -0.320779353380203, 0.000000000000028, 0.000000153286052);
				glVertex3f( -0.294943213462830, 0.259942382574081, 0.000000096484399);
				glVertex3f( -0.255428403615952, 0.259942412376404, -0.147471487522125);

			glNormal3f( -0.962161183357239, 0.088203281164169, -0.257809817790985 );
				glVertex3f( -0.269686371088028, 0.519884705543518, 0.000000073759523);
				glVertex3f( -0.245856896042824, 0.779827117919922, -0.000000008570005);
				glVertex3f( -0.212918385863304, 0.779827117919922, -0.122928440570831);

			glNormal3f( -0.963469088077545, 0.071279041469097, -0.258160173892975 );
				glVertex3f( -0.224102735519409, 1.039769530296326, -0.000000090899540);
				glVertex3f( -0.204871758818626, 1.299711942672729, -0.000000113624424);
				glVertex3f( -0.177424222230911, 1.299711942672729, -0.102435946464539);

			glNormal3f( -0.703870892524719, 0.095565646886826, -0.703870058059692 );
				glVertex3f( -0.277803182601929, -0.000000028922859, -0.160389512777328);
				glVertex3f( -0.255428403615952, 0.259942412376404, -0.147471487522125);
				glVertex3f( -0.147471785545349, 0.259942322969437, -0.255428254604340);

			glNormal3f( -0.704351246356964, 0.088203236460686, -0.704350352287292 );
				glVertex3f( -0.233555331826210, 0.519884705543518, -0.134843111038208);
				glVertex3f( -0.212918385863304, 0.779827117919922, -0.122928440570831);
				glVertex3f( -0.122928604483604, 0.779827117919922, -0.212918341159821);

			glNormal3f( -0.705308973789215, 0.071279034018517, -0.705307424068451 );
				glVertex3f( -0.194078758358955, 1.039769530296326, -0.112051397562027);
				glVertex3f( -0.177424222230911, 1.299711942672729, -0.102435946464539);
				glVertex3f( -0.102436020970345, 1.299711942672729, -0.177424311637878);

			glNormal3f( -0.257635146379471, 0.095565326511860, -0.961504817008972 );
				glVertex3f( -0.147471785545349, 0.259942322969437, -0.255428254604340);
				glVertex3f( -0.000000000501916, 0.259942412376404, -0.294943302869797);
				glVertex3f( -0.000000000296557, -0.000000057845746, -0.320779353380203);

			glNormal3f( -0.257810592651367, 0.088203340768814, -0.962161123752594 );
				glVertex3f( -0.122928604483604, 0.779827117919922, -0.212918341159821);
				glVertex3f( -0.000000000874416, 0.779827117919922, -0.245857000350952);
				glVertex3f( -0.000000000693916, 0.519884824752808, -0.269686460494995);

			glNormal3f( -0.258161157369614, 0.071278683841228, -0.963468790054321 );
				glVertex3f( -0.102436020970345, 1.299711942672729, -0.177424311637878);
				glVertex3f( -0.000000001221039, 1.299711942672729, -0.204872012138367);
				glVertex3f( -0.000000001047811, 1.039769649505615, -0.224102884531021);

			glNormal3f( 0.257634848356247, 0.095565132796764, -0.961504876613617 );
				glVertex3f( 0.160389676690102, -0.000000054088630, -0.277803003787994);
				glVertex3f( -0.000000000296557, -0.000000057845746, -0.320779353380203);
				glVertex3f( 0.147471606731415, 0.259942352771759, -0.255428403615952);

			glNormal3f( 0.257810324430466, 0.088203199207783, -0.962161183357239 );
				glVertex3f( 0.134843185544014, 0.519884705543518, -0.233555346727371);
				glVertex3f( -0.000000000693916, 0.519884824752808, -0.269686460494995);
				glVertex3f( 0.122928455471992, 0.779827117919922, -0.212918460369110);

			glNormal3f( 0.258160561323166, 0.071279086172581, -0.963468909263611 );
				glVertex3f( 0.112051382660866, 1.039769530296326, -0.194078832864761);
				glVertex3f( -0.000000001047811, 1.039769649505615, -0.224102884531021);
				glVertex3f( 0.102435886859894, 1.299711942672729, -0.177424311637878);

			glNormal3f( 0.703870356082916, 0.095565460622311, -0.703870534896851 );
				glVertex3f( 0.277803063392639, -0.000000028922869, -0.160389646887779);
				glVertex3f( 0.160389676690102, -0.000000054088630, -0.277803003787994);
				glVertex3f( 0.255428314208984, 0.259942382574081, -0.147471651434898);

			glNormal3f( 0.704350829124451, 0.088203266263008, -0.704350829124451 );
				glVertex3f( 0.233555242419243, 0.519884705543518, -0.134843289852142);
				glVertex3f( 0.134843185544014, 0.519884705543518, -0.233555346727371);
				glVertex3f( 0.212918341159821, 0.779827117919922, -0.122928559780121);

			glNormal3f( 0.705308377742767, 0.071278929710388, -0.705308020114899 );
				glVertex3f( 0.194078654050827, 1.039769530296326, -0.112051516771317);
				glVertex3f( 0.112051382660866, 1.039769530296326, -0.194078832864761);
				glVertex3f( 0.177424147725105, 1.299711942672729, -0.102436065673828);

			glNormal3f( 0.961504876613617, 0.095565654337406, -0.257634520530701 );
				glVertex3f( 0.320779353380203, 0.000000000000003, 0.000000018173509);
				glVertex3f( 0.277803063392639, -0.000000028922869, -0.160389646887779);
				glVertex3f( 0.294943213462830, 0.259942382574081, -0.000000022724885);

			glNormal3f( 0.962161183357239, 0.088203221559525, -0.257810235023499 );
				glVertex3f( 0.269686371088028, 0.519884705543518, -0.000000045449763);
				glVertex3f( 0.233555242419243, 0.519884705543518, -0.134843289852142);
				glVertex3f( 0.245856910943985, 0.779827117919922, -0.000000127779288);

			glNormal3f( 0.963468968868256, 0.071278974413872, -0.258160918951035 );
				glVertex3f( 0.224102765321732, 1.039769530296326, -0.000000090899540);
				glVertex3f( 0.194078654050827, 1.039769530296326, -0.112051516771317);
				glVertex3f( 0.204871773719788, 1.299711942672729, -0.000000232833713);

			glNormal3f( 0.961504817008972, 0.095565728843212, 0.257634490728378 );
				glVertex3f( 0.277803063392639, 0.000000028922873, 0.160389676690102);
				glVertex3f( 0.320779353380203, 0.000000000000003, 0.000000018173509);
				glVertex3f( 0.255428314208984, 0.259942382574081, 0.147471576929092);

			glNormal3f( 0.962161123752594, 0.088203258812428, 0.257810413837433 );
				glVertex3f( 0.233555242419243, 0.519884705543518, 0.134843111038208);
				glVertex3f( 0.269686371088028, 0.519884705543518, -0.000000045449763);
				glVertex3f( 0.212918341159821, 0.779827117919922, 0.122928380966187);

			glNormal3f( 0.963468790054321, 0.071279235184193, 0.258161008358002 );
				glVertex3f( 0.224102765321732, 1.039769530296326, -0.000000090899540);
				glVertex3f( 0.204871773719788, 1.299711942672729, -0.000000232833713);
				glVertex3f( 0.194078654050827, 1.039769530296326, 0.112051278352737);

			glNormal3f( 0.703870415687561, 0.095565900206566, 0.703870415687561 );
				glVertex3f( 0.160389676690102, 0.000000054088638, 0.277803063392639);
				glVertex3f( 0.277803063392639, 0.000000028922873, 0.160389676690102);
				glVertex3f( 0.147471621632576, 0.259942412376404, 0.255428224802017);

			glNormal3f( 0.704350948333740, 0.088203266263008, 0.704350888729095 );
				glVertex3f( 0.233555242419243, 0.519884705543518, 0.134843111038208);
				glVertex3f( 0.212918341159821, 0.779827117919922, 0.122928380966187);
				glVertex3f( 0.134843215346336, 0.519884705543518, 0.233555138111115);

			glNormal3f( 0.705307841300964, 0.071279056370258, 0.705308496952057 );
				glVertex3f( 0.112051382660866, 1.039769530296326, 0.194078475236893);
				glVertex3f( 0.194078654050827, 1.039769530296326, 0.112051278352737);
				glVertex3f( 0.102435901761055, 1.299711942672729, 0.177423954010010);

			glNormal3f( 0.257634550333023, 0.095566019415855, 0.961504817008972 );
				glVertex3f( 0.000000048139775, 0.000000057845746, 0.320779353380203);
				glVertex3f( 0.160389676690102, 0.000000054088638, 0.277803063392639);
				glVertex3f( 0.000000044033275, 0.259942412376404, 0.294943124055862);

			glNormal3f( 0.257810473442078, 0.088203318417072, 0.962161064147949 );
				glVertex3f( 0.000000040027594, 0.519884705543518, 0.269686281681061);
				glVertex3f( 0.134843215346336, 0.519884705543518, 0.233555138111115);
				glVertex3f( 0.000000036248949, 0.779827117919922, 0.245856821537018);

			glNormal3f( 0.258160591125488, 0.071279115974903, 0.963468968868256 );
				glVertex3f( 0.000000032790762, 1.039769530296326, 0.224102526903152);
				glVertex3f( 0.112051382660866, 1.039769530296326, 0.194078475236893);
				glVertex3f( 0.000000029713739, 1.299711942672729, 0.204871535301208);

			glNormal3f( -0.257634550333023, 0.095565780997276, 0.961504936218262 );
				glVertex3f( -0.160389587283134, 0.000000054088638, 0.277803063392639);
				glVertex3f( 0.000000048139775, 0.000000057845746, 0.320779353380203);
				glVertex3f( -0.147471547126770, 0.259942412376404, 0.255428284406662);

			glNormal3f( -0.257810026407242, 0.088203534483910, 0.962161183357239 );
				glVertex3f( -0.134843140840530, 0.519884705543518, 0.233555197715759);
				glVertex3f( 0.000000040027594, 0.519884705543518, 0.269686281681061);
				glVertex3f( -0.122928410768509, 0.779827117919922, 0.212918221950531);

			glNormal3f( -0.258160591125488, 0.071279078722000, 0.963468968868256 );
				glVertex3f( -0.112051323056221, 1.039769530296326, 0.194078475236893);
				glVertex3f( 0.000000032790762, 1.039769530296326, 0.224102526903152);
				glVertex3f( -0.102435842156410, 1.299711942672729, 0.177423954010010);

			glNormal3f( -0.703870058059692, 0.095565751194954, 0.703870713710785 );
				glVertex3f( -0.277803003787994, 0.000000028922880, 0.160389766097069);
				glVertex3f( -0.160389587283134, 0.000000054088638, 0.277803063392639);
				glVertex3f( -0.255428254604340, 0.259942382574081, 0.147471666336060);

			glNormal3f( -0.704350590705872, 0.088203534483910, 0.704351007938385 );
				glVertex3f( -0.233555227518082, 0.519884705543518, 0.134843170642853);
				glVertex3f( -0.134843140840530, 0.519884705543518, 0.233555197715759);
				glVertex3f( -0.212918281555176, 0.779827117919922, 0.122928380966187);

			glNormal3f( -0.705307722091675, 0.071279279887676, 0.705308675765991 );
				glVertex3f( -0.194078624248505, 1.039769530296326, 0.112051278352737);
				glVertex3f( -0.112051323056221, 1.039769530296326, 0.194078475236893);
				glVertex3f( -0.177424103021622, 1.299711942672729, 0.102435708045959);

			glNormal3f( -0.961504757404327, 0.095565721392632, 0.257634848356247 );
				glVertex3f( -0.277803003787994, 0.000000028922880, 0.160389766097069);
				glVertex3f( -0.255428254604340, 0.259942382574081, 0.147471666336060);
				glVertex3f( -0.320779353380203, 0.000000000000028, 0.000000153286052);

			glNormal3f( -0.962160944938660, 0.088203467428684, 0.257810592651367 );
				glVertex3f( -0.233555227518082, 0.519884705543518, 0.134843170642853);
				glVertex3f( -0.212918281555176, 0.779827117919922, 0.122928380966187);
				glVertex3f( -0.269686371088028, 0.519884705543518, 0.000000073759523);

			glNormal3f( -0.963468790054321, 0.071279063820839, 0.258161008358002 );
				glVertex3f( -0.224102735519409, 1.039769530296326, -0.000000090899540);
				glVertex3f( -0.194078624248505, 1.039769530296326, 0.112051278352737);
				glVertex3f( -0.204871758818626, 1.299711942672729, -0.000000113624424);

			glNormal3f( -0.961504995822906, 0.095565706491470, -0.257633864879608 );
				glVertex3f( -0.277803182601929, -0.000000028922859, -0.160389512777328);
				glVertex3f( -0.320779353380203, 0.000000000000028, 0.000000153286052);
				glVertex3f( -0.255428403615952, 0.259942412376404, -0.147471487522125);

			glNormal3f( -0.962161302566528, 0.088203340768814, -0.257809758186340 );
				glVertex3f( -0.233555331826210, 0.519884705543518, -0.134843111038208);
				glVertex3f( -0.269686371088028, 0.519884705543518, 0.000000073759523);
				glVertex3f( -0.212918385863304, 0.779827117919922, -0.122928440570831);

			glNormal3f( -0.963469088077545, 0.071279078722000, -0.258160054683685 );
				glVertex3f( -0.194078758358955, 1.039769530296326, -0.112051397562027);
				glVertex3f( -0.224102735519409, 1.039769530296326, -0.000000090899540);
				glVertex3f( -0.177424222230911, 1.299711942672729, -0.102435946464539);

			glNormal3f( -0.703870713710785, 0.095565512776375, -0.703870117664337 );
				glVertex3f( -0.160389855504036, -0.000000054088630, -0.277802944183350);
				glVertex3f( -0.277803182601929, -0.000000028922859, -0.160389512777328);
				glVertex3f( -0.147471785545349, 0.259942322969437, -0.255428254604340);

			glNormal3f( -0.704351425170898, 0.088203318417072, -0.704350292682648 );
				glVertex3f( -0.134843349456787, 0.519884705543518, -0.233555257320404);
				glVertex3f( -0.233555331826210, 0.519884705543518, -0.134843111038208);
				glVertex3f( -0.122928604483604, 0.779827117919922, -0.212918341159821);

			glNormal3f( -0.705308794975281, 0.071278922259808, -0.705307602882385 );
				glVertex3f( -0.112051516771317, 1.039769530296326, -0.194078773260117);
				glVertex3f( -0.194078758358955, 1.039769530296326, -0.112051397562027);
				glVertex3f( -0.102436020970345, 1.299711942672729, -0.177424311637878);

			glNormal3f( -0.257634907960892, 0.095565460622311, -0.961504757404327 );
				glVertex3f( -0.160389855504036, -0.000000054088630, -0.277802944183350);
				glVertex3f( -0.147471785545349, 0.259942322969437, -0.255428254604340);
				glVertex3f( -0.000000000296557, -0.000000057845746, -0.320779353380203);

			glNormal3f( -0.257810592651367, 0.088203318417072, -0.962161064147949 );
				glVertex3f( -0.134843349456787, 0.519884705543518, -0.233555257320404);
				glVertex3f( -0.122928604483604, 0.779827117919922, -0.212918341159821);
				glVertex3f( -0.000000000693916, 0.519884824752808, -0.269686460494995);

			glNormal3f( -0.258160769939423, 0.071278862655163, -0.963468909263611 );
				glVertex3f( -0.112051516771317, 1.039769530296326, -0.194078773260117);
				glVertex3f( -0.102436020970345, 1.299711942672729, -0.177424311637878);
				glVertex3f( -0.000000001047811, 1.039769649505615, -0.224102884531021);

		// blue
		SetMaterial( 0, 0, 1, 100.);
			glNormal3f( 0.706661939620972, -0.035452447831631, -0.706662714481354 );
				glVertex3f( 0.163169324398041, 1.559654235839844, -0.094206109642982);
				glVertex3f( 0.094205856323242, 1.559654235839844, -0.163169503211975);
				glVertex3f( 0.100606277585030, 1.908206462860107, -0.174255549907684);

			glNormal3f( 0.965318679809570, -0.035451974719763, -0.258656412363052 );
				glVertex3f( 0.188411712646484, 1.559654235839844, -0.000000255558575);
				glVertex3f( 0.163169324398041, 1.559654235839844, -0.094206109642982);
				glVertex3f( 0.174255162477493, 1.908206462860107, -0.100606486201286);

			glNormal3f( 0.965318679809570, -0.035452026873827, 0.258656412363052 );
				glVertex3f( 0.163169324398041, 1.559654235839844, 0.094205603003502);
				glVertex3f( 0.188411712646484, 1.559654235839844, -0.000000255558575);
				glVertex3f( 0.201212555170059, 1.908206462860107, -0.000000286030001);

			glNormal3f( 0.706662833690643, -0.035452056676149, 0.706661701202393 );
				glVertex3f( 0.094205886125565, 1.559654235839844, 0.163169145584106);
				glVertex3f( 0.163169324398041, 1.559654235839844, 0.094205603003502);
				glVertex3f( 0.174255162477493, 1.908206462860107, 0.100606039166451);

			glNormal3f( 0.258655786514282, -0.035451907664537, 0.965318739414215 );
				glVertex3f( 0.000000027045798, 1.559654235839844, 0.188411474227905);
				glVertex3f( 0.094205886125565, 1.559654235839844, 0.163169145584106);
				glVertex3f( 0.100606292486191, 1.908206462860107, 0.174254953861237);

			glNormal3f( -0.258655875921249, -0.035451807081699, 0.965318739414215 );
				glVertex3f( -0.094205811619759, 1.559654235839844, 0.163169145584106);
				glVertex3f( 0.000000027045798, 1.559654235839844, 0.188411474227905);
				glVertex3f( 0.000000029807616, 1.908206462860107, 0.201212227344513);

			glNormal3f( -0.706661999225616, -0.035451933741570, 0.706662654876709 );
				glVertex3f( -0.163169294595718, 1.559654235839844, 0.094205722212791);
				glVertex3f( -0.094205811619759, 1.559654235839844, 0.163169145584106);
				glVertex3f( -0.100606217980385, 1.908206462860107, 0.174254953861237);

			glNormal3f( -0.965318500995636, -0.035451933741570, 0.258656740188599 );
				glVertex3f( -0.163169294595718, 1.559654235839844, 0.094205722212791);
				glVertex3f( -0.174255132675171, 1.908206462860107, 0.100606039166451);
				glVertex3f( -0.201212525367737, 1.908206462860107, -0.000000166820712);

			glNormal3f( -0.965318739414215, -0.035451997071505, -0.258655667304993 );
				glVertex3f( -0.163169398903847, 1.559654235839844, -0.094205990433693);
				glVertex3f( -0.188411712646484, 1.559654235839844, -0.000000136349300);
				glVertex3f( -0.201212525367737, 1.908206462860107, -0.000000166820712);

			glNormal3f( -0.706662714481354, -0.035452172160149, -0.706661760807037 );
				glVertex3f( -0.094205982983112, 1.559654235839844, -0.163169503211975);
				glVertex3f( -0.163169398903847, 1.559654235839844, -0.094205990433693);
				glVertex3f( -0.174255236983299, 1.908206462860107, -0.100606486201286);

			glNormal3f( -0.258656650781631, -0.035452246665955, -0.965318560600281 );
				glVertex3f( -0.000000001403581, 1.559654235839844, -0.188411951065063);
				glVertex3f( -0.094205982983112, 1.559654235839844, -0.163169503211975);
				glVertex3f( -0.100606389343739, 1.908206462860107, -0.174255430698395);

			glNormal3f( 0.258656948804855, -0.035452112555504, -0.965318441390991 );
				glVertex3f( 0.094205856323242, 1.559654235839844, -0.163169503211975);
				glVertex3f( -0.000000001403581, 1.559654235839844, -0.188411951065063);
				glVertex3f( -0.000000000574626, 1.908206582069397, -0.201212823390961);

			glNormal3f( 0.706663191318512, -0.035451941192150, -0.706661462783813 );
				glVertex3f( 0.174255162477493, 1.908206462860107, -0.100606486201286);
				glVertex3f( 0.163169324398041, 1.559654235839844, -0.094206109642982);
				glVertex3f( 0.100606277585030, 1.908206462860107, -0.174255549907684);

			glNormal3f( 0.965318500995636, -0.035452071577311, -0.258656740188599 );
				glVertex3f( 0.201212555170059, 1.908206462860107, -0.000000286030001);
				glVertex3f( 0.188411712646484, 1.559654235839844, -0.000000255558575);
				glVertex3f( 0.174255162477493, 1.908206462860107, -0.100606486201286);

			glNormal3f( 0.965318560600281, -0.035452015697956, 0.258656442165375 );
				glVertex3f( 0.174255162477493, 1.908206462860107, 0.100606039166451);
				glVertex3f( 0.163169324398041, 1.559654235839844, 0.094205603003502);
				glVertex3f( 0.201212555170059, 1.908206462860107, -0.000000286030001);

			glNormal3f( 0.706662476062775, -0.035451926290989, 0.706662058830261 );
				glVertex3f( 0.100606292486191, 1.908206462860107, 0.174254953861237);
				glVertex3f( 0.094205886125565, 1.559654235839844, 0.163169145584106);
				glVertex3f( 0.174255162477493, 1.908206462860107, 0.100606039166451);

			glNormal3f( 0.258655518293381, -0.035451810806990, 0.965318858623505 );
				glVertex3f( 0.000000029807616, 1.908206462860107, 0.201212227344513);
				glVertex3f( 0.000000027045798, 1.559654235839844, 0.188411474227905);
				glVertex3f( 0.100606292486191, 1.908206462860107, 0.174254953861237);

			glNormal3f( -0.258655577898026, -0.035451903939247, 0.965318858623505 );
				glVertex3f( -0.100606217980385, 1.908206462860107, 0.174254953861237);
				glVertex3f( -0.094205811619759, 1.559654235839844, 0.163169145584106);
				glVertex3f( 0.000000029807616, 1.908206462860107, 0.201212227344513);

			glNormal3f( -0.706662237644196, -0.035451803356409, 0.706662237644196 );
				glVertex3f( -0.174255132675171, 1.908206462860107, 0.100606039166451);
				glVertex3f( -0.163169294595718, 1.559654235839844, 0.094205722212791);
				glVertex3f( -0.100606217980385, 1.908206462860107, 0.174254953861237);

			glNormal3f( -0.965318560600281, -0.035451941192150, 0.258656680583954 );
				glVertex3f( -0.188411712646484, 1.559654235839844, -0.000000136349300);
				glVertex3f( -0.163169294595718, 1.559654235839844, 0.094205722212791);
				glVertex3f( -0.201212525367737, 1.908206462860107, -0.000000166820712);

			glNormal3f( -0.965318858623505, -0.035452052950859, -0.258655458688736 );
				glVertex3f( -0.174255236983299, 1.908206462860107, -0.100606486201286);
				glVertex3f( -0.163169398903847, 1.559654235839844, -0.094205990433693);
				glVertex3f( -0.201212525367737, 1.908206462860107, -0.000000166820712);

			glNormal3f( -0.706662714481354, -0.035452164709568, -0.706661880016327 );
				glVertex3f( -0.100606389343739, 1.908206462860107, -0.174255430698395);
				glVertex3f( -0.094205982983112, 1.559654235839844, -0.163169503211975);
				glVertex3f( -0.174255236983299, 1.908206462860107, -0.100606486201286);

			glNormal3f( -0.258656233549118, -0.035452123731375, -0.965318679809570 );
				glVertex3f( -0.000000000574626, 1.908206582069397, -0.201212823390961);
				glVertex3f( -0.000000001403581, 1.559654235839844, -0.188411951065063);
				glVertex3f( -0.100606389343739, 1.908206462860107, -0.174255430698395);

			glNormal3f( 0.258655428886414, -0.035452570766211, -0.965318858623505 );
				glVertex3f( 0.100606277585030, 1.908206462860107, -0.174255549907684);
				glVertex3f( 0.094205856323242, 1.559654235839844, -0.163169503211975);
				glVertex3f( -0.000000000574626, 1.908206582069397, -0.201212823390961);

	glEndList();

	RockList = glGenLists( 1 );
	glNewList( RockList, GL_COMPILE );
		glTranslatef(0., offset-.4, 0.);
		glBegin( GL_TRIANGLES );
		// rock
		glColor3f( 0.384277135133743, 0.399005770683289, 0.410938382148743);
		SetMaterial( 0.384277135133743, 0.399005770683289, 0.410938382148743, .2);
			glNormal3f( -0.697185337543488, 0.702534079551697, 0.142753481864929 );
				glVertex3f( -0.873154997825623, -0.121425211429596, 0.705753386020660);
				glVertex3f( -0.728239715099335, 0.011649668216705, 0.758593916893005);
				glVertex3f( -0.844372272491455, -0.058297812938690, 0.535654187202454);

			glNormal3f( -0.275874644517899, 0.950226604938507, -0.144785106182098 );
				glVertex3f( -0.617226600646973, -0.129174619913101, -0.155518159270287);
				glVertex3f( -0.308615535497665, -0.035711884498596, -0.130150809884071);
				glVertex3f( -0.440998286008835, -0.109151184558868, -0.359890818595886);

			glNormal3f( -0.236974149942398, 0.888893306255341, -0.392061293125153 );
				glVertex3f( -0.617226600646973, -0.129174619913101, -0.155518159270287);
				glVertex3f( -0.457475423812866, 0.021477319300175, 0.089485868811607);
				glVertex3f( -0.308615535497665, -0.035711884498596, -0.130150809884071);

			glNormal3f( 0.265318065881729, 0.817649364471436, 0.510936260223389 );
				glVertex3f( 0.000284023582935, -0.029501378536224, 0.956667363643646);
				glVertex3f( 0.266146779060364, -0.069089293479919, 0.881963014602661);
				glVertex3f( 0.138556614518166, 0.097998976707458, 0.680827081203461);

			glNormal3f( -0.254978775978088, 0.930317997932434, -0.263617426156998 );
				glVertex3f( -0.800457298755646, -0.065555125474930, 0.355866074562073);
				glVertex3f( -0.535489082336426, 0.006905481219292, 0.355297565460205);
				glVertex3f( -0.705398797988892, -0.112276889383793, 0.099039383232594);

			glNormal3f( -0.255004584789276, 0.930415272712708, -0.263249009847641 );
				glVertex3f( -0.800457298755646, -0.065555125474930, 0.355866074562073);
				glVertex3f( -0.686746001243591, 0.035275697708130, 0.602087855339050);
				glVertex3f( -0.535489082336426, 0.006905481219292, 0.355297565460205);

			glNormal3f( -0.006703592836857, 0.993714988231659, 0.111737892031670 );
				glVertex3f( -0.538170158863068, 0.051156759262085, 0.695012271404266);
				glVertex3f( -0.274486958980560, 0.048274993896484, 0.736459970474243);
				glVertex3f( -0.410304963588715, 0.063634157180786, 0.591718554496765);

			glNormal3f( -0.407259464263916, 0.803778290748596, -0.433682054281235 );
				glVertex3f( -0.267326295375824, 0.014238059520721, -0.588641405105591);
				glVertex3f( 0.003707100404426, 0.136133491992950, -0.617243051528931);
				glVertex3f( -0.134560599923134, 0.013881325721741, -0.713979363441467);

			glNormal3f( -0.417950093746185, 0.899609982967377, -0.126569002866745 );
				glVertex3f( -0.267326295375824, 0.014238059520721, -0.588641405105591);
				glVertex3f( -0.130372121930122, 0.109528183937073, -0.363594710826874);
				glVertex3f( 0.003707100404426, 0.136133491992950, -0.617243051528931);

			glNormal3f( -0.058513760566711, 0.895848929882050, 0.440489083528519 );
				glVertex3f( 0.001831938861869, 0.083842873573303, -0.110991865396500);
				glVertex3f( 0.327808022499084, 0.134485572576523, -0.170684874057770);
				glVertex3f( 0.151239693164825, 0.232734441757202, -0.393954455852509);

			glNormal3f( -0.120589986443520, 0.977851569652557, 0.171068474650383 );
				glVertex3f( 0.001831938861869, 0.083842873573303, -0.110991865396500);
				glVertex3f( 0.132537111639977, 0.058789346367121, 0.124354645609856);
				glVertex3f( 0.327808022499084, 0.134485572576523, -0.170684874057770);

			glNormal3f( -0.001068071462214, 0.994318306446075, 0.106442123651505 );
				glVertex3f( 0.001831938861869, 0.083842873573303, -0.110991865396500);
				glVertex3f( -0.132485613226891, 0.058497447520494, 0.124422073364258);
				glVertex3f( 0.132537111639977, 0.058789346367121, 0.124354645609856);

			glNormal3f( -0.239463165402412, 0.962694525718689, 0.126002952456474 );
				glVertex3f( -0.267154514789581, 0.075991526246071, 0.355222880840302);
				glVertex3f( -0.135981529951096, 0.068181872367859, 0.664179205894470);
				glVertex3f( -0.000000030267984, 0.140909090638161, 0.366951823234558);

			glNormal3f( 0.127671673893929, 0.991396725177765, 0.028852814808488 );
				glVertex3f( 0.267153948545456, 0.075988337397575, 0.355221986770630);
				glVertex3f( 0.545251667499542, 0.040196463465691, 0.354485273361206);
				glVertex3f( 0.467098861932755, 0.058060307055712, 0.086495622992516);

			glNormal3f( 0.127382636070251, 0.987930715084076, 0.088127441704273 );
				glVertex3f( 0.267153948545456, 0.075988337397575, 0.355221986770630);
				glVertex3f( 0.405852496623993, 0.036627203226089, 0.595990240573883);
				glVertex3f( 0.545251667499542, 0.040196463465691, 0.354485273361206);

			glNormal3f( 0.165698945522308, 0.946633815765381, 0.276456594467163 );
				glVertex3f( 0.532166600227356, -0.076863169670105, 0.814927935600281);
				glVertex3f( 0.704617142677307, -0.091265559196472, 0.760882973670959);
				glVertex3f( 0.675445377826691, -0.036268025636673, 0.590046763420105);

			glNormal3f( -0.458255261182785, 0.876527428627014, -0.147315084934235 );
				glVertex3f( -0.844372272491455, -0.058297812938690, 0.535654187202454);
				glVertex3f( -0.686746001243591, 0.035275697708130, 0.602087855339050);
				glVertex3f( -0.800457298755646, -0.065555125474930, 0.355866074562073);

			glNormal3f( -0.508911073207855, 0.860804617404938, -0.004978803917766 );
				glVertex3f( -0.844372272491455, -0.058297812938690, 0.535654187202454);
				glVertex3f( -0.728239715099335, 0.011649668216705, 0.758593916893005);
				glVertex3f( -0.686746001243591, 0.035275697708130, 0.602087855339050);

			glNormal3f( -0.169246912002563, 0.980166912078857, 0.103093571960926 );
				glVertex3f( -0.728239715099335, 0.011649668216705, 0.758593916893005);
				glVertex3f( -0.538170158863068, 0.051156759262085, 0.695012271404266);
				glVertex3f( -0.686746001243591, 0.035275697708130, 0.602087855339050);

			glNormal3f( -0.575615882873535, 0.817710220813751, 0.004058169666678 );
				glVertex3f( -0.440998286008835, -0.109151184558868, -0.359890818595886);
				glVertex3f( -0.130372121930122, 0.109528183937073, -0.363594710826874);
				glVertex3f( -0.267326295375824, 0.014238059520721, -0.588641405105591);

			glNormal3f( -0.573696792125702, 0.816096305847168, 0.069704972207546 );
				glVertex3f( -0.440998286008835, -0.109151184558868, -0.359890818595886);
				glVertex3f( -0.308615535497665, -0.035711884498596, -0.130150809884071);
				glVertex3f( -0.130372121930122, 0.109528183937073, -0.363594710826874);

			glNormal3f( -0.360054284334183, 0.890247344970703, 0.278963267803192 );
				glVertex3f( -0.308615535497665, -0.035711884498596, -0.130150809884071);
				glVertex3f( 0.001831938861869, 0.083842873573303, -0.110991865396500);
				glVertex3f( -0.130372121930122, 0.109528183937073, -0.363594710826874);

			glNormal3f( -0.352129399776459, 0.930515289306641, -0.100728318095207 );
				glVertex3f( -0.308615535497665, -0.035711884498596, -0.130150809884071);
				glVertex3f( -0.132485613226891, 0.058497447520494, 0.124422073364258);
				glVertex3f( 0.001831938861869, 0.083842873573303, -0.110991865396500);

			glNormal3f( -0.076171375811100, 0.951120316982269, -0.299279153347015 );
				glVertex3f( -0.308615535497665, -0.035711884498596, -0.130150809884071);
				glVertex3f( -0.457475423812866, 0.021477319300175, 0.089485868811607);
				glVertex3f( -0.132485613226891, 0.058497447520494, 0.124422073364258);

			glNormal3f( -0.098164767026901, 0.986371397972107, -0.132042005658150 );
				glVertex3f( -0.457475423812866, 0.021477319300175, 0.089485868811607);
				glVertex3f( -0.267154514789581, 0.075991526246071, 0.355222880840302);
				glVertex3f( -0.132485613226891, 0.058497447520494, 0.124422073364258);

			glNormal3f( 0.231436595320702, 0.972511351108551, 0.025664644315839 );
				glVertex3f( 0.138556614518166, 0.097998976707458, 0.680827081203461);
				glVertex3f( 0.405852496623993, 0.036627203226089, 0.595990240573883);
				glVertex3f( 0.267153948545456, 0.075988337397575, 0.355221986770630);

			glNormal3f( 0.335947602987289, 0.818192481994629, 0.466583728790283 );
				glVertex3f( 0.138556614518166, 0.097998976707458, 0.680827081203461);
				glVertex3f( 0.266146779060364, -0.069089293479919, 0.881963014602661);
				glVertex3f( 0.405852496623993, 0.036627203226089, 0.595990240573883);

			glNormal3f( 0.126808851957321, 0.908644020557404, 0.397851079702377 );
				glVertex3f( 0.266146779060364, -0.069089293479919, 0.881963014602661);
				glVertex3f( 0.532166600227356, -0.076863169670105, 0.814927935600281);
				glVertex3f( 0.405852496623993, 0.036627203226089, 0.595990240573883);

			glNormal3f( -0.469767361879349, 0.855072498321533, -0.219475746154785 );
				glVertex3f( -0.705398797988892, -0.112276889383793, 0.099039383232594);
				glVertex3f( -0.457475423812866, 0.021477319300175, 0.089485868811607);
				glVertex3f( -0.617226600646973, -0.129174619913101, -0.155518159270287);

			glNormal3f( -0.475542396306992, 0.874910175800323, -0.091605268418789 );
				glVertex3f( -0.705398797988892, -0.112276889383793, 0.099039383232594);
				glVertex3f( -0.535489082336426, 0.006905481219292, 0.355297565460205);
				glVertex3f( -0.457475423812866, 0.021477319300175, 0.089485868811607);

			glNormal3f( -0.249286189675331, 0.968221604824066, -0.020085489377379 );
				glVertex3f( -0.535489082336426, 0.006905481219292, 0.355297565460205);
				glVertex3f( -0.267154514789581, 0.075991526246071, 0.355222880840302);
				glVertex3f( -0.457475423812866, 0.021477319300175, 0.089485868811607);

			glNormal3f( -0.248111695051193, 0.963573575019836, -0.099832974374294 );
				glVertex3f( -0.535489082336426, 0.006905481219292, 0.355297565460205);
				glVertex3f( -0.410304963588715, 0.063634157180786, 0.591718554496765);
				glVertex3f( -0.267154514789581, 0.075991526246071, 0.355222880840302);

			glNormal3f( -0.107024684548378, 0.978185057640076, -0.178044080734253 );
				glVertex3f( -0.535489082336426, 0.006905481219292, 0.355297565460205);
				glVertex3f( -0.686746001243591, 0.035275697708130, 0.602087855339050);
				glVertex3f( -0.410304963588715, 0.063634157180786, 0.591718554496765);

			glNormal3f( -0.102286182343960, 0.994734048843384, -0.006458763498813 );
				glVertex3f( -0.686746001243591, 0.035275697708130, 0.602087855339050);
				glVertex3f( -0.538170158863068, 0.051156759262085, 0.695012271404266);
				glVertex3f( -0.410304963588715, 0.063634157180786, 0.591718554496765);

			glNormal3f( -0.026165891438723, 0.998996138572693, 0.036361359059811 );
				glVertex3f( -0.410304963588715, 0.063634157180786, 0.591718554496765);
				glVertex3f( -0.135981529951096, 0.068181872367859, 0.664179205894470);
				glVertex3f( -0.267154514789581, 0.075991526246071, 0.355222880840302);

			glNormal3f( -0.058445975184441, 0.985479772090912, 0.159416437149048 );
				glVertex3f( -0.410304963588715, 0.063634157180786, 0.591718554496765);
				glVertex3f( -0.274486958980560, 0.048274993896484, 0.736459970474243);
				glVertex3f( -0.135981529951096, 0.068181872367859, 0.664179205894470);

			glNormal3f( 0.023391352966428, 0.951465070247650, 0.306866347789764 );
				glVertex3f( -0.274486958980560, 0.048274993896484, 0.736459970474243);
				glVertex3f( 0.000284023582935, -0.029501378536224, 0.956667363643646);
				glVertex3f( -0.135981529951096, 0.068181872367859, 0.664179205894470);

			glNormal3f( 0.011405524797738, 0.742421448230743, -0.669836103916168 );
				glVertex3f( -0.134560599923134, 0.013881325721741, -0.713979363441467);
				glVertex3f( 0.135580912232399, 0.010457456111908, -0.713174462318420);
				glVertex3f( -0.001287056598812, -0.081205606460571, -0.817100882530212);

			glNormal3f( 0.010127616114914, 0.613418161869049, -0.789693355560303 );
				glVertex3f( -0.134560599923134, 0.013881325721741, -0.713979363441467);
				glVertex3f( 0.003707100404426, 0.136133491992950, -0.617243051528931);
				glVertex3f( 0.135580912232399, 0.010457456111908, -0.713174462318420);

			glNormal3f( 0.148274645209312, 0.693645715713501, -0.704890131950378 );
				glVertex3f( 0.003707100404426, 0.136133491992950, -0.617243051528931);
				glVertex3f( 0.277214229106903, 0.088261485099792, -0.606818795204163);
				glVertex3f( 0.135580912232399, 0.010457456111908, -0.713174462318420);

			glNormal3f( 0.168783187866211, 0.859093606472015, -0.483187586069107 );
				glVertex3f( 0.003707100404426, 0.136133491992950, -0.617243051528931);
				glVertex3f( 0.151239693164825, 0.232734441757202, -0.393954455852509);
				glVertex3f( 0.277214229106903, 0.088261485099792, -0.606818795204163);

			glNormal3f( -0.408805847167969, 0.904535055160522, -0.121218755841255 );
				glVertex3f( 0.003707100404426, 0.136133491992950, -0.617243051528931);
				glVertex3f( -0.130372121930122, 0.109528183937073, -0.363594710826874);
				glVertex3f( 0.151239693164825, 0.232734441757202, -0.393954455852509);

			glNormal3f( -0.359557241201401, 0.890520989894867, 0.278730958700180 );
				glVertex3f( -0.130372121930122, 0.109528183937073, -0.363594710826874);
				glVertex3f( 0.001831938861869, 0.083842873573303, -0.110991865396500);
				glVertex3f( 0.151239693164825, 0.232734441757202, -0.393954455852509);

			glNormal3f( 0.424748182296753, 0.845840811729431, -0.322710990905762 );
				glVertex3f( 0.151239693164825, 0.232734441757202, -0.393954455852509);
				glVertex3f( 0.460782915353775, 0.073588967323303, -0.403665363788605);
				glVertex3f( 0.277214229106903, 0.088261485099792, -0.606818795204163);

			glNormal3f( 0.457765817642212, 0.888599514961243, 0.029009578749537 );
				glVertex3f( 0.151239693164825, 0.232734441757202, -0.393954455852509);
				glVertex3f( 0.327808022499084, 0.134485572576523, -0.170684874057770);
				glVertex3f( 0.460782915353775, 0.073588967323303, -0.403665363788605);

			glNormal3f( 0.613739907741547, 0.775594174861908, 0.147570058703423 );
				glVertex3f( 0.327808022499084, 0.134485572576523, -0.170684874057770);
				glVertex3f( 0.621830642223358, -0.099825918674469, -0.162029832601547);
				glVertex3f( 0.460782915353775, 0.073588967323303, -0.403665363788605);

			glNormal3f( 0.621617019176483, 0.776110827922821, -0.106038644909859 );
				glVertex3f( 0.327808022499084, 0.134485572576523, -0.170684874057770);
				glVertex3f( 0.467098861932755, 0.058060307055712, 0.086495622992516);
				glVertex3f( 0.621830642223358, -0.099825918674469, -0.162029832601547);

			glNormal3f( 0.032481238245964, 0.962732136249542, 0.268499016761780 );
				glVertex3f( 0.327808022499084, 0.134485572576523, -0.170684874057770);
				glVertex3f( 0.132537111639977, 0.058789346367121, 0.124354645609856);
				glVertex3f( 0.467098861932755, 0.058060307055712, 0.086495622992516);

			glNormal3f( -0.005849236622453, 0.997466444969177, -0.070897899568081 );
				glVertex3f( 0.132537111639977, 0.058789346367121, 0.124354645609856);
				glVertex3f( 0.267153948545456, 0.075988337397575, 0.355221986770630);
				glVertex3f( 0.467098861932755, 0.058060307055712, 0.086495622992516);

			glNormal3f( 0.222968772053719, 0.953863322734833, -0.201071500778198 );
				glVertex3f( 0.132537111639977, 0.058789346367121, 0.124354645609856);
				glVertex3f( -0.000000030267984, 0.140909090638161, 0.366951823234558);
				glVertex3f( 0.267153948545456, 0.075988337397575, 0.355221986770630);

			glNormal3f( -0.001124770264141, 0.947016835212708, -0.321182042360306 );
				glVertex3f( 0.132537111639977, 0.058789346367121, 0.124354645609856);
				glVertex3f( -0.132485613226891, 0.058497447520494, 0.124422073364258);
				glVertex3f( -0.000000030267984, 0.140909090638161, 0.366951823234558);

			glNormal3f( -0.222846522927284, 0.953629970550537, -0.202310279011726 );
				glVertex3f( -0.132485613226891, 0.058497447520494, 0.124422073364258);
				glVertex3f( -0.267154514789581, 0.075991526246071, 0.355222880840302);
				glVertex3f( -0.000000030267984, 0.140909090638161, 0.366951823234558);

			glNormal3f( 0.237205967307091, 0.971054553985596, 0.028041711077094 );
				glVertex3f( -0.000000030267984, 0.140909090638161, 0.366951823234558);
				glVertex3f( 0.138556614518166, 0.097998976707458, 0.680827081203461);
				glVertex3f( 0.267153948545456, 0.075988337397575, 0.355221986770630);

			glNormal3f( -0.117195084691048, 0.975702106952667, 0.185123383998871 );
				glVertex3f( -0.000000030267984, 0.140909090638161, 0.366951823234558);
				glVertex3f( -0.135981529951096, 0.068181872367859, 0.664179205894470);
				glVertex3f( 0.138556614518166, 0.097998976707458, 0.680827081203461);

			glNormal3f( -0.122375048696995, 0.922843575477600, 0.365217775106430 );
				glVertex3f( -0.135981529951096, 0.068181872367859, 0.664179205894470);
				glVertex3f( 0.000284023582935, -0.029501378536224, 0.956667363643646);
				glVertex3f( 0.138556614518166, 0.097998976707458, 0.680827081203461);

			glNormal3f( 0.415121912956238, 0.862483859062195, -0.289474457502365 );
				glVertex3f( 0.467098861932755, 0.058060307055712, 0.086495622992516);
				glVertex3f( 0.719712615013123, -0.059754729270935, 0.097729556262493);
				glVertex3f( 0.621830642223358, -0.099825918674469, -0.162029832601547);

			glNormal3f( 0.424139916896820, 0.903369605541229, -0.063472859561443 );
				glVertex3f( 0.467098861932755, 0.058060307055712, 0.086495622992516);
				glVertex3f( 0.545251667499542, 0.040196463465691, 0.354485273361206);
				glVertex3f( 0.719712615013123, -0.059754729270935, 0.097729556262493);

			glNormal3f( 0.249548733234406, 0.947624385356903, -0.199332192540169 );
				glVertex3f( 0.545251667499542, 0.040196463465691, 0.354485273361206);
				glVertex3f( 0.811047971248627, -0.029342934489250, 0.356652021408081);
				glVertex3f( 0.719712615013123, -0.059754729270935, 0.097729556262493);

			glNormal3f( 0.248003467917442, 0.953299939632416, 0.172375783324242 );
				glVertex3f( 0.545251667499542, 0.040196463465691, 0.354485273361206);
				glVertex3f( 0.675445377826691, -0.036268025636673, 0.590046763420105);
				glVertex3f( 0.811047971248627, -0.029342934489250, 0.356652021408081);

			glNormal3f( 0.260836154222488, 0.951244473457336, 0.164616018533707 );
				glVertex3f( 0.545251667499542, 0.040196463465691, 0.354485273361206);
				glVertex3f( 0.405852496623993, 0.036627203226089, 0.595990240573883);
				glVertex3f( 0.675445377826691, -0.036268025636673, 0.590046763420105);

			glNormal3f( 0.253455132246017, 0.910796403884888, 0.325899451971054 );
				glVertex3f( 0.405852496623993, 0.036627203226089, 0.595990240573883);
				glVertex3f( 0.532166600227356, -0.076863169670105, 0.814927935600281);
				glVertex3f( 0.675445377826691, -0.036268025636673, 0.590046763420105);

			glNormal3f( 0.277302533388138, 0.941997051239014, 0.189063221216202 );
				glVertex3f( 0.675445377826691, -0.036268025636673, 0.590046763420105);
				glVertex3f( 0.842458128929138, -0.073968470096588, 0.532926440238953);
				glVertex3f( 0.811047971248627, -0.029342934489250, 0.356652021408081);

			glNormal3f( 0.293053418397903, 0.923564255237579, 0.247282862663269 );
				glVertex3f( 0.675445377826691, -0.036268025636673, 0.590046763420105);
				glVertex3f( 0.704617142677307, -0.091265559196472, 0.760882973670959);
				glVertex3f( 0.842458128929138, -0.073968470096588, 0.532926440238953);

			glNormal3f( 0.419736325740814, 0.850002408027649, 0.318303823471069 );
				glVertex3f( 0.704617142677307, -0.091265559196472, 0.760882973670959);
				glVertex3f( 0.867598712444305, -0.149593919515610, 0.701725542545319);
				glVertex3f( 0.842458128929138, -0.073968470096588, 0.532926440238953);

		glEnd( );
	glEndList( );


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case '0':
			enableLight0 = ! enableLight0;
			break;

		case '1':
			enableLight1 = ! enableLight1;
			break;

		case '2':
			enableLight2 = ! enableLight2;
			break;

		case 'f':
		case 'F':
			freezeAnimation = ! freezeAnimation;
			if( freezeAnimation )
				glutIdleFunc( NULL );
			else
				glutIdleFunc( Animate );
			break;

		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );


	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


struct point *
PtsPointer( int lat, int lng )
{
	if( lat < 0 )	lat += (NumLats-1);
	if( lng < 0 )	lng += (NumLngs-1);
	if( lat > NumLats-1 )	lat -= (NumLats-1);
	if( lng > NumLngs-1 )	lng -= (NumLngs-1);
	return &Pts[ NumLngs*lat + lng ];
}

void
DrawPoint( struct point *p )
{
	glNormal3f( p->nx, p->ny, p->nz );
	glTexCoord2f( p->s, p->t );
	glVertex3f( p->x, p->y, p->z );
}

void
MjbSphere( float radius, int slices, int stacks )
{
	struct point top, bot;		// top, bottom points
	struct point *p;

	// set the globals:

	NumLngs = slices;
	NumLats = stacks;

	if( NumLngs < 3 )
		NumLngs = 3;

	if( NumLats < 3 )
		NumLats = 3;


	// allocate the point data structure:

	Pts = new struct point[ NumLngs * NumLats ];


	// fill the Pts structure:
	float timestand;
	if(Time < .5)
		timestand = Time;
	else{
		timestand = .5 - (Time - .5);
	}
	for( int ilat = 0; ilat < NumLats; ilat++ )
	{
		float lat = -M_PI/2.  +  M_PI * (float)ilat / (float)(NumLats-1);
		float xz = cos( lat );
		float y = sin( lat );
		for( int ilng = 0; ilng < NumLngs; ilng++ )
		{
			float lng = -M_PI  +  2. * M_PI * (float)ilng / (float)(NumLngs-1);
			float x =  xz * cos( lng );
			float z = -xz * sin( lng );
			p = PtsPointer( ilat, ilng );
			p->x  = radius * x;
			p->y  = radius * y;
			p->z  = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;

			p->s = ( lng + M_PI    ) / ( 2.*M_PI );
			p->t = ( lat + M_PI/2. ) / M_PI;
		}
	}

	top.x =  0.;		top.y  = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s  = 0.;		top.t  = 1.;

	bot.x =  0.;		bot.y  = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s  = 0.;		bot.t  =  0.;


	// connect the north pole to the latitude NumLats-2:

	glBegin( GL_QUADS );
	for( int ilng = 0; ilng < NumLngs-1; ilng++ )
	{
		p = PtsPointer( NumLats-1, ilng );
		DrawPoint( p );

		p = PtsPointer( NumLats-2, ilng );
		DrawPoint( p );

		p = PtsPointer( NumLats-2, ilng+1 );
		DrawPoint( p );

		p = PtsPointer( NumLats-1, ilng+1 );
		DrawPoint( p );
	}
	glEnd( );

	// connect the south pole to the latitude 1:

	glBegin( GL_QUADS );
	for( int ilng = 0; ilng < NumLngs-1; ilng++ )
	{
		p = PtsPointer( 0, ilng );
		DrawPoint( p );

		p = PtsPointer( 0, ilng+1 );
		DrawPoint( p );

		p = PtsPointer( 1, ilng+1 );
		DrawPoint( p );

		p = PtsPointer( 1, ilng );
		DrawPoint( p );
	}
	glEnd( );


	// connect the other 4-sided polygons:

	glBegin( GL_QUADS );
	for( int ilat = 2; ilat < NumLats-1; ilat++ )
	{
		for( int ilng = 0; ilng < NumLngs-1; ilng++ )
		{
			p = PtsPointer( ilat-1, ilng );
			DrawPoint( p );

			p = PtsPointer( ilat-1, ilng+1 );
			DrawPoint( p );

			p = PtsPointer( ilat, ilng+1 );
			DrawPoint( p );

			p = PtsPointer( ilat, ilng );
			DrawPoint( p );
		}
	}
	glEnd( );

	delete [ ] Pts;
	Pts = NULL;
}

//BMP Importer
unsigned char *
BmpToTexture( char *filename, int *width, int *height )
{

	int s, t, e;		// counters
	int numextra;		// # extra bytes each line in the file is padded with
	FILE *fp;
	unsigned char *texture;
	int nums, numt;
	unsigned char *tp;


	fp = fopen( filename, "rb" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
	}

	FileHeader.bfType = ReadShort( fp );


	// if bfType is not 0x4d42, the file is not a bmp:

	if( FileHeader.bfType != 0x4d42 )
	{
		fprintf( stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType );
		fclose( fp );
		return NULL;
	}


	FileHeader.bfSize = ReadInt( fp );
	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );
	FileHeader.bfOffBits = ReadInt( fp );


	InfoHeader.biSize = ReadInt( fp );
	InfoHeader.biWidth = ReadInt( fp );
	InfoHeader.biHeight = ReadInt( fp );

	nums = InfoHeader.biWidth;
	numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );
	InfoHeader.biBitCount = ReadShort( fp );
	InfoHeader.biCompression = ReadInt( fp );
	InfoHeader.biSizeImage = ReadInt( fp );
	InfoHeader.biXPelsPerMeter = ReadInt( fp );
	InfoHeader.biYPelsPerMeter = ReadInt( fp );
	InfoHeader.biClrUsed = ReadInt( fp );
	InfoHeader.biClrImportant = ReadInt( fp );


	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );


	texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\b" );
		return NULL;
	}


	// extra padding bytes:

	numextra =  4*(( (3*InfoHeader.biWidth)+3)/4) - 3*InfoHeader.biWidth;


	// we do not support compression:

	if( InfoHeader.biCompression != birgb )
	{
		fprintf( stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}



	rewind( fp );
	fseek( fp, 14+40, SEEK_SET );

	if( InfoHeader.biBitCount == 24 )
	{
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( e = 0; e < numextra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}



int
ReadInt( FILE *fp )
{
	unsigned char b3, b2, b1, b0;
	b0 = fgetc( fp );
	b1 = fgetc( fp );
	b2 = fgetc( fp );
	b3 = fgetc( fp );
	return ( b3 << 24 )  |  ( b2 << 16 )  |  ( b1 << 8 )  |  b0;
}


short
ReadShort( FILE *fp )
{
	unsigned char b1, b0;
	b0 = fgetc( fp );
	b1 = fgetc( fp );
	return ( b1 << 8 )  |  b0;
}

// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{

				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:

	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;

		case 1:
			r = q;	g = v;	b = p;
			break;

		case 2:
			r = p;	g = v;	b = t;
			break;

		case 3:
			r = p;	g = q;	b = v;
			break;

		case 4:
			r = t;	g = p;	b = v;
			break;

		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

// utility to create an array from 3 separate values:
float *
Array3( float a, float b, float c )
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:
float *
MulArray3( float factor, float array0[3] )
{
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

void
SetMaterial( float r, float g, float b, float shininess )
{
	glMaterialfv( GL_BACK, GL_EMISSION, Array3( 0., 0., 0. ) );
	glMaterialfv( GL_BACK, GL_AMBIENT, MulArray3( .4f, White ) );
	glMaterialfv( GL_BACK, GL_DIFFUSE, MulArray3( 1., White ) );
	glMaterialfv( GL_BACK, GL_SPECULAR, Array3( 0., 0., 0. ) );
	glMaterialf ( GL_BACK, GL_SHININESS, 2.f );
	glMaterialfv( GL_FRONT, GL_EMISSION, Array3( 0., 0., 0. ) );
	glMaterialfv( GL_FRONT, GL_AMBIENT, Array3( r, g, b ) );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, Array3( r, g, b ) );
	glMaterialfv( GL_FRONT, GL_SPECULAR, MulArray3( .8f, White ) );
	glMaterialf ( GL_FRONT, GL_SHININESS, shininess );
	glColor3f(r, g, b);
}

void
SetPointLight( int ilight, float x, float y, float z, float r, float g, float b )
{
	glLightfv( ilight, GL_POSITION, Array3( x, y, z ) );
	glLightfv( ilight, GL_AMBIENT, Array3( 0., 0., 0. ) );
	glLightfv( ilight, GL_DIFFUSE, Array3( r, g, b ) );
	glLightfv( ilight, GL_SPECULAR, Array3( r, g, b ) );
	glLightf ( ilight, GL_CONSTANT_ATTENUATION, 1. );
	glLightf ( ilight, GL_LINEAR_ATTENUATION, 0. );
	glLightf ( ilight, GL_QUADRATIC_ATTENUATION, 0. );
	glEnable( ilight );
}

void
SetSpotLight( int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b )
{
	glLightfv( ilight, GL_POSITION, Array3( x, y, z ) );
	glLightfv( ilight, GL_SPOT_DIRECTION, Array3(xdir,ydir,zdir) );
	glLightf( ilight, GL_SPOT_EXPONENT, .8 );
	glLightf( ilight, GL_SPOT_CUTOFF, 45. );
	glLightfv( ilight, GL_AMBIENT, Array3( 0., 0., 0. ) );
	glLightfv( ilight, GL_DIFFUSE, Array3( r, g, b ) );
	glLightfv( ilight, GL_SPECULAR, Array3( r, g, b ) );
	glLightf ( ilight, GL_CONSTANT_ATTENUATION, .7 );
	glLightf ( ilight, GL_LINEAR_ATTENUATION, 0. );
	glLightf ( ilight, GL_QUADRATIC_ATTENUATION, 0. );
	glEnable( ilight );
}

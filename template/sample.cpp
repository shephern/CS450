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

const char *WINDOWTITLE = { "Assignment 1: Nathan Shepherd" };
const char *GLUITITLE   = { "Window Iterface" };


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

const GLfloat BACKCOLOR[ ] = { 0.2, 0.2, 0.2, 1. };


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
GLuint	BoxList;				// object display list
GLuint  TowerList;
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
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


	// set the eye position, look-at position, and up-vector:

	gluLookAt( 0., 0., 3.,     0., 0., 0.,     0., 1., 0. );


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


	// draw the current object:

	//glCallList( BoxList );
	glCallList( TowerList );

	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.,   0., 1., 0. );
			glCallList( TowerList );
		glPopMatrix( );
	}


	// draw some gratuitous text that just rotates on top of the scene:

	// glDisable( GL_DEPTH_TEST );
	// glColor3f( 0., 1., 1. );
	// DoRasterString( 0., 1., 0., "Text That Moves" );


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
	gluOrtho2D( 0., 100.,     0., 100. );
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
	glutIdleFunc( NULL );

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
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );


	// create the object:
	TowerList = glGenLists( 1 );
	glNewList( TowerList, GL_COMPILE );
		glBegin( GL_QUADS );

		// rock
		glColor3f( 0.467201292514801, 0.336258858442307, 0.398597180843353 );
			glNormal3f( 0.000000000000000, -0.999999940395355, 0.000000000000000 );
				glVertex3f( 0.353552460670471, -1.976793646812439, -0.353552669286728);
				glVertex3f( 0.499998629093170, -1.976793646812439, -0.000000101015686);
				glVertex3f( 0.353552460670471, -1.976793646812439, 0.353552371263504);
				glVertex3f( -0.000000047497451, -1.976793646812439, 0.499998569488525);

			glNormal3f( 0.382683455944061, 0.000000000000000, -0.923879504203796 );
				glVertex3f( -0.000000003725290, -1.976793646812439, -0.499998897314072);
				glVertex3f( -0.000000003725290, -0.024526119232178, -0.499998897314072);
				glVertex3f( 0.353552460670471, -0.024526238441467, -0.353552669286728);
				glVertex3f( 0.353552460670471, -1.976793646812439, -0.353552669286728);

			glNormal3f( 0.923879563808441, 0.000000000000000, -0.382683187723160 );
				glVertex3f( 0.353552460670471, -1.976793646812439, -0.353552669286728);
				glVertex3f( 0.353552460670471, -0.024526238441467, -0.353552669286728);
				glVertex3f( 0.499998629093170, -0.024526238441467, -0.000000101015686);
				glVertex3f( 0.499998629093170, -1.976793646812439, -0.000000101015686);

			glNormal3f( 0.923879623413086, 0.000000004986345, 0.382683306932449 );
				glVertex3f( 0.499998629093170, -1.976793646812439, -0.000000101015686);
				glVertex3f( 0.499998629093170, -0.024526238441467, -0.000000101015686);
				glVertex3f( 0.353552460670471, -0.024526119232178, 0.353552311658859);
				glVertex3f( 0.353552460670471, -1.976793646812439, 0.353552371263504);

			glNormal3f( 0.382683336734772, 0.000000012465859, 0.923879504203796 );
				glVertex3f( 0.353552460670471, -1.976793646812439, 0.353552371263504);
				glVertex3f( 0.353552460670471, -0.024526119232178, 0.353552311658859);
				glVertex3f( -0.000000047497451, -0.024526119232178, 0.499998569488525);
				glVertex3f( -0.000000047497451, -1.976793646812439, 0.499998569488525);

			glNormal3f( -0.382683485746384, 0.000000000000000, 0.923879504203796 );
				glVertex3f( -0.000000047497451, -1.976793646812439, 0.499998569488525);
				glVertex3f( -0.000000047497451, -0.024526119232178, 0.499998569488525);
				glVertex3f( -0.353552550077438, -0.024526357650757, 0.353552311658859);
				glVertex3f( -0.353552550077438, -1.976793646812439, 0.353552311658859);

			glNormal3f( -0.923879623413086, 0.000000012465863, 0.382683306932449 );
				glVertex3f( -0.353552550077438, -1.976793646812439, 0.353552311658859);
				glVertex3f( -0.353552550077438, -0.024526357650757, 0.353552311658859);
				glVertex3f( -0.499998718500137, -0.024526238441467, -0.000000220224976);
				glVertex3f( -0.499998718500137, -1.976793646812439, -0.000000101015686);

			glNormal3f( -0.382683336734772, 0.000000000000000, -0.923879563808441 );
				glVertex3f( -0.353552550077438, -1.976793646812439, -0.353552669286728);
				glVertex3f( -0.353552550077438, -0.024526119232178, -0.353552669286728);
				glVertex3f( -0.000000003725290, -0.024526119232178, -0.499998897314072);
				glVertex3f( -0.000000003725290, -1.976793646812439, -0.499998897314072);

			glNormal3f( -0.923879563808441, -0.000000012465860, -0.382683247327805 );
				glVertex3f( -0.499998718500137, -1.976793646812439, -0.000000101015686);
				glVertex3f( -0.499998718500137, -0.024526238441467, -0.000000220224976);
				glVertex3f( -0.353552550077438, -0.024526119232178, -0.353552669286728);
				glVertex3f( -0.353552550077438, -1.976793646812439, -0.353552669286728);

			glNormal3f( 0.000000000000000, -0.999999940395355, 0.000000000000000 );
				glVertex3f( -0.353552550077438, -1.976793646812439, -0.353552669286728);
				glVertex3f( -0.000000003725290, -1.976793646812439, -0.499998897314072);
				glVertex3f( -0.353552550077438, -1.976793646812439, 0.353552311658859);
				glVertex3f( -0.499998718500137, -1.976793646812439, -0.000000101015686);

			glNormal3f( 0.000000000000000, -0.999999940395355, 0.000000000000000 );
				glVertex3f( -0.000000003725290, -1.976793646812439, -0.499998897314072);
				glVertex3f( 0.353552460670471, -1.976793646812439, -0.353552669286728);
				glVertex3f( -0.000000047497451, -1.976793646812439, 0.499998569488525);
				glVertex3f( -0.353552550077438, -1.976793646812439, 0.353552311658859);

		// wood
		glColor3f( 0.467201381921768, 0.163369387388229, 0.052344761788845 );
			glNormal3f( 0.297244101762772, -0.629825532436371, -0.717611134052277 );
				glVertex3f( 0.353552460670471, -0.024526238441467, -0.353552669286728);
				glVertex3f( -0.000000003725290, -0.024526119232178, -0.499998897314072);
				glVertex3f( 0.000000001396984, 0.199942588806152, -0.697008252143860);
				glVertex3f( 0.492859125137329, 0.199942588806152, -0.492859452962875);

			glNormal3f( 0.717611253261566, -0.629825413227081, -0.297244161367416 );
				glVertex3f( 0.499998629093170, -0.024526238441467, -0.000000101015686);
				glVertex3f( 0.353552460670471, -0.024526238441467, -0.353552669286728);
				glVertex3f( 0.492859125137329, 0.199942588806152, -0.492859452962875);
				glVertex3f( 0.697008073329926, 0.199942588806152, -0.000000101015686);

			glNormal3f( 0.717611074447632, -0.629825532436371, 0.297244310379028 );
				glVertex3f( 0.353552460670471, -0.024526119232178, 0.353552311658859);
				glVertex3f( 0.499998629093170, -0.024526238441467, -0.000000101015686);
				glVertex3f( 0.697008073329926, 0.199942588806152, -0.000000101015686);
				glVertex3f( 0.492859125137329, 0.199942588806152, 0.492859035730362);

			glNormal3f( 0.297244191169739, -0.629825532436371, 0.717611074447632 );
				glVertex3f( -0.000000047497451, -0.024526119232178, 0.499998569488525);
				glVertex3f( 0.353552460670471, -0.024526119232178, 0.353552311658859);
				glVertex3f( 0.492859125137329, 0.199942588806152, 0.492859035730362);
				glVertex3f( -0.000000059604645, 0.199942588806152, 0.697007894515991);

			glNormal3f( -0.297244220972061, -0.629825174808502, 0.717611432075500 );
				glVertex3f( -0.353552550077438, -0.024526357650757, 0.353552311658859);
				glVertex3f( -0.000000047497451, -0.024526119232178, 0.499998569488525);
				glVertex3f( -0.000000059604645, 0.199942588806152, 0.697007894515991);
				glVertex3f( -0.492859214544296, 0.199942588806152, 0.492858916521072);

			glNormal3f( -0.717611312866211, -0.629825174808502, 0.297244191169739 );
				glVertex3f( -0.499998718500137, -0.024526238441467, -0.000000220224976);
				glVertex3f( -0.353552550077438, -0.024526357650757, 0.353552311658859);
				glVertex3f( -0.492859214544296, 0.199942588806152, 0.492858916521072);
				glVertex3f( -0.697008073329926, 0.199942588806152, -0.000000101015686);

			glNormal3f( -0.297244310379028, -0.629825353622437, -0.717611193656921 );
				glVertex3f( -0.000000003725290, -0.024526119232178, -0.499998897314072);
				glVertex3f( -0.353552550077438, -0.024526119232178, -0.353552669286728);
				glVertex3f( -0.492859214544296, 0.199942588806152, -0.492859214544296);
				glVertex3f( 0.000000001396984, 0.199942588806152, -0.697008252143860);

			glNormal3f( -0.717611312866211, -0.629825234413147, -0.297244310379028 );
				glVertex3f( -0.353552550077438, -0.024526119232178, -0.353552669286728);
				glVertex3f( -0.499998718500137, -0.024526238441467, -0.000000220224976);
				glVertex3f( -0.697008073329926, 0.199942588806152, -0.000000101015686);
				glVertex3f( -0.492859214544296, 0.199942588806152, -0.492859214544296);

			glNormal3f( -0.923879504203796, -0.000000066734309, -0.382683217525482 );
				glVertex3f( -0.697008073329926, 0.199942588806152, -0.000000101015686);
				glVertex3f( -0.697008073329926, 0.932432174682617, -0.000000101015686);
				glVertex3f( -0.492859214544296, 0.932432174682617, -0.492859452962875);
				glVertex3f( -0.492859214544296, 0.199942588806152, -0.492859214544296);

			glNormal3f( -0.382683664560318, 0.000000000000000, 0.923879384994507 );
				glVertex3f( -0.000000059604645, 0.199942588806152, 0.697007894515991);
				glVertex3f( -0.000000059604645, 0.932432174682617, 0.697008013725281);
				glVertex3f( -0.492859214544296, 0.932432174682617, 0.492858797311783);
				glVertex3f( -0.492859214544296, 0.199942588806152, 0.492858916521072);

			glNormal3f( 0.923879504203796, 0.000000057200857, 0.382683545351028 );
				glVertex3f( 0.697008073329926, 0.199942588806152, -0.000000101015686);
				glVertex3f( 0.697008073329926, 0.932432174682617, -0.000000101015686);
				glVertex3f( 0.492859125137329, 0.932432174682617, 0.492858797311783);
				glVertex3f( 0.492859125137329, 0.199942588806152, 0.492859035730362);

			glNormal3f( 0.382683187723160, 0.000000000000000, -0.923879623413086 );
				glVertex3f( 0.000000001396984, 0.199942588806152, -0.697008252143860);
				glVertex3f( 0.000000001396984, 0.932432174682617, -0.697008252143860);
				glVertex3f( 0.492859125137329, 0.932432174682617, -0.492859452962875);
				glVertex3f( 0.492859125137329, 0.199942588806152, -0.492859452962875);

			glNormal3f( 0.923879504203796, 0.000000000000000, -0.382683426141739 );
				glVertex3f( 0.531127452850342, 0.832432270050049, -0.400471538305283);
				glVertex3f( 0.531127452850342, 0.299942493438721, -0.400471538305283);
				glVertex3f( 0.492859125137329, 0.199942588806152, -0.492859452962875);
				glVertex3f( 0.492859125137329, 0.932432174682617, -0.492859452962875);

			glNormal3f( 0.923879504203796, 0.000000000000000, -0.382683426141739 );
				glVertex3f( 0.658739745616913, 0.299942493438721, -0.092388018965721);
				glVertex3f( 0.658739745616913, 0.832432270050049, -0.092388018965721);
				glVertex3f( 0.697008073329926, 0.932432174682617, -0.000000101015686);
				glVertex3f( 0.697008073329926, 0.199942588806152, -0.000000101015686);

			glNormal3f( 0.923879504203796, 0.000000000000000, -0.382683277130127 );
				glVertex3f( 0.531127452850342, 0.299942493438721, -0.400471538305283);
				glVertex3f( 0.658739745616913, 0.299942493438721, -0.092388018965721);
				glVertex3f( 0.697008073329926, 0.199942588806152, -0.000000101015686);
				glVertex3f( 0.492859125137329, 0.199942588806152, -0.492859452962875);

			glNormal3f( 0.923879504203796, 0.000000000000000, -0.382683277130127 );
				glVertex3f( 0.658739745616913, 0.832432270050049, -0.092388018965721);
				glVertex3f( 0.531127452850342, 0.832432270050049, -0.400471538305283);
				glVertex3f( 0.492859125137329, 0.932432174682617, -0.492859452962875);
				glVertex3f( 0.697008073329926, 0.932432174682617, -0.000000101015686);

			glNormal3f( 0.382683545351028, 0.000000357074128, 0.923879504203796 );
				glVertex3f( 0.400471150875092, 0.832432270050049, 0.531127154827118);
				glVertex3f( 0.400471180677414, 0.299942493438721, 0.531127393245697);
				glVertex3f( 0.492859125137329, 0.199942588806152, 0.492859035730362);
				glVertex3f( 0.492859125137329, 0.932432174682617, 0.492858797311783);

			glNormal3f( 0.382683783769608, -0.000000184058820, 0.923879384994507 );
				glVertex3f( 0.092387877404690, 0.299942493438721, 0.658739507198334);
				glVertex3f( 0.092387907207012, 0.832432270050049, 0.658739626407623);
				glVertex3f( -0.000000059604645, 0.932432174682617, 0.697008013725281);
				glVertex3f( -0.000000059604645, 0.199942588806152, 0.697007894515991);

			glNormal3f( 0.382683187723160, 0.000000085941835, 0.923879683017731 );
				glVertex3f( 0.400471180677414, 0.299942493438721, 0.531127393245697);
				glVertex3f( 0.092387877404690, 0.299942493438721, 0.658739507198334);
				glVertex3f( -0.000000059604645, 0.199942588806152, 0.697007894515991);
				glVertex3f( 0.492859125137329, 0.199942588806152, 0.492859035730362);

			glNormal3f( 0.382683932781219, -0.000000171883627, 0.923879384994507 );
				glVertex3f( 0.092387907207012, 0.832432270050049, 0.658739626407623);
				glVertex3f( 0.400471150875092, 0.832432270050049, 0.531127154827118);
				glVertex3f( 0.492859125137329, 0.932432174682617, 0.492858797311783);
				glVertex3f( -0.000000059604645, 0.932432174682617, 0.697008013725281);

			glNormal3f( -0.923879504203796, 0.000000053377072, 0.382683634757996 );
				glVertex3f( -0.531127572059631, 0.832432270050049, 0.400470882654190);
				glVertex3f( -0.531127572059631, 0.299942493438721, 0.400470942258835);
				glVertex3f( -0.492859214544296, 0.199942588806152, 0.492858916521072);
				glVertex3f( -0.492859214544296, 0.932432174682617, 0.492858797311783);

			glNormal3f( -0.923879623413086, 0.000000000000000, 0.382683247327805 );
				glVertex3f( -0.658739745616913, 0.299942493438721, 0.092387869954109);
				glVertex3f( -0.658739745616913, 0.832432270050049, 0.092387869954109);
				glVertex3f( -0.697008073329926, 0.932432174682617, -0.000000101015686);
				glVertex3f( -0.697008073329926, 0.199942588806152, -0.000000101015686);

			glNormal3f( -0.923879504203796, -0.000000085941863, 0.382683396339417 );
				glVertex3f( -0.531127572059631, 0.299942493438721, 0.400470942258835);
				glVertex3f( -0.658739745616913, 0.299942493438721, 0.092387869954109);
				glVertex3f( -0.697008073329926, 0.199942588806152, -0.000000101015686);
				glVertex3f( -0.492859214544296, 0.199942588806152, 0.492858916521072);

			glNormal3f( -0.923879504203796, 0.000000257825633, 0.382683455944061 );
				glVertex3f( -0.658739745616913, 0.832432270050049, 0.092387869954109);
				glVertex3f( -0.531127572059631, 0.832432270050049, 0.400470882654190);
				glVertex3f( -0.492859214544296, 0.932432174682617, 0.492858797311783);
				glVertex3f( -0.697008073329926, 0.932432174682617, -0.000000101015686);

			glNormal3f( -0.382683485746384, -0.000000347871122, -0.923879504203796 );
				glVertex3f( -0.400471240282059, 0.832432270050049, -0.531127810478210);
				glVertex3f( -0.400471240282059, 0.299942493438721, -0.531127572059631);
				glVertex3f( -0.492859214544296, 0.199942588806152, -0.492859214544296);
				glVertex3f( -0.492859214544296, 0.932432174682617, -0.492859452962875);

			glNormal3f( -0.382683604955673, -0.000000085941814, -0.923879504203796 );
				glVertex3f( -0.400471240282059, 0.299942493438721, -0.531127572059631);
				glVertex3f( -0.092387959361076, 0.299942493438721, -0.658739924430847);
				glVertex3f( 0.000000001396984, 0.199942588806152, -0.697008252143860);
				glVertex3f( -0.492859214544296, 0.199942588806152, -0.492859214544296);

			glNormal3f( -0.382683128118515, 0.000000085941835, -0.923879683017731 );
				glVertex3f( -0.092387944459915, 0.832432270050049, -0.658739924430847);
				glVertex3f( -0.400471240282059, 0.832432270050049, -0.531127810478210);
				glVertex3f( -0.492859214544296, 0.932432174682617, -0.492859452962875);
				glVertex3f( 0.000000001396984, 0.932432174682617, -0.697008252143860);

			glNormal3f( -0.382683336734772, 0.000000003681177, -0.923879623413086 );
				glVertex3f( -0.092387959361076, 0.299942493438721, -0.658739924430847);
				glVertex3f( -0.092387944459915, 0.832432270050049, -0.658739924430847);
				glVertex3f( 0.000000001396984, 0.932432174682617, -0.697008252143860);
				glVertex3f( 0.000000001396984, 0.199942588806152, -0.697008252143860);

		// shadow
		glColor3f( 0.049549847841263, 0.049549847841263, 0.049549847841263 );
			glNormal3f( 0.091282308101654, -0.971134603023529, -0.220374941825867 );
				glVertex3f( 0.000000001396984, 0.932432174682617, -0.697008252143860);
				glVertex3f( -0.000000000465661, 0.975406408309937, -0.886384606361389);
				glVertex3f( 0.626768231391907, 0.975406408309937, -0.626768469810486);
				glVertex3f( 0.492859125137329, 0.932432174682617, -0.492859452962875);

			glNormal3f( -0.382683366537094, -0.000000209795445, -0.923879563808441 );
				glVertex3f( -0.400471240282059, 0.299942493438721, -0.531127572059631);
				glVertex3f( -0.400471240282059, 0.832432270050049, -0.531127810478210);
				glVertex3f( -0.092387944459915, 0.832432270050049, -0.658739924430847);
				glVertex3f( -0.092387959361076, 0.299942493438721, -0.658739924430847);

			glNormal3f( -0.923879504203796, 0.000000020979563, 0.382683455944061 );
				glVertex3f( -0.531127572059631, 0.299942493438721, 0.400470942258835);
				glVertex3f( -0.531127572059631, 0.832432270050049, 0.400470882654190);
				glVertex3f( -0.658739745616913, 0.832432270050049, 0.092387869954109);
				glVertex3f( -0.658739745616913, 0.299942493438721, 0.092387869954109);

			glNormal3f( 0.382683515548706, 0.000000104897723, 0.923879504203796 );
				glVertex3f( 0.400471180677414, 0.299942493438721, 0.531127393245697);
				glVertex3f( 0.400471150875092, 0.832432270050049, 0.531127154827118);
				glVertex3f( 0.092387907207012, 0.832432270050049, 0.658739626407623);
				glVertex3f( 0.092387877404690, 0.299942493438721, 0.658739507198334);

			glNormal3f( 0.923879623413086, 0.000000000000000, -0.382683187723160 );
				glVertex3f( 0.531127452850342, 0.299942493438721, -0.400471538305283);
				glVertex3f( 0.531127452850342, 0.832432270050049, -0.400471538305283);
				glVertex3f( 0.658739745616913, 0.832432270050049, -0.092388018965721);
				glVertex3f( 0.658739745616913, 0.299942493438721, -0.092388018965721);

			glNormal3f( 0.220374390482903, -0.971134781837463, -0.091282270848751 );
				glVertex3f( 0.492859125137329, 0.932432174682617, -0.492859452962875);
				glVertex3f( 0.626768231391907, 0.975406408309937, -0.626768469810486);
				glVertex3f( 0.886384248733521, 0.975406169891357, -0.000000101015686);
				glVertex3f( 0.697008073329926, 0.932432174682617, -0.000000101015686);

			glNormal3f( 0.220374330878258, -0.971134781837463, 0.091282300651073 );
				glVertex3f( 0.697008073329926, 0.932432174682617, -0.000000101015686);
				glVertex3f( 0.886384248733521, 0.975406169891357, -0.000000101015686);
				glVertex3f( 0.626768231391907, 0.975406408309937, 0.626767992973328);
				glVertex3f( 0.492859125137329, 0.932432174682617, 0.492858797311783);

			glNormal3f( 0.091282323002815, -0.971134781837463, 0.220374315977097 );
				glVertex3f( 0.492859125137329, 0.932432174682617, 0.492858797311783);
				glVertex3f( 0.626768231391907, 0.975406408309937, 0.626767992973328);
				glVertex3f( -0.000000078231096, 0.975406169891357, 0.886384129524231);
				glVertex3f( -0.000000059604645, 0.932432174682617, 0.697008013725281);

			glNormal3f( -0.091282330453396, -0.971134722232819, 0.220374301075935 );
				glVertex3f( -0.000000059604645, 0.932432174682617, 0.697008013725281);
				glVertex3f( -0.000000078231096, 0.975406169891357, 0.886384129524231);
				glVertex3f( -0.626768290996552, 0.975406408309937, 0.626767992973328);
				glVertex3f( -0.492859214544296, 0.932432174682617, 0.492858797311783);

			glNormal3f( -0.220375046133995, -0.971134603023529, 0.091282337903976 );
				glVertex3f( -0.492859214544296, 0.932432174682617, 0.492858797311783);
				glVertex3f( -0.626768290996552, 0.975406408309937, 0.626767992973328);
				glVertex3f( -0.886384248733521, 0.975406408309937, -0.000000339434251);
				glVertex3f( -0.697008073329926, 0.932432174682617, -0.000000101015686);

			glNormal3f( -0.091282293200493, -0.971134543418884, -0.220374956727028 );
				glVertex3f( -0.492859214544296, 0.932432174682617, -0.492859452962875);
				glVertex3f( -0.626768290996552, 0.975406408309937, -0.626768469810486);
				glVertex3f( -0.000000000465661, 0.975406408309937, -0.886384606361389);
				glVertex3f( 0.000000001396984, 0.932432174682617, -0.697008252143860);

			glNormal3f( -0.220375001430511, -0.971134543418884, -0.091282293200493 );
				glVertex3f( -0.697008073329926, 0.932432174682617, -0.000000101015686);
				glVertex3f( -0.886384248733521, 0.975406408309937, -0.000000339434251);
				glVertex3f( -0.626768290996552, 0.975406408309937, -0.626768469810486);
				glVertex3f( -0.492859214544296, 0.932432174682617, -0.492859452962875);

		// roof
		glColor3f( 0.800000071525574, 0.492244005203247, 0.211661756038666 );
			glNormal3f( 0.373689621686935, 0.215524241328239, -0.902167022228241 );
				glVertex3f( -0.000000021420419, 2.114771842956543, -0.193081006407738);
				glVertex3f( -0.000000024680048, 2.668584823608398, -0.060777287930250);
				glVertex3f( 0.042975895106792, 2.668584823608398, -0.042976003140211);
				glVertex3f( 0.136528700590134, 2.114771842956543, -0.136529073119164);

			glNormal3f( 0.902166903018951, 0.215523898601532, -0.373690247535706 );
				glVertex3f( 0.136528700590134, 2.114771842956543, -0.136529073119164);
				glVertex3f( 0.042975895106792, 2.668584823608398, -0.042976003140211);
				glVertex3f( 0.060777135193348, 2.668584823608398, -0.000000577852859);
				glVertex3f( 0.193080723285675, 2.114771842956543, -0.000000101015686);

			glNormal3f( 0.902167201042175, 0.215524032711983, 0.373689383268356 );
				glVertex3f( 0.193080723285675, 2.114771842956543, -0.000000101015686);
				glVertex3f( 0.060777135193348, 2.668584823608398, -0.000000577852859);
				glVertex3f( 0.042975895106792, 2.668584823608398, 0.042975801974535);
				glVertex3f( 0.136528700590134, 2.114771842956543, 0.136528387665749);

			glNormal3f( 0.373689651489258, 0.215523511171341, 0.902167201042175 );
				glVertex3f( 0.136528700590134, 2.114771842956543, 0.136528387665749);
				glVertex3f( 0.042975895106792, 2.668584823608398, 0.042975801974535);
				glVertex3f( -0.000000029802322, 2.668584823608398, 0.060777083039284);
				glVertex3f( -0.000000038184226, 2.114771842956543, 0.193080320954323);

			glNormal3f( -0.373689711093903, 0.215523481369019, 0.902167201042175 );
				glVertex3f( -0.000000038184226, 2.114771842956543, 0.193080320954323);
				glVertex3f( -0.000000029802322, 2.668584823608398, 0.060777083039284);
				glVertex3f( -0.042975962162018, 2.668584823608398, 0.042975801974535);
				glVertex3f( -0.136528715491295, 2.114771842956543, 0.136528387665749);

			glNormal3f( -0.902167558670044, 0.215523838996887, 0.373688489198685 );
				glVertex3f( -0.136528715491295, 2.114771842956543, 0.136528387665749);
				glVertex3f( -0.042975962162018, 2.668584823608398, 0.042975801974535);
				glVertex3f( -0.060777157545090, 2.668584823608398, -0.000000577852859);
				glVertex3f( -0.193080767989159, 2.114771842956543, -0.000000577852859);

			glNormal3f( -0.373689681291580, 0.215524226427078, -0.902167022228241 );
				glVertex3f( -0.136528715491295, 2.114771842956543, -0.136529073119164);
				glVertex3f( -0.042975962162018, 2.668584823608398, -0.042976003140211);
				glVertex3f( -0.000000024680048, 2.668584823608398, -0.060777287930250);
				glVertex3f( -0.000000021420419, 2.114771842956543, -0.193081006407738);

			glNormal3f( -0.902166485786438, 0.215524002909660, -0.373691022396088 );
				glVertex3f( -0.193080767989159, 2.114771842956543, -0.000000577852859);
				glVertex3f( -0.060777157545090, 2.668584823608398, -0.000000577852859);
				glVertex3f( -0.042975962162018, 2.668584823608398, -0.042976003140211);
				glVertex3f( -0.136528715491295, 2.114771842956543, -0.136529073119164);

			glNormal3f( 0.902091145515442, 0.215891331434250, -0.373660981655121 );
				glVertex3f( 0.060777135193348, 2.668584823608398, -0.000000577852859);
				glVertex3f( 0.042975895106792, 2.668584823608398, -0.042976003140211);
				glVertex3f( 0.021081736311316, 2.797962665557861, -0.021082025021315);
				glVertex3f( 0.029814103618264, 2.797962665557861, -0.000000101015686);

			glNormal3f( 0.373659431934357, 0.215890035033226, -0.902092099189758 );
				glVertex3f( 0.042975895106792, 2.668584823608398, -0.042976003140211);
				glVertex3f( -0.000000024680048, 2.668584823608398, -0.060777287930250);
				glVertex3f( -0.000000025145710, 2.797962665557861, -0.029814343899488);
				glVertex3f( 0.021081736311316, 2.797962665557861, -0.021082025021315);

			glNormal3f( 0.902092278003693, 0.215890645980835, 0.373658627271652 );
				glVertex3f( 0.042975895106792, 2.668584823608398, 0.042975801974535);
				glVertex3f( 0.060777135193348, 2.668584823608398, -0.000000577852859);
				glVertex3f( 0.029814103618264, 2.797962665557861, -0.000000101015686);
				glVertex3f( 0.021081736311316, 2.797962665557861, 0.021081347018480);

			glNormal3f( 0.373659014701843, 0.215893194079399, 0.902091503143311 );
				glVertex3f( -0.000000029802322, 2.668584823608398, 0.060777083039284);
				glVertex3f( 0.042975895106792, 2.668584823608398, 0.042975801974535);
				glVertex3f( 0.021081736311316, 2.797962665557861, 0.021081347018480);
				glVertex3f( -0.000000027939677, 2.797962665557861, 0.029813665896654);

			glNormal3f( -0.373658925294876, 0.215893164277077, 0.902091443538666 );
				glVertex3f( -0.042975962162018, 2.668584823608398, 0.042975801974535);
				glVertex3f( -0.000000029802322, 2.668584823608398, 0.060777083039284);
				glVertex3f( -0.000000027939677, 2.797962665557861, 0.029813665896654);
				glVertex3f( -0.021081794053316, 2.797962665557861, 0.021081347018480);

			glNormal3f( -0.902092576026917, 0.215890645980835, 0.373657852411270 );
				glVertex3f( -0.060777157545090, 2.668584823608398, -0.000000577852859);
				glVertex3f( -0.042975962162018, 2.668584823608398, 0.042975801974535);
				glVertex3f( -0.021081794053316, 2.797962665557861, 0.021081347018480);
				glVertex3f( -0.029814137145877, 2.797962665557861, -0.000000101015686);

			glNormal3f( -0.902091383934021, 0.215891316533089, -0.373660176992416 );
				glVertex3f( -0.042975962162018, 2.668584823608398, -0.042976003140211);
				glVertex3f( -0.060777157545090, 2.668584823608398, -0.000000577852859);
				glVertex3f( -0.029814137145877, 2.797962665557861, -0.000000101015686);
				glVertex3f( -0.021081794053316, 2.797962665557861, -0.021082025021315);

			glNormal3f( -0.373659282922745, 0.215890020132065, -0.902092099189758 );
				glVertex3f( -0.000000024680048, 2.668584823608398, -0.060777287930250);
				glVertex3f( -0.042975962162018, 2.668584823608398, -0.042976003140211);
				glVertex3f( -0.021081794053316, 2.797962665557861, -0.021082025021315);
				glVertex3f( -0.000000025145710, 2.797962665557861, -0.029814343899488);

			glNormal3f( 0.375290513038635, 0.195601776242256, -0.906033635139465 );
				glVertex3f( -0.000000011175871, 1.579615473747253, -0.308614820241928);
				glVertex3f( -0.000000021420419, 2.114771842956543, -0.193081006407738);
				glVertex3f( 0.136528700590134, 2.114771842956543, -0.136529073119164);
				glVertex3f( 0.218223497271538, 1.579615473747253, -0.218223914504051);

			glNormal3f( 0.906033635139465, 0.195601731538773, -0.375290423631668 );
				glVertex3f( 0.218223497271538, 1.579615473747253, -0.218223914504051);
				glVertex3f( 0.136528700590134, 2.114771842956543, -0.136529073119164);
				glVertex3f( 0.193080723285675, 2.114771842956543, -0.000000101015686);
				glVertex3f( 0.308614611625671, 1.579614996910095, -0.000000101015686);

			glNormal3f( 0.906033337116241, 0.195601746439934, 0.375291317701340 );
				glVertex3f( 0.308614611625671, 1.579614996910095, -0.000000101015686);
				glVertex3f( 0.193080723285675, 2.114771842956543, -0.000000101015686);
				glVertex3f( 0.136528700590134, 2.114771842956543, 0.136528387665749);
				glVertex3f( 0.218223497271538, 1.579615473747253, 0.218223229050636);

			glNormal3f( 0.375290811061859, 0.195601910352707, 0.906033456325531 );
				glVertex3f( 0.218223497271538, 1.579615473747253, 0.218223229050636);
				glVertex3f( 0.136528700590134, 2.114771842956543, 0.136528387665749);
				glVertex3f( -0.000000038184226, 2.114771842956543, 0.193080320954323);
				glVertex3f( -0.000000038184226, 1.579614996910095, 0.308614403009415);

			glNormal3f( -0.375291198492050, 0.195601835846901, 0.906033396720886 );
				glVertex3f( -0.000000038184226, 1.579614996910095, 0.308614403009415);
				glVertex3f( -0.000000038184226, 2.114771842956543, 0.193080320954323);
				glVertex3f( -0.136528715491295, 2.114771842956543, 0.136528387665749);
				glVertex3f( -0.218223512172699, 1.579614996910095, 0.218223229050636);

			glNormal3f( -0.906033277511597, 0.195601791143417, 0.375291198492050 );
				glVertex3f( -0.218223512172699, 1.579614996910095, 0.218223229050636);
				glVertex3f( -0.136528715491295, 2.114771842956543, 0.136528387665749);
				glVertex3f( -0.193080767989159, 2.114771842956543, -0.000000577852859);
				glVertex3f( -0.308614641427994, 1.579615473747253, -0.000000339434251);

			glNormal3f( -0.375291109085083, 0.195601582527161, -0.906033396720886 );
				glVertex3f( -0.218223512172699, 1.579615473747253, -0.218223676085472);
				glVertex3f( -0.136528715491295, 2.114771842956543, -0.136529073119164);
				glVertex3f( -0.000000021420419, 2.114771842956543, -0.193081006407738);
				glVertex3f( -0.000000011175871, 1.579615473747253, -0.308614820241928);

			glNormal3f( -0.906033158302307, 0.195601627230644, -0.375291615724564 );
				glVertex3f( -0.308614641427994, 1.579615473747253, -0.000000339434251);
				glVertex3f( -0.193080767989159, 2.114771842956543, -0.000000577852859);
				glVertex3f( -0.136528715491295, 2.114771842956543, -0.136529073119164);
				glVertex3f( -0.218223512172699, 1.579615473747253, -0.218223676085472);

			glNormal3f( 0.257779657840729, 0.739086508750916, -0.622334837913513 );
				glVertex3f( -0.000000000465661, 0.975406408309937, -0.886384606361389);
				glVertex3f( -0.000000005122274, 1.303405642509460, -0.496851772069931);
				glVertex3f( 0.351327151060104, 1.303405642509460, -0.351327270269394);
				glVertex3f( 0.626768231391907, 0.975406408309937, -0.626768469810486);

			glNormal3f( 0.622335195541382, 0.739086449146271, -0.257779330015182 );
				glVertex3f( 0.626768231391907, 0.975406408309937, -0.626768469810486);
				glVertex3f( 0.351327151060104, 1.303405642509460, -0.351327270269394);
				glVertex3f( 0.496851623058319, 1.303405404090881, -0.000000101015686);
				glVertex3f( 0.886384248733521, 0.975406169891357, -0.000000101015686);

			glNormal3f( 0.622335195541382, 0.739086389541626, 0.257779419422150 );
				glVertex3f( 0.886384248733521, 0.975406169891357, -0.000000101015686);
				glVertex3f( 0.496851623058319, 1.303405404090881, -0.000000101015686);
				glVertex3f( 0.351327151060104, 1.303405642509460, 0.351327091455460);
				glVertex3f( 0.626768231391907, 0.975406408309937, 0.626767992973328);

			glNormal3f( 0.257779300212860, 0.739086449146271, 0.622335195541382 );
				glVertex3f( 0.626768231391907, 0.975406408309937, 0.626767992973328);
				glVertex3f( 0.351327151060104, 1.303405642509460, 0.351327091455460);
				glVertex3f( -0.000000048428774, 1.303405404090881, 0.496851325035095);
				glVertex3f( -0.000000078231096, 0.975406169891357, 0.886384129524231);

			glNormal3f( -0.257779330015182, 0.739086449146271, 0.622335195541382 );
				glVertex3f( -0.000000078231096, 0.975406169891357, 0.886384129524231);
				glVertex3f( -0.000000048428774, 1.303405404090881, 0.496851325035095);
				glVertex3f( -0.351327151060104, 1.303405880928040, 0.351326853036880);
				glVertex3f( -0.626768290996552, 0.975406408309937, 0.626767992973328);

			glNormal3f( -0.622335195541382, 0.739086270332336, 0.257779508829117 );
				glVertex3f( -0.626768290996552, 0.975406408309937, 0.626767992973328);
				glVertex3f( -0.351327151060104, 1.303405880928040, 0.351326853036880);
				glVertex3f( -0.496851652860641, 1.303405642509460, -0.000000339434251);
				glVertex3f( -0.886384248733521, 0.975406408309937, -0.000000339434251);

			glNormal3f( -0.257779777050018, 0.739086627960205, -0.622334659099579 );
				glVertex3f( -0.626768290996552, 0.975406408309937, -0.626768469810486);
				glVertex3f( -0.351327151060104, 1.303405404090881, -0.351327270269394);
				glVertex3f( -0.000000005122274, 1.303405642509460, -0.496851772069931);
				glVertex3f( -0.000000000465661, 0.975406408309937, -0.886384606361389);

			glNormal3f( -0.622334837913513, 0.739086568355560, -0.257779836654663 );
				glVertex3f( -0.886384248733521, 0.975406408309937, -0.000000339434251);
				glVertex3f( -0.496851652860641, 1.303405642509460, -0.000000339434251);
				glVertex3f( -0.351327151060104, 1.303405404090881, -0.351327270269394);
				glVertex3f( -0.626768290996552, 0.975406408309937, -0.626768469810486);

			glNormal3f( 0.323840022087097, 0.532809495925903, -0.781819581985474 );
				glVertex3f( -0.000000005122274, 1.303405642509460, -0.496851772069931);
				glVertex3f( -0.000000011175871, 1.579615473747253, -0.308614820241928);
				glVertex3f( 0.218223497271538, 1.579615473747253, -0.218223914504051);
				glVertex3f( 0.351327151060104, 1.303405642509460, -0.351327270269394);

			glNormal3f( 0.781819462776184, 0.532809853553772, -0.323839336633682 );
				glVertex3f( 0.351327151060104, 1.303405642509460, -0.351327270269394);
				glVertex3f( 0.218223497271538, 1.579615473747253, -0.218223914504051);
				glVertex3f( 0.308614611625671, 1.579614996910095, -0.000000101015686);
				glVertex3f( 0.496851623058319, 1.303405404090881, -0.000000101015686);

			glNormal3f( 0.781819343566895, 0.532810032367706, 0.323839515447617 );
				glVertex3f( 0.496851623058319, 1.303405404090881, -0.000000101015686);
				glVertex3f( 0.308614611625671, 1.579614996910095, -0.000000101015686);
				glVertex3f( 0.218223497271538, 1.579615473747253, 0.218223229050636);
				glVertex3f( 0.351327151060104, 1.303405642509460, 0.351327091455460);

			glNormal3f( 0.323839217424393, 0.532810091972351, 0.781819343566895 );
				glVertex3f( 0.351327151060104, 1.303405642509460, 0.351327091455460);
				glVertex3f( 0.218223497271538, 1.579615473747253, 0.218223229050636);
				glVertex3f( -0.000000038184226, 1.579614996910095, 0.308614403009415);
				glVertex3f( -0.000000048428774, 1.303405404090881, 0.496851325035095);

			glNormal3f( -0.323839694261551, 0.532810389995575, 0.781819045543671 );
				glVertex3f( -0.000000048428774, 1.303405404090881, 0.496851325035095);
				glVertex3f( -0.000000038184226, 1.579614996910095, 0.308614403009415);
				glVertex3f( -0.218223512172699, 1.579614996910095, 0.218223229050636);
				glVertex3f( -0.351327151060104, 1.303405880928040, 0.351326853036880);

			glNormal3f( -0.781818807125092, 0.532810211181641, 0.323840171098709 );
				glVertex3f( -0.351327151060104, 1.303405880928040, 0.351326853036880);
				glVertex3f( -0.218223512172699, 1.579614996910095, 0.218223229050636);
				glVertex3f( -0.308614641427994, 1.579615473747253, -0.000000339434251);
				glVertex3f( -0.496851652860641, 1.303405642509460, -0.000000339434251);

			glNormal3f( -0.323840469121933, 0.532809555530548, -0.781819343566895 );
				glVertex3f( -0.351327151060104, 1.303405404090881, -0.351327270269394);
				glVertex3f( -0.218223512172699, 1.579615473747253, -0.218223676085472);
				glVertex3f( -0.000000011175871, 1.579615473747253, -0.308614820241928);
				glVertex3f( -0.000000005122274, 1.303405642509460, -0.496851772069931);

			glNormal3f( -0.781819164752960, 0.532809674739838, -0.323840618133545 );
				glVertex3f( -0.496851652860641, 1.303405642509460, -0.000000339434251);
				glVertex3f( -0.308614641427994, 1.579615473747253, -0.000000339434251);
				glVertex3f( -0.218223512172699, 1.579615473747253, -0.218223676085472);
				glVertex3f( -0.351327151060104, 1.303405404090881, -0.351327270269394);

			glNormal3f( -0.378849655389786, 0.141006976366043, -0.914652884006500 );
				glVertex3f( -0.000000025145710, 2.797962665557861, -0.029814343899488);
				glVertex3f( -0.021081794053316, 2.797962665557861, -0.021082025021315);
				glVertex3f( -0.009670881554484, 2.902638912200928, -0.009671311825514);
				glVertex3f( -0.000000025145710, 2.902638912200928, -0.013676743954420);

			glNormal3f( -0.914651095867157, 0.141006886959076, -0.378854125738144 );
				glVertex3f( -0.021081794053316, 2.797962665557861, -0.021082025021315);
				glVertex3f( -0.029814137145877, 2.797962665557861, -0.000000101015686);
				glVertex3f( -0.013676677830517, 2.902638912200928, -0.000000101015686);
				glVertex3f( -0.009670881554484, 2.902638912200928, -0.009671311825514);

			glNormal3f( -0.914646923542023, 0.141006886959076, 0.378864169120789 );
				glVertex3f( -0.029814137145877, 2.797962665557861, -0.000000101015686);
				glVertex3f( -0.021081794053316, 2.797962665557861, 0.021081347018480);
				glVertex3f( -0.009670881554484, 2.902638912200928, 0.009670633822680);
				glVertex3f( -0.013676677830517, 2.902638912200928, -0.000000101015686);

			glNormal3f( -0.378861904144287, 0.141004949808121, 0.914648115634918 );
				glVertex3f( -0.021081794053316, 2.797962665557861, 0.021081347018480);
				glVertex3f( -0.000000027939677, 2.797962665557861, 0.029813665896654);
				glVertex3f( -0.000000026542693, 2.902638912200928, 0.013676542788744);
				glVertex3f( -0.009670881554484, 2.902638912200928, 0.009670633822680);

			glNormal3f( 0.378861904144287, 0.141004920005798, 0.914648115634918 );
				glVertex3f( -0.000000027939677, 2.797962665557861, 0.029813665896654);
				glVertex3f( 0.021081736311316, 2.797962665557861, 0.021081347018480);
				glVertex3f( 0.009670829400420, 2.902638912200928, 0.009670633822680);
				glVertex3f( -0.000000026542693, 2.902638912200928, 0.013676542788744);

			glNormal3f( 0.914646506309509, 0.141006901860237, 0.378865003585815 );
				glVertex3f( 0.021081736311316, 2.797962665557861, 0.021081347018480);
				glVertex3f( 0.029814103618264, 2.797962665557861, -0.000000101015686);
				glVertex3f( 0.013676635921001, 2.902638912200928, -0.000000101015686);
				glVertex3f( 0.009670829400420, 2.902638912200928, 0.009670633822680);

			glNormal3f( 0.914650738239288, 0.141006886959076, -0.378854960203171 );
				glVertex3f( 0.029814103618264, 2.797962665557861, -0.000000101015686);
				glVertex3f( 0.021081736311316, 2.797962665557861, -0.021082025021315);
				glVertex3f( 0.009670829400420, 2.902638912200928, -0.009671311825514);
				glVertex3f( 0.013676635921001, 2.902638912200928, -0.000000101015686);

			glNormal3f( 0.378849774599075, 0.141006991267204, -0.914652824401855 );
				glVertex3f( 0.021081736311316, 2.797962665557861, -0.021082025021315);
				glVertex3f( -0.000000025145710, 2.797962665557861, -0.029814343899488);
				glVertex3f( -0.000000025145710, 2.902638912200928, -0.013676743954420);
				glVertex3f( 0.009670829400420, 2.902638912200928, -0.009671311825514);

		// flagpole
		glColor3f( 0.230541706085205, 0.426226735115051, 0.438546001911163 );
			glNormal3f( 0.382576704025269, 0.021739205345511, -0.923668026924133 );
				glVertex3f( 0.004596952348948, 3.809157848358154, -0.004597288090736);
				glVertex3f( -0.000000025145710, 3.809157848358154, -0.006501298863441);
				glVertex3f( -0.000000025145710, 3.987833023071289, -0.002296071965247);
				glVertex3f( 0.001623169984668, 3.987833023071289, -0.001623731572181);

			glNormal3f( 0.923664569854736, 0.021739913150668, -0.382584780454636 );
				glVertex3f( 0.006501086987555, 3.809157848358154, -0.000000101015686);
				glVertex3f( 0.004596952348948, 3.809157848358154, -0.004597288090736);
				glVertex3f( 0.001623169984668, 3.987833023071289, -0.001623731572181);
				glVertex3f( 0.002295520156622, 3.987833023071289, -0.000000577852859);

			glNormal3f( 0.923654139041901, 0.021741444244981, 0.382609814405441 );
				glVertex3f( 0.004596952348948, 3.809157848358154, 0.004596609156579);
				glVertex3f( 0.006501086987555, 3.809157848358154, -0.000000101015686);
				glVertex3f( 0.002295520156622, 3.987833023071289, -0.000000577852859);
				glVertex3f( 0.001623169984668, 3.987833023071289, 0.001622575800866);

			glNormal3f( 0.382576584815979, 0.021741667762399, 0.923667907714844 );
				glVertex3f( -0.000000025611371, 3.809157848358154, 0.006500619929284);
				glVertex3f( 0.004596952348948, 3.809157848358154, 0.004596609156579);
				glVertex3f( 0.001623169984668, 3.987833023071289, 0.001622575800866);
				glVertex3f( -0.000000025611371, 3.987833023071289, 0.002294916193932);

			glNormal3f( -0.382576614618301, 0.021741667762399, 0.923667907714844 );
				glVertex3f( -0.004597004037350, 3.809157848358154, 0.004596609156579);
				glVertex3f( -0.000000025611371, 3.809157848358154, 0.006500619929284);
				glVertex3f( -0.000000025611371, 3.987833023071289, 0.002294916193932);
				glVertex3f( -0.001623220741749, 3.987833023071289, 0.001622575800866);

			glNormal3f( -0.923654496669769, 0.021741436794400, 0.382608979940414 );
				glVertex3f( -0.006501133088022, 3.809157848358154, -0.000000101015686);
				glVertex3f( -0.004597004037350, 3.809157848358154, 0.004596609156579);
				glVertex3f( -0.001623220741749, 3.987833023071289, 0.001622575800866);
				glVertex3f( -0.002295569982380, 3.987833023071289, -0.000000577852859);

			glNormal3f( -0.923664867877960, 0.021739905700088, -0.382583945989609 );
				glVertex3f( -0.004597004037350, 3.809157848358154, -0.004597288090736);
				glVertex3f( -0.006501133088022, 3.809157848358154, -0.000000101015686);
				glVertex3f( -0.002295569982380, 3.987833023071289, -0.000000577852859);
				glVertex3f( -0.001623220741749, 3.987833023071289, -0.001623731572181);

			glNormal3f( 0.382646352052689, 0.007312791887671, -0.923866033554077 );
				glVertex3f( 0.009670829400420, 2.902638912200928, -0.009671311825514);
				glVertex3f( -0.000000025145710, 2.902638912200928, -0.013676743954420);
				glVertex3f( -0.000000025145710, 3.809157848358154, -0.006501298863441);
				glVertex3f( 0.004596952348948, 3.809157848358154, -0.004597288090736);

			glNormal3f( 0.923859894275665, 0.007312800269574, -0.382660895586014 );
				glVertex3f( 0.013676635921001, 2.902638912200928, -0.000000101015686);
				glVertex3f( 0.009670829400420, 2.902638912200928, -0.009671311825514);
				glVertex3f( 0.004596952348948, 3.809157848358154, -0.004597288090736);
				glVertex3f( 0.006501086987555, 3.809157848358154, -0.000000101015686);

			glNormal3f( 0.923850893974304, 0.007312800735235, 0.382682710886002 );
				glVertex3f( 0.009670829400420, 2.902638912200928, 0.009670633822680);
				glVertex3f( 0.013676635921001, 2.902638912200928, -0.000000101015686);
				glVertex3f( 0.006501086987555, 3.809157848358154, -0.000000101015686);
				glVertex3f( 0.004596952348948, 3.809157848358154, 0.004596609156579);

			glNormal3f( 0.382672578096390, 0.007313034962863, 0.923855066299438 );
				glVertex3f( -0.000000026542693, 2.902638912200928, 0.013676542788744);
				glVertex3f( 0.009670829400420, 2.902638912200928, 0.009670633822680);
				glVertex3f( 0.004596952348948, 3.809157848358154, 0.004596609156579);
				glVertex3f( -0.000000025611371, 3.809157848358154, 0.006500619929284);

			glNormal3f( -0.382672607898712, 0.007313035894185, 0.923855125904083 );
				glVertex3f( -0.009670881554484, 2.902638912200928, 0.009670633822680);
				glVertex3f( -0.000000026542693, 2.902638912200928, 0.013676542788744);
				glVertex3f( -0.000000025611371, 3.809157848358154, 0.006500619929284);
				glVertex3f( -0.004597004037350, 3.809157848358154, 0.004596609156579);

			glNormal3f( -0.923851251602173, 0.007312798872590, 0.382681876420975 );
				glVertex3f( -0.013676677830517, 2.902638912200928, -0.000000101015686);
				glVertex3f( -0.009670881554484, 2.902638912200928, 0.009670633822680);
				glVertex3f( -0.004597004037350, 3.809157848358154, 0.004596609156579);
				glVertex3f( -0.006501133088022, 3.809157848358154, -0.000000101015686);

			glNormal3f( -0.923860311508179, 0.007312798872590, -0.382660061120987 );
				glVertex3f( -0.009670881554484, 2.902638912200928, -0.009671311825514);
				glVertex3f( -0.013676677830517, 2.902638912200928, -0.000000101015686);
				glVertex3f( -0.006501133088022, 3.809157848358154, -0.000000101015686);
				glVertex3f( -0.004597004037350, 3.809157848358154, -0.004597288090736);

			glNormal3f( -0.382646262645721, 0.007312791887671, -0.923865973949432 );
				glVertex3f( -0.000000025145710, 2.902638912200928, -0.013676743954420);
				glVertex3f( -0.009670881554484, 2.902638912200928, -0.009671311825514);
				glVertex3f( -0.004597004037350, 3.809157848358154, -0.004597288090736);
				glVertex3f( -0.000000025145710, 3.809157848358154, -0.006501298863441);

		// flag
		glColor3f( 0.565982162952423, 0.000000000000000, 0.800000071525574 );
			glNormal3f( -0.010571804828942, -0.999621570110321, -0.025395628064871 );
				glVertex3f( -0.437714189291000, 3.832205295562744, -0.782070279121399);
				glVertex3f( -0.442311108112335, 3.832205295562744, -0.780165791511536);
				glVertex3f( -0.423102647066116, 3.837967395782471, -1.014960289001465);
				glVertex3f( -0.418505668640137, 3.837966918945312, -1.016864299774170);

			glNormal3f( 0.000000000000000, 1.000000000000000, 0.000000000000000 );
				glVertex3f( -0.002295569982380, 3.987833023071289, -0.000000577852859);
				glVertex3f( -0.001623220741749, 3.987833023071289, 0.001622575800866);
				glVertex3f( -0.000000025611371, 3.987833023071289, 0.002294916193932);
				glVertex3f( 0.001623169984668, 3.987833023071289, 0.001622575800866);

			glNormal3f( 0.000000000000000, 1.000000000000000, 0.000000000000000 );
				glVertex3f( -0.001623220741749, 3.987833023071289, -0.001623731572181);
				glVertex3f( -0.002295569982380, 3.987833023071289, -0.000000577852859);
				glVertex3f( 0.001623169984668, 3.987833023071289, 0.001622575800866);
				glVertex3f( 0.002295520156622, 3.987833023071289, -0.000000577852859);

			glNormal3f( 0.000000000000000, 1.000000000000000, 0.000000000000000 );
				glVertex3f( -0.000000025145710, 3.987833023071289, -0.002296071965247);
				glVertex3f( -0.001623220741749, 3.987833023071289, -0.001623731572181);
				glVertex3f( 0.002295520156622, 3.987833023071289, -0.000000577852859);
				glVertex3f( 0.001623169984668, 3.987833023071289, -0.001623731572181);

			glNormal3f( -0.380889922380447, 0.096068002283573, -0.919616162776947 );
				glVertex3f( -0.418505668640137, 3.837966918945312, -1.016864299774170);
				glVertex3f( -0.423102647066116, 3.837967395782471, -1.014960289001465);
				glVertex3f( -0.420128881931305, 3.878221988677979, -1.011986732482910);
				glVertex3f( -0.418505668640137, 3.878221988677979, -1.012659072875977);

			glNormal3f( -0.993837356567383, 0.076903574168682, -0.079831771552563 );
				glVertex3f( -0.442311108112335, 3.832205295562744, -0.780165791511536);
				glVertex3f( -0.437339514493942, 3.900144100189209, -0.777827382087708);
				glVertex3f( -0.420128881931305, 3.878221988677979, -1.011986732482910);
				glVertex3f( -0.423102647066116, 3.837967395782471, -1.014960289001465);

			glNormal3f( 0.996645927429199, -0.024026036262512, 0.078227981925011 );
				glVertex3f( -0.435716301202774, 3.900144100189209, -0.778499722480774);
				glVertex3f( -0.437714189291000, 3.832205295562744, -0.782070279121399);
				glVertex3f( -0.418505668640137, 3.837966918945312, -1.016864299774170);
				glVertex3f( -0.418505668640137, 3.878221988677979, -1.012659072875977);

			glNormal3f( -0.039778832346201, 0.994582653045654, -0.096037030220032 );
				glVertex3f( -0.437339514493942, 3.900144100189209, -0.777827382087708);
				glVertex3f( -0.435716301202774, 3.900144100189209, -0.778499722480774);
				glVertex3f( -0.418505668640137, 3.878221988677979, -1.012659072875977);
				glVertex3f( -0.420128881931305, 3.878221988677979, -1.011986732482910);

			glNormal3f( -0.008629463613033, -0.999743103981018, -0.020959880203009 );
				glVertex3f( -0.000000025145710, 3.809157848358154, -0.006501298863441);
				glVertex3f( -0.004597004037350, 3.809157848358154, -0.004597288090736);
				glVertex3f( -0.131148710846901, 3.814919471740723, -0.227323159575462);
				glVertex3f( -0.126551717519760, 3.814919948577881, -0.229227170348167);

			glNormal3f( -0.011784627102315, -0.999276995658875, -0.036148849874735 );
				glVertex3f( -0.126551717519760, 3.814919948577881, -0.229227170348167);
				glVertex3f( -0.131148710846901, 3.814919471740723, -0.227323159575462);
				glVertex3f( -0.323106199502945, 3.820681095123291, -0.324407666921616);
				glVertex3f( -0.316954374313354, 3.820681571960449, -0.326034158468246);

			glNormal3f( -0.010120239108801, -0.999647140502930, -0.024559160694480 );
				glVertex3f( -0.438689082860947, 3.823336124420166, -0.429733365774155);
				glVertex3f( -0.443286061286926, 3.823335647583008, -0.427829355001450);
				glVertex3f( -0.476639032363892, 3.826443195343018, -0.540564179420471);
				glVertex3f( -0.472042053937912, 3.826443195343018, -0.542468190193176);

			glNormal3f( -0.010586401447654, -0.999617338180542, -0.025556173175573 );
				glVertex3f( -0.472042053937912, 3.826443195343018, -0.542468190193176);
				glVertex3f( -0.476639032363892, 3.826443195343018, -0.540564179420471);
				glVertex3f( -0.442311108112335, 3.832205295562744, -0.780165791511536);
				glVertex3f( -0.437714189291000, 3.832205295562744, -0.782070279121399);

			glNormal3f( -0.871063828468323, 0.011560173705220, 0.491033822298050 );
				glVertex3f( -0.004597004037350, 3.809157848358154, -0.004597288090736);
				glVertex3f( -0.001623220741749, 3.987833023071289, -0.001623731572181);
				glVertex3f( -0.126665294170380, 3.965910911560059, -0.224829301238060);
				glVertex3f( -0.131148710846901, 3.814919471740723, -0.227323159575462);

			glNormal3f( -0.448182940483093, -0.006701819133013, 0.893916606903076 );
				glVertex3f( -0.131148710846901, 3.814919471740723, -0.227323159575462);
				glVertex3f( -0.126665294170380, 3.965910911560059, -0.224829301238060);
				glVertex3f( -0.321273297071457, 3.943988800048828, -0.321678251028061);
				glVertex3f( -0.323106199502945, 3.820681095123291, -0.324407666921616);

			glNormal3f( -0.959073960781097, 0.020093515515327, 0.282441765069962 );
				glVertex3f( -0.443286061286926, 3.823335647583008, -0.427829355001450);
				glVertex3f( -0.440312325954437, 3.933889865875244, -0.424855321645737);
				glVertex3f( -0.473552137613297, 3.922067165374756, -0.537626385688782);
				glVertex3f( -0.476639032363892, 3.826443195343018, -0.540564179420471);

			glNormal3f( -0.987692415714264, 0.053404312580824, -0.147009253501892 );
				glVertex3f( -0.476639032363892, 3.826443195343018, -0.540564179420471);
				glVertex3f( -0.473552137613297, 3.922067165374756, -0.537626385688782);
				glVertex3f( -0.437339514493942, 3.900144100189209, -0.777827382087708);
				glVertex3f( -0.442311108112335, 3.832205295562744, -0.780165791511536);

			glNormal3f( 0.870794475078583, 0.007838431745768, -0.491584807634354 );
				glVertex3f( -0.000000025145710, 3.987833023071289, -0.002296071965247);
				glVertex3f( -0.000000025145710, 3.809157848358154, -0.006501298863441);
				glVertex3f( -0.126551717519760, 3.814919948577881, -0.229227170348167);
				glVertex3f( -0.125042095780373, 3.965910911560059, -0.225501641631126);

			glNormal3f( 0.448695987462997, 0.025460483506322, -0.893321633338928 );
				glVertex3f( -0.125042095780373, 3.965910911560059, -0.225501641631126);
				glVertex3f( -0.126551717519760, 3.814919948577881, -0.229227170348167);
				glVertex3f( -0.316954374313354, 3.820681571960449, -0.326034158468246);
				glVertex3f( -0.319071561098099, 3.943988323211670, -0.322247117757797);

			glNormal3f( 0.958883523941040, 0.010993248783052, -0.283586889505386 );
				glVertex3f( -0.438689082860947, 3.933889865875244, -0.425527662038803);
				glVertex3f( -0.438689082860947, 3.823336124420166, -0.429733365774155);
				glVertex3f( -0.472042053937912, 3.826443195343018, -0.542468190193176);
				glVertex3f( -0.471928864717484, 3.922066211700439, -0.538298726081848);

			glNormal3f( 0.989077508449554, -0.019678531214595, 0.146076485514641 );
				glVertex3f( -0.471928864717484, 3.922066211700439, -0.538298726081848);
				glVertex3f( -0.472042053937912, 3.826443195343018, -0.542468190193176);
				glVertex3f( -0.437714189291000, 3.832205295562744, -0.782070279121399);
				glVertex3f( -0.435716301202774, 3.900144100189209, -0.778499722480774);

			glNormal3f( -0.032897166907787, 0.996298134326935, -0.079421892762184 );
				glVertex3f( -0.001623220741749, 3.987833023071289, -0.001623731572181);
				glVertex3f( -0.000000025145710, 3.987833023071289, -0.002296071965247);
				glVertex3f( -0.125042095780373, 3.965910911560059, -0.225501641631126);
				glVertex3f( -0.126665294170380, 3.965910911560059, -0.224829301238060);

			glNormal3f( -0.043971233069897, 0.989748418331146, -0.135883867740631 );
				glVertex3f( -0.126665294170380, 3.965910911560059, -0.224829301238060);
				glVertex3f( -0.125042095780373, 3.965910911560059, -0.225501641631126);
				glVertex3f( -0.319071561098099, 3.943988323211670, -0.322247117757797);
				glVertex3f( -0.321273297071457, 3.943988800048828, -0.321678251028061);

			glNormal3f( -0.038243532180786, 0.994927823543549, -0.093038074672222 );
				glVertex3f( -0.440312325954437, 3.933889865875244, -0.424855321645737);
				glVertex3f( -0.438689082860947, 3.933889865875244, -0.425527662038803);
				glVertex3f( -0.471928864717484, 3.922066211700439, -0.538298726081848);
				glVertex3f( -0.473552137613297, 3.922067165374756, -0.537626385688782);

			glNormal3f( -0.039787866175175, 0.994511723518372, -0.096765108406544 );
				glVertex3f( -0.473552137613297, 3.922067165374756, -0.537626385688782);
				glVertex3f( -0.471928864717484, 3.922066211700439, -0.538298726081848);
				glVertex3f( -0.435716301202774, 3.900144100189209, -0.778499722480774);
				glVertex3f( -0.437339514493942, 3.900144100189209, -0.777827382087708);

			glNormal3f( -0.006019630935043, -0.999808967113495, -0.018597144633532 );
				glVertex3f( -0.316954374313354, 3.820681571960449, -0.326034158468246);
				glVertex3f( -0.323106199502945, 3.820681095123291, -0.324407666921616);
				glVertex3f( -0.443286061286926, 3.823335647583008, -0.427829355001450);
				glVertex3f( -0.438689082860947, 3.823336124420166, -0.429733365774155);

			glNormal3f( -0.653523147106171, -0.005027045961469, 0.756889879703522 );
				glVertex3f( -0.323106199502945, 3.820681095123291, -0.324407666921616);
				glVertex3f( -0.321273297071457, 3.943988800048828, -0.321678251028061);
				glVertex3f( -0.440312325954437, 3.933889865875244, -0.424855321645737);
				glVertex3f( -0.443286061286926, 3.823335647583008, -0.427829355001450);

			glNormal3f( 0.650088489055634, 0.031832586973906, -0.759191453456879 );
				glVertex3f( -0.319071561098099, 3.943988323211670, -0.322247117757797);
				glVertex3f( -0.316954374313354, 3.820681571960449, -0.326034158468246);
				glVertex3f( -0.438689082860947, 3.823336124420166, -0.429733365774155);
				glVertex3f( -0.438689082860947, 3.933889865875244, -0.425527662038803);

			glNormal3f( -0.022930759936571, 0.997209250926971, -0.071047984063625 );
				glVertex3f( -0.321273297071457, 3.943988800048828, -0.321678251028061);
				glVertex3f( -0.319071561098099, 3.943988323211670, -0.322247117757797);
				glVertex3f( -0.438689082860947, 3.933889865875244, -0.425527662038803);
				glVertex3f( -0.440312325954437, 3.933889865875244, -0.424855321645737);

		glEnd( );

	glEndList( );

	BoxList = glGenLists( 1 );
	glNewList( BoxList, GL_COMPILE );

		glBegin( GL_QUADS );

			glColor3f( 0., 0., 1. );
			glNormal3f( 0., 0.,  1. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f( -dx,  dy,  dz );

			glNormal3f( 0., 0., -1. );
				glTexCoord2f( 0., 0. );
				glVertex3f( -dx, -dy, -dz );
				glTexCoord2f( 0., 1. );
				glVertex3f( -dx,  dy, -dz );
				glTexCoord2f( 1., 1. );
				glVertex3f(  dx,  dy, -dz );
				glTexCoord2f( 1., 0. );
				glVertex3f(  dx, -dy, -dz );

			glColor3f( 1., 0., 0. );
			glNormal3f(  1., 0., 0. );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f(  dx,  dy,  dz );

			glNormal3f( -1., 0., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f( -dx,  dy, -dz );
				glVertex3f( -dx, -dy, -dz );

			glColor3f( 0., 1., 0. );
			glNormal3f( 0.,  1., 0. );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f( -dx,  dy, -dz );

			glNormal3f( 0., -1., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx, -dy, -dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx, -dy,  dz );

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

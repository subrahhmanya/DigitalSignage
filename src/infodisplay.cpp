//-----------------------------------------------------------------------------
//	     Name: infodisplay.cpp
//	   Author: Paul Wollaston
//  Last Modified: 29/11/11
//    Description:  Base framework for Information Display
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <FTGL/ftgl.h>

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
int	g_window	= 0;
GLuint g_textureID[10];

FTFont *font;

struct Vertex
{
	float tu, tv;
	float x, y, z;
};

Vertex g_quadVertices[] =
{
	{ 0.0f,0.0f, -3.0f,-0.575f, 0.0f },
	{ 1.0f,0.0f,  3.0f,-0.575f, 0.0f },
	{ 1.0f,1.0f,  3.0f, 0.575f, 0.0f },
	{ 0.0f,1.0f, -3.0f, 0.575f, 0.0f }
};

struct BMPImage 
{
	int   width;
	int   height;
	char *data;
};

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
int main(int argc, char **argv);
void init(void);
void getBitmapImageData(char *pFileName, BMPImage *pImage);
void loadTexture(int texid, char *fName);
void keyboardFunc(unsigned char key, int x, int y);
void idleFunc(void);
void reshapeFunc(int w, int h);
void drawtextFunc(int x, int y, char *fName, int fsize, char *fTxt);
void displayFunc(void);

//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main( int argc, char **argv )
{
	glutInit( &argc, argv );

	init();

	glutSwapBuffers();
	glutMainLoop();

	return 0;
}

//-----------------------------------------------------------------------------
// Name: init()
// Desc: 
//-----------------------------------------------------------------------------
void init( void )
{
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize( 1280, 720 );
	g_window = glutCreateWindow( "Information Display" );

	// 640x480, 16bit pixel depth, 60Hz refresh rate
//	glutGameModeString( "1280x720:16@60" );

	// start fullscreen game mode
//	glutEnterGameMode();

	glutDisplayFunc( displayFunc );
	glutKeyboardFunc( keyboardFunc );
	glutReshapeFunc( reshapeFunc );
	glutIdleFunc( idleFunc );

	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	loadTexture(0, "textures/orblogo.bmp");

	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

	glutSetCursor(GLUT_CURSOR_NONE); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
}

//-----------------------------------------------------------------------------
// Name: getBitmapImageData()
// Desc: Simply image loader for 24 bit BMP files.
//-----------------------------------------------------------------------------
void getBitmapImageData( char *pFileName, BMPImage *pImage )
{
	FILE *pFile = NULL;
	unsigned short nNumPlanes;
	unsigned short nNumBPP;
	int i;

	if( (pFile = fopen(pFileName, "rb") ) == NULL )
		printf("ERROR: getBitmapImageData - %s not found\n",pFileName);

	// Seek forward to width and height info
	fseek( pFile, 18, SEEK_CUR );

	if( (i = fread(&pImage->width, 4, 1, pFile) ) != 1 )
		printf("ERROR: getBitmapImageData - Couldn't read width from %s.\n", pFileName);

	if( (i = fread(&pImage->height, 4, 1, pFile) ) != 1 )
		printf("ERROR: getBitmapImageData - Couldn't read height from %s.\n", pFileName);

	if( (fread(&nNumPlanes, 2, 1, pFile) ) != 1 )
		printf("ERROR: getBitmapImageData - Couldn't read plane count from %s.\n", pFileName);
	
	if( nNumPlanes != 1 )
		printf( "ERROR: getBitmapImageData - Plane count from %s is not 1: %u\n", pFileName, nNumPlanes );

	if( (i = fread(&nNumBPP, 2, 1, pFile)) != 1 )
		printf( "ERROR: getBitmapImageData - Couldn't read BPP from %s.\n", pFileName );
	
	if( nNumBPP != 24 )
		printf( "ERROR: getBitmapImageData - BPP from %s is not 24: %u\n", pFileName, nNumBPP );

	// Seek forward to image data
	fseek( pFile, 24, SEEK_CUR );

	// Calculate the image's total size in bytes. Note how we multiply the 
	// result of (width * height) by 3. This is becuase a 24 bit color BMP 
	// file will give you 3 bytes per pixel.
	int nTotalImagesize = (pImage->width * pImage->height) * 3;

	pImage->data = (char*) malloc( nTotalImagesize );
	
	if( (i = fread(pImage->data, nTotalImagesize, 1, pFile) ) != 1 )
		printf("ERROR: getBitmapImageData - Couldn't read image data from %s.\n", pFileName);

	//
	// Finally, rearrange BGR to RGB
	//
	
	char charTemp;
	for( i = 0; i < nTotalImagesize; i += 3 )
	{ 
		charTemp = pImage->data[i];
		pImage->data[i] = pImage->data[i+2];
		pImage->data[i+2] = charTemp;
	}
}

//-----------------------------------------------------------------------------
// Name: loadTexture()
// Desc: 
//-----------------------------------------------------------------------------
void loadTexture( int texid, char *fName )	
{
	BMPImage textureImage;
	
		getBitmapImageData( fName, &textureImage );

	glGenTextures( 1, &g_textureID[texid] );
	glBindTexture( GL_TEXTURE_2D, g_textureID[texid] );

	// select modulate to mix texture with color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glTexImage2D( GL_TEXTURE_2D, 0, 3, textureImage.width, textureImage.height, 
				   0, GL_RGB, GL_UNSIGNED_BYTE, textureImage.data );
}

//-----------------------------------------------------------------------------
// Name: keyboardFunc()
// Desc: Called when a keyboard key is pressed
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y ) 
{
	switch( key )
	{
		case 27:
			glutDestroyWindow( g_window );
			exit(0);
			break;
	}
}

//-----------------------------------------------------------------------------
// Name: idleFunc()
// Desc: Called during idle time
//-----------------------------------------------------------------------------
void idleFunc( void )
{
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------
// Name: reshapeFunc()
// Desc: Called when the window size has been changed by the user
//-----------------------------------------------------------------------------
void reshapeFunc( int w, int h )
{
	glViewport( 0, 0, w, h );
}

//-----------------------------------------------------------------------------
// Name: drawtextFunc()
// Desc: Called when we want to display informative text at a specific location
//-----------------------------------------------------------------------------
void drawtextFunc( int x, int y, char *fName, int fsize, char *fTxt )
{
	// We need to add in creation and drawing to current buffer.
	// This will be easy enough, as we will process and draw prior to
	// glSwapBuffers() call from parent function.
	// Create a pixmap font from a TrueType file.
	font = new FTPixmapFont(fName);

	// Set the font size and render a small text.
	font->FaceSize(fsize);
	glEnable(GL_TEXTURE_2D);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef( 0.0f, 2.0f, -1.0f );
	font->Render(fTxt);
	glDisable(GL_TEXTURE_2D);
}

//-----------------------------------------------------------------------------
// Name: displayFunc()
// Desc: Called when GLUT is ready to render
//-----------------------------------------------------------------------------
void displayFunc( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glPushMatrix();
	glTranslatef( -8.0f, 5.5f, -15.0f );
	glEnable( GL_TEXTURE_2D );
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture( GL_TEXTURE_2D, g_textureID[0] );
	glInterleavedArrays( GL_T2F_V3F, 0, g_quadVertices );
	glDrawArrays( GL_QUADS, 0, 4 );
	glDisable( GL_TEXTURE_2D );
	glPopMatrix();

	glPushMatrix();
	glPixelTransferf(GL_RED_BIAS, -1.0f);
	glPixelTransferf(GL_GREEN_BIAS, -1.0f);
	glPixelTransferf(GL_BLUE_BIAS, -1.0f);
	drawtextFunc( 0, 0, "/screen/src/infodisplay/src/fonts/cgothic.ttf", 48, "Welcome to Orbital");
	glPopMatrix();

	glutSwapBuffers();
}

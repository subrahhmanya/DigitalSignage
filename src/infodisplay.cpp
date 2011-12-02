//-----------------------------------------------------------------------------
//		 Name: infodisplay.cpp
//	   Author: Paul Wollaston
//  Last Modified: 01/12/11
//	Description:  Base framework for Information Display
//-----------------------------------------------------------------------------

#include <math.h>

// include SDL libraries
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define IS_RUNNING	true

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int SCREEN_BPP = 24;

SDL_Surface* screen = NULL;

bool drawText(const char *text,
			const char *fname,
			int fsize,
			SDL_Color color,
			SDL_Rect *location,
			int alignment);
bool FileExists( const char* FileName );
void doDisplay();
bool init();

SDL_Surface* setColorKeyOrg(SDL_Surface* s, Uint32 maskColor)
{
    maskColor |= 0xff000000u;
    Uint32* surfacepixels = (Uint32*) s->pixels;
    SDL_LockSurface(s);
    for(int y = 0; y < s->h; y++)
	{
	    for(int x = 0; x < s->w; x++)
		{
		    Uint32* p = &(surfacepixels[y * s->pitch / 4  + x]);
		    if(*p == maskColor) *p = 0x00000000u;
		}
	}
    SDL_UnlockSurface(s);
    return s;
}

bool FileExists( const char* FileName )
{
    FILE* fp = NULL;

    //will not work if you do not have read permissions

    //to the file, but if you don''t have read, it

    //may as well not exist to begin with.

    fp = fopen( FileName, "rb" );
    if( fp != NULL )
    {
        fclose( fp );
        return true;
    }

    return false;
}

bool drawText(const char *text,
			const char *fname,
			int fsize,
			SDL_Color color,
			SDL_Rect *location,
			int alignment)
{
	SDL_Surface *initial;
	SDL_Surface *intermediary;
	SDL_Rect rect;
	int w,h,ax,ay;
	GLuint texture;

	TTF_Font *fntChosen;

	fntChosen = TTF_OpenFont(fname, fsize);
	if(fntChosen == NULL) {
		fprintf( stderr, "Failed when loading Font: %s\n",
		SDL_GetError( ) );
		return false;
	}

	/* Use SDL_TTF to render our text */
	initial = TTF_RenderUTF8_Blended(fntChosen, text, color);

	/* Convert the rendered text to a known format */
	w = initial->w;
	h = initial->h;

	/* Do the Alignment - default is left.
	   1 = Left Align
	   2 = Right Align */

	ax=location->x;
	ay=location->y;

	if (alignment==2) {
		ax=(location->x - w) ;
		ay=location->y;
	}

	/* We need to allocate some memory now for out buffer */
	unsigned char *pixels;
	pixels = (unsigned char*) malloc(4 * w * h);

	/* Let's grab a section of the backbuffer into memory */
	glPixelStorei( GL_PACK_ROW_LENGTH, 0 ) ;
	glPixelStorei( GL_PACK_ALIGNMENT, 1 ) ;
	glReadBuffer( GL_BACK );
	glReadPixels( ax, ay, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

	/* Create new SDL Surface where text is layed on top of the backbuffer we just grabbed */
	intermediary = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
	#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		0x000000FF, 0x0000FF00, 0x00FF0000, 0
	#else
		0x00FF0000, 0x0000FF00, 0x000000FF, 0
	#endif
	);

	for (int i = 0 ; i < h ; i++)
		memcpy(((char *) intermediary->pixels) + intermediary->pitch * i, pixels + 4 * w * (h-i - 1), w*4);
	free(pixels);

	/* Blit text on top of new texture */
	SDL_BlitSurface(initial, 0, intermediary, 0);

	/* Tell GL about our new texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, intermediary->pixels );

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* prepare to render our texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);

	/* Draw a quad at location */
	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0f, 1.0f);
			glVertex2f(ax    , ay);
		glTexCoord2f(1.0f, 1.0f);
			glVertex2f(ax + w, ay);
		glTexCoord2f(1.0f, 0.0f);
			glVertex2f(ax + w, ay + h);
		glTexCoord2f(0.0f, 0.0f);
			glVertex2f(ax    , ay + h);
	glEnd();
	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	/* return the deltas in the unused w,h part of the rect */
	location->w = initial->w;
	location->h = initial->h;

	/* Clean up */
	TTF_CloseFont(fntChosen);
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(2, &texture);
}

bool init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,   8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,  8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );

	if (FileExists("/screen/full")) {
		screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_OPENGL);
	} else {
		screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_OPENGL);
	}
	if(screen == NULL) {
		fprintf( stderr, "Video mode set failed: %s\n",
		SDL_GetError( ) );
		return false;
	}

	SDL_ShowCursor(SDL_DISABLE); 

	glClearColor(1, 1, 1, 1);
	glClearDepth(1.0f);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	glLoadIdentity();

	SDL_WM_SetCaption("Info Display", NULL);

	return true;
}

void doDisplay() {
	// This is the main processing and rendering function.
	// In here, we call all required information and pull what we need.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	char *date;
	
	time_t timer;

	timer=time(NULL);
	
	date = asctime(localtime(&timer));

	SDL_Color iro;
	SDL_Rect position;

	iro.r = 0;
	iro.g = 0;
	iro.b = 0;
	position.x = 400;
	position.y = 664;
	drawText("Notification Center", "/screen/fonts/cgothic.ttf", 48, iro, &position, 1);

	iro.r = 0;
	iro.g = 0;
	iro.b = 0;
	position.x = 1316;
	position.y = 0;
	drawText(date, "/screen/fonts/cgothic.ttf", 48, iro, &position, 2);

	SDL_GL_SwapBuffers();
}

int main( int argc, char* argv[] ) {
	if(init() == false) {
		return 1;
	}

	while ( IS_RUNNING ) {
		doDisplay();
		SDL_Delay(1000);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	SDL_FreeSurface(screen);
	TTF_Quit();
	SDL_Quit();
	return 0;
}

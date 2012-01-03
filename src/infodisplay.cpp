//-------------------------------------------------------------------------
//          Name: infodisplay.cpp
//        Author: Paul Wollaston
// Project Start: 25/11/11
// Last Modified: 03/12/12
//   Description: Display agregated data in an easy-to-read format with
//                ability to be flexible with what is shown.
//-------------------------------------------------------------------------

/* Include standard C libraries */
#include <math.h>
#include <stdio.h>
#include <string>
#include <time.h>

/* Include libxml */
#include <libxml/parser.h>
#include <libxml/tree.h>

/* include SDL libraries */
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

/* Display Properties */
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int SCREEN_BPP = 24;
int SCREEN_TARGET_FPS = 5;

/* Define Main Screen Surface */
SDL_Surface* screen = NULL;

/* Define Fonts */
TTF_Font *fntCGothic16;
TTF_Font *fntCGothic22;
TTF_Font *fntCGothic34;
TTF_Font *fntCGothic44;
TTF_Font *fntCGothic48;

/* Variables for application (global) */
bool IS_RUNNING = true;
int wLastCheckH = 99;
int wLastCheckM = 99;
int wUpdateTimer[5] = {0, 0, 0, 0, 0};
int wFadeV[5] = {255, 255, 255, 255, 255};
int wFadeA[5] = {0, 0, 0, 0, 0};
int bScroll[3] = {0, 0, 0};
int bVisible[3] = {0, 0, 0};
int bTimer[3] = {0, 0, 0};
int bDuration[3] = {0, 0, 0};
int bNo[3] = {0, 0, 0};
int bCNo[3] = {0, 0, 0};
int bTimeStamp[3] = {0, 0, 0};
int bCondition[3] = {0, 0, 0};
int bNCondition[3] = {0, 0, 0};
int bBAlpha[3] = {0, 0, 0};
int bCAlpha[3] = {0, 0, 0};
int bCTimer[3] = {15, 15, 0};
int dAnim[5] = {0, 0, 0, 0, 0};
int wCurDisp=0;
int dFPS = 0;
char sFPS[32];
int tC1 = 0;
bool bV1 = false;
bool wOK = false;
char wTemp[4];
int wFarenheight=0;
char wCondition[32];
char wHumidity[32];
char wIcon[64];
char wWind[32];
int wIWind=0;
int pTWidth=0;
float wCelcius=0.0;
int tFarenheight, tCondition, tHumidity, tIcon, tWind;
int logoisWhite=0;
char tFName[128];

/* Define Global Textures */
SDL_Surface *orb_logo;
SDL_Surface *orb_bl;
SDL_Surface *orb_bt;
SDL_Surface *orb_bcrnr;
SDL_Surface *orb_wl;
SDL_Surface *orb_wt;
SDL_Surface *orb_wcrnr;
SDL_Surface *orb_tl;
SDL_Surface *orb_tt;
SDL_Surface *orb_tcrnr;
SDL_Surface *orb_boxb;
SDL_Surface *orb_boxw;
SDL_Surface *wTex_chance_of_storm;
SDL_Surface *wTex_mostly_sunny;
SDL_Surface *wTex_dust;
SDL_Surface *wTex_mostly_cloudy;
SDL_Surface *wTex_cloudy;
SDL_Surface *wTex_chance_of_tstorm;
SDL_Surface *wTex_partly_cloudy;
SDL_Surface *wTex_storm;
SDL_Surface *wTex_sunny;
SDL_Surface *wTex_cold;
SDL_Surface *wTex_windy;
SDL_Surface *wTex_flurries;
SDL_Surface *wTex_chance_of_snow;
SDL_Surface *wTex_chance_of_rain;
SDL_Surface *wTex_fog;
SDL_Surface *wTex_icy;
SDL_Surface *wTex_sleet;
SDL_Surface *wTex_rain;
SDL_Surface *wTex_mist;
SDL_Surface *wTex_haze;
SDL_Surface *wTex_smoke;
SDL_Surface *wTex_snow;
SDL_Surface *wTex_hot;
SDL_Surface *wTex_thunderstorm;
SDL_Surface *boardTex_A;
SDL_Surface *boardTex_B;
SDL_Surface *boardTex_C;

/* Function Declerations */
int calcDay_Dec31(int yyyy);
int dayInYear(int dd, int mm);
void dayInStr (char daysInWord[], int days);
void monthInStr (char monthsInWord[], int days);
void nthInStr (char dowInWord[], int monthday);
bool drawText(const char *text,
			TTF_Font *font,
			SDL_Color color,
			SDL_Rect *location,
			int alignment,
			int cr,
			int cg,
			int cb,
			int alpha,
			int px,
			int py);

bool drawTexture(SDL_Surface *tpoint,
			int px,
			int py,
			int alpha,
			int scale);

bool drawInfoBox(SDL_Surface *tpoint,
			int bcol,
			int px,
			int py,
			int minx,
			int miny,
			int scrollv,
			float br,
			float bg,
			float bb,
			int scale,
			int balpha,
			int calpha);

bool FileExists( const char* FileName );
void doDisplay();
bool init();

static void parseWeather(xmlNode * a_node)
{
	xmlNode *cur_node = NULL;
	char tWord[16];
	for (cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "condition" ))
			{
				if (tCondition == 0)
				{
					sprintf(wCondition, "%s", cur_node->properties->children->content);
					tCondition=1;
					printf("*UPDATE* Item: %s \tData: %s\n", cur_node->name, wCondition);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "temp_f" ))
			{
				sprintf(tWord, "%s", cur_node->properties->children->content);
				if (tFarenheight == 0)
				{
					wFarenheight = strtol(tWord,NULL,0);
					tFarenheight = wFarenheight;
					printf("*UPDATE* Item: %s \t\tData: %i\n",
						cur_node->name, wFarenheight);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "humidity" ))
			{
				if (tHumidity == 0)
				{
					tHumidity=1;
					sprintf(wHumidity, "%s", cur_node->properties->children->content);
					printf("*UPDATE* Item: %s \tData: %s\n", cur_node->name, wHumidity);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "icon" ))
			{
				if (tIcon == 0)
				{
					tIcon=1;
					sprintf(wIcon, "%s", cur_node->properties->children->content);
					printf("*UPDATE* Item: %s \t\tData: %s\n", cur_node->name, wIcon);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "wind_condition" ))
			{
				if (tWind == 0)
				{
					tWind=1;
					sprintf(wWind, "%s", cur_node->properties->children->content);
					/* Get speed of wind and convert to int */
					for(int i = 0; i < strlen(wWind); ++i)
					{
						if (isdigit(wWind[i]))
							if (wIWind == 0)
								wIWind = atoi(&wWind[i]);
					}
					printf("*UPDATE* Item: %s \tData: %s\n",
						cur_node->name, wWind);
				}
				/* Return wOK if all variables have been set. We do this here. as all data is parsed in sequence. 
				   This way, even if only part of the data was missed, we still declare as Dirty */
				if ((tWind != 0) && (tIcon != 0) && (tHumidity != 0) && (tFarenheight != 0) && (tCondition != 0))
					wOK = true;
				else
					wOK = false;
			}
		}
		parseWeather(cur_node->children);
	}
}

int calcDay_Dec31(int yyyy)
{
	int dayCode = 0;
	dayCode = ((yyyy-1)*365 + (yyyy-1)/4 - (yyyy-1)/100 + (yyyy-1)/400) % 7;
	return dayCode;
}

int dayInYear(int dd, int mm)
{
	switch(mm)
	{
	case 11:dd += 30;
	case 10:dd += 31;
	case 9:dd += 30;
	case 8:dd += 31;
	case 7:dd += 31;
	case 6:dd += 30;
	case 5:dd += 31;
	case 4:dd += 30;
	case 3:dd += 31;
	case 2:dd += 28;
	case 1:dd += 31;
	}
	return dd;
}

void dayInStr(char daysInWord[], int days)
{
	switch(days)
	{
	case 0:strcpy(daysInWord, "Sun");break;
	case 1:strcpy(daysInWord, "Mon");break;
	case 2:strcpy(daysInWord, "Tue");break;
	case 3:strcpy(daysInWord, "Wed");break;
	case 4:strcpy(daysInWord, "Thu");break;
	case 5:strcpy(daysInWord, "Fri");break;
	case 6:strcpy(daysInWord, "Sat");break;
	}
}

void monthInStr (char monthsInWord[], int month)
{
	switch(month)
	{
	case 0:strcpy(monthsInWord, "Jan");break;
	case 1:strcpy(monthsInWord, "Feb");break;
	case 2:strcpy(monthsInWord, "Mar");break;
	case 3:strcpy(monthsInWord, "Apr");break;
	case 4:strcpy(monthsInWord, "May");break;
	case 5:strcpy(monthsInWord, "Jun");break;
	case 6:strcpy(monthsInWord, "Jul");break;
	case 7:strcpy(monthsInWord, "Aug");break;
	case 8:strcpy(monthsInWord, "Sep");break;
	case 9:strcpy(monthsInWord, "Oct");break;
	case 10:strcpy(monthsInWord, "Nov");break;
	case 11:strcpy(monthsInWord, "Dec");break;
	}
}

void nthInStr (char dowInWord[], int monthday)
{
	switch(monthday)
	{
	case 1:strcpy(dowInWord, "st");break;
	case 2:strcpy(dowInWord, "nd");break;
	case 3:strcpy(dowInWord, "rd");break;
	case 4:strcpy(dowInWord, "th");break;
	case 5:strcpy(dowInWord, "th");break;
	case 6:strcpy(dowInWord, "th");break;
	case 7:strcpy(dowInWord, "th");break;
	case 8:strcpy(dowInWord, "th");break;
	case 9:strcpy(dowInWord, "th");break;
	case 10:strcpy(dowInWord, "th");break;
	case 11:strcpy(dowInWord, "th");break;
	case 12:strcpy(dowInWord, "th");break;
	case 13:strcpy(dowInWord, "th");break;
	case 14:strcpy(dowInWord, "th");break;
	case 15:strcpy(dowInWord, "th");break;
	case 16:strcpy(dowInWord, "th");break;
	case 17:strcpy(dowInWord, "th");break;
	case 18:strcpy(dowInWord, "th");break;
	case 19:strcpy(dowInWord, "th");break;
	case 20:strcpy(dowInWord, "th");break;
	case 21:strcpy(dowInWord, "st");break;
	case 22:strcpy(dowInWord, "nd");break;
	case 23:strcpy(dowInWord, "rd");break;
	case 24:strcpy(dowInWord, "th");break;
	case 25:strcpy(dowInWord, "th");break;
	case 26:strcpy(dowInWord, "th");break;
	case 27:strcpy(dowInWord, "th");break;
	case 28:strcpy(dowInWord, "th");break;
	case 29:strcpy(dowInWord, "th");break;
	case 30:strcpy(dowInWord, "th");break;
	case 31:strcpy(dowInWord, "st");break;
	}
}

bool FileExists( const char* FileName )
{
	FILE* fp = NULL;
	fp = fopen( FileName, "rb" );
	if( fp != NULL )
	{
		fclose( fp );
		return true;
	}
	return false;
}

bool drawText(const char *text,
			TTF_Font *fntChosen,
			int alignment,
			int cr,
			int cg,
			int cb,
			int alpha,
			int px,
			int py)
{
	SDL_Surface *initial;
	SDL_Color color;
	SDL_Rect location;
	int w,h,ax,ay;
	GLuint texture;

	color.b = cr;
	color.g = cg;
	color.r = cb;
	location.x = px;
	location.y = py;

	/* Use SDL_TTF to render our text */
	initial = TTF_RenderUTF8_Blended(fntChosen, text, color);

	/* Convert the rendered text to a known format */
	w = initial->w;
	h = initial->h;

	/* Do the Alignment - default is left.
	   1 = Left Align
	   2 = Right Align */

	ax=location.x;
	ay=location.y;

	if (alignment==2) {
		ax=(location.x - w) ;
		ay=location.y;
	}

	/* Tell GL about our new texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, initial->pixels );

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* prepare to render our texture */
//	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor4f(1.0f, 1.0f, 1.0f, (float)alpha/255.0);

	/* Draw a quad at location */
	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0f, 1.0f);
			glVertex2f(ax, ay);
		glTexCoord2f(1.0f, 1.0f);
			glVertex2f(ax + w, ay);
		glTexCoord2f(1.0f, 0.0f);
			glVertex2f(ax + w, ay + h);
		glTexCoord2f(0.0f, 0.0f);
			glVertex2f(ax, ay + h);
	glEnd();
	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	/* return the deltas in the unused w,h part of the rect */
	location.w = initial->w;
	location.h = initial->h;
	pTWidth = initial->w;

	/* Clean up */
	SDL_FreeSurface(initial);
	glDeleteTextures(2, &texture);
	return true;
}

bool drawTexture(SDL_Surface *tpoint,
			int px,
			int py,
			int alpha,
			int scale)
{

	SDL_Rect location;
	GLuint TextureID = 0;

	location.x = px;
	location.y = py;

	int w,h,ax,ay;

	w = tpoint->w;
	h = tpoint->h;

	/* Convert the rendered text to a known format */
	w = tpoint->w;
	h = tpoint->h;

	/* Do the Alignment - default is left.
	   1 = Left Align
	   2 = Right Align */

	ax=location.x;
	ay=location.y;

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
 
	int Mode = GL_RGB;
 
	if(tpoint->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}
 
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, w, h, 0, Mode, GL_UNSIGNED_BYTE, tpoint->pixels);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* prepare to render our texture */
//	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glColor4f(1.0f, 1.0f, 1.0f, (float)alpha/255.0);

	/* Draw a quad at location */
	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0f, 1.0f);
			glVertex2f(ax, ay);
		glTexCoord2f(1.0f, 1.0f);
			glVertex2f(ax + (w/scale), ay);
		glTexCoord2f(1.0f, 0.0f);
			glVertex2f(ax + (w/scale), ay + (h/scale));
		glTexCoord2f(0.0f, 0.0f);
			glVertex2f(ax, ay + (h/scale));
	glEnd();
	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	/* return the deltas in the unused w,h part of the rect */
	location.w = tpoint->w;
	location.h = tpoint->h;
	pTWidth = tpoint->w/scale;

	/* Clean up */
	glDeleteTextures(1, &TextureID);
	return true;
}

bool drawInfoBox(SDL_Surface *tpoint,
			int bcol,
			int px,
			int py,
			int minx,
			int miny,
			int scrollv,
			float br,
			float bg,
			float bb,
			int scale,
			int balpha,
			int calpha)
{
	/* Box Types (bcol)
		1 = Black BG
		2 = White BG
		3 = Transparent BG */

	GLuint TextureID = 0;
	glGenTextures(1, &TextureID);
	SDL_Surface *brdr_top;
	SDL_Surface *brdr_left;
	SDL_Surface *brdr_crnr;

	/* We don't want the border to have less opacity than the contents, so match contents to border if required */
	if (balpha < calpha)
		calpha = balpha;

	switch(bcol)
	{
		case 1:
			brdr_top = orb_bt;
			brdr_left = orb_bl;
			brdr_crnr = orb_bcrnr;
			break;
		case 2:
			brdr_top = orb_wt;
			brdr_left = orb_wl;
			brdr_crnr = orb_wcrnr;
			break;
		case 3:
			brdr_top = orb_tt;
			brdr_left = orb_tl;
			brdr_crnr = orb_tcrnr;
			break;
	}

	int w = (minx / 255.0) * scale;
	int h = (miny / 255.0) * scale;

	glBindTexture(GL_TEXTURE_2D, TextureID);
	int tw = brdr_left->w;
	int th = brdr_left->h;
	int Mode = GL_RGB;
	if(brdr_left->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, tw, th, 0, Mode, GL_UNSIGNED_BYTE, brdr_left->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* prepare to render our texture */
	glBindTexture(GL_TEXTURE_2D, TextureID);
	/* Testing of Box Drawing on screen */
	glColor4f(br, bg, bb, (float)balpha/255.0);

	/* Draw Left Border */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px-12, py+4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px+4, py+4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px+4, py+h-4);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px-12, py+h-4);
	glEnd();
	/* Draw Right Border */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px+w+12, py+4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px+w-4, py+4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px+w-4, py+h-4);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px+w+12, py+h-4);
	glEnd();
	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	tw = brdr_top->w;
	th = brdr_top->h;
	Mode = GL_RGB;
	if(brdr_top->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, tw, th, 0, Mode, GL_UNSIGNED_BYTE, brdr_top->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* prepare to render our texture */
	glBindTexture(GL_TEXTURE_2D, TextureID);

	/* Draw Top Border */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px+4, py+h-4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px+w-4, py+h-4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px+w-4, py+h+12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px+4, py+h+12);
	glEnd();
	/* Draw Bottom Border */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px+4, py+4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px+w-4, py+4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px+w-4, py-12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px+4, py-12);
	glEnd();
	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	tw = brdr_crnr->w;
	th = brdr_crnr->h;
	Mode = GL_RGB;
	if(brdr_crnr->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, tw, th, 0, Mode, GL_UNSIGNED_BYTE, brdr_crnr->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* prepare to render our texture */
	glBindTexture(GL_TEXTURE_2D, TextureID);

	/* Draw TL */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px-12, py+h-4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px+4, py+h-4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px+4, py + h + 12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px-12, py + h + 12);
	glEnd();
	/* Draw BL */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px-12, py+4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px+4, py+4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px+4, py-12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px-12, py-12);
	glEnd();
	/* Draw TR */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px+w+12, py+h-4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px+w-4, py+h-4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px+w-4, py+h+12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px+w+12, py+h+12);
	glEnd();
	/* Draw BR */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px+w+12, py+4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px+w-4, py+4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px+w-4, py-12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px+w+12, py-12);
	glEnd();

	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	/* Draw Box + Contents */
	if (bcol != 3)
	{
		/* Draw a quad at location */
		if (bcol == 1)
		{
			tw = orb_boxb->w;
			th = orb_boxb->h;

			Mode = GL_RGB;
			if(orb_boxb->format->BytesPerPixel == 4) {
				Mode = GL_RGBA;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, Mode, tw, th, 0, Mode, GL_UNSIGNED_BYTE, orb_boxb->pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			/* prepare to render our texture */
			glBindTexture(GL_TEXTURE_2D, TextureID);
		} else {
			tw = orb_boxw->w;
			th = orb_boxw->h;

			Mode = GL_RGB;
			if(orb_boxb->format->BytesPerPixel == 4) {
				Mode = GL_RGBA;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, Mode, tw, th, 0, Mode, GL_UNSIGNED_BYTE, orb_boxw->pixels);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			/* prepare to render our texture */
			glBindTexture(GL_TEXTURE_2D, TextureID);
		}

		glColor4f(br, bg, bb, (float)balpha/255.0);

		glBegin(GL_QUADS);
			/* Recall that the origin is in the lower-left corner
			   That is why the TexCoords specify different corners
			   than the Vertex coors seem to. */
			glTexCoord2f(0.0f, 1.0f);
				glVertex2f(px, py);
			glTexCoord2f(1.0f, 1.0f);
				glVertex2f(px + w, py);
			glTexCoord2f(1.0f, 0.0f);
				glVertex2f(px + w, py + h);
			glTexCoord2f(0.0f, 0.0f);
				glVertex2f(px, py + h);
		glEnd();
		/* Bad things happen if we delete the texture before it finishes */
		glFinish();
	}

	/* Now draw Texture */
	tw = tpoint->w;
	th = tpoint->h;

	w = (tpoint->w / 255.0) * scale;
	h = (tpoint->h / 255.0) * scale;
	int hs = (tpoint->h / 255.0) * scale;

	Mode = GL_RGB;
	if(tpoint->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, Mode, tw, th, 0, Mode, GL_UNSIGNED_BYTE, tpoint->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* prepare to render our texture */
	glBindTexture(GL_TEXTURE_2D, TextureID);

	glColor4f(1.0f, 1.0f, 1.0f, (float)calpha/255.0);

	if (h > miny)
		h = miny;

	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0f, (float)((float)((float)scrollv+h)/hs));
			glVertex2f(px, py);
		glTexCoord2f(1.0f, (float)((float)((float)scrollv+h)/hs));
			glVertex2f(px + w, py);
		glTexCoord2f(1.0f, (float)((float)scrollv/hs));
			glVertex2f(px + w, py + h);
		glTexCoord2f(0.0f, (float)((float)scrollv/hs));
			glVertex2f(px, py + h);
	glEnd();

	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	/* If we are scrolling, let's add shaded top/bottom bit (only 8px needed) */

	/* Clean up */
	glDeleteTextures(1, &TextureID);
	return true;
}

bool init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );

	if (FileExists("/screen/full")) {
		/* We're running full screen on the target, so use full screen */
		screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_OPENGL);
	} else {
		/* We're running in a window (no fullscreen flag set) so don't pass SDL_FULLSCREEN */
		screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_OPENGL);
	}
	if(screen == NULL) {
		fprintf( stderr, "Video mode set failed: %s\n",
		SDL_GetError( ) );
		IS_RUNNING=false;
		return false;
	}

	/* Load Fonts */
	fntCGothic16 = TTF_OpenFont("/screen/fonts/cgothic.ttf", 16);
	if(fntCGothic16 == NULL) {
		fprintf( stderr, "Failed when loading Font: %s\n",
		SDL_GetError( ) );
		IS_RUNNING=false;
		return false;
	}

	fntCGothic22 = TTF_OpenFont("/screen/fonts/cgothic.ttf", 22);
	if(fntCGothic22 == NULL) {
		fprintf( stderr, "Failed when loading Font: %s\n",
		SDL_GetError( ) );
		IS_RUNNING=false;
		return false;
	}

	fntCGothic34 = TTF_OpenFont("/screen/fonts/cgothic.ttf", 34);
	if(fntCGothic34 == NULL) {
		fprintf( stderr, "Failed when loading Font: %s\n",
		SDL_GetError( ) );
		IS_RUNNING=false;
		return false;
	}

	fntCGothic44 = TTF_OpenFont("/screen/fonts/cgothic.ttf", 44);
	if(fntCGothic44 == NULL) {
		fprintf( stderr, "Failed when loading Font: %s\n",
		SDL_GetError( ) );
		IS_RUNNING=false;
		return false;
	}

	fntCGothic48 = TTF_OpenFont("/screen/fonts/cgothic.ttf", 48);
	if(fntCGothic48 == NULL) {
		fprintf( stderr, "Failed when loading Font: %s\n",
		SDL_GetError( ) );
		IS_RUNNING=false;
		return false;
	}

	/* Load Textures */
	orb_logo = IMG_Load("/screen/textures/orblogo.png");
	orb_bl = IMG_Load("/screen/textures/orb_bl.png");
	orb_bt = IMG_Load("/screen/textures/orb_bt.png");
	orb_bcrnr = IMG_Load("/screen/textures/orb_bcrnr.png");
	orb_wl = IMG_Load("/screen/textures/orb_wl.png");
	orb_wt = IMG_Load("/screen/textures/orb_wt.png");
	orb_wcrnr = IMG_Load("/screen/textures/orb_wcrnr.png");
	orb_tl = IMG_Load("/screen/textures/orb_tl.png");
	orb_tt = IMG_Load("/screen/textures/orb_tt.png");
	orb_tcrnr = IMG_Load("/screen/textures/orb_tcrnr.png");
	orb_boxb = IMG_Load("/screen/textures/orb_boxb.png");
	orb_boxw = IMG_Load("/screen/textures/orb_boxw.png");
	wTex_chance_of_storm = IMG_Load("/screen/textures/weather/chance_of_storm.png");
	wTex_mostly_sunny = IMG_Load("/screen/textures/weather/mostly_sunny.png");
	wTex_dust = IMG_Load("/screen/textures/weather/dust.png");
	wTex_mostly_cloudy = IMG_Load("/screen/textures/weather/mostly_cloudy.png");
	wTex_cloudy = IMG_Load("/screen/textures/weather/cloudy.png");
	wTex_chance_of_tstorm = IMG_Load("/screen/textures/weather/chance_of_tstorm.png");
	wTex_partly_cloudy = IMG_Load("/screen/textures/weather/partly_cloudy.png");
	wTex_storm = IMG_Load("/screen/textures/weather/storm.png");
	wTex_sunny = IMG_Load("/screen/textures/weather/sunny.png");
	wTex_cold = IMG_Load("/screen/textures/weather/cold.png");
	wTex_windy = IMG_Load("/screen/textures/weather/windy.png");
	wTex_flurries = IMG_Load("/screen/textures/weather/flurries.png");
	wTex_chance_of_snow = IMG_Load("/screen/textures/weather/chance_of_snow.png");
	wTex_chance_of_rain = IMG_Load("/screen/textures/weather/chance_of_rain.png");
	wTex_fog = IMG_Load("/screen/textures/weather/fog.png");
	wTex_icy = IMG_Load("/screen/textures/weather/icy.png");
	wTex_sleet = IMG_Load("/screen/textures/weather/sleet.png");
	wTex_rain = IMG_Load("/screen/textures/weather/rain.png");
	wTex_mist = IMG_Load("/screen/textures/weather/mist.png");
	wTex_haze = IMG_Load("/screen/textures/weather/haze.png");
	wTex_smoke = IMG_Load("/screen/textures/weather/smoke.png");
	wTex_snow = IMG_Load("/screen/textures/weather/snow.png");
	wTex_hot = IMG_Load("/screen/textures/weather/hot.png");
	wTex_thunderstorm = IMG_Load("/screen/textures/weather/thunderstorm.png");

	/* This is a testing texture */
	boardTex_B = IMG_Load("/screen/boards/2/1.png");

	SDL_ShowCursor(SDL_DISABLE); 

	glEnable(GL_BLEND);
	glClearColor(0, 0, 0, 1);
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
	/* This is the main processing and rendering function.
	   In here, we call all required information and pull what we need. */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	int days;
	char daysInWord[3];
	char monthsInWord[3];
	char nthsInWord[2];
	char dateString[25];
	char mins[2];

	time_t now = time(0);
	tm *ltm = localtime(&now);

	/* Calculate the day for Dec 31 of the previous year */
	days = calcDay_Dec31(1900 + ltm->tm_year);

	/* Calculate the day for the given date */
	days = (dayInYear(ltm->tm_mday, ltm->tm_mon) + days) % 7;

	/* Add one day if the year is leap year and desired date is after February */
	if ((!((1900 + ltm->tm_year) % 4) && ((1900 + ltm->tm_year) % 100) || !((1900 + ltm->tm_year) % 400)) && ltm->tm_mon > 2)
		days++;

	/* Get string values for Day and Year */
	dayInStr(daysInWord, days);	
	monthInStr(monthsInWord, ltm->tm_mon);
	nthInStr(nthsInWord, ltm->tm_mday);

	/* Add a leading 0 to the date if we are less than the 10th into the month */
	if (ltm->tm_min < 10)
		sprintf(mins, "0%i", ltm->tm_min);
	else
		sprintf(mins, "%i", ltm->tm_min);

	/* Create the Date/Time String - We want blinking : */
	if (tC1 != ltm->tm_sec)
	{
		if (bV1)
			bV1 = false;
		else
			bV1 = true;

		/* Process Other Counters */
		bCTimer[0]++;
		bCTimer[1]++;
		bCTimer[2]++;
		bCTimer[3]++;
		tC1 = ltm->tm_sec;
	}

	sprintf(dateString, "%i %s - %s, %s %i  %i", ltm->tm_hour, mins, daysInWord, monthsInWord, ltm->tm_mday, (1900 + ltm->tm_year));

	/* Main Drawing Section*/

	/* Draw Text */
	drawText("Notification Centre", fntCGothic48, 1, 255, 255, 255, 255, 420, 670);
	drawText(dateString, fntCGothic34, 2, 255, 255, 255, 255, 1270, 15);

	if (bV1)
		if (ltm->tm_hour > 9)
			drawText(":", fntCGothic34, 2, 255, 255, 255, 255, 1275 - pTWidth + 42, 17);
		else
			drawText(":", fntCGothic34, 2, 255, 255, 255, 255, 1275 - pTWidth + 22, 17);

	drawText(nthsInWord, fntCGothic16, 1, 255, 255, 255, 255, 1175, 38);

	/* Do Weather Check (update once every 15 minutes) */
	if ((wLastCheckH != ltm->tm_hour) || (ltm->tm_min == 15) || (ltm->tm_min == 30) || (ltm->tm_min == 45) || (ltm->tm_min == 0))
	{
		if (wLastCheckM != ltm->tm_min)
		{
			/* Update last check interval */
			wLastCheckM = ltm->tm_min;

			if (ltm->tm_min < 10)
				{
				printf("Weather Update - %i:0%i\n", ltm->tm_hour, ltm->tm_min);
				}
			else
				printf("Weather Update - %i:%i\n", ltm->tm_hour, ltm->tm_min);

			/* Hour is odd, we call check */
			tFarenheight=0;
			tCondition=0;
			tHumidity=0;
			tIcon=0;
			tWind=0;
			wOK = false; /* New Check - default wOK to false (so data regarded dirty first) */
			xmlDoc *doc = NULL;
			xmlNode *root_element = NULL;

			LIBXML_TEST_VERSION    // Macro to check API for match with
			                       // the DLL we are using

			/*parse the file and get the DOM */
			doc = xmlReadFile("http://www.google.com/ig/api?weather=ST150QN", NULL, 0);
			if (doc)
			{

				/*Get the root element node */
				root_element = xmlDocGetRootElement(doc);
				parseWeather(root_element);
				xmlFreeDoc(doc);       // free document
				xmlCleanupParser();    // Free globals
				/* Update last check interval */
				if (wOK)
					wLastCheckH = ltm->tm_hour;
				else
					wLastCheckH = 0;
			} else {
				/* Update last check interval (we want to check in another minute) */
				printf("NO DATA/NET CONNECTION\n");
				wLastCheckH = 0;
				wOK = false;
			}
			/* Calculate Weather */
			wCelcius = floorf(((5.0 / 9.0) * (wFarenheight - 32.0)) * 10 + 0.5) / 10;
			sprintf(wTemp, "%.1fºC", wCelcius);
		}
	}

	if (wOK)
	{
		/* Do Looping Weather Animations */
		if (now > (wUpdateTimer[1] + 5)) /* Temperature Alert Flash */
		{
			if ((wCelcius <= 3.0) || (wCelcius >= 25.0))
			{
				wUpdateTimer[1]=now;
				switch(wFadeA[1])
				{
				case 0:wFadeA[1]=1;;break;
				case 1:wFadeA[1]=2;;break;
				case 2:wFadeA[1]=1;;break;
				}
				dAnim[1]=1;
			} else
				wFadeA[1]=0;
		}

		if (now > (wUpdateTimer[0] + 15)) /* Weather Condition Cycle */
		{
			wUpdateTimer[0]=now;
			wFadeA[0]=1;
			dAnim[0]=1;
		}

		/* Process Alternating Condition Icon */
		if ((wFadeA[1] == 1) || (wFadeA[1] == 2))
		{
			switch(wFadeA[1])
			{
			case 1:wFadeV[1]=wFadeV[1]-15;;break;
			case 2:wFadeV[1]=wFadeV[1]+15;;break;
			}

			if ((wFadeV[1] > 255) && (wFadeA[1]==2))
			{
				wFadeV[1] = 255;
				dAnim[1]=0;
			}
			if ((wFadeV[1] < 0) && (wFadeA[1]==1))
			{
				wFadeV[1] = 0;
				dAnim[1]=0;
			}
		}

		/* Process Fading Weather Info */
		if ((wFadeA[0] == 1) || (wFadeA[0] == 2))
		{
			switch(wFadeA[0])
			{
			case 1:wFadeV[0]=wFadeV[0]-15;;break;
			case 2:wFadeV[0]=wFadeV[0]+15;;break;
			}
			if (wFadeV[0] <0)
			{
				wFadeA[0]=2;
				wFadeV[0]=0;
				wCurDisp++;
				if (wCurDisp == 3)
					wCurDisp = 0;
			}
			if (wFadeV[0] >255)
			{
				wFadeV[0]=255;
				wFadeA[0]=0;
				dAnim[0]=0;
			}
		}

		pTWidth = 0;

		if (wCelcius <= 3.0)
			drawText(wTemp, fntCGothic34, 1, 128, 128, 255, 255, 10, 15);
		else if (wCelcius >= 25.0)
			drawText(wTemp, fntCGothic34, 1, 255, 0, 0, 255, 10, 15);
		else
			drawText(wTemp, fntCGothic34, 1, 255, 255, 255, 255, 10, 15);

		int pCIW = pTWidth;

		/* Draw weather condition icon */
		if (wCelcius <= 3.0)
			drawTexture(wTex_cold, pCIW+12, 5, (255-wFadeV[1]),4);
		else if (wCelcius >= 25.0)
			drawTexture(wTex_hot, pCIW+12, 5, (255-wFadeV[1]),4);

		if (strcmp("/ig/images/weather/chance_of_storm.gif", wIcon) == 0 )
			drawTexture(wTex_chance_of_storm, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/mostly_sunny.gif", wIcon) == 0 )
			drawTexture(wTex_mostly_sunny, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/dust.gif", wIcon) == 0 )
			drawTexture(wTex_dust, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/mostly_cloudy.gif", wIcon) == 0 )
			drawTexture(wTex_mostly_cloudy, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/cloudy.gif", wIcon) == 0 )
			drawTexture(wTex_cloudy, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/chance_of_tstorm.gif", wIcon) == 0 )
			drawTexture(wTex_chance_of_tstorm, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/partly_cloudy.gif", wIcon) ==0 )
			drawTexture(wTex_partly_cloudy, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/storm.gif", wIcon) == 0 )
			drawTexture(wTex_storm, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/sunny.gif", wIcon) == 0 )
			drawTexture(wTex_sunny, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/flurries.gif", wIcon) == 0 )
			drawTexture(wTex_flurries, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/chance_of_snow.gif", wIcon) == 0 )
			drawTexture(wTex_chance_of_snow, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/chance_of_rain.gif", wIcon) == 0 )
			drawTexture(wTex_chance_of_rain, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/fog.gif", wIcon) == 0 )
			drawTexture(wTex_fog, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/icy.gif", wIcon) == 0 )
			drawTexture(wTex_icy, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/sleet.gif", wIcon) == 0 )
			drawTexture(wTex_sleet, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/rain.gif", wIcon) == 0 )
			drawTexture(wTex_rain, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/mist.gif", wIcon) == 0 )
			drawTexture(wTex_mist, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/haze.gif", wIcon) == 0 )
			drawTexture(wTex_haze, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/smoke.gif", wIcon) == 0 )
			drawTexture(wTex_smoke, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/snow.gif", wIcon) == 0 )
			drawTexture(wTex_snow, pCIW+12, 5, wFadeV[1],4);

		if (strcmp("/ig/images/weather/thunderstorm.gif", wIcon) == 0 )
			drawTexture(wTex_thunderstorm, pCIW+12, 5, wFadeV[1],4);

		/* Draw Leaves over Weather Icon if wind is 20mph+ */
		if (wIWind >= 20)
			drawTexture(wTex_windy, pCIW+12, 5, 255, 4);

		switch(wCurDisp)
		{
			case 0:drawText(wCondition, fntCGothic34, 1, 255, 255, 255, wFadeV[0], pCIW + pTWidth + 15, 15);;break;
			case 1:drawText(wHumidity, fntCGothic34, 1, 255, 255, 255, wFadeV[0], pCIW + pTWidth + 15, 15);;break;
			case 2:drawText(wWind, fntCGothic34, 1, 255, 255, 255, wFadeV[0], pCIW + pTWidth + 15, 15);;break;
		}
	} else {
		drawText("Weather Unavailable", fntCGothic34, 1, 255, 255, 255, 255, 10, 15);
	}

	/* Draw Boards */
	/* We need to pull and process all information from /screen/boards 
	   Also checki any other information we need on the 15s Interval */
	if (bCTimer[0] > 14) {
		/* Do the check every 15 seconds */

		if (ltm->tm_min < 10)
			{
			if (ltm->tm_sec < 10)
				printf("Performing 15s Timer Check - %i:0%i:0%i\n", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
			else
				printf("Performing 15s Timer Check - %i:0%i:%i\n", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
			}
		else
			{
			if (ltm->tm_sec < 10)
				printf("Performing 15s Timer Check - %i:%i:0%i\n", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
			else
				printf("Performing 15s Timer Check - %i:%i:%i\n", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
			}

		/* Determine if Logo is White or Black */
		if (FileExists("/screen/white"))
			logoisWhite=1;

		/* LH Board Check */
		if (FileExists("/screen/boards/1/bTimeStamp")) {
			if (FileExists("/screen/boards/1/bDuration")) {
				if (FileExists("/screen/boards/1/bNum")) {
					/* Board Active, pull data to variables */
					char str[20];
					int tTS=0;
					FILE *fp;
					fp = fopen("/screen/boards/1/bTimeStamp", "r");
					while(fgets(str,sizeof(str),fp) != NULL)
					{
						/* strip trailing '\n' if it exists */
						int len = strlen(str)-1;
						if(str[len] == '\n') 
							str[len] = 0;
						tTS = strtol(str,NULL,0);
					}
					fclose(fp);

					if (bTimeStamp[0] != tTS)
					{
						/* Transition to new Board */
						if (bTimeStamp[0] == 0)
							bCondition[0] = 1;
						else
							bCondition[0] = 3;

						bTimeStamp[0] = tTS;

						/* Parse other files */
						fp = fopen("/screen/boards/1/bDuration", "r");
						while(fgets(str,sizeof(str),fp) != NULL)
						{
							/* strip trailing '\n' if it exists */
							int len = strlen(str)-1;
							if(str[len] == '\n') 
								str[len] = 0;
							tTS = strtol(str,NULL,0);
						}
						fclose(fp);

						bDuration[0] = tTS;

						fp = fopen("/screen/boards/1/bNum", "r");
						while(fgets(str,sizeof(str),fp) != NULL)
						{
							/* strip trailing '\n' if it exists */
							int len = strlen(str)-1;
							if(str[len] == '\n') 
								str[len] = 0;
							tTS = strtol(str,NULL,0);
						}
						fclose(fp);

						bNo[0] = tTS;
						bCNo[0] = 0;
					}
				} else {
					/* Board Not Active */
					bTimeStamp[0]=0;
					bCondition[0]=3;
				}
			} else {
				/* Board Not Active */
				bTimeStamp[0]=0;
				bCondition[0]=3;
			}
		} else {
			/* Board Not Active */
			bTimeStamp[0]=0;
			bCondition[0]=3;
		}

		/* RH Board Check */
		if (FileExists("/screen/boards/2/bTimeStamp")) {
			if (FileExists("/screen/boards/2/bDuration")) {
				if (FileExists("/screen/boards/2/bNum")) {
					/* Board Active, pull data to variables */
					char str[20];
					int tTS=0;
					FILE *fp;
					fp = fopen("/screen/boards/2/bTimeStamp", "r");
					while(fgets(str,sizeof(str),fp) != NULL)
					{
						/* strip trailing '\n' if it exists */
						int len = strlen(str)-1;
						if(str[len] == '\n') 
							str[len] = 0;
						tTS = strtol(str,NULL,0);
					}
					fclose(fp);

					if (bTimeStamp[1] != tTS)
					{
						/* Transition to new Board */
						if (bTimeStamp[1] == 0)
							bCondition[1] = 1;
						else
							bCondition[1] = 3;

						bTimeStamp[1] = tTS;

						/* Parse other files */
						fp = fopen("/screen/boards/2/bDuration", "r");
						while(fgets(str,sizeof(str),fp) != NULL)
						{
							/* strip trailing '\n' if it exists */
							int len = strlen(str)-1;
							if(str[len] == '\n') 
								str[len] = 0;
							tTS = strtol(str,NULL,0);
						}
						fclose(fp);

						bDuration[1] = tTS;

						fp = fopen("/screen/boards/2/bNum", "r");
						while(fgets(str,sizeof(str),fp) != NULL)
						{
							/* strip trailing '\n' if it exists */
							int len = strlen(str)-1;
							if(str[len] == '\n') 
								str[len] = 0;
							tTS = strtol(str,NULL,0);
						}
						fclose(fp);

						bNo[1] = tTS;
						bCNo[1] = 0;
					}
				} else {
					/* Board Not Active */
					bTimeStamp[1]=0;
					bCondition[1]=3;
				}
			} else {
				/* Board Not Active */
				bTimeStamp[1]=0;
				bCondition[1]=3;
			}
		} else {
			/* Board Not Active */
			bTimeStamp[1]=0;
			bCondition[1]=3;
		}

		/* FS Board Check */
		if (FileExists("/screen/boards/3/bTimeStamp")) {
			if (FileExists("/screen/boards/3/bDuration")) {
				if (FileExists("/screen/boards/3/bNum")) {
					/* Board Active, pull data to variables */
					char str[20];
					int tTS=0;
					FILE *fp;
					fp = fopen("/screen/boards/3/bTimeStamp", "r");
					while(fgets(str,sizeof(str),fp) != NULL)
					{
						/* strip trailing '\n' if it exists */
						int len = strlen(str)-1;
						if(str[len] == '\n') 
							str[len] = 0;
						tTS = strtol(str,NULL,0);
					}
					fclose(fp);

					if (bTimeStamp[2] != tTS)
					{
						/* Transition to new Board */
						if (bTimeStamp[2] == 0)
							bCondition[2] = 1;
						else
							bCondition[2] = 3;

						bTimeStamp[2] = tTS;

						/* Parse other files */
						fp = fopen("/screen/boards/3/bDuration", "r");
						while(fgets(str,sizeof(str),fp) != NULL)
						{
							/* strip trailing '\n' if it exists */
							int len = strlen(str)-1;
							if(str[len] == '\n') 
								str[len] = 0;
							tTS = strtol(str,NULL,0);
						}
						fclose(fp);

						bDuration[2] = tTS;

						fp = fopen("/screen/boards/3/bNum", "r");
						while(fgets(str,sizeof(str),fp) != NULL)
						{
							/* strip trailing '\n' if it exists */
							int len = strlen(str)-1;
							if(str[len] == '\n') 
								str[len] = 0;
							tTS = strtol(str,NULL,0);
						}
						fclose(fp);

						bNo[2] = tTS;
						bCNo[2] = 0;
					}
				} else {
					/* Board Not Active */
					bTimeStamp[2]=0;
					bCondition[2]=3;
				}
			} else {
				/* Board Not Active */
				bTimeStamp[2]=0;
				bCondition[2]=3;
			}
		} else {
			/* Board Not Active */
			bTimeStamp[2]=0;
			bCondition[2]=3;
		}

		if ((bTimeStamp[0]==0) && (bTimeStamp[1]==0) && (bTimeStamp[2]==0))
			printf("*No Boards Defined!\n");

		bCTimer[0]=0;
	}

	/* Process Board Animation/Texture Loading */
	/* Board 1 */
	if (bTimeStamp[0] != 0)
	{
		if (bCondition[0] == 1)
		{
			/* Load Texture */
			bCNo[0]++;
			if (bCNo[0] > bNo[0])
				bCNo[0] = 1;
			sprintf(tFName, "/screen/boards/1/%i.png", bCNo[0]);
			SDL_FreeSurface(boardTex_A);
			boardTex_A = IMG_Load(tFName);
			bVisible[0] = 1;
			if (bNCondition[0] != 0)
				bCondition[0] = bNCondition[0];
			else
				bCondition[0] = 2;
			bNCondition[0] = 0;
			bCTimer[1] = 0;
			printf("Tex Loaded %s\n", tFName);
		}
		if (bCondition[0] == 2)
		{
			/* Fade in (New Board) */
			if (bBAlpha[0] != 255)
			{
				bBAlpha[0] = bBAlpha[0] + 15;
				dAnim[2] = 1;
				if (bBAlpha[0] > 255)
				{
					bBAlpha[0] = 255;
					bCondition[0] = 0;
					dAnim[2] = 0;
				}
			} else {
				bCondition[0] = 0;
				dAnim[2] = 0;
			}
			bCAlpha[0] = bBAlpha[0];
		}
		if (bCondition[0] == 3)
		{
			/* Fade out (New Board) */
			if (bBAlpha[0] != 0)
			{
				bBAlpha[0] = bBAlpha[0] - 15;
				dAnim[2] = 1;
				if (bBAlpha[0] < 0)
				{
					bBAlpha[0] = 0;
					bCondition[0] = 1;
					dAnim[2] = 0;
					bScroll[0] = 0;
				}
			} else {
				bCondition[0] = 1;
				dAnim[2] = 0;
				bScroll[0] = 0;
			}
			bCAlpha[0] = bBAlpha[0];
		}

		if (bCondition[0] == 4)
		{
			/* Fade out (Content Scroll Reached) */
			if (bCAlpha[0] != 0)
			{
				bCAlpha[0] = bCAlpha[0] - 15;
				dAnim[2] = 1;
				if (bCAlpha[0] < 0)
				{
					bCAlpha[0] = 0;
					bCondition[0] = 5;
					dAnim[2] = 0;
					bScroll[0] = 0;
				}
			} else {
				bCondition[0] = 5;
				dAnim[2] = 0;
				bScroll[0] = 0;
			}
		}

		if (bCondition[0] == 5)
		{
			/* Fade in (Content Scroll Reached) */
			if (bCAlpha[0] != 255)
			{
				bCAlpha[0] = bCAlpha[0] + 15;
				dAnim[2] = 1;
				if (bCAlpha[0] > 255)
				{
					bCAlpha[0] = 255;
					bCondition[0] = 5;
					dAnim[2] = 0;
					bScroll[0] = 0;
				}
			} else {
				bCondition[0] = 0;
				dAnim[2] = 0;
				bScroll[0] = 0;
			}
		}

		if (bCondition[0] == 6)
		{
			/* Fade out (then load new pane) */
			if (bCAlpha[0] != 0)
			{
				bCAlpha[0] = bCAlpha[0] - 15;
				dAnim[2] = 1;
				if (bCAlpha[0] < 0)
				{
					bCAlpha[0] = 0;
					bCondition[0] = 1;
					bNCondition[0] = 5;
					dAnim[2] = 0;
					bScroll[0] = 0;
				}
			} else {
				bCondition[0] = 1;
				bNCondition[0] = 5;
				dAnim[2] = 0;
				bScroll[0] = 0;
			}
		}

		if (bCondition[0] == 7)
		{
			/* Fade in (new pane loaded) */
			if (bCAlpha[0] != 255)
			{
				bCAlpha[0] = bCAlpha[0] + 15;
				dAnim[2] = 1;
				if (bCAlpha[0] > 255)
				{
					bCAlpha[0] = 255;
					bCondition[0] = 5;
					dAnim[2] = 0;
					bScroll[0] = 0;
				}
			} else {
				bCondition[0] = 0;
				dAnim[2] = 0;
				bScroll[0] = 0;
			}
		}
	} else {
		/* No Board.  Fade old if present */
		if (bCondition[0] == 3)
		{
			if (bBAlpha[0] != 0)
			{
				bBAlpha[0] = bBAlpha[0] - 15;
				dAnim[2] = 1;
				if (bBAlpha[0] < 0)
				{
					bBAlpha[0] = 0;
					bCondition[0] = 0;
					bVisible[0] = 0;
					dAnim[2] = 0;
					bScroll[0] = 0;
					SDL_FreeSurface(boardTex_A);
				}
			} else {
				bCondition[0] = 0;
				bVisible[0] = 0;
				dAnim[2] = 0;
				bScroll[0] = 0;
				SDL_FreeSurface(boardTex_A);
			}
			bCAlpha[0] = bBAlpha[0];
		}
	}

	/* Board 2 */
	if (bTimeStamp[1] != 0)
	{
		if (bCondition[1] == 1)
		{
			/* Load Texture */
			bCNo[1]++;
			if (bCNo[1] > bNo[1])
				bCNo[1] = 1;
			sprintf(tFName, "/screen/boards/2/%i.png", bCNo[1]);
			SDL_FreeSurface(boardTex_B);
			boardTex_B = IMG_Load(tFName);
			bVisible[1] = 1;
			if (bNCondition[1] != 0)
				bCondition[1] = bNCondition[1];
			else
				bCondition[1] = 2;
			bNCondition[1] = 0;
			bCTimer[2] = 0;
			printf("Tex Loaded %s\n", tFName);
		}
		if (bCondition[1] == 2)
		{
			/* Fade in (New Board) */
			if (bBAlpha[1] != 255)
			{
				bBAlpha[1] = bBAlpha[1] + 15;
				dAnim[3] = 1;
				if (bBAlpha[1] > 255)
				{
					bBAlpha[1] = 255;
					bCondition[1] = 0;
					dAnim[3] = 0;
				}
			} else {
				bCondition[1] = 0;
				dAnim[3] = 0;
			}
			bCAlpha[1] = bBAlpha[1];
		}
		if (bCondition[1] == 3)
		{
			/* Fade out (New Board) */
			if (bBAlpha[1] != 0)
			{
				bBAlpha[1] = bBAlpha[1] - 15;
				dAnim[3] = 1;
				if (bBAlpha[1] < 0)
				{
					bBAlpha[1] = 0;
					bCondition[1] = 1;
					dAnim[3] = 0;
					bScroll[1] = 0;
				}
			} else {
				bCondition[1] = 1;
				dAnim[3] = 0;
				bScroll[1] = 0;
			}
			bCAlpha[1] = bBAlpha[1];
		}

		if (bCondition[1] == 4)
		{
			/* Fade out (Content Scroll Reached) */
			if (bCAlpha[1] != 0)
			{
				bCAlpha[1] = bCAlpha[1] - 15;
				dAnim[3] = 1;
				if (bCAlpha[1] < 0)
				{
					bCAlpha[1] = 0;
					bCondition[1] = 5;
					dAnim[3] = 0;
					bScroll[1] = 0;
				}
			} else {
				bCondition[1] = 5;
				dAnim[3] = 0;
				bScroll[1] = 0;
			}
		}

		if (bCondition[1] == 5)
		{
			/* Fade in (Content Scroll Reached) */
			if (bCAlpha[1] != 255)
			{
				bCAlpha[1] = bCAlpha[1] + 15;
				dAnim[3] = 1;
				if (bCAlpha[1] > 255)
				{
					bCAlpha[1] = 255;
					bCondition[1] = 5;
					dAnim[3] = 0;
					bScroll[1] = 0;
				}
			} else {
				bCondition[1] = 0;
				dAnim[3] = 0;
				bScroll[1] = 0;
			}
		}

		if (bCondition[1] == 6)
		{
			/* Fade out (then load new pane) */
			if (bCAlpha[1] != 0)
			{
				bCAlpha[1] = bCAlpha[1] - 15;
				dAnim[3] = 1;
				if (bCAlpha[1] < 0)
				{
					bCAlpha[1] = 0;
					bCondition[1] = 1;
					bNCondition[1] = 5;
					dAnim[3] = 0;
					bScroll[1] = 0;
				}
			} else {
				bCondition[1] = 1;
				bNCondition[1] = 5;
				dAnim[3] = 0;
				bScroll[1] = 0;
			}
		}

		if (bCondition[1] == 7)
		{
			/* Fade in (new pane loaded) */
			if (bCAlpha[1] != 255)
			{
				bCAlpha[1] = bCAlpha[1] + 15;
				dAnim[3] = 1;
				if (bCAlpha[1] > 255)
				{
					bCAlpha[1] = 255;
					bCondition[1] = 5;
					dAnim[3] = 0;
					bScroll[1] = 0;
				}
			} else {
				bCondition[1] = 0;
				dAnim[3] = 0;
				bScroll[1] = 0;
			}
		}
	} else {
		/* No Board.  Fade old if present */
		if (bCondition[1] == 3)
		{
			if (bBAlpha[1] != 0)
			{
				bBAlpha[1] = bBAlpha[1] - 15;
				dAnim[3] = 1;
				if (bBAlpha[1] < 0)
				{
					bBAlpha[1] = 0;
					bCondition[1] = 0;
					bVisible[1] = 0;
					dAnim[3] = 0;
					bScroll[1] = 0;
					SDL_FreeSurface(boardTex_B);
				}
			} else {
				bCondition[1] = 0;
				bVisible[1] = 0;
				dAnim[3] = 0;
				bScroll[1] = 0;
				SDL_FreeSurface(boardTex_B);
			}
			bCAlpha[1] = bBAlpha[1];
		}
	}

	/* Board 3 */
	if (bTimeStamp[2] != 0)
	{
		if (bCondition[2] == 1)
		{
			/* Load Texture */
			bCNo[2]++;
			if (bCNo[2] > bNo[2])
				bCNo[2] = 1;
			sprintf(tFName, "/screen/boards/3/%i.png", bCNo[2]);
			SDL_FreeSurface(boardTex_C);
			boardTex_C = IMG_Load(tFName);
			bVisible[2] = 1;
			if (bNCondition[2] != 0)
				bCondition[2] = bNCondition[2];
			else
				bCondition[2] = 2;
			bNCondition[2] = 0;
			bCTimer[3] = 0;
			printf("Tex Loaded %s\n", tFName);
		}
		if (bCondition[2] == 2)
		{
			/* Fade in (New Board) */
			if (bBAlpha[2] != 255)
			{
				bBAlpha[2] = bBAlpha[2] + 15;
				dAnim[4] = 1;
				if (bBAlpha[2] > 255)
				{
					bBAlpha[2] = 255;
					bCondition[2] = 0;
					dAnim[4] = 0;
				}
			} else {
				bCondition[2] = 0;
				dAnim[4] = 0;
			}
			bCAlpha[2] = bBAlpha[2];
		}
		if (bCondition[2] == 3)
		{
			/* Fade out (New Board) */
			if (bBAlpha[2] != 0)
			{
				bBAlpha[2] = bBAlpha[2] - 15;
				dAnim[4] = 1;
				if (bBAlpha[2] < 0)
				{
					bBAlpha[2] = 0;
					bCondition[2] = 1;
					dAnim[4] = 0;
					bScroll[2] = 0;
				}
			} else {
				bCondition[2] = 1;
				dAnim[4] = 0;
				bScroll[2] = 0;
			}
			bCAlpha[2] = bBAlpha[2];
		}

		if (bCondition[2] == 4)
		{
			/* Fade out (Content Scroll Reached) */
			if (bCAlpha[2] != 0)
			{
				bCAlpha[2] = bCAlpha[2] - 15;
				dAnim[4] = 1;
				if (bCAlpha[2] < 0)
				{
					bCAlpha[2] = 0;
					bCondition[2] = 5;
					dAnim[4] = 0;
					bScroll[2] = 0;
				}
			} else {
				bCondition[2] = 5;
				dAnim[4] = 0;
				bScroll[2] = 0;
			}
		}

		if (bCondition[2] == 5)
		{
			/* Fade in (Content Scroll Reached) */
			if (bCAlpha[2] != 255)
			{
				bCAlpha[2] = bCAlpha[2] + 15;
				dAnim[4] = 1;
				if (bCAlpha[2] > 255)
				{
					bCAlpha[2] = 255;
					bCondition[2] = 5;
					dAnim[4] = 0;
					bScroll[2] = 0;
				}
			} else {
				bCondition[2] = 0;
				dAnim[4] = 0;
				bScroll[2] = 0;
			}
		}

		if (bCondition[2] == 6)
		{
			/* Fade out (then load new pane) */
			if (bCAlpha[2] != 0)
			{
				bCAlpha[2] = bCAlpha[2] - 15;
				dAnim[4] = 1;
				if (bCAlpha[2] < 0)
				{
					bCAlpha[2] = 0;
					bCondition[2] = 1;
					bNCondition[2] = 5;
					dAnim[4] = 0;
					bScroll[2] = 0;
				}
			} else {
				bCondition[2] = 1;
				bNCondition[2] = 5;
				dAnim[4] = 0;
				bScroll[2] = 0;
			}
		}

		if (bCondition[2] == 7)
		{
			/* Fade in (new pane loaded) */
			if (bCAlpha[2] != 255)
			{
				bCAlpha[2] = bCAlpha[2] + 15;
				dAnim[4] = 1;
				if (bCAlpha[2] > 255)
				{
					bCAlpha[2] = 255;
					bCondition[2] = 5;
					dAnim[4] = 0;
					bScroll[2] = 0;
				}
			} else {
				bCondition[2] = 0;
				dAnim[4] = 0;
				bScroll[2] = 0;
			}
		}
	} else {
		/* No Board.  Fade old if present */
		if (bCondition[2] == 3)
		{
			if (bBAlpha[2] != 0)
			{
				bBAlpha[2] = bBAlpha[2] - 15;
				dAnim[4] = 1;
				if (bBAlpha[2] < 0)
				{
					bBAlpha[2] = 0;
					bCondition[2] = 0;
					bVisible[2] = 0;
					dAnim[4] = 0;
					bScroll[2] = 0;
					SDL_FreeSurface(boardTex_C);
				}
			} else {
				bCondition[2] = 0;
				bVisible[2] = 0;
				dAnim[4] = 0;
				bScroll[2] = 0;
				SDL_FreeSurface(boardTex_C);
			}
			bCAlpha[2] = bBAlpha[2];
		}
	}

	/* Draw Boards */
	/* Process LH Board */
	if (bVisible[0]==1)
	{
		/* Scrolling */
		/* Only scroll when Alpha = 255 */
		if ((bBAlpha[0] == 255) && (bCAlpha[0] == 255))
		{
			if (boardTex_A->h > 588)
			{
				/* We have to scroll, it's taller than 588 pixels high */
				if ((bCTimer[1] > 14)  && (bCondition[0] == 0))
				{
					dAnim[2] = 1;
					bScroll[0]++;
				}				
			} else {
				/* It's not a scrolling image, so we use the timer variable */
				if (bCTimer[1] > bDuration[0])
				{
					bCTimer[1] = 0;
					if (bNo[0] > 1)
						bCondition[0] = 6;
				}
			}

			if ((bScroll[0] / 3) > (boardTex_A->h-588))
			{
				/* Instead of eventually hitting an upper limit on the scroll
				   we loop the scroll value to keep things clean */
				bScroll[0] = 3 * (boardTex_A->h - 588);
				if (bCTimer[1] > 15)
				{
					bCTimer[1] = 0;
					dAnim[2] = 0;
				}
				
				if (bCTimer[1] > 14)
				{
					bCTimer[1] = 0;
					if (bNo[0] > 1)
						bCondition[0] = 6;
					else
						bCondition[0] = 4;
					dAnim[2] = 0;
				}
			}
		}
		/* Draw Board */
		drawInfoBox(boardTex_A,
				1,
				(1280/2)-(boardTex_A->w)-12,
				78,
				1280-19-19,
				588,
				bScroll[0] / 3,
				1.0f,
				1.0f,
				1.0f,
				255,
				bBAlpha[0],
				bCAlpha[0]);
	}

	/* Process RH Board */
	if (bVisible[1]==1)
	{
		/* Scrolling */
		/* Only scroll when Alpha = 255 */
		if ((bBAlpha[1] == 255) && (bCAlpha[1] == 255))
		{
			if (boardTex_B->h > 588)
			{
				/* We have to scroll, it's taller than 588 pixels high */
				if ((bCTimer[2] > 14)  && (bCondition[1] == 0))
				{
					dAnim[3] = 1;
					bScroll[1]++;
				}				
			} else {
				/* It's not a scrolling image, so we use the timer variable */
				if (bCTimer[2] > bDuration[1])
				{
					bCTimer[2] = 0;
					if (bNo[1] > 1)
						bCondition[1] = 6;
				}
			}

			if ((bScroll[1] / 3) > (boardTex_B->h-588))
			{
				/* Instead of eventually hitting an upper limit on the scroll
				   we loop the scroll value to keep things clean */
				bScroll[1] = 3 * (boardTex_B->h - 588);
				if (bCTimer[2] > 15)
				{
					bCTimer[2] = 0;
					dAnim[3] = 0;
				}
				
				if (bCTimer[2] > 14)
				{
					bCTimer[2] = 0;
					if (bNo[1] > 1)
						bCondition[1] = 6;
					else
						bCondition[1] = 4;
					dAnim[3] = 0;
				}
			}
		}
		/* Draw Board */
		drawInfoBox(boardTex_B,
				1,
				(1280/2)+12,
				78,
				609,
				588,
				bScroll[1] / 3,
				1.0f,
				1.0f,
				1.0f,
				255,
				bBAlpha[1],
				bCAlpha[1]);
	}

	/* Process FS Board */
	if (bVisible[2]==1)
	{
		/* Scrolling */
		/* Only scroll when Alpha = 255 */
		if ((bBAlpha[2] == 255) && (bCAlpha[2] == 255))
		{
			if (boardTex_C->h > 588)
			{
				/* We have to scroll, it's taller than 588 pixels high */
				if ((bCTimer[3] > 14)  && (bCondition[2] == 0))
				{
					dAnim[4] = 1;
					bScroll[2]++;
				}				
			} else {
				/* It's not a scrolling image, so we use the timer variable */
				if (bCTimer[3] > bDuration[2])
				{
					bCTimer[3] = 0;
					if (bNo[2] > 1)
						bCondition[2] = 6;
				}
			}

			if ((bScroll[2] / 3) > (boardTex_C->h-588))
			{
				/* Instead of eventually hitting an upper limit on the scroll
				   we loop the scroll value to keep things clean */
				bScroll[2] = 3 * (boardTex_C->h - 588);
				if (bCTimer[3] > 15)
				{
					bCTimer[3] = 0;
					dAnim[4] = 0;
				}
				
				if (bCTimer[3] > 14)
				{
					bCTimer[3] = 0;
					if (bNo[2] > 1)
						bCondition[2] = 6;
					else
						bCondition[2] = 4;
					dAnim[4] = 0;
				}
			}
		}
		/* Draw Board */
		drawInfoBox(boardTex_C,
				2,
				19,
				78,
				1280-19-19,
				588,
				bScroll[2] / 3,
				1.0f,
				1.0f,
				1.0f,
				255,
				bBAlpha[2],
				bCAlpha[2]);
	}

	/* Orbital Logo above everything else */
	int logoScale=130;
	if (logoisWhite==0) {
		drawInfoBox(orb_logo,
				1,
				(1280/2)-((((orb_logo->w / 255.0) * logoScale)+8)/2),
				12,
				orb_logo->w,
				orb_logo->h,
				0,
				1.0f,
				1.0f,
				1.0f,
				logoScale,
				255,
				255);
	} else {
		drawInfoBox(orb_logo,
				2,
				(1280/2)-((((orb_logo->w / 255.0) * logoScale)+8)/2),
				12,
				orb_logo->w,
				orb_logo->h,
				0,
				1.0f,
				1.0f,
				1.0f,
				logoScale,
				255,
				255);
	}

	/* Are we Animating? */
	if ((dAnim[0] == 1) || (dAnim[1] == 1) || (dAnim[2] == 1) || (dAnim[3] == 1) || (dAnim[4] == 1))
		SCREEN_TARGET_FPS = 30;
	else
		SCREEN_TARGET_FPS = 5;

	SDL_GL_SwapBuffers();
}

int main( int argc, char* argv[] ) {
	if(init() == false) {
		return 1;
	}

	while ( IS_RUNNING ) {
		doDisplay();
		SDL_Delay(1000 / SCREEN_TARGET_FPS);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	TTF_CloseFont(fntCGothic22);
	TTF_CloseFont(fntCGothic16);
	TTF_CloseFont(fntCGothic34);
	TTF_CloseFont(fntCGothic44);
	TTF_CloseFont(fntCGothic48);

	/* Clear Textures */
	SDL_FreeSurface(orb_logo);
	SDL_FreeSurface(orb_bl);
	SDL_FreeSurface(orb_bt);
	SDL_FreeSurface(orb_bcrnr);
	SDL_FreeSurface(orb_wl);
	SDL_FreeSurface(orb_wt);
	SDL_FreeSurface(orb_wcrnr);
	SDL_FreeSurface(orb_tl);
	SDL_FreeSurface(orb_tt);
	SDL_FreeSurface(orb_tcrnr);
	SDL_FreeSurface(orb_boxb);
	SDL_FreeSurface(orb_boxw);
	SDL_FreeSurface(wTex_chance_of_storm);
	SDL_FreeSurface(wTex_mostly_sunny);
	SDL_FreeSurface(wTex_dust);
	SDL_FreeSurface(wTex_mostly_cloudy);
	SDL_FreeSurface(wTex_cloudy);
	SDL_FreeSurface(wTex_chance_of_tstorm);
	SDL_FreeSurface(wTex_partly_cloudy);
	SDL_FreeSurface(wTex_storm);
	SDL_FreeSurface(wTex_sunny);
	SDL_FreeSurface(wTex_cold);
	SDL_FreeSurface(wTex_windy);
	SDL_FreeSurface(wTex_flurries);
	SDL_FreeSurface(wTex_chance_of_snow);
	SDL_FreeSurface(wTex_chance_of_rain);
	SDL_FreeSurface(wTex_fog);
	SDL_FreeSurface(wTex_icy);
	SDL_FreeSurface(wTex_sleet);
	SDL_FreeSurface(wTex_rain);
	SDL_FreeSurface(wTex_mist);
	SDL_FreeSurface(wTex_haze);
	SDL_FreeSurface(wTex_smoke);
	SDL_FreeSurface(wTex_snow);
	SDL_FreeSurface(wTex_hot);
	SDL_FreeSurface(wTex_thunderstorm);
	SDL_FreeSurface(boardTex_A);
	SDL_FreeSurface(boardTex_B);
	SDL_FreeSurface(boardTex_C);
	SDL_FreeSurface(screen);

	TTF_Quit();
	SDL_Quit();
	return 0;
}

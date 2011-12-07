//-------------------------------------------------------------------------
//          Name: infodisplay.cpp
//        Author: Paul Wollaston
// Project Start: 25/12/11
// Last Modified: 07/12/11
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
int SCREEN_TARGET_FPS = 10;

/* Define Main Screen Surface */
SDL_Surface* screen = NULL;

/* Define Fonts */
TTF_Font *fntCGothic22;
TTF_Font *fntCGothic44;
TTF_Font *fntCGothic48;

/* Variables for application (global) */
bool IS_RUNNING = true;
int wLastCheckH, wLastCheckM, wUpdateTimer;
int wFadeV = 255;
int wFadeA = 0;
int wCurDisp=0;
int tC1 = 0;
bool bV1 = false;
char wTemp[4];
int wFarenheight=0;
char wCondition[32];
char wHumidity[32];
char wIcon[64];
char wWind[32];
float wCelcius=0.0;
int tFarenheight, tCondition, tHumidity, tIcon, tWind;

/* Define Global Textures */
SDL_Surface *orb_logo;
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

bool FileExists( const char* FileName );
void doDisplay();
bool init();

static void parseWeather(xmlNode * a_node)
{
	xmlNode *cur_node = NULL;
	char tWord[16];
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
	if (cur_node->type == XML_ELEMENT_NODE) {
		if (!xmlStrcmp(cur_node->name, (const xmlChar *) "condition" )) {
			if (tCondition == 0) {
				sprintf(wCondition, "%s", cur_node->properties->children->content);
				tCondition=1;
				printf("*UPDATE* node type: Element, name: %s ** data: %s\n",
					cur_node->name, wCondition);
				}
			}

		if (!xmlStrcmp(cur_node->name, (const xmlChar *) "temp_f" )) {
			sprintf(tWord, "%s", cur_node->properties->children->content);
			if (tFarenheight == 0) {
				wFarenheight = strtol(tWord,NULL,0);
				tFarenheight = wFarenheight;
				printf("*UPDATE* node type: Element, name: %s ** data: %i\n",
					cur_node->name, wFarenheight);
				}
			}

		if (!xmlStrcmp(cur_node->name, (const xmlChar *) "humidity" )) {
			if (tHumidity == 0) {
				tHumidity=1;
				sprintf(wHumidity, "%s", cur_node->properties->children->content);
				printf("*UPDATE* node type: Element, name: %s ** data: %s\n",
					cur_node->name, wHumidity);
				}
			}

		if (!xmlStrcmp(cur_node->name, (const xmlChar *) "icon" )) {
			if (tIcon == 0) {
				tIcon=1;
				sprintf(wIcon, "%s", cur_node->properties->children->content);
				printf("*UPDATE* node type: Element, name: %s ** data: %s\n",
					cur_node->name, wIcon);
				}
			}

		if (!xmlStrcmp(cur_node->name, (const xmlChar *) "wind_condition" )) {
			if (tWind == 0) {
				tWind=1;
				sprintf(wWind, "%s", cur_node->properties->children->content);
				printf("*UPDATE* node type: Element, name: %s ** data: %s\n",
					cur_node->name, wWind);
				}
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
	glEnable(GL_TEXTURE_2D);
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
	glEnable(GL_TEXTURE_2D);
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
	fntCGothic22 = TTF_OpenFont("/screen/fonts/cgothic.ttf", 22);
	if(fntCGothic22 == NULL) {
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
	char tBlinker[1];
	if (tC1 != ltm->tm_sec)
	{
		if (bV1)
			bV1 = false;
		else
			bV1 = true;
		tC1 = ltm->tm_sec;
	}
	if (bV1)
		sprintf(dateString, "%i:%s - %s, %s %i  %i", ltm->tm_hour, mins, daysInWord, monthsInWord, ltm->tm_mday, (1900 + ltm->tm_year));
	else
		sprintf(dateString, "%i %s - %s, %s %i  %i", ltm->tm_hour, mins, daysInWord, monthsInWord, ltm->tm_mday, (1900 + ltm->tm_year));

	/* Do Weather Check (update once every 15 minutes) */
	if ((wLastCheckH != ltm->tm_hour) || (ltm->tm_min == 15) || (ltm->tm_min == 30) || (ltm->tm_min == 45))
	{
		/* Update Timer */
		wLastCheckH = ltm->tm_hour;

		if (wLastCheckM != ltm->tm_min)
		{
			/* Only update if not already done so */
			wLastCheckM = ltm->tm_min;

			/* Hour is odd, we call check */
			tFarenheight=0;
			tCondition=0;
			tHumidity=0;
			tIcon=0;
			tWind=0;
			xmlDoc *doc = NULL;
			xmlNode *root_element = NULL;

			LIBXML_TEST_VERSION    // Macro to check API for match with
			                       // the DLL we are using

			/*parse the file and get the DOM */
			doc = xmlReadFile("http://www.google.com/ig/api?weather=ST150QN", NULL, 0);

			/*Get the root element node */
			root_element = xmlDocGetRootElement(doc);
			parseWeather(root_element);
			xmlFreeDoc(doc);       // free document
			xmlCleanupParser();    // Free globals

			/* Calculate Weather */
			wCelcius = floorf(((5.0 / 9.0) * (wFarenheight - 32.0)) * 10 + 0.5) / 10;

			sprintf(wTemp, "%.1fºC", wCelcius);
		}
	}

	/* Main Drawing Section*/

	drawTexture(orb_logo, 15, 620, 255,1);

	/* Draw Text */
	drawText("Notification Centre", fntCGothic48, 1, 255, 255, 255, 255, 450, 655);
	drawText(dateString, fntCGothic44, 2, 255, 255, 255, 255, 1270, 10);
	if (wCelcius <= 3.0)
		drawText(wTemp, fntCGothic44, 1, 255, 0, 0, 255, 18, 0);
	else
		drawText(wTemp, fntCGothic44, 1, 255, 255, 255, 255, 18, 10);
	drawText(nthsInWord, fntCGothic22, 1, 255, 255, 255, 255, 1147, 38);

	/* Do Looping Weather Info */
	if (now > (wUpdateTimer + 15))
	{
		wUpdateTimer=now;
		wFadeA=1;
		SCREEN_TARGET_FPS = 30;
	}

	/* Process Fading Weather Info (if applicable) */
	if ((wFadeA == 1) || (wFadeA == 2))
	{
		switch(wFadeA)
		{
		case 1:wFadeV=wFadeV-15;;break;
		case 2:wFadeV=wFadeV+15;;break;
		}
		if (wFadeV <0)
		{
			wFadeA=2;
			wFadeV=0;
			wCurDisp++;
			if (wCurDisp == 3)
				wCurDisp = 0;
		}
		if (wFadeV >255)
		{
			wFadeV=255;
			wFadeA=0;
			SCREEN_TARGET_FPS = 10;
		}
	}

	/* Draw weather condition icon */
	if (strcmp("/ig/images/weather/chance_of_storm.gif", wIcon) == 0 )
		drawTexture(wTex_chance_of_storm, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/mostly_sunny.gif", wIcon) == 0 )
		drawTexture(wTex_mostly_sunny, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/dust.gif", wIcon) == 0 )
		drawTexture(wTex_dust, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/mostly_cloudy.gif", wIcon) == 0 )
		drawTexture(wTex_mostly_cloudy, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/cloudy.gif", wIcon) == 0 )
		drawTexture(wTex_cloudy, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/chance_of_tstorm.gif", wIcon) == 0 )
		drawTexture(wTex_chance_of_tstorm, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/partly_cloudy.gif", wIcon) ==0 )
		drawTexture(wTex_partly_cloudy, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/storm.gif", wIcon) == 0 )
		drawTexture(wTex_storm, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/sunny.gif", wIcon) == 0 )
		drawTexture(wTex_sunny, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/flurries.gif", wIcon) == 0 )
		drawTexture(wTex_flurries, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/chance_of_snow.gif", wIcon) == 0 )
		drawTexture(wTex_chance_of_snow, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/chance_of_rain.gif", wIcon) == 0 )
		drawTexture(wTex_chance_of_rain, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/fog.gif", wIcon) == 0 )
		drawTexture(wTex_fog, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/icy.gif", wIcon) == 0 )
		drawTexture(wTex_icy, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/sleet.gif", wIcon) == 0 )
		drawTexture(wTex_sleet, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/rain.gif", wIcon) == 0 )
		drawTexture(wTex_rain, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/mist.gif", wIcon) == 0 )
		drawTexture(wTex_mist, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/haze.gif", wIcon) == 0 )
		drawTexture(wTex_haze, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/smoke.gif", wIcon) == 0 )
		drawTexture(wTex_smoke, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/snow.gif", wIcon) == 0 )
		drawTexture(wTex_snow, 185, 0, 255,3);

	if (strcmp("/ig/images/weather/thunderstorm.gif", wIcon) == 0 )
		drawTexture(wTex_thunderstorm, 185, 0, 255,3);

	switch(wCurDisp)
	{
		case 0:drawText(wCondition, fntCGothic44, 1, 255, 255, 255, wFadeV, 275, 10);;break;
		case 1:drawText(wHumidity, fntCGothic44, 1, 255, 255, 255, wFadeV, 275, 10);;break;
		case 2:drawText(wWind, fntCGothic44, 1, 255, 255, 255, wFadeV, 275, 10);;break;
	}

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
	TTF_CloseFont(fntCGothic44);
	TTF_CloseFont(fntCGothic48);

	/* Clear Textures */
	SDL_FreeSurface(orb_logo);
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
	SDL_FreeSurface(screen);

	TTF_Quit();
	SDL_Quit();
	return 0;
}

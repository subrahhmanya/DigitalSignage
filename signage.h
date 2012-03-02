/*
 * signage.h
 *
 *  Created on: 28 Feb 2012
 *      Author: pwollaston
 */

#ifndef SIGNAGE_H_
#define SIGNAGE_H_

#include "textures.h"

#include <X11/Xlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_syswm.h>

/* Include libxml */
#include <libxml/parser.h>
#include <libxml/tree.h>

/* Include standard C libraries */
#include <math.h>
#include <stdio.h>
#include <string>
#include <time.h>

class Signage {
public:
	Signage();
	void Init(const char* title, int width, int height, int bpp, bool fullscreen);
	void HandleEvents(Signage* signage);
	void Update();
	void Draw();
	void Clean();
	bool Running() {
		return m_bRunning;
	}
	void Quit() {
		m_bRunning = false;
	}
private:
	SDL_Surface* screen;
	bool m_bFullscreen, m_bRunning;
	Texture weather[24], layout[12];
	TTF_Font *fntCGothic[10];
	int pTWidth, tC1, tFarenheight, tCondition, tHumidity, tIcon, tWind, wFarenheight, wIWind;
	bool bV1, wOK;
	char nthsInWord[8], dateString[32];
	char wCondition[32], wHumidity[32], wIcon[64], wWind[32];
	int wLastCheckH;
	int wLastCheckM;
	int wUpdateTimer[32];
	float wCelcius;
	char wTemp[32];
	int iPlayerScale, iPlayerPosX, iPlayerPosY;
	tm *ltm;

	/* MPlayer Integration */
	Window play_win;
	FILE *mplayer_fp;

	void drawInfoBox(GLuint TextureID, int bcol, int px, int py, int minx, int miny, int scrollv, int absh, float br, float bg, float bb, int scale,
			int balpha, int calpha);

	void drawText(const char* text, TTF_Font*& fntChosen, int alignment, int cr, int cg, int cb, int alpha, int px, int py);

	int calcDay_Dec31(int yyyy);
	int dayInYear(int dd, int mm);
	void dayInStr(char daysInWord[], int days);
	void monthInStr(char monthsInWord[], int month);
	void nthInStr(char dowInWord[], int monthday);
	void parseWeather(xmlNode * a_node);
	void createiPlayer(int width, int height, int x, int y, int scale);
	Window create_x11_subwindow(Display *dpy, Window parent, int x, int y, int width, int height);
	Window create_sdl_x11_subwindow(int x, int y, int width, int height);
	SDL_SysWMinfo get_sdl_wm_info(void);
	void create_iplayer(const char *streamid, const char *quality, int cache, Window win, FILE **mplayer_fp);
};

#endif /* SIGNAGE_H_ */

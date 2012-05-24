/*
 * signage.h
 *
 *  Created on: 28 Feb 2012
 *      Author: pwollaston
 */

#ifndef SIGNAGE_H_
#define SIGNAGE_H_

#include "textures.h"
#include "box.h"
#include "fps_counter.h"
#include "inifile.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

/* Include libxml */
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

/* Include standard C libraries */
#include <math.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <dirent.h>
#include <vector>
#include <algorithm>

using namespace std;

class Signage
{
public:
	Signage();
	void Init(const char* title, int width, int height, int bpp, bool fullscreen);
	void HandleEvents(Signage* signage);
	void Update();
	void Draw();
	void Clean();
	bool Running()
	{
		return m_bRunning;
	}
	void Quit()
	{
		m_bRunning = false;
	}
private:
	SDL_Surface* screen;
	bool m_bFullscreen, m_bRunning;
	Texture pLogo;
	Texture weather[3];
	Texture bTex[64];
	Box iBoxes[128];
	TTF_Font *fntCGothic[10];
	int pTWidth, pTHeight, tC1, tFarenheight, tCondition, tHumidity, tIcon, tOIcon, tWind, tSrS, wFarenheight, wIWind, wFadeTI;
	int sWidth, sHeight;
	bool bV1, wOK;
	char nthsInWord[8], dateString[32];
	char wCondition[32], wHumidity[32], wIcon[64], wWind[32];
	int wFadeA[5], wFadeV[5], wCurDisp, wLastCheckH, wLastCheckM, wUpdateTimer[32];
	float wCelcius;
	char wTemp[32];

	tm *ltm;

	/* FPS */
	fps_counter counter;

	void drawText(const char* text, TTF_Font*& fntChosen, int alignment, int cr, int cg, int cb, int alpha, int px, int py);
	bool FileExists(const char* FileName);
	int calcDay_Dec31(int yyyy);
	int dayInYear(int dd, int mm);
	void dayInStr(char daysInWord[], int days);
	void monthInStr(char monthsInWord[], int month);
	void nthInStr(char dowInWord[], int monthday);
	void parseWeather(xmlNode * a_node);

	/* Board Specific Vars */
	bool validConfig[64];
	int tEn[64], tPX[64], tPY[64], tSc[64], tBr[64], tW[64], tH[64], tBt[64], tA[64], tTs[64], tDuration[64][64], tSType[64][64], tBC[64], tBR[64], pFade[64];
	char tFldr[64][1024], tUID[64][128], tType[64][64][128], tSrc[64][64][128], bSection[64][32];
};

#endif /* SIGNAGE_H_ */

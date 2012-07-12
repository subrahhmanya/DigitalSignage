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
#include <iostream>

using namespace std;

class Signage
{
public:
	Signage();
	void Init(const char* title, int width, int height, int bpp, bool fullscreen, const char* header, const char* weatherloc, const char* weathercountry,
			const char* weatherapi);
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
	bool m_bFullscreen, m_bRunning, m_bQuitting;
	Texture pLogo;
	Texture weather[3];
	Texture bTex[64];
	Texture tScrollTex[2];
	Box iBoxes[128];
	TTF_Font *fntCGothic[10];
	int pTWidth, pTHeight, tC1, tSrS, wFadeTI, tIcon, tOIcon, trObservationTime, trTemp, trWeatherCode, trWindSpeedMPH, trWindDir, trHumidity, trPressure, trCloudCover, trVisibility, trConditionDesc;

	/* BEGIN Weather Vars */

	char tObservationTime[16];
	int tTemp;
	int tWeatherCode;
	int tWindSpeedMPH;
	char tWindDir[8];
	int tHumidity;
	int tPressure;
	int tCloudCover;
	int tVisibility;
	char tConditionDesc[64];

	/* END Weather Vars */

	int sWidth, sHeight;
	bool bV1, wOK;
	char nthsInWord[8], dateString[32];
	char wIcon[64];
	int wFadeA[5], wFadeV[5], wCurDisp, wLastCheckH, wLastCheckM, wUpdateTimer[32], tScrolling[64], tScrollCycle[64], tScrollingTimer[64], tScrollSFader[64], tScrollEFader[64];
	float wCelcius;
	char wTemp[32];

	tm *ltm;

	/* FPS */
	fps_counter counter;

	int drawText(const char* text, TTF_Font*& fntChosen, int alignment, int cr, int cg, int cb, int alpha, int offset, int px, int py, int mw, int tScrollV);
	bool FileExists(const char* FileName);
	void dayInStr(char daysInWord[], int days);
	void monthInStr(char monthsInWord[], int month);
	void nthInStr(char dowInWord[], int monthday);
	void parseWeather(xmlNode * a_node);

	/* Board Specific Vars */
	bool validConfig[64], bChanger[64], bHeaderVis[64][64], tHeaderVis[64], tPRunning[64];
	int tEn[64], tPX[64], tPY[64], tSc[64], tBr[64], tW[64], tH[64], tBt[64], tA[64], tTs[64], tDuration[64][64], tSType[64][64], tSSpeed[64][64], tBC[64],
			tBR[64], tOR[64], pFade[64], aDuration[64], aSType[64], aSSpeed[64], aActive[64], tScrollV[64], tSComp[64], tSHeaderSize[64], tBHeaderSize[64][64];
	char tFldr[64][1024], tUID[64][128], tType[64][64][128], tSrc[64][64][128], aType[64][128], aSrc[64][128], bSection[64][32], tAudEnable[64][64][16],
			sHeader[256], tSHeaderTxt[64][256], sWLoc[64], sWCountry[64], sWAPI[64], tHeaderTxt[64][64][256], bPluginCmd[64][1024];
	FILE *pPluginCMD[64];
};

#endif /* SIGNAGE_H_ */

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
			const char* weatherapi, const char* versionstr, int dLevel);
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
	bool OKToQuit()
	{
		return m_bOKKill;
	}
private:
	time_t cTime;
	SDL_Surface* screen;
	bool m_bFullscreen, m_bRunning, m_bQuitting, m_bOKKill;
	Texture pLogo;
	Texture weather[4];
	Texture tScrollTex[2];
	Box iBoxes[128];
	TTF_Font *fntCGothic[49];
	int pTWidth, pTHeight, tC1, tSrS, wFadeTI, tIcon, tOIcon, trObservationTime, trTemp, trWeatherCode, trWindSpeedMPH, trWindDir, trHumidity, trPressure,
			trCloudCover, trVisibility, trConditionDesc;

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
	int wFadeA[5], wFadeV[5], wCurDisp, wLastCheckH, wLastCheckM, wUpdateTimer[32], tScrolling[64], tScrollCycle[64], tScrollingTimer[64], tScrollSFader[64],
			tScrollEFader[64], dTimeEvent, debugLevel;
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
	struct
	{
		char UID[128];
		char UIDS[128];
		int UIDI;
		char Folder[1024];
		int curBoard;
		int reqBoard;
		int oldBoard;
		int X;
		int Y;
		int Scale;
		int Border;
		int Width;
		int Height;
		int Scroll;
		int ScrollComplete;
		int pFade;
		int nBoards;
		int Alert;
		int AlertActive;
		char Header[128];
		int HeaderSize;
		bool HeaderVis;
		char sPluginCMD[1024];
		FILE *fPluginCMD;
		bool rPluginCMD;
		int TimeStampCFG;
		int TimeStampCheck;
		bool isDestroying;
		int CreatedID;

		/* Define Child Board Variables */
		struct
		{
			char Section[128];
			int Duration;
			char Type[128];
			char Src[128];
			int Quality;
			int ScrollSpeed;
			char Header[128];
			int HeaderSize;
			bool HeaderVis;
			char AudioOut[16];
			bool isAlert;
		} cBoard[64];
	} mBoard[64];
	Texture mBoardTex[64];

	char sHeader[256], sWLoc[64], sWCountry[64], sWAPI[64], sVersionString[32];
};

#endif /* SIGNAGE_H_ */

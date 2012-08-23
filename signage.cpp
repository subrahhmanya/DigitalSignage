/*
 * signage.cpp
 *
 *  Created on: 28 Feb 2012
 *      Author: pwollaston
 */

#include "signage.h"
#include "textures.h"

int FPSLimit = 30;

/* Sunrise */
double pi = 3.14;
double tpi = 2 * pi;
double degs = 180.0 / pi;
double rads = pi / 180.0;

double L, g, daylen;
double SunDia = 0.53; // Sunradius degrees

double AirRefr = 34.0 / 60.0; // athmospheric refraction degrees //

double FNday(int y, int m, int d, float h)
{
	long int luku = -7 * (y + (m + 9) / 12) / 4 + 275 * m / 9 + d;

	// Typecasting needed for TClite on PC DOS at least, to avoid product overflow
	luku += (long int) y * 367;

	return (double) luku - 730531.5 + h / 24.0;
}
;

// the function below returns an angle in the range
// 0 to 2*pi

double FNrange(double x)
{
	double b = x / tpi;
	double a = tpi * (b - (long) (b));
	if (a < 0)
		a = tpi + a;
	return a;
}
;

// Calculating the hourangle
double f0(double lat, double declin)
{

	double fo, dfo;
	// Correction: different sign at S HS
	dfo = rads * (0.5 * SunDia + AirRefr);
	if (lat < 0.0)
		dfo = -dfo;
	fo = tan(declin + dfo) * tan(lat * rads);

	if (fo > 0.99999)
		fo = 1.0; // to avoid overflow //
	fo = asin(fo) + pi / 2.0;
	return fo;
}
;

// Calculating the hourangle for twilight times
//
double f1(double lat, double declin)
{

	double fi, df1;
	// Correction: different sign at S HS
	df1 = rads * 6.0;
	if (lat < 0.0)
		df1 = -df1;
	fi = tan(declin + df1) * tan(lat * rads);

	if (fi > 0.99999)
		fi = 1.0; // to avoid overflow //
	fi = asin(fi) + pi / 2.0;
	return fi;
}
;

// Find the ecliptic longitude of the Sun
double FNsun(double d)
{

	// mean longitude of the Sun
	L = FNrange(280.461 * rads + .9856474 * rads * d);

	// mean anomaly of the Sun
	g = FNrange(357.528 * rads + .9856003 * rads * d);

	// Ecliptic longitude of the Sun
	return FNrange(L + 1.915 * rads * sin(g) + .02 * rads * sin(2 * g));
}
;

/* Signage constructor */
Signage::Signage()
{
	/* Preset variables */
	wLastCheckH = 99;
	wLastCheckM = 99;
	wUpdateTimer[0] = 0;
	wUpdateTimer[1] = 0;
	wUpdateTimer[2] = 0;
	wUpdateTimer[3] = 0;
	wUpdateTimer[4] = 0;
	wCurDisp = 99;

	tIcon = 0;
	tOIcon = 0;
	trObservationTime = 0;
	trTemp = 0;
	trWeatherCode = 0;
	trWindSpeedMPH = 0;
	trWindDir = 0;
	trHumidity = 0;
	trPressure = 0;

	tSrS = 0;
	bV1 = false;
	wOK = false;
	pTWidth = 0;
	wCelcius = 0.0;

	dTimeEvent = 0;

	trConditionDesc = 0;
	m_bRunning = NULL;
	screen = NULL;
	trVisibility = 0;
	tTemp = 0;
	tC1 = 0;
	sWidth = 0;
	m_bQuitting = NULL;
	sHeight = 0;
	tHumidity = 0;
	m_bOKKill = NULL;
	ltm = NULL;
	m_bFullscreen = NULL;
	tVisibility = 0;
	trCloudCover = 0;
	tPressure = 0;
	tWindSpeedMPH = 0;
	pTHeight = 0;
	debugLevel = 0;
	tCloudCover = 0;
	cTime = 0;
	tWeatherCode = 0;
}

void Signage::Init(const char* title, int width, int height, int bpp, bool fullscreen, const char* header, const char* weatherloc, const char* weathercountry,
		const char* weatherapi, const char* versionstr, int dLevel)
{
	/* Default to True, as we will falsify later if fail. */
	m_bRunning = true;
	m_bQuitting = false;
	m_bOKKill = false;
	wCurDisp = 99;
	debugLevel = dLevel;

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

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (fullscreen)
	{
		/* We're running full screen on the target, so use full screen */
		screen = SDL_SetVideoMode(width, height, bpp, SDL_ASYNCBLIT | SDL_FULLSCREEN | SDL_HWSURFACE | SDL_OPENGL);
		SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		/* We're running in a window (no fullscreen flag set) so don't pass SDL_FULLSCREEN */
		screen = SDL_SetVideoMode(width, height, bpp, SDL_ASYNCBLIT | SDL_HWSURFACE | SDL_OPENGL);
	}

	sWidth = width;
	sHeight = height;

	//go through and get the values to see if everything was set
	int red, green, blue, alpha, doublebuf;
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &green);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blue);
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alpha);
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doublebuf);
	if (debugLevel > 0)
		printf("Video Initialisation Results\nRed Size:\t%d\nGreen Size:\t%d\nBlue Size:\t%d\nAlpha Size:\t%d\nDouble Buffered? %s\n", red, green, blue, alpha,
				(doublebuf == 1 ? "Yes" : "No"));

	//print video card memory
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	if (debugLevel > 0)
	{
		if (info->video_mem == 0)
		{
			printf("**Unable to detect how much Video Memory is available**\n");
		}
		else
		{
			printf("Video Memory (in MB): %d\n", info->video_mem);
		}
	}

	if (screen == NULL)
	{
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		m_bRunning = false;
		m_bQuitting = true;
	}

	m_bFullscreen = fullscreen;

	glEnable(GL_BLEND);
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glViewport(0, 0, width, height);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1280, 0, 720, -1, 1); /* Always ensure output thinks it is 1280x720 (16:9) regardless of Res */
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glLoadIdentity();

	SDL_WM_SetCaption(title, NULL);

	/* Load Default Texture Items */
	pLogo.Load("/screen/textures/orblogo.png", debugLevel);
	tScrollTex[0].Load("/screen/textures/fader_blft.png", debugLevel);
	tScrollTex[1].Load("/screen/textures/fader_brght.png", debugLevel);
	weather[1].Load("/screen/textures/weather/gales.png", debugLevel);
	weather[2].Load("/screen/textures/weather/hot.png", debugLevel);
	weather[3].Load("/screen/textures/weather/cold.png", debugLevel);

	/* Load Fonts */
	for (int n = 0; n < 49; n++)
	{
		if (debugLevel > 1)
			printf("Loading Font fntCGothic[%i] - " "/screen/fonts/cgothic.ttf" " size %i... ", n, n);
		fntCGothic[n] = TTF_OpenFont("/screen/fonts/cgothic.ttf", n);
		if (fntCGothic[n] == NULL)
		{
			fprintf(stderr, "FAILED -  %s\n", SDL_GetError());
		}
		else
		{
			if (debugLevel > 1)
				printf("OK\n");
		}
	}

	sprintf(sHeader, "%s", header);
	sprintf(sWLoc, "%s", weatherloc);
	sprintf(sWCountry, "%s", weathercountry);
	sprintf(sWAPI, "%s", weatherapi);

	sprintf(sVersionString, "%s", versionstr);

	/* FPS Timer */
	counter = fps_counter();
	counter.set_cap(FPSLimit);
	counter.cap_on();

	/* Reset Debug Timer */
	if (debugLevel > 1)
	{
		cTime = time(0);
		dTimeEvent = cTime;
	}
}

void Signage::HandleEvents(Signage* signage)
{
	SDL_Event event;

	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			signage->Quit();
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				signage->Quit();
				break;

			case SDLK_d:
				/* Reset Debug Timer */
				dTimeEvent = cTime;
				break;
			}
			break;
		}
	}
}

void Signage::Update()
{
	/* Process and Time String */
	char daysInWord[8];
	char monthsInWord[8];
	char mins[8];

	cTime = time(0);
	ltm = localtime(&cTime);

	/* Only perform Update if Running */
	if (m_bQuitting == false)
	{
		/* Get string values for Day and Year */
		dayInStr(daysInWord, ltm->tm_wday);
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

			tC1 = ltm->tm_sec;
		}
		sprintf(dateString, "%i %s - %s, %s %i  %i", ltm->tm_hour, mins, daysInWord, monthsInWord, ltm->tm_mday, (1900 + ltm->tm_year));

		if (!iBoxes[100].isCreated() && iBoxes[100].stype() != -1)
			iBoxes[100].Create((char *)"Orbital Logo", (char *)"", 0, pLogo.gltex(), 2, (1280 / 2) - ((((pLogo.width() / 255.0) * 225.0) + 8) / 2), 10, pLogo.width(),
					pLogo.height(), pLogo.width(), pLogo.height(), 225, 1, 1, (char *)"null", false, false, (char *)"", (char *)"", debugLevel);
		else
			iBoxes[100].doUpdate();

		/* Process Weather Information */
		/* Do Weather Check (update once every 15 minutes) */
		if ((wLastCheckH != ltm->tm_hour) || (ltm->tm_min == 15) || (ltm->tm_min == 30) || (ltm->tm_min == 45) || (ltm->tm_min == 0))
		{
			if (wLastCheckM != ltm->tm_min)
			{
				/* Update last check interval */
				wLastCheckM = ltm->tm_min;

				if (debugLevel > 1)
				{
					if (ltm->tm_min < 10)
					{
						printf("Weather Update - %i:0%i\n", ltm->tm_hour, ltm->tm_min);
					}
					else
					{
						printf("Weather Update - %i:%i\n", ltm->tm_hour, ltm->tm_min);
					}
				}
				/* Hour is odd, we call check */
				tIcon = 0;
				tOIcon = 0;
				trObservationTime = 0;
				trTemp = 0;
				trWeatherCode = 0;
				trWindSpeedMPH = 0;
				trWindDir = 0;
				trHumidity = 0;
				trPressure = 0;

				wOK = false; /* New Check - default wOK to false (so data regarded dirty first) */
				xmlDoc *doc = NULL;
				xmlNode *root_element = NULL;

				//LIBXML_TEST_VERSION
				// Macro to check API for match with
				// the DLL we are using
				/*parse the file and get the DOM */

				/* We also need to ensure we remove any white spacing from the Country and Location variables, replacing with + */
				char tWString[512];
				string tsWCountry = string(sWCountry);
				replace(tsWCountry.begin(), tsWCountry.end(), ' ', '+');
				string tsWLoc = string(sWLoc);
				replace(tsWLoc.begin(), tsWLoc.end(), ' ', '+');

				sprintf(tWString, "http://free.worldweatheronline.com/feed/weather.ashx?q=%s,%s&format=xml&key=%s", tsWLoc.c_str(), tsWCountry.c_str(), sWAPI);
				if (debugLevel > 1)
					printf("Checking URL - %s\n", tWString);
				doc = xmlReadFile(tWString, NULL, 0);
				if (doc)
				{
					/*Get the root element node */
					root_element = xmlDocGetRootElement(doc);
					parseWeather(root_element);
					xmlFreeDoc(doc); // free document
					xmlCleanupParser(); // Free globals
					/* Update last check interval */
					if (wOK)
						wLastCheckH = ltm->tm_hour;
					else
						wLastCheckH = -5;
				}
				else
				{
					/* Update last check interval (we want to check in another minute) */
					if (debugLevel > 1)
						printf("NO DATA/NET CONNECTION\n");
					wLastCheckH = -5;
					wOK = false;
				}
				/* Calculate Weather */
				wCelcius = floorf(((5.0 / 9.0) * (tTemp - 32.0)) * 10 + 0.5) / 10;
				sprintf(wTemp, "%.1fºC", wCelcius);
			}
		}
		if (wOK)
		{
			/* Set the Weather Icon */
			tIcon = tWeatherCode;

			/* Do Looping Weather Animations */
			if (cTime > (wUpdateTimer[1] + 5)) /* Temperature Alert Flash */
			{
				if ((wCelcius <= 3.0) || (wCelcius >= 25.0))
				{
					wUpdateTimer[1] = cTime;

					if (!iBoxes[105].isCreated() && iBoxes[104].stype() != -1)
					{
						if (wCelcius <= 3.0)
							iBoxes[105].Create((char *)"Weather Temp Cold Alert Cycle", (char *)"", 0, weather[3].gltex(), 4, 0, 0, weather[3].width() / 2,
									weather[3].height() / 2, weather[3].width() / 2, weather[3].height() / 2, 255, 1, 1, (char *)"null", false, false, (char *)"", (char *)"",
									debugLevel);
						else
							iBoxes[105].Create((char *)"Weather Temp Hot Alert Cycle", (char *)"", 0, weather[2].gltex(), 4, 0, 0, weather[2].width() / 2,
									weather[2].height() / 2, weather[2].width() / 2, weather[2].height() / 2, 255, 1, 1, (char *)"null", false, false, (char *)"", (char *)"",
									debugLevel);

					}
					else if (iBoxes[105].isCreated() && iBoxes[104].stype() != -1)
						iBoxes[105].Destroy();
				}
				//else
				//	wFadeA[1] = 0;
			}

			if (cTime > (wUpdateTimer[0] + 15)) /* Weather Condition Cycle */
			{
				wUpdateTimer[0] = cTime;
				wFadeA[0] = 1;
			}

			/* Calculate Sunrise/Sunset */
			double tzone = 0.0;
			double d = FNday(ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday, 12);
			double lambda = FNsun(d);
			double obliq = 23.439 * rads - .0000004 * rads * d;
			double alpha = atan2(cos(obliq) * sin(lambda), cos(lambda));
			double delta = asin(sin(obliq) * sin(lambda));

			double LL = L - alpha;
			if (L < pi)
				LL += tpi;
			double equation = 1440.0 * (1.0 - LL / tpi);
			double latit = 52.8943;
			double longit = -2.2158;
			double ha = f0(latit, delta);

			double hb = f1(latit, delta);
			double twx = hb - ha; // length of twilight in radians
			twx = 12.0 * twx / pi; // length of twilight in degrees

			// Conversion of angle to hours and minutes //
			daylen = degs * ha / 7.5;
			if (daylen < 0.0001)
			{
				daylen = 0.0;
			}
			// arctic winter   //

			double riset = 12.0 - 12.0 * ha / pi + tzone - longit / 15.0 + equation / 60.0;
			double settm = 12.0 + 12.0 * ha / pi + tzone - longit / 15.0 + equation / 60.0;
			//double noont = riset + 12.0 * ha / pi;
			double altmax = 90.0 + delta * degs - latit;
			/* Express as degrees from the N horizon */

			if (delta * degs > latit)
				altmax = 90.0 + latit - delta * degs;

			if (riset > 24.0)
				riset -= 24.0;
			if (settm > 24.0)
				settm -= 24.0;
			int rhr, rmn, shr, smn;
			rhr = (int) riset;
			rmn = (riset - (double) rhr) * 60;
			shr = (int) settm;
			smn = (settm - (double) shr) * 60;

			char tBuff[64] = "";

			if (((rhr <= ltm->tm_hour) && (shr >= ltm->tm_hour)))
			{
				/* We are in Day */
				if (ltm->tm_hour == rhr)
				{
					/* We are at first hour */
					if (ltm->tm_min >= rmn)
					{
						if (tSrS != 1)
						{
							/* Set Day */
							tSrS = 1;
							tOIcon = -1;
						}
					}
					else
					{
						if (tSrS != 2)
						{
							/* Set Night */
							tSrS = 2;
							tOIcon = -1;
						}
					}
				}
				else if (ltm->tm_hour == shr)
				{
					/* We are at last hour */
					if (ltm->tm_min < smn)
					{
						if (tSrS != 1)
						{
							/* Set Day */
							tSrS = 1;
							tOIcon = -1;
						}
					}
					else
					{
						if (tSrS != 2)
						{
							/* Set Night */
							tSrS = 2;
							tOIcon = -1;
						}
					}
				}
			}
			else
			{
				/* Obvious Night */
				if (tSrS != 2)
				{
					/* Set Night */
					tSrS = 2;
					tOIcon = -1;
				}
			}

			if (tSrS == 2)
				sprintf(tBuff, "/screen/textures/weather/night");
			else
				sprintf(tBuff, "/screen/textures/weather/day");

			/* Weather Icon Fading Transition */
			if ((tIcon != tOIcon) && (wFadeA[1] == 0))
			{
				wUpdateTimer[1] = cTime;
				wFadeA[1] = 1;
			}
			if ((wFadeA[1] == 1) || (wFadeA[1] == 2))
			{
				switch (wFadeA[1])
				{
				case 1:
					wFadeV[1] = wFadeV[1] - 15;
					;
					break;
				case 2:
					wFadeV[1] = wFadeV[1] + 15;
					;
					break;
				}
				if (wFadeV[1] < 0)
				{
					wFadeA[1] = 2;
					wFadeV[1] = 0;
					sprintf(tBuff, "%s/%i.png", tBuff, tIcon);

					weather[0].Load(tBuff, debugLevel);
					iBoxes[101].Create((char *)"Weather Condition", (char *)"", 0, weather[0].gltex(), 4, 0, 0, weather[0].width() / 2, weather[0].height() / 2,
							weather[0].width() / 2, weather[0].height() / 2, 255, 1, 1, (char *)"null", false, false, (char *)"", (char *)"", debugLevel);
				}
				if (wFadeV[1] > 255)
				{
					wFadeV[1] = 255;
					wFadeA[1] = 0;
					tOIcon = tIcon;
				}
			}

			/* Process Fading Weather Info */
			if ((wFadeA[0] == 1) || (wFadeA[0] == 2))
			{
				switch (wFadeA[0])
				{
				case 1:
					wFadeV[0] = wFadeV[0] - 15;
					;
					break;
				case 2:
					wFadeV[0] = wFadeV[0] + 15;
					;
					break;
				}
				if (wFadeV[0] < 0)
				{
					wFadeA[0] = 2;
					wFadeV[0] = 0;
					wCurDisp++;
					tScrolling[0] = 0;
					tScrollCycle[0] = 0;
					tScrollSFader[0] = 0;
					tScrollEFader[0] = 0;
					tScrollingTimer[0] = cTime;
					if (iBoxes[102].isCreated() && iBoxes[102].stype() != -1)
						iBoxes[102].Destroy();
					if (iBoxes[103].isCreated() && iBoxes[103].stype() != -1)
						iBoxes[103].Destroy();
					if (wCurDisp >= 6)
						wCurDisp = 0;
				}
				if (wFadeV[0] > 255)
				{
					wFadeV[0] = 255;
					wFadeA[0] = 0;
				}
			}
		}

		if (cTime > (wUpdateTimer[2] + 10))
		{
			/* Start Board Checking Loop */

			/* Check Board Configurations */
			if (debugLevel > 1)
				printf("Board Check - %i:%i:%i\n", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
			wUpdateTimer[2] = cTime;

			DIR *d;
			vector<string> dList;
			struct dirent *dir;
			d = opendir("/screen/boards/");
			if (d)
			{
				/* Boards Folder Exists, check contents */
				while ((dir = readdir(d)) != NULL)
				{
					if ((strcmp(".", dir->d_name) != 0) && (strcmp("..", dir->d_name) != 0))
					{
						dList.push_back(dir->d_name);
					}
				}

				closedir(d);
				sort(dList.begin(), dList.end()); /* Sort Array */
				if (debugLevel > 1)
					printf("Found %d Folders to check...\n", (int)dList.size());
				for (int dS = 0; dS < dList.size(); dS++)
				{
					char tFName[128];
					sprintf(tFName, "/screen/boards/%s/config.ini", dList[dS].c_str());
					if (FileExists(tFName))
					{
						/* Found a config file, parse it... */
						CIniFile ini;
						ini.Load(tFName);
						CIniSection* pSection = ini.GetSection("BoardSettings");
						if (pSection)
						{
							if ((pSection->GetKey("Enabled"))
									&& ((pSection->GetKey("UID")) && (pSection->GetKey("UIDS")) && (pSection->GetKey("UIDI")) && (pSection->GetKey("PosX"))
											&& (pSection->GetKey("PosY")) && (pSection->GetKey("Scale")) && (pSection->GetKey("Border"))
											&& (pSection->GetKey("Width")) && (pSection->GetKey("Height")) && (pSection->GetKey("Boards"))
											&& (pSection->GetKey("Alert")) && (pSection->GetKey("TimeStamp"))))
							{
								/* We have a Board which we can use.  Let's check against current mBoard members first, see if
								 * it exists. */
								bool tBFound = false;
								for (int cB = 0; cB < 64; cB++)
								{
									if ((atoi(ini.GetKeyValue("BoardSettings", "Enabled").c_str()) == 1)
											&& (strcmp(ini.GetKeyValue("BoardSettings", "UID").c_str(), mBoard[cB].UID) == 0))
									{
										/* We found matching UID
										 *
										 * We now need to check TimeStamp, see if we are matching or not, so we can decide if
										 * board information is changing. */
										tBFound = true;
										if (debugLevel > 1)
											printf("\n\tFound Existing board at ID '%i'. Checking to see if it's changed...\n", cB);
										if (mBoard[cB].TimeStampCFG != atoi(ini.GetKeyValue("BoardSettings", "TimeStamp").c_str()))
										{
											if (debugLevel > 1)
												printf("\t\tNew TimeStamp Detected - Marking as Dirty...\n");
											mBoard[cB].TimeStampCheck = 0;
											mBoard[cB].isDestroying = true;
										}
										else
										{
											if (debugLevel > 1)
												printf("\t\tConfiguration is unchanged.\n\t\tBoard ID - %i\n", mBoard[cB].CreatedID);
											/* Reset TimeStampCheck */
											mBoard[cB].TimeStampCheck = cTime;
										}

										/* Break out of loop */
										break;
									}
								}
								if ((atoi(ini.GetKeyValue("BoardSettings", "Enabled").c_str()) == 1) && (tBFound == false))
								{
									/* No board with matching UID was found, add.
									 *
									 * This is also performed when a board TimeStamp changes, as we destroy the old board before
									 * creating the new one, so to ensure no dirty variables are left. */
									for (int cB = 0; cB < 64; cB++)
									{
										if (strcmp("", mBoard[cB].UID) == 0)
										{
											/* Construct Main Boards */
											sprintf(mBoard[cB].UID, "%s", ini.GetKeyValue("BoardSettings", "UID").c_str());
											sprintf(mBoard[cB].UIDS, "%s", ini.GetKeyValue("BoardSettings", "UIDS").c_str());
											mBoard[cB].UIDI = atoi(ini.GetKeyValue("BoardSettings", "UIDI").c_str());
											mBoard[cB].TimeStampCFG = atoi(ini.GetKeyValue("BoardSettings", "TimeStamp").c_str());
											mBoard[cB].TimeStampCheck = cTime;
											mBoard[cB].curBoard = -1;
											mBoard[cB].reqBoard = 0;
											sprintf(mBoard[cB].Folder, "%s", dList[dS].c_str());
											mBoard[cB].X = atoi(ini.GetKeyValue("BoardSettings", "PosX").c_str());
											mBoard[cB].Y = atoi(ini.GetKeyValue("BoardSettings", "PosY").c_str());
											mBoard[cB].Scale = atoi(ini.GetKeyValue("BoardSettings", "Scale").c_str());
											mBoard[cB].Border = atoi(ini.GetKeyValue("BoardSettings", "Border").c_str());
											mBoard[cB].Width = atoi(ini.GetKeyValue("BoardSettings", "Width").c_str());
											mBoard[cB].Height = atoi(ini.GetKeyValue("BoardSettings", "Height").c_str());
											mBoard[cB].nBoards = atoi(ini.GetKeyValue("BoardSettings", "Boards").c_str());
											mBoard[cB].Alert = atoi(ini.GetKeyValue("BoardSettings", "Alert").c_str());
											sprintf(mBoard[cB].Header, "%s", ini.GetKeyValue("BoardSettings", "Header").c_str());
											mBoard[cB].HeaderSize = atoi(ini.GetKeyValue("BoardSettings", "HeaderSize").c_str());
											if (atoi(ini.GetKeyValue("BoardSettings", "HeaderEn").c_str()) == 1)
												mBoard[cB].HeaderVis = true;
											else
												mBoard[cB].HeaderVis = false;
											sprintf(mBoard[cB].sPluginCMD, "%s", ini.GetKeyValue("BoardSettings", "Plugin").c_str());
											mBoard[cB].isDestroying = false;

											/* Construct Child Boards */
											int tcBoardCount = 0;
											for (int brdC = 0; brdC < mBoard[cB].nBoards; brdC++)
											{
												sprintf(mBoard[cB].cBoard[tcBoardCount].Section, "Board-%i", brdC + 1);
												pSection = ini.GetSection(mBoard[cB].cBoard[tcBoardCount].Section);
												if (pSection)
												{
													/* Now get the Data */
													if ((pSection->GetKey("Type")) && (pSection->GetKey("Src")) && (pSection->GetKey("Duration")))
													{
														sprintf(mBoard[cB].cBoard[tcBoardCount].Type, "%s",
																ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Type").c_str());
														sprintf(mBoard[cB].cBoard[tcBoardCount].Src, "%s",
																ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Src").c_str());
														mBoard[cB].cBoard[tcBoardCount].Duration = atoi(
																ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Duration").c_str());
														mBoard[cB].cBoard[tcBoardCount].Quality = atoi(
																ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Quality").c_str());
														mBoard[cB].cBoard[tcBoardCount].ScrollSpeed = atoi(
																ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "ScrollSpeed").c_str());
														sprintf(mBoard[cB].cBoard[tcBoardCount].Header, "%s",
																ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Header").c_str());
														mBoard[cB].cBoard[tcBoardCount].HeaderSize = atoi(
																ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "HeaderSize").c_str());
														if (atoi(ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "HeaderEn").c_str()) == 1)
															mBoard[cB].cBoard[tcBoardCount].HeaderVis = true;
														else
															mBoard[cB].cBoard[tcBoardCount].HeaderVis = false;
														sprintf(mBoard[cB].cBoard[tcBoardCount].AudioOut, "%s",
																ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "EnableAudio").c_str());
														mBoard[cB].cBoard[tcBoardCount].isAlert = false;
														tcBoardCount++;
													}
												}
											}

											/* Get Alert Board Information */
											sprintf(mBoard[cB].cBoard[tcBoardCount].Section, "Alert");
											pSection = ini.GetSection(mBoard[cB].cBoard[tcBoardCount].Section);
											if (pSection)
											{
												/* Now get the Data */
												if ((pSection->GetKey("Type")) && (pSection->GetKey("Src")) && (pSection->GetKey("Duration")))
												{
													sprintf(mBoard[cB].cBoard[tcBoardCount].Type, "%s",
															ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Type").c_str());
													sprintf(mBoard[cB].cBoard[tcBoardCount].Src, "%s",
															ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Src").c_str());
													mBoard[cB].cBoard[tcBoardCount].Duration = atoi(
															ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Duration").c_str());
													mBoard[cB].cBoard[tcBoardCount].Quality = atoi(
															ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Quality").c_str());
													mBoard[cB].cBoard[tcBoardCount].ScrollSpeed = atoi(
															ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "ScrollSpeed").c_str());
													sprintf(mBoard[cB].cBoard[tcBoardCount].Header, "%s",
															ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "Header").c_str());
													mBoard[cB].cBoard[tcBoardCount].HeaderSize = atoi(
															ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "HeaderSize").c_str());
													if (atoi(ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "HeaderEn").c_str()) == 1)
														mBoard[cB].cBoard[tcBoardCount].HeaderVis = true;
													else
														mBoard[cB].cBoard[tcBoardCount].HeaderVis = false;
													sprintf(mBoard[cB].cBoard[tcBoardCount].AudioOut, "%s",
															ini.GetKeyValue(mBoard[cB].cBoard[tcBoardCount].Section, "EnableAudio").c_str());
													mBoard[cB].cBoard[tcBoardCount].isAlert = true;
													tcBoardCount++;
												}
											}
											if (debugLevel > 1)
											{
												printf("\nBoard '%s':\n\tUIDS:\t'%s'\n\tUID:\t'%s'\n\tUIDI:\t%i\n\tBoards:\t%i\n", dList[dS].c_str(),
														mBoard[cB].UIDS, mBoard[cB].UID, mBoard[cB].UIDI, mBoard[cB].nBoards);
												for (int brdO = 0; brdO < mBoard[cB].nBoards; brdO++)
												{
													printf("\t\tChild Board %i (%i,%i)\n\t\t\tType\t\t%s\n\t\t\tSrc\t\t%s\n\t\t\tDuration\t%i\n", brdO + 1, cB,
															brdO, mBoard[cB].cBoard[brdO].Type, mBoard[cB].cBoard[brdO].Src, mBoard[cB].cBoard[brdO].Duration);
												}
												if (mBoard[cB].cBoard[mBoard[cB].nBoards].isAlert == true)
												{
													printf("\t\tAlert Board %i (%i,%i)\n\t\t\tType\t\t%s\n\t\t\tSrc\t\t%s\n\t\t\tDuration\t%i\n",
															mBoard[cB].nBoards, cB, mBoard[cB].nBoards, mBoard[cB].cBoard[mBoard[cB].nBoards].Type,
															mBoard[cB].cBoard[mBoard[cB].nBoards].Src, mBoard[cB].cBoard[mBoard[cB].nBoards].Duration);
												}
											}
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				if (debugLevel > 1)
					printf("**Error Parsing Boards - Does the folder exist?\n");
			}
			/* End Board Checking Loop */
		}

		if (cTime > (wUpdateTimer[3] + 1))
		{
			/* Sort out Board Creation/Destruction methods. */
			wUpdateTimer[3] = cTime;
			for (int cB = 0; cB < 64; cB++)
			{
				bool bWipeNeed = false;
				if (mBoard[cB].TimeStampCheck >= (cTime - 15))
				{
					/* We've got something... Check it */
					if (mBoard[cB].CreatedID == 0)
					{
						/* Board needs creating */
						for (int cBC = 1; cBC < 65; cBC++) /* Intentionally do 1 to 65 */
						{
							/* We also want sanity checking, incase we double up on board ID's (shouldn't happen) */
							if (!iBoxes[cBC].isCreated())
							{
								/* Empty ID Found - Create*/
								mBoard[cB].pFade = 0;
								mBoard[cB].Scroll = 0;
								bool mBoardMod = false;
								if (mBoard[cB].curBoard == -1)
								{
									mBoard[cB].curBoard = 0;
									mBoardMod = true;
								}

								/* Run Plugin (Initial) */
								if (strlen(mBoard[cB].sPluginCMD) != 0)
								{
									/* Clean up Any-and-All Open Plugins */
									if (mBoard[cB].fPluginCMD != NULL)
									{
										pclose(mBoard[cB].fPluginCMD);
										mBoard[cB].fPluginCMD = NULL;
									}
									mBoard[cB].fPluginCMD = popen(mBoard[cB].sPluginCMD, "r");
								}

								/* Create Board (Initial) */
								int tBoardType;
								if (strcmp(mBoard[cB].cBoard[0].Type, "image") == 0)
								{
									tBoardType = 1;
								}

								if (strcmp(mBoard[cB].cBoard[0].Type, "mplayer") == 0)
								{
									tBoardType = 3;
								}

								if (strcmp(mBoard[cB].cBoard[0].Type, "iplayer") == 0)
								{
									tBoardType = mBoard[cB].cBoard[mBoard[cB].curBoard].Quality;
								}

								if (debugLevel > 1)
									printf("Creating Board '%s' with BoardID (%i,%i) - Type %i...\n", mBoard[cB].UIDS, cBC, cB, tBoardType);
								iBoxes[cBC].Create(mBoard[cB].UID, mBoard[cB].cBoard[mBoard[cB].curBoard].Src, mBoard[cB].TimeStampCFG, 0, mBoard[cB].Border,
										mBoard[cB].X, mBoard[cB].Y, mBoard[cB].Width, mBoard[cB].Height, sWidth, sHeight, mBoard[cB].Scale, tBoardType,
										mBoard[cB].curBoard, mBoard[cB].cBoard[mBoard[cB].curBoard].AudioOut, mBoard[cB].cBoard[mBoard[cB].curBoard].HeaderVis,
										mBoard[cB].HeaderVis, mBoard[cB].cBoard[mBoard[cB].curBoard].Header, mBoard[cB].Header, debugLevel);
								mBoard[cB].CreatedID = cBC;
								if (mBoardMod == true)
									mBoard[cB].curBoard = -1;
								break;
							}
						}
					}
				}
				else
				{
					/* Something or nothing... Destroy/Clean Event */
					/* We've got something... Check it */
					if (mBoard[cB].CreatedID != 0)
					{
						/* Something Exists which needs destroying */
						if (!iBoxes[mBoard[cB].CreatedID].isCreated())
						{
							/* Destroy board information (if not already flagged) */
							bWipeNeed = true;
						}
						else
						{
							/* Board is created, we need to destroy it */
							if (iBoxes[mBoard[cB].CreatedID].stype() != -1)
								iBoxes[mBoard[cB].CreatedID].Destroy();
						}
					}
					else
					{
						if (strcmp("", mBoard[cB].UID) != 0)
							bWipeNeed = true;
					}
				}
				if (bWipeNeed)
				{
					if (debugLevel > 1)
						printf("\tWiping Board ID '%i' (%s)...", cB, mBoard[cB].UIDS);
					/* Clear Variables */
					sprintf(mBoard[cB].UID, "");
					sprintf(mBoard[cB].UIDS, "");
					mBoard[cB].UIDI = 0;
					sprintf(mBoard[cB].Folder, "");
					mBoard[cB].curBoard = 0;
					mBoard[cB].reqBoard = 0;
					mBoard[cB].oldBoard = 0;
					mBoard[cB].X = 0;
					mBoard[cB].Y = 0;
					mBoard[cB].Scale = 0;
					mBoard[cB].Border = 0;
					mBoard[cB].Width = 0;
					mBoard[cB].Height = 0;
					mBoard[cB].Scroll = 0;
					mBoard[cB].ScrollComplete = 0;
					mBoard[cB].pFade = 0;
					mBoard[cB].nBoards = 0;
					mBoard[cB].Alert = 0;
					mBoard[cB].AlertActive = 0;
					sprintf(mBoard[cB].Header, "");
					mBoard[cB].HeaderSize = 0;
					mBoard[cB].HeaderVis = false;
					/* Clean up Any-and-All Open Plugins */
					if (mBoard[cB].fPluginCMD != NULL)
					{
						pclose(mBoard[cB].fPluginCMD);
						mBoard[cB].fPluginCMD = NULL;
					}
					sprintf(mBoard[cB].sPluginCMD, "");
					mBoard[cB].rPluginCMD = false;
					mBoard[cB].TimeStampCFG = 0;
					mBoard[cB].TimeStampCheck = 0;
					mBoard[cB].isDestroying = false;
					mBoard[cB].CreatedID = 0;
					for (int bClean = 0; bClean < 64; bClean++)
					{
						sprintf(mBoard[cB].cBoard[bClean].Section, "");
						mBoard[cB].cBoard[bClean].Duration = 0;
						sprintf(mBoard[cB].cBoard[bClean].Type, "");
						sprintf(mBoard[cB].cBoard[bClean].Src, "");
						mBoard[cB].cBoard[bClean].Quality = 0;
						mBoard[cB].cBoard[bClean].ScrollSpeed = 0;
						sprintf(mBoard[cB].cBoard[bClean].Header, "");
						mBoard[cB].cBoard[bClean].HeaderSize = 0;
						mBoard[cB].cBoard[bClean].HeaderVis = false;
						sprintf(mBoard[cB].cBoard[bClean].AudioOut, "");
						mBoard[cB].cBoard[bClean].isAlert = false;
					}
					if (debugLevel > 1)
						printf(" [OK]\n");
				}
			}
		}

		/* Do Board Timer Events - Only process boards where we are valid */
		for (int cB = 0; cB < 64; cB++)
		{
			if ((iBoxes[mBoard[cB].CreatedID].isCreated()) && (iBoxes[mBoard[cB].CreatedID].stype() != -1))
			{
				/* Board is created, and we are not in a destructor mode.  Process */
				bool resetClicks = false;
				bool scrUpdate = false;

				/* Check and run Plugin */
				if (((cTime > (iBoxes[mBoard[cB].CreatedID].getClicks() + (mBoard[cB].cBoard[iBoxes[mBoard[cB].CreatedID].getScreen()].Duration - 6))))
						&& ((strlen(mBoard[cB].sPluginCMD) != 0) && (mBoard[cB].rPluginCMD == false)))
				{
					/* Run Plugin Command */
					if (debugLevel > 1)
						printf("Plugin Timer Event Check (%i) - Board %i, Screen %i of %i (%i seconds) - FLAG #%ix%ix%i\n", (int)cTime, cB,
								iBoxes[mBoard[cB].CreatedID].getScreen(), mBoard[cB].nBoards,
								mBoard[cB].cBoard[iBoxes[mBoard[cB].CreatedID].getScreen()].Duration, iBoxes[mBoard[cB].CreatedID].stype(), mBoard[cB].pFade,
								mBoard[cB].Scroll);
					if (debugLevel > 1)
						printf("\tCommand:\t%s\n", mBoard[cB].sPluginCMD);
					mBoard[cB].rPluginCMD = true;
					/* Clean up Any-and-All Open Plugins */
					if (mBoard[cB].fPluginCMD != NULL)
					{
						pclose(mBoard[cB].fPluginCMD);
						mBoard[cB].fPluginCMD = NULL;
					}
					mBoard[cB].fPluginCMD = popen(mBoard[cB].sPluginCMD, "r");
				}

				if ((cTime > (iBoxes[mBoard[cB].CreatedID].getClicks() + mBoard[cB].cBoard[iBoxes[mBoard[cB].CreatedID].getScreen()].Duration)))
				{
					if ((mBoard[cB].reqBoard == mBoard[cB].curBoard) && (mBoard[cB].pFade == 255))
					{
						if ((mBoard[cB].Scroll == 0)
								|| ((mBoard[cB].Scroll / mBoard[cB].cBoard[iBoxes[mBoard[cB].CreatedID].getScreen()].ScrollSpeed)
										== (mBoard[cB].Scale * (mBoardTex[iBoxes[mBoard[cB].CreatedID].getScreen()].height() / 255))
												- (mBoard[cB].Scale * (mBoard[cB].Height / 255))))
							if (debugLevel > 1)
								printf("Board Timer Event Check (%i - Board %i) - Screen %i of %i (%i Seconds) - FLAG #%ix%ix%i\n", (int)cTime, cB,
										iBoxes[mBoard[cB].CreatedID].getScreen(), mBoard[cB].nBoards,
										mBoard[cB].cBoard[iBoxes[mBoard[cB].CreatedID].getScreen()].Duration, iBoxes[mBoard[cB].CreatedID].stype(),
										mBoard[cB].pFade, mBoard[cB].Scroll);

						/* Scrolling Event */
						if ((mBoardTex[cB].height() > mBoard[cB].Height) && (mBoard[cB].pFade == 255.0))
						{
							/* We need to scroll! */
							if (((mBoard[cB].Scroll / mBoard[cB].cBoard[iBoxes[mBoard[cB].CreatedID].getScreen()].ScrollSpeed)
									< (mBoard[cB].Scale * (mBoardTex[cB].height() / 255.0)) - (mBoard[cB].Scale * (mBoard[cB].Height / 255.0))))
							{
								mBoard[cB].Scroll++;
								scrUpdate = true;
							}
							if (((mBoard[cB].Scroll / mBoard[cB].cBoard[iBoxes[mBoard[cB].CreatedID].getScreen()].ScrollSpeed)
									>= (mBoard[cB].Scale * (mBoardTex[cB].height() / 255.0)) - (mBoard[cB].Scale * (mBoard[cB].Height / 255.0)))
									&& (mBoard[cB].ScrollComplete == 0))
							{
								/* Instead of eventually hitting an upper limit on the scroll
								 we loop the scroll value to keep things clean */
								iBoxes[mBoard[cB].CreatedID].setClicks(cTime);
								mBoard[cB].rPluginCMD = false;
								scrUpdate = true;
								mBoard[cB].ScrollComplete = 1;
							}
							iBoxes[mBoard[cB].CreatedID].setScrollV(
									mBoard[cB].Scroll / mBoard[cB].cBoard[iBoxes[mBoard[cB].CreatedID].getScreen()].ScrollSpeed);
						}

						if ((scrUpdate == false) && (iBoxes[mBoard[cB].CreatedID].stype() != -1))
						{
							/* Destroy broken Streaming Events */
							if ((iBoxes[mBoard[cB].CreatedID].stype() != 1) && (iBoxes[mBoard[cB].CreatedID].stype() == 2))
							{
								/* Problem with Media Streaming - Send a Kill Flag */
								if (debugLevel > 1)
									printf("Destroying ID %i\n", iBoxes[mBoard[cB].CreatedID].stype());
								/* Destroy iPlayer/Media Reference */
								iBoxes[mBoard[cB].CreatedID].Destroy();
								/* Make Board Dirty so it is properly destroyed. */
								mBoard[cB].TimeStampCFG = 0;
								mBoard[cB].TimeStampCheck = 0;
							}
							else
							{
								if (mBoard[cB].Alert == 1)
								{
									/* Alert Board Configured */
									if (mBoard[cB].AlertActive == 1)
									{
										/* Alert Board currently showing - Turn Off and resume as normal */
										mBoard[cB].AlertActive = 0;
										mBoard[cB].reqBoard = mBoard[cB].oldBoard + 1;
										if (mBoard[cB].reqBoard > mBoard[cB].nBoards - 1)
											mBoard[cB].reqBoard = 0;
									}
									else
									{
										mBoard[cB].oldBoard = mBoard[cB].reqBoard;
										mBoard[cB].reqBoard = mBoard[cB].nBoards;
										mBoard[cB].AlertActive = 1;
									}
								}
								else
								{
									/* No Alerts Showing - Run as Normal */
									mBoard[cB].reqBoard++;
									if (mBoard[cB].reqBoard > (mBoard[cB].nBoards - 1))
										mBoard[cB].reqBoard = 0;
								}
							}
						}
					}

					/* Fade Out Event */
					if (mBoard[cB].reqBoard != mBoard[cB].curBoard)
					{
						mBoard[cB].pFade = mBoard[cB].pFade - 5;
						if (mBoard[cB].pFade < 0)
							mBoard[cB].pFade = 0;
					}

					/* Swap Texture Event */
					if (((mBoard[cB].reqBoard != mBoard[cB].curBoard) && (mBoard[cB].pFade == 0)) && (iBoxes[mBoard[cB].CreatedID].stype() != -1))
					{
						mBoard[cB].curBoard = mBoard[cB].reqBoard;
						char bdID[128];
						if ((mBoard[cB].AlertActive == 1) && (mBoard[cB].Alert == 1))
						{
							if (strcmp(mBoard[cB].cBoard[mBoard[cB].curBoard].Type, "iplayer") == 0)
								sprintf(bdID, "/screen/textures/iplayer/%s.png", mBoard[cB].cBoard[mBoard[cB].nBoards].Src);
							else
								sprintf(bdID, "/screen/boards/%s/alert/%s", mBoard[cB].Folder, mBoard[cB].cBoard[mBoard[cB].nBoards].Src);
						}
						else
						{
							if (strcmp(mBoard[cB].cBoard[mBoard[cB].curBoard].Type, "iplayer") == 0)
								sprintf(bdID, "/screen/textures/iplayer/%s.png", mBoard[cB].cBoard[mBoard[cB].curBoard].Src);
							else
								sprintf(bdID, "/screen/boards/%s/boards/%s", mBoard[cB].Folder, mBoard[cB].cBoard[mBoard[cB].curBoard].Src);
						}

						iBoxes[mBoard[cB].CreatedID].setScreen(mBoard[cB].curBoard);
						if (debugLevel > 1)
							printf("Attempting to load Texture '%s'...", bdID);
						if (FileExists(bdID) == false)
							sprintf(bdID, "/screen/textures/iplayer/generic_fail.png");
						mBoardTex[cB].Destroy();
						mBoardTex[cB].Load(bdID, debugLevel);
						/* Scale Texture to Box unless we scroll */
						int texH = 0;
						if (mBoardTex[cB].height() < mBoard[cB].Height)
							texH = mBoard[cB].Height;
						else
							texH = mBoardTex[cB].height();
						iBoxes[mBoard[cB].CreatedID].SwapTex(mBoardTex[cB].gltex(), mBoardTex[cB].width(), texH);
						mBoard[cB].Scroll = 0;
						mBoard[cB].ScrollComplete = 0;
						iBoxes[mBoard[cB].CreatedID].setScrollV(mBoard[cB].Scroll);
						iBoxes[mBoard[cB].CreatedID].setClicks(0); /* Set to 0 to take into account of different timings per page */
					}

					/* Fade In Event */
					if ((mBoard[cB].reqBoard == mBoard[cB].curBoard) && (iBoxes[mBoard[cB].CreatedID].stype() != -1))
					{
						mBoard[cB].pFade = mBoard[cB].pFade + 5;
						if (mBoard[cB].pFade > 255)
						{
							mBoard[cB].pFade = 255;
						}
						if (mBoard[cB].pFade == 255)
							resetClicks = true;
					}

					/* Reset Clicks */
					if ((resetClicks == true) && (scrUpdate == false))
					{
						iBoxes[mBoard[cB].CreatedID].setClicks(cTime);
						mBoard[cB].rPluginCMD = false;
					}
				}
			}
		}
	}

	/* Save Screenshot every 29 seconds */
	if (wUpdateTimer[4] == 0)
		wUpdateTimer[4] = cTime;

	if (cTime >= wUpdateTimer[4] + 29)
	{
		if (debugLevel > 1)
			printf("Generating Screendump...");
		wUpdateTimer[4] = cTime;
		SDL_Surface * scrimage = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);

		glReadBuffer(GL_FRONT);

		GLubyte *pixels = new GLubyte[screen->w * screen->h * 3];
		if (pixels == NULL)
		{
			SDL_FreeSurface(scrimage);
			return;
		}

		// Read OpenGL frame buffer
		glReadPixels(0, 0, screen->w, screen->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		uint8_t *imagepixels = reinterpret_cast<uint8_t*>(scrimage->pixels);

		// Copy the "reversed_image" memory to the "image" memory
		for (int y = (screen->h - 1); y >= 0; --y) {
			uint8_t *row_begin = pixels + y * screen->w * 3;
			uint8_t *row_end = row_begin + screen->w * 3;

			std::copy(row_begin, row_end, imagepixels);

			// Advance a row in the output surface.
			imagepixels += scrimage->pitch;
		}

		/* Free pixels */
		free (pixels);

		SDL_SaveBMP(scrimage, "/screen/SCRDUMP.bmp");
		SDL_FreeSurface(scrimage);
		if (debugLevel > 1)
			printf("[OK] (%is)\n", (int)cTime - wUpdateTimer[4]);
	}
}

void Signage::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	/* Global for Weather */
	int tPXX;

	/* Draw Title */
	drawText(sHeader, fntCGothic[40], 3, 255, 255, 255, 255, 0, 1280, 680, 0, 0);

	/* Draw Time */
	drawText(dateString, fntCGothic[32], 2, 255, 255, 255, 255, 0, 1262, 8, 0, 0);

	if (bV1)
	{
		if (ltm->tm_hour > 9)
			drawText(":", fntCGothic[32], 2, 255, 255, 255, 255, 0, 1262 - pTWidth + 44, 11, 0, 0);
		else
			drawText(":", fntCGothic[32], 2, 255, 255, 255, 255, 0, 1262 - pTWidth + 26, 11, 0, 0);
	}

	drawText(nthsInWord, fntCGothic[12], 1, 255, 255, 255, 255, 0, 1172, 32, 0, 0);

	/* Draw Weather */
	if (wOK)
	{
		pTWidth = 0;

		if (wCelcius <= 3.0)
			drawText(wTemp, fntCGothic[32], 1, 128, 128, 255, 255, 0, 16, 8, 0, 0);
		else if (wCelcius >= 25.0)
			drawText(wTemp, fntCGothic[32], 1, 255, 128, 128, 255, 0, 16, 8, 0, 0);
		else
			drawText(wTemp, fntCGothic[32], 1, 255, 255, 255, 255, 0, 16, 8, 0, 0);

		/* Still check Icon position and move if required.  Temp could change, but if Icon remains same, we need to refresh */
		if (iBoxes[101].isCreated())
			iBoxes[101].rePos(16 + pTWidth, -6);
		if (iBoxes[104].isCreated())
			iBoxes[104].rePos(16 + pTWidth, -6);
		if (iBoxes[105].isCreated())
			iBoxes[105].rePos(16 + pTWidth, -6);

		char tWString[64];
		switch (wCurDisp)
		{
		case 0:
			// Weather Condition //
			sprintf(tWString, "%s", tConditionDesc);
			;
			break;
		case 1:
			// Wind Speed //
			sprintf(tWString, "Wind: %s at %imph", tWindDir, tWindSpeedMPH);
			break;
		case 2:
			// Humidity //
			sprintf(tWString, "Humidity: %i%%", tHumidity);
			;
			break;
		case 3:
			// Air Pressure //
			sprintf(tWString, "Air Pressure: %i mb", tPressure);
			;
			break;
		case 4:
			// Cloud Cover //
			sprintf(tWString, "Cloud Cover: %i%%", tCloudCover);
			;
			break;
		case 5:
			// Visibility //
			sprintf(tWString, "Visibility: %ikm", tVisibility);
			;
			break;
		}

		int oPX = ((iBoxes[101].width() / 255.0) * iBoxes[101].scale()) + pTWidth + 32;
		int plPX = (1280 / 2) - ((((pLogo.width() / 255.0) * 200.0) + 8) / 2);

		tPXX = ((iBoxes[101].width() / 255.0) * iBoxes[101].scale()) + pTWidth + 16;
		int tPXM = plPX - oPX;

		if (drawText(tWString, fntCGothic[32], 1, 255, 255, 255, wFadeV[0], 0, tPXX, 8, tPXM, tScrolling[0]) > 0)
		{
			/* We're either Scrolling or Scrolled. */
			/* Draw End Fader (obviously) */

			if (!iBoxes[102].isCreated() && iBoxes[102].stype() != -1)
			{
				iBoxes[102].Create((char *)"Weather Condition Fader - Right", (char *)"", 0, tScrollTex[0].gltex(), 4, plPX - 32, 8, tScrollTex[0].width(), pTHeight,
						tScrollTex[0].width(), pTHeight, 255, 1, 1, (char *)"null", false, false, (char *)"", (char *)"", debugLevel);
			}

			tScrollSFader[0] = 0;
			tScrollEFader[0] = 0;
			if (wFadeV[0] >= 255)
			{
				wUpdateTimer[0] = cTime - 10;
				if (cTime > tScrollingTimer[0] + 5)
				{
					if (!iBoxes[103].isCreated() && iBoxes[103].stype() != -1)
					{
						iBoxes[103].Create((char *)"Weather Condition Fader - Left", (char *)"", 0, tScrollTex[1].gltex(), 4, tPXX, 8, tScrollTex[1].width(), 64,
								tScrollTex[1].width(), 64, 255, 1, 1, (char *)"null", false, false, (char *)"", (char *)"", debugLevel);
					}
					else
					{
						iBoxes[102].doUpdate();

						if (tScrollCycle[0] == 1)
						{
							tScrolling[0]++;
							tScrollCycle[0] = 0;
						}
						else
							tScrollCycle[0] = 1;
					}
				}
			}
		}
		else
		{
			/* No Scrolling/Scrolled Text Complete. */
			if (tScrolling[0] > 0)
			{
				/* We did some Scrolling, we need to fade out ending fader */
				if (iBoxes[102].isCreated() && iBoxes[102].stype() != -1)
					iBoxes[102].Destroy();
			}
			tScrollingTimer[0] = cTime;
		}

		/* Draw Leaves if Windy */
		if (tWindSpeedMPH >= 25)
		{
			if (!iBoxes[104].isCreated() && iBoxes[104].stype() != -1)
			{
				iBoxes[104].Create((char *)"Weather Wind Alert", (char *)"", 0, weather[1].gltex(), 4, 0, 0, weather[1].width() / 2, weather[1].height() / 2,
						weather[1].width() / 2, weather[1].height() / 2, 255, 1, 1, (char *)"null", false, false, (char *)"", (char *)"", debugLevel);
			}
		}
		else
		{
			if (iBoxes[104].isCreated() && iBoxes[104].stype() != -1)
				iBoxes[104].Destroy();
		}
	}
	else
	{
		drawText("Weather Currently Unavailable", fntCGothic[32], 1, 255, 255, 255, 255, 0, 16, 8, 0, 0);
		if (iBoxes[101].isCreated())
			iBoxes[101].Destroy();
	}

	/* Draw Standard Boxes */
	for (int n = 100; n < 128; n++)
	{
		if (iBoxes[n].isCreated())
		{
			if ((n == 101))
			{
				/* Weather Box */
				if (!iBoxes[n].doDraw(wFadeV[1]))
				{
					m_bQuitting = true;
					m_bRunning = false;
				}
			}
			else
			{
				if (!iBoxes[n].doDraw(-1))
				{
					m_bQuitting = true;
					m_bRunning = false;
				}
			}
		}
	}

	for (int cB = 0; cB < 64; cB++)
	{
		if (!iBoxes[mBoard[cB].CreatedID].doDraw(mBoard[cB].pFade))
		{
			m_bRunning = false;
			m_bQuitting = true;
		}
		else
		{
			if (iBoxes[mBoard[cB].CreatedID].hasSHeader() == true)
			{
				drawText(iBoxes[mBoard[cB].CreatedID].txtSHeader(), fntCGothic[mBoard[cB].HeaderSize], 3, 0, 0, 0, 255,
						mBoard[cB].X + ((mBoard[cB].Width / 255.0) * iBoxes[mBoard[cB].CreatedID].scale()), mBoard[cB].X,
						((mBoard[cB].Height / 255.0) * iBoxes[mBoard[cB].CreatedID].scale()) + mBoard[cB].Y - mBoard[cB].HeaderSize, 0, 0);
			}
		}
	}

	/* Draw *Weather As of: */
	if (wOK)
	{
		char tWOB[64];
		sprintf(tWOB, "Weather Observed at %s", tObservationTime);
		drawText(tWOB, fntCGothic[16], 1, 255, 255, 255, 255, 0, tPXX, 40, 0, 0);
	}

	/* Limit FPS */
	counter.tick();

	/* Draw FPS and Version Debug Info for 10 seconds only. */
	if (cTime <= (dTimeEvent + 10) && m_bQuitting == false)
	{
		if (!iBoxes[106].isCreated() && iBoxes[106].stype() != -1)
		{
			iBoxes[106].Create((char *)"Debug Info Pane", (char *)"", 0, 0, 2, 1068, 688, 260, 30, 260, 30, 200, 1, 1, (char *)"null", false, false, (char *)"", (char *)"", debugLevel);
		}
	}
	else
	{
		if (iBoxes[106].isCreated() && iBoxes[106].stype() != -1)
			iBoxes[106].Destroy();
	}

	if (iBoxes[106].isCreated())
	{
		int currentFPS = 0;
		char FPSC[32] = "";
		currentFPS = counter.get_fps();
		sprintf(FPSC, "FPS - %i", currentFPS);
		char FPSL[32] = "";
		sprintf(FPSL, "(FPS Limit - %i)", FPSLimit);
		drawText("Debug Information", fntCGothic[14], 1, 0, 0, 0, 255, 0, 1108, 702, 0, 0);

		/* Draw Debug Variables */
		drawText(FPSC, fntCGothic[12], 1, 0, 0, 0, 255, 0, 1088, 692, 0, 0);
		drawText(FPSL, fntCGothic[12], 1, 0, 0, 0, 255, 0, 1168, 692, 0, 0);

		/* Draw Version String */
		drawText(sVersionString, fntCGothic[12], 1, 0, 0, 0, 255, 0, 1068, 682, 0, 0);
	}

	/* Swap Buffers */
	SDL_GL_SwapBuffers();
}

void Signage::Clean()
{
	/* Destroy Boxes */
	for (int n = 0; n < 128; n++)
	{
		if (iBoxes[n].isCreated())
		{
			if (iBoxes[n].stype() != -1)
			{
				if (debugLevel > 1)
					printf("Destroying Box iBoxes[%i]...\n", n);
				iBoxes[n].Destroy();
			}
		}
	}

	m_bRunning = false;
	m_bQuitting = true;

	/* Sanity Check */
	for (int n = 0; n < 128; n++)
	{
		if (iBoxes[n].isCreated())
		{
			m_bRunning = true;
		}
	}

	if (m_bRunning == false)
	{
		/* Delete Any and All Testures */
		if (debugLevel > 1)
			printf("Destroying Texture pLogo... ");
		pLogo.Destroy();

		if (debugLevel > 1)
			printf("Destroying Texture weather... ");
		weather[0].Destroy();
		weather[1].Destroy();
		weather[2].Destroy();
		weather[3].Destroy();
		int n;
		for (n = 0; n < 49; n++)
		{
			if (debugLevel > 1)
				printf("Destroying Font fntCGothic[%i]... ", n);
			TTF_CloseFont(fntCGothic[n]);
			if (debugLevel > 1)
				printf("OK\n");
		}

		if (debugLevel > 1)
			printf("Destroying Texture Faders... ");
		tScrollTex[0].Destroy();
		tScrollTex[1].Destroy();

		/* Set Destroy Flag */
		m_bOKKill = true;
	}
}

int Signage::drawText(const char* text, TTF_Font*& fntChosen, int alignment, int cr, int cg, int cb, int alpha, int offset, int px, int py, int mw,
		int tScrollV)
{
	SDL_Surface* initial;
	SDL_Color color;
	SDL_Rect location;
	int w, h, ax, ay, fw, fh, sRes;
	float cpw, bpw;
	GLuint texture;

	color.b = cr;
	color.g = cg;
	color.r = cb;
	location.x = px + offset;
	location.y = py;

	/* Use SDL_TTF to render our text */
	initial = TTF_RenderUTF8_Blended(fntChosen, text, color);

	w = initial->w;
	h = initial->h;

	/* Calculate Texture from Minimum Width for Scrolling Effect */
	if ((mw > 0) && (mw < w))
	{
		if (tScrollV >= (w - mw))
		{
			tScrollV = w - mw;
			sRes = 0;
		}
		else
			sRes = 1;

		cpw = (1.0f / w) * (mw + tScrollV);
		bpw = (1.0f / w) * tScrollV;
		fw = mw;
		fh = initial->h;
	}
	else
	{
		cpw = 1.0f;
		bpw = 0.0f;
		fw = initial->w;
		fh = initial->h;
		sRes = 0;
	}

	/* Do the Alignment - default is left.
	 1 = Left Align
	 2 = Right Align
	 3 = Centre to Screen (of that of px) */

	ax = location.x;
	ay = location.y;

	if (alignment == 2)
	{
		ax = (location.x - w);
		ay = location.y;
	}
	if (alignment == 3)
	{
		ax = (location.x / 2) - (w / 2);
		ay = location.y;
	}

	/* Tell GL about our new texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, initial->pixels);

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* prepare to render our texture */
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor4f(1.0f, 1.0f, 1.0f, (float) alpha / 255.0);

	/* Draw a quad at location */
	glBegin(GL_QUADS);
	/* Recall that the origin is in the lower-left corner
	 That is why the TexCoords specify different corners
	 than the Vertex coors seem to. */
	glTexCoord2f(bpw, 1.0f);
	glVertex2f(ax, ay);
	glTexCoord2f(cpw, 1.0f);
	glVertex2f(ax + fw, ay);
	glTexCoord2f(cpw, 0.0f);
	glVertex2f(ax + fw, ay + fh);
	glTexCoord2f(bpw, 0.0f);
	glVertex2f(ax, ay + fh);
	glEnd();
	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	/* return the deltas in the unused w,h part of the rect */
	location.w = initial->w;
	location.h = initial->h;
	pTWidth = initial->w;
	pTHeight = initial->h;

	/* Clean up */
	SDL_FreeSurface(initial);
	glDeleteTextures(1, &texture);
	return sRes;
}

void Signage::dayInStr(char daysInWord[], int days)
{
	switch (days)
	{
	case 0:
		strcpy(daysInWord, "Sunday");
		break;
	case 1:
		strcpy(daysInWord, "Monday");
		break;
	case 2:
		strcpy(daysInWord, "Tuesday");
		break;
	case 3:
		strcpy(daysInWord, "Wednesday");
		break;
	case 4:
		strcpy(daysInWord, "Thursday");
		break;
	case 5:
		strcpy(daysInWord, "Friday");
		break;
	case 6:
		strcpy(daysInWord, "Saturday");
		break;
	}
}

void Signage::monthInStr(char monthsInWord[], int month)
{
	switch (month)
	{
	case 0:
		strcpy(monthsInWord, "Jan");
		break;
	case 1:
		strcpy(monthsInWord, "Feb");
		break;
	case 2:
		strcpy(monthsInWord, "Mar");
		break;
	case 3:
		strcpy(monthsInWord, "Apr");
		break;
	case 4:
		strcpy(monthsInWord, "May");
		break;
	case 5:
		strcpy(monthsInWord, "Jun");
		break;
	case 6:
		strcpy(monthsInWord, "Jul");
		break;
	case 7:
		strcpy(monthsInWord, "Aug");
		break;
	case 8:
		strcpy(monthsInWord, "Sep");
		break;
	case 9:
		strcpy(monthsInWord, "Oct");
		break;
	case 10:
		strcpy(monthsInWord, "Nov");
		break;
	case 11:
		strcpy(monthsInWord, "Dec");
		break;
	}
}

void Signage::nthInStr(char dowInWord[], int monthday)
{
	switch (monthday)
	{
	case 1:
		strcpy(dowInWord, "st");
		break;
	case 2:
		strcpy(dowInWord, "nd");
		break;
	case 3:
		strcpy(dowInWord, "rd");
		break;
	case 4:
		strcpy(dowInWord, "th");
		break;
	case 5:
		strcpy(dowInWord, "th");
		break;
	case 6:
		strcpy(dowInWord, "th");
		break;
	case 7:
		strcpy(dowInWord, "th");
		break;
	case 8:
		strcpy(dowInWord, "th");
		break;
	case 9:
		strcpy(dowInWord, "th");
		break;
	case 10:
		strcpy(dowInWord, "th");
		break;
	case 11:
		strcpy(dowInWord, "th");
		break;
	case 12:
		strcpy(dowInWord, "th");
		break;
	case 13:
		strcpy(dowInWord, "th");
		break;
	case 14:
		strcpy(dowInWord, "th");
		break;
	case 15:
		strcpy(dowInWord, "th");
		break;
	case 16:
		strcpy(dowInWord, "th");
		break;
	case 17:
		strcpy(dowInWord, "th");
		break;
	case 18:
		strcpy(dowInWord, "th");
		break;
	case 19:
		strcpy(dowInWord, "th");
		break;
	case 20:
		strcpy(dowInWord, "th");
		break;
	case 21:
		strcpy(dowInWord, "st");
		break;
	case 22:
		strcpy(dowInWord, "nd");
		break;
	case 23:
		strcpy(dowInWord, "rd");
		break;
	case 24:
		strcpy(dowInWord, "th");
		break;
	case 25:
		strcpy(dowInWord, "th");
		break;
	case 26:
		strcpy(dowInWord, "th");
		break;
	case 27:
		strcpy(dowInWord, "th");
		break;
	case 28:
		strcpy(dowInWord, "th");
		break;
	case 29:
		strcpy(dowInWord, "th");
		break;
	case 30:
		strcpy(dowInWord, "th");
		break;
	case 31:
		strcpy(dowInWord, "st");
		break;
	}
}

void Signage::parseWeather(xmlNode * a_node)
{
	xmlNode *cur_node = NULL;

	/* Temporary Variables for storing data */
	char tTempI[32];

	for (cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			/* GoogleIG Weather API is officially unsupported, and as-such, come November 1st will be disabled completely.
			 *
			 * http://support.google.com/websearch/bin/answer.py?hl=en&answer=2664197
			 *
			 * New Weather API Support to use data from http://www.worldweatheronline.com/
			 * We want the following variables:
			 *
			 * 		observation_time
			 * 		temp_F
			 * 		weatherCode
			 * 		windspeedMiles
			 * 		winddir16Point
			 * 		humidity
			 * 		pressure
			 * 		cloudcover
			 * 		visibility
			 */

			// Observation Time //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "observation_time")) && (trObservationTime == 0))
			{
				sprintf(tObservationTime, "%s", xmlNodeGetContent(cur_node));
				trObservationTime = 1;
				if (debugLevel > 1)
					printf("\tItem: %s \tData: %s\n", cur_node->name, tObservationTime);
			}

			// Weather Temperature (Farenheight) //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "temp_F")) && (trTemp == 0))
			{
				trTemp = 1;
				sprintf(tTempI, "%s", xmlNodeGetContent(cur_node));
				for (int i = 0; i < strlen(tTempI); ++i)
				{
					if (!isdigit(tTempI[i]))
						trTemp = 0;
				}
				if (trTemp == 1)
					tTemp = atoi(tTempI);
				if (debugLevel > 1)
					printf("\tItem: %s \t\tData: %i\n", cur_node->name, tTemp);
			}

			// Weather Condition Code (for description and Icon check) //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "weatherCode")) && (trWeatherCode == 0))
			{
				trWeatherCode = 1;
				sprintf(tTempI, "%s", xmlNodeGetContent(cur_node));
				for (int i = 0; i < strlen(tTempI); ++i)
				{
					if (!isdigit(tTempI[i]))
						trWeatherCode = 0;
				}
				if (trWeatherCode == 1)
					tWeatherCode = atoi(tTempI);
				if (debugLevel > 1)
					printf("\tItem: %s \tData: %i\n", cur_node->name, tWeatherCode);
			}

			// Wind Speed //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "windspeedMiles")) && (trWindSpeedMPH == 0))
			{
				trWindSpeedMPH = 1;
				sprintf(tTempI, "%s", xmlNodeGetContent(cur_node));
				for (int i = 0; i < strlen(tTempI); ++i)
				{
					if (!isdigit(tTempI[i]))
						trWindSpeedMPH = 0;
				}
				if (trWindSpeedMPH == 1)
					tWindSpeedMPH = atoi(tTempI);
				if (debugLevel > 1)
					printf("\tItem: %s \tData: %i\n", cur_node->name, tWindSpeedMPH);
			}

			// Wind Direction //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "winddir16Point")) && (trWindDir == 0))
			{
				sprintf(tWindDir, "%s", xmlNodeGetContent(cur_node));
				trWindDir = 1;
				if (debugLevel > 1)
					printf("\tItem: %s \tData: %s\n", cur_node->name, tWindDir);
			}

			// Humidity //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "humidity")) && (trHumidity == 0))
			{
				trHumidity = 1;
				sprintf(tTempI, "%s", xmlNodeGetContent(cur_node));
				for (int i = 0; i < strlen(tTempI); ++i)
				{
					if (!isdigit(tTempI[i]))
						trHumidity = 0;
				}
				if (trHumidity == 1)
					tHumidity = atoi(tTempI);
				if (debugLevel > 1)
					printf("\tItem: %s \t\tData: %i\n", cur_node->name, tHumidity);
			}

			// Air Pressure //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "pressure")) && (trPressure == 0))
			{
				trPressure = 1;
				sprintf(tTempI, "%s", xmlNodeGetContent(cur_node));
				for (int i = 0; i < strlen(tTempI); ++i)
				{
					if (!isdigit(tTempI[i]))
						trPressure = 0;
				}
				if (trPressure == 1)
					tPressure = atoi(tTempI);
				if (debugLevel > 1)
					printf("\tItem: %s \t\tData: %i\n", cur_node->name, tPressure);
			}

			// Cloud Cover //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "cloudcover")) && (trCloudCover == 0))
			{
				trCloudCover = 1;
				sprintf(tTempI, "%s", xmlNodeGetContent(cur_node));
				for (int i = 0; i < strlen(tTempI); ++i)
				{
					if (!isdigit(tTempI[i]))
						trCloudCover = 0;
				}
				if (trCloudCover == 1)
					tCloudCover = atoi(tTempI);
				if (debugLevel > 1)
					printf("\tItem: %s \tData: %i\n", cur_node->name, tCloudCover);
			}

			// Visibility //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "visibility")) && (trVisibility == 0))
			{
				trVisibility = 1;
				sprintf(tTempI, "%s", xmlNodeGetContent(cur_node));
				for (int i = 0; i < strlen(tTempI); ++i)
				{
					if (!isdigit(tTempI[i]))
						trVisibility = 0;
				}
				if (trVisibility == 1)
					tVisibility = atoi(tTempI);
				if (debugLevel > 1)
					printf("\tItem: %s \tData: %i\n", cur_node->name, tVisibility);
			}

			// Weather Description //
			if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "weatherDesc")) && (trConditionDesc == 0))
			{
				sprintf(tConditionDesc, "%s", xmlNodeGetContent(cur_node));
				trConditionDesc = 1;
				if (debugLevel > 1)
					printf("\tItem: %s \tData: %s\n", cur_node->name, tConditionDesc);
			}
		}

		// Ensure we only parse data from Current Condition section.
		if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "data")) || (!xmlStrcmp(cur_node->name, (const xmlChar *) "current_condition")))
			parseWeather(cur_node->children);
	}
	if ((trObservationTime == 1) && (trTemp == 1) && (trWeatherCode == 1) && (trWindSpeedMPH == 1) && (trWindDir == 1) && (trHumidity == 1)
			&& (trPressure == 1))
		wOK = true;
	else
		wOK = false;
}

bool Signage::FileExists(const char* FileName)
{
	FILE* fp = NULL;
	fp = fopen(FileName, "rb");
	if (fp != NULL)
	{
		fclose(fp);
		return true;
	}
	return false;
}

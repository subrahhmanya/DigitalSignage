/*
 * signage.cpp
 *
 *  Created on: 28 Feb 2012
 *      Author: pwollaston
 */

#include "signage.h"
#include "textures.h"

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
	wCurDisp = 0;
	wFarenheight = 0;
	wIWind = 0;
	tFarenheight = 0;
	tCondition = 0;
	tHumidity = 0;
	tIcon = -1;
	tOIcon = -2;
	tWind = 0;
	tSrS = 0;
	bV1 = false;
	wOK = false;
	pTWidth = 0;
	wCelcius = 0.0;
}

void Signage::Init(const char* title, int width, int height, int bpp, bool fullscreen)
{

	/* Default to True, as we will falsify later if fail. */
	m_bRunning = true;
	m_bQuitting = false;
	wCurDisp = 0;

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
	printf("Video Initialisation Results\nRed Size:\t%d\nGreen Size:\t%d\nBlue Size:\t%d\nAlpha Size:\t%d\nDouble Buffered? %s\n", red, green, blue, alpha,
			(doublebuf == 1 ? "Yes" : "No"));

	//print video card memory
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	if (info->video_mem == 0)
	{
		printf("**Unable to detect how much Video Memory is available**\n");
	}
	else
	{
		printf("Video Memory (in MB): %d\n", info->video_mem);
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
	pLogo.Load("/screen/textures/orblogo.png");

	/* Load Fonts */
	int n;

	for (n = 0; n < 10; n++)
	{
		printf("Loading Font fntCGothic[%i] - " "/screen/fonts/cgothic.ttf" " size %i... ", n, 12 + (n * 4));
		fntCGothic[n] = TTF_OpenFont("/screen/fonts/cgothic.ttf", 12 + (n * 4));
		if (fntCGothic[n] == NULL)
		{
			fprintf(stderr, "FAILED -  %s\n", SDL_GetError());
		}
		else
		{
			printf("OK\n");
		}
	}

	/* FPS Timer */
	counter = fps_counter();
	counter.set_cap(30);
	counter.cap_on();
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
			}
			break;
		}
	}
}

void Signage::Update()
{
	/* Process and Time String */
	int days;
	char daysInWord[8];
	char monthsInWord[8];
	char mins[8];

	time_t now = time(0);
	ltm = localtime(&now);

	/* Only perform Update if Running */
	if (m_bQuitting == false)
	{
		/* Calculate Sunrise/Sunset */

		/* Calculate the day for Dec 31 of the previous year */
		days = calcDay_Dec31(1900 + ltm->tm_year);
		if (!iBoxes[0].isCreated() && iBoxes[0].stype() != -1)
			iBoxes[0].Create("Orbital Logo", "", 0, pLogo.gltex(), 2, (1280 / 2) - ((((pLogo.width() / 255.0) * 200.0) + 8) / 2), 10, pLogo.width(),
					pLogo.height(), sWidth, sHeight, 200, 1, 1);
		else
			iBoxes[0].doUpdate();

		/* Calculate the day for the given date */
		days = (dayInYear(ltm->tm_mday, ltm->tm_mon) + days) % 7;

		/* Add one day if the year is leap year and desired date is after February */
		if ((1900 + ltm->tm_year) % 4 == 0 && ((1900 + ltm->tm_year) % 100 != 0 || (1900 + ltm->tm_year) % 400 == 0) && ltm->tm_mon > 1)
			days++;
		if (days == 7)
			days = 0;
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

			tC1 = ltm->tm_sec;
		}
		sprintf(dateString, "%i %s - %s, %s %i  %i", ltm->tm_hour, mins, daysInWord, monthsInWord, ltm->tm_mday, (1900 + ltm->tm_year));

		/* Process Weather Information */
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
				{
					printf("Weather Update - %i:%i\n", ltm->tm_hour, ltm->tm_min);
				}
				/* Hour is odd, we call check */
				tFarenheight = 0;
				tCondition = 0;
				tHumidity = 0;
				tIcon = 0;
				tWind = 0;
				wFadeTI = 0;
				wOK = false; /* New Check - default wOK to false (so data regarded dirty first) */
				xmlDoc *doc = NULL;
				xmlNode *root_element = NULL;

				LIBXML_TEST_VERSION // Macro to check API for match with
				// the DLL we are using
				/*parse the file and get the DOM */
				doc = xmlReadFile("http://www.google.com/ig/api?weather=Swynnerton", NULL, 0);
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
					printf("NO DATA/NET CONNECTION\n");
					wLastCheckH = -5;
					wOK = false;
				}
				/* Calculate Weather */
				wCelcius = floorf(((5.0 / 9.0) * (wFarenheight - 32.0)) * 10 + 0.5) / 10;
				sprintf(wTemp, "%.1fºC", wCelcius);
			}
		}
		if (wOK)
		{
			/* Set the Weather Icon */
			if (strcmp("/ig/images/weather/chance_of_storm.gif", wIcon) == 0)
				tIcon = 0;
			if (strcmp("/ig/images/weather/mostly_sunny.gif", wIcon) == 0)
				tIcon = 1;
			if (strcmp("/ig/images/weather/dust.gif", wIcon) == 0)
				tIcon = 2;
			if (strcmp("/ig/images/weather/mostly_cloudy.gif", wIcon) == 0)
				tIcon = 3;
			if (strcmp("/ig/images/weather/cloudy.gif", wIcon) == 0)
				tIcon = 4;
			if (strcmp("/ig/images/weather/chance_of_tstorm.gif", wIcon) == 0)
				tIcon = 5;
			if (strcmp("/ig/images/weather/partly_cloudy.gif", wIcon) == 0)
				tIcon = 6;
			if (strcmp("/ig/images/weather/storm.gif", wIcon) == 0)
				tIcon = 7;
			if (strcmp("/ig/images/weather/sunny.gif", wIcon) == 0)
				tIcon = 8;
			if (strcmp("/ig/images/weather/flurries.gif", wIcon) == 0)
				tIcon = 11;
			if (strcmp("/ig/images/weather/chance_of_snow.gif", wIcon) == 0)
				tIcon = 12;
			if (strcmp("/ig/images/weather/chance_of_rain.gif", wIcon) == 0)
				tIcon = 13;
			if (strcmp("/ig/images/weather/fog.gif", wIcon) == 0)
				tIcon = 14;
			if (strcmp("/ig/images/weather/icy.gif", wIcon) == 0)
				tIcon = 15;
			if (strcmp("/ig/images/weather/sleet.gif", wIcon) == 0)
				tIcon = 16;
			if (strcmp("/ig/images/weather/rain.gif", wIcon) == 0)
				tIcon = 17;
			if (strcmp("/ig/images/weather/mist.gif", wIcon) == 0)
				tIcon = 18;
			if (strcmp("/ig/images/weather/haze.gif", wIcon) == 0)
				tIcon = 19;
			if (strcmp("/ig/images/weather/smoke.gif", wIcon) == 0)
				tIcon = 20;
			if (strcmp("/ig/images/weather/snow.gif", wIcon) == 0)
				tIcon = 21;
			if (strcmp("/ig/images/weather/thunderstorm.gif", wIcon) == 0)
				tIcon = 23;

			/* Do Looping Weather Animations */
			if (now > (wUpdateTimer[1] + 5)) /* Temperature Alert Flash */
			{
				if ((wCelcius <= 3.0) || (wCelcius >= 25.0))
				{
					wUpdateTimer[1] = now;
					switch (wFadeA[1])
					{
					case 0:
						wFadeA[1] = 1;
						;
						break;
					case 1:
						wFadeA[1] = 2;
						;
						break;
					case 2:
						wFadeA[1] = 1;
						;
						break;
					}
				}
				else
					wFadeA[1] = 0;
			}

			if (now > (wUpdateTimer[0] + 15)) /* Weather Condition Cycle */
			{
				wUpdateTimer[0] = now;
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
			double noont = riset + 12.0 * ha / pi;
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
				wUpdateTimer[1] = now;
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
					if ((wCelcius <= 3.0) || (wCelcius >= 25.0))
					{
						if (wFadeTI == 1)
						{
							if (wCelcius <= 3.0)
								sprintf(tBuff, "%s/9.png", tBuff);
							else
								sprintf(tBuff, "%s/22.png", tBuff);
						}
						else
							sprintf(tBuff, "%s/%i.png", tBuff, tIcon);
						if (wFadeTI == 1)
							wFadeTI = 0;
						else
							wFadeTI = 1;
					}
					else
						sprintf(tBuff, "%s/%i.png", tBuff, tIcon);
					weather[0].Load(tBuff);
					iBoxes[1].Create("Weather Condition", "", 0, weather[0].gltex(), 4, 0, 0, weather[0].width(), weather[0].height(), sWidth, sHeight, 255, 1,
							1);
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
					if (wCurDisp == 3)
						wCurDisp = 0;
				}
				if (wFadeV[0] > 255)
				{
					wFadeV[0] = 255;
					wFadeA[0] = 0;
				}
			}
		}

		if (now > (wUpdateTimer[2] + 10))
		{
			/* Check Board Information */
			printf("Board Check - %i:%i:%i\n", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
			wUpdateTimer[2] = now;
			/* Clear old Board Variables */
			for (int wC = 0; wC < 64; wC++)
			{
				for (int vC = 0; vC < 64; vC++)
				{
					tDuration[wC][vC] = false;
					sprintf(tType[wC][vC], "");
					sprintf(tSrc[wC][vC], "");
				}
				validConfig[wC] = false;
				tEn[wC] = false;
				tPX[wC] = false;
				tPY[wC] = false;
				tSc[wC] = false;
				tBr[wC] = false;
				tW[wC] = false;
				tH[wC] = false;
				tBt[wC] = false;
				tA[wC] = false;
				// tTs[wC] = false;
				// sprintf(tUID[wC], "");
				sprintf(bSection[wC], "");
			}
			DIR *d;
			vector<string> dList;
			struct dirent *dir;
			d = opendir("/screen/boards/");
			if (d)
			{
				while ((dir = readdir(d)) != NULL)
				{
					if ((strcmp(".", dir->d_name) != 0) && (strcmp("..", dir->d_name) != 0))
					{
						dList.push_back(dir->d_name);
					}
				}
				closedir(d);
				sort(dList.begin(), dList.end()); /* Sort Array */
				printf("Directories Found = %i\n", dList.size());
				for (int dS = 0; dS < dList.size(); dS++)
				{
					printf("Parsing Board '%s'.", dList[dS].c_str());
					/* Check for valid Configuration Files and Contents */
					char tFName[128];
					sprintf(tFName, "/screen/boards/%s/config.ini", dList[dS].c_str());
					if (FileExists(tFName))
					{
						/* Found a config gile, parse it... */
						CIniFile ini;
						ini.Load(tFName);
						CIniSection* pSection = ini.GetSection("BoardSettings");
						if (pSection)
						{
							CIniKey* pKey;
							if (pSection->GetKey("Enabled"))
							{
								tEn[dS] = atoi(ini.GetKeyValue("BoardSettings", "Enabled").c_str());
								if (tEn[dS] == 1)
								{
									if ((pSection->GetKey("UID")) && (pSection->GetKey("PosX")) && (pSection->GetKey("PosY")) && (pSection->GetKey("Scale"))
											&& (pSection->GetKey("Border")) && (pSection->GetKey("Width")) && (pSection->GetKey("Height"))
											&& (pSection->GetKey("Boards")) && (pSection->GetKey("Alert")) && (pSection->GetKey("TimeStamp")))
									{
										/* Only alter these details if UID or TS differ */
										if ((strcmp(ini.GetKeyValue("BoardSettings", "UID").c_str(), tUID[dS]) != 0)
												|| (tTs[dS] != atoi(ini.GetKeyValue("BoardSettings", "TimeStamp").c_str())))
										{
											tBC[dS] = -1;
											tBR[dS] = 0;
										}
										sprintf(tFldr[dS], "%s", dList[dS].c_str());
										sprintf(tUID[dS], "%s", ini.GetKeyValue("BoardSettings", "UID").c_str());
										tPX[dS] = atoi(ini.GetKeyValue("BoardSettings", "PosX").c_str());
										tPY[dS] = atoi(ini.GetKeyValue("BoardSettings", "PosY").c_str());
										tSc[dS] = atoi(ini.GetKeyValue("BoardSettings", "Scale").c_str());
										tBr[dS] = atoi(ini.GetKeyValue("BoardSettings", "Border").c_str());
										tW[dS] = atoi(ini.GetKeyValue("BoardSettings", "Width").c_str());
										tH[dS] = atoi(ini.GetKeyValue("BoardSettings", "Height").c_str());
										tBt[dS] = atoi(ini.GetKeyValue("BoardSettings", "Boards").c_str());
										tA[dS] = atoi(ini.GetKeyValue("BoardSettings", "Alert").c_str());
										tTs[dS] = atoi(ini.GetKeyValue("BoardSettings", "TimeStamp").c_str());
										validConfig[dS] = true;
									}
								}
							}
						}
						if (validConfig[dS] == true)
						{
							/* Check Each Board for this item */
							int tBtC = tBt[dS];
							for (int brdC = 0; brdC < tBtC; brdC++)
							{
								sprintf(bSection[dS], "Board-%i", brdC + 1);
								pSection = ini.GetSection(bSection[dS]);
								if (pSection)
								{
									printf(".");
									/* Now get the Data */
									if ((pSection->GetKey("Type")) && (pSection->GetKey("Src")) && (pSection->GetKey("Duration")))
									{
										sprintf(tType[dS][brdC], "%s", ini.GetKeyValue(bSection[dS], "Type").c_str());
										sprintf(tSrc[dS][brdC], "%s", ini.GetKeyValue(bSection[dS], "Src").c_str());
										tDuration[dS][brdC] = atoi(ini.GetKeyValue(bSection[dS], "Duration").c_str());
										tSType[dS][brdC] = atoi(ini.GetKeyValue(bSection[dS], "Quality").c_str());
									}
									else
										validConfig[dS] = false;
								}
								else
								{
									printf(".");
									tBt[dS]--;
								}
							}

							/* Get Alert Board Information */
							sprintf(bSection[dS], "Alert");
							pSection = ini.GetSection(bSection[dS]);
							if (pSection)
							{
								printf(".");
								/* Now get the Data */
								if ((pSection->GetKey("Type")) && (pSection->GetKey("Src")) && (pSection->GetKey("Duration")))
								{
									sprintf(aType[dS], "%s", ini.GetKeyValue(bSection[dS], "Type").c_str());
									sprintf(aSrc[dS], "%s", ini.GetKeyValue(bSection[dS], "Src").c_str());
									aDuration[dS] = atoi(ini.GetKeyValue(bSection[dS], "Duration").c_str());
									aSType[dS] = atoi(ini.GetKeyValue(bSection[dS], "Quality").c_str());
								}
								else
									tA[dS] = 0;
							}
						}
					}
					if (validConfig[dS] == true)
					{
						/* We need to check to see if the board already exists, with the correct timestamp data. */
						printf("\n\tBoard '%s' ('%s') with %i boards configured.\n", dList[dS].c_str(), tUID[dS], tBt[dS]);
						for (int brdO = 0; brdO < tBt[dS]; brdO++)
						{
							printf("\t\tBoard %i (%i,%i)\n\t\t\tType\t\t%s\n\t\t\tSrc\t\t%s\n\t\t\tDuration\t%i\n", brdO + 1, dS, brdO, tType[dS][brdO],
									tSrc[dS][brdO], tDuration[dS][brdO]);
						}
						if (tA[dS] == 1)
						{
							printf("\t\tAlert Board %i (%i,%i)\n\t\t\tType\t\t%s\n\t\t\tSrc\t\t%s\n\t\t\tDuration\t%i\n", tBt[dS] + 1, dS, tBt[dS], aType[dS],
									aSrc[dS], aDuration[dS]);
						}
					}
					else
					{
						sprintf(tFldr[dS], "%s", "");
						sprintf(tUID[dS], "%s", "");
						tPX[dS] = 0;
						tPY[dS] = 0;
						tSc[dS] = 0;
						tBr[dS] = 0;
						tW[dS] = 0;
						tH[dS] = 0;
						tBt[dS] = 0;
						tA[dS] = 0;
						tTs[dS] = 0;
						printf("\n\tBoard '%s' has been marked as Disabled.\n", dList[dS].c_str());
					}
				}
			}
		}

		if (now > (wUpdateTimer[3] + 1))
		{
			/* Do Forward Board Sorting to add/update boards when configs change */
			wUpdateTimer[3] = now;
			for (int tB = 0; tB < 64; tB++)
			{
				bool bFound = false;
				for (int pB = 10; pB < 74; pB++)
				{
					if (iBoxes[pB].isCreated())
					{
						for (int ttB = 0; ttB < 64; ttB++)
						{
							if (strlen(tSrc[tB][ttB]) > 0)
							{
								if (strcmp(iBoxes[pB].GetUID(), tUID[tB]) == 0)
								{
									/* We Found a Board!  Check the TimeStamp */
									if (tTs[tB] != iBoxes[pB].GetTStamp())
										/* Time Stamp Not Match - Signal for Board Destroy */
										if (iBoxes[pB].stype() != -1)
											iBoxes[pB].Destroy();
									bFound = true;
									break;
								}
							}
						}
					}
					else if (bFound == true)
						break;
				}
				if (bFound == false)
				{
					/* Add Board Config */
					for (int pB = 10; pB < 74; pB++)
					{
						if ((strlen(tSrc[tB][0]) > 0) && (iBoxes[pB].isCreated() == false))
						{
							/* Empty Board Location - Create */
							if (strcmp(*tType[tB], "image") == 0)
							{
								printf("Creating Board SRC(%i) = %s (%s) Type %i\n", tB, tUID[tB], tSrc[tB][0], 1);
								iBoxes[pB].Create(tUID[tB], tSrc[tB][0], tTs[tB], 0, tBr[tB], tPX[tB], tPY[tB], tW[tB], tH[tB], sWidth, sHeight, tSc[tB], 1,
										tBC[tB]);
								break;
							}
							if (strcmp(*tType[tB], "mplayer") == 0)
							{
								printf("Creating Board SRC(%i) = %s (%s) Type %i\n", tB, tUID[tB], tSrc[tB][0], 3);
								iBoxes[pB].Create(tUID[tB], tSrc[tB][0], tTs[tB], 0, tBr[tB], tPX[tB], tPY[tB], tW[tB], tH[tB], sWidth, sHeight, tSc[tB], 3,
										tBC[tB]);
								break;
							}
							if (strcmp(*tType[tB], "iplayer") == 0)
							{
								printf("Creating Board SRC(%i) = %s (%s) Type %i\n", tB, tUID[tB], tSrc[tB][0], tSType[tB][0]);
								iBoxes[pB].Create(tUID[tB], tSrc[tB][0], tTs[tB], 0, tBr[tB], tPX[tB], tPY[tB], tW[tB], tH[tB], sWidth, sHeight, tSc[tB],
										tSType[tB][0], tBC[tB]);
								break;
							}
						}
					}
				}
			}
			/* Do Reverse Sorting - Remove Boards which are no longer present/disabled */
			for (int pB = 10; pB < 74; pB++)
			{
				if (iBoxes[pB].isCreated())
				{
					bool bFound = false;
					for (int tB = 0; tB < 64; tB++)
					{
						if ((strcmp(iBoxes[pB].GetUID(), tUID[tB]) == 0) && (tTs[tB] == iBoxes[pB].GetTStamp()))
						{
							/* We Found a Board!  Check the TimeStamp */
							bFound = true;
						}
					}
					if (bFound == false)
						iBoxes[pB].Destroy();
				}
			}
		}

		/* Do Board Timer Events */
		for (int tB = 0; tB < 64; tB++)
		{
			for (int pB = 10; pB < 74; pB++)
			{
				if (iBoxes[pB].isCreated())
				{
					for (int ttB = 0; ttB < 64; ttB++)
					{
						if ((strlen(tSrc[tB][ttB]) > 0) && (strcmp(iBoxes[pB].GetUID(), tUID[tB]) == 0) && (iBoxes[pB].getScreen() != -1))
						{
							/* Valid Board Found */
							bool resetClicks = false;
							if ((now > (iBoxes[pB].getClicks() + tDuration[tB][iBoxes[pB].getScreen()])))
							{
								if ((tBR[tB] == tBC[tB]) && (pFade[tB] == 255))
								{
									printf("Timer Event Check (%i) (%i,%i) - Screen %i of %i (%i Seconds) - FLAG #%ix%i\n", now, pB, iBoxes[pB].getScreen(),
											ttB + 1, tBt[tB], tDuration[tB][iBoxes[pB].getScreen()], iBoxes[pB].stype(), pFade[tB]);

									/* Destroy broken Streaming Events */
									if ((iBoxes[pB].stype() != 1) && (tBt[tB] > 1))
									{
										/* Destroy iPlayer/Media Reference */
										iBoxes[pB].Destroy(); /* Problem with Media Streaming - Send a Kill Flag */
									}

									tBR[tB]++;
									if (tBR[tB] > tBt[tB] - 1)
										tBR[tB] = 0;
								}

								/* Process Fade Out Events */
								if (tBR[tB] != tBC[tB])
								{
									pFade[tB] = pFade[tB] - 5;
									if (pFade[tB] < 0)
										pFade[tB] = 0;
								}

								/* Swap Textures (if applicable) */
								if ((tBR[tB] != tBC[tB]) && (pFade[tB] == 0))
								{
									tBC[tB] = tBR[tB];
									iBoxes[pB].setScreen(tBC[tB]);
									char bdID[128];
									sprintf(bdID, "/screen/boards/%s/boards/%s", tFldr[tB], tSrc[tB][tBC[tB]]);
									if (FileExists(bdID) == false)
										sprintf(bdID, "/screen/textures/iplayer/generic_fail.png");
									bTex[tB].Destroy();
									bTex[tB].Load(bdID);
									iBoxes[pB].SwapTex(bTex[tB].gltex());
									iBoxes[pB].setClicks(0); /* Set to 0 to take into account of different timings per page */
								}

								/* Process Fade In Events */
								if (tBR[tB] == tBC[tB])
								{
									pFade[tB] = pFade[tB] + 5;
									if (pFade[tB] > 255)
									{
										pFade[tB] = 255;
									}
									if (pFade[tB] == 255)
										resetClicks = true;
								}

								/* Reset Clicks */
								if (resetClicks)
									iBoxes[pB].setClicks(now);
							}
						}
					}
				}
			}
		}
	}
	/* Save Screenshot every 5 seconds */
	if (now > wUpdateTimer[4] + 5)
	{

	}
}

void Signage::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	/* Draw Title */
	drawText("Notification Centre", fntCGothic[7], 3, 255, 255, 255, 255, 1280, 680);

	/* Draw Time */
	drawText(dateString, fntCGothic[5], 2, 255, 255, 255, 255, 1262, 8);

	if (bV1)
	{
		if (ltm->tm_hour > 9)
			drawText(":", fntCGothic[5], 2, 255, 255, 255, 255, 1262 - pTWidth + 44, 11);
		else
			drawText(":", fntCGothic[5], 2, 255, 255, 255, 255, 1262 - pTWidth + 26, 11);
	}

	drawText(nthsInWord, fntCGothic[0], 1, 255, 255, 255, 255, 1172, 32);

	/* Draw Weather */
	if (wOK)
	{
		pTWidth = 0;

		if (wCelcius <= 3.0)
			drawText(wTemp, fntCGothic[5], 1, 128, 128, 255, 255, 16, 8);
		else if (wCelcius >= 25.0)
			drawText(wTemp, fntCGothic[5], 1, 255, 0, 0, 255, 16, 8);
		else
			drawText(wTemp, fntCGothic[5], 1, 255, 255, 255, 255, 16, 8);

		/* Still check Icon position and move if required.  Temp could change, but if Icon remains same, we need to refresh */
		iBoxes[1].rePos(16 + pTWidth, 0);

		switch (wCurDisp)
		{
		case 0:
			drawText(wCondition, fntCGothic[5], 1, 255, 255, 255, wFadeV[0], ((iBoxes[1].width() / 255.0) * iBoxes[1].scale()) + pTWidth + 16, 8);
			;
			break;
		case 1:
			drawText(wHumidity, fntCGothic[5], 1, 255, 255, 255, wFadeV[0], ((iBoxes[1].width() / 255.0) * iBoxes[1].scale()) + pTWidth + 16, 8);
			;
			break;
		case 2:
			drawText(wWind, fntCGothic[5], 1, 255, 255, 255, wFadeV[0], ((iBoxes[1].width() / 255.0) * iBoxes[1].scale()) + pTWidth + 16, 8);
			;
			break;
		}

	}
	else
	{
		drawText("Weather Unavailable", fntCGothic[5], 1, 255, 255, 255, 255, 16, 8);
		iBoxes[1].Destroy();
	}

	/* Draw Boxes */
	for (int n = 0; n < 128; n++)
	{
		if (iBoxes[n].isCreated())
		{
			if (n < 10)
			{
				if ((n == 1))
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
			else
			{
				if (!iBoxes[n].doDraw(pFade[n - 10]))
				{
					m_bRunning = false;
					m_bQuitting = true;
				}
			}
		}
	}

	/* Draw FPS */
	int currentFPS = 0;
	char FPSC[32] = "";
	counter.tick();
	currentFPS = counter.get_fps();
	sprintf(FPSC, "FPS - %i", currentFPS);
	// drawText(FPSC, fntCGothic[0], 1, 255, 255, 255, 255, 2, 0);

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
		printf("Destroying Texture pLogo... ");
		pLogo.Destroy();

		printf("Destroying Texture weather... ");
		weather[0].Destroy();
		int n;
		for (n = 0; n < 10; n++)
		{
			printf("Destroying Font fntCGothic[%i]... ", n);
			TTF_CloseFont(fntCGothic[n]);
			printf("OK\n");
		}
	}
}

void Signage::drawText(const char* text, TTF_Font*& fntChosen, int alignment, int cr, int cg, int cb, int alpha, int px, int py)
{
	SDL_Surface* initial;
	SDL_Color color;
	SDL_Rect location;
	int w, h, ax, ay;
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
	pTHeight = initial->h;

	/* Clean up */
	SDL_FreeSurface(initial);
	glDeleteTextures(1, &texture);
}

int Signage::calcDay_Dec31(int yyyy)
{
	int dayCode = 0;
	dayCode = ((yyyy - 1) * 365 + (yyyy - 1) / 4 - (yyyy - 1) / 100 + (yyyy - 1) / 400) % 7;
	return dayCode;
}

int Signage::dayInYear(int dd, int mm)
{
	switch (mm)
	{
	case 11:
		dd += 30;
		break;
	case 10:
		dd += 31;
		break;
	case 9:
		dd += 30;
		break;
	case 8:
		dd += 31;
		break;
	case 7:
		dd += 31;
		break;
	case 6:
		dd += 30;
		break;
	case 5:
		dd += 31;
		break;
	case 4:
		dd += 30;
		break;
	case 3:
		dd += 31;
		break;
	case 2:
		dd += 28;
		break;
	case 1:
		dd += 31;
		break;
	}
	return dd;
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
	char tWord[16];
	for (cur_node = a_node; cur_node; cur_node = cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "condition"))
			{
				if (tCondition == 0)
				{
					sprintf(wCondition, "%s", cur_node->properties->children->content);
					tCondition = 1;
					printf("\tItem: %s \tData: %s\n", cur_node->name, wCondition);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "temp_f"))
			{
				sprintf(tWord, "%s", cur_node->properties->children->content);
				if (tFarenheight == 0)
				{
					wFarenheight = strtol(tWord, NULL, 0);
					tFarenheight = wFarenheight;
					printf("\tItem: %s \t\tData: %i\n", cur_node->name, wFarenheight);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "humidity"))
			{
				if (tHumidity == 0)
				{
					tHumidity = 1;
					sprintf(wHumidity, "%s", cur_node->properties->children->content);
					printf("\tItem: %s \t\tData: %s\n", cur_node->name, wHumidity);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "icon"))
			{
				if (tIcon == 0)
				{
					tIcon = 1;
					sprintf(wIcon, "%s", cur_node->properties->children->content);
					printf("\tItem: %s \t\tData: %s\n", cur_node->name, wIcon);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "wind_condition"))
			{
				if (tWind == 0)
				{
					tWind = 1;
					sprintf(wWind, "%s", cur_node->properties->children->content);
					/* Get speed of wind and convert to int */
					for (int i = 0; i < strlen(wWind); ++i)
					{
						if (isdigit(wWind[i]))
							if (wIWind == 0)
								wIWind = atoi(&wWind[i]);
					}
					printf("\tItem: %s \tData: %s\n", cur_node->name, wWind);
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

int Screenshot(char *filename)
{
	SDL_Surface *screen;
	SDL_Surface *temp;
	unsigned char *pixels;
	int i;

	if (!(screen->flags & SDL_OPENGL))
	{
		SDL_SaveBMP(temp, filename);
		return 0;
	}

	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
			0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
			);
	if (temp == NULL)
		return -1;

	pixels = malloc(3 * w * h);
	if (pixels == NULL)
	{
		SDL_FreeSurface(temp);
		return -1;
	}

	glReadPixels(0, 0, screen->w, screen->h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	for (i = 0; i < h; i++)
		memcpy(((char *) temp->pixels) + temp->pitch * i, pixels + 3 * w * (h - i - 1), w * 3);
	free(pixels);

	SDL_SaveBMP(temp, filename);
	SDL_FreeSurface(temp);
	return 0;
}

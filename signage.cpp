/*
 * signage.cpp
 *
 *  Created on: 28 Feb 2012
 *      Author: pwollaston
 */

#include "signage.h"
#include "textures.h"

/* Signage constructor */
Signage::Signage() {
}

void Signage::Init(const char* title, int width, int height, int bpp, bool fullscreen) {

	/* Default to True, as we will falsify later if fail. */
	m_bRunning = true;

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

	if (fullscreen) {
		/* We're running full screen on the target, so use full screen */
		screen = SDL_SetVideoMode(width, height, bpp, SDL_ASYNCBLIT | SDL_FULLSCREEN | SDL_HWSURFACE | SDL_OPENGL);
		SDL_ShowCursor(SDL_DISABLE);
	} else {
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
	if (info->video_mem == 0) {
		printf("**Unable to detect how much Video Memory is available**\n");
	} else {
		printf("Video Memory (in MB): %d\n", info->video_mem);
	}

	if (screen == NULL) {
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		m_bRunning = false;
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
	weather[0].Load("/screen/textures/weather/chance_of_storm.png");
	weather[1].Load("/screen/textures/weather/mostly_sunny.png");
	weather[2].Load("/screen/textures/weather/dust.png");
	weather[3].Load("/screen/textures/weather/mostly_cloudy.png");
	weather[4].Load("/screen/textures/weather/cloudy.png");
	weather[5].Load("/screen/textures/weather/chance_of_tstorm.png");
	weather[6].Load("/screen/textures/weather/partly_cloudy.png");
	weather[7].Load("/screen/textures/weather/storm.png");
	weather[8].Load("/screen/textures/weather/sunny.png");
	weather[9].Load("/screen/textures/weather/cold.png");
	weather[10].Load("/screen/textures/weather/windy.png");
	weather[11].Load("/screen/textures/weather/flurries.png");
	weather[12].Load("/screen/textures/weather/chance_of_snow.png");
	weather[13].Load("/screen/textures/weather/chance_of_rain.png");
	weather[14].Load("/screen/textures/weather/fog.png");
	weather[15].Load("/screen/textures/weather/icy.png");
	weather[16].Load("/screen/textures/weather/sleet.png");
	weather[17].Load("/screen/textures/weather/rain.png");
	weather[18].Load("/screen/textures/weather/mist.png");
	weather[19].Load("/screen/textures/weather/haze.png");
	weather[20].Load("/screen/textures/weather/smoke.png");
	weather[21].Load("/screen/textures/weather/snow.png");
	weather[22].Load("/screen/textures/weather/hot.png");
	weather[23].Load("/screen/textures/weather/thunderstorm.png");

	/* Load Fonts */
	int n;

	for (n = 0; n < 10; n++) {
		printf("Loading Font fntCGothic[%i] - " "/screen/fonts/cgothic.ttf" " size %i... ", n, 16 + (n * 4));
		fntCGothic[n] = TTF_OpenFont("/screen/fonts/cgothic.ttf", 16 + (n * 4));
		if (fntCGothic[n] == NULL) {
			fprintf(stderr, "FAILED -  %s\n", SDL_GetError());
		} else {
			printf("OK\n");
		}
	}

	/* Preset variables */
	tFarenheight = 0;
	tCondition = 0;
	tHumidity = 0;
	tIcon = 0;
	tWind = 0;
	wFarenheight = 0;
	wIWind = 0;

	/* Test iPlayer Feed */
	iPlayerScale = 400;
	iPlayerPosX = 60;
	iPlayerPosY = 40;

	/* FPS Timer */
	counter = fps_counter();
	counter.set_cap(30);
	counter.cap_on();
}

void Signage::HandleEvents(Signage* signage) {
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			signage->Quit();
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				signage->Quit();
			}
			break;
		}
	}
}

void Signage::Update() {
	/* Process and Time String */
	int days;
	char daysInWord[8];
	char monthsInWord[8];
	char mins[8];

	time_t now = time(0);
	ltm = localtime(&now);

	/* Calculate the day for Dec 31 of the previous year */
	days = calcDay_Dec31(1900 + ltm->tm_year);
	if (!iBoxes[0].isCreated())
		iBoxes[0].Create(true, pLogo.gltex(), 2, (1280 / 2) - ((((pLogo.width() / 255.0) * 200.0) + 8) / 2), 10, pLogo.width(), pLogo.height(), sWidth,
				sHeight, 200, 1);
	else
		iBoxes[0].doUpdate();

	if (!iBoxes[1].isCreated())
		iBoxes[1].Create(true, 0, 2, 50, 65, 688, 384, sWidth, sHeight, 255, 6);
	else
		iBoxes[1].doUpdate();

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
	if (tC1 != ltm->tm_sec) {
		if (bV1)
			bV1 = false;
		else
			bV1 = true;

		tC1 = ltm->tm_sec;
	}
	sprintf(dateString, "%i %s - %s, %s %i  %i", ltm->tm_hour, mins, daysInWord, monthsInWord, ltm->tm_mday, (1900 + ltm->tm_year));

	/* Process Weather Information */
	/* Do Weather Check (update once every 15 minutes) */
	if ((wLastCheckH != ltm->tm_hour) || (ltm->tm_min == 15) || (ltm->tm_min == 30) || (ltm->tm_min == 45) || (ltm->tm_min == 0)) {
		if (wLastCheckM != ltm->tm_min) {
			/* Update last check interval */
			wLastCheckM = ltm->tm_min;

			if (ltm->tm_min < 10) {
				printf("Weather Update - %i:0%i\n", ltm->tm_hour, ltm->tm_min);
			} else {
				printf("Weather Update - %i:%i\n", ltm->tm_hour, ltm->tm_min);
			}
			/* Hour is odd, we call check */
			tFarenheight = 0;
			tCondition = 0;
			tHumidity = 0;
			tIcon = 0;
			tWind = 0;
			wOK = false; /* New Check - default wOK to false (so data regarded dirty first) */
			xmlDoc *doc = NULL;
			xmlNode *root_element = NULL;

			LIBXML_TEST_VERSION // Macro to check API for match with
			// the DLL we are using
			/*parse the file and get the DOM */
			doc = xmlReadFile("http://www.google.com/ig/api?weather=ST150QN", NULL, 0);
			if (doc) {

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
			} else {
				/* Update last check interval (we want to check in another minute) */
				printf("NO DATA/NET CONNECTION\n");
				wLastCheckH = -5;
				wOK = false;
			}
			/* Calculate Weather */
			wCelcius = floorf(((5.0 / 9.0) * (wFarenheight - 32.0)) * 10 + 0.5) / 10;
			sprintf(wTemp, "%.1fºC", wCelcius);
			printf("%i %f %s\n", wFarenheight, wCelcius, wTemp);
		}
	}
}

void Signage::Draw() {
	// Draw something with Logotex
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (!iBoxes[0].doDraw())
		m_bRunning = false;
	if (!iBoxes[1].doDraw())
		m_bRunning = false;

	/* Draw Title */
	drawText("Notification Centre", fntCGothic[6], 3, 255, 255, 255, 255, 1280, 680);

	/* Draw Time */
	drawText(dateString, fntCGothic[5], 2, 255, 255, 255, 255, 1262, 8);

	if (bV1) {
		if (ltm->tm_hour > 9)
			drawText(":", fntCGothic[5], 2, 255, 255, 255, 255, 1262 - pTWidth + 49, 11);
		else
			drawText(":", fntCGothic[5], 2, 255, 255, 255, 255, 1262 - pTWidth + 29, 11);
	}

	drawText(nthsInWord, fntCGothic[0], 1, 255, 255, 255, 255, 1160, 32);

	/* Draw Weather */
	if (!wOK) {

	} else {
		drawText("Weather Unavailable", fntCGothic[5], 1, 255, 255, 255, 255, 16, 8);
	}

	/* iPlayer Testing - Box is 688x384, iPlayer pos is 20x60 */
	//drawInfoBox(0, 2, iPlayerPosX, 720 - ((384.0 / 255.0) * iPlayerScale)
	//		- iPlayerPosY, 688, 384, 0, 384, 1.0f, 1.0f, 1.0f, iPlayerScale,
	//		255, 255);

	/* Draw FPS */
	int currentFPS = 0;
	char FPSC[32] = "";
	counter.tick();
	currentFPS = counter.get_fps();
	sprintf(FPSC, "FPS - %i", currentFPS);
	drawText(FPSC, fntCGothic[0], 1, 255, 255, 255, 255, 2, 0);
	SDL_GL_SwapBuffers();
}

void Signage::Clean() {
	/* Delete Any and All Testures */
	printf("Destroying mplayer reference...");
	system("killall -9 get_iplayer");
	system("killall -9 mplayer");
	system("killall -9 rtmpdump");

	printf("Destroying Texture pLogo... ");
	pLogo.Destroy();

	int n;
	for (n = 0; n < 24; n++) {
		printf("Destroying Texture weather[%i]... ", n);
		weather[n].Destroy();
	}
	for (n = 0; n < 10; n++) {
		printf("Destroying Font ftnCGothic[%i]... ", n);
		TTF_CloseFont(fntCGothic[n]);
		printf("OK\n");
	}
}

void Signage::drawText(const char* text, TTF_Font*& fntChosen, int alignment, int cr, int cg, int cb, int alpha, int px, int py) {
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

	if (alignment == 2) {
		ax = (location.x - w);
		ay = location.y;
	}
	if (alignment == 3) {
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

	/* Clean up */
	SDL_FreeSurface(initial);
	glDeleteTextures(2, &texture);
}

int Signage::calcDay_Dec31(int yyyy) {
	int dayCode = 0;
	dayCode = ((yyyy - 1) * 365 + (yyyy - 1) / 4 - (yyyy - 1) / 100 + (yyyy - 1) / 400) % 7;
	return dayCode;
}

int Signage::dayInYear(int dd, int mm) {
	switch (mm) {
	case 11:
		dd += 30;
	case 10:
		dd += 31;
	case 9:
		dd += 30;
	case 8:
		dd += 31;
	case 7:
		dd += 31;
	case 6:
		dd += 30;
	case 5:
		dd += 31;
	case 4:
		dd += 30;
	case 3:
		dd += 31;
	case 2:
		dd += 28;
	case 1:
		dd += 31;
	}
	return dd;
}

void Signage::dayInStr(char daysInWord[], int days) {
	switch (days) {
	case 0:
		strcpy(daysInWord, "Sun");
		break;
	case 1:
		strcpy(daysInWord, "Mon");
		break;
	case 2:
		strcpy(daysInWord, "Tue");
		break;
	case 3:
		strcpy(daysInWord, "Wed");
		break;
	case 4:
		strcpy(daysInWord, "Thu");
		break;
	case 5:
		strcpy(daysInWord, "Fri");
		break;
	case 6:
		strcpy(daysInWord, "Sat");
		break;
	}
}

void Signage::monthInStr(char monthsInWord[], int month) {
	switch (month) {
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

void Signage::nthInStr(char dowInWord[], int monthday) {
	switch (monthday) {
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

void Signage::parseWeather(xmlNode * a_node) {
	xmlNode *cur_node = NULL;
	char tWord[16];
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "condition")) {
				if (tCondition == 0) {
					sprintf(wCondition, "%s", cur_node->properties->children->content);
					tCondition = 1;
					printf("\tItem: %s \tData: %s\n", cur_node->name, wCondition);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "temp_f")) {
				sprintf(tWord, "%s", cur_node->properties->children->content);
				if (tFarenheight == 0) {
					wFarenheight = strtol(tWord, NULL, 0);
					tFarenheight = wFarenheight;
					printf("\tItem: %s \t\tData: %i\n", cur_node->name, wFarenheight);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "humidity")) {
				if (tHumidity == 0) {
					tHumidity = 1;
					sprintf(wHumidity, "%s", cur_node->properties->children->content);
					printf("\tItem: %s \t\tData: %s\n", cur_node->name, wHumidity);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "icon")) {
				if (tIcon == 0) {
					tIcon = 1;
					sprintf(wIcon, "%s", cur_node->properties->children->content);
					printf("\tItem: %s \t\tData: %s\n", cur_node->name, wIcon);
				}
			}

			if (!xmlStrcmp(cur_node->name, (const xmlChar *) "wind_condition")) {
				if (tWind == 0) {
					tWind = 1;
					sprintf(wWind, "%s", cur_node->properties->children->content);
					/* Get speed of wind and convert to int */
					for (int i = 0; i < strlen(wWind); ++i) {
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

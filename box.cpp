/*
 * box.cpp
 *
 *  Created on: 5 Mar 2012
 *      Author: pwollaston
 */

#include "box.h"
#include "textures.h"

/* Signage constructor */
Box::Box()
{
	m_bRunning = false;
	bType = false;
	bCol = 0;
	bX = 0;
	bY = 0;
	bW = 0;
	bH = 0;
	bScale = 0;
	sType = 0;
	scrollv = 0;
	sWidth = 0;
	sHeight = 0;
	bTStamp = 0;
	ipLFail = 0;
	ipLooper = 0;
	debugLevel = 0;
	tAlpha = 0;
	tSTimer = 0;
	tCScreen = -1;
	bHeaderEnab = false;
	sprintf(bHeaderTxt, "*");
	sprintf(audEnable, "*");
	sprintf(bUID, "*");
	sprintf(bMSRC, "*");
	glTex = 0;
	mplayer_fp = NULL;
	sHeaderEnab = NULL;
	play_win = 0;
	ipVis = NULL;
	ipCC = 0;
}

bool Box::doDraw(int aOverride, ...)
{
	/* mplayer/iPlayer check
	 * Streaming Issue will have sType of 2
	 * mPlayer will have sType of 3
	 * iPlayer will have sType of 4+ */

	/* tAlpha is the initial Alpha value when creating a new Box.
	 * All boxes need to fade in, and fade out, thus being clean. */

	int tAlphaO = -1;
	int tbR = 255;
	int tbB = 255;
	int tbG = 255;
	int cRGB = 0;

	va_list Arguments;
	va_start(Arguments, aOverride);

	for (int i = 0; i < aOverride; ++i)
	{
		switch (cRGB)
		{
		case 0:
			tAlphaO = va_arg(Arguments, int);
			break;
		case 1:
			tbR = va_arg(Arguments, int);
			break;
		case 2:
			tbB = va_arg(Arguments, int);
			break;
		case 3:
			tbG = va_arg(Arguments, int);
			break;
		}
		cRGB++;
	}

	va_end(Arguments);

	if (sType > 0)
		tAlpha = tAlpha + 15;
	else
		tAlpha = tAlpha - 15;

	if ((tAlpha > 255) && (sType > 0))
	{
		tAlpha = 255;
		if (tCScreen == -1) /* Initial Creation */
			tCScreen = 0;
	}
	if ((tAlpha < 0) && (sType == -1))
		tAlpha = 0;

	if ((tAlpha == 255) && (sType > 0))
	{
		if ((sType >= 4) && (!ipVis) && (tAlphaO == 255))
			createiPlayer(sType - 3, bW, bH, bX, bY, bScale);

		if ((sType >= 4) && (ipVis))
		{
			/* Check for getiPlayer Instance, restart if required. */
			ipCC++;
			if (ipCC > 100)
			{
				ipCC = 0;
				char pgc[1024];
				sprintf(pgc, "echo $(ps aux | grep '0x%lx' | grep -vn grep | awk '{print $2}')", play_win);
				FILE *fp = popen(pgc, "r");
				char buff[1024];
				fgets(buff, sizeof buff, fp);
				pclose(fp);
				if (strcmp(buff, "\n") == 0)
				{
					/* get_iplayer isn't running - Close current window and set ipVis to false
					 * iPlayer Window will automatically re-launch when closed.
					 * Also, worth cycling through Quality settings, just in case it's an issue
					 * with the stream, as experienced around May 20th 2012. */
					if (debugLevel > 1)
						printf("\nKilling Window Reference - 0x%lx\n", play_win);

					SDL_SysWMinfo sdl_info;

					sdl_info = get_sdl_wm_info();
					sdl_info.info.x11.lock_func();

					destroy_x11_subwindow(sdl_info.info.x11.display, play_win);

					/* Try '2' for Quality */
					if (ipLFail == 0)
						ipLFail = 1;
					else
					{
						ipLFail = 0;
						ipLooper++;
					}

					if (ipLooper == 2)
					{
						sType = 4;
					}
					else if (ipLooper >= 3)
					{
						sType++;
					}

					if ((sType - 3) > 4)
					{
						/* Set iPlayer to type 2 (Broken) */
						sType = 2;
					}

					sdl_info.info.x11.unlock_func();
					ipVis = false;
				}
			}
		}
	}
	else if ((tAlpha == 0) && (sType == -1))
	{
		/* Delete Board */
		if (debugLevel > 1)
			printf("    BOX \"%s\" Destroy Event Complete\n", bUID);
		debugLevel = 0;
		glTex = 0;
		m_bRunning = false;
		bType = false;
		bCol = 0;
		bX = 0;
		bY = 0;
		bW = 0;
		bH = 0;
		bScale = 0;
		scrollv = 0;
		sWidth = 0;
		sHeight = 0;
		sType = 0;
		bTStamp = 0;
		ipLFail = 0;
		ipLooper = 0;
		tAlpha = 0;
		tSTimer = 0;
		tCScreen = -1;
		bHeaderEnab = false;
		sHeaderEnab = false;
		sprintf(bHeaderTxt, "*");
		sprintf(sHeaderTxt, "*");
		sprintf(audEnable, "*");
		sprintf(bUID, "*");
		sprintf(bMSRC, "*");
		if (ipBG.width() != 0)
		{
			if (debugLevel > 1)
				printf(" - ipBG");
			ipBG.Destroy();
		}
		if (layout[0].width() != 0)
		{
			if (debugLevel > 1)
				printf(" - BoxTex1 ");
			layout[0].Destroy();
		}
		if (layout[1].width() != 0)
		{
			if (debugLevel > 1)
				printf(" - BoxTex2 ");
			layout[1].Destroy();
		}
		if (layout[2].width() != 0)
		{
			if (debugLevel > 1)
				printf(" - BoxTex3 ");
			layout[2].Destroy();
		}
		if (layout[3].width() != 0)
		{
			if (debugLevel > 1)
				printf(" - BoxTex4 ");
			layout[3].Destroy();
		}
		if (layout[4].width() != 0)
		{
			if (debugLevel > 1)
				printf(" - BoxTex5 ");
			layout[4].Destroy();
		}
	}

	if (tAlphaO == -1)
		tAlphaO = 255;

	drawInfoBox(glTex, bType, bX, bY, bW, bH, sHeight, (1.0 / 255.0) * tbR, (1.0 / 255.0) * tbB, (1.0 / 255.0) * tbG, bScale, tAlpha, tAlphaO);

	return true;
}

void Box::doUpdate()
{
	/* Update Routine */
}

void Box::Destroy()
{
	if (debugLevel > 1)
		printf("    BOX \"%s\" Destroy Event Called\n", bUID);
	if (m_bRunning)
	{
		if ((sType >= 4) && (ipVis))
		{
			if (debugLevel > 1)
				printf(" - Destroying iplayer/mplayer reference...\n");
			/* Get Current PID matching WID */
			char tipc[1024];
			sprintf(tipc, "echo $(ps aux | grep '0x%lx' | grep -vn grep | awk '{print $2}')", play_win);
			FILE *tIPC = popen(tipc, "r");
			char tIPCbuff[1024];
			fgets(tIPCbuff, sizeof tIPCbuff, tIPC);
			pclose(tIPC);
			if (strcmp(tIPCbuff, "\n") != 0)
			{
				char * tIPCLst;
				tIPCLst = strtok(tIPCbuff, " ,.-");
				while (tIPCLst != NULL)
				{
					if (debugLevel > 1)
						printf("Killing PID - %s...", tIPCLst);
					char tcb[1024];
					sprintf(tcb, "kill -9 %s", tIPCLst);
					system(tcb);
					tIPCLst = strtok(NULL, "\n ,.-");
					if (debugLevel > 1)
						printf(" [OK]\n");
				}
				pclose(mplayer_fp);
			}

			char pgc[1024];
			sprintf(pgc, "echo $(ps aux | grep '0x%lx' | grep -vn grep | awk '{print $2}')", play_win);
			FILE *fp = popen(pgc, "r");
			char buff[1024];
			fgets(buff, sizeof buff, fp);
			pclose(fp);
			if (strcmp(buff, "\n") == 0)
			{
				/* get_iplayer isn't running - Close current window and set ipVis to false
				 * iPlayer Window will automatically re-launch when closed. */
				if (debugLevel > 1)
					printf("\nDestroying Window Reference - 0x%lx\n", play_win);

				SDL_SysWMinfo sdl_info;

				sdl_info = get_sdl_wm_info();
				sdl_info.info.x11.lock_func();

				destroy_x11_subwindow(sdl_info.info.x11.display, play_win);

				sdl_info.info.x11.unlock_func();
				ipVis = false;
			}
		}
		if (ipVis == false)
		{
			if (sType != -1)
			{
				sType = -1;
			}
		}
	}
}

void Box::SwapTex(GLuint TextureID, int w, int h)
{
	glTex = 0;
	glGenTextures(1, &glTex);
	glTex = TextureID;
	sWidth = w;
	sHeight = h;
}

void Box::Create(char btUID[128], char btMSRC[1024], int tStamp, GLuint TextureID, int bcol, int px, int py, int w, int h, int aw, int ah, int scale,
		int sourceType, int dScreen, char dAudio[16], bool hasHeader, bool hasSHeader, char txtHeader[256], char txtSHeader[256], int dbgLVL)
{
	bType = false;
	if (bcol == 1)
	{
		bType = true;
		layout[0].Load("/screen/textures/orb_bl.png", debugLevel);
		layout[1].Load("/screen/textures/orb_bt.png", debugLevel);
		layout[2].Load("/screen/textures/orb_bcrnr.png", debugLevel);
		layout[3].Load("/screen/textures/orb_boxb.png", debugLevel);
		layout[4].Load("/screen/textures/orb_bh.png", debugLevel);
	}
	else if (bcol == 2)
	{
		bType = true;
		layout[0].Load("/screen/textures/orb_wl.png", debugLevel);
		layout[1].Load("/screen/textures/orb_wt.png", debugLevel);
		layout[2].Load("/screen/textures/orb_wcrnr.png", debugLevel);
		layout[3].Load("/screen/textures/orb_boxw.png", debugLevel);
		layout[4].Load("/screen/textures/orb_wh.png", debugLevel);
	}
	else if (bcol == 3)
	{
		bType = true;
		layout[0].Load("/screen/textures/orb_tl.png", debugLevel);
		layout[1].Load("/screen/textures/orb_tt.png", debugLevel);
		layout[2].Load("/screen/textures/orb_tcrnr.png", debugLevel);
	}
	debugLevel = dbgLVL;
	sType = sourceType;
	bCol = bcol;
	bX = px;
	bY = py;
	bW = w;
	bH = h;
	scrollv = 0;
	sWidth = aw;
	sHeight = ah;
	bScale = scale;
	m_bRunning = true;
	bTStamp = tStamp;
	tCScreen = dScreen;
	bHeaderEnab = hasHeader;
	sHeaderEnab = hasSHeader;
	sprintf(sHeaderTxt, "%s", txtSHeader);
	sprintf(bHeaderTxt, "%s", txtHeader);
	sprintf(audEnable, "%s", dAudio);
	sprintf(bUID, "%s", btUID);
	sprintf(bMSRC, "%s", btMSRC);
	glGenTextures(1, &glTex);
	glTex = TextureID;
}

void Box::drawInfoBox(GLuint TextureID, bool bVis, int px, int py, int minx, int miny, int absh, float br, float bg, float bb, int scale, int balpha,
		int calpha)
{

	/* Box Types (bcol)
	 * 1 = Black BG
	 * 2 = White BG
	 * 3 = Transparent BG
	 *
	 * Any other setting will result in *no* border */

	/* We don't want the border to have less opacity than the contents, so match contents to border if required */
	if (balpha < calpha)
		calpha = balpha;

	int w = (minx / 255.0) * scale;
	int h = (miny / 255.0) * scale;

	if (bVis)
	{
		/* We only want to draw the border of each frame if this is set.
		 * Otherwise, we are most likely drawing just an overlaying texture object */
		glColor4f(br, bg, bb, (float) balpha / 255.0);
		/* Draw Left & Right Sides of Box */
		glBindTexture(GL_TEXTURE_2D, layout[0].gltex());

		/* Draw Left Border */
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px - 12, py + 4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px + 4, py + 4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px + 4, py + h - 4);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px - 12, py + h - 4);
		glEnd();
		/* Draw Right Border */
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px + w + 12, py + 4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px + w - 4, py + 4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px + w - 4, py + h - 4);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px + w + 12, py + h - 4);
		glEnd();
		glFinish();

		/* Draw Top & Bottom of Box */
		glBindTexture(GL_TEXTURE_2D, layout[1].gltex());

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px + 4, py + h - 4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px + w - 4, py + h - 4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px + w - 4, py + h + 12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px + 4, py + h + 12);
		glEnd();
		/* Draw Bottom Border */
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px + 4, py + 4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px + w - 4, py + 4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px + w - 4, py - 12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px + 4, py - 12);
		glEnd();
		glFinish();

		/* Draw Corners of Box */
		glBindTexture(GL_TEXTURE_2D, layout[2].gltex());
		/* Draw TL */
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px - 12, py + h - 4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px + 4, py + h - 4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px + 4, py + h + 12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px - 12, py + h + 12);
		glEnd();
		/* Draw BL */
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px - 12, py + 4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px + 4, py + 4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px + 4, py - 12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px - 12, py - 12);
		glEnd();
		/* Draw TR */
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px + w + 12, py + h - 4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px + w - 4, py + h - 4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px + w - 4, py + h + 12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px + w + 12, py + h + 12);
		glEnd();
		/* Draw BR */
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(px + w + 12, py + 4);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(px + w - 4, py + 4);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(px + w - 4, py - 12);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(px + w + 12, py - 12);
		glEnd();
		glFinish();

		if (bCol != 3)
		{
			/* Draw Background */
			glBindTexture(GL_TEXTURE_2D, layout[3].gltex());
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(px + 4, py + 4);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(px + w - 4, py + 4);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(px + w - 4, py + h - 4);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(px + 4, py + h - 4);
			glEnd();
			glFinish();
		}
	}
	if (TextureID != 0)
	{
		/* Draw Image Texture */
		glColor4f(br, bg, bb, (float) calpha / 255.0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		int hs = (absh / 255.0) * scale;

		if (h > ((miny / 255.0) * scale))
			h = (miny / 255.0) * scale;

		glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		 That is why the TexCoords specify different corners
		 than the Vertex coors seem to. */
		glTexCoord2f(0.0f, (float) ((float) ((float) scrollv + h) / hs));
		glVertex2f(px, py);
		glTexCoord2f(1.0f, (float) ((float) ((float) scrollv + h) / hs));
		glVertex2f(px + w, py);
		glTexCoord2f(1.0f, (float) ((float) scrollv / hs));
		glVertex2f(px + w, py + h);
		glTexCoord2f(0.0f, (float) ((float) scrollv / hs));
		glVertex2f(px, py + h);
		glEnd();
		/* Are we Scrolling? */
		if (hs > h)
		{
			if (layout[4].width() != 0)
			{
				if (bHeaderEnab == false)
				{
					/* Draw Top Fade Line */
					glBindTexture(GL_TEXTURE_2D, layout[4].gltex());
					glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 1.0f);
					glVertex2f(px, py + h - 20);
					glTexCoord2f(1.0f, 1.0f);
					glVertex2f(px + w, py + h - 20);
					glTexCoord2f(1.0f, 0.0f);
					glVertex2f(px + w, py + h);
					glTexCoord2f(0.0f, 0.0f);
					glVertex2f(px, py + h);
					glEnd();
					glFinish();
				}
				/* Draw Lower Fade Line */
				glBindTexture(GL_TEXTURE_2D, layout[4].gltex());
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(px, py + 20);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(px + w, py + 20);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(px + w, py);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(px, py);
				glEnd();
				glFinish();
			}
		}
	}
	if ((bHeaderEnab == true) || (sHeaderEnab == true))
	{
		if (layout[4].width() != 0)
		{
			/* Draw a Header Image */
			if (sHeaderEnab == true)
				glColor4f(br, bg, bb, (float) balpha / 255.0);
			glBindTexture(GL_TEXTURE_2D, layout[4].gltex());
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(px, py + h - 45);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(px + w, py + h - 45);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(px + w, py + h - 25);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(px, py + h - 25);
			glEnd();
			glFinish();

			/* White Box */
			glBegin(GL_QUADS);
			glVertex2f(px, py + h - 25);
			glVertex2f(px + w, py + h - 25);
			glVertex2f(px + w, py + h);
			glVertex2f(px, py + h);
			glEnd();
			glFinish();
		}
	}
}

void Box::destroy_x11_subwindow(Display *dpy, Window parent)
{
	XDestroyWindow(dpy, parent);
}

Window Box::create_x11_subwindow(Display *dpy, Window parent, int x, int y, int width, int height)
{
	Window win;

	if (!dpy)
		return 0;

	win = XCreateWindow(dpy, parent, x, y, width, height, 0, CopyFromParent, CopyFromParent, CopyFromParent, 0, 0);

	if (!win)
		return 0;

	if (!XSelectInput(dpy, win, StructureNotifyMask))
		return 0;

	if (!XMapWindow(dpy, win))
		return 0;

	while (1)
	{
		XEvent e;
		XNextEvent(dpy, &e);
		if (e.type == MapNotify && e.xmap.window == win)
			break;
	}

	XSelectInput(dpy, win, NoEventMask);

	return win;
}

Window Box::create_sdl_x11_subwindow(int x, int y, int width, int height)
{
	SDL_SysWMinfo sdl_info;
	Window play_win;

	sdl_info = get_sdl_wm_info();
	if (!sdl_info.version.major)
		return 0;

	sdl_info.info.x11.lock_func();

	play_win = create_x11_subwindow(sdl_info.info.x11.display, sdl_info.info.x11.window, x, y, width, height);

	sdl_info.info.x11.unlock_func();

	return play_win;
}

SDL_SysWMinfo Box::get_sdl_wm_info(void)
{
	SDL_SysWMinfo sdl_info;

	memset(&sdl_info, 0, sizeof(sdl_info));

	SDL_VERSION(&sdl_info.version);
	if (SDL_GetWMInfo(&sdl_info) <= 0 || sdl_info.subsystem != SDL_SYSWM_X11)
	{
		fprintf(stderr, "This is not X11\n");

		memset(&sdl_info, 0, sizeof(sdl_info));
		return sdl_info;
	}

	return sdl_info;
}

void Box::create_iplayer(const char *streamid, const char *quality, int cache, Window win, FILE **mplayer_fp)
{
	char cmdline[1024];
	sprintf(cmdline,
			"/screen/src/orbital_get_iplayer/get_iplayer --stream --modes=%s --type=livetv %s --player=\"mplayer -really-quiet -vo xv -ao %s -mc 1 -autosync 30 -noconsolecontrols -nokeepaspect -hardframedrop -cache %i -wid 0x%lx -\" > /dev/null 2>&1",
			quality, streamid, audEnable, cache, win);
	if (debugLevel > 1)
		printf("%s\n", cmdline);
	*mplayer_fp = popen(cmdline, "r");
}

void Box::createiPlayer(int maxqual, int width, int height, int x, int y, int scale)
{
	/* MPLAYER Testing */
	/* Default IPlayer Feed - 688x384 */
	ipVis = true;

	int mplayer_t_width = 688;
	int mplayer_t_height = 384;

	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	int tWidth = info->current_w;
	int tHeight = info->current_h;

	y = (720 - y) - ((height / 255.0) * scale);
	int mplayer_pos_y = (y / 720.0) * tHeight;
	int mplayer_pos_x = (x / 1280.0) * tWidth;
	int mplayer_width = (((mplayer_t_width / 1280.0) * tWidth) / 255.0) * scale;
	int mplayer_height = (((mplayer_t_height / 720.0) * tHeight) / 255.0) * scale;
	if (debugLevel > 1)
		printf("Creating X11 Child at %ix%i (%ix%i)\n", mplayer_pos_x, mplayer_pos_y, mplayer_width, mplayer_height);
	play_win = create_sdl_x11_subwindow(mplayer_pos_x, mplayer_pos_y, mplayer_width, mplayer_height);

	if (!play_win)
	{
		fprintf(stderr, "Cannot create X11 window\n");
		ipVis = false;
	}
	else
	{
		mplayer_fp = NULL;
		// Determine Quality Automatically....
		// flashlow   - 400x225 @ 396kbps  (300v, 96a)
		// flashstd   - 640x360 @ 496kbps  (400v, 96a)
		// flashhigh  - 640x360 @ 800kbps  (704v, 96a)
		// flashvhigh - 688x384 @ 1500kbps (1372v, 128a)
		if ((mplayer_width <= 400) || (maxqual <= 1))
		{
			if (ipLFail == 1)
				create_iplayer(bMSRC, "flashlow", 1024, play_win, &mplayer_fp);
			else
				create_iplayer(bMSRC, "flashlow2", 1024, play_win, &mplayer_fp);
		}
		else if (((mplayer_width > 400) && (mplayer_width <= 600)) || (maxqual == 2))
		{
			if (ipLFail == 1)
				create_iplayer(bMSRC, "flashstd", 2048, play_win, &mplayer_fp);
			else
				create_iplayer(bMSRC, "flashstd2", 2048, play_win, &mplayer_fp);
		}
		else if (((mplayer_width > 600) && (mplayer_width <= 800)) || (maxqual == 3))
		{
			if (ipLFail == 1)
				create_iplayer(bMSRC, "flashhigh", 3072, play_win, &mplayer_fp);
			else
				create_iplayer(bMSRC, "flashhigh2", 3072, play_win, &mplayer_fp);
		}
		else if ((mplayer_width > 800) || (maxqual >= 4))
		{
			if (ipLFail == 1)
				create_iplayer(bMSRC, "flashvhigh", 4096, play_win, &mplayer_fp);
			else
				create_iplayer(bMSRC, "flashvhigh2", 4096, play_win, &mplayer_fp);
		}
	}
}

bool Box::FileExists(const char* FileName)
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

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
	tAlpha = 0;
	tSTimer = 0;
	tCScreen = -1;
	sprintf(audEnable, "*");
	sprintf(bUID, "*");
	sprintf(bMSRC, "*");
}

bool Box::doDraw(int aOverride)
{
	/* mplayer/iPlayer check
	 * Streaming Issue will have sType of 2
	 * mPlayer will have sType of 3
	 * iPlayer will have sType of 4+ */

	/* tAlpha is the initial Alpha value when creating a new Box.
	 * All boxes need to fade in, and fade out, thus being clean. */
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
		if ((sType >= 4) &&  (!ipVis))
				createiPlayer(sType - 3, bW, bH, bX, bY, bScale);

		if ((sType >= 4) && (ipVis))
		{
			/* Check for getiPlayer Instance, restart if required. */
			char pgc[1024];
			sprintf(pgc, "ps aux | grep get_iplayer | grep %s | grep -vn grep", bMSRC);
			FILE *fp = popen(pgc, "r");
			char buff[1024];
			if (!fgets(buff, sizeof buff, fp) != NULL)
			{
				/* get_iplayer isn't running - Close current window and set ipVis to false
				 * iPlayer Window will automatically re-launch when closed.
				 * Also, worth cycling through Quality settings, just in case it's an issue
				 * with the stream, as experienced around May 20th 2012. */
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
					/* Remove iPlayer feed, set as Static Image with "Unavailable" feed. */
					sType = 2;
					char bgID[128];
					sprintf(bgID, "/screen/textures/iplayer/%s.png", bUID);
					if (FileExists(bgID) == false)
						sprintf(bgID, "/screen/textures/iplayer/generic_fail.png");
					ipBG.Load(bgID);
					sWidth = ipBG.width();
					sHeight = ipBG.height();
					glTex = ipBG.gltex();
				}
				else
				{
					glTex = 0;
				}

				sdl_info.info.x11.unlock_func();
				ipVis = false;
			}
			pclose(fp);
		}
	}
	else if ((tAlpha == 0) && (sType == -1))
	{
		/* Delete Board */
		printf("    BOX \"%s\" Destroy Event Complete\n", bUID);
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
		sprintf(audEnable, "*");
		sprintf(bUID, "*");
		sprintf(bMSRC, "*");
		if (ipBG.width() != 0)
		{
			printf(" - ipBG");
			ipBG.Destroy();
		}
		if (layout[0].width() != 0)
		{
			printf(" - BoxTex1 ");
			layout[0].Destroy();
		}
		if (layout[1].width() != 0)
		{
			printf(" - BoxTex2 ");
			layout[1].Destroy();
		}
		if (layout[2].width() != 0)
		{
			printf(" - BoxTex3 ");
			layout[2].Destroy();
		}
		if (layout[3].width() != 0)
		{
			printf(" - BoxTex4 ");
			layout[3].Destroy();
		}
	}

	if (aOverride != -1)
		drawInfoBox(glTex, bType, bX, bY, bW, bH, sHeight, 1.0f, 1.0f, 1.0f, bScale, tAlpha, aOverride);
	else
		drawInfoBox(glTex, bType, bX, bY, bW, bH, sHeight, 1.0f, 1.0f, 1.0f, bScale, tAlpha, 255);
	return true;
}

void Box::doUpdate()
{
	/* Update Routine */
}

void Box::Destroy()
{
	printf("    BOX \"%s\" Destroy Event Called\n", bUID);
	if (m_bRunning)
	{
		if ((sType >= 4) && (ipVis))
		{
			printf(" - Destroying iplayer/mplayer reference...\n");
			system("killall -9 mplayer");
			system("killall -9 rtmpdump");
			system("killall -9 get_iplayer");
			char pgc[1024];
			sprintf(pgc, "ps aux | grep get_iplayer | grep %s | grep -vn grep", bMSRC);
			FILE *fp = popen(pgc, "r");
			char buff[1024];
			if (!fgets(buff, sizeof buff, fp) != NULL)
			{
				/* get_iplayer isn't running - Close current window and set ipVis to false
				 * iPlayer Window will automatically re-launch when closed. */
				SDL_SysWMinfo sdl_info;

				sdl_info = get_sdl_wm_info();
				sdl_info.info.x11.lock_func();

				destroy_x11_subwindow(sdl_info.info.x11.display, play_win);

				sdl_info.info.x11.unlock_func();
				ipVis = false;
			}
			pclose(fp);
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
	if (sType == 1)
	{
		sWidth = w;
		sHeight = h;
	}
}

void Box::Create(char btUID[128], char btMSRC[1024], int tStamp, GLuint TextureID, int bcol, int px, int py, int w, int h, int aw, int ah, int scale,
		int sourceType, int dScreen, char dAudio[16])
{
	bType = false;
	if (bcol == 1)
	{
		bType = true;
		layout[0].Load("/screen/textures/orb_bl.png");
		layout[1].Load("/screen/textures/orb_bt.png");
		layout[2].Load("/screen/textures/orb_bcrnr.png");
		layout[3].Load("/screen/textures/orb_boxb.png");
	}
	else if (bcol == 2)
	{
		bType = true;
		layout[0].Load("/screen/textures/orb_wl.png");
		layout[1].Load("/screen/textures/orb_wt.png");
		layout[2].Load("/screen/textures/orb_wcrnr.png");
		layout[3].Load("/screen/textures/orb_boxw.png");
	}
	else if (bcol == 3)
	{
		bType = true;
		layout[0].Load("/screen/textures/orb_tl.png");
		layout[1].Load("/screen/textures/orb_tt.png");
		layout[2].Load("/screen/textures/orb_tcrnr.png");
	}
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
	}
}

void Box::destroy_x11_subwindow(Display *dpy, Window parent)
{
	XDestroyWindow(dpy, parent);
}

Window Box::create_x11_subwindow(Display *dpy, Window parent, int x, int y, int width, int height)
{
	Window win;
	int winbgcol;

	if (!dpy)
		return 0;

	winbgcol = WhitePixel(dpy, DefaultScreen(dpy));

	win = XCreateSimpleWindow(dpy, parent, x, y, width, height, 0, winbgcol, winbgcol);

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
			"/screen/src/orbital_get_iplayer/get_iplayer --stream --modes=%s --type=livetv %s --player=\"mplayer -really-quiet -vo gl2 -ao %s -mc 1 -autosync 30 -noconsolecontrols -nokeepaspect -hardframedrop -cache %i -wid 0x%lx -\"",
			quality, streamid, audEnable, cache, win);
	printf("%s\n", cmdline);
	*mplayer_fp = popen(cmdline, "w");
}

void Box::createiPlayer(int maxqual, int width, int height, int x, int y, int scale)
{
	/* MPLAYER Testing */
	/* Default IPlayer Feed - 688x384 */
	ipVis = true;

	int mplayer_t_width = 688;
	int mplayer_t_height = 384;
	y = (720 - y) - ((height / 255.0) * scale);
	int mplayer_pos_y = (y / 720.0) * sHeight;
	int mplayer_pos_x = (x / 1280.0) * sWidth;
	int mplayer_width = (((mplayer_t_width / 1280.0) * sWidth) / 255.0) * scale;
	int mplayer_height = (((mplayer_t_height / 720.0) * sHeight) / 255.0) * scale;
	printf("Creating X11 Child at %ix%i (%ix%i)\n", mplayer_pos_y, mplayer_pos_y, mplayer_width, mplayer_height);
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

/*
 * main.cpp
 *
 *  Created on: 28 Feb 2012
 *      Author: pwollaston
 */

#include "main.h"
#include "signage.h"

int main(int argc, char* argv[])
{
	srand(time(NULL)); /* Randomise Time Seed */
	Signage signage;

	bool isFullScreen = false;

	if (FileExists("/screen/full"))
		isFullScreen = true;

	signage.Init("Digital Signage", 1280, 720, 32, isFullScreen);

	while (signage.Running())
	{
		signage.HandleEvents(&signage);
		signage.Update();
		signage.Draw();
	}

	/* Cleanup Engine */
	signage.Clean();

	/* Sanity */
	while (signage.Running())
	{
		signage.Clean();
		signage.Update();
		signage.Draw();
	}

	printf("OrbitalDigitalSignage has cleanly exited.\n");

	return 0;
}

bool FileExists(const char* FileName)
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

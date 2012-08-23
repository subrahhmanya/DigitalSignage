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

	printf("Loading Configuration....\n");
	/* Check for valid Configuration Files and Contents */
	char tFName[128];
	sprintf(tFName, "/screen/config.ini");
	if (FileExists(tFName))
	{
		/* Found a config gile, parse it... */
		CIniFile ini;
		ini.Load(tFName);
		CIniSection* pSection = ini.GetSection("GeneralSettings");
		if (pSection)
		{
			if ((pSection->GetKey("Enabled")) && (atoi(ini.GetKeyValue("GeneralSettings", "Enabled").c_str()) == 1))
			{
				/* Enabled - Get Configs! */
				bool tFS = false;
				int dbgLvl;
				int tWX, tWH, tBPP;
				char tMID[256];
				char tHeader[256];
				char tWLoc[64];
				char tWCountry[64];
				char tWAPI[64];
				if (atoi(ini.GetKeyValue("GeneralSettings", "FullScreen").c_str()) == 1)
					tFS = true;
				tWX = atoi(ini.GetKeyValue("GeneralSettings", "SWidth").c_str());
				tWH = atoi(ini.GetKeyValue("GeneralSettings", "SHeight").c_str());
				tBPP = atoi(ini.GetKeyValue("GeneralSettings", "BPP").c_str());
				dbgLvl = atoi(ini.GetKeyValue("GeneralSettings", "DebugLevel").c_str());
				sprintf(tMID, "%s", ini.GetKeyValue("GeneralSettings", "MID").c_str());
				sprintf(tHeader, "%s", ini.GetKeyValue("GeneralSettings", "Header").c_str());
				sprintf(tWLoc, "%s", ini.GetKeyValue("GeneralSettings", "WeatherLoc").c_str());
				sprintf(tWCountry, "%s", ini.GetKeyValue("GeneralSettings", "WeatherCountry").c_str());
				sprintf(tWAPI, "%s", ini.GetKeyValue("GeneralSettings", "WeatherAPI").c_str());

				/* Load Version String */
				char tVS[64];
				ifstream vFile("/screen/src/orbital_infodisplay/version");
				string line;
				getline(vFile, line);
				sprintf(tVS, "Client Version %s", line.c_str());

				signage.Init(tMID, tWX, tWH, tBPP, tFS, tHeader, tWLoc, tWCountry, tWAPI, tVS, dbgLvl);

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
					if (signage.OKToQuit() == false)
					{
						signage.Update();
						signage.Draw();
					}
				}
			}
			else
			{
				printf("\n\nUnable to Launch.\nPlease ensure that /screen/config.ini exists, and that the 'Enabled' option is set to '1'.\n");
			}
		}
	}
	printf("\n\nOrbitalDigitalSignage has cleanly exited.\n");
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

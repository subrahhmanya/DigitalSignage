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

	printf("Loading Configuration....");
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
			CIniKey* pKey;
			if (pSection->GetKey("Enabled"))
			{
				if (atoi(ini.GetKeyValue("GeneralSettings", "Enabled").c_str()) == 1)
				{
					/* Enabled - Get Configs! */
					bool tFS = false;
					int tWX, tWH, tBPP;
					char tMID[256];
					if (atoi(ini.GetKeyValue("GeneralSettings", "FullScreen").c_str()) == 1)
						tFS = true;
					tWX = atoi(ini.GetKeyValue("GeneralSettings", "SWidth").c_str());
					tWH = atoi(ini.GetKeyValue("GeneralSettings", "SHeight").c_str());
					tBPP = atoi(ini.GetKeyValue("GeneralSettings", "BPP").c_str());
					sprintf(tMID, "%s", ini.GetKeyValue("GeneralSettings", "MID").c_str());

					signage.Init(tMID, tWX, tWH, tBPP, tFS);

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
				}
			}
		}
	}
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

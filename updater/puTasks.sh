#!/bin/bash
#Build things
# Static Variables
	logName="[Info-Updater]"
	scrroot="/screen/"
	srcroot="${scrroot}src/orbital_infodisplay/"

function logOut {
	echo "$logName $(date +%s): $1"
}

#We're updating - kill previous infodisplay
logOut "Killing existing infodisplay process..."
killall -9 infodisplay > /dev/null 2>&1

#Create Directory Structure (if not already done)
logOut "Creating structure for Boards..."
mkdir -p ${scrroot}boards/1 > /dev/null 2>&1
mkdir -p ${scrroot}boards/2 > /dev/null 2>&1
mkdir -p ${scrroot}boards/3 > /dev/null 2>&1

#Delete old binary and associated files...
logOut "Removing old infodisplay files..."
rm ${scrroot}infodisplay > /dev/null 2>&1
rm ${srcroot}infodisplay > /dev/null 2>&1
rm ${scrroot}runcheck.sh > /dev/null 2>&1
rm -rf ${scrroot}fonts > /dev/null 2>&1
rm -rf ${scrroot}textures > /dev/null 2>&1

#Compile infodisplay
logOut "Compiling InfoDisplay..."
cd ${srcroot}Debug/ > /dev/null 2>&1
make > /dev/null 2>&1

#Move new binary and associated files...
logOut "Copying new infodisplay files..."
cp ${srcroot}Debug/OrbitalDigitalSignage ${scrroot}infodisplay > /dev/null 2>&1
rm ${srcroot}Debug/OrbitalDigitalSignage > /dev/null 2>&1
cp ${srcroot}updater/runcheck.sh ${scrroot} > /dev/null 2>&1
cp -a fonts ${scrroot} > /dev/null 2>&1
cp -a textures ${scrroot} > /dev/null 2>&1

# Load Application
logOut "Launching InfoDisplay..."
cd ${scrroot}
env DISPLAY=:0 ${scrroot}infodisplay &

logOut "Update Complete!"

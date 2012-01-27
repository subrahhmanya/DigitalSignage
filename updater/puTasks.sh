#!/bin/bash
#Build things
# Static Variables
	logName="[InfoUpdater]"
	scrroot="/screen/"
	srcroot="${scrroot}src/orbital_infodisplay/"

function logOut {
	echo "$logName $(date +%s): $1"
}

#We're updating - kill previous infodisplay
killall -9 infodisplay > /dev/null 2>&1

#Compile infodisplay
logOut "Compiling InfoDisplay..."
cd ${srcroot} > /dev/null 2>&1
make > /dev/null 2>&1

#Create Directory Structure (if not already done)
mkdir -p ${scrroot}boards/1
mkdir -p ${scrroot}boards/2
mkdir -p ${scrroot}boards/3

#Move new binary and associated files...
rm ${scrroot}infodisplay
cp infodisplay ${scrroot} > /dev/null 2>&1
rm infodisplay > /dev/null 2>&1
rm -rf ${scrroot}fonts
rm -rf ${scrroot}textures
cp -a fonts ${scrroot}
cp -a textures ${scrroot}
cp ${srcroot}updater/runcheck.sh ${scrroot} > /dev/null 2>&1

# Load Application
logOut "Launching InfoDisplay..."
cd ${scrroot}
env DISPLAY=:0 ${scrroot}infodisplay &

logOut "Update Complete!"

#!/bin/bash
#Build things
# Static Variables
        logName="[InfoUpdater]"
        verUrl="https://raw.github.com/PaulW/infodisplay/master/version"
        tmcegitUrl="git://github.com/PaulW/tinymce.git"
        scrroot="/screen/"
        srcroot="${scrroot}src/infodisplay/"
	webroot="/var/www/"

function logOut {
        echo "$logName $(date +%s): $1"
}

#Let's compile some required binaries.  We do this every time, just incase.

#Compile CutyCapt
if [ -f ${scrroot}CutyCapt ]
then
	logOut "Skipping CutyCapt Compilation - already latest."
else
	logOut "Compiling CutyCapt..."
	cd ${srcroot}ext-libs/CutyCapt > /dev/null 2>&1
	qmake > /dev/null 2>&1
	make > /dev/null 2>&1
	cp CutyCapt ${scrroot} > /dev/null 2>&1
	rm CutyCapt > /dev/null 2>&1
fi

#Compile and install tinymce
logOut "Getting TinyMCE..."
cd ${scrroot}
if [ ! -d tinymce ] ; then
	#No TinyMCE Source
	git clone ${tmcegitUrl} > /dev/null 2>&1
else
	#Update TinyMCE
	mkdir tinymce > /dev/null 2>&1
	cd tinymce > /dev/null 2>&1
	git pull > /dev/null 2>&1
fi
logOut "Building TinyMCE..."
cd ${scrroot}/tinymce > /dev/null 2>&1
ant > /dev/null 2>&1
cp jscripts ${webroot} > /dev/null 2>&1

#We're updating - kill previous infodisplay
killall -9 infodisplay > /dev/null 2>&1

#Compile infodisplay
logOut "Compiling InfoDisplay..."
cd ${srcroot}src > /dev/null 2>&1
make > /dev/null 2>&1

#Create Directory Structure (if not already done)
mkdir -p ${scrroot}boards/1
mkdir -p ${scrroot}boards/2
mkdir -p ${scrroot}boards/3

#Move new binary
rm ${scrroot}infodisplay
cp infodisplay ${scrroot} > /dev/null 2>&1
rm infodisplay > /dev/null 2>&1
cp -a fonts ${scrroot}
cp -a textures ${scrroot}

# Load Application
logOut "Launching InfoDisplay..."
cd ${scrroot}
env DISPLAY=:0 ${scrroot}infodisplay &

logOut "Complete!"

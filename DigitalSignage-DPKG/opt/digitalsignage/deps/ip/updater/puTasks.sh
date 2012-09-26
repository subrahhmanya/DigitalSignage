#!/bin/bash
#Build things
# Static Variables
	logName="[getIplayer-Updater]"
	scrroot="/screen/"
	srcroot="${scrroot}src/orbital_get_iplayer/"

function logOut {
	echo "$logName $(date +%s): $1"
}

#We're updating - kill previous infodisplay
logOut "Nothing to do yet!"

#!/bin/bash

# Static Variables
	logName="[InfoUpdater]"
	verUrl="https://raw.github.com/PaulW/infodisplay/master/version"
	gitUrl="git://github.com/PaulW/infodisplay.git"
	scrroot="/screen/"
	srcroot="${scrroot}src/infodisplay/"

function logOut {
	echo "$logName $(date +%s): $1"
}

function CheckForUpdate {
	if [ ! -f ${srcroot}version ] ; then
		#No current version installed.
		echo 1
	else
		#We have something installed, check if same via MD5
		wget "$verUrl" -O ${scrroot}version > /dev/null 2>&1
		overmd5="$(cat ${srcroot}version | md5sum)"
		nvermd5="$(cat ${scrroot}version | md5sum)"
		overmd5=${overmd5:0:32}
		nvermd5=${nvermd5:0:32}
		if [ "$overmd5" == "$nvermd5" ] ; then
			#Same Version
                        over="$(cat ${srcroot}version)"
                        nver="$(cat ${scrroot}version)"
			echo 0
		else
			#New Version
			over="$(cat ${srcroot}version)"
			nver="$(cat ${scrroot}version)"
			echo 1
		fi
	fi
}

function GrabUpdate {
	#We need to pull down the new version...  We use git for this!
	if [ ! -d $srcroot ] ; then
		#No source exists, was it deleted? Corrupted? We need to pull...
		mkdir -p ${scrroot}/src
		cd ${scrroot}/src
		git clone ${gitUrl} > /dev/null 2>&1
	else
		#Folder exists, do update.
		cd ${srcroot}
		git pull > /dev/null 2>&1
	fi
}

logOut "Checking for update..."
if [ $(CheckForUpdate) == "1" ] ; then
	#Updater returned an update available
	logOut "Update is Available... Fetching"
	GrabUpdate
	#Check to make sure we have current...
	if [ $(CheckForUpdate) == "0" ]; then
		logOut "Updated to version $(cat ${srcroot}version)"
		#Now we need to build and whatnot...
	else
		logOut "Failed to update."
		exit 0
	fi
else
	#No update available
	logOut "Update is not available.  Current version is $(cat ${srcroot}version)"
fi
echo $over

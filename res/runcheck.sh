#!/bin/bash
# Run
# Static Variables
	scrroot="/opt/digitalsignage/"

if [ ! $(pidof DigitalSignage) ] ; then
	# Kill any other instance of mplayer/rtmpdump/get_iplayer just incase already running.

	if ps ax | grep -v "grep" | grep "mplayer" > /dev/null; then
	        killall -9 mplayer > /dev/null 2>&1
	fi

	if ps ax | grep -v "grep" | grep "rtmpdump" > /dev/null; then
	        killall -9 rtmpdump > /dev/null 2>&1
	fi

	if ps ax | grep -v "grep" | grep "get_iplayer" > /dev/null; then
	        killall -9 get_iplayer > /dev/null 2>&1
	fi
	cd ${scrroot}
        # Clean out getiPlayer Prefs.
        ${scrroot}deps/ip/get_iplayer --prefs-clear > /dev/null 2>&1
        ${scrroot}deps/ip/get_iplayer --refresh > /dev/null 2>&1
	env DISPLAY=:0 ${scrroot}DigitalSignage & > /dev/null 2>&1
fi

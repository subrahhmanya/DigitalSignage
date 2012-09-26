#!/bin/bash
#Build things
# Static Variables
	scrroot="/opt/digitalsignage/"

if [ ! $(pidof DigitalSignage) ] ; then
	# Kill any other instance of mplayer/rtmpdump/get_iplayer just incase already running.
	killall -9 mplayer > /dev/null 2>&1
	killall -9 rtmpdump > /dev/null 2>&1
	killall -9 get_iplayer > /dev/null 2>&1
	cd ${scrroot}
        # Clean out getiPlayer Prefs.
        ${scrroot}deps/ip/get_iplayer --prefs-clear > /dev/null 2>&1
        ${scrroot}deps/ip/get_iplayer --refresh > /dev/null 2>&1
	env DISPLAY=:0 ${scrroot}DigitalSignage & > /dev/null 2>&1
fi

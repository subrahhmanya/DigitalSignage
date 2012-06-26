#!/bin/bash
#Build things
# Static Variables
	scrroot="/screen/"

if [ ! $(pidof infodisplay) ] ; then
	# Kill any other instance of mplayer/rtmpdump/get_iplayer just incase already running.
	killall -9 mplayer > /dev/null 2>&1
	killall -9 rtmpdump > /dev/null 2>&1
	killall -9 get_iplayer > /dev/null 2>&1
	cd ${scrroot}
	env DISPLAY=:0 ${scrroot}infodisplay &
fi


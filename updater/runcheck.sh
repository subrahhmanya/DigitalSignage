#!/bin/bash
#Build things
# Static Variables
	scrroot="/screen/"

if [ ! $(pidof infodisplay) ] ; then
	cd ${scrroot}
	env DISPLAY=:0 ${scrroot}infodisplay &
fi


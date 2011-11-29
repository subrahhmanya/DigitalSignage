#!/bin/bash
#Build things
# Static Variables
        scrroot="/screen/"

if [ ! $(pidof infodisplay) ] ; then
	env DISPLAY=:0 ${scrroot}infodisplay &
fi

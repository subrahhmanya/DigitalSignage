#!/bin/bash
#Build things
# Static Variables
        scrroot="/screen/"

if [ ! -z "$(w -h screen)" ] ; then
	if [ ! $(pidof infodisplay) ] ; then
		cd ${scrroot}
		env DISPLAY=:0 ${scrroot}infodisplay &
	fi
fi

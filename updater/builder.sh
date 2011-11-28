#!/bin/bash
#Build things
# Static Variables
        logName="[InfoUpdater]"
        verUrl="https://raw.github.com/PaulW/infodisplay/master/version"
        gitUrl="git://github.com/PaulW/infodisplay.git"
        scrroot="/screen/"
        srcroot="${scrroot}src/infodisplay/"

function logOut {
        echo "$logName $(date +%s): $1"
}

#Let's compile some required binaries.  We do this every time, just incase.
#Compile FTGL
logOut "Compiling and installing FTGL..."
cd ${srcroot}ext-libs/ftgl > /dev/null 2>&1
./autogen.sh > /dev/null 2>&1
./configure > /dev/null 2>&1
make > /dev/null 2>&1
sudo make install > /dev/null 2>&1

# To call new screen:
# env DISPLAY=:0 command &

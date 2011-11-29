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
#logOut "Compiling and installing FTGL..."
#cd ${srcroot}ext-libs/ftgl > /dev/null 2>&1
#./autogen.sh > /dev/null 2>&1
#./configure > /dev/null 2>&1
#make > /dev/null 2>&1
#sudo make install > /dev/null 2>&1
logOut "Skipping FTGL Compilation - already installed."

#Compile CutyCapt
logOut "Compiling CutyCapt..."
cd ${srcroot}ext-libs/CutyCapt
qmake
make
cp CutyCapt ${scrroot}
rm CutyCapt

#Compile infodisplay
cd ${srcroot}src
make
killall infodisplay
cp infodisplay ${scrroot}
rm infodisplay

# Load Application
env DISPLAY=:0 ${scrroot}infodisplay &

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

#We're updating - kill previous infodisplay
killall infodisplay > /dev/null 2>&1

#Let's compile some required binaries.  We do this every time, just incase.
#Compile FTGL
logOut "Compiling and installing FTGL..."
cd ${srcroot}ext-libs/ftgl > /dev/null 2>&1
./autogen.sh > /dev/null 2>&1
./configure > /dev/null 2>&1
make > /dev/null 2>&1
sudo make install > /dev/null 2>&1
#logOut "Skipping FTGL Compilation - already installed."

#Compile CutyCapt
logOut "Compiling CutyCapt..."
cd ${srcroot}ext-libs/CutyCapt > /dev/null 2>&1
qmake > /dev/null 2>&1
make > /dev/null 2>&1
cp CutyCapt ${scrroot} > /dev/null 2>&1
rm CutyCapt > /dev/null 2>&1

#Compile infodisplay
logOut "Compiling InfoDisplay..."
cd ${srcroot}src > /dev/null 2>&1
make > /dev/null 2>&1
cp infodisplay ${scrroot} > /dev/null 2>&1
rm infodisplay > /dev/null 2>&1
cp -a fonts ${scrroot}
cp -a textures ${scrroot}

# Load Application
logOut "Launching InfoDisplay..."
env DISPLAY=:0 ${scrroot}infodisplay &

logOut "Complete!"

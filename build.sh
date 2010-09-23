#!/bin/sh

VER=""
if [ "$1" != "" ]; then
	VER="_"$1
fi

NMDIR=`pwd`

echo "=========================="
echo "NSL_MODULES$VER"
echo "=========================="


rm -f bin/*.dll


cd OpenSim.NSLModules$VER
./runprebuild.sh
nant clean
nant
cd $NMDIR


# OS Profile
PROFDIR="N"
PROFMOD="OpenSimProfile.Modules.dll"

if [ -d ../opensim.osprofile ]; then
	cd ../opensim.osprofile
	PROFDIR="Y"
elif [ -d ../osprofile ]; then
	cd ../osprofile
	PROFDIR="Y"
fi

if [ "$PROFDIR" = "Y" ]; then
	if [ ! -f bin/$PROFMOD ]; then
		./build.sh
	fi
	cp bin/$PROFMOD $NMDIR/bin
	cd $NMDIR
fi


# OS Search
SRCHDIR="N"
SRCHMOD="OpenSimSearch.Modules.dll"

if [ -d ../opensim.ossearch ]; then
	cd ../opensim.ossearch
	SRCHDIR="Y"
elif [ -d ../ossearch ]; then
	cd ../ossearch
	SRCHDIR="Y"
fi

if [ $SRCHDIR = "Y" ]; then
	if [ ! -f bin/$SRCHMOD ]; then
		./build.sh
	fi
	cp bin/$SRCHMOD $NMDIR/bin
	cd $NMDIR
fi


cp -f bin/*.dll ../bin
ls -l bin/*.dll
echo


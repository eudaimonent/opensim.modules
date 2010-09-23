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
if [ -d ../opensim.osprofile ]; then
	cd ../opensim.osprofile
	./build.sh
	cp bin/*.dll $NMDIR/bin
	cd $NMDIR
elif [ -d ../osprofile ]; then
	cd ../osprofile
	./build.sh
	cp bin/*.dll $NMDIR/bin
	cd $NMDIR
fi

pwd

# OS Search
if [ -d ../opensim.ossearch ]; then
	cd ../opensim.ossearch
	./build.sh
	cp bin/*.dll $NMDIR/bin
	cd $NMDIR
elif [ -d ../ossearch ]; then
	cd ../ossearch
	./build.sh
	cp bin/*.dll $NMDIR/bin
	cd $NMDIR
fi


ls -l bin/*.dll
cp  -f bin/*.dll ../bin/


#!/bin/sh

VER=$1
DVER=""
if [ "$VER" != "" ]; then
	DVER="_"$VER
fi


echo "=========================="
echo "NSL_MODULES$DVER"
echo "=========================="


BASEDIR=OpenSim.Modules
NMDIR=`pwd`
rm -f bin/*.dll


# MuteList
cd $BASEDIR/Messaging
./build.sh $VER
cd ../..


# OS Profile
cd $BASEDIR/Profile
./build.sh $VER
cd ../..


# OS Search
cd $BASEDIR/Search
./build.sh $VER
cd ../..


# Physics
cd $BASEDIR/Physics
#./build.sh $VER
cd ../..


#
#
#
echo


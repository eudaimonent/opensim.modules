#!/bin/sh

VER=$1
DVER=""
if [ "$VER" != "" ]; then
	DVER="_"$VER
fi


echo "=========================="
echo "NSL_MODULES$DVER"
echo "=========================="


rm -f bin/*.dll


# MuteList
cd Messaging
./build.sh $VER
cd ../..


# OS Profile
cd Profile
./build.sh $VER
cd ../..


# OS Search
cd Search
./build.sh $VER
cd ../..


# Physics
cd Physics
#./build.sh $VER
cd ../..


#
#
#
echo


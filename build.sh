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
./build.sh $VER || exit 1
cd ..

# OS Profile
cd Profile
./build.sh $VER || exit 1
cd ..

# OS Search
cd Search
./build.sh $VER || exit 1
cd ..

# Physics
cd Physics
#./build.sh $VER || exit 1
cd ..

# World
cd World
./build.sh $VER || exit 1
cd ..

#
cp bin/*.dll ../bin || exit 1

#
#
echo


#!/bin/sh

VER=""
if [ "$1" != "" ]; then
	VER="_"$1
fi

echo "=========================="
echo "MuteList$VER"
echo "=========================="

./clean.sh

cd MuteList$VER
./runprebuild.sh
nant clean
nant

if [ -f ../bin/Messaging.NSLMuteList.dll ]; then
    cp -f ../bin/Messaging.NSLMuteList.dll ../../../bin
fi

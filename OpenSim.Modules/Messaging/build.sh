#!/bin/sh

DLL=Messaging.NSLMuteList.dll

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

cp -f ../bin/$DLL ../../../bin

echo


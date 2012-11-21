#!/bin/sh

VER=""
if [ "$1" != "" ]; then
	VER="_"$1
fi

echo "=========================="
echo "MuteList$VER"
echo "=========================="

cd MuteList$VER
./runprebuild.sh
nant clean
nant


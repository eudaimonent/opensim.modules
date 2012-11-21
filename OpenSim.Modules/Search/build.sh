#!/bin/sh

VER=""
if [ "$1" != "" ]; then
	VER="_"$1
fi

echo "=========================="
echo "OpenSimSearch$VER"
echo "=========================="

./clean

cd OpenSimSearch$VER
./runprebuild.sh
nant clean
nant


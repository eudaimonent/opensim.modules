#!/bin/sh

VER=""
if [ "$1" != "" ]; then
	VER="_"$1
fi

echo "=========================="
echo "OpenSimProfile$VER"
echo "=========================="

./clean

cd OpenSimProfile$VER
./runprebuild.sh
nant clean
nant


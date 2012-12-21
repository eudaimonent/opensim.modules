#!/bin/sh

DLL=OpenSimProfile.Modules.dll

VER=""
if [ "$1" != "" ]; then
	VER="_"$1
fi

echo "=========================="
echo "OpenSimProfile$VER"
echo "=========================="

./clean.sh

cd OpenSimProfile$VER
./runprebuild.sh
nant clean
nant

cp -f ../../bin/$DLL ../../../../bin || exit 1

echo


#!/bin/sh

DLL=OpenSimSearch.Modules.dll

VER=""
if [ "$1" != "" ]; then
	VER="_"$1
fi

echo "=========================="
echo "OpenSimSearch$VER"
echo "=========================="

./clean.sh

cd OpenSimSearch$VER
./runprebuild.sh
xbuild

cp -f ../bin/$DLL ../../bin || exit 1

echo

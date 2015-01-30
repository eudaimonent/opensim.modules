#!/bin/sh

DLL="SimpleFluidSolverWind.dll"

VER=""
if [ "$1" != "" ]; then
	VER="_"$1
fi

echo "=========================="
echo "SFS_Wind$VER"
echo "=========================="

./clean.sh

cd SFS_Wind$VER
./runprebuild.sh
xbuild
cd ..

cd sfsw
make
cp -f libsfsw.so ../bin

cd ..
cp -f ./bin/$DLL ../../bin || exit 1
cp -f ./bin/libsfsw.so ../../bin || exit 1
cp -f conf/sfsw.dll.config ../../bin || exit 1

echo

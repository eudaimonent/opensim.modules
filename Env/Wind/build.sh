#!/bin/sh

./clean.sh
./runprebuild.sh
xbuild

#cp -f ../bin/$DLL ../../bin || exit 1

echo
cd sfsw
make clean
make
cp -f libsfsw.so ../../bin
cd ..

echo
cp -f sfsw.dll.config ../bin


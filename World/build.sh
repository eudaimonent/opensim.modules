#!/bin/sh

DLL=OpenSimProfile.Modules.dll

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

cp -f ../bin/$DLL ../../bin || exit 1

echo

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


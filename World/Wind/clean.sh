#!/bin/sh

find . -name "*~"|xargs rm -f 

rm -rf ../bin/*
rm -f  OpenSim.sln
rm -f  *.build
rm -f  Modules/*.build
rm -f  Modules/*.csproj*
rm -rf Modules/bin
rm -rf Modules/obj
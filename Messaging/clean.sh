#!/bin/sh

find . -name "*~"|xargs rm -f

rm -f  MuteList*/*.build
rm -f  MuteList*/Modules/*.build
rm -rf MuteList*/Modules/bin

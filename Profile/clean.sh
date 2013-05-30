#!/bin/sh

find . -name "*~"|xargs rm -f

rm -f  OpenSimProfile*/*.build
rm -f  OpenSimProfile*/Modules/*.build
rm -rf OpenSimProfile*/Modules/bin

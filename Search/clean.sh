#!/bin/sh

find . -name "*~"|xargs rm -f 

rm -f  OpenSimSearch*/*.build
rm -f  OpenSimSearch*/Modules/*.build
rm -rf OpenSimSearch*/Modules/bin

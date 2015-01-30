#!/bin/sh


find . -name "*~"|xargs rm -f 

rm -rf ./bin/*

(cd sfsw_v2; make clean >/devnull 2>&1)
(cd sfsw_v3; make clean >/devnull 2>&1)

(cd SFS_Wind_0.8.1 && ./clean.sh)

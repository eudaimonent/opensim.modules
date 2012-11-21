#!/bin/sh

find . -name "*~"|xargs rm -f 

rm -f bin/*.dll

BASEDIR=OpenSim.Modules

(cd $BASEDIR/Messaging && ./clean)
(cd $BASEDIR/Profile && ./clean)
(cd $BASEDIR/Search && ./clean)
(cd $BASEDIR/Physics && ./clean)

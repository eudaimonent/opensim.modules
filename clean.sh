#!/bin/sh

find . -name "*~"|xargs rm -f 

rm -f bin/*.dll

(cd OpenSim.Modules/Messaging && ./clean)
(cd OpenSim.Modules/Profile && ./clean)
(cd OpenSim.Modules/Search && ./clean)
#(cd OpenSim.Modules/Physics && ./clean)

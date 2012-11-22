#!/bin/sh

find . -name "*~"|xargs rm -f 

rm -f bin/*.dll


(cd Messaging && ./clean.sh)
(cd Profile && ./clean.sh)
(cd Search && ./clean.sh)
(cd Physics && ./clean.sh)

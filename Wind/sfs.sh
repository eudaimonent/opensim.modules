gcc sfs.c -fPIC -W -Wall -I/usr/local/include -c
gcc sfs.o -L/usr/local/lib -lrfftw -lfftw -shared -O2 -o sfs.so

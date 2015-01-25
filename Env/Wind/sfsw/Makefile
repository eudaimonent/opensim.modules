

all: libsfsw.so

libsfsw.so: sfsw.o
	gcc sfsw.o -L/usr/local/lib -lrfftw -lfftw -shared -O2 -o libsfsw.so
	
sfsw.o: sfsw.c 
	gcc sfsw.c -fPIC -W -Wall -I/usr/local/include -c


clean:
	rm -f *.o *.so


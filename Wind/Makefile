

all: sfs.so

sfs.so: sfs.o
	gcc sfs.o -L/usr/local/lib -lrfftw -lfftw -shared -O2 -o sfs.so
	
sfs.o: sfs.c 
	gcc sfs.c -fPIC -W -Wall -I/usr/local/include -c


clean:
	rm *.o *.so


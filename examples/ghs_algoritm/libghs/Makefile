libghs.a: ghs.o
	msp430-ar -cvq libghs.a ghs.o

ghs.o: ghs.h ghs.c
	msp430-gcc -mmcu=msp430f1611 -c ghs.c -o ghs.o

clean:
	rm -f ghs.o
	rm -f libghs.a

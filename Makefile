all: readAkai writeAkai

readAkai: readAkai.o akai_tools.o akai_aux.o
	cc -Wall -o readAkai readAkai.o akai_tools.o akai_aux.o -l sndfile -l m

writeAkai: writeAkai.o akai_tools.o akai_aux.o
	cc -Wall -o writeAkai writeAkai.o akai_tools.o akai_aux.o -l sndfile -l m

readAkai.o: readAkai.c akai_tools.h
	cc -Wall -c  readAkai.c

akai_tools.o: akai_tools.c akai_tools.h
	cc -Wall -c  akai_tools.c

akai_aux.o: akai_aux.c akai_aux.h
	cc -Wall -c  akai_aux.c

clean: 
	rm *.o
	rm readAkai
	rm writeAkai

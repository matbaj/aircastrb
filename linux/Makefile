CFLAGS = -g -Wall -fPIC -lX11 -ljpeg -O2
CC = g++

all: libscreenshot_linux 

libscreenshot_linux:  screenshot_linux.o
	${CC} ${CFLAGS} -shared -o $@.so screenshot_linux.o 

.cpp.o:
	${CC} ${CFLAGS} -c $<
 
.c.o:
	${CC} ${CFLAGS} -c $< 

clean:
	rm *.o libscreenshot_linux.so




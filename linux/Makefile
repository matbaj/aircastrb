CFLAGS = -g -Wall -fPIC -lX11 -lXext -lturbojpeg -O2
CC = g++

all: libscreenshot_linux 

libscreenshot_linux:  screenshot_linux.o
	${CC}  -shared -o $@.so screenshot_linux.o ${CFLAGS}

.cpp.o:
	${CC}  -c $< ${CFLAGS}
 
.c.o:
	${CC} -c $<  ${CFLAGS}

clean:
	rm *.o libscreenshot_linux.so




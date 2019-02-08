CFLAGS := -Wall -g 
#INC = -I /home/hardyk/buildroot/output/host/usr/bin/
PATH := $(PATH):/home/hardyk/buildroot/output/host/usr/bin/	

#CC := gcc
#CC := arm-cortex_a8-linux-gnueabihf-gcc
CC := arm-buildroot-linux-uclibcgnueabihf-gcc
#CC := arm-linux-gcc


CFILES = file_io.c
OBJS= ${CFILES:.c=.o}

all: fileio_pkg

fileio_pkg: fileio_pkg.o
	$(CC) $(CFLAGS)  $(OBJS) -o $@

fileio_pkg.o: file_io.c
	$(CC) $(CFLAGS)  file_io.c -c 

clean:
	-rm -f *.o *.d *.x
	-rm -f fileio_pkg
CC = gcc
#CC = arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS = -O0 -g -W

CFILES = pthreads.c
OBJS = ${CFILES:.c=.o}

all: pthreads

pthreads: pthreads.o
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread

.c.o:
	$(CC) $(CFLAGS) -c $<

run:
	./pthreads $(fileName)

clean:
	-rm -f *.o
	-rm -f pthreads
	-rm -f log*.txt
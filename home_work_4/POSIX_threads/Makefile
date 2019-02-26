#CC = gcc
CC = arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS = -g -W
PATH := $(PATH):/home/hardyk/buildroot/output/host/usr/bin
.PHONY = clean 

CFILES = pthreads.c
OBJS = ${CFILES:.c=.o}

all: pthreads

pthreads: pthreads.o
	@echo "Invoking compiler $(CC)"
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread -lrt

.c.o:
	@echo "Building file" $@ "using" $<
	$(CC) $(CFLAGS) -c $<

run:
	./pthreads $(fileName)

clean:
	@echo "deleting object file"
	-rm -f *.o
	@echo "deleting executable file"
	-rm -f pthreads
	@echo "deleting logfile.txt"
	-rm -f log*.txt
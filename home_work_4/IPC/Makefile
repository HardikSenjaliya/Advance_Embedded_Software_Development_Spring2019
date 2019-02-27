CC = gcc
#CC = arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS = -g -W
PATH := $(PATH):/home/hardyk/buildroot/output/host/usr/bin
.PHONY = clean 

CFILES = ipc_pipe.c
OBJS = ${CFILES:.c=.o}

all: ipc_pipe

ipc_pipe: ipc_pipe.o
	@echo "Invoking compiler $(CC)"
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread -lrt

.c.o:
	@echo "Building file" $@ "using" $<
	$(CC) $(CFLAGS) -c $<

clean:
	@echo "deleting object file"
	-rm -f *.o
	@echo "deleting executable file"
	-rm -f ipc_pipe
	@echo "deleting logfile.txt"
	-rm -f log*.txt
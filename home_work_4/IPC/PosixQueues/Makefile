#CC=gcc
CC = arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS = -g -W
PATH := $(PATH):/home/hardyk/buildroot/output/host/usr/bin


all:
	@echo "Building target"
	gcc process1.c -o process1 -lrt
	gcc process2.c -o process2 -lrt
	$(CC) Q_Threads.c -o Q_Threads -lrt -lpthread
	@echo "Done."

.PHONY:clean

clean:
	rm -rf process1 process2 logfile.txt
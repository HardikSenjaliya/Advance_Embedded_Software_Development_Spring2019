
#CC = arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS = -g -W
PATH := $(PATH):/home/hardyk/buildroot/output/host/usr/bin

CC=gcc

all:
	@echo "Bulding target"
	$(CC) server.c -o server
	$(CC) client.c -o client
	@echo "Done"

clean:
	rm -rf server client

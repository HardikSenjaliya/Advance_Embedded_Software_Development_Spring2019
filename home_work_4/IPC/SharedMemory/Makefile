#CC = arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS = -g -W
PATH := $(PATH):/home/hardyk/buildroot/output/host/usr/bin


all:
	gcc producer.c -o producer -lrt -pthread
	gcc consumer.c -o consumer -lrt -pthread

clean:
	rm -rf producer consumer

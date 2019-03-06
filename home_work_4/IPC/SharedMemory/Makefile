CC = arm-buildroot-linux-uclibcgnueabihf-gcc
#CFLAGS = -g -W
PATH := $(PATH):/home/hardyk/buildroot/output/host/usr/bin


all:
	$(CC) producer.c -o producer -lrt -pthread
	$(CC) consumer.c -o consumer -lrt -pthread

clean:
	rm -rf producer consumer

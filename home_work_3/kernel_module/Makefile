LINUXDIR := /home/hardyk/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220
PATH := $(PATH):/home/hardyk/buildroot/output/host/usr/bin


obj-m := my_module.o

all:
	make ARCH=arm CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf- -C $(LINUXDIR) M=$(shell pwd)

clean:
	make -C $(LINUXDIR) M=$(shell pwd) clean
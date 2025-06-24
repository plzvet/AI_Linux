# Simple Makefile for cross-compiling the LED control kernel module

# Module object
obj-m += led_control.o

# Kernel build directory (set to your kernel source/headers location)
KDIR ?= /home/pjk/project/linux
PWD  := $(shell pwd)
ARCH ?= arm64
CROSS_COMPILE ?= aarch64-linux-gnu-

.PHONY: all clean

all:
	+$(MAKE) -C $(KDIR) M=$(PWD) \
		ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	+$(MAKE) -C $(KDIR) M=$(PWD) \
		ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean

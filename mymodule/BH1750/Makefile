obj-m += bh1750_module.o

KDIR      ?= /home/pjk/project/linux     # 실제 커널 소스/헤더 디렉토리
PWD       := $(shell pwd)
ARCH      ?= arm64
CROSS_COMPILE ?= aarch64-linux-gnu-

all:
	+make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	+make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean

ifneq ($(KERNELRELEASE),)
    ccflags-y := -std=gnu99 -g -Og -DENABLE_DEBUG
    scull-objs := main.o fops.o
    obj-m := scull.o
else
    KERNELDIR ?= /home/ankit/dev/linux-mainline/build_qemu
    PWD := $(shell pwd)

HOSTDIR ?= /lib/modules/$(shell uname -r)/build

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

host:
	$(MAKE) -C $(HOSTDIR) M=$(PWD) modules

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

CFLAGS_main.o := -DDEBUG
CFLAGS_fops.o := -DDEBUG

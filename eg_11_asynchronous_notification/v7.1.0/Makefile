ifneq ($(KERNELRELEASE),)
    ccflags-y := -std=gnu99 -DENABLE_DEBUG
    async_notify-objs := main.o fops.o
    obj-m := async_notify.o
else
	KERNELDIR ?= /home/ankit/dev/linux-src/linux-mainline/build_qemu
    PWD := $(shell pwd)

.PHONY: modules
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc --static -o async_notify.out test/async_notify.c
endif

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

CFLAGS_main.o := -DDEBUG
CFLAGS_fops.o := -DDEBUG

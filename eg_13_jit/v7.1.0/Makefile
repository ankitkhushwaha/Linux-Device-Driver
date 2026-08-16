ifneq ($(KERNELRELEASE),)
    ccflags-y := -std=gnu99 -DENABLE_DEBUG
    jit-objs := main.o fops.o
    obj-m := jit.o
else
	KERNELDIR ?= /home/ankit/dev/linux-src/linux-mainline/build_qemu
    PWD := $(shell pwd)

.PHONY: modules
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

CFLAGS_main.o := -DDEBUG
CFLAGS_fops.o := -DDEBUG

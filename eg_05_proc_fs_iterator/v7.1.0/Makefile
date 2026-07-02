ifneq ($(KERNELRELEASE),)
    ccflags-y := -std=gnu99 -DENABLE_DEBUG
    proc_fs_iterator-objs := main.o fops.o
    obj-m := proc_fs_iterator.o
else
    KERNELDIR ?= /home/ankit/dev/linux-mainline/build_qemu
    PWD := $(shell pwd)

HOSTDIR ?= /lib/modules/$(shell uname -r)/build

.PHONY: modules
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

CFLAGS_main.o := -DDEBUG
CFLAGS_fops.o := -DDEBUG

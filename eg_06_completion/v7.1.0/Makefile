ifneq ($(KERNELRELEASE),)
    ccflags-y += -std=gnu99 -Wno-unused-variable -DENABLE_DEBUG
    completion-objs := main.o
    obj-m := completion.o
else
    KERNELDIR ?= /home/ankit/dev/linux-mainline/build_qemu
    PWD := $(shell pwd)

HOSTDIR ?= /lib/modules/$(shell uname -r)/build

.PHONY: clean
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

CFLAGS_main.o := -DDEBUG

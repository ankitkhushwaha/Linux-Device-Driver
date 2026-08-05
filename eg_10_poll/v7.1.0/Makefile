ifneq ($(KERNELRELEASE),)
    ccflags-y := -std=gnu99 -DENABLE_DEBUG
    poll-objs := main.o fops.o
    obj-m := poll.o
else
	KERNELDIR ?= /home/ankit/dev/linux-src/linux-mainline/build_qemu
    PWD := $(shell pwd)

.PHONY: modules
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -std=gnu99 -o poll.out --static test/poll.c
endif

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm poll.out

CFLAGS_main.o := -DDEBUG
CFLAGS_fops.o := -DDEBUG

ifneq ($(KERNELRELEASE),)
	ccflags-y += -DENABLE_DEBUG -DDEBUG
	ioctl-objs := fops.o main.o
	obj-m := ioctl.o
else
	KERNELDIR ?= /home/ankit/dev/linux-mainline/build_qemu
	PWD := $(shell pwd)
endif

BIN := ioctl_test.out

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc -static -o $(BIN) ./test/main.c

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm *.out
KERNEL_DIR := /home/wyg/linux/ebf_linux_kernel/
ARCH=arm
CROSS_COMPILE=/home/wyg/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
export ARCH CROSS_COMPILE
obj-m:=beep_driver.o
all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
.PHONY:clean
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean
	rm $(out)
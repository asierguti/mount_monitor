mountmon-objs := mount_monitor.o probe.o
obj-m += mountmon.o

KBUILD_CFLAGS += -w

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

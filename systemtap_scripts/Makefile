obj-m := kmalloc_ioctl.o
obj-m += dummy_ioctl.o

all:
	make -C  /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc kmalloc_app.c -o kmalloc_app

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm kmalloc_app
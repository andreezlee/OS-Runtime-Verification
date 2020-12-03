obj-m := kmalloc_ioctl.o


all:
	make -C  /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc kmalloc_test1.c -o kmalloc_app
	gcc dummy.c -o dummy
	gcc kmalloc_monitor_spec1.c -o monitor1
	gcc kmalloc_monitor_spec2.c -o monitor2
	gcc kmalloc_monitor_spec3.c -o monitor3

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm kmalloc_app dummy monitor1 monitor2 monitor3

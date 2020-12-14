obj-m := kmalloc_ioctl.o online/monitor_lib.o online/monitor_spec1.o


all:
	make -C  /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	@echo "MAKE COMPLETE FOR ONLINE ARTIFACTS"

	gcc kmalloc_test1.c -o kmalloc_app
	gcc dummy.c -o dummy
	@echo "MAKE COMPLETE FOR TEST ARTIFACTS"

	gcc offline/kmalloc_monitor_spec1.c -o offline/monitor1
	gcc offline/kmalloc_monitor_spec2.c -o offline/monitor2
	gcc offline/kmalloc_monitor_spec3.c -o offline/monitor3
	@echo "MAKE COMPLETE FOR OFFLINE ARTIFACTS"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	@echo "REMOVED ONLINE ARTIFACTS"
	rm kmalloc_app dummy
	cd offline; rm monitor1 monitor2 monitor3
	@echo "REMOVED OFFLINE ARTIFACTS"

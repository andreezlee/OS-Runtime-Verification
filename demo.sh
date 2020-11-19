#!/bin/bash

alias stap=~/systemtap-4.0/stap
cd systemtap_scripts

if [ "$1" = "clean" ]
then
	sudo rmmod kmalloc_ioctl
	rm kmalloc.txt
	make clean
else
	make
	sudo insmod kmalloc_ioctl.ko
	sudo ./dummy &
	sudo stap -g kmalloc_instrumentation.stp &
	sleep 4
	sudo ./kmalloc_app
	sleep 1
	killall dummy
	sudo ./monitor1
fi

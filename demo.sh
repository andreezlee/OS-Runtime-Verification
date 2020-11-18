#!/bin/bash

alias stap=~/systemtap-4.0/stap
cd systemtap_scripts

if [$1 = "clean"]
then
	sudo rmmod kmalloc_ioctl
	rm kmalloc.txt
	make clean
else
	make
	sudo insmod kmalloc_ioctl
	sudo ./dummy &
	sudo stap kmalloc_instrumentation.stp
	sleep 2
	sudo ./kmalloc_app
	sleep 1
	killall dummy
	sudo ./monitor1
fi
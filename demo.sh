#!/bin/bash

alias stap=~/systemtap-4.0/stap
cd systemtap_scripts

if [ "$1" = "clean" ]
then
	# Remove compiled files, kmalloc module, and trace
	sudo rmmod kmalloc_ioctl
	rm kmalloc.txt
	make clean
else
	# Compile and add module to system
	make
	sudo insmod kmalloc_ioctl.ko
	# Set up instrumentation, give it some time
	sudo ./dummy &
	sudo stap -g kmalloc_instrumentation.stp &
	sleep 4
	# Run test program
	sudo ./kmalloc_app
	sleep 1
	killall dummy
	# Offline monitoring
	sudo ./monitor1
fi

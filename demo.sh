#!/bin/bash

alias stap=~/systemtap-4.0/stap
cd systemtap_scripts

if [ "$1" = "clean" ]
then
	# Remove compiled files, kmalloc module, and trace
	sudo rmmod kmalloc_ioctl &> /dev/null
	sudo rmmod monitor_spec1 &> /dev/null
	sudo rmmod monitor_spec2 &> /dev/null
	sudo rmmod monitor_spec3 &> /dev/null
	sudo rmmod monitor_lib &> /dev/null
	rm kmalloc.txt &> /dev/null
	make clean &> /dev/null
	sleep 10
	echo "DEMO FILES CLEANED FROM PROJECT"
else
	# Compile and add module to system
	make &> /dev/null
	sudo insmod kmalloc_ioctl.ko
	echo "DEMO FILES CREATED AND LOADED"
	if [[ "$1" == "offline" ]]
	then
		# Set up instrumentation, give it some time
		sudo ./dummy &
		sudo stap -g offline/kmalloc_instrumentation.stp /home/andre/OS-Runtime-Verification/systemtap_scripts/kmalloc_app kmalloc_app &
		sleep 6
		echo "INSTRUMENTATION SETUP COMPLETED"
		#Run test program
		sudo ./kmalloc_app
		echo "RUNNING TEST FILE"
		sleep 10
		sudo killall dummy
		# Offline monitoring
		echo "OFFLINE MONITORING STARTED"
		sleep 2
		sudo ./monitor1
		sleep 2
		sudo ./monitor2
		sleep 2
		sudo ./monitor3
		sleep 2
		echo "OFFLINE MONITORING COMPLETED"
	else
		# Set up instrumentation with online monitor, give it some time
		cd online
		sudo dmesg -c &> /dev/null
		sudo insmod monitor_lib.ko
		sudo insmod monitor_spec1.ko
		sudo insmod monitor_spec2.ko
		sudo insmod monitor_spec3.ko
		sudo ../dummy &
		sudo stap -g kmalloc_instrumentation.stp /home/andre/OS-Runtime-Verification/systemtap_scripts/kmalloc_app kmalloc_app &
		sleep 10
		echo "INSTRUMENTATION SETUP COMPLETED"
		echo "ONLINE MONITORING ENGAGED"
		#Run test program
		echo "RUNNING TEST FILE"
		sudo time ../kmalloc_app
		sleep 3
		sudo killall dummy
		echo "ONLINE MONITORING COMPLETED"
	fi
fi

#!/bin/bash

alias stap=~/systemtap-4.0/stap
cd systemtap_scripts

if [ "$1" = "clean" ]
then
	# Remove compiled files, kmalloc module, and trace
	sudo rmmod kmalloc_ioctl
	rm kmalloc.txt
	make clean
	sleep 2
	echo "DEMO FILES CLEANED FROM PROJECT"
else
	# Compile and add module to system
	make
	sudo insmod kmalloc_ioctl.ko
	echo "DEMO FILES CREATED AND LOADED"
	if ["$1" = "offline"]
	then
		# Set up instrumentation, give it some time
		sudo ./dummy &
		sudo stap -g offline/kmalloc_instrumentation.stp &
		sleep 6
		echo "INSTRUMENTATION SETUP COMPLETED"
		#Run test program
		sudo ./kmalloc_app
		echo "RUNNING TEST FILE"
		sleep 3
		killall dummy
		# Offline monitoring
		echo "OFFLINE MONITORING STARTED"
		cd offline
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
		sudo ./dummy &
		sudo stap -g online/kmalloc_instrumentation.stp &
		sudo offline/main_monitor
		sleep 6
		echo "INSTRUMENTATION SETUP COMPLETED"
		echo "ONLINE MONITORING ENGAGED"
		#Run test program
		sudo ./kmalloc_app
		echo "RUNNING TEST FILE"
		sleep 3
		killall dummy
		echo "ONLINE MONITORING COMPLETED"
	fi
fi

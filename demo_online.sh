#!/bin/bash

alias stap=../systemtap-4.0/stap
cd systemtap_scripts

file_path=/home/andre/OS-Runtime-Verification/systemtap_scripts/kmalloc_app

# Compile and add module to system
make &> /dev/null
sudo insmod kmalloc_ioctl.ko
echo "DEMO FILES CREATED AND LOADED"

# Set up instrumentation with online monitor, give it some time
cd online
sudo dmesg -c &> /dev/null
sudo insmod monitor_lib.ko
sudo insmod monitor_spec1.ko
sudo insmod monitor_spec2.ko
sudo insmod monitor_spec3.ko
sudo ../dummy &
sudo stap -g kmalloc_instrumentation.stp $file_path kmalloc_app &
echo "INSTRUMENTATION SETUP COMPLETED"
sleep 10
echo "ONLINE MONITORING ENGAGED"
#Run test program
echo "RUNNING TEST FILE"
sudo ../kmalloc_app
sleep 3
sudo killall dummy
cd ..

# Remove compiled files & kmalloc modules
echo "CLEANING UP DEMO FILES"
sudo rmmod kmalloc_ioctl &> /dev/null
sudo rmmod monitor_spec1 &> /dev/null
sudo rmmod monitor_spec2 &> /dev/null
sudo rmmod monitor_spec3 &> /dev/null
sudo rmmod monitor_lib &> /dev/null
make clean &> /dev/null
sleep 5
echo "ONLINE MONITORING COMPLETED"

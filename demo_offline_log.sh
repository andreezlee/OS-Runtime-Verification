#!/bin/bash

alias stap=../systemtap-4.0/stap
cd systemtap_scripts

file_path=/home/andre/OS-Runtime-Verification/systemtap_scripts/kmalloc_app

# Compile and add module to system
sudo rm kmalloc.txt &> /dev/null
make &> /dev/null
sudo insmod kmalloc_ioctl.ko
echo "DEMO FILES CREATED AND LOADED"

# Set up instrumentation, give it some time
sudo ./dummy &
sudo stap -g offline/kmalloc_instrumentation.stp $file_path kmalloc_app &
sleep 10
echo "INSTRUMENTATION SETUP COMPLETED"

#Run test program
echo "RUNNING TEST FILE"
sudo ./kmalloc_app
sleep 10
echo "CLEANING UP DEMO FILES"
sudo killall dummy
sudo rmmod kmalloc_ioctl
make clean &> /dev/null
sleep 5
echo "LOG FROM DEMO COMPLETED"

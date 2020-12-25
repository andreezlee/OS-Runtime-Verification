#!/bin/bash
cd systemtap_scripts

# Compile monitor code
make &> /dev/null
echo "DEMO FILES CREATED AND LOADED"

# Offline monitoring
echo "OFFLINE MONITORING STARTED"
sleep 2
sudo ./monitor1
sleep 2
sudo ./monitor2
sleep 2
sudo ./monitor3
sleep 2
echo "CLEANING UP DEMO FILES"
make clean &> /dev/null
sleep 5
echo "OFFLINE MONITORING COMPLETED"

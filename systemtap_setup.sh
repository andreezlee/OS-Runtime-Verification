#!/bin/bash

# Setting up SystemTap 4.0 for the system
cd ..
echo RETRIEVING SYSTEMTAP
wget https://sourceware.org/systemtap/ftp/releases/systemtap-4.0.tar.gz
tar xf systemtap-4.0.tar.gz
echo SYSTEMTAP DOWNLOADED, INSTALLING DEPENDENCIES
cd systemtap-4.0/
sudo apt-get install -y elfutils-devel &> /dev/null
sudo apt-get install -y libebl-dev &> /dev/null
sudo apt-get install -y libdw-dev &> /dev/null
sudo apt-get install -y libebl-devel &> /dev/null
echo INSTALLED DEPENDENCIES, INSTALLING SYSTEMTAP
sudo ./configure
sudo make
sudo make install
echo PROCESS COMPLETE
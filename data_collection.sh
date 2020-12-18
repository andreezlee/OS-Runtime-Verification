#!/bin/bash

alias stap=../systemtap-4.0/stap

benchmarks=(avrora batik eclipse fop h2 jython luindex lusearch lusearch-fix pmd sunflow tomcat tradebeans tradesoap xalan)
dacapo=dacapo-9.12-MR1-bach.jar

function perform_online(){
    echo STARTING TESTS ON $1
    cd online
    sudo dmesg -c &> /dev/null
    sudo stap -g kmalloc_instrumentation.stp java java &
    sleep 5
    cd ..
    touch results/$1_oh_n.txt
    java -jar $dacapo $1 &>> results/$1_oh_n.txt
    sleep 3
    touch results/$1_viol_1_n.txt
    dmesg | grep SPEC1 &>> results/$1_viol_1_n.txt
    touch results/$1_viol_2_n.txt
    dmesg | grep SPEC2 &>> results/$1_viol_2_n.txt
    touch results/$1_viol_3_n.txt
    dmesg | grep SPEC3 &>> results/$1_viol_3_n.txt
    echo ONLINE MONITORING DONE FOR $1
}

function perform_offline(){
    sudo stap -g -DMAXMAPENTRIES=1000000 -DMAXACTION=1000000 offline/kmalloc_instrumentation.stp java java &
    sleep 5
    touch results/$1_oh_f.txt
    java -jar $dacapo $1 &>> results/$1_oh_f.txt
    sleep 10
    touch results/$1_viol_1_f.txt
    sudo ./monitor1 &>> results/$1_viol_1_f.txt
    sleep 2
    touch results/$1_viol_2_f.txt
    sudo ./monitor2 &>> results/$1_viol_2_f.txt
    sleep 2
    touch results/$1_viol_3_f.txt
    sudo ./monitor3 &>> results/$1_viol_3_f.txt
    sleep 2
    rm kmalloc.txt &> /dev/null
    echo OFFLINE MONITORING DONE FOR $1
}

cd systemtap_scripts
make &> /dev/null
cd online
sudo insmod monitor_lib.ko
sudo insmod monitor_spec1.ko
sudo insmod monitor_spec2.ko
sudo insmod monitor_spec3.ko
cd ..
echo FILES CREATED AND LOADED

echo RUNNING WITHOUT MONITORING
for n in {1..3}
do
    echo TRIAL $n
    for b in ${benchmarks[@]}
    do
    	touch results/$b.txt
        java -jar $dacapo $b &>> results/$b.txt
    done
done

echo ONLINE MONITORING
for n in {1..3}
do
    echo TRIAL $n
    for b in ${benchmarks[@]}
    do
        perform_online $b
    done
done

echo OFFLINE MONITORING
for n in {1..3}
do
    echo TRIAL $n
    for b in ${benchmarks[@]}
    do
        perform_offline $b
    done
done

echo CLEANING TEST FILES
sudo rmmod kmalloc_ioctl &> /dev/null
sudo rmmod monitor_spec1 &> /dev/null
sudo rmmod monitor_spec2 &> /dev/null
sudo rmmod monitor_spec3 &> /dev/null
sudo rmmod monitor_lib &> /dev/null
make clean &> /dev/null
sleep 10
echo TEST FILES CLEANED FROM PROJECT

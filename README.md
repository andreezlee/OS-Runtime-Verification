# OS-Runtime-Verification
Runtime Verification Project for Linux Operating System

## Installation and Setup
Instructions for Ubuntu 16.04 LTS (can also be run in a VM):

Should have ```git``` installed

Run the script with ```sudo ./install_dbgsym.sh```.

Install systemtap-4.0 using ```sudo ./systemtap_setup.sh```

## Running Demo

Before running the demo, please change the ```file_path``` variable in the ```demo.sh``` file to the correct path for your system.

After installation, demo can be run with ```sudo ./demo.sh [option]```.

After the demo, ```sudo ./demo.sh clean``` can be used to remove the kernel modules from the system and other artifacts of the computation.

### Offline option

With ```sudo ./demo.sh offline```, the script sets up the kernel module and the instrumentation, then produces a trace ```kmalloc.txt``` when the test program is run. The offline monitor for the demo specification is then run on the trace.

### Online option

With ```sudo ./demo.sh online```, the script sets up the kernel modules for monitoring and the instrumentation, then performs online monitoring of the test program. Output from the monitors is put in kernel logs accessible with ```dmesg```.

Use ```dmesg | grep SPEC[n]```, where [n] denotes the specification (1, 2, or 3) that is checked.

## Running Full Tests

You must have Java installed (script works with OpenJDK 8)

To run the full DaCapo suite, please download DaCapo [here](http://dacapobench.org/) into the ```systemtap_scripts``` folder. The original scripts were built for version 9.12, you may change them as necessary.

Full tests (three trials) are run with ```sudo ./data_collection.sh```. All output from monitors is redirected to the ```systemtap_scripts/results``` directory.

## Troubleshooting

#### When installing kernel debug symbols:

If issues appear along the lines of ```public key unavailable```, then run ```sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys [MISSING-KEY-HERE]```. Newer kernels might also use the key ```C8CAB6595FDFF622``` instead, if it leads to semantics errors in SystemTap.


#### When SystemTap has error with ```get_user_pages```:

Apply the fix mentioned [here](https://launchpadlibrarian.net/425005337/fix-get-user-pages-prototype.patch) to fix ```get_user_pages``` issue in the systemtap-4.0 directory.
In systemtap-4.0, re-run:

```sudo ./configure```
```sudo make```
```sudo make install```

Possible errors might also require:
```sudo apt-get install [elfutils-devel libebl-dev libdw-dev, and/or libebl-devel]``` This depends on the specific Linux version available on the system, may also need ```elfutils``` or ```linux-generic```.
If the system has been recently updated, you would need ```sudo apt-get update``` and ```sudo apt-get upgrade```.


#### If offline monitors are giving issues:

This might be due to read-after-write issues. Feel free to increase the ```sleep``` time between running the SystemTap script and running the monitors, or ```sudo ./demo.sh clean``` multiple times and waiting more before running offline or online.

## Previous Progress

This project contains files no longer used but might be useful for understanding the current implementation.

The ```systemtap_scripts/demos``` directory contains test scripts for SystemTap that originally were used to check functionality.

The ```systemtap_scripts/online/First Approach (FAILS)``` is the first approach to online monitoring implemented in this project. It failed due to the locks used to enforce FIFO causing SystemTap scripts to drop probes.
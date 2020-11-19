# OS-Runtime-Verification
Runtime Verification Project for Linux Operating System

## Installation and Setup
Instructions for Ubuntu 16.04 LTS (can also be run in a VM):

Should have ```git``` installed

Install systemtap-4.0 from https://sourceware.org/systemtap/ftp/releases/ and extract in the home directory.

Apply the fix mentioned [here](https://launchpadlibrarian.net/425005337/fix-get-user-pages-prototype.patch) to fix ```get_user_pages``` issue in the systemtap-4.0 directory.

Run the script with ```sudo install_dbgsym.sh```.

If issues appear along the lines of ```public key unavailable```, then run ```sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys [MISSING-KEY-HERE]```

In systemtap-4.0:

```sudo ./configure```
```sudo make```
```sudo make install```

Possible errors might require:
```sudo apt-get install [elfutils-devel libebl-dev libdw-dev, and/or libebl-devel]``` This depends on the specific Linux version available on the system, may also need ```elfutils``` or ```linux-generic```.

## Running Demo

Before running the demo, please change the filepath in the ```kmalloc_instrumentation.stp``` file to the correct path for your system.

After installation, demo can be run with ```sudo demo.sh```. The script sets up the kernel module and the instrumentation, then produces a trace ```kmalloc.txt``` when the test program is run. The offline monitor for the demo specification is then run on the trace.

After the demo, ```sudo demo.sh clean``` can be used to remove the kernel module from the system and other artifacts of the computation.
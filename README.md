# OS-Runtime-Verification
Runtime Verification Project for Linux Operating System

Instructions for Ubuntu 16.04 LTS (can also be run in a VM):

Should have ```git``` installed

installed systemtap 4.0  from https://sourceware.org/systemtap/ftp/releases/

https://launchpadlibrarian.net/425005337/fix-get-user-pages-prototype.patch to fix ```get_user_pages``` issue

run the script

if issues along the lines of ```public key unavailable```, then run ```sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys [MISSING-KEY-HERE]```

In systemtap-4.0:

```sudo ./configure```
```sudo make```
```sudo make install```

Possible errors might require:
```sudo apt-get install [elfutils-devel libebl-dev libdw-dev, and/or libebl-devel]``` This depends on the specific linux version available, may also need ```elfutils``` or ```linux-generic```.

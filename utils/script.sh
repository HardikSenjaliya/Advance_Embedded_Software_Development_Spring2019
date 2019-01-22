#!/bin/bash

echo "This is the output file of the script"

echo "User Information"

id hardyk

echo "OS Information"
uname -a

echo "OS Distribution"
lsb_release -a

echo "OS Version"
uname -r

echo "Kernel Version"
uname -a

echo "gcc version"
gcc --version

echo "Kernel build time"
uname -v

echo "CPU Architecture Information"
lscpu

echo "Information on FIle system memory"
cat /proc/meminfo



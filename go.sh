#!/bin/bash

set -e

gcc -g -O sud.c -o sud

time ./sud ./sud1.txt
#time ./sud ./sud2.txt
time ./sud ./sud3.txt
time ./sud ./sud4.txt
time ./sud ./sud6.txt
time ./sud ./sud7.txt

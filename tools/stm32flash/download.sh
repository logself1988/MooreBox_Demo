#!/bin/sh 
./stm32flash -b 38400 -m 8e1 -w ../../main.hex -v -g 0x8000000 /dev/ttyUSB0

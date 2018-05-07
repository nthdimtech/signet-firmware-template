#!/bin/sh
dfu-util -d 0483:df11 -a 0 -v -s 0x8000000 -D fw.bin

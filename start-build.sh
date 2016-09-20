#!/bin/bash
make ARCH=arm distclean
rm -rf ../out
mkdir -p ../out
make ARCH=arm O=../out merge_hi3630_defconfig
make ARCH=arm O=../out -j8
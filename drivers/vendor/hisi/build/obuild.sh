#!/bin/bash -e

BALONG_TOPDIR=$(pwd)/..
rsync -rt ${BALONG_TOPDIR}/drv/acore/ ${BALONG_TOPDIR}/system/android/android_4.2_r1
cd ${BALONG_TOPDIR}/system/android/android_4.2_r1
. build/envsetup.sh
choosecombo release balongv7r2 eng
USE_CCACHE=1
./prebuilts/misc/linux-x86/ccache/ccache -M 30G
make kernel -j 48
make BUILD_TINY_ANDROID=true -j 48
make clean-fastboot.img clean-libboot clean-libboot_arch_balongv7r2 clean-libnand clean-libboot_c
make fastboot.img -j 48

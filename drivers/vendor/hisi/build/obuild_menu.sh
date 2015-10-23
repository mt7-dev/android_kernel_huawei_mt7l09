#!/bin/bash
set -e

BTOPDIR=$(pwd)/..
#cp -f ${BTOPDIR}/config/product/hi6930_p531_asic/os/balongv7r2_defconfig 
make -C ${BTOPDIR}/modem/system/android/android_4.2_r1/kernel O=${BTOPDIR}/modem/system/android/android_4.2_r1/out/target/product/balongv7r2/obj/KERNEL_OBJ ARCH=arm CROSS_COMPILE=${BTOPDIR}/modem/system/android/android_4.2_r1/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi- balongv7r2_defconfig menuconfig

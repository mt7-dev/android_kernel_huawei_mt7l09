#!/bin/bash -e

KERNEL_OUT=$1
TOP=$2
DTS_NAME=$3

${KERNEL_OUT}/scripts/dtc/dtc -I dts -O dtb -o ${KERNEL_OUT}/arch/arm/boot/${DTS_NAME}.dtb ${TOP}/kernel/arch/arm/boot/dts/${DTS_NAME}.dts
cat ${KERNEL_OUT}/arch/arm/boot/zImage ${KERNEL_OUT}/arch/arm/boot/${DTS_NAME}.dtb > ${KERNEL_OUT}/arch/arm/boot/tmp-zImage
rm -f ${KERNEL_OUT}/arch/arm/boot/zImage
mv ${KERNEL_OUT}/arch/arm/boot/tmp-zImage ${KERNEL_OUT}/arch/arm/boot/zImage

exit $?


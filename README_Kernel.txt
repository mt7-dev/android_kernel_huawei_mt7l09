################################################################################

1. How to Build
	- get Toolchain
		From android git server, codesourcery and etc ..
		 - gcc-linaro-arm-linux-gnueabihf-4.7

	- edit Makefile
		edit "CROSS_COMPILE" to right toolchain path(You downloaded).
		Ex)   export PATH=$PATH:$(android platform directory you download)/prebuilts/gcc/linux-x86/arm/gcc-linaro-arm-linux-gnueabihf-4.7/bin
		Ex)   export CROSS_COMPILE=$(android platform directory you download)/prebuilts/gcc/linux-x86/arm/gcc-linaro-arm-linux-gnueabihf-4.7/bin/arm-linux-gnueabihf-

		$ mkdir ../out
		$ make ARCH=arm O=../out merge_hi3630_defconfig
		$ make ARCH=arm O=../out -j8

2. Output files
		- Kernel : out/arch/arm/boot/zImage
		- module : out/drivers/*/*.ko

3. How to Clean
		$ make ARCH=arm distclean
		$ rm -rf out
################################################################################

# Kernel for Huawei Ascend Mate 7
**Version**: 3.10.86
**Shipped with**: Android 6.0 Marshmallow
**Type**: adapted for aosp build tree

## Details
This is the kernel 3.10.86 shipped with the Marshmallow release for Huawei Mate 7. This is the adapted version to build with the aosp tree, please see the branch *3.10.86-stock* to get the stock version or the branch *3.10.86-adapted* to get the compilable stock version

### Differences from adapted version
 *Work in progress...*

### Differences from stock
The branch differs from the stock one in these edited files:
- `modified:   drivers/hisi/modem_hi363/drv/Makefile`

#### diff
Here's the differences:
```
diff --git a/drivers/hisi/modem_hi3630/drv/Makefile b/drivers/hisi/modem_hi3630/drv/Makefile
index 430a5b8..24668a2 100755
--- a/drivers/hisi/modem_hi3630/drv/Makefile
+++ b/drivers/hisi/modem_hi3630/drv/Makefile
@@ -75,7 +75,9 @@ ifeq ($(strip $(CFG_CONFIG_IPF)),YES)
 obj-y				+= ipf/
 endif
 obj-y				+= lowpower_mntn/
-obj-$(CONFIG_BALONG_OAM_PS)  += $(if $(findstring .o,$(shell ls $(CURDIR)/drivers/hisi/modem_hi3630/drv/balong_oam_ps)), balong_oam_ps/, )
+#EDIT TO GET IT WORK - gabry3795
+#obj-$(CONFIG_BALONG_OAM_PS)  += $(if $(findstring .o,$(shell ls $(CURDIR)/drivers/hisi/modem_hi3630/drv/balong_oam_ps)), balong_oam_ps/, )
++obj-$(CONFIG_BALONG_OAM_PS)  += $(if $(findstring .o,$(shell ls $(BALONG_TOPDIR)/../../../drivers/hisi/modem_hi3630/drv/balong_oam_ps)), balong_oam_ps/, )
 
 subdir-ccflags-y := -Werror
```

## How to build
### Only the kernel
Please read [README_Kernel.txt](README_Kernel.txt)

### The kernel inside Android source
In [Android.mk](Android.mk) edit the line
`KERNEL_TOOLCHAIN := /home/gabry3795/cyanogen/bin/gcc-linaro-arm-linux-gnueabihf-4.7/bin`
with the directory where you have the linaro cross compiling toolchain binaries.

# Thanks
Thanks to:
- @[ruffy91](https://github.com/ruffy91) for his first compiling suggestions [XDA Post](http://forum.xda-developers.com/showpost.php?p=66223787&postcount=18) 
- @acbka for the *hisi_partitions.h* header - [XDA post](http://forum.xda-developers.com/showpost.php?p=66382078&postcount=28)

That's all.

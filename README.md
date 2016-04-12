# Kernel for Huawei Ascend Mate 7
**Version**: 3.10.86
**Shipped with**: Android 6.0 Marshmallow
**Type**: adapted to be compiled

## Details
This is the kernel 3.10.86 shipped with the Marshmallow release for Huawei Mate 7. This is the adapted version and it's **compilable**, please see the branch *3.10.86-stock* to get the stock version
### Edited files at init branch
From the first commit this branch differs from the stock one in these edited files:
- `modified:   drivers/hisi/modem_hi363/drv/Makefile`
- `modified:   drivers/mmc/card/block.c`
- `modified:   drivers/mtd/hisi_nve.h`

#### diff
Here's the differences
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
 
diff --git a/drivers/mmc/card/block.c b/drivers/mmc/card/block.c
index 8cbd06e..ed44262 100755
--- a/drivers/mmc/card/block.c
+++ b/drivers/mmc/card/block.c
@@ -56,7 +56,8 @@
 
 #include "queue.h"
 
-#include "hisi_partition.h"
+// EDIT TO GET IT TO WORK - gabry3795
+//#include "hisi_partition.h"
 #ifdef CONFIG_HW_SD_HEALTH_DETECT
 #include "mmc_health_diag.h"
 #endif
diff --git a/drivers/mtd/hisi_nve.h b/drivers/mtd/hisi_nve.h
index 472fe50..d2416e6 100644
--- a/drivers/mtd/hisi_nve.h
+++ b/drivers/mtd/hisi_nve.h
@@ -8,8 +8,8 @@
 
 #ifndef __NVE_H
 #define __NVE_H
-
-#include "hisi_partition.h"
+// EDIT TO GET IT TO WORK - gabry3795
+// #include "hisi_partition.h"
 
 #define TEST_NV_IN_KERNEL       1
```


## How to build
### Only the kernel
Please read [README_Kernel.txt](README_Kernel.txt)

### With the Cyanogenmod
- In [Android.mk](Android.mk) edit the line with the directory where you have the linaro cross compiling toolchain
```make
KERNEL_TOOLCHAIN := /home/gabry3795/cyanogen/bin/gcc-linaro-arm-linux-gnueabihf-4.7/bin
``` 


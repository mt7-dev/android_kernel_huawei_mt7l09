#!/bin/bash
export PATH=$PATH:/opt/4.5.1/bin 

ifeq ($(CFG_TTF_SKB_EXP),NO)
arm-linux-gcc -static -o NwInquire -I $LINUX_KERNEL/include $BALONG_SRC_PATH/ps/as/gu/ttf/TTFComm/NFEXT/NetworkInquire.c $BALONG_SRC_PATH/oam/gu/acore/om/AppLinuxDemo.c
else
arm-linux-gcc -static -o NwInquire -I $BALONG_SRC_PATH/modem/drv/source/src/acore/android-2.6.35/include $BALONG_SRC_PATH/ps/as/gu/ttf/TTFComm/NFEXT/NetworkInquire.c $BALONG_SRC_PATH/oam/gu/acore/om/AppLinuxDemo.c
endif
arm-linux-strip NwInquire

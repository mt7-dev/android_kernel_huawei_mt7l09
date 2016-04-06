
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <DrvInterface.h>
#include <hifidrvinterface.h>

#include <bsp_ipc.h>
#include <bsp_icc.h>
#include <bsp_dump.h>
#include <bsp_dsp.h>
#include <bsp_reset.h>
#include "reset_balong.h"

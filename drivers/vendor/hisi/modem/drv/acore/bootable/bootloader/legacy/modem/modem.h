#ifndef _FASTBOOT_MODEM_H_
#define _FASTBOOT_MODEM_H_

#include <platform.h>
#include "bsp_sram.h"
#include "bsp_shared_ddr.h"
#include "product_config.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/(sizeof((a)[0])))
#endif

#define PERI_CTRL11			(REG_BASE_PCTRL + 0xC18)
#define CRG_SRSTDIS1		(HI_SYSCTRL_BASE_ADDR + 0x64)
#define MDMA9_PD_SRST_DIS	(0x1 << 15)
#define MDMA9_CPU_SRST_EN	(0x1 << 6)

#define PERI_CRG_RSTDIS0			(REG_BASE_PERI_CRG + 0x64)
#define PERI_CRG_RSTDIS4			(REG_BASE_PERI_CRG + 0x94)

#define BIT_RST_MODEM			(0x1 << 28)
#define BIT_RST_MCPU2DDR		(0x1 << 27)
#define BIT_RST_MBUS2BUS		(0x1 << 28)

#define MODEM_TDS_SIZE		(0x40000)		/* 0.25 MB*/
#define MODEM_DSP_SIZE		(0x180000)		/* 1.5 MB*/
#define MODEM_NV_SIZE		(4*1024*1024)

#define DSP_PARTITON_SIZE	(2*1024*1024)

#define LOAD_DSP_ERROR_FLAG	(-1)
#define LOAD_DSP_OK_FLAG	(0x5312ABCD)

#define LOAD_MODEM_OK_FLAG      (0xABCD5132)
#define LOAD_MODEM_ERROR_FLAG   (0xFFFFFFFF)

#define BBE_PLL_DFS1        0x20c
#define BBE_PLL_DFS2        0x210

#define BBP_PLL_DFS1        0x224
#define BBP_PLL_DFS2        0x228
#define CLK_CRGEN3          0x18
#define CLK_CRGEN4          0x24
#define CLK_CRGEN5          0x30
#define RESET_DIS1          0x64
#define RESET_DIS3          0x7C

struct image_head
{
	char image_name[16];
	char image_version[32];

	unsigned int image_index;
	unsigned int image_length;
	unsigned int load_addr;
	unsigned int crc;
	unsigned int is_sec;

	char reserved[128 - 16 - 32 -sizeof(unsigned int) * 5];
};

/* votage range */
typedef struct _tvol_range
{
    u16 vol_low;
    u16 vol_high;
}vol_range;
int bsp_version_get_hwversion_index(void);

#endif

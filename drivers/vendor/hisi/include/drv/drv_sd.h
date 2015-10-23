/**
 * drv_sd.h - mmc interface
 *
 * Copyright (C), 2011-2013, Hisilicon Technologies Co., Ltd.
 *
 * Authors: jingyong
 *	    
 *
 */


#ifndef __DRV_SD_H__
#define __DRV_SD_H__
#ifdef __KERNEL__
#include <linux/scatterlist.h>
#endif
typedef struct _sd_dev_type_str
{
    unsigned long   devNameLen;
    char            devName[16];
} SD_DEV_TYPE_STR;

enum SD_MMC_OPRT_ENUM
{
    SD_MMC_OPRT_FORMAT = 0,
    SD_MMC_OPRT_ERASE_ALL,
    SD_MMC_OPRT_ERASE_SPEC_ADDR,
    SD_MMC_OPRT_WRITE,
    SD_MMC_OPRT_READ,
    SD_MMC_OPRT_BUTT
};

#ifdef __VXWORKS__
static __inline__ unsigned long  DRV_SDMMC_AT_PROCESS(unsigned int ulOp,
                unsigned long ulAddr,  unsigned long ulData, unsigned char *pucBuffer, unsigned long *pulErr)
{
	return 0;
}
static __inline__ unsigned long DRV_SDMMC_GET_OPRT_STATUS(void)
{
	return 0;
}
static __inline__ unsigned int DRV_SDMMC_ADD_HOOK(int hookType, void *p)
{
	return 0;
}

static __inline__ int DRV_SDMMC_USB_STATUS(void)
{
	return 0;
}

static __inline__ void DRV_SDMMC_UPDATE_DISPLAY(int state)
{
	return;
}
static __inline__ void DRV_SDMMC_CLEAR_WHOLE_SCREEN(void)
{
	return;
}

static __inline__ int DRV_SD_GET_STATUS(void)
{
	return 0;
}

static __inline__ int DRV_SD_GET_CAPACITY(void)
{
	return 0;
}

#elif defined (__KERNEL__)
unsigned long  DRV_SDMMC_AT_PROCESS(unsigned int ulOp,
                unsigned long ulAddr,  unsigned long ulData, unsigned char *pucBuffer, unsigned long *pulErr);
unsigned long DRV_SDMMC_GET_OPRT_STATUS(void);
int DRV_SDMMC_GET_STATUS(void);
int DRV_SD_GET_STATUS(void);
int DRV_SD_GET_CAPACITY(void);
int DRV_SD_SG_INIT_TABLE(const void *buf,unsigned int buflen);
int DRV_SD_TRANSFER(struct scatterlist *sg, 
	unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags);
int DRV_SD_MULTI_TRANSFER(unsigned dev_addr, 
	unsigned blocks,unsigned blksz,int write);
static inline void DRV_SDMMC_CLEAR_WHOLE_SCREEN(void)
{
	return;
}

#endif


#endif




#ifndef __OSL_CACHE_H
#define __OSL_CACHE_H

#ifdef __KERNEL__
#include <linux/dma-mapping.h>

#elif defined(__VXWORKS__)
#include <cacheLib.h>

enum dma_data_direction {
	DMA_BIDIRECTIONAL = 0,
	DMA_TO_DEVICE = 1,
	DMA_FROM_DEVICE = 2,
	DMA_NONE = 3,
};


static __inline__ void __dma_single_cpu_to_dev(const void *kaddr, size_t size,
	enum dma_data_direction dir)
{
	int dma_ret = cacheFlush(DATA_CACHE, kaddr, size);
	if(dma_ret != OK)
	{
		logMsg("cacheFlush error\n", 0, 0, 0, 0, 0, 0);
	}
}

static __inline__ void __dma_single_dev_to_cpu(const void *kaddr, size_t size,
	enum dma_data_direction dir)
{
	int dma_ret = cacheInvalidate(DATA_CACHE, kaddr, size);
	if(dma_ret != OK)
	{
		logMsg("cacheInvalidate error\n", 0, 0, 0, 0, 0, 0);
	}
}

#elif defined(__CMSIS_RTOS)

#else

#endif /* __KERNEL__ */

#endif


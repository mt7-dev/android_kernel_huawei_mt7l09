#ifndef __LOAD_M_H__
#define __LOADM_H__

#define CACHE_DMA_TO_DEVICE                 (1)

#define LOAD_MODEM_OK_FLAG      (0xABCD5132)
#define LOAD_MODEM_ERROR_FLAG   (0xFFFFFFFF)

typedef enum tagDEBUG_LEVEL_E{
    HI_ERR = 0,
    HI_INFO,
    HI_WARN,
    HI_DEBUG,
    HI_MAX
}DEBUG_LEVEL_E;

extern long g_debug_level;

#define HIS_IMAGE_TRACE_LOG           (g_debug_level)
#define hi_trace(level, format, args...) do { \
	if((level) <= HIS_IMAGE_TRACE_LOG) { \
		printk("%s:%d: "format,  __FUNCTION__,  __LINE__, ## args); \
	} \
}while(0)

#define REG_GETBIT32(addr, pos, bits)     \
                 ((*(volatile u32 *)(addr)  >> (pos)) & \
                 (((u32)1 << (bits)) - 1))

/*add for ccore image length parse*/
#define CCORE_IMAGE_NAME "VXWORKS"
#endif


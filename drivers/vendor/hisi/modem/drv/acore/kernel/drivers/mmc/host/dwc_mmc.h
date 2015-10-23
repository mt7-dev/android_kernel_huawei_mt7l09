#ifndef _HI_MCI_H_
#define _HI_MCI_H_

#ifndef BSP_CONFIG_P531_ASIC
#define CONFIG_MMC_V7R2
#endif

/*for fgpa low hz*/
//#define CONFIG_MMC_V7R2_FPGA

/*for mmc0-sd else mmc0-sdio*/
/*#define CONFIG_MMC_SDIO_LOOP*/

#ifdef CONFIG_MMC_V7R2
#ifdef CONFIG_MMC_V7R2_FPGA
/*sd*/
#define MMC0_CCLK			    	UHS_SDR25_MAX_DTR
/*wifi*/
#define MMC1_CCLK			    	UHS_SDR25_MAX_DTR
#else
/*sd*/
#define MMC0_CCLK			    	UHS_SDR50_MAX_DTR /*sdio card 96MHz*/
/*wifi*/
#define MMC1_CCLK			    	UHS_SDR104_MAX_DTR
#endif
#endif

#define MMC_CLOCK_SOURCE	480000000

#define MMC_RESOURCES_SIZE	SZ_4K

#define HI_MAX_REQ_SIZE     (128*1024)

/* maximum size of one mmc block */
#define MAX_BLK_SIZE        512

/* maximum number of bytes in one req */
#define MAX_REQ_SIZE        HI_MAX_REQ_SIZE

/* maximum number of blocks in one req */
#define MAX_BLK_COUNT       (HI_MAX_REQ_SIZE/512)

/* maximum number of segments, see blk_queue_max_phys_segments */
#define MAX_SEGS            (HI_MAX_REQ_SIZE/512)

/* maximum size of segments, see blk_queue_max_segment_size */
#define MAX_SEG_SIZE        HI_MAX_REQ_SIZE

#ifdef CONFIG_MMC_SDIO_LOOP
#define SDIO_VENDOR_ID_ARASAN			0x0296
#define SDIO_DEVICE_ID_ARASAN_COMBO		0x5437
#endif

#define DBG(host, fmt, ...)                   \
do { \
	dev_dbg(host->dev, "[%s] "fmt, __func__, ##__VA_ARGS__); \
} while(0)

#define ERROR(host, fmt, ...)                   \
do { \
	dev_err(host->dev, "[%s] "fmt, __func__, ##__VA_ARGS__); \
} while(0)
	
#define INFO(host, fmt, ...)                   \
do { \
	 dev_info(host->dev, "[%s] "fmt, __func__, ##__VA_ARGS__); \
} while(0)


#endif

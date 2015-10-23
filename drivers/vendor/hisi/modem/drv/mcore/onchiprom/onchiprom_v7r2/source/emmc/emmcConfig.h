#ifndef __EMMC_CONFIG_H__
#define __EMMC_CONFIG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "sys.h"
#include "config.h"
#include "OnChipRom.h"

#pragma pack(4)

#define EMMC_CARD_NUM           (1)
#define EMMC_CURRENT_SLOT       (0)
#define EMMC_CURRENT_CLKSRC     (0)

#define EMMC_SEC_SIZE           (512)
#define EMMC_READ_FLAG          (0x16)
#define EMMC_WRITE_FLAG         (0x96)

/* pmu寄存器 */
#define EMMC_PMU_LDO10_ADDR     (0xE)
#define EMMC_PMU_LDO10_ON       (0x20)
#define EMMC_PMU_LDO10_OFF      (0x00)
#define EMMC_PMU_LDO10_MASK     (0x30)

/*
 * if idmac support
 */
#define EMMC_IDMAC_SUPPORT

/*
 * emmc timer macro define
 */

/*
 * emmc timeout macro define.
 * The unit is msecond. add 10ms for each timeout value.
 */    
#define EMMC_RESET_IP_TIMEOUT           (100 + 10)
#define EMMC_RESET_FIFO_TIMEOUT         (50 + 10)
#define EMMC_RESET_IDMAC_TIMEOUT        (50  + 10)
#define EMMC_BOOT_ACK_TIMEOUT           (50  + 10)    /* The boot ack timeout is 50 ms, see MMC4.3/MMC4.4 for more information */
#define EMMC_BOOT_DATA_START_TIMEOUT    (1000 + 10)    /* The boot data start timeout is 1000 ms, see MMC4.3/MMC4.4 */
#define EMMC_BOOT_DATA_READ_TIMEOUT     (300  + 10)    /* It is large than The read one sector timeout, see MMC4.3/MMC4.4 */
#define EMMC_CMD_ACCEPT_TIMEOUT         (200  + 10)    /* The command accept by the IP */
#define EMMC_CMD_COMPLETE_TIMEOUT       (3000 + 10)    /* The total command execute time, 3 second for read 16K */
#define EMMC_DATA0_READY_TIMEOUT        (300  + 10)    /* The timeout while card programing */

/*
    The distance is 10ms.
*/
#define EMMC_CMD_DISTANCE               10              /* 10 ms */
#define EMMC_CMD0_RETRY_COUNT           10               /* total time is 5  * 10ms = 50ms  */
#define EMMC_CMD1_QUERY_OCR_COUNT       10       
#define EMMC_CMD1_WAIT_READY_COUNT      200             /* total time is 200 * 10ms = 2000ms */
#define EMMC_CMD2_RETRY_COUNT           10
#define EMMC_CMD3_RETRY_COUNT           10
#define EMMC_CMD8_RETRY_COUNT           10
#define EMMC_ACMD41_RETRY_COUNT         10

/*
 * Config the emmc function.显式配置MMC时序优化不使能
 */
#if PLATFORM==PLATFORM_P530_PORTING
#define EMMC_SYS_CONFIG() \
{\
        CLRREG32(SC_CTRL3,1<<6);\
}

#else   /* Polit */
#define EMMC_SYS_CONFIG() \
{\
	    CLRREG32(SC_CTRL3,1<<5) ;\
}
#endif
#pragma pack(0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of emmcConfig.h */

/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  sdio_slave.h
*
*   作    者 :  xumushui
*
*   描    述 :  slave模块配置信息
*
*   修改记录 :  2011年8月27日  v1.00  xumushui      创建
*
*************************************************************************/
#ifndef __SLAVE_CFG_H__
#define __SLAVE_CFG_H__

#ifdef __cplusplus
extern "C"
{
#endif
#define CSA_BUF_SIZE            0x400   /*(1*1024)*/
/*接收buffer大小*/
//#define DATA_BUF_SIZE           0x8000  /*(32*1024)，共享内存最大只能分8K*/
#define DATA_BUF_SIZE           0x10000  /* 512K */
/* 数据最大包长度 */
#define SLAVE_TRANS_PKT_SIZE    0x80000 /*(512*1024)，由于IP限制，单次传数最大只能传512KB*/

/* 每个描述符项buffer大小 */
#define ADMA_BUF_MAXSIZE        0x2000  /*(8*1024)*/

/* 预分配描述符个数 */
#define ADMA_DESC_LINE_COUNT    (SLAVE_TRANS_PKT_SIZE/ADMA_BUF_MAXSIZE)
#define ADMA_DESC_FUN0_COUNT     2

/* 描述符存储类型 */
#define ADMA_DESC_MEM_CACHEABLE
//#define ADMA_DESC_MEM_NOCACHEABLE
//#define ADMA_DESC_MEM_AXI

/*SLAVE debug 宏*/

/* 描述符cache操作 */
#ifdef ADMA_DESC_MEM_CACHEABLE
#define ADMA_DESC_CACHE   /* if desc need synchronization cache */
#endif

/* 数据cache操作 */
#define TX_BUF_FLUSH_CACHE      /* 是否flush cache */
#define RX_BUF_INVALIDATE_CACHE /* 是否valid cache */

#define SLAVE_CATALOG_IOONLY        (0)
#define SLAVE_CATALOG_COMBO         (1)
#define SLAVE_CATALOG_MEMONLY       (2)
#define SLAVE_CATALOG_MAX           (3)

#define SLAVE_CFG_CATALOG           (SLAVE_CATALOG_IOONLY)

#define SLAVE_CFG_INT_TASK_NAME     "tSlaveIntrProc"
#define SLAVE_CFG_INT_TASK_PRI      (142)
#define SLAVE_CFG_INT_TASK_STACK    (0x1000)

#define SLAVE_CFG_MSG_NUM           (64)

/*interrupt define*/
#define INT_VEC_SDIO_SLAVE  INT_LVL_SDCC    	//INT_LVL_SDCC  
#define FUN_CNT             0x2
#define SLAVE_REG_SIZE HI_MMC_DEV_REG_SIZE		//SLAVE_REG_SIZE_1k

#define SD_DEV_REGBASE_ADR              HI_MMC_DEV_REGBASE_ADDR

#define ASIC_SYSTEM_BASE    HI_SYSCTRL_BASE_ADDR_VIRT
#define SDIO_TIMER_REG        0x454         //自动门控时钟
#define SDIO_CONTROL_REG      0x958
#define SDIO_GPIO_REG         0x988

#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_GLOBAL_H_*/


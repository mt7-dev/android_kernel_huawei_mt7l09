

/************************************************************
                     包含其它模块的头文件
************************************************************/
/************************************************************
                               宏定义
************************************************************/

/************************************************************
                             数据结构定义
************************************************************/

/************************************************************
                             接口函数声明
 ************************************************************/

#ifndef __TL_PHY_INTERFACE_H__
#define __TL_PHY_INTERFACE_H__

#include "product_config.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */




#if (defined(MS_VC6_PLATFORM))
    #include "sim.h"

    #define MASTER_TCM_ADDR_OFFSET                  (0x0)
    #define SLAVE_TCM_ADDR_OFFSET                   (MASTER_TCM_ADDR_OFFSET)


    #ifdef COMPILE_PUBLIC_IMAGE
    /*邮箱数组定义*/
    extern UINT32  gulArrMailxBox[];                /*400k字节大小*/

    /*HARQ缓冲区数组大小*/
    extern UINT32  gulArrHarqBuf[];                 /*400k字节大小*/

    /*模拟邮箱*/
    #define LPHY_SHARE_MEM_BASE_ADDR                ((UINT32)&gulArrMailxBox[0])

    /*模拟HARQ缓冲区*/
    #define LPHY_LMAC_HARQ_BUFF_BASE_ADDR           ((UINT32)&gulArrHarqBuf[0])
    #else
    extern REG_BASE_ADDR_STRU gstRegBaseAddr;
    /*模拟邮箱*/
    #define LPHY_SHARE_MEM_BASE_ADDR                gstRegBaseAddr.gulArrMailxBoxAddr//((UINT32)&gulArrMailxBox[0])

    /*模拟HARQ缓冲区*/
    #define LPHY_LMAC_HARQ_BUFF_BASE_ADDR           gstRegBaseAddr.gulArrHarqBufAddr//((UINT32)&gulArrHarqBuf[0])
    #endif

    #define LPHY_DSP_ARM_RAM_BASE_OFFSET            (0x00000000UL)


    #define LPHY_CHIP_BB_6930CS
    #define LPHY_RFIC_HI6361

/* 基地址定义*/
#elif defined(TL_PHY_FPGA_P500)

    #define MASTER_TCM_ADDR_OFFSET  				(0x0 - 0xf000000)
    #define SLAVE_TCM_ADDR_OFFSET   				(0x0 - 0xd000000)

    /*邮箱共享基地定义*/
    #define LPHY_SHARE_MEM_BASE_ADDR                (0xE0FC0000UL)
    #define LPHY_DSP_ARM_RAM_BASE_OFFSET            (0x0F000000UL)

    /*HARQ缓冲区基地址定义*/
    #define LPHY_LMAC_HARQ_BUFF_BASE_ADDR           (0x52680000UL)
    #define LPHY_LMAC_TIMER_LOCK_ADDR               (0x5270E094UL)
    #define LPHY_LMAC_FRAME_ADDR                    (0x5270E014UL)
    #define LPHY_LMAC_SUB_FRAME_ADDR                (0x5270E018UL)

    /*加载地址，供BSP用*/
    #define LPHY_CORE0_PUB_DTCM_LOAD_ADDR           (0xE0FC0000UL) /*P500 bbe16 core0*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_ADDR           (0xE1000000UL) /*P500 bbe16 core0*/
	#define LPHY_CORE1_PUB_DTCM_LOAD_ADDR           (0xE2FC0000UL) /*P500 bbe16 core1*/
    #define LPHY_CORE1_PUB_ITCM_LOAD_ADDR           (0xE3000000UL) /*P500 bbe16 core1*/

    /*映射地址，供vxworks 映射使用*/
    #define LPHY_CORE0_TCM_MAP_BASE_ADDR            (LPHY_CORE0_PUB_DTCM_LOAD_ADDR)
	#define LPHY_CORE1_TCM_MAP_BASE_ADDR            (LPHY_CORE1_PUB_DTCM_LOAD_ADDR)
	#define LPHY_CORE0_TCM_MAP_SIZE                 (256*1024 + 256*1024) /*256K + 256K*/
    #define LPHY_CORE1_TCM_MAP_SIZE                 (256*1024 + 256*1024) /*256K + 256K*/

	#define LPHY_CHIP_BB_6930ES
	#define LPHY_RFIC_HI6361

#elif defined(TL_PHY_FPGA_P531)

	#define MASTER_TCM_ADDR_OFFSET  				(0x01000000)
	#define SLAVE_TCM_ADDR_OFFSET   				(0x02000000)

	#define BBE_TCM_IN_SOC_SAPCE(x) 				(MASTER_TCM_ADDR_OFFSET + (x))

	/*邮箱共享基地定义*/
	#define LPHY_SHARE_MEM_BASE_ADDR                BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_DTCM_BASE)
	#define LPHY_DSP_ARM_RAM_BASE_OFFSET            (0xFF000000UL)

	/*HARQ缓冲区基地址定义*/
	#define LPHY_LMAC_HARQ_BUFF_BASE_ADDR           (0x20680000UL)
	#define LPHY_LMAC_TIMER_LOCK_ADDR               (0x2070E094UL)
	#define LPHY_LMAC_FRAME_ADDR                    (0x2070E014UL)
	#define LPHY_LMAC_SUB_FRAME_ADDR                (0x2070E018UL)



	/*加载地址，供BSP用*/
	#define LPHY_CORE0_UNI_DTCM_LOAD_ADDR           BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_DTCM_BASE) 	/*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_UNI_DTCM_LOAD_SIZE           (LPHY_UNI_DTCM_SIZE) 					 	/*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_UNI_ITCM_LOAD_ADDR			BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_ITCM_BASE) 	/*P530 bbe16 UNIPHY ITCM*/
	#define LPHY_CORE0_UNI_ITCM_LOAD_SIZE			(LPHY_UNI_ITCM_SIZE) 						/*P530 bbe16 UNIPHY DTCM*/


	#define LPHY_CORE0_PUB_DTCM_LOAD_ADDR           BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_DTCM_BASE) 	/*P530 bbe16 UNIPHY DTCM*/ /*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_PUB_DTCM_LOAD_SIZE           (LPHY_PUB_DTCM_SIZE) 						/*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_ADDR           BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_ITCM_BASE) 	/*P530 bbe16 PUBPHY ITCM*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_SIZE           (LPHY_PUB_ITCM_SIZE) 						/*P530 bbe16 PUBPHY DTCM*/

	#define LPHY_CORE0_MOD_DTCM_LOAD_ADDR			BBE_TCM_IN_SOC_SAPCE(LPHY_PRV_DTCM_BASE) 	/*P530 bbe16 MODPHY DTCM*/
	#define LPHY_CORE0_MOD_DTCM_LOAD_SIZE			(LPHY_PRV_DTCM_SIZE) 						/*P530 bbe16 MODPHY DTCM*/
	#define LPHY_CORE0_MOD_ITCM_LOAD_ADDR           BBE_TCM_IN_SOC_SAPCE(LPHY_PRV_ITCM_BASE) 	/*P530 bbe16 MODPHY ITCM*/
	#define LPHY_CORE0_MOD_ITCM_LOAD_SIZE           (LPHY_PRV_ITCM_SIZE) 						/*P530 bbe16 MODPHY DTCM*/


	/*映射地址，供vxworks 映射使用*/
	#define LPHY_CORE_TCM_MAP_BASE_ADDR             (LPHY_CORE0_PUB_DTCM_LOAD_ADDR)
	#define LPHY_CORE_TCM_MAP_SIZE                  (LPHY_UNI_DTCM_SIZE + LPHY_UNI_ITCM_SIZE)

	#define LPHY_CHIP_BB_6930ES
	#define LPHY_RFIC_HI6361
#elif defined(CHIP_BB_HI6210)

    //#error "goto CHIP_BB_HI6210"

	#define MASTER_TCM_ADDR_OFFSET  				(0x00000000)
	#define SLAVE_TCM_ADDR_OFFSET   				(0x00000000)
   	#define BBE_TCM_IN_SOC_SAPCE(x) 				(MASTER_TCM_ADDR_OFFSET + (x))


	/*邮箱共享基地定义*/
	#define LPHY_SHARE_MEM_BASE_ADDR                BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_DTCM_BASE) //!!!!
	#define LPHY_DSP_ARM_RAM_BASE_OFFSET            (0x00000000UL) //honghuiyong

	/*HARQ缓冲区基地址定义*/
	#define LPHY_LMAC_HARQ_BUFF_BASE_ADDR           (0xFD680000UL)
	#define LPHY_LMAC_TIMER_LOCK_ADDR               (0xFD70E094UL)
	#define LPHY_LMAC_FRAME_ADDR                    (0xFD70E014UL)
	#define LPHY_LMAC_SUB_FRAME_ADDR                (0xFD70E018UL)

	#define LPHY_CORE0_UNI_DTCM_LOAD_ADDR           BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_DTCM_BASE) 	/*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_UNI_DTCM_LOAD_SIZE           (LPHY_UNI_DTCM_SIZE) 					 	/*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_UNI_ITCM_LOAD_ADDR			BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_ITCM_BASE) 	/*P530 bbe16 UNIPHY ITCM*/
	#define LPHY_CORE0_UNI_ITCM_LOAD_SIZE			(LPHY_UNI_ITCM_SIZE) 						/*P530 bbe16 UNIPHY DTCM*/


	#define LPHY_CORE0_PUB_DTCM_LOAD_ADDR           BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_DTCM_BASE) 	/*P530 bbe16 UNIPHY DTCM*/ /*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_PUB_DTCM_LOAD_SIZE           (LPHY_PUB_DTCM_SIZE) 						/*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_ADDR           BBE_TCM_IN_SOC_SAPCE(LPHY_PUB_ITCM_BASE) 	/*P530 bbe16 PUBPHY ITCM*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_SIZE           (LPHY_PUB_ITCM_SIZE) 						/*P530 bbe16 PUBPHY DTCM*/

	#define LPHY_CORE0_MOD_DTCM_LOAD_ADDR			BBE_TCM_IN_SOC_SAPCE(LPHY_PRV_DTCM_BASE) 	/*P530 bbe16 MODPHY DTCM*/
	#define LPHY_CORE0_MOD_DTCM_LOAD_SIZE			(LPHY_PRV_DTCM_SIZE) 						/*P530 bbe16 MODPHY DTCM*/
	#define LPHY_CORE0_MOD_ITCM_LOAD_ADDR           BBE_TCM_IN_SOC_SAPCE(LPHY_PRV_ITCM_BASE) 	/*P530 bbe16 MODPHY ITCM*/
	#define LPHY_CORE0_MOD_ITCM_LOAD_SIZE           (LPHY_PRV_ITCM_SIZE) 						/*P530 bbe16 MODPHY DTCM*/


	/*映射地址，供vxworks 映射使用*/
	#define LPHY_CORE_TCM_MAP_BASE_ADDR             (LPHY_CORE0_PUB_DTCM_LOAD_ADDR)
	#define LPHY_CORE_TCM_MAP_SIZE                  (512*1024 + 512*1024) /*256K + 256K*/

	#define LPHY_CHIP_BB_6930ES
	#define LPHY_RFIC_HI6361
#elif (defined(TL_PHY_ASIC_HI6930) || defined(TL_PHY_ASIC_BIGPACK) ||defined(TL_PHY_SFT))

    #define MASTER_TCM_ADDR_OFFSET                  (0x0)
    #define SLAVE_TCM_ADDR_OFFSET                   (MASTER_TCM_ADDR_OFFSET)

    /*邮箱共享基地定义*/
    #define LPHY_SHARE_MEM_BASE_ADDR                (0x48f80000UL) /*128K+256K*/
    #define LPHY_DSP_ARM_RAM_BASE_OFFSET            (0x00000000UL)

    /*HARQ缓冲区基地址定义*/
    #define LPHY_LMAC_HARQ_BUFF_BASE_ADDR           (0x20680000UL)
    #define LPHY_LMAC_TIMER_LOCK_ADDR               (0x2070E094UL)
    #define LPHY_LMAC_FRAME_ADDR                    (0x2070E014UL)
    #define LPHY_LMAC_SUB_FRAME_ADDR                (0x2070E018UL)

	/*加载地址，供BSP用*/
	#define LPHY_CORE0_UNI_DTCM_LOAD_ADDR           (0x48f80000UL) /*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_UNI_DTCM_LOAD_SIZE           (0x00080000UL) /*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_PUB_DTCM_LOAD_ADDR           (0x48f80000UL) /*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_PUB_DTCM_LOAD_SIZE           (0x0003f000UL) /*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_MOD_DTCM_LOAD_ADDR           (0x48fbf000UL) /*P530 bbe16 MODPHY DTCM*/
	#define LPHY_CORE0_MOD_DTCM_LOAD_SIZE           (0x00040000UL) /*P530 bbe16 MODPHY DTCM*/

	#define LPHY_CORE0_UNI_ITCM_LOAD_ADDR           (0x49000000UL) /*P530 bbe16 UNIPHY ITCM*/
	#define LPHY_CORE0_UNI_ITCM_LOAD_SIZE           (0x00080000UL) /*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_ADDR           (0x48fff000UL) /*P530 bbe16 PUBPHY ITCM*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_SIZE           (0x00041000UL) /*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_MOD_ITCM_LOAD_ADDR           (0x49040000UL) /*P530 bbe16 MODPHY ITCM*/
	#define LPHY_CORE0_MOD_ITCM_LOAD_SIZE           (0x00040000UL) /*P530 bbe16 MODPHY DTCM*/


	/*映射地址，供vxworks 映射使用*/
	#define LPHY_CORE_TCM_MAP_BASE_ADDR             (LPHY_CORE0_PUB_DTCM_LOAD_ADDR)
	#define LPHY_CORE_TCM_MAP_SIZE                  (512*1024 + 512*1024) /*256K + 256K*/

	#define LPHY_CHIP_BB_6930CS
	#define LPHY_RFIC_HI6361

#elif (defined(TL_PHY_ASIC_K3V3) || defined(TL_PHY_K3V3_SFT))


    #define MASTER_TCM_ADDR_OFFSET                  (0x0)
    #define SLAVE_TCM_ADDR_OFFSET                   (MASTER_TCM_ADDR_OFFSET)

    /*邮箱共享基地定义*/
    #define LPHY_SHARE_MEM_BASE_ADDR                (0xe2780000UL) /*128K+256K*/
    #define LPHY_DSP_ARM_RAM_BASE_OFFSET            (0x00000000UL)

    /*HARQ缓冲区基地址定义*/
    #define LPHY_LMAC_HARQ_BUFF_BASE_ADDR           (0xE1680000UL)
    #define LPHY_LMAC_TIMER_LOCK_ADDR               (0xE170E094UL)
    #define LPHY_LMAC_FRAME_ADDR                    (0xE170E014UL)
    #define LPHY_LMAC_SUB_FRAME_ADDR                (0xE170E018UL)

	/*加载地址，供BSP用*/
	#define LPHY_CORE0_UNI_DTCM_LOAD_ADDR           (0xe2780000UL) /*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_UNI_DTCM_LOAD_SIZE           (0x00080000UL) /*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_PUB_DTCM_LOAD_ADDR           (0xe2780000UL) /*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_PUB_DTCM_LOAD_SIZE           (0x0003f000UL) /*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_MOD_DTCM_LOAD_ADDR           (0xe27bf000UL) /*P530 bbe16 MODPHY DTCM*/
	#define LPHY_CORE0_MOD_DTCM_LOAD_SIZE           (0x00040000UL) /*P530 bbe16 MODPHY DTCM*/

	#define LPHY_CORE0_UNI_ITCM_LOAD_ADDR           (0xe2800000UL) /*P530 bbe16 UNIPHY ITCM*/
	#define LPHY_CORE0_UNI_ITCM_LOAD_SIZE           (0x00080000UL) /*P530 bbe16 UNIPHY DTCM*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_ADDR           (0xe27ff000UL) /*P530 bbe16 PUBPHY ITCM*/
	#define LPHY_CORE0_PUB_ITCM_LOAD_SIZE           (0x00041000UL) /*P530 bbe16 PUBPHY DTCM*/
	#define LPHY_CORE0_MOD_ITCM_LOAD_ADDR           (0xe2840000UL) /*P530 bbe16 MODPHY ITCM*/
	#define LPHY_CORE0_MOD_ITCM_LOAD_SIZE           (0x00040000UL) /*P530 bbe16 MODPHY DTCM*/


	/*映射地址，供vxworks 映射使用*/
	#define LPHY_CORE_TCM_MAP_BASE_ADDR             (LPHY_CORE0_PUB_DTCM_LOAD_ADDR)
	#define LPHY_CORE_TCM_MAP_SIZE                  (512*1024 + 512*1024) /*256K + 256K*/

	#define LPHY_CHIP_BB_6930CS
	#define LPHY_RFIC_HI6361

#else

    #error "sorry, this is not a supported board version, please include product_config.h"

#endif

typedef enum __LPHY_WORKING_MODE_ENUM__
{
    LPHY_WORKING_MODE_RTT = 0,
    LPHY_WORKING_MODE_SINGLE_MODE  = 1,
    LPHY_WORKING_MODE_MULTI_MODE   = 2,
    LPHY_WORKING_MODE_DUO_SIM_MODE = 3,
}LPHY_WORKING_MODE_ENUM;

typedef enum __LPHY_RTT_LPC_MODE_ENUM__
{
	POWER_SAVING_NULL,
	POWER_SAVING_SNOOZE,
	POWER_SAVING_DEEP_SLEEP
}LPHY_RTT_LPC_MODE_ENUM;


typedef struct __MULTIMODE_BB_STARTUP_MODE_IND_STRU__
{
	unsigned int bLteStartInd	:4;		//0:Inactive,1:Active
	unsigned int bTdsStartInd	:4;		//0:Inactive,1:Active
	unsigned int bReserved		:24;
}MULTIMODE_BB_STARTUP_MODE_IND_STRU;

/*HARQ缓冲区地址*/
#if (defined(LPHY_CHIP_BB_6930ES) || defined(LPHY_CHIP_BB_6930CS))
    #define LPHY_LMAC_HARQ_BUFF_ADDR_BY_HARQID(ucHarqId) (LPHY_LMAC_HARQ_BUFF_BASE_ADDR + (ucHarqId*0x4000))
#else
    #error "sorry, this is not a supported baseband version, please include product_config.h"
#endif

typedef struct __TLPHY_DDR_ADDR_INFO_STRU__
{
	unsigned long ulImageAddr;
	unsigned long ulImageSize;
	unsigned long ulSdrAddr;
	unsigned long ulSdrSize;
	unsigned long ulLcsAddr;
	unsigned long ulLcsSize;
	unsigned long ulBandNvAddr;
	unsigned long ulBandNvSize;
	unsigned long ulSramAddr;
	unsigned long ulSramSize;
	unsigned long ulTempAddr;
	unsigned long ulTempSize;
	unsigned long ulSramSleepFlagAddr;
	unsigned long ulSramSleepFlagSize;
	unsigned long ulTimeAddr;
	unsigned long ulTimeSize;
	unsigned long ulSramMailBoxProtectAddr;
	unsigned long ulSramMailBoxProtectSize;
	unsigned long ulAFCAddr;
	unsigned long ulAFCSize;
	unsigned long ulRsv[2];
}TLPHY_DDR_ADDR_INFO_STRU;

/* 共享邮箱大小定义*/
#define LPHY_MAILBOX_SHARE_MEM_SIZE                 (0x00000400UL)  /*1k字节，TBD*/
#define LPHY_MAILBOX_HDLC_BUF_SIZE                  (0x00002000UL)  /*8K字节, TBD*/
#define LPHY_MAILBOX_LHPA_UP_SIZE                   (0x00002000UL)  /*8K字节，PS上行邮箱大小*/
#define LPHY_MAILBOX_LHPA_DOWN_SIZE                 (0x00002000UL)  /*8k字节，PS下行邮箱大小*/
#define LPHY_MAILBOX_OM_DOWN_SIZE                   (0x00000800UL)  /*2k字节，OM下行邮箱大小*/
#define LPHY_MAILBOX_LMAC_UP_SIZE                   (0x00000400UL)  /*1k字节，MAC专用邮箱大小*/
#define LPHY_MAILBOX_LMAC_DOWN_SIZE                 (0x00000400UL)  /*1k字节，MAC专用邮箱大小*/
#define LPHY_MAILBOX_DLTB_QUEUE_SIZE                (0x00000400UL)  /*1k字节，MACPHY下行TB QUEUE处理大小*/
#define LPHY_MAILBOX_LPP_DOWN_SIZE                  (0x00000A00UL)  /*2.5字节，LPP 定位辅助信息邮箱大小*/
#define LPHY_MAILBOX_TOA_SIZE                       (0x00000600UL)  /*1.5k字节，TOA邮箱大小*/
#ifdef FEATURE_TLPHY_SINGLE_XO
#define LTPHY_NV_COMM_CFG_SIZE						(0x00000400UL)  /*1k字节，LT PHY 公共NV 大小*/
#else
#define LTPHY_NV_COMM_CFG_SIZE						(0x00000000UL)
#endif
#define LPHY_NV_CFG_SIZE                            (0x00002000UL)  /*8k字节，LTE NV项配置区大小*/
#define TPHY_NV_CFG_SIZE                            (0x00002000UL)  /*8k字节，TDS NV项配置区大小*/

#if defined(TL_PHY_FPGA_P500)
    #define LPHY_MAILBOX_LCS_BANKA_MEM_SIZE         (0x00000000UL)  /*LPHY_HI6920ES或者 fpga不保留这块内存*/
	#define LPHY_MAILBOX_CSU_BANKA_MEM_SIZE         (0x00003000UL)  /*小区搜索软基带 banka memory 12K*/
	#define LPHY_MAILBOX_STU_BANKA_MEM_SIZE			(0x00000400UL)	/*STU软基带 banka memory 12K*/
	#define LPHY_MAILBOX_RSV_BANKA_MEM_SIZE			(0x00000000UL)	/*STU软基带 banka memory 16K*/
#else
    #define LPHY_MAILBOX_LCS_BANKA_MEM_SIZE         (0x00008000UL)  /*24K字节, 用于  pliot版本lcs计算使用*/
	#define LPHY_MAILBOX_CSU_BANKA_MEM_SIZE         (0x00003000UL)  /*LPHY_HI6920ES或者 fpga不保留这块内存*/
	#define LPHY_MAILBOX_STU_BANKA_MEM_SIZE			(0x00000800UL)	/*STU软基带 banka memory 2K*/
	#define LPHY_MAILBOX_RSV_BANKA_MEM_SIZE			(0x00000000UL)	/*STU软基带 banka memory 16K*/
#endif



/*邮箱总大小*/
#define LPHY_TOTAL_RESERVED_MEM_SIZE    ( \
      LPHY_MAILBOX_SHARE_MEM_SIZE \
    + LPHY_MAILBOX_HDLC_BUF_SIZE \
    + LPHY_MAILBOX_LHPA_UP_SIZE   \
    + LPHY_MAILBOX_LHPA_DOWN_SIZE \
    + LPHY_MAILBOX_OM_DOWN_SIZE  \
    + LPHY_MAILBOX_LMAC_UP_SIZE  \
    + LPHY_MAILBOX_LMAC_DOWN_SIZE \
    + LPHY_MAILBOX_DLTB_QUEUE_SIZE\
    + LPHY_MAILBOX_LPP_DOWN_SIZE \
    + LPHY_MAILBOX_TOA_SIZE \
    + LTPHY_NV_COMM_CFG_SIZE\
    + LPHY_NV_CFG_SIZE\
    + TPHY_NV_CFG_SIZE\
    + LPHY_MAILBOX_LCS_BANKA_MEM_SIZE \
    + LPHY_MAILBOX_CSU_BANKA_MEM_SIZE \
    + LPHY_MAILBOX_STU_BANKA_MEM_SIZE)\
    + LPHY_MAILBOX_RSV_BANKA_MEM_SIZE

/*邮箱基地址定义*/
#if (defined(TENSILICA_PLATFORM) || defined(MS_VC6_PLATFORM))
    #define LPHY_MAILBOX_BASE_ADDR                  (LPHY_SHARE_MEM_BASE_ADDR - MASTER_TCM_ADDR_OFFSET)
#else
    #define LPHY_MAILBOX_BASE_ADDR                  (LPHY_SHARE_MEM_BASE_ADDR)
#endif

/*外部系统访问各个邮箱基地址*/
#define LPHY_MAILBOX_VERSION_ADDR                   (LPHY_MAILBOX_BASE_ADDR + 0x0200)   /*DSP版本查询地址*/
#define LPHY_MAILBOX_LOAD_MEMORY_ADDR               (LPHY_MAILBOX_BASE_ADDR + 0x020C)   /*开机初始化握手地址*/
#define LPHY_MAILBOX_RFIC_AD9361_VERSIOM_ADDR       (LPHY_MAILBOX_BASE_ADDR + 0x0210)   /*AD9361射频芯片版本查询地址*/
#define TLPHY_DDR_ADDR_INFO							(LPHY_MAILBOX_BASE_ADDR + 0x0214)	/*TL PHY 在DDR地址，包括IMAGE SDR LCS BANDNV。64B*/
#define LPHY_MAILBOX_IPCM_MUTEX_DSP_SEND            (LPHY_MAILBOX_BASE_ADDR + 0x0324)   /*P500和A9IPC通讯互斥量地址*/
#define LPHY_MAILBOX_IPCM_MUTEX_DSP_RECEIVE         (LPHY_MAILBOX_BASE_ADDR + 0x0328)   /*P500和A9IPC通讯互斥量地址*/
#define LPHY_MAILBOX_SLAVE_AWAKE_ADDR               (LPHY_MAILBOX_BASE_ADDR + 0x032C)   /*从模PS IPCM任务状态地址*/
#define LPHY_MAILBOX_LPHY_WORKING_MODE_ADDR         (LPHY_MAILBOX_BASE_ADDR + 0x0340)   /*LTE PHY WorkModeInd*/
#define LPHY_MAILBOX_LPHY_CTU_REG_BASE_ADDR         (LPHY_MAILBOX_BASE_ADDR + 0x0344)   /*LTE 编译模式*/
#define LPHY_MAILBOX_LPHY_DSP_NV_PARA_SIZE_ADDR     (LPHY_MAILBOX_BASE_ADDR + 0x0348)   /*LTE PHY DSP 编译时 NV Para_Size*/
#define LPHY_MAILBOX_LPHY_ARM_NV_PARA_SIZE_ADDR     (LPHY_MAILBOX_BASE_ADDR + 0x034C)   /*LTE PHY ARM下发的  NV Para_Size*/
#define LPHY_MAILBOX_MULTIMODE_STARTUP_MODE_ADDR    (LPHY_MAILBOX_BASE_ADDR + 0x0350)   /*多模基带启动模式指示*/

#define LPHY_MAILBOX_HDLC_BUF_BASE_ADDR             (LPHY_MAILBOX_BASE_ADDR 			+ LPHY_MAILBOX_SHARE_MEM_SIZE)
#define LPHY_MAILBOX_LHPA_UP_BASE_ADDR              (LPHY_MAILBOX_HDLC_BUF_BASE_ADDR 	+ LPHY_MAILBOX_HDLC_BUF_SIZE)
#define LPHY_MAILBOX_LHPA_DOWN_BASE_ADDR            (LPHY_MAILBOX_LHPA_UP_BASE_ADDR 	+ LPHY_MAILBOX_LHPA_UP_SIZE)
#define LPHY_MAILBOX_OM_DOWN_BASE_ADDR              (LPHY_MAILBOX_LHPA_DOWN_BASE_ADDR 	+ LPHY_MAILBOX_LHPA_DOWN_SIZE)
#define LPHY_MAILBOX_LMAC_UP_BASE_ADDR              (LPHY_MAILBOX_OM_DOWN_BASE_ADDR 	+ LPHY_MAILBOX_OM_DOWN_SIZE)
#define LPHY_MAILBOX_LMAC_DOWN_BASE_ADDR            (LPHY_MAILBOX_LMAC_UP_BASE_ADDR 	+ LPHY_MAILBOX_LMAC_UP_SIZE)
#define LPHY_MAILBOX_DL_TB_QUEUE_BASE_ADDR          (LPHY_MAILBOX_LMAC_DOWN_BASE_ADDR 	+ LPHY_MAILBOX_LMAC_DOWN_SIZE)
#define LPHY_MAILBOX_LPP_DOWN_BASE_ADDR           	(LPHY_MAILBOX_DL_TB_QUEUE_BASE_ADDR + LPHY_MAILBOX_DLTB_QUEUE_SIZE)
#define LPHY_MAILBOX_TOA_BASE_ADDR                  (LPHY_MAILBOX_LPP_DOWN_BASE_ADDR 	+ LPHY_MAILBOX_LPP_DOWN_SIZE)
#define LPHY_MAILBOX_NV_CFG_BASE_ADDR               (LPHY_MAILBOX_TOA_BASE_ADDR 		+ LPHY_MAILBOX_TOA_SIZE)
#define TPHY_MAILBOX_NV_CFG_BASE_ADDR				(LPHY_MAILBOX_NV_CFG_BASE_ADDR		+ LPHY_NV_CFG_SIZE)
#define LTPHY_MAILBOX_NV_COMM_CFG_BASE_ADDR 		(TPHY_MAILBOX_NV_CFG_BASE_ADDR 		+ TPHY_NV_CFG_SIZE)
//#define LPHY_MAILBOX_LCS_BANKA_BASE_ADDR            (TPHY_MAILBOX_NV_CFG_BASE_ADDR 		+ TPHY_NV_CFG_SIZE)
#define LPHY_MAILBOX_LCS_BANKA_BASE_ADDR            (LTPHY_MAILBOX_NV_COMM_CFG_BASE_ADDR 		+ LTPHY_NV_COMM_CFG_SIZE)
#define LPHY_MAILBOX_CSU_BANKA_BASE_ADDR            (LPHY_MAILBOX_LCS_BANKA_BASE_ADDR  	+ LPHY_MAILBOX_LCS_BANKA_MEM_SIZE)
#define LPHY_MAILBOX_STU_BANKA_BASE_ADDR            (LPHY_MAILBOX_CSU_BANKA_BASE_ADDR  	+ LPHY_MAILBOX_CSU_BANKA_MEM_SIZE)
#define LPHY_MAILBOX_RSV_BANKA_BASE_ADDR			(LPHY_MAILBOX_STU_BANKA_BASE_ADDR  	+ LPHY_MAILBOX_STU_BANKA_MEM_SIZE)

/*	----------------------------------------------------------------------------------
* WARNING: This memory is a reused memory space for LTE[OTDOA in Matser Mode] & TDS
* [CellSrch&EMU in slave Mode]; Since TDS [EMU&Cellsrch] have higher priority than
* OTDOA, SO TDS need backup the content of this buffer before start to use this bufer
* and recover the content of this buffer before TDS task exit.
-------------------------------------------------------------------------------------*/
#define TPHY_RSVD_32K_BUFFER_BASE_ADDR				(LPHY_MAILBOX_LCS_BANKA_BASE_ADDR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MEM_MAP_BASE_H__ */

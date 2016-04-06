
#ifndef __BSP_MEMORY_H__
#define __BSP_MEMORY_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_memmap.h"
#include "soc_onchiprom.h"

/*sram虚拟地址和物理地址转换的宏*/
#define SRAM_V2P(addr) ((unsigned long)addr - (unsigned long)HI_SRAM_MEM_ADDR_VIRT + (unsigned long)HI_SRAM_MEM_ADDR)
#define SRAM_P2V(addr) ((unsigned long)addr - (unsigned long)HI_SRAM_MEM_ADDR + (unsigned long)HI_SRAM_MEM_ADDR_VIRT)

#ifndef __ASSEMBLY__

struct uart_infor
{
	unsigned int flag;          /*flag which denotes wether this uart will be used*/
	unsigned int base_addr;     /*the register base address*/
	unsigned int interrupt_num; /*interrupt number*/
};

/*占用空间较小的部分*/
#define SRAM_TEMP_PROTECT_SIZE 32
#define SRAM_DLOAD_SIZE 64
#define SRAM_DSP_MNTN_SIZE 32
struct dfs_ddr_cfg
{
    unsigned int ddr_freq;
    unsigned int ddr_reg_83c;
    unsigned int ddr_reg_abc;
    unsigned int ddr_reg_b3c;
    unsigned int ddr_reg_bbc;
    unsigned int ddr_reg_c3c;
};
typedef struct tag_SRAM_SMALL_SECTIONS
{
	unsigned int        SRAM_USB_ASHELL;
	struct uart_infor   UART_INFORMATION[3];                            /*three uarts:0/1/2*/
	unsigned int        SRAM_ONOFF[8];
	unsigned int        SRAM_DICC[8];				                    /*GU使用的DICC*/
	unsigned int        SRAM_DSP_DRV;
	unsigned int        SRAM_PCIE_INFO[64];			                    /*DSP镜像加载时使用*/
	unsigned int        SRAM_SEC_ROOTCA[65];
	unsigned int        SRAM_WDT_AM_FLAG;	                            /*连仿真器停狗标志*/
	unsigned int        SRAM_WDT_CM_FLAG;
	unsigned int        SRAM_BUCK3_ACORE_ONOFF_FLAG;                    /*BUCK3上下电标志*/
	unsigned int        SRAM_BUCK3_CCORE_ONOFF_FLAG;
	unsigned int        SRAM_CUR_CPUFREQ_PROFILE;                       /* current profile */
	unsigned int        SRAM_MAX_CPUFREQ_PROFILE;                       /* max profile */
	unsigned int        SRAM_MIN_CPUFREQ_PROFILE;                       /* min profile */
	unsigned int        SRAM_CPUFREQ_DOWN_FLAG[2];
	unsigned int        SRAM_CPUFREQ_DOWN_PROFILE[2];
	unsigned int        SRAM_REBOOT_INFO[8];		                    /* E5 开机信息区 不可以被修改*/
	unsigned int        SRAM_TEMP_PROTECT[SRAM_TEMP_PROTECT_SIZE];		/*温保使用的该地址不能被修改*/
	unsigned char       SRAM_DLOAD[SRAM_DLOAD_SIZE];			        /*升级模块使用，不可以被修改*/
	struct tagOcrShareData  SRAM_SEC_SHARE;			                    /*onchiprom启动时存放信息的标志位，放在SRAM的高地址处,不可修改*/
    
	unsigned char       SRAM_DSP_MNTN_INFO[SRAM_DSP_MNTN_SIZE];		    /* DSP邮箱异常时的可维可测信息 */
	struct dfs_ddr_cfg SRAM_DFS_DDRC_CFG[2];
	unsigned int 	    SRAM_DUMP_POWER_OFF_FLAG;
}SRAM_SMALL_SECTIONS;

#define SRAM_USB_CSHELL_ADDR             (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_USB_CSHELL))
#define SRAM_USB_ASHELL_ADDR             (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_USB_ASHELL))
#define SRAM_UART_INFO_ADDR              (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION)
#define SRAM_AT_UART_FLAG_ADDR           (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_AT_UART_FLAG))
#define SRAM_ONOFF_ADDR                  (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_ONOFF)
#define MEMORY_AXI_DICC_ADDR             (void *)((((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_DICC))
#define SRAM_DSP_DRV_ADDR                (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_DSP_DRV))
#define SRAM_PCIE_INFO_ADDR              (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_PCIE_INFO)
#define SRAM_SEC_ROOTCA_ADDR             (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_SEC_ROOTCA)
#define SRAM_WDT_AM_FLAG_ADDR            (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_WDT_AM_FLAG))
#define SRAM_WDT_CM_FLAG_ADDR            (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_WDT_CM_FLAG))
#define SRAM_BUCK3_ACORE_ONOFF_FLAG_ADDR (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_BUCK3_ACORE_ONOFF_FLAG))
#define SRAM_BUCK3_CCORE_ONOFF_FLAG_ADDR (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_BUCK3_CCORE_ONOFF_FLAG))
#define SRAM_CUR_CPUFREQ_PROFILE_ADDR    (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_CUR_CPUFREQ_PROFILE))
#define SRAM_MAX_CPUFREQ_PROFILE_ADDR    (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_MAX_CPUFREQ_PROFILE))
#define SRAM_MIN_CPUFREQ_PROFILE_ADDR    (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_MIN_CPUFREQ_PROFILE))
#define SRAM_DFS_DDRC_CFG_ADDR           (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_DFS_DDRC_CFG)
#define SRAM_REBOOT_ADDR                 (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_REBOOT_INFO)
#define SRAM_TEMP_PROTECT_ADDR           (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_TEMP_PROTECT)
#define SRAM_DLOAD_ADDR                  (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_DLOAD)
#define SRAM_SEC_SHARE                   (void *)(&(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_SEC_SHARE))
#define SRAM_DSP_MNTN_INFO_ADDR          (void *)((((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_DSP_MNTN_INFO))
#define DSP_MAILBOX_PROTECT_FLAG_ADDR    (void *)(&((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_DSP_DRV)
#define DSP_MAILBOX_PROTECT_FLAG_SIZE    (sizeof(unsigned int))

#define SRAM_SUPPORT_ASHELL_ADDR (void *)(((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_USB_ASHELL)
#define SRAM_DUMP_POWER_OFF_FLAG_ADDR    (void *)(&((SRAM_SMALL_SECTIONS*)SRAM_SMALL_SECTIONS_ADDR)->SRAM_DUMP_POWER_OFF_FLAG)

/*RTT 低功耗部分使用的AXI,存放标志等信息,共32字节*/
typedef struct tag_SRAM_RTT_SLEEP_FLAG
{
    unsigned int        SRAM_DSP_FLAG;          /*DSP使用，存放DSP是进入深睡或者是浅睡的标志*/
}SRAM_RTT_SLEEP_FLAG;

#define SRAM_DSP_SLEEP_FLAG_ADDR (void *)(&(((SRAM_RTT_SLEEP_FLAG*)SRAM_RTT_SLEEP_FLAG_ADDR)->SRAM_DSP_FLAG))
#endif/*__ASSEMBLY__*/

#define SRAM_MEM_TOP_ADDR				(void *)((unsigned long)HI_SRAM_MEM_ADDR_VIRT + HI_SRAM_MEM_SIZE )

/*以下定义为GU使用的，底软应按照最新的使用方式*/
#define GLOBAL_AXI_MEM_BASE_ADDR		(HI_SRAM_MEM_ADDR_VIRT)
#define GLOBAL_AXI_MEM_BASE_ADDR_PHY	(HI_SRAM_MEM_ADDR)

#define SRAM_TOP_RESERVE_ADDR           (HI_SRAM_MEM_ADDR_VIRT)
#define SRAM_TOP_RESERVE_SIZE           (2*1024)
#define SRAM_SMALL_SECTIONS_ADDR      	(SRAM_TOP_RESERVE_ADDR)
#define SRAM_SMALL_SECTIONS_SIZE		(sizeof(SRAM_SMALL_SECTIONS))

/* MCU PM 1K */
#define SRAM_MCU_RESERVE_ADDR           (void *)((unsigned long)HI_SRAM_MEM_ADDR_VIRT+SRAM_TOP_RESERVE_SIZE)
#define SRAM_MCU_RESERVE_SIZE           (1*1024)

/* ICC通道(mcore与acore; mcore与ccore), 各2KB, 共4KB */
#define SRAM_ICC_ADDR                   (void *)((unsigned long)SRAM_MCU_RESERVE_ADDR + SRAM_MCU_RESERVE_SIZE)
#define SRAM_ICC_SIZE                   (4*1024)

/*RTT 低功耗部分用于存放标志信息等的区域*/
#define SRAM_RTT_SLEEP_FLAG_ADDR        (void *)((unsigned long)SRAM_ICC_ADDR + SRAM_ICC_SIZE)
#define SRAM_RTT_SLEEP_FLAG_SIZE        (32)

#define SRAM_GU_MAC_HEADER_ADDR         (void *)((unsigned long)SRAM_RTT_SLEEP_FLAG_ADDR + SRAM_RTT_SLEEP_FLAG_SIZE)
#define SRAM_GU_MAC_HEADER_SIZE         (56*1024)

/*K3V3 fpga GBBP DRX寄存器地址访问data abort屏蔽*/
#define SRAM_ADP_GDRX_ADDR              (void *)((unsigned long)SRAM_GU_MAC_HEADER_ADDR+SRAM_GU_MAC_HEADER_SIZE)
#if (defined BSP_CONFIG_HI3630) && (defined BSP_CONFIG_BOARD_SFT)
#define SRAM_ADP_GDRX_SIZE              (8*1024)
#else
#define SRAM_ADP_GDRX_SIZE              (0)
#endif

#define SRAM_TLPHY_ADDR                (void *)((unsigned long)SRAM_ADP_GDRX_ADDR + SRAM_ADP_GDRX_SIZE)
#define SRAM_TLPHY_SIZE                 (12*1024)

#define SRAM_BUSSTRESS_ADDR				(void *)((unsigned long)SRAM_TLPHY_ADDR+SRAM_TLPHY_SIZE)
#ifdef   CONFIG_MODULE_BUSSTRESS
#define SRAM_BUSSTRESS_SIZE				(16*1024)
#else
#define SRAM_BUSSTRESS_SIZE				(0)
#endif

/*SRAM动态区*/
#define SRAM_DYNAMIC_SEC_ADDR           (void *)((unsigned long)SRAM_BUSSTRESS_ADDR + SRAM_BUSSTRESS_SIZE)
#define SRAM_DYNAMIC_SEC_SIZE           ((unsigned long)SRAM_MEM_TOP_ADDR - (unsigned long)SRAM_DYNAMIC_SEC_ADDR)

#ifdef __cplusplus
}
#endif

#endif



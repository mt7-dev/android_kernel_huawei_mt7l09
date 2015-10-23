/*This file contains shared ddr  .*/

#ifndef __BSP_SHARED_DDR_H__
#define __BSP_SHARED_DDR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_memmap.h"
#include "product_config.h"
/*共享内存从虚拟地址到物理地址转换的宏*/
#define SHD_DDR_V2P(addr) ((addr) - (DDR_SHARED_MEM_VIRT_ADDR) + (DDR_SHARED_MEM_ADDR))
#define SHD_DDR_P2V(addr) ((addr) - (DDR_SHARED_MEM_ADDR) + (DDR_SHARED_MEM_VIRT_ADDR))


/*以下定义为GU使用的*/
#define CORESHARE_MEM_WAN_ADDR			((SHM_MEM_WAN_ADDR - DDR_SHARED_MEM_VIRT_ADDR + DDR_SHARED_MEM_ADDR))

#define SHM_MEM_TOP_ADDR                (DDR_SHARED_MEM_VIRT_ADDR +DDR_SHARED_MEM_SIZE )
/********** pm - start **********/
/* v7r2 a9_boot_addr 对齐方式可配置，需要64/32/16KB/8KB对齐 */
/* p531 a9_boot_addr 需要64k对齐  */
#define SHM_MEM_APPA9_PM_BOOT_SIZE      (0x10000)    /* 64K全占满 */
#define SHM_MEM_APPA9_PM_BOOT_ADDR      (SHM_MEM_TOP_ADDR-SHM_MEM_APPA9_PM_BOOT_SIZE)

#define SHM_MEM_MDMA9_PM_BOOT_SIZE      (0x2000)
#define SHM_MEM_MDMA9_PM_BOOT_ADDR      (SHM_MEM_APPA9_PM_BOOT_ADDR - SHM_MEM_APPA9_PM_BOOT_SIZE)
/********** pm - end **********/

#define SHM_MEM_SYNC_SIZE               (0x60)
#define SHM_MEM_SYNC_ADDR             	(SHM_MEM_MDMA9_PM_BOOT_ADDR - SHM_MEM_SYNC_SIZE)

/* 总大小为236KB, 其中, GU通道192KB, L通道40KB, icc控制结构体为页对齐取为4KB */
#define SHM_MEM_ICC_SIZE                (0x61800)
#define SHM_MEM_ICC_ADDR                (SHM_MEM_SYNC_ADDR - SHM_MEM_ICC_SIZE)

/* 总大小为64KB, 其中, 用于从AXI(SRAM)移过来的IPF描述符和IPF低功耗的寄存器保存恢复, 首地址要求8字节对齐 */
#define SHM_MEM_IPF_SIZE                (0x10000)
#define SHM_MEM_IPF_ADDR             	(SHM_MEM_ICC_ADDR - SHM_MEM_IPF_SIZE)

#define SHM_MEM_WAN_SIZE                (0x8000)
#define SHM_MEM_WAN_ADDR             	(SHM_MEM_IPF_ADDR - SHM_MEM_WAN_SIZE)

#define SHM_MEM_NV_SIZE				    (0x100000)
#define SHM_MEM_NV_ADDR				    (SHM_MEM_WAN_ADDR - SHM_MEM_NV_SIZE)

#define SHM_MEM_M3_MNTN_SIZE			(0x20000)
#define SHM_MEM_M3_MNTN_ADDR			(SHM_MEM_NV_ADDR - SHM_MEM_M3_MNTN_SIZE)

#define SHM_MEM_HIFI_SIZE				(10*1024)
#define SHM_MEM_HIFI_ADDR				(SHM_MEM_M3_MNTN_ADDR -SHM_MEM_HIFI_SIZE)

#define SHM_MEM_HIFI_MBX_SIZE			(0x9800)
#define SHM_MEM_HIFI_MBX_ADDR			(SHM_MEM_HIFI_ADDR - SHM_MEM_HIFI_MBX_SIZE)

#define SHM_DDM_LOAD_SIZE				(1*1024)
#define SHM_DDM_LOAD_ADDR				(SHM_MEM_HIFI_MBX_ADDR - SHM_DDM_LOAD_SIZE)

#define SHM_TIMESTAMP_SIZE				(1*1024)
#define SHM_TIMESTAMP_ADDR				(SHM_DDM_LOAD_ADDR - SHM_TIMESTAMP_SIZE)

#define SHM_MEM_IOS_SIZE		        (2*1024)
#define SHM_MEM_IOS_ADDR		        (SHM_TIMESTAMP_ADDR - SHM_MEM_IOS_SIZE)

#define SHM_MEM_MODEM_PINTRL_SIZE		(4*1024)
#define SHM_MEM_MODEM_PINTRL_ADDR		(SHM_MEM_IOS_ADDR - SHM_MEM_MODEM_PINTRL_SIZE)

#define SHM_MEM_TEMPERATURE_SIZE		(3*1024)
#define SHM_MEM_TEMPERATURE_ADDR		(SHM_MEM_MODEM_PINTRL_ADDR - SHM_MEM_TEMPERATURE_SIZE)

#define SHM_MEM_RESTORE_AXI_SIZE        (96*1024)
#define SHM_MEM_RESTORE_AXI_ADDR        (SHM_MEM_TEMPERATURE_ADDR - SHM_MEM_RESTORE_AXI_SIZE)

#define SHM_MEMMGR_FLAG_SIZE            (0x180)
#define SHM_MEMMGR_FLAG_ADDR            (SHM_MEM_RESTORE_AXI_ADDR - SHM_MEMMGR_FLAG_SIZE)

/* 电压源过流标志位，被置位的，根据nv配置决定能否再次被打开 */
#define SHM_PMU_OCP_INFO_SIZE            (0x40)
#define SHM_PMU_OCP_INFO_ADDR            (SHM_MEMMGR_FLAG_ADDR - SHM_PMU_OCP_INFO_SIZE)
/* 电压源属性控制表，大小有预留，实际使用为0x85c */
#define SHM_PMU_VOLTTABLE_SIZE           (3*1024)
#define SHM_PMU_VOLTTABLE_ADDR           (SHM_PMU_OCP_INFO_ADDR - SHM_PMU_VOLTTABLE_SIZE)

/*硬件版本号*/
#define SHM_MEM_HW_VER_SIZE             (0x4)
#define SHM_MEM_HW_VER_ADDR             (SHM_PMU_VOLTTABLE_ADDR-SHM_MEM_HW_VER_SIZE)

#define SHM_MEM_AT_FLAG_SIZE            (0x4)
#define SHM_MEM_AT_FLAG_ADDR            (SHM_MEM_HW_VER_ADDR - SHM_MEM_AT_FLAG_SIZE)

#define SHM_MEM_CSHELL_FLAG_SIZE        (0x4)
#define SHM_MEM_CHSELL_FLAG_ADDR        (SHM_MEM_AT_FLAG_ADDR - SHM_MEM_CSHELL_FLAG_SIZE)

#define SHM_MEM_DSP_FLAG_SIZE           (0x8)
#define SHM_MEM_DSP_FLAG_ADDR           (SHM_MEM_CHSELL_FLAG_ADDR - SHM_MEM_DSP_FLAG_SIZE)

/* 分区表 */
#define SHM_MEM_PTABLE_SIZE             (2*1024)
#define SHM_MEM_PTABLE_ADDR             (SHM_MEM_DSP_FLAG_ADDR - SHM_MEM_PTABLE_SIZE)

/*pastar dpm flag*/
#define SHM_MEM_PASTAR_DPM_INFO_SIZE    (0xc)
#define SHM_MEM_PASTAR_DPM_INFO_ADDR    (SHM_MEM_PTABLE_ADDR - SHM_MEM_PASTAR_DPM_INFO_SIZE)

/* L2CACHE开关标记 */
#define SHM_MEM_CCORE_L2CACHE_SIZE       (0x4)
#define SHM_MEM_CCORE_L2CACHE_ADDR       (SHM_MEM_PASTAR_DPM_INFO_ADDR - SHM_MEM_CCORE_L2CACHE_SIZE)

/* modem 镜像标记 */
#define SHM_MEM_LOADM_SIZE              (0x0C)
#define SHM_MEM_LOADM_ADDR              (SHM_MEM_CCORE_L2CACHE_ADDR - SHM_MEM_LOADM_SIZE)

/* modem 单独复位标记 */
#define SHM_MEM_CCORE_RESET_SIZE       (0x400)
#define SHM_MEM_CCORE_RESET_ADDR       (SHM_MEM_LOADM_ADDR - SHM_MEM_CCORE_RESET_SIZE)

/*bbp power flag*/
#define SHM_MEM_MEMREPAIR_SIZE           (0x20)
#define SHM_MEM_MEMREPAIR_ADDR           (SHM_MEM_CCORE_RESET_ADDR - SHM_MEM_MEMREPAIR_SIZE)

/* nand spec info */
#define SHM_MEM_NAND_SPEC_SIZE          (24)
#define SHM_MEM_NAND_SPEC_ADDR          (SHM_MEM_MEMREPAIR_ADDR - SHM_MEM_NAND_SPEC_SIZE)


/*总线压力测试用空间*/
#ifdef CONFIG_MODULE_BUSSTRESS
#define SHM_MEM_BUSSSTRESS_SIZE             (70*1024)
#else
#define SHM_MEM_BUSSSTRESS_SIZE             (0)
#endif
#define SHM_MEM_BUSSSTRESS_ADDR             (SHM_MEM_NAND_SPEC_ADDR - SHM_MEM_BUSSSTRESS_SIZE)


#define SHM_MEM_MEMMGR_SIZE             (SHM_MEM_BUSSSTRESS_ADDR - SHM_MEM_MEMMGR_ADDR)
#define SHM_MEM_MEMMGR_ADDR             (DDR_SHARED_MEM_VIRT_ADDR + CORESHARE_MEM_TENCILICA_MULT_BAND_SIZE)

#ifdef __cplusplus
}
#endif

#endif    /* __BSP_SHARED_DDR_H__ */




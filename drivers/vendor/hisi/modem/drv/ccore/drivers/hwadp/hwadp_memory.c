
#include <string.h>
#include <stdio.h>
#include "product_config.h"
#include "osl_types.h"
#include "bsp_sram.h"
#include "bsp_shared_ddr.h"
#include "drv_memory.h"
#include "MemoryMap.h"
#include "ptable_com.h"
#include <bsp_dump_drv.h>

/*lint -e545*/

/* 全局的结构体，记录AXI内存段分配信息 */
const BSP_AXI_SECT_INFO g_stAxiSectInfo[BSP_AXI_SECT_TYPE_BUTTOM] =
{
    {BSP_AXI_SECT_TYPE_TEMPERATURE,       SRAM_TEMP_PROTECT_ADDR,            SRAM_TEMP_PROTECT_ADDR,         SRAM_TEMP_PROTECT_SIZE},
    {BSP_AXI_SECT_TYPE_DICC,              (u32)MEMORY_AXI_DICC_ADDR,              (u32)MEMORY_AXI_DICC_ADDR,              8},
    {BSP_AXI_SECT_TYPE_HIFI,              SHM_MEM_HIFI_ADDR,                     SHM_MEM_HIFI_ADDR,        SHM_MEM_HIFI_SIZE},
    {BSP_AXI_SECT_TYPE_TTF_BBP,           SRAM_GU_MAC_HEADER_ADDR,           SRAM_V2P(SRAM_GU_MAC_HEADER_ADDR),           SRAM_GU_MAC_HEADER_SIZE},

};

/* 全局的结构体，记录DDR内存段分配信息 */
const BSP_DDR_SECT_INFO g_stDdrSectInfo[BSP_DDR_SECT_TYPE_BUTTOM] = {
    {BSP_DDR_SECT_TYPE_TTF,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_TTF_BASE_ADDR,                ECS_TTF_BASE_ADDR,              ECS_TTF_SIZE},
    {BSP_DDR_SECT_TYPE_ARMDSP,    BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_ARM_DSP_BUFFER_BASE_ADDR,     ECS_ARM_DSP_BUFFER_BASE_ADDR,   ECS_ARM_DSP_BUFFER_SIZE},
    {BSP_DDR_SECT_TYPE_UPA,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_UPA_BASE_ADDR,                ECS_UPA_BASE_ADDR,              ECS_UPA_SIZE},
    {BSP_DDR_SECT_TYPE_CQI,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_CQI_BASE_ADDR,                ECS_CQI_BASE_ADDR,              ECS_CQI_SIZE},
    {BSP_DDR_SECT_TYPE_APT,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_APT_BASE_ADDR,                ECS_APT_BASE_ADDR,              ECS_APT_SIZE},
    {BSP_DDR_SECT_TYPE_ET,        BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_ET_BASE_ADDR,                 ECS_ET_BASE_ADDR,               ECS_ET_SIZE},
    {BSP_DDR_SECT_TYPE_BBPMASTER, BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_BBP_MASTER_BASE_ADDR,         ECS_BBP_MASTER_BASE_ADDR,       ECS_BBP_MASTER_SIZE},

    /* GU NV/DICC内存上层要求C/A核虚地址必须一致，by 陈迎国 */
    {BSP_DDR_SECT_TYPE_NV,        BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_NV_BASE_ADDR_VIRT,            ECS_NV_BASE_ADDR,               ECS_NV_SIZE},
    {BSP_DDR_SECT_TYPE_DICC,      BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_TTF_DICC_ADDR_VIRT,           ECS_TTF_DICC_ADDR,              ECS_TTF_DICC_SIZE},

    {BSP_DDR_SECT_TYPE_WAN,       BSP_DDR_SECT_ATTR_NONCACHEABLE, SHM_MEM_WAN_ADDR,                 SHM_MEM_WAN_ADDR,               SHM_MEM_WAN_SIZE},
    {BSP_DDR_SECT_TYPE_SHARE_MEM, BSP_DDR_SECT_ATTR_NONCACHEABLE, DDR_SHARED_MEM_ADDR,              DDR_SHARED_MEM_ADDR,            DDR_SHARED_MEM_SIZE},
    {BSP_DDR_SECT_TYPE_EXCP,      BSP_DDR_SECT_ATTR_NONCACHEABLE, DDR_MNTN_ADDR,                    DDR_MNTN_ADDR,                  DDR_MNTN_SIZE},
    {BSP_DDR_SECT_TYPE_HIFI,      BSP_DDR_SECT_ATTR_NONCACHEABLE, DDR_HIFI_ADDR,                    DDR_HIFI_ADDR,                  DDR_HIFI_SIZE},
    {BSP_DDR_SECT_TYPE_ZSP_UP,    BSP_DDR_SECT_ATTR_NONCACHEABLE, ZSP_UP_ADDR,                      ZSP_UP_ADDR,                    ZSP_UP_SIZE},

    /* GU BBPHAC内存上层要求C/A核虚地址必须一致，by 陈迎国 */
    {BSP_DDR_SECT_TYPE_HARQ,      BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_BBPHAC_BASE_ADDR_VIRT,        ECS_BBPHAC_BASE_ADDR,         ECS_BBPHAC_SIZE},
    {BSP_DDR_SECT_TYPE_BBPSAMPLE, BSP_DDR_SECT_ATTR_NONCACHEABLE, 0,   0,0},
};

BSP_VOID BSP_DDR_ShowSectInfo(BSP_VOID);
void show_global_ddr_status(void);
void show_sram_status(void);
void show_shared_ddr_status(void);
void writeM(UINT32 pAddr, UINT32 value);
void memRead32 (unsigned int * uiMemAddr32);
void r(unsigned int * newMemAddr32);

/*****************************************************************************
 函 数 名  : BSP_AXI_GetSectInfo
 功能描述  : AXI内存段查询接口
 输入参数  : enSectType: 需要查询的内存段类型
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
s32 BSP_AXI_GetSectInfo(BSP_AXI_SECT_TYPE_E enSectType, BSP_AXI_SECT_INFO *pstSectInfo)
{
    if(BSP_NULL == pstSectInfo)
    {
        return BSP_ERROR;
    }

    memcpy((void *)pstSectInfo, (const void *)(&g_stAxiSectInfo[enSectType]), sizeof(BSP_AXI_SECT_INFO));

    return BSP_OK;
}
/*****************************************************************************
 函 数 名  : BSP_DDR_GetSectInfo
 功能描述  : DDR内存段查询接口
 输入参数  : pstSectQuery: 需要查询的内存段类型、属性
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
s32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY *pstSectQuery, BSP_DDR_SECT_INFO *pstSectInfo)
{
    if((BSP_NULL == pstSectQuery) || (BSP_NULL == pstSectInfo))
    {
        return BSP_ERROR;
    }

    memcpy((void *)pstSectInfo, (const void *)(&g_stDdrSectInfo[pstSectQuery->enSectType]), sizeof(BSP_DDR_SECT_INFO));

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : BSP_DDR_ShowSectInfo
 功能描述  : 打印DDR内存段信息
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
BSP_VOID BSP_DDR_ShowSectInfo(BSP_VOID)
{
    BSP_DDR_SECT_TYPE_E     enSectTypeIndex = (BSP_DDR_SECT_TYPE_E)0;

    printk("\ntype       paddr      vaddr      size       attr\n");
    for(; enSectTypeIndex < BSP_DDR_SECT_TYPE_BUTTOM; enSectTypeIndex++)
    {
        printk("0x%-8.8x 0x%-8.8x 0x%-8.8x 0x%-8.8x 0x%-8.8x\n\n", \
          g_stDdrSectInfo[enSectTypeIndex].enSectType, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectPhysAddr, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectVirtAddr, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectSize, \
          g_stDdrSectInfo[enSectTypeIndex].enSectAttr);
    }
}

/*************************内存虚实转换 start*********************************/
/*****************************************************************************
 函 数 名  : DRV_DDR_VIRT_TO_PHY
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
    unsigned int DRV_DDR_VIRT_TO_PHY(unsigned int ulVAddr)
    {

        if((ulVAddr >= DDR_SHARED_MEM_VIRT_ADDR)
            && (ulVAddr < SHM_MEM_TOP_ADDR))
        {
            return SHD_DDR_V2P(ulVAddr);
        }
		/* coverity[unsigned_compare] */
        if((ulVAddr >= DDR_GU_ADDR_VIRT)/*lint !e685 !e568 */
            && (ulVAddr <= DDR_GU_ADDR_VIRT + DDR_GU_SIZE))
        {
            return (ulVAddr - DDR_GU_ADDR_VIRT + DDR_GU_ADDR);
        }

        printk("DRV_DDR_VIRT_TO_PHY: ulVAddr(0x%x) is invalid!\n", ulVAddr);

        return 0;
    }


/*****************************************************************************
 函 数 名  : DRV_DDR_PHY_TO_VIRT
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
    unsigned int DRV_DDR_PHY_TO_VIRT(unsigned int ulPAddr)
    {
        if((ulPAddr >= DDR_SHARED_MEM_ADDR)
            && (ulPAddr < DDR_SHARED_MEM_ADDR + DDR_SHARED_MEM_SIZE))
        {
            return SHD_DDR_P2V(ulPAddr);
        }

        if((ulPAddr >= DDR_GU_ADDR)
            && (ulPAddr < DDR_GU_ADDR + DDR_GU_SIZE))
        {
            return (ulPAddr - DDR_GU_ADDR + DDR_GU_ADDR_VIRT);
        }

        printk("DRV_DDR_PHY_TO_VIRT: ulVAddr(0x%x) is invalid!\n",ulPAddr);
        return 0;
    }

/*****************************************************************************
 函 数 名  : TTF_VIRT_TO_PHY
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
unsigned int TTF_VIRT_TO_PHY(unsigned int ulVAddr)
{
    return (ulVAddr - ECS_TTF_BASE_ADDR_VIRT + ECS_TTF_BASE_ADDR);
}

/*****************************************************************************
 函 数 名  : TTF_PHY_TO_VIRT
 功能描述  : TTF内存实地址往虚地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int TTF_PHY_TO_VIRT(unsigned int ulPAddr)
{
    return (ulPAddr - ECS_TTF_BASE_ADDR + ECS_TTF_BASE_ADDR_VIRT);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_VIRT_TO_PHY
 功能描述  : AXI内虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int DRV_AXI_VIRT_TO_PHY(unsigned int ulVAddr)
{
    return (ulVAddr - HI_SRAM_MEM_ADDR_VIRT + HI_SRAM_MEM_ADDR);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_PHY_TO_VIRT
 功能描述  : AXI内实地址往虚地址转换
 输入参数  : ulVAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int DRV_AXI_PHY_TO_VIRT(unsigned int ulPAddr)
{
    return (ulPAddr - HI_SRAM_MEM_ADDR + HI_SRAM_MEM_ADDR_VIRT);
}

/*****************************************************************************
 函 数 名  : show_global_ddr_status
 功能描述  : DDR内存段信息打印
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
void show_global_ddr_status(void)
{
    printf("%-30s%10s%10s\t\n", "name", "phy addr", "size");
    printf("%-30s%10x%10x\n", "DDR_APP_ACP_ADDR", DDR_APP_ACP_ADDR, DDR_APP_ACP_SIZE);
    printf("%-30s%10x%10x\n", "DDR_ACORE_ADDR", DDR_ACORE_ADDR, DDR_ACORE_SIZE);
    printf("%-30s%10x%10x\n", "DDR_MCORE_ADDR", DDR_MCORE_ADDR, DDR_MCORE_SIZE);
    printf("%-30s%10x%10x\n", "DDR_SHARED_MEM_ADDR", DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_SIZE);
    printf("%-30s%10x%10x\n", "DDR_MNTN_ADDR", DDR_MNTN_ADDR, DDR_MNTN_SIZE);
    printf("%-30s%10x%10x\n", "DDR_GU_ADDR", DDR_GU_ADDR, DDR_GU_SIZE);
    printf("%-30s%10x%10x\n", "DDR_TLPHY_IMAGE_ADDR", DDR_TLPHY_IMAGE_ADDR, DDR_TLPHY_IMAGE_SIZE);
    printf("%-30s%10x%10x\n", "DDR_LPHY_SDR_ADDR", DDR_LPHY_SDR_ADDR, DDR_LPHY_SDR_SIZE);
    printf("%-30s%10x%10x\n", "DDR_SOCP_ADDR", DDR_SOCP_ADDR, DDR_SOCP_SIZE);
    printf("%-30s%10x%10x\n", "DDR_LCS_ADDR", DDR_LCS_ADDR, DDR_LCS_SIZE);
    printf("%-30s%10x%10x\n", "DDR_HIFI_ADDR", DDR_HIFI_ADDR, DDR_HIFI_SIZE);
    printf("%-30s%10x%10x\n", "DDR_MDM_ACP_ADDR", DDR_MDM_ACP_ADDR, DDR_MDM_ACP_SIZE);


}
/*****************************************************************************
 函 数 名  : show_sram_status
 功能描述  : SRAM内存段信息打印
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/

void show_sram_status(void)
{
	/*请依照先后顺序增加打印输出项*/
	int total_size = SRAM_TOP_RESERVE_SIZE + SRAM_MCU_RESERVE_SIZE + SRAM_ICC_SIZE + SRAM_RTT_SLEEP_FLAG_SIZE +
                     SRAM_GU_MAC_HEADER_SIZE + SRAM_DYNAMIC_SEC_ADDR;/*lint !e569 */
	printf("%-30s%10s%10s%10s\n", "name", "phy addr", "virt addr", "size");
	printf("%-30s%10x%10x%10x\n", "SRAM_TOP_RESERVE_ADDR", SRAM_V2P(SRAM_TOP_RESERVE_ADDR),SRAM_TOP_RESERVE_ADDR, SRAM_TOP_RESERVE_SIZE);/*lint !e778 */
	printf("%-30s%10x%10x%10x\n", "SRAM_MCU_RESERVE_ADDR", SRAM_V2P(SRAM_MCU_RESERVE_ADDR),SRAM_MCU_RESERVE_ADDR, SRAM_MCU_RESERVE_SIZE);
	printf("%-30s%10x%10x%10x\n", "SRAM_ICC_ADDR", SRAM_V2P(SRAM_ICC_ADDR),SRAM_ICC_ADDR, SRAM_ICC_SIZE);
	printf("%-30s%10x%10x%10x\n", "SRAM_RTT_SLEEP_FLAG_ADDR", SRAM_V2P(SRAM_RTT_SLEEP_FLAG_ADDR),SRAM_RTT_SLEEP_FLAG_ADDR, SRAM_RTT_SLEEP_FLAG_SIZE);
	printf("%-30s%10x%10x%10x\n", "SRAM_GU_MAC_HEADER_ADDR", SRAM_V2P(SRAM_GU_MAC_HEADER_ADDR), SRAM_GU_MAC_HEADER_ADDR, SRAM_GU_MAC_HEADER_SIZE);
	printf("%-30s%10x%10x%10x\n", "SRAM_TLPHY_ADDR", SRAM_V2P(SRAM_TLPHY_ADDR), SRAM_TLPHY_ADDR, SRAM_TLPHY_SIZE);
	printf("%-30s%10x%10x%10x\n", "SRAM_DYNAMIC_SEC_ADDR", SRAM_V2P(SRAM_DYNAMIC_SEC_ADDR), SRAM_DYNAMIC_SEC_ADDR, SRAM_DYNAMIC_SEC_SIZE);
#ifdef   CONFIG_MODULE_BUSSTRESS
	printf("%-30s%10x%10x%10x\n", "SRAM_BUSSTRESS_ADDR", SRAM_V2P(SRAM_BUSSTRESS_ADDR), SRAM_BUSSTRESS_ADDR, SRAM_BUSSTRESS_SIZE);
#endif
	printf("total size: 0x%x\n", total_size);
	printf("detailed info of SRAM_TOP_RESERVE_ADDR: \n");
	printf("%-30s%10x%10x\n", "SRAM_USB_ASHELL_ADDR", SRAM_V2P(SRAM_USB_ASHELL_ADDR), SRAM_USB_ASHELL_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_UART_INFO_ADDR", SRAM_V2P(SRAM_UART_INFO_ADDR), SRAM_UART_INFO_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_ONOFF_ADDR", SRAM_V2P(SRAM_ONOFF_ADDR), SRAM_ONOFF_ADDR);
	printf("%-30s%10x%10x\n", "MEMORY_AXI_DICC_ADDR", SRAM_V2P(MEMORY_AXI_DICC_ADDR), MEMORY_AXI_DICC_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_DSP_DRV_ADDR", SRAM_V2P(SRAM_DSP_DRV_ADDR), SRAM_DSP_DRV_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_PCIE_INFO_ADDR", SRAM_V2P(SRAM_PCIE_INFO_ADDR), SRAM_PCIE_INFO_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_SEC_ROOTCA_ADDR", SRAM_V2P(SRAM_SEC_ROOTCA_ADDR), SRAM_SEC_ROOTCA_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_WDT_AM_FLAG_ADDR", SRAM_V2P(SRAM_WDT_AM_FLAG_ADDR), SRAM_WDT_AM_FLAG_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_WDT_CM_FLAG_ADDR", SRAM_V2P(SRAM_WDT_CM_FLAG_ADDR), SRAM_WDT_CM_FLAG_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_BUCK3_ACORE_ONOFF_FLAG_ADDR", SRAM_V2P(SRAM_BUCK3_ACORE_ONOFF_FLAG_ADDR), SRAM_BUCK3_ACORE_ONOFF_FLAG_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_BUCK3_CCORE_ONOFF_FLAG_ADDR", SRAM_V2P(SRAM_BUCK3_CCORE_ONOFF_FLAG_ADDR), SRAM_BUCK3_CCORE_ONOFF_FLAG_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_CUR_CPUFREQ_PROFILE_ADDR", SRAM_V2P(SRAM_CUR_CPUFREQ_PROFILE_ADDR), SRAM_CUR_CPUFREQ_PROFILE_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_MAX_CPUFREQ_PROFILE_ADDR", SRAM_V2P(SRAM_MAX_CPUFREQ_PROFILE_ADDR), SRAM_MAX_CPUFREQ_PROFILE_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_MIN_CPUFREQ_PROFILE_ADDR", SRAM_V2P(SRAM_MIN_CPUFREQ_PROFILE_ADDR), SRAM_MIN_CPUFREQ_PROFILE_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_REBOOT_ADDR", SRAM_V2P(SRAM_REBOOT_ADDR), SRAM_REBOOT_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_TEMP_PROTECT_ADDR", SRAM_V2P(SRAM_TEMP_PROTECT_ADDR), SRAM_TEMP_PROTECT_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_DLOAD_ADDR", SRAM_V2P(SRAM_DLOAD_ADDR), SRAM_DLOAD_ADDR);
	printf("%-30s%10x%10x\n", "SRAM_SEC_SHARE", SRAM_V2P(SRAM_SEC_SHARE), SRAM_SEC_SHARE);
	printf("%-30s%10x%10x\n", "SRAM_DSP_MNTN_INFO", SRAM_V2P(SRAM_DSP_MNTN_INFO_ADDR), SRAM_DSP_MNTN_INFO_ADDR);
    printf("%-30s%10x%10x\n", "SRAM_DUMP_POWER_OFF_FLAG_ADDR", SRAM_V2P(SRAM_DUMP_POWER_OFF_FLAG_ADDR), SRAM_DUMP_POWER_OFF_FLAG_ADDR);
}
/*****************************************************************************
 函 数 名  : show_shared_ddr_status
 功能描述  : 共享内存内存段信息打印
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/

void show_shared_ddr_status(void)
{
	/*请依照先后顺序增加打印输出项*/
	int total_size = SHM_MEM_APPA9_PM_BOOT_SIZE + SHM_MEM_MDMA9_PM_BOOT_SIZE + SHM_MEM_SYNC_SIZE          + SHM_MEM_ICC_SIZE +
                     SHM_MEM_IPF_SIZE           + SHM_MEM_WAN_SIZE           + SHM_MEM_NV_SIZE            + SHM_MEM_M3_MNTN_SIZE +
                     SHM_MEM_HIFI_SIZE          + SHM_MEM_HIFI_MBX_SIZE      + SHM_DDM_LOAD_SIZE          + SHM_TIMESTAMP_SIZE   +
                     SHM_MEM_IOS_SIZE           + SHM_MEM_MODEM_PINTRL_SIZE  + SHM_MEM_TEMPERATURE_SIZE   +SHM_MEM_RESTORE_AXI_SIZE +
                     SHM_MEMMGR_FLAG_SIZE       + SHM_PMU_OCP_INFO_SIZE      + SHM_PMU_VOLTTABLE_SIZE     + SHM_MEM_HW_VER_SIZE     +
                     SHM_MEM_PTABLE_SIZE        + SHM_MEM_MEMMGR_SIZE        + SHM_MEM_NAND_SPEC_SIZE     +CORESHARE_MEM_TENCILICA_MULT_BAND_SIZE;
	printf("%-30s%10s%10s%10s\n", "name", "phy addr", "virt addr", "size");
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_APPA9_PM_BOOT_ADDR", SHD_DDR_V2P(SHM_MEM_APPA9_PM_BOOT_ADDR), SHM_MEM_APPA9_PM_BOOT_ADDR, SHM_MEM_APPA9_PM_BOOT_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_MDMA9_PM_BOOT_ADDR", SHD_DDR_V2P(SHM_MEM_MDMA9_PM_BOOT_ADDR), SHM_MEM_MDMA9_PM_BOOT_ADDR, SHM_MEM_MDMA9_PM_BOOT_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_SYNC_ADDR", SHD_DDR_V2P(SHM_MEM_SYNC_ADDR), SHM_MEM_SYNC_ADDR, SHM_MEM_SYNC_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_ICC_ADDR", SHD_DDR_V2P(SHM_MEM_ICC_ADDR), SHM_MEM_ICC_ADDR, SHM_MEM_ICC_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_IPF_ADDR", SHD_DDR_V2P(SHM_MEM_IPF_ADDR),SHM_MEM_IPF_ADDR, SHM_MEM_IPF_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_WAN_ADDR", SHD_DDR_V2P(SHM_MEM_WAN_ADDR), SHM_MEM_WAN_ADDR, SHM_MEM_WAN_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_NV_ADDR", SHD_DDR_V2P(SHM_MEM_NV_ADDR), SHM_MEM_NV_ADDR, SHM_MEM_NV_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_M3_MNTN_ADDR", SHD_DDR_V2P(SHM_MEM_M3_MNTN_ADDR), SHM_MEM_M3_MNTN_ADDR, SHM_MEM_M3_MNTN_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_HIFI_ADDR", SHD_DDR_V2P(SHM_MEM_HIFI_ADDR), SHM_MEM_HIFI_ADDR, SHM_MEM_HIFI_SIZE);
    printf("%-30s%10x%10x%10x\n", "SHM_MEM_HIFI_MBX_ADDR", SHD_DDR_V2P(SHM_MEM_HIFI_MBX_ADDR), SHM_MEM_HIFI_MBX_ADDR, SHM_MEM_HIFI_MBX_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_DDM_LOAD_ADDR", SHD_DDR_V2P(SHM_DDM_LOAD_ADDR), SHM_DDM_LOAD_ADDR, SHM_DDM_LOAD_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_TIMESTAMP_ADDR", SHD_DDR_V2P(SHM_TIMESTAMP_ADDR), SHM_TIMESTAMP_ADDR, SHM_TIMESTAMP_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_IOS_ADDR", SHD_DDR_V2P(SHM_MEM_IOS_ADDR), SHM_MEM_IOS_ADDR, SHM_MEM_IOS_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_MODEM_PINTRL_ADDR", SHD_DDR_V2P(SHM_MEM_MODEM_PINTRL_ADDR), SHM_MEM_MODEM_PINTRL_ADDR, SHM_MEM_MODEM_PINTRL_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_TEMPERATURE_ADDR", SHD_DDR_V2P(SHM_MEM_TEMPERATURE_ADDR), SHM_MEM_TEMPERATURE_ADDR, SHM_MEM_TEMPERATURE_SIZE);
    printf("%-30s%10x%10x%10x\n", "SHM_MEM_RESTORE_AXI_ADDR", SHD_DDR_V2P(SHM_MEM_RESTORE_AXI_ADDR), SHM_MEM_RESTORE_AXI_ADDR, SHM_MEM_RESTORE_AXI_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEMMGR_FLAG_ADDR", SHD_DDR_V2P(SHM_MEMMGR_FLAG_ADDR), SHM_MEMMGR_FLAG_ADDR, SHM_MEMMGR_FLAG_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_PMU_OCP_INFO_ADDR", SHD_DDR_V2P(SHM_PMU_OCP_INFO_ADDR), SHM_PMU_OCP_INFO_ADDR, SHM_PMU_OCP_INFO_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_PMU_VOLTTABLE_ADDR", SHD_DDR_V2P(SHM_PMU_VOLTTABLE_ADDR), SHM_PMU_VOLTTABLE_ADDR, SHM_PMU_VOLTTABLE_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_HW_VER_ADDR", SHD_DDR_V2P(SHM_MEM_HW_VER_ADDR), SHM_MEM_HW_VER_ADDR, SHM_MEM_HW_VER_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_DSP_FLAG_ADDR", SHD_DDR_V2P(SHM_MEM_DSP_FLAG_ADDR), SHM_MEM_DSP_FLAG_ADDR, SHM_MEM_DSP_FLAG_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_PTABLE_ADDR", SHD_DDR_V2P(SHM_MEM_PTABLE_ADDR), SHM_MEM_PTABLE_ADDR, SHM_MEM_PTABLE_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_PASTAR_DPM_INFO_ADDR", SHD_DDR_V2P(SHM_MEM_PASTAR_DPM_INFO_ADDR), SHM_MEM_PASTAR_DPM_INFO_ADDR, SHM_MEM_PASTAR_DPM_INFO_SIZE);
    printf("%-30s%10x%10x%10x\n", "SHM_MEM_LOADM_ADDR", SHD_DDR_V2P(SHM_MEM_LOADM_ADDR), SHM_MEM_LOADM_ADDR, SHM_MEM_LOADM_SIZE);
    printf("%-30s%10x%10x%10x\n", "SHM_MEM_NAND_SPEC_ADDR", SHD_DDR_V2P(SHM_MEM_NAND_SPEC_ADDR), SHM_MEM_NAND_SPEC_ADDR, SHM_MEM_NAND_SPEC_SIZE);
	printf("%-30s%10x%10x%10x\n", "SHM_MEM_MEMMGR_ADDR", SHD_DDR_V2P(SHM_MEM_MEMMGR_ADDR), SHM_MEM_MEMMGR_ADDR, SHM_MEM_MEMMGR_SIZE);
	printf("%-30s%10x%10x%10x\n", "CORESHARE_MEM_TENCILICA_MULT_BAND_ADDR", CORESHARE_MEM_TENCILICA_MULT_BAND_ADDR, SHD_DDR_P2V(CORESHARE_MEM_TENCILICA_MULT_BAND_ADDR), CORESHARE_MEM_TENCILICA_MULT_BAND_SIZE);/*lint !e778 */
	printf("total size: 0x%x\n", total_size);
}

/*
* adpt for gu
*/
unsigned int   * MemAddr32 = 0x00000000;
/*******************************************************************************
* writeM - write a UINT32 value to perihical address
* adpt for gu
* RETURNS: N/A
*/
void writeM(UINT32 pAddr, UINT32 value)
{
    *(volatile UINT32 *)(pAddr)=value;
}
/***********************************************************
 Function: memRead32--read memory and display the value
            adpt for gu
 Input:    the start address
 return:   void
************************************************************/
void memRead32 (unsigned int * uiMemAddr32)
{
    if (0 != uiMemAddr32)
    {
         MemAddr32 = (unsigned int *)((unsigned int)uiMemAddr32 & 0xFFFFFFFC);
    }

    printf("0x%.8X: 0x%.8X   0x%.8X   0x%.8X   0x%.8X\n",
            (UINT32)(MemAddr32+0x0), *(MemAddr32+0x0), *(MemAddr32+0x1), *(MemAddr32+0x2), *(MemAddr32+0x3));
    printf("0x%.8X: 0x%.8X   0x%.8X   0x%.8X   0x%.8X\n",
            (UINT32)(MemAddr32+0x4), *(MemAddr32+0x4), *(MemAddr32+0x5), *(MemAddr32+0x6), *(MemAddr32+0x7));
    printf("0x%.8X: 0x%.8X   0x%.8X   0x%.8X   0x%.8X\n",
            (UINT32)(MemAddr32+0x8), *(MemAddr32+0x8), *(MemAddr32+0x9), *(MemAddr32+0xa), *(MemAddr32+0xb));
    printf("0x%.8X: 0x%.8X   0x%.8X   0x%.8X   0x%.8X\n",
            (UINT32)(MemAddr32+0xc), *(MemAddr32+0xc), *(MemAddr32+0xd), *(MemAddr32+0xe), *(MemAddr32+0xf));
    MemAddr32 += 0x10;
}

/***********************************************************
 Function: r--simple read memory command
           adpt for gu
 Input:    the start address
 return:  void
************************************************************/
void r(unsigned int * newMemAddr32)
{
    memRead32(newMemAddr32);
}

void show_hpm_temp(void)
{
#ifndef BSP_CONFIG_HI3630
	struct hpm_tem_print *hpm_ptr = (struct hpm_tem_print *)DUMP_EXT_BOOST_MCORE_ADDR;
	printf("****************hpm_temperature****************\n");
	printf("hpm_uhvt_opc: 0x%x\n", hpm_ptr->hpm_uhvt_opc);
	printf("hpm_hvt_opc : 0x%x\n", hpm_ptr->hpm_hvt_opc);
	printf("hpm_svt_opc : 0x%x\n", hpm_ptr->hpm_svt_opc);
	printf("temperature : %d\n", hpm_ptr->temperature);
	printf("up_volt     : %d\n", hpm_ptr->up_volt);
#endif
}

/*lint +e545*/

#ifdef __cplusplus
}
#endif


/*lint --e{537,958}*/
#include <osl_types.h>
#include <bsp_memmap.h>
#include <drv_hw_adp.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>
#include "MemoryMap.h"
#include <bsp_om.h>
#include <drv_mailbox_cfg.h>
#include <product_config.h>

typedef struct{
    BSP_IP_TYPE_E enIPType;
    void* ulIPBaseAddr;
}BSP_IP_BASE_ADDR_INFO;

typedef struct
{
	BSP_DDR_TYPE_E enIPType;
	void* mem_base;
	BSP_U32 size;
}BSP_MEM_ADDR_SIZE_INFO;

/* 全局变量，记录IP基地址 */
BSP_IP_BASE_ADDR_INFO g_ulIPBaseAddr[BSP_IP_TYPE_BUTTOM + 1] = {
    {BSP_IP_TYPE_CICOM0,		(void*)HI_CICOM0_REGBASE_ADDR},
    {BSP_IP_TYPE_CICOM1,		(void*)HI_CICOM1_REGBASE_ADDR},
    {BSP_IP_TYPE_ZSP_DTCM,       (void*)HI_ZSP_DTCM_REG_BASE_ADDR},
    {BSP_IP_TYPE_AHB,            (void*)ZSP_UP_ADDR},/* GU DSP上移后地址未归一，需要低软将地址指向TTF的DDR空间,by sunbing */
    {BSP_IP_TYPE_GBBP,           (void*)HI_GBBP_REG_BASE_ADDR},
    {BSP_IP_TYPE_GBBP_DRX,       (void*)HI_GBBP_DRX_REG_BASE_ADDR},
    {BSP_IP_TYPE_GBBP1,          (void*)HI_GBBP1_REG_BASE_ADDR},
    {BSP_IP_TYPE_GBBP1_DRX,      (void*)HI_GBBP1_DRX_REG_BASE_ADDR},
    {BSP_IP_TYPE_WBBP,           (void*)HI_WBBP_REG_BASE_ADDR},
    {BSP_IP_TYPE_WBBP_DRX,       (void*)HI_WBBP_DRX_REG_BASE_ADDR},
    {BSP_IP_TYPE_SYSCTRL,        (void*)HI_SYSCTRL_BASE_ADDR},
    {BSP_IP_TYPE_TDSSYS,         (void*)HI_BBP_TSTU_BASE_ADDR},
    {BSP_IP_TYPE_BBPMASTER,		(void*)HI_BBPMASTER_REG_BASE_ADDR},
    {BSP_IP_TYPE_HDLC,			(void*)HI_HDLC_REGBASE_ADDR},	/* 目前只有HDLC需要在A核使用,上层自己映射虚地址 */
    {BSP_IP_TYPE_CTU,       	(void*)HI_CTU_BASE_ADDR}, /* 这个地址保证多模平台可正常工作，现由BSP_CONFIG_V7R2_ASIC宏控制 */
    {BSP_IP_TYPE_COMMON,		(void*)HI_BBP_COMM_ON_BASE_ADDR},

    {BSP_IP_TYPE_BUTTOM, 0}
};

BSP_MEM_ADDR_SIZE_INFO g_ulMemBaseSize[] = {
    {BSP_DDR_TYPE_DDR_GU,                            (void*)DDR_GU_ADDR,                               DDR_GU_SIZE},
    {BSP_DDR_TYPE_DDR_TLPHY_IMAGE,                     (void*)DDR_TLPHY_IMAGE_ADDR,                      DDR_TLPHY_IMAGE_SIZE},
    {BSP_DDR_TYPE_DDR_LPHY_SDR,                        (void*)DDR_LPHY_SDR_ADDR,                         DDR_LPHY_SDR_SIZE},
    {BSP_DDR_TYPE_DDR_TLPHY_LCS,                       (void*)DDR_LCS_ADDR,                              DDR_LCS_SIZE},
    {BSP_DDR_TYPE_DDR_TLPHY_BANDNV,                   (void*) CORESHARE_MEM_TENCILICA_MULT_BAND_ADDR,    CORESHARE_MEM_TENCILICA_MULT_BAND_SIZE},
    {BSP_DDR_TYPE_DDR_HIFI,                            (void*)DDR_HIFI_ADDR,                             DDR_HIFI_SIZE},
    {BSP_DDR_TYPE_SRAM_TLPHY,                        /*SRAM_TLPHY_ADDR*/  (void*)0,                      SRAM_TLPHY_SIZE},
    {BSP_DDR_TYPE_SRAM_RTT_SLEEP_FLAG,               /*SRAM_RTT_SLEEP_FLAG_ADDR*/  (void*)0,             SRAM_RTT_SLEEP_FLAG_SIZE},
    {BSP_DDR_TYPE_SHM_TIMESTAMP,                     /*SHM_TIMESTAMP_ADDR*/  (void*)0,                   SHM_TIMESTAMP_SIZE},
    {BSP_DDR_TYPE_SRAM_MAILBOX_PROTECT_FLG,          /*DSP_MAILBOX_PROTECT_FLAG_ADDR*/  (void*)0,        DSP_MAILBOX_PROTECT_FLAG_SIZE},
    {BSP_DDR_TYPE_MAILBOX_HEAD_BBE16_HIFI_MSG,         (void*)MAILBOX_HEAD_ADDR(BBE16, HIFI, MSG),       MAILBOX_HEAD_LEN},
    {BSP_DDR_TYPE_MAILBOX_HEAD_HIFI_BBE16_MSG,         (void*)MAILBOX_HEAD_ADDR(HIFI, BBE16, MSG),       MAILBOX_HEAD_LEN},
    {BSP_DDR_TYPE_MAILBOX_QUEUE_BBE16_HIFI_MSG,        (void*)MAILBOX_QUEUE_ADDR(BBE16, HIFI, MSG),      MAILBOX_QUEUE_SIZE(BBE16, HIFI, MSG)},
    {BSP_DDR_TYPE_MAILBOX_QUEUE_HIFI_BBE16_MSG,        (void*)MAILBOX_QUEUE_ADDR(HIFI, BBE16, MSG),      MAILBOX_QUEUE_SIZE(HIFI, BBE16, MSG)},
    {BSP_DDR_TYPE_BUTTOM,                              (void*)0,                                         0}
};

void* bsp_get_ip_base_addr(u32 ip_type)
{
	u32 i;

    if(ip_type >= BSP_IP_TYPE_BUTTOM)
    {
        return 0;
    }
	/*lint --e{64, 63, 52 } */
	for(i=0; i<BSP_IP_TYPE_BUTTOM; i++)
	{
		if(i == ip_type)
		{
    		return g_ulIPBaseAddr[i].ulIPBaseAddr;
		}
	}

    return 0;
}

void bsp_show_ip_addr(void)
{
	u32 i=0;
	for(i=0;i<BSP_IP_TYPE_BUTTOM;i++)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HWADP,"ip_addr[i] = 0x%p\n",g_ulIPBaseAddr[i].ulIPBaseAddr);
	}
}
void* BSP_GetIPBaseAddr(BSP_IP_TYPE_E enIPType)
{
    return bsp_get_ip_base_addr(enIPType);
}

BSP_S32 BSP_GetMemAddrSize(BSP_DDR_TYPE_E enDdrType, void**  addr, BSP_U32 * size)
{
	s32 i = 0;

	if(!addr || !size || (enDdrType >= BSP_DDR_TYPE_BUTTOM))
		return -1;
	for(i = 0; (i < (s32)BSP_DDR_TYPE_BUTTOM) && (enDdrType != g_ulMemBaseSize[i].enIPType); i++)
		;
	if(i >= (s32)BSP_DDR_TYPE_BUTTOM)
	{
		return -1;
	}
	*addr = g_ulMemBaseSize[i].mem_base;
	*size = g_ulMemBaseSize[i].size;
	return 0;
}

BSP_S32 BSP_GetIntNO(BSP_INT_TYPE_E enIntType)
{
	/* GU HDLC单模存在问题，需与上层讨论后再适配 */
	return enIntType ;
}


/*lint -save -e437*/

/*****************************************************************************
 函 数 名  : BSP_CheckModuleSupport
 功能描述  : 查询模块是否支持
 输入参数  : enModuleType: 需要查询的模块类型
 输出参数  : 无
 返回值    ：BSP_MODULE_SUPPORT或BSP_MODULE_UNSUPPORT
*****************************************************************************/
BSP_MODULE_SUPPORT_E BSP_CheckModuleSupport(BSP_MODULE_TYPE_E enModuleType)
{
	u32 ret;
    bool bSupport = BSP_MODULE_SUPPORT;
	DRV_MODULE_SUPPORT_STRU   stSupportNv = {0};

    if(enModuleType >= BSP_MODULE_TYPE_BUTTOM)
    {
        return BSP_MODULE_UNSUPPORT;
    }
	
	ret = bsp_nvm_read(NV_ID_DRV_MODULE_SUPPORT, (u8*)&stSupportNv, (unsigned int)sizeof(DRV_MODULE_SUPPORT_STRU));/*lint !e26 !e119 */
	if(NV_OK != ret)
	{
		memset(&stSupportNv, 0xFF,sizeof(DRV_MODULE_SUPPORT_STRU));
		/*bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_HWADP,
			"bsp_nvm_read error, ret %#x, item %#x\n, set to default %#x",
			ret, (u32)NV_ID_DRV_MODULE_SUPPORT, stSupportNv);*/
	}

    switch(enModuleType)
    {
        case BSP_MODULE_TYPE_SD:
            bSupport = stSupportNv.sdcard;
            break;

        case BSP_MODULE_TYPE_CHARGE:
            bSupport = stSupportNv.charge;
            break;

        case BSP_MODULE_TYPE_WIFI:
            bSupport = stSupportNv.wifi;
            break;

        case BSP_MODULE_TYPE_OLED:
            bSupport = stSupportNv.oled;
            break;

        case BSP_MODULE_TYPE_HIFI:
			bSupport = stSupportNv.hifi;
            break;

        case BSP_MODULE_TYPE_POWER_ON_OFF:
			bSupport = stSupportNv.onoff;
            break;

        case BSP_MODULE_TYPE_HSIC:
			bSupport = stSupportNv.hsic;
            break;

        case BSP_MODULE_TYPE_LOCALFLASH:
			bSupport = stSupportNv.localflash;
            break;

		default:
			bSupport = 0;
    }

    return bSupport ? BSP_MODULE_SUPPORT : BSP_MODULE_UNSUPPORT;/* [false alarm]:误报 */
}/* [false alarm]:误报 */

static int hwadp_init(void)
{
	s32 i = 0;

	for(i = 0; i < BSP_DDR_TYPE_BUTTOM; i++)
	{
		switch(g_ulMemBaseSize[i].enIPType)
		{
			case BSP_DDR_TYPE_SRAM_RTT_SLEEP_FLAG:
				g_ulMemBaseSize[i].mem_base = SRAM_RTT_SLEEP_FLAG_ADDR;
				break;
			case BSP_DDR_TYPE_SHM_TIMESTAMP:
				g_ulMemBaseSize[i].mem_base = SHM_TIMESTAMP_ADDR;
				break;
			case BSP_DDR_TYPE_SRAM_MAILBOX_PROTECT_FLG:
				g_ulMemBaseSize[i].mem_base = DSP_MAILBOX_PROTECT_FLAG_ADDR;
				break;
			case BSP_DDR_TYPE_SRAM_TLPHY:
				g_ulMemBaseSize[i].mem_base = SRAM_TLPHY_ADDR;
				break;
			default:
				break;
		}
	}
	return 0;
}

subsys_initcall(hwadp_init);
/*lint -restore +e437*/


#include "osl_bio.h"
#include "boot/boot.h"
#include "bsp_shared_ddr.h"
#include "balongv7r2/types.h"
#include "drv_version.h"
#include "product_config.h"
#include "balongv7r2/version.h"
#include <bsp_hkadc.h>

#ifndef VER_ERROR
#define VER_ERROR (-1)
#endif

#ifndef VER_OK
#define VER_OK 0
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/(sizeof((a)[0])))
#endif

#define UDP_OFFSET 24
#define PRODUCT_OFFSET 16
#define BIG_VER_OFFSET 8


/* votage range */
typedef struct _tvol_range
{
    u16 vol_low;
    u16 vol_high;
}vol_range;


/* local variables */
static vol_range version_vol_tab[] =
{
    {0,147},
    {148,423},
    {424,699},
    {700,975},
    {976,1250},
    {1251,1526},
    {1527,1802},
    {1803,2078},
    {2079,2354},
    {2355,2500}
};


int balong_version_get_hkadc_value(hard_ware_version hw_ver,u8 *hkadc_ver)
{

/*TODO:waiting HKADC OK*/
	u16 adc_result = 0x00;/*存储hkadc读出的电压值*/
	u8 ret;
	u32 idx;

	/*get hkadc value*/
	ret=bsp_hkadc_convert((enum HKADC_CHANNEL_ID)hw_ver,&adc_result);
	if(VER_OK!=ret){
		cprintf("get hkadc value fail!\n");
		return VER_ERROR;
	}

	/*get version table value*/
	for(idx=0;idx<ARRAY_SIZE(version_vol_tab);idx++)
	{
		if(version_vol_tab[idx].vol_low<= adc_result
            && adc_result <= version_vol_tab[idx].vol_high)
		{
			/*cprintf("ver%d is:%d\n", hw_ver, idx);*/
			*hkadc_ver=idx;
			break;
		}
	}

	/* not found */
	if(idx == ARRAY_SIZE(version_vol_tab))
	{
		cprintf("get hkadc value fail, idx:%d, adc_result:%d\n", idx, adc_result);
		return VER_ERROR;
	}

	return VER_OK;

}

/*****************************************************************************
* 函 数 名  	: balong_version_get_hw_version
*
* 功能描述  : 获得硬件版本号索引
*
* 输入参数  : 无
* 输出参数  :
*
* 返 回 值  	: 硬件版本号索引
* 修改记录  :
*
*****************************************************************************/

u32 balong_version_get_hw_version(void)
{
	u8 udp=0;
	u8 product=0;
	u8 big_ver_high=0;
	u8 big_ver_low =0;

	int ret=VER_OK;
	u32 hardId = 0xFFFFFFFF;

/*p531fpga*/
#if (defined(BSP_CONFIG_P531_ASIC)&&defined (BSP_CONFIG_BOARD_FPGA))
	return HW_VER_PRODUCT_P531_FPGA;
#endif

/*p531asic va*/
#if (defined (BSP_CONFIG_P531_ASIC)&&defined (BSP_CONFIG_BOARD_VA))
     return HW_VER_PRODUCT_P531_ASIC_VA;
#endif
/*p531asic vc*/
#if (defined (BSP_CONFIG_P531_ASIC)&&defined(BSP_CONFIG_BOARD_VC))
	return HW_VER_PRODUCT_P531_ASIC_VC;
#endif

/*v7r2 sft*/
#ifdef BSP_CONFIG_V7R2_SFT
	return HW_VER_PRODUCT_SFT;
#endif

/*v711 porting*/
#ifdef BSP_CONFIG_V711_PORTING
	return HW_VER_PRODUCT_PORTING;
#endif

/*v711 porting*/
#ifdef BSP_CONFIG_V711_ASIC
	return HW_VER_V711_UDP;
#endif

	/*others by HKADC read*/

	/*判断是否为UDP单板*/
	/* coverity[un_reachable] */
	ret= balong_version_get_hkadc_value(HW_VERSION_UDP,&udp);
	if(VER_OK!=ret){
		cprintf("balong_version_get_hkadc_value error!%d\n",ret);
		return HW_VER_INVALID;
	}

	/*查询大版本号高8bit*/
	ret=balong_version_get_hkadc_value(HW_VERSION_BIG_HIGH,&big_ver_high);

	if(VER_OK!=ret){
		cprintf("balong_version_get_hkadc_value error!%d\n",ret);
		return HW_VER_INVALID;
	}

	/*查询大版本号低8bit*/
	ret=balong_version_get_hkadc_value(HW_VERSION_BIG_LOW,&big_ver_low);
	if(VER_OK!=ret){
		cprintf("balong_version_get_hkadc_value error!%d\n",ret);
		return HW_VER_INVALID;
	}

	/*若不是UDP单板，查询单板产品形态*/
	if(udp){
		ret=balong_version_get_hkadc_value(HW_VERSION_PRODUCT,&product);
		if(VER_OK!=ret){
			cprintf("balong_version_get_hkadc_value error!%d\n",ret);
			return HW_VER_INVALID;
		}

		/*版本号编码规则中将非udp版本号高8位置0X00*/
		hardId=product<<PRODUCT_OFFSET|big_ver_high<<BIG_VER_OFFSET|big_ver_low;
		if(0x00000000==hardId){
			hardId=HW_VER_PRODUCT_E5379;
		}
		return hardId;
	}
	/*版本号编码规则中将udp版本号高8位置0X7F*/
	return (CONFIG_VER_MASK)|(big_ver_high<<BIG_VER_OFFSET)|big_ver_low;

}

/*****************************************************************************
* 函 数 名  	: bsp_save_hw_version
*
* 功能描述  : 向AXI MEMORY中存储hw_id
*
* 输入参数  : 无
* 输出参数  :
*
* 返 回 值  	:
				0:成功
				-1:失败
* 修改记录  :
*
*****************************************************************************/
int bsp_save_hw_version(void)
{
	u32 hw_main = HW_VER_INVALID;

    u32 *pu32HwId = (u32 *)SHM_MEM_HW_VER_ADDR;

	/*sub_Ver 也是通过HAKDC识别的，无需关注*/
    hw_main = balong_version_get_hw_version();
	if(HW_VER_INVALID==hw_main){
		cprintf("[bsp_save_hw_version]:get hw version failed!\n");
	}

    *pu32HwId = hw_main;

	return VER_OK;

}

/*****************************************************************************
* 函数	: bsp_get_board_chip_type
* 功能	: get board type and chip type
* 输入	: void
* 输出	: void
* 返回	: BOARD_TYPE_E
*****************************************************************************/
u32 bsp_version_get_board_chip_type(void)
{
	u32 type = 0xffffffff;
	type = *(u32 *)SHM_MEM_HW_VER_ADDR;

	if(HW_VER_INVALID==type){
		cprintf("[bsp_get_board_chip_type]:get hw version failed!hw_ver:0x%x\n",type);
		return type;
	}

	switch( type & (~CONFIG_TYPE_MASK)){
		case HW_VER_UDP_MASK:
		case HW_VER_K3V3_UDP_MASK:
		case HW_VER_K3V3_PLUS_UDP_MASK:
		case HW_VER_V711_UDP_MASK:
			return (CONFIG_VER_MASK | CONFIG_VER_UDP_MASK);
		default:
			return type;
	}
	return type;
}

/*****************************************************************************
* 函数	: bsp_get_board_mode_type
* 功能	: get board type(GUL or LTE)
* 输入	: void
* 输出	: void
* 返回	: BOARD_TYPE_E
*****************************************************************************/
BOARD_TYPE_E bsp_get_board_mode_type(void)
{
#if(defined(FEATURE_MULTIMODE_GUL))
	return BOARD_TYPE_GUL;
#else
	return BOARD_TYPE_LTE_ONLY;
#endif
}

/*****************************************************************************
* 函数	: bsp_board_type_is_gutl
* 功能	: get board type is GUTL
* 输入	: void
* 输出	: void
* 返回	: BOARD_TYPE_E
*****************************************************************************/
bool bsp_board_type_is_gutl(void)
{
	return (BOARD_TYPE_GUL == bsp_get_board_mode_type() ? 1 : 0);
}


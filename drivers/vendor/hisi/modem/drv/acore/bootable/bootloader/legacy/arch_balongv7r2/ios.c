#include <boot/boot.h>
#include "product_config.h"
#include "bsp_reg_def.h"
#include "version.h"
#include "gpio.h"

#include "ios_list.h"
#include "ios.h"

#if defined(BSP_CONFIG_P531_ASIC)
#include "ios_drv_macro.h"
#include "ios_save.h"
#include "p531_va_ios_config.h"
#include "p531_vc_ios_config.h"

#elif (defined(BSP_CONFIG_V7R2_SFT))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_save.h"
#include "ios_pd_save.h"
#include "v7r2_porting_ios_config.h"

#elif (defined(BSP_CONFIG_V7R2_ASIC))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_save.h"
#include "ios_pd_save.h"
#include "udp_ios_pd_config.h"
#include "udp_ios_ao_config.h"
#include "udp_ios_pd_config_save.h"
#include "udp_ios_ao_config_save.h"
#include "e5379_ios_pd_config.h"
#include "e5379_ios_ao_config.h"
#include "e5379_ios_pd_config_save.h"
#include "e5379_ios_ao_config_save.h"

#elif (defined(BSP_CONFIG_V711_PORTING))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_save.h"
#include "ios_pd_save.h"
#include "v711_porting_ios_config.h"

#elif (defined(BSP_CONFIG_V711_ASIC))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_save.h"
#include "ios_pd_save.h"
#include "v711_udp_ios_ao_config.h"
#include "v711_udp_ios_ao_config_save.h"
#include "v711_udp_ios_pd_config.h"
#include "v711_udp_ios_pd_config_save.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


void sep_io_mux(void)
{
#if ((FEATURE_ON == FEATURE_MULTI_MODEM) && defined(BSP_CONFIG_V7R2_ASIC))
	/*USIM1(3PIN)*/
    /*usim1_clk*/
    SET_IOS_USIM1_CTRL1_1;
    CLR_IOS_GPIO0_10_CTRL1_1;
    /*usim1_clk*/
    NASET_IOS_AO_IOM_CTRL14;

    /*usim1_rst*/
    SET_IOS_USIM1_CTRL1_1;
    OUTSET_IOS_AO_IOM_CTRL15;
    CLR_IOS_GPIO0_11_CTRL1_1;
    /*usim1_rst*/
    NASET_IOS_AO_IOM_CTRL15;

    /*usim1_data*/
    SET_IOS_USIM1_CTRL1_1;
    CLR_IOS_GPIO0_12_CTRL1_1;
    /*usim1_data*/
    PUSET_IOS_AO_IOM_CTRL16;

    /*usim1_clk*/
    add_ios_list(IOS_AO_MF_CTRL2);
    add_ios_list(IOS_AO_AF_CTRL2);
    /*usim1_clk*/
    add_ios_list(IOS_AO_IOM_CTRL14);

    /*usim1_rst*/
    add_ios_list(IOS_AO_MF_CTRL2);
    add_ios_list(IOS_AO_AF_CTRL2);
    add_ios_list(IOS_AO_IOM_CTRL15);
    /*usim1_rst*/
    add_ios_list(IOS_AO_IOM_CTRL15);

    /*usim1_data*/
    add_ios_list(IOS_AO_MF_CTRL2);
    add_ios_list(IOS_AO_AF_CTRL2);
    /*usim1_data*/
    add_ios_list(IOS_AO_IOM_CTRL16);
#endif
}

void pcm_io_mux(void)
{
#if defined(BSP_CONFIG_V7R2_ASIC)
    I4MASET_IOS_PD_IOM_CTRL21;
    /*pcm_clk管脚复用配置*/
    CLR_IOS_PCM_CTRL2_1;
    SET_IOS_PCM_CTRL2_2;
    CLR_IOS_MMC0_CLK_CTRL1_1;
    CLR_IOS_JTAG1_CTRL1_1;
    /*pcm_clk管脚上下拉配置*/
    NASET_IOS_PD_IOM_CTRL21;

    /*pcm_sync管脚复用配置*/
    CLR_IOS_PCM_CTRL2_1;
    SET_IOS_PCM_CTRL2_2;
    CLR_IOS_MMC0_CTRL1_1;
    CLR_IOS_JTAG1_CTRL1_1;
    /*pcm_sync管脚上下拉配置*/
    NASET_IOS_PD_IOM_CTRL22;

    /*pcm_di管脚复用配置*/
    CLR_IOS_PCM_CTRL2_1;
    SET_IOS_PCM_CTRL2_2;
    INSET_IOS_PD_IOM_CTRL23;
    CLR_IOS_MMC0_CTRL1_1;
    CLR_IOS_JTAG1_CTRL1_1;
    /*pcm_di管脚上下拉配置*/
    PDSET_IOS_PD_IOM_CTRL23;

    /*pcm_do管脚复用配置*/
    CLR_IOS_PCM_CTRL2_1;
    SET_IOS_PCM_CTRL2_2;
    OUTSET_IOS_PD_IOM_CTRL24;
    CLR_IOS_MMC0_CTRL1_1;
    CLR_IOS_JTAG1_CTRL1_1;
    /*pcm_do管脚上下拉配置*/
    NASET_IOS_PD_IOM_CTRL24;

    /*pcm_clk管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL3);
    /*pcm_clk管脚上下拉配置保存*/
    add_ios_list(IOS_PD_IOM_CTRL21);

    /*pcm_sync管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL3);
    /*pcm_sync管脚上下拉配置保存*/
    add_ios_list(IOS_PD_IOM_CTRL22);

    /*pcm_di管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL3);
    add_ios_list(IOS_PD_IOM_CTRL23);
    /*pcm_di管脚上下拉配置保存*/
    add_ios_list(IOS_PD_IOM_CTRL23);

    /*pcm_do管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL3);
    add_ios_list(IOS_PD_IOM_CTRL24);
    /*pcm_do管脚上下拉配置保存*/
    add_ios_list(IOS_PD_IOM_CTRL24);

#endif

}

void ios_init(void)
{

	u32 product_type = 0xff;
	product_type = bsp_version_get_board_chip_type();

    switch(product_type)
    {
#if defined(BSP_CONFIG_P531_ASIC)
        case HW_VER_PRODUCT_P531_ASIC_VA:
            P531_VA_IOS_CONFIG;
            break;

        case HW_VER_PRODUCT_P531_ASIC_VC:
            P531_VC_IOS_CONFIG;
            IOS_SAVE;
            break;

        case HW_VER_PRODUCT_P531_FPGA:
            P531_VA_IOS_CONFIG;
            break;

#elif (defined(BSP_CONFIG_V7R2_SFT))
        case HW_VER_PRODUCT_SFT:
            V7R2_PORTING_IOS_CONFIG;
            break;

#elif (defined(BSP_CONFIG_V7R2_ASIC))
        case HW_VER_PRODUCT_UDP:
            UDP_IOS_CONFIG;
            UDP_IOS_CONFIG_SAVE;
			sep_io_mux();
            break;

        case HW_VER_PRODUCT_E5379:
            E5379_IOS_CONFIG;
            E5379_IOS_CONFIG_SAVE;
            break;
#elif (defined(BSP_CONFIG_V711_PORTING))
        case HW_VER_PRODUCT_PORTING:
            V711_PORTING_IOS_CONFIG;
            break;

#elif (defined(BSP_CONFIG_V711_ASIC))
        case HW_VER_V711_UDP:
            V711_UDP_IOS_CONFIG;
			V711_UDP_IOS_CONFIG_SAVE;
            break;

#endif
        case HW_VER_INVALID:
            cprintf("hardware version is invalid.\n");
            break;

        default:
            cprintf("hardware version cannot be identified. id:0x%x\n", product_type);

    }


}

void jtag1_config(void)
{
#if (defined(BSP_CONFIG_V7R2_SFT) || defined(BSP_CONFIG_V7R2_ASIC))
/*配置MMC0（6个PIN）（SD MASTER/SDIO SLAVE）*/
    /*jtag1_rtck管脚复用配置*/
    SET_IOS_JTAG1_CTRL1_1;
    OUTSET_IOS_PD_IOM_CTRL21;
    CLR_IOS_MMC0_CLK_CTRL1_1;
    /*jtag1_rtck管脚上下拉配置*/
    NASET_IOS_PD_IOM_CTRL21;

    /*jtag1_tck管脚复用配置*/
    SET_IOS_JTAG1_CTRL1_1;
    INSET_IOS_PD_IOM_CTRL22;
    CLR_IOS_MMC0_CTRL1_1;
    /*jtag1_tck管脚上下拉配置*/
    PDSET_IOS_PD_IOM_CTRL22;

    /*jtag1_trst_n管脚复用配置*/
    SET_IOS_JTAG1_CTRL1_1;
    CLR_IOS_MMC0_CTRL1_1;

    /*jtag1_tms管脚复用配置*/
    SET_IOS_JTAG1_CTRL1_1;
    INSET_IOS_PD_IOM_CTRL24;
    CLR_IOS_MMC0_CTRL1_1;

    /*jtag1_tdi管脚复用配置*/
    SET_IOS_JTAG1_CTRL1_1;
    INSET_IOS_PD_IOM_CTRL25;
    CLR_IOS_MMC0_CTRL1_1;

    /*jtag1_tdo管脚复用配置*/
    SET_IOS_JTAG1_CTRL1_1;
    CLR_IOS_MMC0_CTRL1_1;
	
/*配置MMC0（6个PIN）（SD MASTER/SDIO SLAVE）*/
    /*jtag1_rtck管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
    add_ios_list(IOS_PD_IOM_CTRL21);
    /*jtag1_rtck管脚上下拉配置保存*/
    add_ios_list(IOS_PD_IOM_CTRL21);

    /*jtag1_tck管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
    add_ios_list(IOS_PD_IOM_CTRL22);
    /*jtag1_tck管脚上下拉配置保存*/
    add_ios_list(IOS_PD_IOM_CTRL22);

    /*jtag1_trst_n管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);

    /*jtag1_tms管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
    add_ios_list(IOS_PD_IOM_CTRL24);

    /*jtag1_tdi管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
    add_ios_list(IOS_PD_IOM_CTRL25);

    /*jtag1_tdo管脚复用配置保存*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
#endif

}

#ifdef __cplusplus
}
#endif




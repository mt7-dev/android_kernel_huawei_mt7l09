/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 : mtcmos_fastboot.c
*
*   作    者 :  yangqiang
*
*   描    述 : 本文件用于关闭系统时钟策略中的非用即关IP时钟
*
*   修改记录 :  2013年8月10日  v1.00 yangqiang创建
*************************************************************************/

#include <hi_pwrctrl_interface.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>

#ifdef REGULATOR_V7R2_MTCMOS

void bsp_disable_mtcmos(void)
{
	unsigned int cmos_dis_flag =0xd,ret=0;
	DRV_NV_PM_CLKINIT_STRU  g_clk_init;
    ret = bsp_nvm_read(NV_ID_DRV_PM_CLKINIT, (u8 *)&g_clk_init, sizeof(g_clk_init));
	if(NV_OK!=ret)
	  	return;
       cmos_dis_flag = g_clk_init.mtcmosdis;

	//sd mtcmos
	if(cmos_dis_flag&0x1)
	{
		hi_pwrctrl_regu_sdmtcmos_dis();
	}

	//usb mtcmos
	if(cmos_dis_flag&0x2)
	{
	 	hi_pwrctrl_regu_usbmtcmos_dis();
	}
	//hsic mtcmos
	if(cmos_dis_flag&0x4)
	{
		hi_pwrctrl_regu_hiscmtcmos_dis();
	}
	//hifi mtcmos
	if(cmos_dis_flag&0x8)
	{
		hi_pwrctrl_regu_hifimtcmos_dis();
	}
	//irm mtcmos
	if(cmos_dis_flag&0x10)
	{
		hi_pwrctrl_regu_irmmtcmos_dis();
	}
	//g2bbp mtcmos
	if(cmos_dis_flag&0x20)
	{
		hi_pwrctrl_regu_g2bbpmtcmos_dis();
	}
	//g1bbp mtcmos
	if(cmos_dis_flag&0x40)
	{
		hi_pwrctrl_regu_g1bbpmtcmos_dis();
	}
	//wbbp mtcmos
	if(cmos_dis_flag&0x80)
	{
		hi_pwrctrl_regu_wbbpmtcmos_dis();
	}
	//twbbp mtcmos
	if(cmos_dis_flag&0x100)
	{
		hi_pwrctrl_regu_twbbpmtcmos_dis();
	}

	//lbbp mtcmos
	if(cmos_dis_flag&0x200)
	{
		hi_pwrctrl_regu_lbbpmtcmos_dis();
	}

	//dsp0 mtcmos
	if(cmos_dis_flag&0x400)
	{
		hi_pwrctrl_regu_dsp0mtcmos_dis();
	}
	return;
}

#else

void bsp_disable_mtcmos(void)
{
	return;
}

#endif
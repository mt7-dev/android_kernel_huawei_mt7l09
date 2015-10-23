#ifndef __HI_PWRCTRL_INTER_H__
#define __HI_PWRCTRL_INTER_H__

#include "hi_base.h"
#include "hi_pwrctrl.h"
#include "hi_syssc.h"


/*irm 钳位禁使能*/
static __inline__ void set_hi_pwr_ctrl5_irm_iso_dis(unsigned int v)
{
	return set_hi_pwr_ctrl5_irmbbp_iso_ctrl_dis(v);
}

/*irm 钳位使能*/
static __inline__ void set_hi_pwr_ctrl4_irm_iso_en(unsigned int v)
{
	set_hi_pwr_ctrl4_irmbbp_iso_ctrl_en(v);
}

/*irm上电*/
static __inline__ void set_hi_pwr_ctrl6_irm_mtcmos_en(unsigned int v)
{
	set_hi_pwr_ctrl6_irmbbp_mtcmos_ctrl_en(v);
}

/*irm 下电*/
static __inline__ void set_hi_pwr_ctrl7_irm_mtcmos_dis(unsigned int v)
{
	set_hi_pwr_ctrl7_irmbbp_mtcmos_ctrl_dis(v);
}

/*irm 状态查询*/
static __inline__ unsigned int get_hi_pwr_stat1_irm_mtcmos_rdy(void)
{
	return get_hi_pwr_stat1_irmbbp_mtcmos_rdy_stat();
}


#ifndef CONFIG_REGULATOR_FOR_HI6421
#define CONFIG_REGULATOR_FOR_HI6421
#endif
/********************************************************************************/
/*    pwrctrl 函数（hi_pwrctrl_模块名_功能)        */
/********************************************************************************/
/*以下接口用于regulator给mtcmos上下电操作，en表示上电，dis表示下电 ,stat表示状态*/
/****************************V7R2 / K3 start*********************************/
/*sd_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_sdmtcmos_en(void)
{
}
static INLINE void hi_pwrctrl_regu_sdmtcmos_dis(void)
{
}
static INLINE unsigned int hi_pwrctrl_regu_sdmtcmos_stat(void)
{
	return 0;
}
/*usb_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_usbmtcmos_en(void)
{
}
static INLINE void hi_pwrctrl_regu_usbmtcmos_dis(void)
{
}
static INLINE unsigned int hi_pwrctrl_regu_usbmtcmos_stat(void)
{
	return 0;
}
/*irm_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_irmmtcmos_en(void)
{
	 set_hi_pwr_ctrl6_irmbbp_mtcmos_ctrl_en(1);
	 while(!get_hi_pwr_stat1_irmbbp_mtcmos_rdy_stat())
	{
		;
	}
	set_hi_crg_srstdis3_irm_pd_srst_dis(1);
	set_hi_pwr_ctrl5_irmbbp_iso_ctrl_dis(1);
	set_hi_crg_clken5_irm_pd_clk_en(1);
}

static INLINE void hi_pwrctrl_regu_irmmtcmos_dis(void)
{
	set_hi_crg_clkdis5_irmbbp_pd_clk_dis(1);
	set_hi_pwr_ctrl4_irmbbp_iso_ctrl_en(1);
	set_hi_crg_srsten3_irm_pd_srst_en(1);
	set_hi_pwr_ctrl7_irmbbp_mtcmos_ctrl_dis(1);
}
static INLINE unsigned int hi_pwrctrl_regu_irmmtcmos_stat(void)
{
	return get_hi_pwr_stat1_irmbbp_mtcmos_rdy_stat();
}
/*g2bbp_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_g2bbpmtcmos_en(void)
{
	set_hi_pwr_ctrl6_g2bbp_mtcmos_ctrl_en(1);
	while(!get_hi_pwr_stat1_g2bbp_mtcmos_rdy_stat())
	{
		;
	}
	set_hi_crg_srstdis3_g2bbp_pd_srst_dis(1);
	set_hi_pwr_ctrl5_g2bbp_iso_ctrl_dis(1);
	set_hi_crg_clken5_g2bbp_pd_clk_en(1);
}
static INLINE void hi_pwrctrl_regu_g2bbpmtcmos_dis(void)
{
	set_hi_crg_clkdis5_g2bbp_pd_clk_dis(1);
	set_hi_pwr_ctrl4_g2bbp_iso_ctrl_en(1);
	set_hi_crg_srsten3_g2bbp_pd_srst_en(1);
	set_hi_pwr_ctrl7_g2bbp_mtcmos_ctrl_dis(1);
}
static INLINE unsigned int hi_pwrctrl_regu_g2bbpmtcmos_stat(void)
{
	return get_hi_pwr_stat1_g2bbp_mtcmos_rdy_stat();
}
/*g1bbp_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_g1bbpmtcmos_en(void)
{
	set_hi_pwr_ctrl6_g1bbp_mtcmos_ctrl_en(1);
	while(!get_hi_pwr_stat1_g1bbp_mtcmos_rdy_stat())
	{
		;
	}
	set_hi_crg_srstdis3_g1bbp_pd_srst_dis(1);
	set_hi_pwr_ctrl5_g1bbp_iso_ctrl_dis(1);
	set_hi_crg_clken5_g1bbp_pd_clk_en(1);
}
static INLINE void hi_pwrctrl_regu_g1bbpmtcmos_dis(void)
{
	set_hi_crg_clkdis5_g1bbp_pd_clk_dis(1);
	set_hi_pwr_ctrl4_g1bbp_iso_ctrl_en(1);
	set_hi_crg_srsten3_g1bbp_pd_srst_en(1);
	set_hi_pwr_ctrl7_g1bbp_mtcmos_ctrl_dis(1);
}
static INLINE unsigned int hi_pwrctrl_regu_g1bbpmtcmos_stat(void)
{
	return get_hi_pwr_stat1_g1bbp_mtcmos_rdy_stat();
}
/*wbbp_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_wbbpmtcmos_en(void)
{
	set_hi_pwr_ctrl6_wbbp_mtcmos_ctrl_en(1);
	while(!get_hi_pwr_stat1_wbbp_mtcmos_rdy_stat())
	{
		;
	}
	set_hi_crg_srstdis3_wbbp_pd_srst_dis(1);
	set_hi_pwr_ctrl5_wbbp_iso_ctrl_dis(1);
	set_hi_crg_clken5_wbbp_pd_clk_en(1);
}
static INLINE void hi_pwrctrl_regu_wbbpmtcmos_dis(void)
{
	set_hi_crg_clkdis5_wbbp_pd_clk_dis(1);
	set_hi_pwr_ctrl4_wbbp_iso_ctrl_en(1);
	set_hi_crg_srsten3_wbbp_pd_srst_en(1);
	set_hi_pwr_ctrl7_wbbp_mtcmos_ctrl_dis(1);
}
static INLINE unsigned int hi_pwrctrl_regu_wbbpmtcmos_stat(void)
{
	return get_hi_pwr_stat1_wbbp_mtcmos_rdy_stat();
}
/*twbbp_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_twbbpmtcmos_en(void)
{
	set_hi_pwr_ctrl6_twbbp_mtcmos_ctrl_en(1);
	while(!get_hi_pwr_stat1_twbbp_mtcmos_rdy_stat())
	{
		;
	}
	set_hi_crg_srstdis3_twbbp_pd_srst_dis(1);
	set_hi_pwr_ctrl5_twbbp_iso_ctrl_dis(1);
	set_hi_crg_clken5_twbbp_pd_clk_en(1);
}
static INLINE void hi_pwrctrl_regu_twbbpmtcmos_dis(void)
{
	set_hi_crg_clkdis5_twbbp_pd_clk_dis(1);
	set_hi_pwr_ctrl4_twbbp_iso_ctrl_en(1);
	set_hi_crg_srsten3_twbbp_pd_srst_en(1);
	set_hi_pwr_ctrl7_twbbp_mtcmos_ctrl_dis(1);
}
static INLINE unsigned int hi_pwrctrl_regu_twbbpmtcmos_stat(void)
{
	return get_hi_pwr_stat1_twbbp_mtcmos_rdy_stat();
}
/*lbbp_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_lbbpmtcmos_en(void)
{
	set_hi_pwr_ctrl6_ltebbp_mtcmos_ctrl_en(1);
	while((!get_hi_pwr_stat1_ltebbp_mtcmos_rdy_stat()) || (!get_hi_pwr_stat1_ltebbp0_mtcmos_rdy_stat() ))
	{
		;
	}
	set_hi_crg_srstdis3_lbbp_pd_srst_dis(1);
	set_hi_pwr_ctrl5_ltebbp_iso_ctrl_dis(1);
	set_hi_crg_clken5_ltebbp_pd_clk_en(1);
}
static INLINE void hi_pwrctrl_regu_lbbpmtcmos_dis(void)
{
	set_hi_crg_clkdis5_lbbp_pd_clk_dis(1);
	set_hi_pwr_ctrl4_ltebbp_iso_ctrl_en(1);
	set_hi_crg_srsten3_lbbp_pd_srst_en(1);
	set_hi_pwr_ctrl7_ltebbp_mtcmos_ctrl_dis(1);
}
static INLINE unsigned int hi_pwrctrl_regu_lbbpmtcmos_stat(void)
{
	return get_hi_pwr_stat1_ltebbp_mtcmos_rdy_stat();
}
/*hifi_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_hifimtcmos_en(void)
{
}
static INLINE void hi_pwrctrl_regu_hifimtcmos_dis(void)
{
}
static INLINE unsigned int hi_pwrctrl_regu_hifimtcmos_stat(void)
{
	return 0;
}
/*dsp0_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_dsp0mtcmos_en(void)
{
	set_hi_pwr_ctrl6_bbe16_mtcmos_ctrl_en(1);
	while(!get_hi_pwr_stat1_bbe16_mtcmos_rdy_stat())
	{
		;
	}
	set_hi_crg_srsten1_bbe_pd_srst_en(1);
	set_hi_crg_srsten1_bbe_core_srst_en(1);
	set_hi_pwr_ctrl5_bbe16_iso_ctrl_dis(1);
	set_hi_crg_clken4_bbe_pd_clk_en(1);
	set_hi_crg_clken4_bbe_core_clk_en(1);
}
static INLINE void hi_pwrctrl_regu_dsp0mtcmos_dis(void)
{
	set_hi_crg_clkdis4_dsp0_pd_clk_dis(1);
	set_hi_pwr_ctrl4_bbe16_iso_ctrl_en(1);
	set_hi_crg_srsten1_bbe_pd_srst_en(1);
	set_hi_crg_srsten1_bbe_core_srst_en(1);
	set_hi_pwr_ctrl7_bbe16_mtcmos_ctrl_dis(1);
}
static INLINE unsigned int hi_pwrctrl_regu_dsp0mtcmos_stat(void)
{
	return get_hi_pwr_stat1_bbe16_mtcmos_rdy_stat();
}
/*hisc_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_hiscmtcmos_en(void)
{
}
static INLINE void hi_pwrctrl_regu_hiscmtcmos_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_hiscmtcmos_stat(void)
{
	return 0;
}
/*moda9_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_moda9mtcmos_en(void)
{
	set_hi_pwr_ctrl6_ccpu_mtcmos_ctrl_en(1);
	while(!get_hi_pwr_stat1_ccpu_mtcmos_rdy_stat())
	{
		;
	}
	set_hi_crg_clken4_mcpu_pd_clk_en(1);
	set_hi_crg_clken4_l2c_clk_en(1);
	set_hi_pwr_ctrl5_ccpu_iso_ctrl_dis(1);
	set_hi_crg_srstdis1_mdma9_pd_srst_dis(1);
}
static INLINE void hi_pwrctrl_regu_moda9mtcmos_dis(void)
{
	set_hi_crg_clkdis4_mcpu_pd_clk_dis(1);
	set_hi_crg_clkdis4_l2c_clk_dis(1);
	set_hi_pwr_ctrl4_ccpu_iso_ctrl_en(1);
	set_hi_crg_srsten1_mdma9_pd_srst_en(1);
	set_hi_pwr_ctrl7_ccpu_mtcmos_ctrl_dis(1);
}
static INLINE unsigned int hi_pwrctrl_regu_moda9mtcmos_stat(void)
{
	return get_hi_pwr_stat1_ccpu_mtcmos_rdy_stat();
}
/*appa9_mtcmos上下电*/
static INLINE void hi_pwrctrl_regu_appa9mtcmos_en(void)
{

}
static INLINE void hi_pwrctrl_regu_appa9mtcmos_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_appa9mtcmos_stat(void)
{
	return 0;
}


/****************************V7R2 / K3 end*********************************/
/****************************p531 start*******************************/
/*tcsdrv1上下电*/
static INLINE void hi_pwrctrl_regu_tcsdrv1_en(void)
{

}
static INLINE void hi_pwrctrl_regu_tcsdrv1_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_tcsdrv1_stat(void)
{
	return 0;
}
/*fpgaif上下电*/
static INLINE void hi_pwrctrl_regu_fpgaif_en(void)
{

}
static INLINE void hi_pwrctrl_regu_fpgaif_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_fpgaif_stat(void)
{
	return 0;
}
/*t570t上下电*/
static INLINE void hi_pwrctrl_regu_t570t_en(void)
{

}
static INLINE void hi_pwrctrl_regu_t570t_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_t570t_stat(void)
{
	return 0;
}
/*a15上下电*/
static INLINE void hi_pwrctrl_regu_a15_en(void)
{

}
static INLINE void hi_pwrctrl_regu_a15_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_a15_stat(void)
{
	return 0;
}
/*hsicphy上下电*/
static INLINE void hi_pwrctrl_regu_hsicphy_en(void)
{

}
static INLINE void hi_pwrctrl_regu_hsicphy_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_hsicphy_stat(void)
{
	return 0;
}
/*usbphy上下电*/
static INLINE void hi_pwrctrl_regu_usbphy_en(void)
{

}
static INLINE void hi_pwrctrl_regu_usbphy_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_usbphy_stat(void)
{
	return 0;
}
/*appa9上下电*/
static INLINE void hi_pwrctrl_regu_appa9_en(void)
{

}
static INLINE void hi_pwrctrl_regu_appa9_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_appa9_stat(void)
{
	return 0;
}
/*hifi上下电*/
static INLINE void hi_pwrctrl_regu_hifi_en(void)
{

}
static INLINE void hi_pwrctrl_regu_hifi_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_hifi_stat(void)
{
	return 0;
}
/*bbe16上下电*/
static INLINE void hi_pwrctrl_regu_bbe16_en(void)
{

}
static INLINE void hi_pwrctrl_regu_bbe16_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_bbe16_stat(void)
{
	return 0;
}
/*moda9上下电*/
static INLINE void hi_pwrctrl_regu_moda9_en(void)
{

}
static INLINE void hi_pwrctrl_regu_moda9_dis(void)
{

}
static INLINE unsigned int hi_pwrctrl_regu_moda9_stat(void)
{
	return 0;
}

#if 1
static __inline__ void set_hi_pwr_ctrl6_g1bbp_mtcmos_en(unsigned int v)
{
	set_hi_pwr_ctrl6_g1bbp_mtcmos_ctrl_en(1);
	return ;

}

static __inline__ unsigned int get_hi_pwr_stat1_g1bbp_mtcmos_rdy(void)
{
	return get_hi_pwr_stat1_g1bbp_mtcmos_rdy_stat();
}

#if 0
static __inline__ void set_hi_crg_clken5_g1bbp_pd_clk_en(unsigned int v)
{
}

static __inline__ void set_hi_crg_srstdis3_g1bbp_pd_srst_dis(unsigned int v)
{
}
#endif

static __inline__ void set_hi_pwr_ctrl5_g1bbp_iso_dis(unsigned int v)
{
	set_hi_pwr_ctrl5_g1bbp_iso_ctrl_dis(1);
	return ;
}

static __inline__ void set_hi_pwr_ctrl4_g1bbp_iso_en(unsigned int v)
{
	set_hi_pwr_ctrl4_g1bbp_iso_ctrl_en(1);
	return ;
}

#if 0
static __inline__ void set_hi_crg_srsten3_g1bbp_pd_srst_en(unsigned int v)
{
}
static __inline__ void set_hi_crg_clkdis5_g1bbp_pd_clk_dis(unsigned int v)
{
}
#endif

static __inline__ void set_hi_pwr_ctrl7_g1bbp_mtcmos_dis(unsigned int v)
{
	set_hi_pwr_ctrl7_g1bbp_mtcmos_ctrl_dis(1);
	return ;
}
static __inline__ void set_hi_pwr_ctrl6_g2bbp_mtcmos_en(unsigned int v)
{
	set_hi_pwr_ctrl6_g2bbp_mtcmos_ctrl_en(1);
	return ;
}
static __inline__ unsigned int get_hi_pwr_stat1_g2bbp_mtcmos_rdy(void)
{
	return get_hi_pwr_stat1_g2bbp_mtcmos_rdy_stat();
}

#if 0
static __inline__ void set_hi_crg_clken5_g2bbp_pd_clk_en(unsigned int v)
{
}
static __inline__ void set_hi_crg_srstdis3_g2bbp_pd_srst_dis(unsigned int v)
{
}
#endif

static __inline__ void set_hi_pwr_ctrl5_g2bbp_iso_dis(unsigned int v)
{
	set_hi_pwr_ctrl5_g2bbp_iso_ctrl_dis(1);
	return ;
}
static __inline__ void set_hi_pwr_ctrl4_g2bbp_iso_en(unsigned int v)
{
	set_hi_pwr_ctrl4_g2bbp_iso_ctrl_en(1);
	return ;
}

#if 0
static __inline__ void set_hi_crg_srsten3_g2bbp_pd_srst_en(unsigned int v)
{
}

static __inline__ void set_hi_crg_clkdis5_g2bbp_pd_clk_dis(unsigned int v)
{
}
#endif
static __inline__ void set_hi_pwr_ctrl7_g2bbp_mtcmos_dis(unsigned int v)
{
	set_hi_pwr_ctrl7_g2bbp_mtcmos_ctrl_dis(1);
	return ;
}

static __inline__ void set_hi_pwr_ctrl6_wbbp_mtcmos_en(unsigned int v)
{
	set_hi_pwr_ctrl6_wbbp_mtcmos_ctrl_en(1);
	return ;
}
static __inline__ unsigned int get_hi_pwr_stat1_wbbp_mtcmos_rdy(void)
{
	return get_hi_pwr_stat1_wbbp_mtcmos_rdy_stat();
}

#if 0
static __inline__ void set_hi_crg_clken5_wbbp_pd_clk_en(unsigned int v)
{
}
static __inline__ void set_hi_crg_srstdis3_wbbp_pd_srst_dis(unsigned int v)
{
}
#endif

static __inline__ void set_hi_pwr_ctrl5_wbbp_iso_dis(unsigned int v)
{
	set_hi_pwr_ctrl5_wbbp_iso_ctrl_dis(1);
	return ;
}
static __inline__ void set_hi_pwr_ctrl4_wbbp_iso_en(unsigned int v)
{
	set_hi_pwr_ctrl4_wbbp_iso_ctrl_en(1);
	return ;
}
#if 0
static __inline__ void set_hi_crg_srsten3_wbbp_pd_srst_en(unsigned int v)
{
}

static __inline__ void set_hi_crg_clkdis5_wbbp_pd_clk_dis(unsigned int v)
{
}

#endif
static __inline__ void set_hi_pwr_ctrl7_wbbp_mtcmos_dis(unsigned int v)
{
	set_hi_pwr_ctrl7_wbbp_mtcmos_ctrl_dis(1);
	return ;
}

static __inline__ void set_hi_pwr_ctrl6_twbbp_mtcmos_en(unsigned int v)
{
	set_hi_pwr_ctrl6_twbbp_mtcmos_ctrl_en(1);
	return ;
}

static __inline__ unsigned int get_hi_pwr_stat1_twbbp_mtcmos_rdy(void)
{
	return get_hi_pwr_stat1_twbbp_mtcmos_rdy_stat();
}
#if 0
static __inline__ void set_hi_crg_clken5_twbbp_pd_clk_en(unsigned int v)
{
}

static __inline__ void set_hi_crg_srstdis3_twbbp_pd_srst_dis(unsigned int v)
{
}
#endif
static __inline__ void set_hi_pwr_ctrl5_twbbp_iso_dis(unsigned int v)
{
	set_hi_pwr_ctrl5_twbbp_iso_ctrl_dis(1);
	return ;
}

static __inline__ void set_hi_pwr_ctrl4_twbbp_iso_en(unsigned int v)
{
	set_hi_pwr_ctrl4_twbbp_iso_ctrl_en(1);
	return ;
}

#if 0
static __inline__ void set_hi_crg_srsten3_twbbp_pd_srst_en(unsigned int v)
{
}

static __inline__ void set_hi_crg_clkdis5_twbbp_pd_clk_dis(unsigned int v)
{
}
#endif

static __inline__ void set_hi_pwr_ctrl7_twbbp_mtcmos_dis(unsigned int v)
{
	set_hi_pwr_ctrl7_twbbp_mtcmos_ctrl_dis(1);
	return ;
}

#if 0
static __inline__ void set_hi_crg_srsten3_bbpcommon_2a_srst_en(unsigned int v)
{
}
#endif

static __inline__ void set_hi_crg_clkdis5_bbpcommon_clk_dis(unsigned int v)
{
	set_hi_crg_clkdis5_bbpcommon_2a_clk_dis(1);
	return ;
}
#if 0
static __inline__ void set_hi_crg_clken5_bbpcommon_2a_clk_en(unsigned int v)
{
}

static __inline__ void set_hi_crg_clken5_bbp_ao_clk_en(unsigned int v)
{

}

static __inline__ void set_hi_crg_srstdis3_bbpcommon_2a_pd_srst_dis(unsigned int v)
{
}


static __inline__ void set_hi_crg_srsten3_lbbp_pd_srst_en(unsigned int v)
{
}
#endif
static __inline__ void set_hi_pwr_ctrl4_lbbp_iso_en(unsigned int v)
{
	set_hi_pwr_ctrl4_ltebbp_iso_ctrl_en(1);
	return ;
}

static __inline__ void set_hi_crg_clkdis5_ltebbp_pd_clk_dis(unsigned int v)
{
	set_hi_crg_clkdis5_lbbp_pd_clk_dis(1);
	return ;
}

static __inline__ void set_hi_pwr_ctrl7_lbbp_mtcmos_dis(unsigned int v)
{
	set_hi_pwr_ctrl7_ltebbp_mtcmos_ctrl_dis(1);
	return ;
}

/*bbp电源开关状态查询*/

static __inline__ unsigned int get_hi_pwr_stat4_irm_mtcmos_ctrl(void)
{
	return get_hi_pwr_stat4_irmbbp_mtcmos_ctrl_stat();
}

static __inline__ unsigned int get_hi_pwr_stat4_g1bbp_mtcmos_ctrl(void)
{
	return get_hi_pwr_stat4_g1bbp_mtcmos_ctrl_stat();
}

static __inline__ unsigned int get_hi_pwr_stat4_g2bbp_mtcmos_ctrl(void)
{
	return get_hi_pwr_stat4_g2bbp_mtcmos_ctrl_stat();
}

static __inline__ unsigned int get_hi_pwr_stat4_wbbp_mtcmos_ctrl(void)
{
	return get_hi_pwr_stat4_wbbp_mtcmos_ctrl_stat();
}

static __inline__ unsigned int get_hi_pwr_stat4_twbbp_mtcmos_ctrl(void)
{
	return get_hi_pwr_stat4_twbbp_mtcmos_ctrl_stat();
}

static __inline__ unsigned int get_hi_pwr_stat4_lbbp_mtcmos_ctrl(void)
{
	return get_hi_pwr_stat4_ltebbp_mtcmos_ctrl_stat();
}

#if 0
static __inline__ unsigned int set_hi_crg_clken5_twbbp_tbbp_122m_clk_en(unsigned int)
{
	return 0;
}

static __inline__ unsigned int set_hi_crg_clken5_twbbp_tbbp_245m_clk_en(unsigned int)
{
	return 0;
}

static __inline__ unsigned int  set_hi_crg_clkdis5_pdtw_tbbp_122m_clk_dis(unsigned int)
{
	return 0;
}
#endif

static __inline__ void set_hi_crg_clkdis5_pdtw_tbbp_245m_clk_dis(unsigned int arg)
{
	set_hi_crg_clkdis5_twbbp_tbbp_245m_clk_dis(1);
	return ;
}
static __inline__ void	set_hi_crg_clkdis5_pdtw_tbbp_122m_clk_dis(unsigned int arg)
{
	set_hi_crg_clkdis5_twbbp_tbbp_122m_clk_dis(1);
	return ;
}
static __inline__ void set_hi_crg_clkdis5_pdtw_wbbp_61m_clk_dis(unsigned int arg)
{
	set_hi_crg_clkdis5_twbbp_wbbp_61m_clk_dis(1);
	return ;
}
static __inline__ void set_hi_pwr_ctrl6_lbbp_mtcmos_en(unsigned int arg)
{
	set_hi_pwr_ctrl6_ltebbp_mtcmos_ctrl_en(1);
	return ;
}

static __inline__ unsigned int get_hi_pwr_stat1_lbbp_mtcmos_rdy(void)
{
	return get_hi_pwr_stat1_ltebbp_mtcmos_rdy_stat();
}

static __inline__ void set_hi_pwr_ctrl5_lbbp_iso_dis(unsigned int arg)
{
	set_hi_pwr_ctrl5_ltebbp_iso_ctrl_dis(1);
	return ;
}
static __inline__ unsigned int get_hi_crg_clkstat5_ltebbp_pd_clk_stat(void)
{
	return get_hi_crg_clkstat5_lbbp_pd_clk_stat();
}
#if 0
static __inline__ unsigned int  set_hi_crg_clken5_twbbp_wbbp_61m_clk_en(unsigned int)
{
	return 0;
}
#endif

#endif
/****************************p531 end*********************************/

/* for pm */
static __inline__ unsigned int  hi_pwrctrl_get_pwr_ctrl2_abb_ch1_tcxo_en(void){return 0;}
static __inline__ unsigned int  hi_pwrctrl_get_pwr_ctrl2_pmu_ch1_tcxo_en(void){return 0;}

static __inline__ void hi_pwrctrl_set_pwr_ctrl2_abb_ch1_tcxo_en(unsigned int val){};
static __inline__ void hi_pwrctrl_set_pwr_ctrl2_pmu_ch1_tcxo_en(unsigned int val){};

static __inline__ unsigned int get_hi_pwr_stat1(void)
{
#if defined(__KERNEL__)
	return readl((const volatile void *)(HI_SYSCRG_BASE_ADDR+HI_PWR_STAT1_OFFSET));
#else
	return readl((HI_SYSCRG_BASE_ADDR+HI_PWR_STAT1_OFFSET));
#endif
}
/* for pm --end */


#endif

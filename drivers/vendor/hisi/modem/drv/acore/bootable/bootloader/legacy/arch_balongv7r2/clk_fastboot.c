/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 : clk.c
*
*   作    者 :  xujingcui
*
*   描    述 : 本文件用于关闭系统时钟策略中的非用即关IP时钟
*
*   修改记录 :  2013年8月10日  v1.00 xujingcui创建
*************************************************************************/
#include <bsp_memmap.h>
#include <osl_bio.h>
#include <balongv7r2/clk_fastboot.h>
#include <hi_syscrg.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <bsp_nvim.h>


#ifdef CONFIG_V7R2_CLK_CRG
void bsp_pm_init(void)
{
    unsigned int tmp = 0;

    tmp = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_PLLLOCK_CTRL_OFFSET);
    tmp &= ~(0x200000);
    writel(tmp, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_PLLLOCK_CTRL_OFFSET);

    tmp = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DFS4_CTRL3_OFFSET);
    tmp |= 0x8;
    writel(tmp, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DFS4_CTRL3_OFFSET);
}
void bsp_disable_non_on_clk(void)
{
	unsigned int clkcon = 0;

	DRV_NV_PM_CLKINIT_STRU clk_init_nv;
       //if(NV_OK == bsp_nvm_read(NV_ID_DRV_PM_CLKINIT, &clk_init_nv, sizeof(clk_init_nv))){
	   	//writel(clk_init_nv.clkdis1, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);
	   	//writel(clk_init_nv.clkdis2, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);
	   	//writel(clk_init_nv.clkdis3, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_03);
	   //	writel(clk_init_nv.clkdis4, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);
	   	//writel(clk_init_nv.clkdis5, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_05);

		/*disable BBP PLL*/
		//clkcon = readl(HI_SYSCRG_BASE_ADDR_VIRT + 0x22c);
		//clkcon |= ((unsigned int)0x1 << 0);
	    	//writel(clkcon, HI_SYSCRG_BASE_ADDR_VIRT + 0x22c);
      // }else{

	        /*axi monitor cpufast*/
	        writel(0x1 << 3, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*axi monitor soc*/
	        writel(0x1 << 4, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

            /*pmu ssi1*/
            writel(0x1 << 14, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*dw ssi0*/
	        writel(0x1 << 15, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*efuse*/
	        writel(0x1 << 16, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*nandc*/
	        writel(0x1 << 17, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*hs uart*/
	        writel(0x1 << 18, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*emi*/
	       writel(0x1 << 19, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*bootrom*/
	        writel(0x1 << 21, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*pwm1*/
	        writel(0x1 << 22, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*pwm2*/
	        writel(0x1 << 23, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*dt*/
	        writel(0x1 << 24, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*dw ssi1*/
	        writel(0x1 << 26, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*i2c1*/
	        writel(0x1 << 27, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*gpio1*/
	        writel(0x1 << 28, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*i2c0*/
	        writel(0x1 << 29, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*gpio3*/
	        writel(0x1 << 30, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_01);

	        /*uart3*/
	        writel(0x1 << 0, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*gpio2*/
	        writel(0x1 << 2, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*appa9 mbist*/
	        writel(0x1 << 4, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*moda9 mbist*/
	        writel(0x1 << 5, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*usb otg*/
	        writel(0x1 << 13, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*usbotg bc*/
	        writel(0x1 << 14, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*mmc0*/
	        writel(0x1 << 17, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*mmc1*/
	        writel(0x1 << 18, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*mmc0 phase*/
	        writel(0x1 << 19, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*mmc1 phase*/
	        writel(0x1 << 20, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*mmc0 refclk*/
	        writel(0x1 << 21, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*mmc1 refclk*/
	        writel(0x1 << 22, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*usb hsic*/
	        writel(0x1 << 23, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*sdcc*/
	        writel(0x1 << 24, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*hpm*/
	        writel(0x1 << 25, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*pcie ctrl bus and ctrl phy*/
	        writel(0x3 << 27, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*pcie phy*/
	        writel(0x1 << 29, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_02);

	        /*lte sio*/
	        writel(0x1 << 29, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_03);
		
		/*cicom0*/
		//writel(0x1 << 8, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);

		/*cicom1*/
		//writel(0x1 << 9, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);

		/*hdlc*/
		//writel(0x1 << 10, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);
		
		/*upacc*/
		//writel(0x1 << 11, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);

	        /*sci0*/
	        writel(0x1 << 12, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);

	        /*sci1*/
	        writel(0x1 << 13, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);

	        /*uicc*/
	        writel(0x1 << 14, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);

	        /*dsp vic0*/
	        writel(0x1 << 16, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_04);

		/*disable BBP PLL*/
		clkcon = readl(HI_SYSCRG_BASE_ADDR_VIRT + 0x22c);
		 clkcon |= ((unsigned int)0x1 << 0);
	    	writel(clkcon, HI_SYSCRG_BASE_ADDR_VIRT + 0x22c);

		/*HIFI 默认关闭*/

		/*关闭HIFI工作时钟*/
		writel(0x1 << 31, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DISABLE_OFFSET_03);

		/*嵌位不使能*/		
		writel(0x1 << 5, HI_SYSCRG_BASE_ADDR_VIRT + HI_PWRCTR_ISO_CELL_OFFSET_01);

		/*HIFI复位*/		
		writel(0x3 << 19, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_RESETEN_OFFSET_02);

		/*HIFI下电*/		
		writel(0x1 << 5, HI_SYSCRG_BASE_ADDR_VIRT + HI_PWRCTR_MTCOMS_OFFSET);
		
		/*disable HIFI PLL*/
		clkcon = readl(HI_SYSCRG_BASE_ADDR_VIRT + 0x220);
		 clkcon |= ((unsigned int)0x1 << 0);
	    	writel(clkcon, HI_SYSCRG_BASE_ADDR_VIRT + 0x220);			
		
    	        /*luting*/
	        bsp_pm_init();
                return;

}
#else
void bsp_disable_non_on_clk(void)
{
    return;
}
#endif




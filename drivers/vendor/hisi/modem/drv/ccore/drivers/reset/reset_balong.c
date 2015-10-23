
#include <bsp_reset.h>
#include <hi_cipher.h>
#include <hi_ipf.h>
#include <socp_balong.h>
#include "reset_balong.h"

/*lint --e{666} */

u32 print_sw = 1;
#define  reset_print_debug(fmt, ...) \
do {                               \
    if (print_sw)    \
        reset_print_err(fmt, ##__VA_ARGS__);\
} while (0)

#define CCORE_RST_OK                  (0)
#define CCORE_RST_ERR                 (-1)

#define CCORE_RST_TIMEOUT_NUM         (327) /*10ms*/
#define CHECK_TIMEOUT(a)   (get_timer_slice_delta(a, bsp_get_slice_value()) < CCORE_RST_TIMEOUT_NUM)
#define CHECK_TIMEOUT_50MS(a)   (get_timer_slice_delta(a, bsp_get_slice_value()) < (5 *CCORE_RST_TIMEOUT_NUM))
#define CCORE_RST_TIME_PRINT(a)   \
    do{ \
        reset_print_debug("\r wait idle spend time: %d slice\n", (bsp_get_slice_value() - a)); \
    }while(0)

struct ccore_reset_ctrl g_reset_ctrl = {0};

/* 失败信息记录 */
typedef enum
{
    ENUM_RESET_CICOM = 0,
    ENUM_RESET_UPACC_STEP1,
    ENUM_RESET_UPACC_STEP2,
    ENUM_RESET_UPACC_STEP3,
    ENUM_RESET_CIPHER,
    ENUM_RESET_IPF_STEP1,
    ENUM_RESET_IPF_STEP2,
    ENUM_RESET_EDMAC,
    ENUM_RESET_UICC,
    ENUM_RESET_BBP_DMA,
    ENUM_RESET_WBBP,
    ENUM_RESET_NULL = 32
} RESET_FIAL_ENUM;

int BSP_CCPU_Rest_Over(void)
{
	reset_print_debug("boot mode=0x%x\n", readl(SCBAKDATA13));
	if(!bsp_reset_ccore_is_reboot())
	{
		reset_print_debug("normal\n");
		return 0;
	}
	else
	{
		bsp_ipc_int_send(IPC_CORE_ACORE, g_reset_ctrl.ipc_send_irq_reboot_ok);
		reset_print_debug("reboot\n");
	}

	return 0;

}

/*****************************************************************************
 函 数 名  : ccpu_reset_cicom_and_wait_idle
 功能描述  : c核复位后关闭并等待cicom进入空闲态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : ccpu_reset_cicom_and_wait_idle
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
int ccpu_reset_cicom_and_wait_idle(void)
{
    unsigned int regvalue = 0;	
    unsigned int regvalue1 = 0;
    unsigned int u32slicebegin = 0;
    *(u32 *)STAMP_RESET_CICOM0_SOFT_RESET = bsp_get_slice_value();

	u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_CICOM0_REGBASE_ADDR_VIRT + 0x28);
        regvalue1 = readl(HI_CICOM0_REGBASE_ADDR_VIRT + 0x128);
    }while((regvalue || regvalue1)&& CHECK_TIMEOUT(u32slicebegin));
    if((0x0 != regvalue)||(0x0 != regvalue1)){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_CICOM;		
        return CCORE_RST_ERR;
    }
	
	u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_CICOM1_REGBASE_ADDR_VIRT + 0x28);
        regvalue1 = readl(HI_CICOM1_REGBASE_ADDR_VIRT + 0x128);
    }while((regvalue || regvalue1)&& CHECK_TIMEOUT(u32slicebegin));
    if((0x0 != regvalue)||(0x0 != regvalue1)){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_CICOM;
        return CCORE_RST_ERR;
    }
	
    /*复位cicom0*/
    regvalue = readl(HI_CICOM0_REGBASE_ADDR_VIRT + 0x0);
    regvalue |= (unsigned int)0x1;
    writel(regvalue, HI_CICOM0_REGBASE_ADDR_VIRT + 0x0);

    *(u32 *)STAMP_RESET_CICOM1_SOFT_RESET = bsp_get_slice_value();

    /*复位cicom1*/
    regvalue = readl(HI_CICOM1_REGBASE_ADDR_VIRT + 0x0);
    regvalue |= (unsigned int)0x1;
    writel(regvalue, HI_CICOM1_REGBASE_ADDR_VIRT + 0x0);
    return CCORE_RST_OK;
}

/*****************************************************************************
 函 数 名  : ccpu_reset_upacc_and_wait_idle
 功能描述  : c核复位后关闭并等待upacc进入空闲态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : ccpu_reset_upacc_and_wait_idle
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
int ccpu_reset_upacc_and_wait_idle(void)
{
    unsigned int regvalue = 0;
    unsigned int u32slicebegin = 0;
    *(u32 *)STAMP_RESET_UPACC_ENTER_IDLE_1 = bsp_get_slice_value();

    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_UPACC_BASE_ADDR_VIRT + 0x1000);
        regvalue &= (unsigned int)0x3;
    }while((0x0 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x0 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_UPACC_STEP1;
        return CCORE_RST_ERR;
    }

    *(u32 *)STAMP_RESET_UPACC_ENTER_IDLE_2 = bsp_get_slice_value();
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_UPACC_BASE_ADDR_VIRT + 0x1100);
        regvalue &= (unsigned int)0x3;
    }while((0x0 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x0 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_UPACC_STEP2;
        return CCORE_RST_ERR;
    }

    *(u32 *)STAMP_RESET_UPACC_ENTER_IDLE_3 = bsp_get_slice_value();
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_UPACC_BASE_ADDR_VIRT + 0x1200);
        regvalue &= (unsigned int)0x3;
    }while((0x0 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x0 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_UPACC_STEP3;
        return CCORE_RST_ERR;
    }

	/*disable UPACC*/
	writel(0x0,HI_UPACC_BASE_ADDR_VIRT + 0xD8);

    return CCORE_RST_OK;
}

/*****************************************************************************
 函 数 名  : ccpu_reset_cipher_and_wait_idle
 功能描述  : c核复位后关闭并等待cipher进入空闲态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : ccpu_reset_cipher_and_wait_idle
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
int ccpu_reset_cipher_and_wait_idle(void)
{
    unsigned int regvalue = 0;
    unsigned int u32slicebegin = 0;

    /*查询BBP PLL的状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_BBPPLL_CFG0_OFFSET);
    regvalue &= (unsigned int)0x1;
    if(regvalue != 0x1){
        return CCORE_RST_OK;
    }

    /*查询cipher clk的状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKSTAT4_OFFSET);
    regvalue &= (unsigned int)0x1 << 17;
    if(regvalue != ((unsigned int)0x1 << 17)){
        return CCORE_RST_OK;
    }
    *(u32 *)STAMP_RESET_CIPHER_DISABLE_CHANNLE = bsp_get_slice_value();
	/*disable all channels*/
	writel(0x40000000, HI_CIPHER_BASE_ADDR_VIRT + HI_CH0_EN_OFFSET);//chn-0
	writel(0x40000000, HI_CIPHER_BASE_ADDR_VIRT + HI_CH1_EN_OFFSET);//chn-1
	writel(0x40000000, HI_CIPHER_BASE_ADDR_VIRT + HI_CH2_EN_OFFSET);//chn-2
	writel(0x40000000, HI_CIPHER_BASE_ADDR_VIRT + HI_CH3_EN_OFFSET);//chn-3
	writel(0x40000000, HI_CIPHER_BASE_ADDR_VIRT + HI_CH4_EN_OFFSET);//chn-4
	
    *(u32 *)STAMP_RESET_CIPHER_ENTER_IDLE = bsp_get_slice_value();
    u32slicebegin = bsp_get_slice_value();
	//wait for cipher being idle
    do{
        regvalue = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CIPHER_CTRL_OFFSET);
        regvalue &= (0x1U << 31);
    }while(regvalue && CHECK_TIMEOUT(u32slicebegin));
    if(regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= 0x1U << ENUM_RESET_CIPHER;
        return CCORE_RST_ERR;
    }

	/* reset whole cipher ip*/
    *(u32 *)STAMP_RESET_CIPHER_SOFT_RESET = bsp_get_slice_value();
    regvalue = readl(HI_CIPHER_BASE_ADDR_VIRT + HI_CIPHER_CTRL_OFFSET);
    regvalue |= 0x1U;
    writel(regvalue, HI_CIPHER_BASE_ADDR_VIRT + HI_CIPHER_CTRL_OFFSET);

    return CCORE_RST_OK;
}

/*****************************************************************************
 函 数 名  : ccpu_reset_ipf_and_wait_idle
 功能描述  : c核复位后关闭并等待ipf进入空闲态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : ccpu_reset_ipf_and_wait_idle
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
int ccpu_reset_ipf_and_wait_idle(void)
{
    unsigned int regvalue = 0;
    unsigned int u32slicebegin = 0;

    /*查询ipf clk的状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKSTAT4_OFFSET);
    regvalue &= (unsigned int)0x1 << 10;
    if(regvalue != ((unsigned int)0x1 << 10)){
        return CCORE_RST_OK;
    }
    *((u32 *)STAMP_RESET_IPF_SOFT_RESET) = bsp_get_slice_value();

    /*查询IPF是否空闲*/
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_SC_STAT26_OFFSET);
        regvalue &= (unsigned int)0x2;
    }while((0x2 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x2 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_IPF_STEP2;
        return CCORE_RST_ERR;
    }

    /*复位IPF*/
    regvalue = readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_SRST_OFFSET);
    regvalue |= (unsigned int)0x1;
    writel(regvalue, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_SRST_OFFSET);

    *((u32 *)STAMP_RESET_IPF_ENTER_IDLE) = bsp_get_slice_value();

    /*判断复位是否成功*/
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_SRST_STATE_OFFSET);
        regvalue &= (unsigned int)0x1;
    }while((0x1 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x1 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_IPF_STEP1;
        return CCORE_RST_ERR;
    }

    /*查询IPF是否空闲*/
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_SC_STAT26_OFFSET);
        regvalue &= (unsigned int)0x2;
    }while((0x2 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x2 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_IPF_STEP2;
        return CCORE_RST_ERR;
    }
    return CCORE_RST_OK;
}

/*****************************************************************************
 函 数 名  : ccpu_reset_edmac_and_wait_idle
 功能描述  : c核复位后关闭并等待edmac进入空闲态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : ccpu_reset_edmac_and_wait_idle
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
int ccpu_reset_edmac_and_wait_idle(void)
{
    unsigned int regvalue = 0;
    unsigned int u32slicebegin = 0;

    *(u32 *)STAMP_RESET_EDMA_STOP_BUS = bsp_get_slice_value();

    /* 请求停止总线访问,相当于halt en */
    regvalue = readl(HI_EDMA_REGBASE_ADDR_VIRT + 0x698);
    regvalue |= (unsigned int)0x2;
    writel(regvalue, HI_EDMA_REGBASE_ADDR_VIRT + 0x698);

    *(u32 *)STAMP_RESET_EDMA_ENTER_IDLE = bsp_get_slice_value();

    /*查询是否停止EDMA总线访问，查是否进halt状态 */
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_EDMA_REGBASE_ADDR_VIRT + 0x698);
        regvalue &= (unsigned int)0x1;
    }while((0x1 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x1 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_EDMAC;
        return CCORE_RST_ERR;
    }
    return CCORE_RST_OK;
}

/*****************************************************************************
 函 数 名  : ccpu_reset_uicc_and_wait_idle
 功能描述  : c核复位后关闭并等待uicc进入空闲态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : ccpu_reset_uicc_and_wait_idle
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
int ccpu_reset_uicc_and_wait_idle(void)
{
    unsigned int regvalue = 0;
    unsigned int u32slicebegin = 0;

    /*复位uicc*/
    regvalue = readl(HI_UICC_REGBASE_ADDR_VIRT + 0x10);
    regvalue |= (unsigned int)0x1;
    writel(regvalue, HI_UICC_REGBASE_ADDR_VIRT + 0x10);

    /*查询uicc复位完成*/
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_UICC_REGBASE_ADDR_VIRT + 0x10);
        regvalue &= (unsigned int)0x1;
    }while((0x0 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x0 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_UICC;
        return CCORE_RST_ERR;
    }
    return CCORE_RST_OK;
}

/*****************************************************************************
 函 数 名  : ccpu_reset_bbe16_and_wait_idle
 功能描述  : c核复位后关闭并等bbe16进入空闲态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : ccpu_reset_bbe16_and_wait_idle
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
int ccpu_halt_bbe16_and_wait_idle(void)
{
    unsigned int regvalue = 0;
    unsigned int u32slicebegin = 0;

    /*halt*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_SC_CTRL13_OFFSET);

    if (!(regvalue & (0x01<<17)))
    {
        (void)bsp_ipc_int_send(IPC_CORE_LDSP, IPC_DSP_INT_SRC_CCPU_RESET);

	    /*wait for holding*/
	    u32slicebegin = bsp_get_slice_value();
	    do{
	    }while(CHECK_TIMEOUT(u32slicebegin));
        regvalue |= ((unsigned int)0x1 << 17);

        writel(regvalue, HI_SYSCRG_BASE_ADDR_VIRT + HI_SC_CTRL13_OFFSET);

	    u32slicebegin = bsp_get_slice_value();
	    do{
	    }while(CHECK_TIMEOUT(u32slicebegin));
    }

	/* stop socp channel to prevent data transferring when bbe16 powered down */
	regvalue = readl(SOCP_REG_BASEADDR + SOCP_REG_ENCSRC_BUFCFG1(SOCP_CODER_SRC_LDSP1));
	writel((regvalue & 0xfffffffe), (SOCP_REG_BASEADDR + SOCP_REG_ENCSRC_BUFCFG1(SOCP_CODER_SRC_LDSP1)));
	while(readl(SOCP_REG_BASEADDR + SOCP_REG_ENCSTAT) & (1 << (SOCP_CODER_SRC_LDSP1)));

    return CCORE_RST_OK;
}
void ccpu_reset_bbe16_sysbus(void)
{
    /*reset bbe16 and sysbus*/
    writel(0x7, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_SRSTEN1_OFFSET);
}

/*****************************************************************************
 函 数 名  : ccpu_reset_bbp_and_wait_idle
 功能描述  : c核复位后关闭并等bbe16进入空闲态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
 调用函数  : ccpu_reset_bbp_and_wait_idle
 被调函数  :

 修改历史      :
  1.日    期   : 2014年3月12日
    修改内容   : 新生成函数

*****************************************************************************/
int ccpu_reset_bbp_and_wait_idle(void)
{
    unsigned int regvalue = 0;
    unsigned int u32slicebegin = 0;
	u32 i = 0;

    /*查询2A的clk状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKSTAT5_OFFSET);
    regvalue &= (unsigned int)0x2;
    if(regvalue != 0x2){
        goto wbbp_idle;
    }
    /*查询2A的reset状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT3_OFFSET);
    regvalue &= (unsigned int)0x2;
    if(regvalue != 0x0){
        goto wbbp_idle;
    }
    /*查询LBBP/TWBBP clk状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKSTAT5_OFFSET);
    regvalue &= (unsigned int)0x24;
    if(regvalue == 0x0){
        goto wbbp_idle;
    }

    /*DSP状态查询*/
    /*0x2c[0]==0x1 clk*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKSTAT4_OFFSET);
    regvalue &= (unsigned int)0x1;
    if(regvalue != 0x1){
        goto wbbp_idle;
    }

    /*0x68[0]==0x0 reset*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT1_OFFSET);
    regvalue &= (unsigned int)0x1;
    if(regvalue != 0x0){
        goto wbbp_idle;
    }

    /*0xe04[4]==0x1 mtcmos*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_PWR_STAT1_OFFSET);
    regvalue &= (unsigned int)0x1 << 4;
    if(regvalue != ((unsigned int)0x1 << 4)){
        goto wbbp_idle;
    }

    /*0xe0c[4]==0x0 iso*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_PWR_STAT3_OFFSET);
    regvalue &= (unsigned int)0x1 << 4;
    if(regvalue != 0x0){
        goto wbbp_idle;
    }

    /*0x20c[0]==0x1 pll*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_DSPPLL_CFG0_OFFSET);
    regvalue &= (unsigned int)0x1;
    if(regvalue != 0x1){
        goto wbbp_idle;
    }
    *(u32 *)STAMP_RESET_BBP_DMA_ENTER_IDLE = bsp_get_slice_value();

	writel(0x0, HI_BBP_DMA_BASE_ADDR_VIRT + 0x0294);
	writel(0x0, HI_BBP_DMA_BASE_ADDR_VIRT + 0x02A8);
	writel(0x0, HI_BBP_DMA_BASE_ADDR_VIRT + 0x02B4);
	writel(0x0, HI_BBP_DMA_BASE_ADDR_VIRT + 0x02B8);
	for (i = 0; i < 96; i++)
	{
		writel(0x0, HI_BBP_DMA_BASE_ADDR_VIRT + 0x0308 + 0x10 *i);
	}
	for (i = 0; i < 32; i++)
	{
		regvalue = readl(HI_BBP_DMA_BASE_ADDR_VIRT + 0x0A0C + 0x10 *i);
		regvalue &= ~(1 << 16);
		writel(regvalue, HI_BBP_DMA_BASE_ADDR_VIRT + 0x0A0C + 0x10 *i);
	}
	u32slicebegin = bsp_get_slice_value();
	do{
	}while(CHECK_TIMEOUT_50MS(u32slicebegin));

	writel(0x7, HI_BBP_DMA_BASE_ADDR_VIRT);
	u32slicebegin = bsp_get_slice_value();
	do{
	}while(CHECK_TIMEOUT_50MS(u32slicebegin));

    /*查询BBP DMA 8/9/10bit 3类通道(包含所有)是否空闲*/
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_BBP_DMA_BASE_ADDR_VIRT + 0x10);
        regvalue &= (unsigned int)0x700;
    }while((0x0 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x0 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_BBP_DMA;
        return CCORE_RST_ERR;
    }

    /*reset*/
    writel(0x2, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_SRSTEN3_OFFSET);

wbbp_idle:

    /*查询BBP PLL的状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_BBPPLL_CFG0_OFFSET);
    regvalue &= (unsigned int)0x1;
    if(regvalue != 0x1){
        return CCORE_RST_OK;
    }

    /*查询WBBP的clk状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKSTAT5_OFFSET);
    regvalue &= (unsigned int)0x40;
    if(regvalue != 0x40){
        return CCORE_RST_OK;
    }

    /*查询WBBP的reset状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_SRSTSTAT3_OFFSET);
    regvalue &= (unsigned int)0x40;
    if(regvalue != 0x0){
        return CCORE_RST_OK;
    }

    /*查询WBBP的ISO状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_PWR_STAT3_OFFSET);
    regvalue &= (unsigned int)0x100;
    if(regvalue != 0x0){
        return CCORE_RST_OK;
    }

    /*查询WBBP的MTCOMS状态*/
    regvalue = readl(HI_SYSCRG_BASE_ADDR_VIRT + HI_PWR_STAT1_OFFSET);
    regvalue &= (unsigned int)0x100;
    if(regvalue != 0x100){
        return CCORE_RST_OK;
    }
    /*配置WBBP*/

    *(u32 *)STAMP_RESET_WBBP_MSTER_STOP = bsp_get_slice_value();
    writel(0x0, HI_WBBP_REG_BASE_ADDR_VIRT + 0x9070);

    *(u32 *)STAMP_RESET_WBBP_SLAVE_STOP = bsp_get_slice_value();
    writel(0x0, HI_WBBP_REG_BASE_ADDR_VIRT + 0x29070);

    *(u32 *)STAMP_RESET_WBBP_ENTER_IDLE = bsp_get_slice_value();

    /*查询WBBP 0/16/17/18/19是否空闲*/
    u32slicebegin = bsp_get_slice_value();
    do{
        regvalue = readl(HI_WBBP_REG_BASE_ADDR_VIRT + 0x949c);
        regvalue &= (unsigned int)0xF0001;
    }while((0x0 != regvalue) && CHECK_TIMEOUT(u32slicebegin));
    if(0x0 != regvalue){
        *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT |= (u32)0x1 << ENUM_RESET_WBBP;
        return CCORE_RST_ERR;
    }
    return CCORE_RST_OK;
}

void bsp_modem_master_enter_idle(void)
{
	/*lint --e{438} */
    unsigned long flags = 0; 
    UNUSED(flags);
    local_irq_save(flags);
    *(u32 *)STAMP_RESET_MASTER_ENTER_IDLE = bsp_get_slice_value();

    (void)ccpu_halt_bbe16_and_wait_idle();
    /*close, and then wait for being idle till timeout*/
    (void)ccpu_reset_cipher_and_wait_idle();
    (void)ccpu_reset_edmac_and_wait_idle();
    (void)ccpu_reset_upacc_and_wait_idle();
    (void)ccpu_reset_cicom_and_wait_idle();
    #if 0/*UICC no use*/
    (void)ccpu_reset_uicc_and_wait_idle();
    #endif
    (void)ccpu_reset_ipf_and_wait_idle();
    (void)ccpu_reset_bbp_and_wait_idle();
    ccpu_reset_bbe16_sysbus();

    *(u32 *)STAMP_RESET_MASTER_IDLE_QUIT = bsp_get_slice_value();
    return;
}
static void reset_stamp_addr_value(void)
{
    *(u32 *)STAMP_RESET_BASE_ADDR = 0xcccccccc;
    *(u32 *)STAMP_RESET_IDLE_FAIL_COUNT = 0x0;
    *(u32 *)STAMP_RESET_MASTER_ENTER_IDLE = 0x0;
    *(u32 *)STAMP_RESET_CIPHER_SOFT_RESET = 0x0;
    *(u32 *)STAMP_RESET_CIPHER_ENTER_IDLE = 0x0;
    *(u32 *)STAMP_RESET_EDMA_STOP_BUS = 0x0;
    *(u32 *)STAMP_RESET_EDMA_ENTER_IDLE = 0x0;
    *(u32 *)STAMP_RESET_UPACC_ENTER_IDLE_1 = 0x0;
    *(u32 *)STAMP_RESET_UPACC_ENTER_IDLE_2 = 0x0;
    *(u32 *)STAMP_RESET_UPACC_ENTER_IDLE_3 = 0x0;
    *(u32 *)STAMP_RESET_CICOM0_SOFT_RESET = 0x0;
    *(u32 *)STAMP_RESET_CICOM1_SOFT_RESET = 0x0;
    *(u32 *)STAMP_RESET_IPF_SOFT_RESET = 0x0;
    *(u32 *)STAMP_RESET_IPF_ENTER_IDLE = 0x0;
    *(u32 *)STAMP_RESET_BBP_DMA_ENTER_IDLE = 0x0;
    *(u32 *)STAMP_RESET_WBBP_MSTER_STOP = 0x0;
    *(u32 *)STAMP_RESET_WBBP_SLAVE_STOP = 0x0;
    *(u32 *)STAMP_RESET_WBBP_ENTER_IDLE = 0x0;
    *(u32 *)STAMP_RESET_MASTER_IDLE_QUIT = 0x0;
}
void FIQ_IntHandle(void)
{
    reset_stamp_addr_value();
	bsp_modem_master_enter_idle();
	bsp_ipc_int_send(IPC_CORE_ACORE, g_reset_ctrl.ipc_send_irq_in_idle);
    g_reset_ctrl.fiq_cnt++;
    *(u32 *)(STAMP_RESET_FIQ_COUNT) = g_reset_ctrl.fiq_cnt;
	return;
}

extern void pm_wake_lock(void);
void reset_ipc_isr(u32 data)
{
	pm_wake_lock();
	//asm volatile ( "wfi;");
}

s32 bsp_reset_init(void)
{
    memset(&(g_reset_ctrl), 0, sizeof(g_reset_ctrl));

	g_reset_ctrl.ipc_send_irq_in_idle = IPC_ACPU_INT_SRC_CCPU_RESET_IDLE;
	g_reset_ctrl.ipc_send_irq_reboot_ok = IPC_ACPU_INT_SRC_CCPU_RESET_SUCC;
	g_reset_ctrl.ipc_recv_irq_wakeup = IPC_CCPU_INT_SRC_ACPU_RESET;

    g_reset_ctrl.stamp_ccpu_addr = STAMP_RESET_BASE_ADDR;
    g_reset_ctrl.stamp_m3_reset_addr = STAMP_RESET_M3_BASE_ADDR;
    g_reset_ctrl.stamp_m3_unreset_addr = STAMP_UNRESET_M3_BASE_ADDR;
	if (bsp_ipc_int_connect(g_reset_ctrl.ipc_recv_irq_wakeup, (voidfuncptr)reset_ipc_isr, 0))
	{
		goto ipc_err;
	}
	if (bsp_ipc_int_enable(g_reset_ctrl.ipc_recv_irq_wakeup))
	{
		goto ipc_err;
	}
	if (!bsp_reset_ccore_is_reboot())
	{
		g_reset_ctrl.fiq_cnt = 0;
        *(u32 *)(STAMP_RESET_FIQ_COUNT) = 0;
        *(u32 *)(STAMP_RESET_IDLE_FAIL_COUNT) = 0;
	}
    /*此标记用于记录失败*/
    if(*(u32 *)(STAMP_RESET_IDLE_FAIL_COUNT)){
        reset_print_err("master enter idle fail cnt: 0x%x\n", *(u32 *)(STAMP_RESET_IDLE_FAIL_COUNT));
    }
	g_reset_ctrl.fiq_cnt = *(u32 *)(STAMP_RESET_FIQ_COUNT);
	reset_print_err("master enter fiq cnt: %d\n", g_reset_ctrl.fiq_cnt);

	g_reset_ctrl.state = 1;
	reset_print_err("ok\n");
	return 0;

ipc_err:
	reset_print_err("ipc error\n");
	return 0;
	
}



#include <bsp_version.h>
#include <bsp_clk.h>
#include <bsp_abb.h>
#include "clock.h"

static struct clk tcxo = {
    .name = "tcxo",
    .rate = TCXO,
};
  /*lint -save -e31*/
static struct clk lte_sio;
/*
**define clock reg message
*
*clk_reg(_enable_flag, _enable_reg, _enable_bit, _gate_reg, _gate_bit, _srst_reg, _srst_bit, _sel_reg, _sel_bit, _sel, _div_reg, _div_bit, _div)
*
*this clk_id=0,for the clk  not crg register
*p531 and v7r2 must be added at the same time
*/
#ifdef CONFIG_P531_CLK_CRG /*CONFIG_P531_CLK_CRG*/
struct crg_tag reg_message[] = {
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},              /*CLK_NO_REG*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MST_ACLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DSP_ACLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SOC_HCLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AMON_CPUFAST_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AMON_SOC_CLK*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TSENSOR_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*IPF_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                   /*EDMAC_CHE4_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SOCP_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MIPI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PMUSSI0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PMUSSI1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},             /*DW_SSI0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EFUSE_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*NANDC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*HS_UART_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EMI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EDMAC_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*BOOTROM_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*PWM1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*PWM0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DT_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*IPCM_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DW_SSI1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*I2C1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*CPIO1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*I2C0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO3_CLK_ID*/

        {clk_reg(HAS_ENABLE_REG, 0xc, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*UART3_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*UART2_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO2_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*CS_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*APPA9_MBIST_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MONA9_MBIST_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*CPU_ACLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MDDRC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DDRPHY_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*DDRPHY_DDR_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AXI_MEN_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*FUNC_WAKEUP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*SOC_PERI_USB_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USBOTG_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USB_BC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},              /*MMC0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},              /*MMC1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*MMC0_PHASE_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                       /*MMC1_PHASE_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                       /*MMC0_REFCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                       /*MMC1_REFCLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USBHSIC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SDCC_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                       /*HPM_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x9c, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PCIE_CTRL1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                       /*PCIE_CTRL2_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                       /*PCIE_PHY_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WDOG_CLK_ID*/

        {clk_reg(HAS_ENABLE_REG, 0x18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*UART0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM2_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM3_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM4_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM5_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM6_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM7_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM8_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM9_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM10_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM11_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM12_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM13_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM14_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM15_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM16_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM17_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM18_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM19_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM20_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM21_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM22_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM23_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                  /*APPA9_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*MOMA9_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 4, 0, 0, 0, 0, 0x34, 19, 0x1, 0, 0, 0)},        /*LET_SIO_CLK*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                       /*HIFI_DBG_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*HIFI_CLK_ID*/

        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*DSP0_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                    /*DSP0_CORE_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*DSP0_DBG_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*GUACC_AXI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*CICOM0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*CICOM1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*HDLC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*UPACC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*SCI0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*SCI1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*UICC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*UART1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*DSP_VIC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*CIPEHER_CLK_ID*/

        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*BBP_AO_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*BBPCOMMON_2A_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LTEBBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*G1BBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*G2BBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_AXI_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_PTD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_PDF_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_TDL_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_VDL_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_TDS_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_TURBO_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_BBP_245M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_WBBP_122M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_WBBP_122M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_WBBP_61M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_TBBP_122M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_TBBP_245M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_61M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_122M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_245M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_AXI_CLK_ID*/

        {clk_reg(HAS_ENABLE_REG, 0xc, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},              /*MMC2_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x4c, 0, 0x3F)},           /*MMC0_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x4c, 8, 0x3F)},           /*MMC1_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x4c, 16, 0x3F)},          /*MMC2_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x44, 0, 0xFFFF)},         /*SIO_BCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x44, 16, 0xFFF)},         /*SIO_SYNCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0x30, 6, 0x1, 0, 0, 0)},             /*SIO_OUTER_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x64, 0, 0x64, 31, 0, 0, 0, 0, 0, 0, 0, 0)},        /*BBE16_DFS_PD_ID,其中gate寄存器代表等稳定寄存器*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*BBP_DFS_PD_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*ABB_CH0_LWT_PLL_ID,*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*ABB_CH0_G_PLL_ID,*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*ABB_CH1_LWT_PLL_ID,*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                      /*ABB_CH1_G_PLL_ID,*/
};
#elif defined(CONFIG_V7R2_CLK_CRG) /* CONFIG_V7R2_CLK_CRG */

struct crg_tag reg_message[] = {
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0x0, 0, 0, 0x0)},               /*CLK_NO_REG*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MST_ACLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DSP_ACLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SOC_HCLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AMON_CPUFAST_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AMON_SOC_CLK*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TSENSOR_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*IPF_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EDMAC_CHE4_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SOCP_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MIPI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PMUSSI0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PMUSSI1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},              /*DW_SSI0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EFUSE_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*NANDC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*HS_UART_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EMI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EDMAC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*BOOTROM_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PWM1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PWM0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DT_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*IPCM_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DW_SSI1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*I2C1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*CPIO1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*I2C0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO3_CLK_ID*/

        {clk_reg(HAS_ENABLE_REG, 0xc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*UART3_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*UART2_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO2_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*CS_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*APPA9_MBIST_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MONA9_MBIST_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*CPU_ACLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MDDRC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DDRPHY_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DDRPHY_DDR_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AXI_MEN_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*FUNC_WAKEUP_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SOC_PERI_USB_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USBOTG_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USB_BC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 19, 0, 0, 0, 0, 0, 0, 0, 0x108, 8, 0x7)},         /*MMC0_PHASE_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 20, 0, 0, 0, 0, 0, 0, 0, 0x108, 20, 0x7)},        /*MMC1_PHASE_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC0_REFCLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC1_REFCLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USBHSIC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SDCC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*HPM_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PCIE_CTRL1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PCIE_CTRL2_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PCIE_PHY_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0xc, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*WDOG_CLK_ID*/

        {clk_reg(HAS_ENABLE_REG, 0x18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*UART0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM2_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM3_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM4_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM5_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM6_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM7_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM8_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM9_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM10_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM11_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM12_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM13_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM14_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM15_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM16_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM17_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM18_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM19_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM20_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM21_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM22_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM23_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*APPA9_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MOMA9_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LET_SIO_CLK*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*HIFI_DBG_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x18, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*HIFI_CLK_ID*/

        {clk_reg(HAS_ENABLE_REG, 0x24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*DSP0_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*DSP0_CORE_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*DSP0_DBG_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*GUACC_AXI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*CICOM0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*CICOM1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*HDLC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*UPACC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 12, 0, 0, 0, 0, 0, 0, 0, 0x110, 8, 0x7)},          /*SCI0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 13, 0, 0, 0, 0, 0, 0, 0, 0x110, 12, 0x7)},         /*SCI1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*UICC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*UART1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*DSP_VIC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 17, 0, 0, 0, 0, 0, 0, 0, 0x108, 28, 0xF)},         /*CIPEHER_CLK_ID*/

        {clk_reg(HAS_ENABLE_REG, 0x30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*BBP_AO_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*BBPCOMMON_2A_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LTEBBP_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*G1BBP_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*G2BBP_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_PD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_AXI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_PTD_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_PDF_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_TDL_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_VDL_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_TDS_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_TURBO_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_BBP_245M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_WBBP_122M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_WBBP_122M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_WBBP_61M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_TBBP_122M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_TBBP_245M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_61M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_122M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_245M_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x30, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_AXI_CLK_ID*/

        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*MMC2_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x108, 0, 0x1F)},              /*MMC0_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x108, 12, 0x1F)},             /*MMC1_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                     /*MMC2_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x100, 0, 0xFFFF)},            /*SIO_BCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x100, 16, 0xFFF)},            /*SIO_SYNCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0x140, 6, 0x1, 0, 0, 0)},               /*SIO_OUTER_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x214, 0, 0x214, 31, 0, 0, 0, 0, 0, 0, 0, 0)},           /*BBE16_DFS_PD_ID,      其中gate寄存器代表等稳定寄存器*/
        {clk_reg(HAS_ENABLE_REG, 0x22c, 0, 0x22c, 31, 0, 0, 0, 0, 0, 0, 0, 0)},           /*BBP_DFS_PD_ID，     其中gate寄存器代表等稳定寄存器*/
        {clk_reg(HAS_ENABLE_REG, 0xD0, 3, 0x7B, 7, 0, 0, 0, 0, 0, 0, 0, 0)},              /*ABB_CH0_LWT_PLL_ID, 其中gate寄存器代表等稳定寄存器*/
        {clk_reg(HAS_ENABLE_REG, 0xD0, 2, 0x7B, 6, 0, 0, 0, 0, 0, 0, 0, 0)},              /*ABB_CH0_G_PLL_ID,     其中gate寄存器代表等稳定寄存器*/
        {clk_reg(HAS_ENABLE_REG, 0x70, 3, 0xDB, 7, 0, 0, 0, 0, 0, 0, 0, 0)},              /*ABB_CH1_LWT_PLL_ID, 其中gate寄存器代表等稳定寄存器*/
        {clk_reg(HAS_ENABLE_REG, 0x70, 2, 0xDB, 6, 0, 0, 0, 0, 0, 0, 0, 0)},              /*ABB_CH1_G_PLL_ID,      其中gate寄存器代表等稳定寄存器*/
};
#elif defined(CONFIG_K3V3_CLK_CRG) /* CONFIG_K3V3_CLK_CRG */
struct crg_tag reg_message[] = {
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0x0, 0, 0, 0x0)},           /*CLK_NO_REG*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MST_ACLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DSP_ACLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SOC_HCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AMON_CPUFAST_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AMON_SOC_CLK*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TSENSOR_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*IPF_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EDMAC_CHE4_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SOCP_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MIPI_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PMUSSI0_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PMUSSI1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DW_SSI0_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EFUSE_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*NANDC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*HS_UART_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*EMI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*EDMAC_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*BOOTROM_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PWM1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PWM0_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DT_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*IPCM_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DW_SSI1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*I2C1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*CPIO1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*I2C0_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO3_CLK_ID*/

        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*UART3_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*UART2_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO2_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*CS_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*APPA9_MBIST_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MONA9_MBIST_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*CPU_ACLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MDDRC_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DDRPHY_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*DDRPHY_DDR_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*AXI_MEN_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*FUNC_WAKEUP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SOC_PERI_USB_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USBOTG_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USB_BC_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC0_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC0_PHASE_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC1_PHASE_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC0_REFCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MMC1_REFCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*USBHSIC_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*SDCC_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*HPM_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PCIE_CTRL1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PCIE_CTRL2_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*PCIE_PHY_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*WDOG_CLK_ID*/

        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*GPIO0_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*UART0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*TM0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*TM1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*TM2_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*TM3_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*TM4_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*TM5_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*TM6_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},          /*TM7_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM8_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM9_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM10_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM11_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM12_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM13_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM14_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM15_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM16_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM17_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM18_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM19_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM20_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM21_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM22_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*TM23_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*APPA9_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},               /*MOMA9_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LET_SIO_CLK*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*HIFI_DBG_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*HIFI_CLK_ID*/

        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*DSP0_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*DSP0_CORE_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*DSP0_DBG_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*GUACC_AXI_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},            /*CICOM0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},            /*CICOM1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*HDLC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},           /*UPACC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 12, 0, 0, 0, 0, 0, 0, 0, 0x110, 8, 0x7)},     /*SCI0_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 13, 0, 0, 0, 0, 0, 0, 0, 0x110, 12, 0x7)},    /*SCI1_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},           /*UICC_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*UART1_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*DSP_VIC_CLK_ID*/
        {clk_reg(HAS_ENABLE_REG, 0x24, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},            /*CIPEHER_CLK_ID*/

        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*BBP_AO_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*BBPCOMMON_2A_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LTEBBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*G1BBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*G2BBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_PD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_AXI_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_PTD_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_PDF_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_TDL_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_VDL_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*LBBP_TDS_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_TURBO_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_BBP_245M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*IRM_WBBP_122M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_WBBP_122M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_WBBP_61M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_TBBP_122M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*TWBBP_TBBP_245M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_61M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_122M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_245M_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                /*WBBP_AXI_CLK_ID*/

        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*MMC2_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*MMC0_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*MMC1_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*MMC2_CCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*SIO_BCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*SIO_SYNCLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},                 /*SIO_OUTER_CLK_ID*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},       /*BBE16_DFS_PD_ID,      其中gate寄存器代表等稳定寄存器*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},       /*BBP_DFS_PD_ID，     其中gate寄存器代表等稳定寄存器*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},           /*ABB_CH0_LWT_PLL_ID, 其中gate寄存器代表等稳定寄存器*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},           /*ABB_CH0_G_PLL_ID,     其中gate寄存器代表等稳定寄存器*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},           /*ABB_CH1_LWT_PLL_ID, 其中gate寄存器代表等稳定寄存器*/
        {clk_reg(NO_ENABLE_REG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},           /*ABB_CH1_G_PLL_ID,      其中gate寄存器代表等稳定寄存器*/
};
#endif

static struct clk_ops abb_pll_ops = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
};

static struct clk_ops dfs_mode_ops = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
};

 static struct clk bbe16_pll = {
    "bbe16_pll",
     0,
     &tcxo,
     0,
     &dfs_mode_ops,
     NULL,
     BBE16_DFS_PD_ID,
};
 static struct clk bbp_pll = {
     "bbp_pll",
     0,
     &tcxo,
     0,
     &dfs_mode_ops,
     NULL,
     BBP_DFS_PD_ID,
};

 static struct clk abb_ch0_lwt_pll = {
     "abb_ch0_lwt_pll",
     0,
     &tcxo,
     0,
     &abb_pll_ops,
     NULL,
     ABB_CH0_LWT_PLL_ID,
};
 static struct clk abb_ch0_g_pll = {
     "abb_ch0_g_pll",
     0,
     &tcxo,
     0,
     &abb_pll_ops,
     NULL,
     ABB_CH0_G_PLL_ID,
};
 static struct clk abb_ch1_lwt_pll = {
     "abb_ch1_lwt_pll",
     0,
     &tcxo,
     0,
     &abb_pll_ops,
     NULL,
     ABB_CH1_LWT_PLL_ID,
};
 static struct clk abb_ch1_g_pll = {
     "abb_ch1_g_pll",
     0,
     &tcxo,
     0,
     &abb_pll_ops,
     NULL,
     ABB_CH1_G_PLL_ID,
};

/*
*define struct clk variability for clk used in this module
*
*define_clock(_clk, _name, _parent, _rate, _sel_parents, _clkid)
*
*define_clock_without_reg(_clk, _name, _rate)
*/
define_clock_without_reg(apb, "apb_pclk", 0);
define_clock_without_reg(sleep_clk, "sleep_clk", CLK_32K);
define_clock_without_reg(peri_48m, "peri_48m", PREI_48M);
define_clock_without_reg(peripll_fout1, "peripll_fout1", PERIPLL_FOUNT1);
define_clock_without_reg(peripll_foutpost, "peripll_foutpost", PERIPLL_FOUNTPOST);
define_clock_without_reg(peripll_fout3, "peripll_fout3", PERIPLL_FOUNT3);
define_clock_without_reg(peripll_fout4, "peripll_fout4", PERIPLL_FOUNT4);
define_clock_without_reg(dwc, "usbdrd30", 0);

/*用于设置SIO外部时钟反向*/
define_clock_without_reg(reverse_sio, "reverse_sio_clk", TCXO);

static struct clksel perifout4_tcxo[] = {
	{&tcxo, 0x0,TCXO, TCXO},
	{&peripll_fout4, 0x1, PERIPLL_FOUNT4, PERIPLL_FOUNT4},
	{NULL, 0x0, 0x0, 0x0 },
};


/*用于SIO外部时钟极性选择*/
static struct clksel sio_outer_pol[] = {
	{&lte_sio, 0x0, TCXO, TCXO},
	{&reverse_sio, 0x1, TCXO, TCXO},
	{NULL, 0x0, 0x0, 0x0 },
};

define_clock(axi_master, "mst_aclk", peripll_fout1, PERIPLL_FOUNT1, NULL, MST_ACLK_ID);
define_clock(dsp, "dsp_aclk", peripll_fout1, PERIPLL_FOUNT1, NULL, DSP_ACLK_ID);
define_clock(soc_hclk, "soc_hclk", peripll_fout1, PERIPLL_FOUNT1, NULL, SOC_HCLK_ID);
define_clock(amon_cpufast, "amon_cpufast_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, AMON_CPUFAST_CLK_ID);
define_clock(amon_soc, "amon_soc_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, AMON_SOC_CLK);
define_clock(tsensor, "tsensor_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, TSENSOR_CLK_ID);
define_clock(ipf, "ipf_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, IPF_CLK_ID);
define_clock(edmac_ch4, "edmac_ch4_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, EDMAC_CHE4_CLK_ID);
define_clock(socp, "socp_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, SOCP_CLK_ID);
define_clock(mipi, "mipi_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, MIPI_CLK_ID);
define_clock(pmussi0, "pmussi0_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, PMUSSI0_CLK_ID);
define_clock(pmussi1, "pmussi1_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, PMUSSI1_CLK_ID);
define_clock(dw_ssi0, "dw_ssi0_clk", peri_48m, PREI_48M, NULL, DW_SSI0_CLK_ID);
define_clock(efuse, "efuse_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, EFUSE_CLK_ID);
define_clock(nandc, "nandc_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, NANDC_CLK_ID);
define_clock(hs_uart, "hs_uart_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, HS_UART_CLK_ID);
define_clock(emi, "emi_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, EMI_CLK_ID);
define_clock(edmac, "emdac_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, EDMAC_CLK_ID);
define_clock(bootrom, "bootrom_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, BOOTROM_CLK_ID);
define_clock(pwm1, "pwm1_clk", tcxo, TCXO, NULL, PWM1_CLK_ID);
define_clock(pwm0, "pwm0_clk", tcxo, TCXO, NULL, PWM0_CLK_ID);
define_clock(dt, "dt_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, DT_CLK_ID);
define_clock(ipcm, "ipcm_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, IPCM_CLK_ID);
define_clock(dw_ssi1, "dw_ssi1_clk", peri_48m, PREI_48M, NULL, DW_SSI1_CLK_ID);
define_clock(i2c1, "i2c1_clk", peri_48m, PREI_48M, NULL, I2C1_CLK_ID);
define_clock(gpio1, "gpio1_clk", peri_48m, PREI_48M, NULL, CPIO1_CLK_ID);
define_clock(i2c0, "i2c0_clk", peri_48m, PREI_48M, NULL, I2C0_CLK_ID);
define_clock(gpio3, "gpio3_clk", peri_48m, PREI_48M, NULL, GPIO3_CLK_ID);

define_clock(uart3, "uart3_clk", peri_48m, PREI_48M, NULL, UART3_CLK_ID);
define_clock(uart2, "uart2_clk", peri_48m, PREI_48M, NULL, UART2_CLK_ID);
define_clock(gpio2, "gpio2_clk", peri_48m, PREI_48M, NULL, GPIO2_CLK_ID);
define_clock(coresight, "cs_clk", peri_48m, PREI_48M, NULL, CS_CLK_ID);
define_clock(appa9_mbist, "appa9_mbist_clk", peri_48m, PREI_48M, NULL, APPA9_MBIST_CLK_ID);
define_clock(moda9_mbist, "moda9_mbist_clk", peri_48m, PREI_48M, NULL, MONA9_MBIST_CLK_ID);
define_clock(fast_aclk, "cpu_aclk", peri_48m, PREI_48M, NULL, CPU_ACLK_ID);
define_clock(mddrc, "mddrc_clk", peri_48m, PREI_48M, NULL, MDDRC_CLK_ID);
define_clock(ddrphy, "ddrphy_clk", peri_48m, PREI_48M, NULL, DDRPHY_CLK_ID);
define_clock(ddrphy_2x, "ddrphy_ddr_clk", peri_48m, PREI_48M, NULL, DDRPHY_DDR_CLK_ID);
define_clock(axi_mem, "axi_men_clk", peri_48m, PREI_48M, NULL, AXI_MEN_CLK_ID);
define_clock(func_wakeup, "func_wakeup_pd_clk", peri_48m, PREI_48M, NULL, FUNC_WAKEUP_PD_CLK_ID);
define_clock(soc_peri_usb, "soc_peri_usb_pd_clk", peri_48m, PREI_48M, NULL, SOC_PERI_USB_PD_CLK_ID);
define_clock(usbotg, "usbotg_clk", peri_48m, PREI_48M, NULL, USBOTG_CLK_ID);
define_clock(usbotg_bc, "usb_bc_clk", peri_48m, PREI_48M, NULL, USB_BC_CLK_ID);
define_clock(mmc0, "mmc0_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, MMC0_CLK_ID);
define_clock(mmc1, "mmc1_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, MMC1_CLK_ID);
define_clock(mmc0_phase, "mmc0_phase_clk", mmc0, PERIPLL_FOUNT1/2, NULL, MMC0_PHASE_CLK_ID);
define_clock(mmc1_phase, "mmc1_phase_clk", mmc1, PERIPLL_FOUNT1/2, NULL, MMC1_PHASE_CLK_ID);
define_clock(mmc0_ref, "mmc0_refclk", peripll_fout1, PERIPLL_FOUNT1, NULL, MMC0_REFCLK_ID);
define_clock(mmc1_ref, "mmc1_refclk", peripll_fout1, PERIPLL_FOUNT1, NULL, MMC1_REFCLK_ID);
define_clock(usbhsic, "usbhsic_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, USBHSIC_CLK_ID);
define_clock(sdcc, "sdcc_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, SDCC_CLK_ID);
define_clock(hpm, "hpm_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, HPM_CLK_ID);
define_clock(pcie_ctrl1, "pcie_ctrl1_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, PCIE_CTRL1_CLK_ID);
define_clock(pcie_ctrl2, "pcie_ctrl2_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, PCIE_CTRL2_CLK_ID);
define_clock(pcie_phy, "pcie_phy_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, PCIE_PHY_CLK_ID);
define_clock(watchdog, "watchdog_clk", peri_48m, PREI_48M, NULL, WDOG_CLK_ID);

define_clock(gpio0, "gpio0_clk", peri_48m, PREI_48M, NULL, GPIO0_CLK_ID);
define_clock(uart0, "uart0_clk", peri_48m, PREI_48M, NULL, UART0_CLK_ID);
define_clock(timer0, "tm0_clk", sleep_clk, CLK_32K, NULL, TM0_CLK_ID);
define_clock(timer1, "tm1_clk", sleep_clk, CLK_32K, NULL, TM1_CLK_ID);
define_clock(timer2, "tm2_clk", sleep_clk, CLK_32K, NULL, TM2_CLK_ID);
define_clock(timer3, "tm3_clk", sleep_clk, CLK_32K, NULL, TM3_CLK_ID);
define_clock(timer4, "tm4_clk", sleep_clk, CLK_32K, NULL, TM4_CLK_ID);
define_clock(timer5, "tm5_clk", sleep_clk, CLK_32K, NULL, TM5_CLK_ID);
define_clock(timer6, "tm6_clk", sleep_clk, CLK_32K, NULL, TM6_CLK_ID);
define_clock(timer7, "tm7_clk", sleep_clk, CLK_32K, NULL, TM7_CLK_ID);
define_clock(timer8, "tm8_clk", sleep_clk, CLK_32K, NULL, TM8_CLK_ID);
define_clock(timer9, "tm9_clk", sleep_clk, CLK_32K, NULL, TM9_CLK_ID);
define_clock(timer10, "tm10_clk", sleep_clk, CLK_32K, NULL, TM10_CLK_ID);
define_clock(timer11, "tm11_clk", sleep_clk, CLK_32K, NULL, TM11_CLK_ID);
define_clock(timer12, "tm12_clk", sleep_clk, CLK_32K, NULL, TM12_CLK_ID);
define_clock(timer13, "tm13_clk", sleep_clk, CLK_32K, NULL, TM13_CLK_ID);
define_clock(timer14, "tm14_clk", sleep_clk, CLK_32K, NULL, TM14_CLK_ID);
define_clock(timer15, "tm15_clk", sleep_clk, CLK_32K, NULL, TM15_CLK_ID);
define_clock(timer16, "tm16_clk", sleep_clk, CLK_32K, NULL, TM16_CLK_ID);
define_clock(timer17, "tm17_clk", sleep_clk, CLK_32K, NULL, TM17_CLK_ID);
define_clock(timer18, "tm18_clk", sleep_clk, CLK_32K, NULL, TM18_CLK_ID);
define_clock(timer19, "tm19_clk", sleep_clk, CLK_32K, NULL, TM19_CLK_ID);
define_clock(timer20, "tm20_clk", sleep_clk, CLK_32K, NULL, TM20_CLK_ID);
define_clock(timer21, "tm21_clk", sleep_clk, CLK_32K, NULL, TM21_CLK_ID);
define_clock(timer22, "tm22_clk", sleep_clk, CLK_32K, NULL, TM22_CLK_ID);
define_clock(timer23, "tm23_clk", sleep_clk, CLK_32K, NULL, TM23_CLK_ID);
define_clock(appa9_vol, "appa9_clk", peri_48m, PREI_48M, NULL, APPA9_CLK_ID);
define_clock(moda9_vol, "mdma9_clk", peri_48m, PREI_48M, NULL, MOMA9_CLK_ID);
  /*lint -e31*/
define_clock(lte_sio, "sio_clk", tcxo, TCXO, perifout4_tcxo, LET_SIO_CLK);
define_clock(hifi_dbg, "hifi_dbg_clk", peri_48m, PREI_48M, NULL, HIFI_DBG_CLK_ID);
define_clock(hifi, "hifi_clk", peri_48m, PREI_48M, NULL, HIFI_CLK_ID);

define_clock(dsp0, "dsp0_pd_clk", peri_48m, PREI_48M, NULL, DSP0_PD_CLK_ID);
define_clock(dsp_core, "dsp0_core_clk", peri_48m, PREI_48M, NULL, DSP0_CORE_CLK_ID);
define_clock(dsp_dbg, "dsp0_dbg_clk", peri_48m, PREI_48M, NULL, DSP0_DBG_CLK_ID);
define_clock(guacc, "guacc_axi_clk", peri_48m, PREI_48M, NULL, GUACC_AXI_CLK_ID);
define_clock(cicom0, "cicom0_clk", peri_48m, PREI_48M, NULL, CICOM0_CLK_ID);
define_clock(cicom1, "cicom1_clk", peri_48m, PREI_48M, NULL, CICOM1_CLK_ID);
define_clock(hdlc, "hdlc_clk", peri_48m, PREI_48M, NULL, HDLC_CLK_ID);
define_clock(upacc, "upacc_clk", peri_48m, PREI_48M, NULL, UPACC_CLK_ID);
define_clock(sci0, "sci0_clk", peripll_fout4, PERIPLL_FOUNT4, NULL, SCI0_CLK_ID);
define_clock(sci1, "sci1_clk", peripll_fout4, PERIPLL_FOUNT4, NULL, SCI1_CLK_ID);
define_clock(uicc, "uicc_clk", peri_48m, PREI_48M, NULL, UICC_CLK_ID);
define_clock(uart1, "uart1_clk", peri_48m, PREI_48M, NULL, UART1_CLK_ID);
define_clock(dsp_vic, "dsp_vic_clk", peri_48m, PREI_48M, NULL, DSP_VIC_CLK_ID);
 static struct clk cipher = {
    "cipher_clk",
    0,
    &peripll_foutpost,
    PERIPLL_FOUNTPOST/5,
    &clock_ops_div_n,
    NULL,
    CIPEHER_CLK_ID,
};

define_clock(bbp_ao, "bbp_ao_clk", peri_48m, PREI_48M, NULL, BBP_AO_CLK_ID);
define_clock(bbp_common, "bbpcommon_2a_clk", peri_48m, PREI_48M, NULL, BBPCOMMON_2A_CLK_ID);
define_clock(lte_bbp, "ltebbp_pd_clk", peri_48m, PREI_48M, NULL, LTEBBP_PD_CLK_ID);
define_clock(g1_bbp, "g1bbp_pd_clk", peri_48m, PREI_48M, NULL, G1BBP_PD_CLK_ID);
define_clock(g2_bbp, "g2bbp_pd_clk", peri_48m, PREI_48M, NULL, G2BBP_PD_CLK_ID);
define_clock(tw_bbp, "twbbp_pd_clk", peri_48m, PREI_48M, NULL, TWBBP_PD_CLK_ID);
define_clock(w_bbp, "wbbp_pd_clk", peri_48m, PREI_48M, NULL, WBBP_PD_CLK_ID);
define_clock(irm_bbp, "irm_pd_clk", peri_48m, PREI_48M, NULL, IRM_PD_CLK_ID);
define_clock(lte_bbp_axi, "lbbp_axi_clk", peri_48m, PREI_48M, NULL, LBBP_AXI_CLK_ID);
define_clock(lte_bbp_time, "lbbp_pdt_clk", peri_48m, PREI_48M, NULL, LBBP_PTD_CLK_ID);
define_clock(lte_bbp_freq1, "lbbp_pdf_clk", peri_48m, PREI_48M, NULL, LBBP_PDF_CLK_ID);
define_clock(lte_bbp_tdl, "lbbp_tdl_clk", peri_48m, PREI_48M, NULL, LBBP_TDL_CLK_ID);
define_clock(lte_bbp_vdl, "lbbp_vdl_clk", peri_48m, PREI_48M, NULL, LBBP_VDL_CLK_ID);
define_clock(lte_bbp_freq2, "lbbp_tds_clk", peri_48m, PREI_48M, NULL, LBBP_TDS_CLK_ID);
define_clock(irm_bbp_turbo, "irm_turbo_clk", peri_48m, PREI_48M, NULL, IRM_TURBO_CLK_ID);
define_clock(irm_bbp_245m, "irm_bbp_245m_clk", peri_48m, PREI_48M, NULL, IRM_BBP_245M_CLK_ID);
define_clock(irm_wbbp_122m, "irm_wbbp_122m_clk", peri_48m, PREI_48M, NULL, IRM_WBBP_122M_CLK_ID);
define_clock(tw_wbbp_122m, "twbbp_wbbp_122m_clk", peri_48m, PREI_48M, NULL, TWBBP_WBBP_122M_CLK_ID);
define_clock(tw_wbbp_61m, "twbbp_wbbp_61m_clk", peri_48m, PREI_48M, NULL, TWBBP_WBBP_61M_CLK_ID);
define_clock(tw_tbbp_122m, "twbbp_tbbp_122m_clk", peri_48m, PREI_48M, NULL, TWBBP_TBBP_122M_CLK_ID);
define_clock(tw_tbbp_245m, "twbbp_tbbp_245m_clk", peri_48m, PREI_48M, NULL, TWBBP_TBBP_245M_CLK_ID);
define_clock(wbbp_61m, "wbbp_61m_clk", peri_48m, PREI_48M, NULL, WBBP_61M_CLK_ID);
define_clock(wbbp_122m, "wbbp_122m_clk", peri_48m, PREI_48M, NULL, WBBP_122M_CLK_ID);
define_clock(wbbp_245m, "wbbp_245m_clk", peri_48m, PREI_48M, NULL, WBBP_245M_CLK_ID);
define_clock(wbbp_axi, "wbbp_axi_clk", peri_48m, PREI_48M, NULL, WBBP_AXI_CLK_ID);

define_clock(mmc2, "mmc2_clk", peripll_fout1, PERIPLL_FOUNT1, NULL, MMC2_CLK_ID);
define_clock(mmc0_card, "mmc0_cclk", mmc0_phase, MMC_DIV8, NULL, MMC0_CCLK_ID);
define_clock(mmc1_card, "mmc1_cclk", mmc1_phase, MMC_DIV8, NULL, MMC1_CCLK_ID);
define_clock(mmc2_card, "mmc2_cclk", mmc2, MMC_DIV8, NULL, MMC2_CCLK_ID);
define_clock(pcm_bclk, "sio_bclk", lte_sio, TCXO/0x80, NULL, SIO_BCLK_ID);
define_clock(pcm_sync, "sio_syncclk", pcm_bclk, TCXO/0x78, NULL, SIO_SYNCLK_ID);
define_clock(sio_outer, "sio_outer_clk", lte_sio, TCXO, sio_outer_pol, SIO_OUTER_CLK_ID);


/*时钟查询数据结构数组*/
struct clk_lookup hi6930_clk_lookup[] = {
    {regiser_clock(NULL, "abb_ch0_lwt_pll", abb_ch0_lwt_pll)},
    {regiser_clock(NULL, "abb_ch0_g_pll", abb_ch0_g_pll)},
    {regiser_clock(NULL, "abb_ch1_lwt_pll", abb_ch1_lwt_pll)},
    {regiser_clock(NULL, "abb_ch1_g_pll", abb_ch1_g_pll)},
    {regiser_clock(NULL, "bbe16_pll", bbe16_pll)},
    {regiser_clock(NULL, "bbp_pll", bbp_pll)},
    {regiser_clock("uart0", "apb_pclk", apb)},
    {regiser_clock(NULL, "usbdrd30", dwc)},
    {regiser_clock(NULL, "sleep_clk", sleep_clk)},
    {regiser_clock(NULL, "peripll_fout1", peripll_fout1)},
    {regiser_clock(NULL, "peripll_fout3", peripll_fout3)},
    {regiser_clock(NULL, "peripll_fout4", peripll_fout4)},
    {regiser_clock(NULL, "peripll_foutpost", peripll_foutpost)},
    {regiser_clock(NULL, "peri_48m", peri_48m)},
    {regiser_clock(NULL, "tcxo", tcxo)},

    {regiser_clock(NULL, "mst_aclk", axi_master)},
    {regiser_clock(NULL, "dsp_aclk", dsp)},
    {regiser_clock(NULL, "soc_hclk", soc_hclk)},
    {regiser_clock(NULL, "amon_cpufast_clk", amon_cpufast)},
    {regiser_clock(NULL, "amon_soc_clk", amon_soc)},
    {regiser_clock(NULL, "tsensor_clk", tsensor)},
    {regiser_clock(NULL, "ipf_clk", ipf)},
    {regiser_clock(NULL, "edmac_ch4_clk", edmac_ch4)},
    {regiser_clock(NULL, "socp_clk", socp)},
    {regiser_clock(NULL, "mipi_clk", mipi)},
    {regiser_clock(NULL, "pmussi0_clk", pmussi0)},
    {regiser_clock(NULL, "pmussi1_clk", pmussi1)},
    {regiser_clock(NULL, "dw_ssi0_clk", dw_ssi0)},
    {regiser_clock(NULL, "efuse_clk", efuse)},
    {regiser_clock(NULL, "nandc_clk", nandc)},
    {regiser_clock(NULL, "hs_uart_clk", hs_uart)},
    {regiser_clock(NULL, "emi_clk", emi)},
    {regiser_clock(NULL, "emdac_clk", edmac)},
    {regiser_clock(NULL, "bootrom_clk", bootrom)},
    {regiser_clock(NULL, "pwm1_clk", pwm1)},
    {regiser_clock(NULL, "pwm0_clk", pwm0)},
    {regiser_clock(NULL, "dt_clk", dt)},
    {regiser_clock(NULL, "ipcm_clk", ipcm)},
    {regiser_clock(NULL, "dw_ssi1_clk", dw_ssi1)},
    {regiser_clock(NULL, "i2c1_clk", i2c1)},
    {regiser_clock(NULL, "gpio1_clk", gpio1)},
    {regiser_clock(NULL, "i2c0_clk", i2c0)},
    {regiser_clock(NULL, "gpio3_clk", gpio3)},

    {regiser_clock(NULL, "uart3_clk", uart3)},
    {regiser_clock(NULL, "uart2_clk", uart2)},
    {regiser_clock(NULL, "gpio2_clk", gpio2)},
    {regiser_clock(NULL, "cs_clk", coresight)},
    {regiser_clock(NULL, "appa9_mbist_clk", appa9_mbist)},
    {regiser_clock(NULL, "moda9_mbist_clk", moda9_mbist)},
    {regiser_clock(NULL, "cpu_aclk", fast_aclk)},
    {regiser_clock(NULL, "mddrc_clk", mddrc)},
    {regiser_clock(NULL, "ddrphy_clk", ddrphy)},
    {regiser_clock(NULL, "ddrphy_ddr_clk", ddrphy_2x)},
    {regiser_clock(NULL, "axi_men_clk", axi_mem)},
    {regiser_clock(NULL, "func_wakeup_pd_clk", func_wakeup)},
    {regiser_clock(NULL, "soc_peri_usb_pd_clk", soc_peri_usb)},
    {regiser_clock(NULL, "usbotg_clk", usbotg)},
    {regiser_clock(NULL, "usb_bc_clk", usbotg_bc)},
    {regiser_clock(NULL, "mmc0_clk", mmc0)},
    {regiser_clock(NULL, "mmc1_clk", mmc1)},
    {regiser_clock(NULL, "mmc0_phase_clk", mmc0_phase)},
    {regiser_clock(NULL, "mmc1_phase_clk", mmc1_phase)},
    {regiser_clock(NULL, "mmc0_refclk", mmc0_ref)},
    {regiser_clock(NULL, "mmc1_refclk", mmc1_ref)},
    {regiser_clock(NULL, "usbhsic_clk", usbhsic)},
    {regiser_clock(NULL, "sdcc_clk", sdcc)},
    {regiser_clock(NULL, "hpm_clk", hpm)},
    {regiser_clock(NULL, "pcie_ctrl1_clk", pcie_ctrl1)},
    {regiser_clock(NULL, "pcie_ctrl2_clk", pcie_ctrl2)},
    {regiser_clock(NULL, "pcie_phy_clk", pcie_phy)},
    {regiser_clock(NULL, "watchdog_clk", watchdog)},

    {regiser_clock(NULL, "gpio0_clk", gpio0)},
    {regiser_clock("uart0", "uart0_clk", uart0)},
    {regiser_clock(NULL, "tm0_clk", timer0)},
    {regiser_clock(NULL, "tm1_clk", timer1)},
    {regiser_clock(NULL, "tm2_clk", timer2)},
    {regiser_clock(NULL, "tm3_clk", timer3)},
    {regiser_clock(NULL, "tm4_clk", timer4)},
    {regiser_clock(NULL, "tm5_clk", timer5)},
    {regiser_clock(NULL, "tm6_clk", timer6)},
    {regiser_clock(NULL, "tm7_clk", timer7)},
    {regiser_clock(NULL, "tm8_clk", timer8)},
    {regiser_clock(NULL, "tm9_clk", timer9)},
    {regiser_clock(NULL, "tm10_clk", timer10)},
    {regiser_clock(NULL, "tm11_clk", timer11)},
    {regiser_clock(NULL, "tm12_clk", timer12)},
    {regiser_clock(NULL, "tm13_clk", timer13)},
    {regiser_clock(NULL, "tm14_clk", timer14)},
    {regiser_clock(NULL, "tm15_clk", timer15)},
    {regiser_clock(NULL, "tm16_clk", timer16)},
    {regiser_clock(NULL, "tm17_clk", timer17)},
    {regiser_clock(NULL, "tm18_clk", timer18)},
    {regiser_clock(NULL, "tm19_clk", timer19)},
    {regiser_clock(NULL, "tm20_clk", timer20)},
    {regiser_clock(NULL, "tm21_clk", timer21)},
    {regiser_clock(NULL, "tm22_clk", timer22)},
    {regiser_clock(NULL, "tm23_clk", timer23)},
    {regiser_clock(NULL, "appa9_clk", appa9_vol)},
    {regiser_clock(NULL, "mdma9_clk", moda9_vol)},
    {regiser_clock(NULL, "hifi_dbg_clk", hifi_dbg)},
    {regiser_clock(NULL, "sio_clk", lte_sio)},
    {regiser_clock(NULL, "hifi_clk", hifi)},

    {regiser_clock(NULL, "dsp0_pd_clk", dsp0)},
    {regiser_clock(NULL, "dsp0_core_clk", dsp_core)},
    {regiser_clock(NULL, "dsp0_dbg_clk", dsp_dbg)},
    {regiser_clock(NULL, "guacc_axi_clk", guacc)},
    {regiser_clock(NULL, "cicom0_clk", cicom0)},
    {regiser_clock(NULL, "cicom1_clk", cicom1)},
    {regiser_clock(NULL, "hdlc_clk", hdlc)},
    {regiser_clock(NULL, "upacc_clk", upacc)},
    {regiser_clock(NULL, "sci0_clk", sci0)},
    {regiser_clock(NULL, "sci1_clk", sci1)},
    {regiser_clock(NULL, "uicc_clk", uicc)},
    {regiser_clock(NULL, "uart1_clk", uart1)},
    {regiser_clock(NULL, "dsp_vic_clk", dsp_vic)},
    {regiser_clock(NULL, "cipher_clk", cipher)},

    {regiser_clock(NULL, "bbp_ao_clk", bbp_ao)},
    {regiser_clock(NULL, "bbpcommon_2a_clk", bbp_common)},
    {regiser_clock(NULL, "ltebbp_pd_clk", lte_bbp)},
    {regiser_clock(NULL, "g1bbp_pd_clk", g1_bbp)},
    {regiser_clock(NULL, "g2bbp_pd_clk", g2_bbp)},
    {regiser_clock(NULL, "twbbp_pd_clk", tw_bbp)},
    {regiser_clock(NULL, "wbbp_pd_clk", w_bbp)},
    {regiser_clock(NULL, "irm_pd_clk", irm_bbp)},
    {regiser_clock(NULL, "lbbp_axi_clk", lte_bbp_axi)},
    {regiser_clock(NULL, "lbbp_pdt_clk", lte_bbp_time)},
    {regiser_clock(NULL, "lbbp_pdf_clk", lte_bbp_freq1)},
    {regiser_clock(NULL, "lbbp_tdl_clk", lte_bbp_tdl)},
    {regiser_clock(NULL, "lbbp_vdl_clk", lte_bbp_vdl)},
    {regiser_clock(NULL, "lbbp_tds_clk", lte_bbp_freq2)},
    {regiser_clock(NULL, "irm_turbo_clk", irm_bbp_turbo)},
    {regiser_clock(NULL, "irm_bbp_245m_clk", irm_bbp_245m)},
    {regiser_clock(NULL, "irm_wbbp_122m_clk", irm_wbbp_122m)},
    {regiser_clock(NULL, "twbbp_wbbp_122m_clk", tw_wbbp_122m)},
    {regiser_clock(NULL, "twbbp_wbbp_61m_clk", tw_wbbp_122m)},
    {regiser_clock(NULL, "twbbp_wbbp_122m_clk", tw_wbbp_61m)},
    {regiser_clock(NULL, "twbbp_tbbp_122m_clk", tw_tbbp_122m)},
    {regiser_clock(NULL, "twbbp_tbbp_245m_clk", tw_tbbp_245m)},
    {regiser_clock(NULL, "wbbp_61m_clk", wbbp_61m)},
    {regiser_clock(NULL, "wbbp_122m_clk", wbbp_122m)},
    {regiser_clock(NULL, "wbbp_245m_clk", wbbp_245m)},
    {regiser_clock(NULL, "wbbp_axi_clk", wbbp_axi)},

    {regiser_clock(NULL, "mmc2_clk", mmc2)},
    {regiser_clock(NULL, "mmc2_cclk", mmc2_card)},
    {regiser_clock(NULL, "mmc0_cclk", mmc0_card)},
    {regiser_clock(NULL, "mmc1_cclk", mmc1_card)},
    {regiser_clock(NULL, "sio_bclk", pcm_bclk)},
    {regiser_clock(NULL, "sio_syncclk", pcm_sync)},
    {regiser_clock(NULL, "reverse_sio_clk", reverse_sio)},
    {regiser_clock(NULL, "sio_outer_clk", sio_outer)},
    {{NULL,NULL}, NULL, NULL, NULL},
};
/*lint -restore +e31*/


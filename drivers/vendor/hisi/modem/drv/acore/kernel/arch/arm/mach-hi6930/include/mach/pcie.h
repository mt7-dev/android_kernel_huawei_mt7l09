#ifndef __BALONG_PCIE_H__
#define __BALONG_PCIE_H__

#include "asm/delay.h"
#include "bsp_memmap.h"
#include "hi_syssc.h"
#include "hi_syscrg.h"

#if defined(CONFIG_ARCH_P531)
#define REG_BASE_SCTRL           (HI_SYSCRG_BASE_ADDR)
#define RSTREQ3                  (HI_CRG_CTRL16_OFFSET)
    /*default 0, not need modify?*/
    #define pcie_phy_ahb_srst_req (10)
    #define pcie_phy_por_srst_req (9)
    /*bit8..3*/
    #define pcie_phy_srst_req     (3)
    #define pcie_phy_srst_req_mask (0x3f)
    #define pcie_ctrl_srst_req    (2)
#define CLKEN5                  (HI_CRG_CTRL39_OFFSET)
#define CLKDIS5                 (HI_CRG_CTRL40_OFFSET)
    #define pcie_clk_en          (21)
    #define pcie_clk_dis         (21)
#define REG_BASE_PCTRL          (HI_SYSSC_BASE_ADDR)
#define PCIE0_STAT0             (HI_SC_STAT30_OFFSET)
#define PCIE0_STAT1             (HI_SC_STAT31_OFFSET)
#define PCIE0_STAT2             (HI_SC_STAT32_OFFSET)
#define PCIE0_STAT3             (HI_SC_STAT33_OFFSET)
#define PCIE0_STAT4             (HI_SC_STAT34_OFFSET)
#define PCIE0_PHY_STAT          (HI_SC_STAT35_OFFSET)
#define PCIE0_CTRL0             (HI_SC_CTRL138_OFFSET)
#define PCIE0_CTRL1             (HI_SC_CTRL139_OFFSET)
#define PCIE0_CTRL2             (HI_SC_CTRL140_OFFSET)
#define PCIE0_CTRL3             (HI_SC_CTRL141_OFFSET)
#define PCIE0_CTRL4             (HI_SC_CTRL142_OFFSET)
#define PCIE0_CTRL5             (HI_SC_CTRL143_OFFSET)
#define PCIE0_CTRL6             (HI_SC_CTRL144_OFFSET)
#define PCIE0_CTRL7             (HI_SC_CTRL145_OFFSET)
#define PCIE0_CTRL8             (HI_SC_CTRL146_OFFSET)
#define PCIE0_CTRL9             (HI_SC_CTRL147_OFFSET)
#define PCIE0_CTRL10            (HI_SC_CTRL148_OFFSET)
#define PCIE0_CTRL11            (HI_SC_CTRL149_OFFSET)
#define PCIE0_CTRL12            (HI_SC_CTRL150_OFFSET)
#define PCIE_PHY_CTRL0          (HI_SC_CTRL151_OFFSET)
#define PCIE_PHY_CTRL1          (HI_SC_CTRL152_OFFSET)
#define PCIE_PHY_CTRL2          (HI_SC_CTRL153_OFFSET)
#define PCIE_PHY_CTRL3          (HI_SC_CTRL154_OFFSET)
#define PCIE_PHY_CTRL4          (HI_SC_CTRL155_OFFSET)
#define PCIE_PHY_CTRL5          (HI_SC_CTRL156_OFFSET)
#define PCIE_PIPE_CTRL0         (HI_SC_CTRL157_OFFSET)
#define PCIE_PIPE_CTRL1         (HI_SC_CTRL158_OFFSET)
#define REG_BASE_PCIE_CFG       (HI_PCIE0_CFG_BASE_ADDR)
#define REG_BASE_PCIE_CFG_SIZE  (HI_PCIE0_CFG_SIZE)
#define REG_BASE_PCIE_DATA      (HI_PCIE0_DATA_BASE_ADDR)
#define REG_BASE_PCIE_DATA_SIZE (HI_PCIE0_DATA_SIZE)
#elif defined(CONFIG_ARCH_V7R2)
#define REG_BASE_SCTRL          (HI_SYSCRG_BASE_ADDR)
#define CLKEN2                  (HI_CRG_CLKEN2_OFFSET)
#define CLKDIS2                 (HI_CRG_CLKDIS2_OFFSET)
#define CLKSTAT2                (HI_CRG_CLKSTAT2_OFFSET)
    #define pcie_phy_clk_pos    (29)
    #define pcie_ctrl_clk_pos   (27)
    #define pcie_ctrl_clk_mask  (0x3)
    #define pcie_clk_mask       (0x7)
    #define soc_peri_usb_pd_clk_pos (12)
#define SRSTEN2                 (HI_CRG_SRSTEN2_OFFSET)
#define SRSTDIS2                (HI_CRG_SRSTDIS2_OFFSET)
#define SRSTSTAT2               (HI_CRG_SRSTSTAT2_OFFSET)
    #define pcie_phy_srst_pos    (15)
    #define pcie_ctrl_srst_pos   (14)
    #define soc_peri_usb_pd_srst_pos (5)
#define REG_BASE_PCTRL          (HI_SYSSC_BASE_ADDR)
#define PCIE0_STAT0             (HI_SC_STAT70_OFFSET)
#define PCIE0_STAT1             (HI_SC_STAT71_OFFSET)
#define PCIE0_STAT2             (HI_SC_STAT72_OFFSET)
#define PCIE0_STAT3             (HI_SC_STAT73_OFFSET)
#define PCIE0_STAT4             (HI_SC_STAT74_OFFSET)
#define PCIE0_PHY_STAT          (HI_SC_STAT75_OFFSET)
#define PCIE0_CTRL0             (HI_SC_CTRL58_OFFSET)
#define PCIE0_CTRL1             (HI_SC_CTRL59_OFFSET)
#define PCIE0_CTRL2             (HI_SC_CTRL60_OFFSET)
#define PCIE0_CTRL3             (HI_SC_CTRL61_OFFSET)
#define PCIE0_CTRL4             (HI_SC_CTRL62_OFFSET)
#define PCIE0_CTRL5             (HI_SC_CTRL63_OFFSET)
#define PCIE0_CTRL6             (HI_SC_CTRL64_OFFSET)
#define PCIE0_CTRL7             (HI_SC_CTRL65_OFFSET)
#define PCIE0_CTRL8             (HI_SC_CTRL66_OFFSET)
#define PCIE0_CTRL9             (HI_SC_CTRL67_OFFSET)
#define PCIE0_CTRL10            (HI_SC_CTRL106_OFFSET)
#define PCIE0_CTRL11            (HI_SC_CTRL107_OFFSET)
#define PCIE0_CTRL12            (HI_SC_CTRL108_OFFSET)
#define PCIE_PHY_CTRL0          (HI_SC_CTRL109_OFFSET)
#define PCIE_PHY_CTRL1          (HI_SC_CTRL110_OFFSET)
#define PCIE_PHY_CTRL2          (HI_SC_CTRL111_OFFSET)
#define PCIE_PHY_CTRL3          (HI_SC_CTRL112_OFFSET)
#define REG_BASE_PCIE_CFG       (HI_PCIE_CFG_BASE_ADDR)
#define REG_BASE_PCIE_CFG_SIZE  (HI_PCIE_CFG_SIZE)
#define REG_BASE_PCIE_DATA      (HI_PCIE_DATA_BASE_ADDR)
#define REG_BASE_PCIE_DATA_SIZE (HI_PCIE_DATA_SIZE)
#endif

/*
pcie controller offset & mask define
*/
/*PCIE0_STAT0*/
    /*phy Link up/down indicator*/
    #define pcie_xmlh_link_up   (15)
    /*data Link layer up/down indicator*/
    #define pcie_rdlh_link_up   (5)
/*PCIE0_STAT4*/
    /*link up successful 0x11*/
    #define ltssm_state         (0)
    #define ltssm_state_mask    (0x3f)
    #define ltssm_state_l0      (0x11)
    #define mac_phy_rate        (6)
    #define mac_phy_rate_mask   (0x3)
/*PCIE0_CTRL0*/
	#define  pcie_device_type (28)
    #define  pcie_slv_awmisc_info (0)
/*PCIE0_CTRL1*/
    #define  pcie_slv_armisc_info (0)
/*PCIE0_CTRL7*/
	#define  pcie_app_ltssm_enable	(11)
    #define  pcie_int_ctrl          (21)
    #define  pcie_int_ctrl_mask     (0xff)
/*PCIE0_CTRL8*/
    #define  pcie_err_mask           (0x1f)


/*config&io&mem space define*/
#define PCIE0_MEM_SPACE0_START       (REG_BASE_PCIE_DATA)
#define PCIE0_MEM_SPACE0_SIZE        (0x8000000)
#define PCIE0_MEM_SPACE0_END         (REG_BASE_PCIE_DATA + PCIE0_MEM_SPACE0_SIZE - 1)
#define PCIE0_TYPE0_CFG_SPACE_START	 (PCIE0_MEM_SPACE0_START + 0x8000000)
#define PCIE0_TYPE0_CFG_SPACE_SIZE   (0x200000)
#define PCIE0_TYPE0_CFG_SPACE_END	 (PCIE0_TYPE0_CFG_SPACE_START + PCIE0_TYPE0_CFG_SPACE_SIZE - 1)
#define PCIE0_TYPE1_CFG_SPACE_START	 (PCIE0_TYPE0_CFG_SPACE_START + PCIE0_TYPE0_CFG_SPACE_SIZE)
#define PCIE0_TYPE1_CFG_SPACE_SIZE   (0xC00000)
#define PCIE0_TYPE1_CFG_SPACE_END	 (PCIE0_TYPE1_CFG_SPACE_START + PCIE0_TYPE1_CFG_SPACE_SIZE - 1)
#define PCIE0_IO_SPACE_START         (PCIE0_TYPE1_CFG_SPACE_START + PCIE0_TYPE1_CFG_SPACE_SIZE)
#define PCIE0_IO_SPACE_SIZE          (0x10000)
#define PCIE0_IO_SPACE_END           (PCIE0_IO_SPACE_START + PCIE0_IO_SPACE_SIZE - 1)
#define PCIE0_MEM_SPACE1_START       (PCIE0_TYPE0_CFG_SPACE_START + 0x1000000)
#define PCIE0_MEM_SPACE1_SIZE        (REG_BASE_PCIE_DATA + REG_BASE_PCIE_DATA_SIZE - PCIE0_MEM_SPACE1_START)
#define PCIE0_MEM_SPACE1_END         (REG_BASE_PCIE_DATA + REG_BASE_PCIE_DATA_SIZE - 1)
/*pcie core register*/
#define PORT_LOGIC_BASE (0x700)

/*msi*/
#define MSIC_LOWER_ADDR    (PORT_LOGIC_BASE + 0x120)
#define MSIC_UPPER_ADDR    (PORT_LOGIC_BASE + 0x124)
#define MSIC_IRQ_ENABLE(x) (PORT_LOGIC_BASE + 0x128 + x*0xC)
#define MSIC_IRQ_MASK(x)   (PORT_LOGIC_BASE + 0x12c + x*0xC)
#define MSIC_IRQ_STATUS(x) (PORT_LOGIC_BASE + 0x130 + x*0xC)

/*iatu*/
#define IATU_IDX           (PORT_LOGIC_BASE + 0x200)
#define IATU_CTRL1         (PORT_LOGIC_BASE + 0x204)
#define IATU_CTRL2         (PORT_LOGIC_BASE + 0x208)
#define IATU_LOWER_ADDR    (PORT_LOGIC_BASE + 0x20C)
#define IATU_UPPER_ADDR    (PORT_LOGIC_BASE + 0x210)
#define IATU_LIMIT_ADDR    (PORT_LOGIC_BASE + 0x214)
#define IATU_LTAR_ADDR     (PORT_LOGIC_BASE + 0x218)
#define IATU_UTAR_ADDR     (PORT_LOGIC_BASE + 0x21C)
#define IATU_CTRL3         (PORT_LOGIC_BASE + 0x220)

/*DMA Global Registers*/
#define DMA_WR_ENGINE_ENABLE (PORT_LOGIC_BASE + 0x27C)
#define DMA_WR_DOORBELL      (PORT_LOGIC_BASE + 0x280)
#define DMA_WR_ENGINE_CHN_ARB_WT_LOW (PORT_LOGIC_BASE + 0x288)
#define DMA_WR_ENGINE_CHN_ARB_WT_HIGH (PORT_LOGIC_BASE + 0x28C)

#define DMA_RD_ENGINE_ENABLE (PORT_LOGIC_BASE + 0x29C)
#define DMA_RD_DOORBELL      (PORT_LOGIC_BASE + 0x2A0)
#define DMA_RD_ENGINE_CHN_ARB_WT_LOW (PORT_LOGIC_BASE + 0x2A8)
#define DMA_RD_ENGINE_CHN_ARB_WT_HIGH (PORT_LOGIC_BASE + 0x2AC)

#define DMA_ENGINE_ENABLE  (1)
#define DMA_ENGINE_DISABLE (0)

/*DMA Interrupt Registers*/
/*Write*/
#define DMA_WR_INT_STATUS (PORT_LOGIC_BASE + 0x2BC)
#define DMA_WR_INT_MASK   (PORT_LOGIC_BASE + 0x2C4)
#define DMA_WR_INT_CLEAR  (PORT_LOGIC_BASE + 0x2C8)
#define DMA_WR_ERR_STATUS (PORT_LOGIC_BASE + 0x2CC)
#define DMA_WR_DONE_IMWR_ADDR_LOW (PORT_LOGIC_BASE + 0x2D0)
#define DMA_WR_DONE_IMWR_ADDR_HIGH (PORT_LOGIC_BASE + 0x2D4)
#define DMA_WR_ABORT_IMWR_ADDR_LOW (PORT_LOGIC_BASE + 0x2D8)
#define DMA_WR_ABORT_IMWR_ADDR_HIGH (PORT_LOGIC_BASE + 0x2DC)
#define DMA_WR_CHN_1_0_IMWR_DATA (PORT_LOGIC_BASE + 0x2E0)
#define DMA_WR_CHN_3_2_IMWR_DATA (PORT_LOGIC_BASE + 0x2E4)
#define DMA_WR_CHN_5_4_IMWR_DATA (PORT_LOGIC_BASE + 0x2E8)
#define DMA_WR_CHN_7_6_IMWR_DATA (PORT_LOGIC_BASE + 0x2EC)
#define DMA_WR_LL_ERR_ENABLE  (PORT_LOGIC_BASE + 0x300)
/*Read*/
#define DMA_RD_INT_STATUS (PORT_LOGIC_BASE + 0x310)
#define DMA_RD_INT_MASK   (PORT_LOGIC_BASE + 0x318)
#define DMA_RD_INT_CLEAR  (PORT_LOGIC_BASE + 0x31C)
#define DMA_RD_ERR_STATUS_LOW (PORT_LOGIC_BASE + 0x324)
#define DMA_RD_ERR_STATUS_HIGH (PORT_LOGIC_BASE + 0x328)
#define DMA_RD_LL_ERR_ENABLE  (PORT_LOGIC_BASE + 0x334)
#define DMA_RD_DONE_IMWR_ADDR_LOW (PORT_LOGIC_BASE + 0x33C)
#define DMA_RD_DONE_IMWR_ADDR_HIGH (PORT_LOGIC_BASE + 0x340)
#define DMA_RD_ABORT_IMWR_ADDR_LOW (PORT_LOGIC_BASE + 0x344)
#define DMA_RD_ABORT_IMWR_ADDR_HIGH (PORT_LOGIC_BASE + 0x348)
#define DMA_RD_CHN_1_0_IMWR_DATA (PORT_LOGIC_BASE + 0x34C)
#define DMA_RD_CHN_3_2_IMWR_DATA (PORT_LOGIC_BASE + 0x350)
#define DMA_RD_CHN_5_4_IMWR_DATA (PORT_LOGIC_BASE + 0x354)
#define DMA_RD_CHN_7_6_IMWR_DATA (PORT_LOGIC_BASE + 0x358)

#define done_int_pos (0)
#define done_int_mask (0xFF)
#define abort_int_pos (16)
#define abort_int_mask (0xFF)

/*DMA Channel Context Registers*/
#define DMA_CHN_CTX_IDX          (PORT_LOGIC_BASE + 0x36C)
#define DMA_CHN_CTRL1_REG        (PORT_LOGIC_BASE + 0x370)
#define DMA_CHN_CTRL2_REG        (PORT_LOGIC_BASE + 0x374)
#define DMA_TRANSFER_SIZE        (PORT_LOGIC_BASE + 0x378)
#define DMA_SAR_LOW              (PORT_LOGIC_BASE + 0x37C)
#define DMA_SAR_HIGH             (PORT_LOGIC_BASE + 0x380)
#define DMA_DAR_LOW              (PORT_LOGIC_BASE + 0x384)
#define DMA_DAR_HIGH             (PORT_LOGIC_BASE + 0x388)
#define DMA_LL_PTR_LOW           (PORT_LOGIC_BASE + 0x38C)
#define DMA_LL_PTR_HIGH          (PORT_LOGIC_BASE + 0x390)

#define TIME_TO_WAIT      10000   /*default is 10s*/

#define PCIE_INTA_PIN   (1)
#define PCIE_INTB_PIN   (2)
#define PCIE_INTC_PIN   (3)
#define PCIE_INTD_PIN   (4)

#define MSI_PCIE_LOWER_ADDR (0xf0000000)
#define MSI_PCIE_UPPER_ADDR (0x0)
#define MSI_LOCAL_LOWER_ADDR (0x38000000)
#define MSI_LOCAL_UPPER_ADDR (0x0)

#if defined(CONFIG_PCI_MSI)
/*#define MSI_SUPPORT_1_DEV_1_BIT*/
#define MSI_SUPPORT_1_DEV_32_BIT
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
#define CFG_MAX_MSI_NUM		32
#define MSI_IRQ_BASE (INT_LVL_PCIE0_RADM_D + 1)
#define MSI_NR_IRQS   32
#define MSI_IRQ_END    (MSI_IRQ_BASE + MSI_NR_IRQS)
#elif defined(MSI_SUPPORT_1_DEV_32_BIT)
#define CFG_MAX_MSI_NUM		8
#define MSI_IRQ_BASE (INT_LVL_PCIE0_RADM_D + 1)
#define MSI_NR_IRQS   8
#define MSI_IRQ_END    (MSI_IRQ_BASE + MSI_NR_IRQS)
#endif
#endif

enum pcie_sel{
	pcie_x1_sel,	/*x1 device*/
	pcie_x2_sel,
};

enum pcie_work_mode{
	pcie_wm_ep = 0x0,	/*ep mode*/
	pcie_wm_lep = 0x1,	/*legacy ep mode*/
	pcie_wm_rc = 0x4,	/*rc mode*/
};

enum pcie_controller{
	pcie_controller_none,
	pcie_controller_0,
	pcie_controller_1,
};
struct pcie_iatu{
	unsigned int viewport;		/*iATU Viewport Register*/
	unsigned int region_ctrl_1;	/*Region Control 1 Register*/
	unsigned int region_ctrl_2;	/*Region Control 2 Register*/
	unsigned int lbar;		    /*Lower Base Address Register*/
	unsigned int ubar;		    /*Upper Base Address Register*/
	unsigned int lar;		    /*Limit Address Register*/
	unsigned int ltar;		    /*Lower Target Address Register*/
	unsigned int utar;		    /*Upper Target Address Register*/
};

struct pcie_info{
	u8		root_bus_nr;			/*root bus number*/
	enum		pcie_controller controller;	/*belong to which controller*/
	unsigned int	base_addr;			/*device config space base addr*/
	unsigned int	conf_base_addr;			/*rc config space base addr*/
};

static __inline__ void dbi_enable(u32 pctrl_addr)
{
    u32 val1 = 0,val2 = 0;
    u32 tmp1 = 0,tmp2 = 0;

    val1 = readl(pctrl_addr+PCIE0_CTRL0);
    val2 = readl(pctrl_addr+PCIE0_CTRL1);
    tmp1 = (val1>>21)&0x1;
    tmp2 = (val2>>21)&0x1;

    if((tmp1 != 1)||(tmp2 != 1))
    {
        val1 |= (1<<21);
        writel(val1,pctrl_addr+PCIE0_CTRL0);

        val2 |= (1<<21);
        writel(val2,pctrl_addr+PCIE0_CTRL1);

        /*only fpga version need delay*/
#if defined(BSP_CONFIG_V7R2_SFT)
        __delay(4000);
#endif
    }


}

static __inline__ void dbi_disable(u32 pctrl_addr)
{
    u32 val1 = 0,val2 = 0;
    u32 tmp1 = 0,tmp2 = 0;

    val1 = readl(pctrl_addr+PCIE0_CTRL0);
    val2 = readl(pctrl_addr+PCIE0_CTRL1);
    tmp1 = (val1>>21)&0x1;
    tmp2 = (val2>>21)&0x1;

    if((tmp1 != 0)||(tmp2 != 0))
    {
        val1 &= ~(1<<21);
        writel(val1,pctrl_addr+PCIE0_CTRL0);

        val2 &= ~(1<<21);
        writel(val2,pctrl_addr+PCIE0_CTRL1);

        /*only fpga version need delay*/
#if defined(BSP_CONFIG_V7R2_SFT)
        __delay(4000);
#endif
    }
}

#define PCIE_DEBUG_LEVEL_REG    (1)
#define PCIE_DEBUG_LEVEL_FUNC   (2)
#define PCIE_DEBUG_LEVEL_MODULE (3)

#define PCIE_DEBUG_LEVEL PCIE_DEBUG_LEVEL_MODULE

#define PCIE_DEBUG
#ifdef PCIE_DEBUG
#define pcie_debug(level,str,arg...)\
do{\
    if((level)<= PCIE_DEBUG_LEVEL){\
        printk(KERN_DEBUG "%s->%d," str "\n" ,__func__,__LINE__,##arg);\
    }\
}while(0)
#else
#define pcie_debug(level,str,arg...)
#endif

#define pcie_assert(con)\
do{\
    if(!(con)){\
        printk(KERN_ERR "%s->%d,assert fail!\n",__func__,__LINE__);\
    }\
}while(0)

#define pcie_error(str,arg...)\
do{\
    printk(KERN_ERR "%s->%d" str "\n",__func__,__LINE__,##arg);\
}while(0)

#endif


/*lint -save -e537*/
#include <osl_types.h>
#include <osl_bio.h>
#include "product_config.h"
#include "bsp_reg_def.h"

#if defined(BSP_CONFIG_P531_ASIC)
#include "ios_drv_macro.h"

#elif (defined(BSP_CONFIG_V7R2_SFT))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"

#elif (defined(BSP_CONFIG_V7R2_ASIC))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_pd_config_save.h"

#endif

#include "bsp_version.h"
#include "hi_gpio.h"
#include "bsp_memmap.h"

#include "hi_pwrctrl.h"
#include "hi_syscrg.h"

#include "ios_list.h"
#include "ios_pd.h"
/*lint -restore */

#ifdef HI_GPIO4_REGBASE_ADDR
#define GPIO_MAX_BANK_NUM      (6)
#else
#define GPIO_MAX_BANK_NUM      (4)
#endif
#define GPIO_REG_SAVE_MUX      (7)

u32 reg_value[GPIO_MAX_BANK_NUM][GPIO_REG_SAVE_MUX];

/*每组GPIO基址*/
unsigned int bank_reg_addr[GPIO_MAX_BANK_NUM]=
{
    HI_GPIO0_REGBASE_ADDR,
    HI_GPIO1_REGBASE_ADDR,
    HI_GPIO2_REGBASE_ADDR,
    HI_GPIO3_REGBASE_ADDR,
#if (GPIO_MAX_BANK_NUM == 6)
    HI_GPIO4_REGBASE_ADDR,
    HI_GPIO5_REGBASE_ADDR
#endif
};

void gpio_resume(void)
{
#if (4 == GPIO_MAX_BANK_NUM)
    int i = 0;
    int j = 0;

    /* gpio1 2 3 开钟 */
    writel(0x1 << 28, HI_SYSCRG_BASE_ADDR_VIRT + 0x0);
    writel(0x1 << 2,  HI_SYSCRG_BASE_ADDR_VIRT + 0xc);
    writel(0x1 << 30, HI_SYSCRG_BASE_ADDR_VIRT + 0x0);

    for(i = 0;i < GPIO_MAX_BANK_NUM;i++)
    {
        if(0 == i)
        {
            continue;
        }

        writel(reg_value[j][0],bank_reg_addr[i] + HI_GPIO_SWPORT_DDR_OFFSET);
        writel(reg_value[j][1],bank_reg_addr[i] + HI_GPIO_SWPORT_DR_OFFSET);
        writel(reg_value[j][2],bank_reg_addr[i] + HI_GPIO_EXT_PORT_OFFSET);
        writel(reg_value[j][3],bank_reg_addr[i] + HI_GPIO_INTEN_OFFSET);
        writel(reg_value[j][4],bank_reg_addr[i] + HI_GPIO_INTMASK_OFFSET);

        writel(reg_value[j][5],bank_reg_addr[i] + HI_GPIO_INTTYPE_LEVEL_OFFSET);
        writel(reg_value[j][6],bank_reg_addr[i] + HI_GPIO_INT_PLOARITY_OFFSET);

        ++j;

    }
    
#endif

}

void gpio_suspend(void)
{
#if (4 == GPIO_MAX_BANK_NUM)
    int i = 0;
    int j = 0;

    for(i = 0;i < GPIO_MAX_BANK_NUM;i++)
    {
        if(0 == i)
        {
            continue;
        }

        reg_value[j][0] = readl(bank_reg_addr[i] + HI_GPIO_SWPORT_DDR_OFFSET);
        reg_value[j][1] = readl(bank_reg_addr[i] + HI_GPIO_SWPORT_DR_OFFSET);
        reg_value[j][2] = readl(bank_reg_addr[i] + HI_GPIO_EXT_PORT_OFFSET);
        reg_value[j][3] = readl(bank_reg_addr[i] + HI_GPIO_INTEN_OFFSET);
        reg_value[j][4] = readl(bank_reg_addr[i] + HI_GPIO_INTMASK_OFFSET);

        reg_value[j][5] = readl(bank_reg_addr[i] + HI_GPIO_INTTYPE_LEVEL_OFFSET);
        reg_value[j][6] = readl(bank_reg_addr[i] + HI_GPIO_INT_PLOARITY_OFFSET);

        ++j;

    }

    /* gpio1 2 3 关钟 */
    writel(0x1 << 28, HI_SYSCRG_BASE_ADDR_VIRT + 0x4);
    writel(0x1 << 2,  HI_SYSCRG_BASE_ADDR_VIRT + 0x10);
    writel(0x1 << 30, HI_SYSCRG_BASE_ADDR_VIRT + 0x4);

#endif   
}


void ios_ao_suspend(void)
{
#ifdef BSP_CONFIG_V7R2_ASIC
    DEVICE_POWER_DOWN_CONFIG;
#endif
}

void ios_suspend(void)
{
    ios_ao_suspend();
    gpio_suspend();
}

void ios_resume(void)
{
    gpio_resume();
    ios_config_resume();
}


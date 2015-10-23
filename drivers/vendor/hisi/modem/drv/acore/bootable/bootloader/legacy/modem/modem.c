#include <boot.h>
#include <debug.h>
#include <platform.h>
#include <bsp_memmap.h>
#include <libfdt.h>
#include <flash.h>
#include <modem.h>
#include <nv_balong.h>
#include <sys.h>
#include <adc.h>
#include <gpio.h>
#include <drv_nv_def.h>
#include <drv_nv_id.h>
#include <modem_id_conversion.h>
#include <hisi_udp_board_id.h>
#include <bsp_memrepair.h>
#include <bsp_pmu_hi6561.h>
//#include <preboot.h>

//#include <bsp_om.h>

#ifdef HISI_SECBOOT
#include <hisi_secureboot.h>
#endif

#define HW_VER_K3V3_UDP_RF_HI6361EVB5_VER_D_RF_T_B	(unsigned int)0X3F000606/*丝印名称HI6361EVB5 VER.D RF T*/
#define HW_VER_K3V3_RF_HIBV7R2RF7					(unsigned int)0X3F000604/*丝印名称HIBV7R2RF7*/

#ifdef BSP_CONFIG_BOARD_SFT
#define HW_VERSION				0x33333333
#elif defined(BSP_CONFIG_BOARD_UDP)
#define HW_VERSION				HW_VER_K3V3_UDP_RF_HI6361EVB5_VER_D_RF_T_B
#else
#error "error board config."
#endif

#define HW_VER_K3V3_UDP_MASK        (unsigned int)0X3F000000

#define MODEM_CSHELL_DEFAULT	0x0
#define PRT_FLAG_EN_MAGIC_M		0x24680136
#define EMMC_BLOCK_SIZE			512

unsigned int modem_addr = 0;

#define GPIO_DIR(x)		    (HI_GPIO0_REGBASE_ADDR + (x) * 0x1000 + 0x400)
#define GPIO_SYS_DIR(x)		(HI_GPIO22_REGBASE_ADDR + (x - 22) * 0x1000 + 0x400)
#define GPIO_DATA(x, y)		(HI_GPIO0_REGBASE_ADDR + (x) * 0x1000 + (1 << (2 + y)))
#define GPIO_SYS_DATA(x, y)	(HI_GPIO22_REGBASE_ADDR + (x - 22) * 0x1000 + (1 << (2 + y)))
#define GPIO_BIT(x, y)		((x) << (y))
#define GPIO_IS_SET(x, y)	(((x) >> (y)) & 0x1)

#define    GPIO_TOTAL	     213

typedef enum
{
    MODEM_PIN_INIT = 0,
    MODEM_PIN_NORMAL,
    MODEM_PIN_POWERDOWN,
    MODEM_PIN_BUTT,
};

enum
{
    MODEM_IO_MUX = 0,
    MODEM_IO_CFG,
    MODEM_GPIO_INPUT,
    MODEM_GPIO_OUTPUT,
    MODEM_IO_BUTT,
};


struct pintrl_stru{
	unsigned int ugpiog;
	unsigned int ugpio_bit;
	unsigned int uiomg_off;
	unsigned int iomg_val;
	unsigned int uiocg_off;
	unsigned int iocg_val;
	unsigned int gpio_dir;
	unsigned int gpio_val;
};

struct modem_pintrl_cfg
{
    struct pintrl_stru *pintrl_addr[MODEM_PIN_BUTT];
    unsigned int pintrl_size[MODEM_PIN_BUTT];
};

extern int is_load_modem(void);

/*board id ,local variables */
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
int bsp_version_get_hwversion_index(void)
{
    int hw_ver=0;
	hw_ver=*(int *)SHM_MEM_HW_VER_ADDR;/*get hardware version*/

    cprintf("%s :%d\n",__func__,hw_ver);
	return hw_ver;
}

int set_modem_cshell_mod(unsigned int mod)
{
	writel(mod, SHM_MEM_CHSELL_FLAG_ADDR);
	return 0;
}
unsigned int get_hkadc_convert_value(unsigned int chn)
{
	unsigned int idx;
	int adc_result;			/*hkadc读取到的值*/
	unsigned int hkadc_ver = 0X7FFFFFFF;	/*按照转换列表将hkadc值进行转换*/

	adc_result = hisi_adc_get_value(chn)/1000;
	cprintf("chn:%d,hkadc_ver:0x%x \n",chn,adc_result);
	/*将HKADC的值转换为board_id*/
	for(idx=0;idx<ARRAY_SIZE(version_vol_tab);idx++)
	{
		if(version_vol_tab[idx].vol_low<= adc_result
            && adc_result <= version_vol_tab[idx].vol_high)
		{
			/*cprintf("ver%d is:%d\n", hw_ver, idx);*/
			hkadc_ver=idx;
			break;
		}
	}

	/* not found */
	if(idx == ARRAY_SIZE(version_vol_tab))
	{
		cprintf("get hkadc value fail, idx:%d, adc_result:%d\n", idx, adc_result);
		return 0X7FFFFFFF;/*返回一个无效version值*/
	}
	//cprintf("chn:%d,hkadc_ver:0x%x \n",chn,hkadc_ver);
	return hkadc_ver;
}


#ifdef CONFIG_MODEM_PINTRL
void pintrl_data_convert(struct pintrl_stru *iocfg, unsigned int len)
{
    int i = 0;
    for(i = 0;i < len;i++)
    {
        iocfg[i].ugpiog    = cpu_to_fdt32(iocfg[i].ugpiog);
        iocfg[i].ugpio_bit = cpu_to_fdt32(iocfg[i].ugpio_bit);
        iocfg[i].uiomg_off = cpu_to_fdt32(iocfg[i].uiomg_off);
        iocfg[i].iomg_val  = cpu_to_fdt32(iocfg[i].iomg_val);
        iocfg[i].uiocg_off = cpu_to_fdt32(iocfg[i].uiocg_off);
        iocfg[i].iocg_val  = cpu_to_fdt32(iocfg[i].iocg_val);
        iocfg[i].gpio_dir  = cpu_to_fdt32(iocfg[i].gpio_dir);
        iocfg[i].gpio_val  = cpu_to_fdt32(iocfg[i].gpio_val);
    }
}

/* config modem pintrl status */
void pintrl_config(struct pintrl_stru *iocfg_stru, unsigned int length)
{
	unsigned int i;
	unsigned int uregv, value, value1, addr, addr1;

    if(NULL == iocfg_stru || (0 == length))
    {
        return;
    }

	for (i = 0; i < length; i++) {

        //pintrl_data_convert(&iocfg_stru, &(iocfg_table[i]));

		uregv = ((iocfg_stru[i].ugpiog << 3) + iocfg_stru[i].ugpio_bit);

		/* set iomg register */
		if (0xfff != iocfg_stru[i].uiomg_off) {
			value = iocfg_stru[i].iomg_val;

			if (uregv <= 164) {
				addr = HI_PINTRL_REG_ADDR + (iocfg_stru[i].uiomg_off);
			} else {
				addr = HI_PINTRL_SYS_REG_ADDR + (iocfg_stru[i].uiomg_off);
			}

			writel(value, addr);
		}

		/* set iocg register */
		if (uregv <= 164) {
			addr = HI_PINTRL_REG_ADDR + 0x800 + (iocfg_stru[i].uiocg_off);
		} else {
			addr = HI_PINTRL_SYS_REG_ADDR + 0x800 + (iocfg_stru[i].uiocg_off);
		}
		value = readl(addr) & (~0x03);
		writel(value | iocfg_stru[i].iocg_val, addr);

		/* gpio controller register */
		if (!iocfg_stru[i].iomg_val) {
			if (!iocfg_stru[i].gpio_dir) {	/* direction is in */
				if (uregv <= 164) {
					addr = GPIO_DIR(iocfg_stru[i].ugpiog);
				} else {
					addr = GPIO_SYS_DIR(iocfg_stru[i].ugpiog);
				}

				value = readl(addr) & (~(GPIO_BIT(1, iocfg_stru[i].ugpio_bit)));
				writel(value, addr);
			} else {	/* direction is out */
				if (uregv <= 164) {
					addr = GPIO_DIR(iocfg_stru[i].ugpiog);
					addr1 = GPIO_DATA(iocfg_stru[i].ugpiog, iocfg_stru[i].ugpio_bit);
				} else {
					addr = GPIO_SYS_DIR(iocfg_stru[i].ugpiog);
					addr1= GPIO_SYS_DATA(iocfg_stru[i].ugpiog, iocfg_stru[i].ugpio_bit);
				}

				value = readl(addr) | GPIO_BIT(1, iocfg_stru[i].ugpio_bit);
				value1 = GPIO_BIT(iocfg_stru[i].gpio_val, iocfg_stru[i].ugpio_bit);
				writel(value, addr);
				writel(value1, addr1);
			}
		}
	}

}

struct pintrl_stru * get_pintrl(unsigned int mode, unsigned int ugpiog, unsigned int ugpio_bit)
{
    unsigned int j = 0;
    struct pintrl_stru *pincfg = NULL;
    struct modem_pintrl_cfg *pintrl_cfg = (struct modem_pintrl_cfg *)SHM_MEM_MODEM_PINTRL_ADDR;

    if(NULL == pintrl_cfg->pintrl_addr[mode])
    {
        return NULL;
    }

    pincfg = pintrl_cfg->pintrl_addr[mode];

    for(j = 0;j < pintrl_cfg->pintrl_size[mode];j++)
    {
        if(ugpiog == pincfg->ugpiog && (ugpio_bit == pincfg->ugpio_bit))
        {
            return pincfg;

        }
        ++pincfg;
    }

    return NULL;

}


void bsp_set_pintrl(unsigned int type, unsigned int gpio, unsigned int value)
{
    unsigned int group = 0;
    unsigned int pin = 0;

    struct pintrl_stru *init_stru = NULL;
    struct pintrl_stru *normal_stru = NULL;
    struct pintrl_stru *lowpower_stru = NULL;

    if(gpio > GPIO_TOTAL || (type >= MODEM_IO_BUTT))
    {
        PRINT_ERROR("para is error, gpio = %d, type = %d, value = %d.\n", gpio, type, value);
        return ;
    }

    group = gpio / GPIO_MAX_NUMBER;
    pin = gpio % GPIO_MAX_NUMBER;

    init_stru = get_pintrl(MODEM_PIN_INIT, group, pin);
    if(NULL == init_stru)
    {
        PRINT_ERROR("get_pintrl is fail, init_stru = 0x%x.\n", init_stru);
        return ;
    }

    normal_stru = get_pintrl(MODEM_PIN_NORMAL, group, pin);
    if(NULL == normal_stru)
    {
        PRINT_ERROR("get_pintrl is fail, normal_stru = 0x%x.\n", normal_stru);
        return ;
    }

    lowpower_stru = get_pintrl(MODEM_PIN_POWERDOWN, group, pin);
    if(NULL == lowpower_stru)
    {
        PRINT_ERROR("get_pintrl is fail, lowpower_stru = 0x%x.\n", lowpower_stru);
        return ;
    }

    switch(type)
    {
        case MODEM_IO_MUX:
            init_stru->iomg_val     = value;
            normal_stru->iomg_val   = value;
            lowpower_stru->iomg_val = value;
            break;

        case MODEM_IO_CFG:
            init_stru->iocg_val     = value;
            normal_stru->iocg_val   = value;
            lowpower_stru->iocg_val = value;
            break;

        case MODEM_GPIO_INPUT:
            init_stru->iomg_val   = 0;
            init_stru->gpio_dir   = 0;

            normal_stru->iomg_val = 0;
            normal_stru->gpio_dir = 0;

            lowpower_stru->iomg_val = 0;
            lowpower_stru->gpio_dir = 0;
            break;

        case MODEM_GPIO_OUTPUT:
            init_stru->iomg_val   = 0;
            init_stru->gpio_dir   = 1;
            init_stru->gpio_val = !!value;

            normal_stru->iomg_val = 0;
            normal_stru->gpio_dir = 1;
            normal_stru->gpio_val = !!value;

            lowpower_stru->iomg_val = 0;
            lowpower_stru->gpio_dir = 1;
            lowpower_stru->gpio_val = !!value;
            break;

        default:
            PRINT_ERROR("type is error, type = %d.\n", type);
            return ;

    }

    pintrl_config(normal_stru, 1);
}

void modem_ldo_io_mux(void)
{
    u32 ret = 0;

    /*用于标识RF供电单元是Pastar or 其它*/
    NV_RFPOWER_UNIT_STRU rfpower_unit = {0,0};

    unsigned int ldo_gpio_num = sizeof(DRV_DRV_LDO_GPIO_STRU)/sizeof(DRV_DRV_LDO_GPIO_CFG);
    DRV_DRV_LDO_GPIO_STRU ldo_gpio_cfg_data;
    unsigned int i = 0;

	ret = bsp_nvm_read(NV_ID_DRV_RFPOWER_UNIT,(u8 *)&rfpower_unit,sizeof(NV_RFPOWER_UNIT_STRU));
	if(NV_OK != ret){
		cprintf("get rf power unit nv error,not set!\n");
        return;
	}

    ret = bsp_nvm_read(NV_ID_DRV_LDO_GPIO_CFG,(u8*)&ldo_gpio_cfg_data,sizeof(DRV_DRV_LDO_GPIO_STRU));
    if (NV_OK != ret)
    {
        cprintf("ldo gpio read NV=0x%x, ret = %d \n",NV_ID_DRV_LDO_GPIO_CFG, ret);
        return;
    }

    if(0x2 == rfpower_unit.rfpower_m1)
    {
        for(i = 0;i < ldo_gpio_num;i++)
        {
            if(1 == ldo_gpio_cfg_data.ldo_gpio[i].used)
            {
                bsp_set_pintrl(MODEM_GPIO_OUTPUT, ldo_gpio_cfg_data.ldo_gpio[i].gpio, 0);
            }

        }

        //set_gpio_direction_output(GPIO_123);
        //set_gpio_value(GPIO_123, 0);
        //writel((readl(IOMG_092) & 0xFFFFFFF8) | 0x0, IOMG_092);

        //set_gpio_direction_output(GPIO_124);
        //set_gpio_value(GPIO_124, 0);
        //writel((readl(IOMG_093) & 0xFFFFFFF8) | 0x0, IOMG_093);
    }

}

/* Modem pintrl extract from the device tree. */
int modem_pintrl_dts_parse(void *fdt)
{
    int i;
	int ret;
    int len;
	int offset;

	int total_space = 0x40000;
    unsigned int *data = NULL;

    char pintrl_name[16]={0};
    const struct fdt_property *pro = NULL;
    struct modem_pintrl_cfg *pintrl_cfg = (struct modem_pintrl_cfg *)SHM_MEM_MODEM_PINTRL_ADDR;
    struct pintrl_stru *cur_addr = (struct pintrl_stru *)(SHM_MEM_MODEM_PINTRL_ADDR + sizeof(struct modem_pintrl_cfg));

    memset(SHM_MEM_MODEM_PINTRL_ADDR, 0, SHM_MEM_MODEM_PINTRL_SIZE);

	/* let's give it all the room it could need */
	ret = fdt_open_into(fdt, (void*)fdt, total_space);
	if (ret < 0){
		PRINT_ERROR("Could not open modem dts, fdt=0x%x ret=0x%x.\n", fdt, ret);
		return ret;
    }

	/* Get offset of the chosen node */
	ret = fdt_path_offset(fdt, "/modem_pintrl");
	if (ret < 0) {
		PRINT_ERROR("Could not find modem_pintrl node, fdt=0x%x ret=0x%x.\n", fdt, ret);
		return ret;
	}

	offset = ret;

	/* Get property of the chosen node */
    pro = fdt_get_property(fdt, offset, (const char*)"pinctrl-num",&len);
    if((int)pro <= 0){
        PRINT_ERROR("Could not get property, pro=0x%x fdt=0x%x offset=0x%x len=0x%x.\n", pro, fdt, offset, len);
        return -1;
    }

    data = (int*)pro->data;

    for(i = 0;i < MODEM_PIN_BUTT;i++)
    {
        pintrl_cfg->pintrl_size[i] = cpu_to_fdt32(data[i]);

        snprintf(pintrl_name,16,"pinctrl-%d",i);

        pro = fdt_get_property(fdt, offset, pintrl_name, &len);
        if((int)pro <= 0){
            PRINT_ERROR("Could not get property, pro=0x%x fdt=0x%x offset=0x%x len=0x%x.\n", pro, fdt, offset, len);
            return -1;
        }

        if(0 != i)
        {
            cur_addr += pintrl_cfg->pintrl_size[i - 1];
        }

        pintrl_cfg->pintrl_addr[i] = cur_addr;

        memcpy(pintrl_cfg->pintrl_addr[i], pro->data, pintrl_cfg->pintrl_size[i] * sizeof(struct pintrl_stru));

        pintrl_data_convert(pintrl_cfg->pintrl_addr[i], pintrl_cfg->pintrl_size[i]);

    }

    modem_ldo_io_mux();

    pintrl_config(pintrl_cfg->pintrl_addr[MODEM_PIN_INIT], pintrl_cfg->pintrl_size[MODEM_PIN_INIT]);

	fdt_pack(fdt);

	return 0;
}

#else

int modem_pintrl_dts_parse(void *fdt)
{
    return 0;
}

#endif

void icc_shared_flag_clear(void)
{
	memset((void *)SHM_MEM_ICC_ADDR, 0, SHM_MEM_ICC_SIZE);
}

int save_hw_version(void)
{
	//lint --e{835}
	unsigned int *hw_addr = (unsigned int *)SHM_MEM_HW_VER_ADDR;

	unsigned int modem_id;

#ifdef BSP_CONFIG_K3V3_SFT

	modem_id = 0x33000100;/*FPGA 形态默认返回EVB5扣板ID*/

#endif

#ifdef BSP_CONFIG_K3V3_ASIC/*后续需要加上UDP与产品板区别*/

	if (UDP_BOARD_BOARDID == get_boardid())
	{
	/*获取HKADC4和HKADC5的值*/
		unsigned int hkadc4_val = get_hkadc_convert_value(ADC_ADCIN4);
		unsigned int hkadc5_val = get_hkadc_convert_value(ADC_ADCIN5);

		modem_id = HW_VER_K3V3_UDP_MASK | hkadc4_val | hkadc5_val<<8;
	}
	else
	{
		modem_id = get_productid();
	}

	//board_id = HW_VERSION;/*for 回片v9r1扣板*/
	//board_id = HW_VER_K3V3_RF_HIBV7R2RF7;/*for V7R2扣板*/
#endif

	*hw_addr = modem_id;

	return 0;
}

 void modem_sync_init(void)
 {
     memset((void*)SHM_MEM_SYNC_ADDR,0,SHM_MEM_SYNC_SIZE);
 }

 void pastar_shmem_init(void)
 {
	unsigned int i;
	for(i = 0; i < SHM_MEM_PASTAR_DPM_INFO_SIZE;i += 4)
    {
		writel(0,SRAM_PASTAR_DPM_INFO + i);
	}
 }

void modem_memrepair_init(void)
{
	unsigned int i;
	for(i=0;i<SHM_MEM_MEMREPAIR_SIZE;){
		writel(MR_POWERON_FLAG,SHM_MEM_MEMREPAIR_ADDR+i);
		i+=4;
	}
}

 #ifdef BSP_CONFIG_BOARD_SFT

void startup_modem(void)
{
	if(0 == modem_addr)
	{
		PRINT_INFO("modem img is not loaded...\n");
		return;
	}

	PRINT_INFO("startup modem...\n");
	writel(modem_addr >> 16,PERI_CTRL11);
	writel(BIT_RST_MODEM,PERI_CRG_RSTDIS4);
	writel((BIT_RST_MCPU2DDR | BIT_RST_MBUS2BUS),PERI_CRG_RSTDIS0);
	mdelay(10);
	writel((MDMA9_PD_SRST_DIS | MDMA9_CPU_SRST_EN),CRG_SRSTDIS1);
}
#elif defined(BSP_CONFIG_BOARD_UDP)

#define    START_MODEM_SYSCTRL_ADDR     0xE0200000
#define    HI_CRG_CLKEN3_OFFSET                              (0x18) /* 时钟使能3 */
#define    HI_CRG_CLKEN4_OFFSET                              (0x24) /* 时钟使能4 */
#define    HI_CRG_SRSTDIS1_OFFSET                            (0x64) /* 软复位请求关闭1 */
#define    HI_CRG_CLKDIV2_OFFSET                             (0x104) /* 时钟分频控制2(总线分频） */
#define    HI_CRG_A9PLL_CFG0_OFFSET                          (0x200) /* A9PLL配置寄存器0 */
#define    HI_CRG_A9PLL_CFG1_OFFSET                          (0x204) /* A9PLL配置寄存器1 */
#define    HI_CRG_DSPPLL_CFG0_OFFSET                         (0x20C) /* BBEPLL配置寄存器0 */
#define    HI_CRG_DSPPLL_CFG1_OFFSET                         (0x210) /* BBEPLL配置寄存器1 */
#define    HI_CRG_BBPPLL_CFG0_OFFSET                         (0x224) /* BBPPLL配置寄存器0 */
#define    HI_CRG_BBPPLL_CFG1_OFFSET                         (0x228) /* BBPPLL配置寄存器1 */
#define    HI_PWR_CTRL5_OFFSET                               (0xC14) /* ISOCELL隔离关闭控制寄存器 */
#define    HI_PWR_CTRL6_OFFSET                               (0xC18) /* MTCMOS上电使能控制寄存器 */
#define    HI_PWR_STAT1_OFFSET                               (0xE04) /* MTCMOS_RDY状态指示寄存器 */

void startup_modem(void)
{
}

void startup_modem_processor(unsigned modem_start_addr)
{
    u32 tmp = 0;

	if(0 == modem_start_addr)
	{
		PRINT_ERROR("modem img is not loaded...\n");
		return;
	}

    /* 1 2 mtcmos power up and wait for complete */
    writel(0x1<<1, START_MODEM_SYSCTRL_ADDR + HI_PWR_CTRL6_OFFSET);

    do
	{
		tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_PWR_STAT1_OFFSET);
		tmp = (tmp >> 1) & 0x1;
	}while(!tmp);

    /* 3 4  5 a9 pll */
	/* a9 pll : 400
    tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_A9PLL_CFG0_OFFSET);
    tmp &= 0xFFFFFFFC;
    tmp |= 0x1;
    writel(tmp, START_MODEM_SYSCTRL_ADDR + HI_CRG_A9PLL_CFG0_OFFSET);
    */
    // a9 pll: 667
    tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_A9PLL_CFG1_OFFSET);
    tmp &= 0xFF000000;
    tmp |= 0x00600000;
    writel(tmp, START_MODEM_SYSCTRL_ADDR + HI_CRG_A9PLL_CFG1_OFFSET);
    writel(0x00A04505, START_MODEM_SYSCTRL_ADDR + HI_CRG_A9PLL_CFG0_OFFSET);

    PRINT_ERROR("startup_modem 6\n");

    do
	{
		tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_A9PLL_CFG0_OFFSET);
		tmp = (tmp >> 26) & 0x1;
	}while(!tmp);

    tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_A9PLL_CFG1_OFFSET);
    tmp |= (0x1<<26);
    writel(tmp, START_MODEM_SYSCTRL_ADDR + HI_CRG_A9PLL_CFG1_OFFSET);

    /* 6 7 8 dsp pll:400*/
    tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_DSPPLL_CFG0_OFFSET);
    tmp &= 0xFFFFFFFC;
    tmp |= 0x1;
    writel(tmp, START_MODEM_SYSCTRL_ADDR + HI_CRG_DSPPLL_CFG0_OFFSET);

    do
	{
		tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_DSPPLL_CFG0_OFFSET);
		tmp = (tmp >> 26) & 0x1;
	}while(!tmp);

    tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_DSPPLL_CFG1_OFFSET);
    tmp |= (0x1<<26);
    writel(tmp, START_MODEM_SYSCTRL_ADDR + HI_CRG_DSPPLL_CFG1_OFFSET);

    /* 9 10 11 bbp pll:491*/
    tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_BBPPLL_CFG0_OFFSET);
    tmp &= 0xFFFFFFFC;
    tmp |= 0x1;
    writel(tmp, START_MODEM_SYSCTRL_ADDR + HI_CRG_BBPPLL_CFG0_OFFSET);

    do
	{
		tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_BBPPLL_CFG0_OFFSET);
		tmp = (tmp >> 26) & 0x1;
	}while(!tmp);

    tmp = readl(START_MODEM_SYSCTRL_ADDR + HI_CRG_BBPPLL_CFG1_OFFSET);
    tmp |= (0x1<<26);
    writel(tmp, START_MODEM_SYSCTRL_ADDR + HI_CRG_BBPPLL_CFG1_OFFSET);

    /* 12  a9 ref_clk enable*/
    writel(0x3<<24, START_MODEM_SYSCTRL_ADDR + HI_CRG_CLKEN3_OFFSET);
    /* 13 bus div: 0,0,3*/
    writel(0x40040300, START_MODEM_SYSCTRL_ADDR + HI_CRG_CLKDIV2_OFFSET);

    /* 14 */
    writel(0x40010300, START_MODEM_SYSCTRL_ADDR + HI_CRG_CLKDIV2_OFFSET);

    /* 15 enable ap and L2 clk */
    writel(0x3<<3, START_MODEM_SYSCTRL_ADDR + HI_CRG_CLKEN4_OFFSET);
    /* 16 a9 start addr */
    writel(modem_start_addr >> 16,0xE8A09C18);
    /* 17 disable iso */
    writel(0x1<<1, START_MODEM_SYSCTRL_ADDR + HI_PWR_CTRL5_OFFSET);
    /* 18 */
    writel(0x1<<15, START_MODEM_SYSCTRL_ADDR + HI_CRG_SRSTDIS1_OFFSET);
    writel(0x1<<6, START_MODEM_SYSCTRL_ADDR + HI_CRG_SRSTDIS1_OFFSET);
}
#else
#error "error board config."
#endif
int load_modem()
{
	unsigned offset = 0;        /* offset of read, must be multiple of 512 */
	unsigned bytes;         /* the data size of read, 0 means read all partition */
	struct ptentry *ptn;
	void *img_addr;
	unsigned char *img_head;
	unsigned head_size;
	struct image_head *head;
	int ret;

    /*lint -save -e570*/
    writel(LOAD_MODEM_ERROR_FLAG, SHM_MEM_LOADM_ADDR);
    /*lint -restore*/

	head_size = sizeof(struct image_head);
	img_head = alloc(EMMC_BLOCK_SIZE);
	if (NULL == img_head) {
		PRINT_ERROR("load_modem: alloc error.\n");
		return -1;
    }

	char *str_modem = "modem";
	ptn = find_ptn(str_modem);
	if (0 == ptn) {
		PRINT_ERROR("can't find ptn modem\n");
		return -1;
	}

	/* read head info */
	bytes = EMMC_BLOCK_SIZE;
	if (flash_read(ptn, offset, (void *)img_head, &bytes)) {
		PRINT_ERROR("read modem fail\n");
		return -1;
	}

	/* copy data to dest */
	//lint --e{826}
	head = (struct image_head*)img_head;

	/* it will overlap head_size ahead of load_addr, does it mater??? */
	img_addr = (void *)(head->load_addr - head_size);
	bytes = head->image_length + head_size;
	bytes = (bytes < ptn->length*EMMC_BLOCK_SIZE) ? bytes : ptn->length*EMMC_BLOCK_SIZE;
	if (bytes % 64 != 0)
		bytes = bytes + 64 - (bytes % 64);
	offset = 0;

	/* read the whole modem image to img_addr.
	this will load image head to load_addr - head_size, and
	image data to load_addr. */
	if (flash_read(ptn, offset, (void *)img_addr, &bytes)) {
		PRINT_ERROR("read modem fail\n");
		return -1;
	}

    /*lint -save -e570*/
    writel(LOAD_MODEM_OK_FLAG, SHM_MEM_LOADM_ADDR);
    writel(head->load_addr, SHM_MEM_LOADM_ADDR + 4);
    writel(head->image_length, SHM_MEM_LOADM_ADDR + 8);
    /*lint -restore*/

	/* change to use RAM verification. */
#ifdef HISI_SECBOOT
	ret = hisi_secboot_preload_verification((UINT32)img_addr, ptn->name, SECBOOT_USE_DEFAULT_MAGICNUM);
	if (SECBOOT_RET_SUCCESS != ret) {
		PRINT_ERROR("%s: secure verification error, hold system!\n", __func__);
		hisi_secboot_hold_system();
	}
#endif
	modem_addr = head->load_addr;

	return 0;
}

int load_modem_dsp()
{
	unsigned offset = 0;        /* offset of read, must be multiple of 512 */
	unsigned bytes;         /* the data size of read, 0 means read all partition */
	struct ptentry *ptn;
	char *img_addr;
	void *dest_addr;

	unsigned head_size = sizeof(struct image_head);
	char *str_modem = "modem_dsp";

	img_addr = alloc(DSP_PARTITON_SIZE);
	ptn = find_ptn(str_modem);
	if (0 == ptn) {
		PRINT_ERROR("can't find ptn modem dsp\n");
		return -1;
	}

	bytes = DSP_PARTITON_SIZE;
	if (flash_read(ptn, offset, (void *)img_addr, &bytes)) {
		PRINT_ERROR("read modem dsp fail\n");
		return -1;
	}

	/*load dsp to DDR_TLPHY_IMAGE_ADDR*/
	//lint --e{835}
	dest_addr = (void *)DDR_TLPHY_IMAGE_ADDR;
	memcpy(dest_addr, (char *)(img_addr + head_size), MODEM_DSP_SIZE);

	/* load TDS to DDR_LPHY_SDR_ADDR+1.75MB */
	//lint --e{835}
	dest_addr = (void *)(DDR_LPHY_SDR_ADDR + MODEM_DSP_SIZE + MODEM_TDS_SIZE);
	memcpy(dest_addr, (char *)(img_addr + head_size + MODEM_DSP_SIZE), MODEM_TDS_SIZE);

	return 0;
}

void fastboot_set_modem_cshell()
{
	set_modem_cshell_mod(PRT_FLAG_EN_MAGIC_M);
}

/* set socp encsrc chan for security */
void set_socp_sec_config(void)
{
    /* set 4-9 12-31 encsrc chan to secure access */
    writel(0xfffff3f0, HI_SOCP_REGBASE_ADDR + 0x30);
}
void set_has_l2cache_flag(void)
{
	unsigned int ret = 0;
    ST_PWC_SWITCH_STRU g_nv_pwc_switch;

    /*默认L2CACHE是打开*/
    *(unsigned int *)(SHM_MEM_CCORE_L2CACHE_ADDR) = 0x0;
    ret = bsp_nvm_read(NV_ID_DRV_NV_PWC_SWITCH, (u8 *)(&g_nv_pwc_switch), sizeof(ST_PWC_SWITCH_STRU));
    if(ret!=0)
    {
        PRINT_ERROR("NV_ID_DRV_NV_PWC_SWITCH:D10B read failure!\n");
        return;
    }

    /*l2cache_mntn = 1，标记L2CACHE关闭*/
    if(g_nv_pwc_switch.l2cache_mntn){
        *(unsigned int *)(SHM_MEM_CCORE_L2CACHE_ADDR)= 0x1;
    }
    return;
}
void modem_init(void)
{
    unsigned int tmp;

    tmp = readl(0xFFF35050);
    tmp |= 0x804;
    writel(0x804, 0xFFF35050);

    tmp = readl(0xFFF35064);
    tmp |= 0x18000000;
    writel(0x18000000, 0xFFF35064);

    tmp = readl(0xFFF35094);
    tmp |= 0x10000000;
    writel(0x10000000, 0xFFF35094);

    tmp = readl(0xFFF31380);
    tmp &= 0xFFFFFFF7;
    writel(tmp, 0xFFF31380);

    do
	{
		tmp = readl(0xFFF31388);
		tmp = (tmp >> 3) & 0x1;
	}while(tmp);

    do
	{
		tmp = readl(0xFFF31384);
		tmp = (tmp >> 3) & 0x1;
	}while(tmp);

    /* sec */
    writel(0x0, 0xe0200f04);
}

int modem_main()
{
    unsigned int cshell_nv = 0;

	PRINT_ERROR("modem_main start\n");

#ifdef BSP_CONFIG_BOARD_SFT
	/* uart io_share*/
    *(unsigned int*)(0xfff0a20c)=0x4050;
#endif

	/* icc核间共享标识清零 */
	icc_shared_flag_clear();

    /*modem nv xml解析依赖版本号 ，获取版本号需要提前 yuyangyang*/
    save_hw_version();
    /*modem nv初始化，加载nv数据到共享内存，解析xml*/
	modem_nv_init();

    /*必须放置NV初始化后*/
    set_has_l2cache_flag();

	modem_sync_init();
	modem_memrepair_init();
    pastar_shmem_init();
    //modem_io_mux();

    set_socp_sec_config();
    if(bsp_nvm_read(NV_ID_DRV_UART_SHELL_FLAG, (u8 *)&cshell_nv, sizeof(cshell_nv)))
    {
        PRINT_ERROR("read NV error\n");
        cshell_nv |= (0x1UL << 8);
    }
	(void)set_modem_cshell_mod(cshell_nv & (0x1UL << 8) ? PRT_FLAG_EN_MAGIC_M : MODEM_CSHELL_DEFAULT);

	/* 升级模式，不加载modem和dsp镜像 */
	if(is_load_modem())
	{
		if (load_modem()) {
			PRINT_ERROR("load modem img error\n");
			return -1;
		}
		PRINT_DEBUG("load modem img OK\n");

		if (load_modem_dsp()) {
			PRINT_ERROR("load modem dsp img error\n");
			//lint --e{570}
			writel(LOAD_DSP_ERROR_FLAG, SHM_MEM_DSP_FLAG_ADDR);
			return -1;
		}

		PRINT_DEBUG("load modem dsp img OK\n");
		writel(LOAD_DSP_OK_FLAG, SHM_MEM_DSP_FLAG_ADDR);

	}
	PRINT_ERROR("modem_main end\n");
	return 0;
}

void oem_modem_l2cache_func(const char* sub_cmd, char** details)
{
    unsigned count = 0;
    unsigned value = 0;

extern unsigned char binArrayStart[];
extern unsigned char binArrayEnd[];

    writel(0xA7A7A7A7, SHM_MEM_LOADM_ADDR);

    modem_init();

    memcpy(MCORE_TEXT_START_ADDR_COMPRESSED, binArrayStart, binArrayEnd - binArrayStart);

    startup_modem_processor(MCORE_TEXT_START_ADDR_COMPRESSED);

    do
    {
        count++;
        mdelay(1);
        value = readl(SHM_MEM_LOADM_ADDR);
    } while ((value != 0xA5A5A5A5) && (count < 3000));

    if (value == 0xA5A5A5A5)
        *details = "OKAY";
    else
        *details = "FAILl2cachenotavailable";

    return;
}

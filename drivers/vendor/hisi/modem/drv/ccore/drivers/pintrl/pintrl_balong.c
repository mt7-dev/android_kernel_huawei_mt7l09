#include <osl_common.h>
#include "bsp_shared_ddr.h"
#include "bsp_memmap.h"
#include "gpio_balong.h"
#include "bsp_om.h"

#include "pintrl_balong.h"

#define GPIO_DIR(x)		    (HI_GPIO0_REGBASE_ADDR + (x) * 0x1000 + 0x400)
#define GPIO_SYS_DIR(x)		(HI_GPIO22_REGBASE_ADDR + (x - 22) * 0x1000 + 0x400)
#define GPIO_DATA(x, y)		(HI_GPIO0_REGBASE_ADDR + (x) * 0x1000 + (1 << (2 + y)))
#define GPIO_SYS_DATA(x, y)	(HI_GPIO22_REGBASE_ADDR + (x - 22) * 0x1000 + (1 << (2 + y)))
#define GPIO_BIT(x, y)		((x) << (y))
#define GPIO_IS_SET(x, y)	(((x) >> (y)) & 0x1)

#define  ios_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IOS, "[ios]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  ios_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_IOS, "[ios]: "fmt, ##__VA_ARGS__))


struct modem_pintrl_cfg
{
    struct pintrl_stru *pintrl_addr[MODEM_PIN_BUTT];
    unsigned int pintrl_size[MODEM_PIN_BUTT];
};

/*void pintrl_data_convert(PINTRL_STRU *iocfg_stru, PINTRL_STRU *iocfg_table)
{
    iocfg_stru->ugpiog    = cpu_to_fdt32(iocfg_table->ugpiog);
    iocfg_stru->ugpio_bit = cpu_to_fdt32(iocfg_table->ugpio_bit);
    iocfg_stru->uiomg_off = cpu_to_fdt32(iocfg_table->uiomg_off);
    iocfg_stru->iomg_val  = cpu_to_fdt32(iocfg_table->iomg_val);
    iocfg_stru->uiocg_off = cpu_to_fdt32(iocfg_table->uiocg_off);
    iocfg_stru->iocg_val  = cpu_to_fdt32(iocfg_table->iocg_val);
    iocfg_stru->gpio_dir  = cpu_to_fdt32(iocfg_table->gpio_dir);
    iocfg_stru->gpio_val  = cpu_to_fdt32(iocfg_table->gpio_val);
}*/

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

int is_in_modem_pintrl(unsigned int gpio)
{
    unsigned int j = 0;
    
    unsigned int group = 0;
    unsigned int pin = 0;
    
    unsigned int mode = MODEM_PIN_NORMAL;

    struct pintrl_stru *pincfg = NULL;
    struct modem_pintrl_cfg *pintrl_cfg = (struct modem_pintrl_cfg *)SHM_MEM_MODEM_PINTRL_ADDR;

    group = gpio / GPIO_MAX_NUMBER;
    pin = gpio % GPIO_MAX_NUMBER;

    if(NULL == pintrl_cfg->pintrl_addr[mode])
    {
        return -1;
    }

    pincfg = pintrl_cfg->pintrl_addr[mode];

    for(j = 0;j < pintrl_cfg->pintrl_size[mode];j++)
    {
        if(group == pincfg->ugpiog && (pin == pincfg->ugpio_bit))
        {
            return 0;

        }
        ++pincfg;
    }

    return -1;
    
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
        ios_print_error("para is error, gpio = %d, type = %d, value = %d.\n", gpio, type, value);        
        return ;
    }

    group = gpio / GPIO_MAX_NUMBER;
    pin = gpio % GPIO_MAX_NUMBER;
    
    init_stru = get_pintrl(MODEM_PIN_INIT, group, pin);
    if(NULL == init_stru)
    {
        ios_print_error("get_pintrl is fail, init_stru = 0x%x.\n", init_stru);  
        return ;
    }
    
    normal_stru = get_pintrl(MODEM_PIN_NORMAL, group, pin);
    if(NULL == normal_stru)
    {
        ios_print_error("get_pintrl is fail, normal_stru = 0x%x.\n", normal_stru);  
        return ;
    }
    
    lowpower_stru = get_pintrl(MODEM_PIN_POWERDOWN, group, pin);
    if(NULL == lowpower_stru)
    {
        ios_print_error("get_pintrl is fail, lowpower_stru = 0x%x.\n", lowpower_stru);  
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
            ios_print_error("type is error, type = %d.\n", type);
            return ;
        
    }

    pintrl_config(normal_stru, 1);
}


int bsp_get_pintrl(unsigned int type, unsigned int gpio, unsigned int *value)
{
    int ret = 0;
    unsigned int group = 0;
    unsigned int pin = 0;
    struct pintrl_stru *normal_stru = NULL;

    if(gpio > GPIO_TOTAL || (type >= MODEM_IO_BUTT) || (NULL == value))
    {
        ios_print_error("para is error, gpio = %d, type = %d, value = %d.\n", gpio, type, value);        
        return -1;
    }

    group = gpio / GPIO_MAX_NUMBER;
    pin = gpio % GPIO_MAX_NUMBER;

    normal_stru = get_pintrl(MODEM_PIN_NORMAL, group, pin);
    if(NULL == normal_stru)
    {
        ios_print_error("get_pintrl is fail, normal_stru = 0x%x.\n", normal_stru);  
        return -1;
    }

    switch(type)
    {
        case MODEM_IO_MUX:
            *value   = normal_stru->iomg_val;
            ret = 0;
            break;
            
        case MODEM_IO_CFG:
            *value   = normal_stru->iocg_val;
            ret = 0;
            break;
            
        default:
            ios_print_error("type is error, type = %d.\n", type);
            ret = -1;
    }

    return ret;
}


#if 0
/* Top level function that updates the device tree. */
int modem_pintrl_dts_parse(int mode, PINTRL_STRU **iocfg_table, unsigned int *length)
{
	int ret;
    int len;
	int offset;

    unsigned int fdt = DDR_DTS_ADDR;
	int total_space = 0x40000;
    int *data = NULL;
    
    char pintrl_name[20]={0};
    const struct fdt_property *pro = NULL;

	/* let's give it all the room it could need */
	ret = fdt_open_into((void*)fdt, (void*)fdt, total_space);
	if (ret < 0){
		ios_print_error("Could not open modem dts, fdt=0x%x ret=0x%x.\n", fdt, ret);
		return ret;
    }

	/* Get offset of the chosen node */
	ret = fdt_path_offset((void*)fdt, "/modem_pintrl");
	if (ret < 0) {
		ios_print_error("Could not find modem_pintrl node, fdt=0x%x ret=0x%x.\n", fdt, ret);
		return ret;
	}

	offset = ret;
    
	/* Get property of the chosen node */
    pro = fdt_get_property((void*)fdt, offset, (const char*)"pinctrl-num",&len);
    if((int)pro < 0){
        ios_print_error("Could not get property, pro=0x%x fdt=0x%x offset=0x%x len=0x%x.\n", pro, fdt, offset, len);
        return -1;
    }
    
    data = (int*)pro->data;
    *length = data[mode];

    snprintf(pintrl_name,20,"pinctrl-%d",mode);
    
	/* Adding the cmdline to the chosen node */
    pro = fdt_get_property((void*)fdt, offset, pintrl_name,&len);
    if((int)pro < 0){
        ios_print_error("Could not get property, pro=0x%x fdt=0x%x offset=0x%x len=0x%x.\n", pro, fdt, offset, len);
        return -1;
    }

    *iocfg_table = (PINTRL_STRU*)pro->data;
    
	fdt_pack((void*)fdt);

	return 0;
}

#endif

void modem_pintrl_debug(int mode)
{
    unsigned int i = 0;    
    unsigned int length = 0;
    struct pintrl_stru *pintrl_stru = NULL;
    struct modem_pintrl_cfg *pintrl_cfg = (struct modem_pintrl_cfg *)SHM_MEM_MODEM_PINTRL_ADDR;    

    if((MODEM_PIN_INIT != mode) && (MODEM_PIN_NORMAL != mode) && (MODEM_PIN_POWERDOWN != mode))
    {
        ios_print_error("modem pintrl mode is error, mode=%d.\n", mode);
        return ;
    }
    
    length = pintrl_cfg->pintrl_size[mode];
    pintrl_stru = pintrl_cfg->pintrl_addr[mode];
    /*ret = modem_pintrl_dts_parse(mode, &iocfg_table, &length);
    if(0 != ret)
    {
        ios_print_error("modem pintrl parse is fail, ret=0x%x mode=%d.\n", ret, mode);
        return;
    }*/
    
    ios_print_info("iocfg table length is %d\n", length);
    ios_print_info("  group  pin  iomg  iocg  dir  value\n");
    
    for(i = 0;i < length;i++)
    {
        //pintrl_data_convert(&iocfg_stru, &(iocfg_table[i]));

        ios_print_info("%5d %5d %5d %5d %5d %5d\n", pintrl_stru[i].ugpiog,pintrl_stru[i].ugpio_bit,\
                                                            pintrl_stru[i].iomg_val,pintrl_stru[i].iocg_val,\
                                                            pintrl_stru[i].gpio_dir,pintrl_stru[i].gpio_val);
                                                          

    }

}

void modem_pintrl_config(int mode)
{
    //int ret = 0;
    //unsigned int length = 0;
    
    struct modem_pintrl_cfg *pintrl_cfg = (struct modem_pintrl_cfg *)SHM_MEM_MODEM_PINTRL_ADDR;

    if((MODEM_PIN_INIT != mode) && (MODEM_PIN_NORMAL != mode) && (MODEM_PIN_POWERDOWN != mode))
    {
        ios_print_error("modem pintrl mode is error, mode=%d.\n", mode);
        return ;
    }

    /*ret = modem_pintrl_dts_parse(mode, &iocfg_table, &length);
    if(0 != ret)
    {
        ios_print_error("modem pintrl parse is fail, ret=0x%x mode=%d.\n", ret, mode);
        return;
    }*/
    
    pintrl_config(pintrl_cfg->pintrl_addr[mode], pintrl_cfg->pintrl_size[mode]);

}


/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  gpio_expander.c
*
*   作    者 :  liuwenhui
*
*   描    述 :  GPIO驱动文件
*
*   修改记录 :  2012年11月27日
*
*************************************************************************/
/* includes */
#include "boot/boot.h"
#include "types.h"
#include "osl_common.h"
#include "bsp_memmap.h"
#include "bsp_om.h"
#include "bsp_i2c.h"
#include "gpio.h"

#define PCA953X_INPUT		0
#define PCA953X_OUTPUT		1
#define PCA953X_INVERT		2
#define PCA953X_DIRECTION	3

#define PCA957X_IN		    0
#define PCA957X_INVRT		1
#define PCA957X_BKEN		2
#define PCA957X_PUPD		3
#define PCA957X_CFG		    4
#define PCA957X_OUT		    5
#define PCA957X_MSK		    6
#define PCA957X_INTS		7

#define PCA_GPIO_MASK		0x00FF
#define PCA_INT			    0x0100
#define PCA953X_TYPE		0x1000
#define PCA957X_TYPE		0x2000

//#define GPIO_MAX_PINS       32

#define EXPANDER_GPIO_START 128

#define EXPANDER_GPIO_END   160

#define EXPANDER_GROUP_1_2  8

#define EXPANDER_GROUP_3    16

#define GPIO_I2C_EXPANDER_NUM 1

#define  gpio_print_error(fmt, ...)    (cprintf("[gpio_ex]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  gpio_print_info(fmt, ...)     (cprintf("[gpio_ex]: "fmt, ##__VA_ARGS__))


struct gpio_expander_chip
{
    unsigned int slave_addr;
    unsigned int chip_type;
    unsigned int gpio_base;
    unsigned int ngpio;
	unsigned short reg_output;
	unsigned short reg_direction;
};


struct gpio_expander_chip gpio_ex_chip[] = {
		{/* v711 udp */
        	.slave_addr = 0x21,
            .chip_type = PCA957X_TYPE,
            .gpio_base = 128,
        	.ngpio = 8,
        	.reg_output= 0,
        	.reg_direction= 0,
		},
		{
        	.slave_addr = 0x20,
            .chip_type = PCA957X_TYPE,
            .gpio_base = 136,
        	.ngpio = 8,
        	.reg_output= 0,
        	.reg_direction= 0,
		},
#if 0
		{/* v711 udp e5*/
        	.slave_addr = 0x24,
            .chip_type = PCA953X_TYPE,
            .gpio_base = 144,
        	.ngpio = 16,
        	.reg_output= 0,
        	.reg_direction= 0,
		},
#endif
};


int pca953x_write_reg(struct gpio_expander_chip *chip, int reg, unsigned short val)
{
	int ret = 0;

	if (chip->ngpio <= 8)
		ret = bsp_i2c_byte_data_send(chip->slave_addr, reg, val);
	else {
		switch (chip->chip_type) {
		case PCA953X_TYPE:
			ret = bsp_i2c_word_data_send(chip->slave_addr,
							reg << 1, val);
			break;
		case PCA957X_TYPE:
			ret = bsp_i2c_byte_data_send(chip->slave_addr, reg << 1,
							val & 0xff);
			if (ret < 0)
				break;
			ret = bsp_i2c_byte_data_send(chip->slave_addr,
							(reg << 1) + 1,
							(val & 0xff00) >> 8);
			break;
		}
	}

	if (ret < 0) {
		gpio_print_error("failed writing register, ret=%d\n");
		return ret;
	}

	return 0;
}

int pca953x_read_reg(struct gpio_expander_chip *chip, int reg, unsigned short *val)
{
	int ret;

	if (chip->ngpio <= 8)
		ret = bsp_i2c_byte_data_receive(chip->slave_addr, reg, val);
	else
		ret = bsp_i2c_word_data_receive(chip->slave_addr, reg << 1, val);

	if (ret < 0) {
		gpio_print_error("failed reading register, ret=%d.\n");
		return ret;
	}
    
	return 0;
}


int device_pca953x_init(struct gpio_expander_chip *chip, int invert)
{
	int ret;

	ret = pca953x_read_reg(chip, PCA953X_OUTPUT, &chip->reg_output);
	if (ret)
		goto out;

	ret = pca953x_read_reg(chip, PCA953X_DIRECTION,
			       &chip->reg_direction);
	if (ret)
		goto out;

	/* set platform specific polarity inversion */
	ret = pca953x_write_reg(chip, PCA953X_INVERT, invert);
out:
	return ret;
}

int device_pca957x_init(struct gpio_expander_chip *chip, int invert)
{
	int ret;
	unsigned short val = 0;

	/* Let every port in proper state, that could save power */
	pca953x_write_reg(chip, PCA957X_PUPD, 0x0);
	pca953x_write_reg(chip, PCA957X_CFG, 0xffff);
	pca953x_write_reg(chip, PCA957X_OUT, 0x0);

	ret = pca953x_read_reg(chip, PCA957X_IN, &val);
	if (ret)
		goto out;
	ret = pca953x_read_reg(chip, PCA957X_OUT, &chip->reg_output);
	if (ret)
		goto out;
	ret = pca953x_read_reg(chip, PCA957X_CFG, &chip->reg_direction);
	if (ret)
		goto out;

	/* set platform specific polarity inversion */
	pca953x_write_reg(chip, PCA957X_INVRT, invert);

	/* To enable register 6, 7 to controll pull up and pull down */
	pca953x_write_reg(chip, PCA957X_BKEN, 0x202);

	return 0;
out:
	return ret;
}


int gpio_ex_init(void)
{
	int ret = 0;
    int i = 0;
    struct gpio_expander_chip *chip = &gpio_ex_chip[0];
    
    //gpio_direction_output(88,1);
    
	//bsp_i2c_initial();

    for(i = 0;i < GPIO_I2C_EXPANDER_NUM; i++)
    {
        bsp_i2c_slave_register(I2C_MASTER0, chip[i].slave_addr);
    	if (chip[i].chip_type == PCA953X_TYPE)
    		ret = device_pca953x_init(&chip[i], 0);
    	else
    		ret = device_pca957x_init(&chip[i], 0);
    	if (ret){
            gpio_print_error("device_pca_init is error, ret=%d.\n",ret);
    		return -1;
        }

    }

    return 0;
}


int gpio_ex_direction_input(unsigned int gpio)
{
	struct gpio_expander_chip *chip = NULL;

	int ret, offset = 0;
    int group, off=0;
	unsigned short reg_val;

    if(gpio < EXPANDER_GPIO_START || gpio >= EXPANDER_GPIO_END)
    {
        gpio_print_error("para is error.\n", gpio);
        return -1;
    }
    
    if(gpio >= EXPANDER_GPIO_START && (gpio < EXPANDER_GPIO_START + EXPANDER_GROUP_1_2))
    {
        
        off = gpio - EXPANDER_GPIO_START;
        group = 0;
    }
    else if(gpio >= EXPANDER_GPIO_START + EXPANDER_GROUP_1_2 && (gpio < EXPANDER_GPIO_START + 2 * EXPANDER_GROUP_1_2))
    {
        off = gpio - EXPANDER_GPIO_START - EXPANDER_GROUP_1_2;
        group = 1;
    }
    else
    {
        off = gpio - EXPANDER_GPIO_START - 2 * EXPANDER_GROUP_1_2;
        group = 2;
    }
        
	chip = &gpio_ex_chip[group];


	reg_val = chip->reg_direction | (1u << off);

	switch (chip->chip_type) {
	case PCA953X_TYPE:
		offset = PCA953X_DIRECTION;
		break;
	case PCA957X_TYPE:
		offset = PCA957X_CFG;
		break;
	}
	ret = pca953x_write_reg(chip, offset, reg_val);
	if (ret)
		goto exit;

	chip->reg_direction = reg_val;
	ret = 0;
    
exit:
	return ret;
}

int gpio_ex_direction_output(unsigned int gpio, unsigned int val)
{
	struct gpio_expander_chip *chip = NULL;

	int ret, offset = 0;
    int group, off=0;
	unsigned short reg_val;

    if(gpio < EXPANDER_GPIO_START || gpio >= EXPANDER_GPIO_END)
    {
        gpio_print_error("para is error.\n", gpio);
        return -1;
    }
    
    if(gpio >= EXPANDER_GPIO_START && (gpio < EXPANDER_GPIO_START + EXPANDER_GROUP_1_2))
    {
        
        off = gpio - EXPANDER_GPIO_START;
        group = 0;
    }
    else if(gpio >= EXPANDER_GPIO_START + EXPANDER_GROUP_1_2 && (gpio < EXPANDER_GPIO_START + 2 * EXPANDER_GROUP_1_2))
    {
        off = gpio - EXPANDER_GPIO_START - EXPANDER_GROUP_1_2;
        group = 1;
    }
    else
    {
        off = gpio - EXPANDER_GPIO_START - 2 * EXPANDER_GROUP_1_2;
        group = 2;
    }
        
	chip = &gpio_ex_chip[group];
    
	if (val)
		reg_val = chip->reg_output | (1u << off);
	else
		reg_val = chip->reg_output & ~(1u << off);

	switch (chip->chip_type) {
	case PCA953X_TYPE:
		offset = PCA953X_OUTPUT;
		break;
	case PCA957X_TYPE:
		offset = PCA957X_OUT;
		break;
	}
	ret = pca953x_write_reg(chip, offset, reg_val);
	if (ret)
		goto exit;

	chip->reg_output = reg_val;

	/* then direction */
	reg_val = chip->reg_direction & ~(1u << off);
	switch (chip->chip_type) {
	case PCA953X_TYPE:
		offset = PCA953X_DIRECTION;
		break;
	case PCA957X_TYPE:
		offset = PCA957X_CFG;
		break;
	}
	ret = pca953x_write_reg(chip, offset, reg_val);
	if (ret)
		goto exit;

	chip->reg_direction = reg_val;
	ret = 0;
    
exit:
	return ret;
}

int gpio_ex_get_value(unsigned int gpio)
{
	struct gpio_expander_chip *chip = NULL;

	int ret, offset = 0;
    int group, off=0;
	unsigned short reg_val= 0;

    if(gpio < EXPANDER_GPIO_START || gpio >= EXPANDER_GPIO_END)
    {
        gpio_print_error("para is error.\n", gpio);
        return -1;
    }
    
    if(gpio >= EXPANDER_GPIO_START && (gpio < EXPANDER_GPIO_START + EXPANDER_GROUP_1_2))
    {
        
        off = gpio - EXPANDER_GPIO_START;
        group = 0;
    }
    else if(gpio >= EXPANDER_GPIO_START + EXPANDER_GROUP_1_2 && (gpio < EXPANDER_GPIO_START + 2 * EXPANDER_GROUP_1_2))
    {
        off = gpio - EXPANDER_GPIO_START - EXPANDER_GROUP_1_2;
        group = 1;
    }
    else
    {
        off = gpio - EXPANDER_GPIO_START - 2 * EXPANDER_GROUP_1_2;
        group = 2;
    }
        
	chip = &gpio_ex_chip[group];
    
	switch (chip->chip_type) {
	case PCA953X_TYPE:
		offset = PCA953X_INPUT;
		break;
	case PCA957X_TYPE:
		offset = PCA957X_IN;
		break;
	}
	ret = pca953x_read_reg(chip, offset, &reg_val);

	if (ret < 0) {
		/* NOTE:  diagnostic already emitted; that's all we should
		 * do unless gpio_*_value_cansleep() calls become different
		 * from their nonsleeping siblings (and report faults).
		 */
		return 0;
	}

	return (reg_val & (1u << off)) ? 1 : 0;
}

void gpio_ex_set_value(unsigned int gpio, unsigned int val)
{
	struct gpio_expander_chip *chip = NULL;

	int ret, offset = 0;
    int group, off=0;
	unsigned short reg_val;

    if(gpio < EXPANDER_GPIO_START || gpio >= EXPANDER_GPIO_END)
    {
        gpio_print_error("para is error.\n", gpio);
        return ;
    }
    
    if(gpio >= EXPANDER_GPIO_START && (gpio < EXPANDER_GPIO_START + EXPANDER_GROUP_1_2))
    {
        
        off = gpio - EXPANDER_GPIO_START;
        group = 0;
    }
    else if(gpio >= EXPANDER_GPIO_START + EXPANDER_GROUP_1_2 && (gpio < EXPANDER_GPIO_START + 2 * EXPANDER_GROUP_1_2))
    {
        off = gpio - EXPANDER_GPIO_START - EXPANDER_GROUP_1_2;
        group = 1;
    }
    else
    {
        off = gpio - EXPANDER_GPIO_START - 2 * EXPANDER_GROUP_1_2;
        group = 2;
    }
        
	chip = &gpio_ex_chip[group];
    
	if (val)
		reg_val = chip->reg_output | (1u << off);
	else
		reg_val = chip->reg_output & ~(1u << off);

	switch (chip->chip_type) {
	case PCA953X_TYPE:
		offset = PCA953X_OUTPUT;
		break;
	case PCA957X_TYPE:
		offset = PCA957X_OUT;
		break;
	}
	ret = pca953x_write_reg(chip, offset, reg_val);
	if (ret)
    {
        gpio_print_error("pca953x_write_reg is error, ret=%d.\n",ret);
		return ;

    }   

	chip->reg_output = reg_val;

}


void gpio_ex_show(void)
{   
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned short val = 0;
    unsigned int reg_max = 0;

    for(j = 0;j < GPIO_I2C_EXPANDER_NUM;j++)
    {
        gpio_print_info("slave 0x%x, type %d, base %d, ngpio %d\n", gpio_ex_chip[j].slave_addr, \
            gpio_ex_chip[j].chip_type, gpio_ex_chip[j].gpio_base, gpio_ex_chip[j].ngpio);

        gpio_print_info("direction = 0x%x, value = 0x%x.\n", gpio_ex_chip[j].reg_direction, gpio_ex_chip[j].reg_output);

        if(PCA953X_TYPE == gpio_ex_chip[j].chip_type)
        {
            reg_max = 4;
        }
        else
        {
            reg_max = 8;
        }

        for(i = 0; i < reg_max;i++)
        {
            if(0 == pca953x_read_reg(&gpio_ex_chip[j], i, &val))
            {
                gpio_print_info("reg%d=0x%x.\n", i, val);
            }
        }

    }

    
}

/*
if(memcmp(cmd, "expander", 8) == 0){
        extern int gpio_ex_direction_input(unsigned int gpio);
        extern int gpio_ex_direction_output(unsigned int gpio, unsigned int val);
        extern void gpio_ex_show();
        extern int gpio_ex_init();
        (void)gpio_ex_init();
        gpio_ex_show();
        gpio_ex_direction_input(128);
        gpio_ex_direction_output(135,1);
		gpio_ex_show();
        
	}
*/



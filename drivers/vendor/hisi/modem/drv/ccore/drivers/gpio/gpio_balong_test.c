#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <logLib.h>

#include "osl_bio.h"
#include "bsp_memmap.h"
#include "hi_gpio.h"
#include "gpio_balong.h"

/*#define CCORE_GPIO_IRQ_ENABLE*/
#ifndef HI_K3_GPIO
#define GPIO_TEST_NUM                          (GPIO_0_13)      /*GPIO_0_13*/
#else
#define GPIO_TEST_NUM                          179
#endif
#define GPIO_TEST_BASEADDR                     (HI_GPIO0_REGBASE_ADDR)
#define GPIO_BANK_BITS                         (0)
#define GPIO_PIN_BITS                          (13)

#define GPIO_INVALID_NUM                       (10000)

/*register address offset in gpio bank*/
 #define GPIO_DATA_OFFSET                      (0x00)
 #define GPIO_DIRECIONT_OFFSET                 (0x04)
 #define GPIO_INTEN					           (0x30)
 #define GPIO_INTMASK_OFFSET                   (0x34)
 #define GPIO_INTTYPE_LEVEL			           (0x38)
 #define GPIO_INT_PLOARITY                 	   (0x3C)
 #define GPIO_INTSTATE_OFFSET                  (0x40)
 #define GPIO_INTRAWSTATE_OFFSET               (0x44)
 #define GPIO_INTCLEAR_OFFSET                  (0x4c)
 #define GPIO_EXT_PORTA				           (0X50)

#define gpio_reg_set_bits(base, reg, pos, bits, val) (writel((readl(base + reg) & (~(((0x1 << bits) - 1) << pos))) | ((val & ((0x1 << bits) - 1)) << pos), base + reg))
#define gpio_reg_get_bits(base, reg, pos) ((readl(base + reg) & (0x1 << pos)) >> pos)

int test_bsp_gpio_suspend(void);
int test_bsp_gpio_resume(void);
int test_bsp_gpio_init(void);
int test_bsp_gpio_value_get_valid_value(void);
int test_bsp_gpio_value_set_valid_value(void);
int test_bsp_gpio_direction_get_valid_value(void);
int test_bsp_gpio_direction_output_valid_value(void);
int test_bsp_gpio_direction_input_valid_value(void);
int test_bsp_gpio_to_irq_valid_value(void);
int test_bsp_gpio_int_mask_set_valid_value(void);
int test_bsp_gpio_int_unmask_set_valid_value(void);
int test_bsp_gpio_set_function_valid_value(void);
int test_bsp_gpio_int_state_get_valid_value(void);
int test_bsp_gpio_int_state_clear_valid_value(void);
int test_bsp_gpio_raw_int_state_get_valid_value(void);
int test_bsp_gpio_int_trigger_set_valid_value(void);
int test_bsp_gpio_direction_output(void);
int test_bsp_gpio_output_get_value(void);
int test_bsp_gpio_set_value(void);
int test_bsp_gpio_to_irq(void);
int test_bsp_gpio_direction_get(void);
int test_bsp_gpio_direction_input(void);
int test_bsp_gpio_input_set_value(void);
int test_bsp_gpio_get_value(void);
int test_bsp_gpio_int_mask_set(void);
int test_bsp_gpio_int_trigger_set(void);
int test_bsp_gpio_set_function(void);
int test_bsp_gpio_int_unmask_set(void);
int test_bsp_gpio_int_state_get(void);
int test_bsp_gpio_int_state_clear(void);
int test_bsp_gpio_raw_int_state_get(void);

#ifdef CCORE_GPIO_IRQ_ENABLE
int test_bsp_gpio_direction_input(void)
{
	bsp_gpio_direction_input(GPIO_TEST_NUM);

	if(0 == gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_SWPORT_DDR_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_direction_input is passed!!\n",0,0,0,0,0,0);
		return GPIO_OK;
	}
	else
	{
		logMsg("Test_bsp_gpio_direction_input is fail!!\n",0,0,0,0,0,0);
        return GPIO_ERROR;
	}

}

int test_bsp_gpio_input_set_value(void)
{
    bsp_gpio_set_value(GPIO_TEST_NUM, 0);

    return GPIO_OK;
}

int test_bsp_gpio_direction_output(void)
{
	bsp_gpio_direction_output(GPIO_TEST_NUM,1);

	if(1 == gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_SWPORT_DDR_OFFSET,GPIO_PIN_BITS))
	{

		logMsg("Test_bsp_gpio_direction_output is passed!!\n",0,0,0,0,0,0);
        return GPIO_OK;
	}
	else
	{
		logMsg("Test_bsp_gpio_direction_output is fail!!\n",0,0,0,0,0,0);
        return GPIO_ERROR;
	}

}

int test_bsp_gpio_output_get_value(void)
{
    bsp_gpio_get_value(GPIO_TEST_NUM);

    return GPIO_OK;
}

int test_bsp_gpio_get_value(void)
{

	if(gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_EXT_PORT_OFFSET,GPIO_PIN_BITS) == bsp_gpio_get_value(GPIO_TEST_NUM))
	{
		logMsg("Test_bsp_gpio_get_value is passed!!\n",0,0,0,0,0,0);
        return GPIO_OK;
	}
	else
	{
		logMsg("Test_bsp_gpio_get_value is fail!!\n",0,0,0,0,0,0);
        return GPIO_ERROR;
	}

}

int test_bsp_gpio_set_value(void)
{
	bsp_gpio_set_value(GPIO_TEST_NUM,0);

	if(1 == gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_SWPORT_DR_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_set_value is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	bsp_gpio_set_value(GPIO_TEST_NUM,1);
	if(0 == gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_SWPORT_DR_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_set_value is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	logMsg("Test_bsp_gpio_set_value is passed!!\n",0,0,0,0,0,0);

    return GPIO_OK;

}



int test_bsp_gpio_direction_get(void)
{
	gpio_reg_set_bits(GPIO_TEST_BASEADDR,HI_GPIO_SWPORT_DDR_OFFSET,GPIO_PIN_BITS,1,0);

	if(1 == bsp_gpio_direction_get(GPIO_TEST_NUM))
	{
		logMsg("Test_bsp_gpio_direction_get is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	gpio_reg_set_bits(GPIO_TEST_BASEADDR,HI_GPIO_SWPORT_DDR_OFFSET,GPIO_PIN_BITS,1,1);
	if(0 == bsp_gpio_direction_get(GPIO_TEST_NUM))
	{
		logMsg("Test_bsp_gpio_direction_get is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}
	logMsg("Test_bsp_gpio_direction_get is passed!!\n",0,0,0,0,0,0);
    return GPIO_OK;
}



int test_bsp_gpio_int_mask_set(void)
{
	bsp_gpio_int_mask_set(GPIO_TEST_NUM);

	if(0 == gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTMASK_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_int_mask_set is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}
	logMsg("Test_bsp_gpio_int_mask_set is passed!!\n",0,0,0,0,0,0);
    return GPIO_OK;
}


int test_bsp_gpio_int_unmask_set(void)
{
	bsp_gpio_int_unmask_set(GPIO_TEST_NUM);

	if(1 == gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTMASK_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_int_unmask_set is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}
	logMsg("Test_bsp_gpio_int_unmask_set is passed!!\n",0,0,0,0,0,0);
    return GPIO_OK;
}


int test_bsp_gpio_set_function(void)
{
	bsp_gpio_set_function(GPIO_TEST_NUM,GPIO_INTERRUPT);
	if(GPIO_NORMAL == gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTEN_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_set_function is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	bsp_gpio_set_function(GPIO_TEST_NUM,GPIO_NORMAL);

	if(GPIO_INTERRUPT == gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTEN_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_set_function is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	logMsg("Test_bsp_gpio_set_function is passed!!\n",0,0,0,0,0,0);
    return GPIO_OK;
}


int test_bsp_gpio_int_state_get(void)
{
	if(gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTSTATUS_OFFSET,GPIO_PIN_BITS) == bsp_gpio_int_state_get(GPIO_TEST_NUM))
	{
		logMsg("Test_bsp_gpio_int_state_get is passed!!\n",0,0,0,0,0,0);
        return GPIO_OK;
	}
	else
	{
		logMsg("Test_bsp_gpio_int_state_get is fail!!\n",0,0,0,0,0,0);
        return GPIO_ERROR;
	}
}

/*¼Ä´æÆ÷*/
int test_bsp_gpio_int_state_clear(void)
{
	logMsg("gpio_int_state_clear is passed!!\n",0,0,0,0,0,0);
    return GPIO_OK;
}


int test_bsp_gpio_raw_int_state_get(void)
{
	if(gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_RAWINTSTATUS_OFFSET,GPIO_PIN_BITS) == bsp_gpio_raw_int_state_get(GPIO_TEST_NUM))
	{
		logMsg("test_gpio_int_state_get is passed!!\n",0,0,0,0,0,0);
        return GPIO_OK;
	}
	else
	{
		logMsg("test_gpio_int_state_get is fail!!\n",0,0,0,0,0,0);
        return GPIO_ERROR;
	}
}


int test_bsp_gpio_int_trigger_set(void)
{
	bsp_gpio_int_trigger_set(GPIO_TEST_NUM,1);
	if(1 != gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTTYPE_LEVEL_OFFSET,GPIO_PIN_BITS)
		&& 1 != gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INT_PLOARITY_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_int_trigger_set is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	bsp_gpio_int_trigger_set(GPIO_TEST_NUM,2);
	if(1 != gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTTYPE_LEVEL_OFFSET,GPIO_PIN_BITS)
		&& 0 != gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INT_PLOARITY_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_int_trigger_set is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	bsp_gpio_int_trigger_set(GPIO_TEST_NUM,4);
	if(0 != gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTTYPE_LEVEL_OFFSET,GPIO_PIN_BITS)
		&& 1 != gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INT_PLOARITY_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_int_trigger_set is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	bsp_gpio_int_trigger_set(GPIO_TEST_NUM,5);
	if(0 != gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INTTYPE_LEVEL_OFFSET,GPIO_PIN_BITS)
		&& 0 != gpio_reg_get_bits(GPIO_TEST_BASEADDR,HI_GPIO_INT_PLOARITY_OFFSET,GPIO_PIN_BITS))
	{
		logMsg("Test_bsp_gpio_int_trigger_set is fail!!\n",0,0,0,0,0,0);
		return GPIO_ERROR;
	}

	logMsg("Test_bsp_gpio_int_trigger_set is passed!!\n",0,0,0,0,0,0);

    return GPIO_OK;

}
#else
int test_bsp_gpio_direction_input(void)
{
	return GPIO_OK;
}

int test_bsp_gpio_input_set_value(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_direction_output(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_output_get_value(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_get_value(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_set_value(void)
{
    return GPIO_OK;
}



int test_bsp_gpio_direction_get(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_int_mask_set(void)
{
    return GPIO_OK;
}


int test_bsp_gpio_int_unmask_set(void)
{
    return GPIO_OK;
}


int test_bsp_gpio_set_function(void)
{
    return GPIO_OK;
}


int test_bsp_gpio_int_state_get(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_int_state_clear(void)
{
    return GPIO_OK;
}


int test_bsp_gpio_raw_int_state_get(void)
{

    return GPIO_OK;

}


int test_bsp_gpio_int_trigger_set(void)
{
    return GPIO_OK;

}

int test_bsp_gpio_suspend(void)
{
    return GPIO_ERROR;
}

int test_bsp_gpio_resume(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_init(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_direction_get_init(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_direction_input_init(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_direction_output_init(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_set_value_init(void)
{
    return GPIO_OK;
}

int test_gpio_get_value_init(void)
{
    return GPIO_OK;
}

int test_bsp_gpio_direction_get_invalid_num(void)
{
    bsp_gpio_direction_get(GPIO_INVALID_NUM);
    return GPIO_OK;
}

int test_bsp_gpio_direction_input_invalid_num(void)
{
    bsp_gpio_direction_input(GPIO_INVALID_NUM);
    return GPIO_OK;
}

int test_bsp_gpio_direction_output_invalid_num(void)
{
    bsp_gpio_direction_output(GPIO_INVALID_NUM,1);
    return GPIO_OK;
}

int test_bsp_gpio_set_value_invalid_num(void)
{
    bsp_gpio_set_value(GPIO_INVALID_NUM,1);
    return GPIO_OK;
}

int test_bsp_gpio_get_value_invalid_num(void)
{
    if(GPIO_ERROR == bsp_gpio_get_value(GPIO_INVALID_NUM))
    {
        return GPIO_OK;
    }
    return GPIO_ERROR;
}

int test_bsp_gpio_direction_output_invalid_value(void)
{
    bsp_gpio_direction_output(GPIO_TEST_NUM, 10);
    bsp_gpio_direction_output(GPIO_TEST_NUM, -1);

    return GPIO_OK;
}

int test_bsp_gpio_set_value_invalid_value(void)
{
    bsp_gpio_set_value(GPIO_TEST_NUM, 10);
    bsp_gpio_set_value(GPIO_TEST_NUM, -1);

    return GPIO_OK;
}


int test_gpio_llt(void)
{
    bsp_gpio_direction_input(GPIO_TEST_NUM);
    bsp_gpio_get_value(GPIO_TEST_NUM);
    bsp_gpio_direction_get(GPIO_TEST_NUM);

    bsp_gpio_direction_output(GPIO_TEST_NUM, 0);
    bsp_gpio_set_value(GPIO_TEST_NUM, 1);
    bsp_gpio_direction_get(GPIO_TEST_NUM);

    return GPIO_OK;
}


#endif

void bsp_gpio_test(void)
{
    test_bsp_gpio_direction_get_init();
    test_bsp_gpio_direction_input_init();
    test_bsp_gpio_direction_output_init();
    test_bsp_gpio_set_value_init();
    test_gpio_get_value_init();

	test_bsp_gpio_init();

    test_gpio_llt();

    test_bsp_gpio_set_value_invalid_value();
    test_bsp_gpio_direction_output_invalid_value();

    test_bsp_gpio_direction_get_invalid_num();
    test_bsp_gpio_direction_input_invalid_num();
    test_bsp_gpio_direction_output_invalid_num();
    test_bsp_gpio_set_value_invalid_num();
    test_bsp_gpio_get_value_invalid_num();

    test_bsp_gpio_suspend();
    test_bsp_gpio_resume();

	test_bsp_gpio_direction_output();
    test_bsp_gpio_output_get_value();
	test_bsp_gpio_set_value();

	test_bsp_gpio_direction_input();
    test_bsp_gpio_input_set_value();
 	test_bsp_gpio_int_unmask_set();
 	test_bsp_gpio_int_mask_set();
	test_bsp_gpio_get_value();
	test_bsp_gpio_int_trigger_set();
	test_bsp_gpio_set_function();
	test_bsp_gpio_int_state_get();
	test_bsp_gpio_int_state_clear();
	test_bsp_gpio_raw_int_state_get();
}



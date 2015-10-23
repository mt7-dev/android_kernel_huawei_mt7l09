/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : i2c_balong.c */
/* Version        : 1.0 */
/* Created       : 2013-04-23*/
/* Last Modified : */
/* Description   :  The C union definition file for the module LTE_PMU*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
#include <stdio.h>
#include <i2c_balong.h>
#include <bsp_i2c.h>
#include "hi_i2c.h"

#define I2C_TEST_PASS 0
#define I2C_TEST_FAILE -1

u8 g_test_slave_id = 0x6b;	/*set bq24161 as the test device*/
/************************************************************/
/* bq24161 register description*/

/*offset	default_val	R/W */
/*0x00	0x32			R/W		bit7 and bit3 could be writen*/
/*0x01	0x1b			R/W		bit0 could be writen*/
/*0x02	0x60			R/W		bit7 write only*/
/*0x03	0x11			R/W*/
/*0x04	0xb2			R*/
/*0x05	0x9a			R/W*/
/*0x06	0x03			R/W		bit6 and bit7 read only*/
/*0x07	0x4b			R/W		bit1 and bit2 read only*/
/************************************************************/
/************************************************************/
/*read byte data from bq24161 01# register or 04# register*/
int i2c_ccore_test_init(enum i2c_master i2c_id)
{
	return bsp_i2c_slave_register(i2c_id,g_test_slave_id);
}

int test_i2c_set_slave_id(u32 id)
{
	g_test_slave_id = id;
	return 0;
}

/************************************************************/
int test_i2c_byte_data_receive(void)
{
	u8 slave_id = g_test_slave_id;
	u8 offset = 0x01;
	u16 read_data = 0;
	u8 default_data = 0x1b;
	int ret=0;

	/*read data from 0x01# register*/
	ret = bsp_i2c_byte_data_receive(slave_id, offset, &read_data);
	if(I2C_TEST_PASS!=ret)
	{
		i2c_print_error("bsp_i2c_byte_data_receive fail!\n");
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	/*check read  data is defalut data or not */
	if(default_data!=read_data)
	{
		i2c_print_error("read value is 0x%x\n",read_data);
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;
}

/************************************************************/
/*read word data from bq24161 02# register*/
/*									*/
/************************************************************/
int test_i2c_word_data_receive(void)
{
	u8 slave_id = g_test_slave_id;
	u8 offset = 0x02;
	u16 read_data=0;
	u16 default_data = 0x6011;
	int ret=0;

	/*read data from 0x01# register*/
	ret = bsp_i2c_word_data_receive(slave_id, offset, &read_data);
	if(I2C_TEST_PASS!=ret)
	{
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	/*check read  data is defalut data or not */
	if(default_data!=read_data)
	{
		i2c_print_error("read value is 0x%x\n",read_data);
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;

}

/************************************************************/
/*send byte data from bq24161 03# register*/
/*									*/
/************************************************************/
int test_i2c_byte_data_send(u8 addr,u8 value)
{
	u8 slave_id = g_test_slave_id;
	u8 offset = addr;
	u8 data = value;
	u16 back_data=0;
	int ret=0;

	/*send data to 0x02 register*/
	ret=bsp_i2c_byte_data_send(slave_id, offset, data);
	if(I2C_TEST_PASS!=ret)
	{
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	/*read the 0x02 register data*/
	ret=bsp_i2c_byte_data_receive(slave_id, offset, &back_data);
	if(I2C_TEST_PASS!=ret)
	{
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	/*check read data is write data or not*/
	if(data!=back_data)
	{
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;

}

/************************************************************/
/*send word data to bq24161 02# register	*/
/*									*/
/************************************************************/
int test_i2c_word_data_send(u8 addr,u16 value)
{
	u8 slave_id = g_test_slave_id;
	u8 offset = addr;
	u16 data= value;
	u16 back_data=0;
	int ret=0;

	/*send data to 0x02 register*/
	ret=bsp_i2c_word_data_send(slave_id, offset, data);
	if(I2C_TEST_PASS!=ret)
	{
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	/*read the 0x02 register data*/
	ret=bsp_i2c_word_data_receive(slave_id, offset, &back_data);
	if(I2C_TEST_PASS!=ret)
	{
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	/*check read data is write data or not*/
	if(data!=back_data)
	{
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;
}

int test_i2c_stress(int times,enum i2c_master i2c_id)
{
	int i = 0;	
	int count = 0;

	bsp_i2c_initial();
	bsp_i2c_slave_register(i2c_id,g_test_slave_id);

	count=0;
	for(i=0;i<times;i++){
		
		if(I2C_TEST_PASS!=test_i2c_byte_data_send(0x05,0x8a))
		{
			count++;
			i2c_print_error("register offset:0x05,send data:0x8a,times:%d\n\n",i);
		}
	}
	i2c_print_error("write 05 regitster,test count:%d times,success %d times\n\n",i,i-count);

	count = 0;
	for(i=0;i<times;i++){
		if(I2C_TEST_PASS!=test_i2c_byte_data_send(0x02,0x8a))
		{
			count++;
			i2c_print_error("register offset:%x,send data:0x8a\n",i);
		}
	}


	i2c_print_error("test count:%d times,success %d times\n\n",i,i-count);

	count = 0;
	for(i=0;i<times;i++)
	{
		if(I2C_TEST_PASS!=test_i2c_word_data_send(0x05,0x8a8a))
		{
			count++;
			i2c_print_error("register offset:0x05,send data:0x8a,times:%d\n\n",i);
		}

	}
	i2c_print_error("\ntest count:%d times,success %d times\n\n",i,i-count);

	count =0;
	
	for(i=0;i<times;i++){
		if(I2C_TEST_PASS!=test_i2c_word_data_send(0x02,0x8a8a))
		{
			count++;
			i2c_print_error("register offset:%x,send data:0x8a\n",i);
		}
	}
	
	i2c_print_error("\ntest count:%d times,success %d times\n\n",i-1,i-count);
	return 0;
}


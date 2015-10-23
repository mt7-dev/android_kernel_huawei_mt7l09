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
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <bsp_i2c.h>
#include <bsp_hardtimer.h>

#define I2C_TEST_PASS 0
#define I2C_TEST_FAILE -1

#define I2C_M_WR 00

//#define TEST_SLAVE_ID	0x6b				/* set bq24161 as test slave */
#define TEST_SLAVE_ID	0x23				/* set touch as test slave */
#define I2C_TEST_DEV0_NAME "i2c_test_0"	/* it may be surrport muti slave */
#define BQ24161_OFFSET0_VAL 0x32
#define BQ24161_OFFSET1_VAL 0x1b
#define BQ24161_OFFSET2_VAL 0x60
#define BQ24161_OFFSET3_VAL 0x11
#define BQ24161_OFFSET4_VAL 0xb2
#define BQ24161_OFFSET5_VAL 0x9a
#define BQ24161_OFFSET6_VAL 0x03
#define BQ24161_OFFSET7_VAL 0x4b
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

/************************************************************
/* DATA TYPE DEFINE		*/
/*						*/
/************************************************************/
struct i2c_test_data{
	struct i2c_client *client;
	struct input_dev *input_dev;
	uint16_t addr;
	uint16_t reserved;
};

/************************************************************/
/* I2C TEST DEVICE DEFINE	*/
/*						*/
/************************************************************/
struct i2c_board_info  i2c_test_board_info __initdata=
{	
	.type="i2c_test_0",
	.addr=TEST_SLAVE_ID,
};

/************************************************************/
/* I2C TEST OVERALL VARIABLE	*/
/*							*/
/************************************************************/
static struct i2c_client *g_client;

/************************************************************/
/* I2C TEST DRIVER PROBE	*/
/*						*/
/************************************************************/
static int   i2c_test_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;
	g_client = client;

	/*配置RESET 使用的GPIO*/
	ret = gpio_request(GPIO_1_24,"touch_test_reset");
	if(ret)
	{
		printk("gpio config error\n");
		return ret;
	}
	gpio_direction_output(GPIO_1_24,1);
	gpio_set_function(GPIO_1_24,GPIO_NORMAL);

	/*申请并配置ldo_en使用的gpio*/
	ret = gpio_request(GPIO_0_11,"touch_test_en");
	if(ret){
		printk("gpio config error!\n");
		return ret;
	}

	gpio_direction_output(GPIO_0_11,1);
	gpio_set_function(GPIO_0_11,GPIO_NORMAL);
	return 0;
}
static const struct i2c_device_id i2c_test_id[] = {
	{ I2C_TEST_DEV0_NAME, 0 },
};

static struct i2c_driver i2c_test_driver = {
	.probe		= i2c_test_probe,
	.id_table	= i2c_test_id,
	.driver = {
		.name	=  "i2c_test_0",
	},
};

/************************************************************/
/* I2C TEST DEVICE INIT	*/

/************************************************************/
static int __init i2c_device_init(void)
{
	/*充电芯片*/
	//return i2c_register_board_info(1,&i2c_test_board_info,1);
	
	/*触摸屏*/
	return i2c_register_board_info(0,&i2c_test_board_info,1);
}

/************************************************************/
/* I2C TEST DRIVER INIT	*/

/************************************************************/
static int __devinit i2c_test_init(void)
{
	g_client=kmalloc(sizeof(struct i2c_client *),GFP_KERNEL);
	return i2c_add_driver(&i2c_test_driver);
}

/************************************************************/
/* I2C TEST DRIVER INIT	*/

/************************************************************/
static void __exit i2c_test_exit(void)
{
	i2c_del_driver(&i2c_test_driver);
	return;
}
/************************************************************/
/* read a byte data from bq24161 01# register or 04# register	*/
/*													*/
/* test for "i2c_transfer()" interface						*/
/*													*/
/************************************************************/
int test_i2c_transfer_byte_receive(void)
{
	uint8_t buf0[4] = {0};
	uint8_t buf1[4] = {0};
	int ret = 0;
	int msg_num=2;
	struct i2c_test_data *ts ;
	struct i2c_msg msg[2] = {0};

	ts = (struct i2c_test_data *)kmalloc(sizeof(struct i2c_test_data), GFP_KERNEL);
	if (ts == NULL||NULL == ts->client) {
		ret = -ENOMEM;
		return ret;
	}
	
	ts->client = g_client;

	/*the slave register offset*/
	buf0[0]=0x01;

	/*config the sending message*/
	msg[0].addr = ts->client->addr;
	msg[0].flags = I2C_M_WR;	/*operation type */
	msg[0].len = 1;			/*the first msg is only include register offset*/
	msg[0].buf = buf0;			/*register offset*/

	msg[1].addr = ts->client->addr;
	msg[1].flags = I2C_M_RD;	/*operation type */
	msg[1].len = 1;			/*read byte number*/
	msg[1].buf = buf1;			/*read data buf */

	/*read data*/
	ret = i2c_transfer(ts->client->adapter, msg, msg_num);
	if (ret < 0) {
		i2c_print_error(KERN_ERR "i2c_transfer failed\n");
		return I2C_TEST_FAILE;
	}

	/*check read data is equal register default value*/
	if(BQ24161_OFFSET1_VAL!=buf1[0])
	{
		i2c_print_error("read value is 0x%x\n",buf1[0]);
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;

}

/************************************************************/
/* read muti bytes data from bq24161 01# register and  02# register	*/
/*														*/
/* test for "i2c_transfer()" interface							*/
/*														*/
/************************************************************/
int test_i2c_transfer_word_receive(void)
{
	uint8_t buf0[4] = {0};
	uint8_t buf1[4] = {0};
	int ret = 0;
	int msg_num=2;
	struct i2c_test_data *ts ;
	struct i2c_msg msg[2] = {0};

	ts = (struct i2c_test_data *)kmalloc(sizeof(struct i2c_test_data), GFP_KERNEL);
	if (ts == NULL || NULL == ts->client) {
		ret = -ENOMEM;
		return ret;
	}
	
	ts->client =g_client;
	
	buf0[0]=0x01;	/*the slave register offset*/

	/*config the sending message*/
	msg[0].addr = ts->client->addr;
	msg[0].flags = I2C_M_WR;	/*operation type */
	msg[0].len = 1;			/*the first msg is only include register offset*/
	msg[0].buf = buf0;			/*register offset*/

	msg[1].addr = ts->client->addr;
	msg[1].flags = I2C_M_RD;	/*operation type */
	msg[1].len = 2;			/*read byte number*/
	msg[1].buf = buf1;			/*read data buf */

	/*read data*/
	ret = i2c_transfer(ts->client->adapter, msg, msg_num);
	if (ret < 0) {
		i2c_print_error(KERN_ERR "i2c_transfer failed\n");
		return I2C_TEST_FAILE;
	}

	/*check read data is equal register default value */
	if(BQ24161_OFFSET1_VAL!=buf1[0]||BQ24161_OFFSET2_VAL!=buf1[1]){
		i2c_print_error("read value is 0x%x,0x%x\n",buf1[0],buf1[1]);
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;
}


/************************************************************/
/* read a bytes data from bq24161 01# register	*/
/*										*/
/* test for "i2c_smbus_read_byte_data()" interface*/
/*										*/
/************************************************************/
int test_i2c_smbus_byte_receive(void)
{
	int offset=0x01;
	int default_val=BQ24161_OFFSET1_VAL;
	int read_data=0;

	/*read data */
	read_data=i2c_smbus_read_byte_data(g_client,offset);

	/*check data is equal register default value*/
	if(default_val!=read_data){
		i2c_print_error("read data is:0x%x\n",read_data);
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;
}

/************************************************************/
/* read a bytes data from bq24161 01# register	*/

/* test for "i2c_smbus_read_word_data()" interface*/

/************************************************************/
int test_i2c_smbus_word_receive(void)
{
	int offset=0x01;
	int default_val=BQ24161_OFFSET2_VAL<<8|BQ24161_OFFSET1_VAL;
	int read_data=0;

	/*read data*/
	read_data=i2c_smbus_read_word_data(g_client,offset);

	/*check read data is equal register default value*/
	if(default_val!=read_data){
		i2c_print_error("read data is:0x%x\n",read_data);
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;

}
/************************************************************/
/* read a bytes data from bq24161 01# register	*/

/* test for "i2c_smbus_read_byte_data()" interface*/

/************************************************************/
int test_i2c_data_receive_stress(void)
{
	int num=1000;
	int default_val[]={BQ24161_OFFSET0_VAL,
					BQ24161_OFFSET1_VAL,BQ24161_OFFSET2_VAL,BQ24161_OFFSET3_VAL,\
					BQ24161_OFFSET4_VAL,BQ24161_OFFSET5_VAL,BQ24161_OFFSET6_VAL,\
					BQ24161_OFFSET7_VAL};
	uint8_t buf0[4] = {0};
	uint8_t buf1[8] = {0};
	int ret = 0;
	int msg_num=2;
	struct i2c_test_data *ts ;
	struct i2c_msg msg[2] = {0};
	int i=0;
	
	ts = (struct i2c_test_data *)kmalloc(sizeof(struct i2c_test_data), GFP_KERNEL);
	if (ts == NULL || NULL == ts->client) {
		ret = -ENOMEM;
		return ret;
	}
	
	ts->client =g_client;
	
	buf0[0]=0x0;	/*the slave register offset*/

	/*config the sending message*/
	msg[0].addr = ts->client->addr;
	msg[0].flags = I2C_M_WR;	/*operation type */
	msg[0].len = 1;			/*the first msg is only include register offset*/
	msg[0].buf = buf0;			/*register offset*/

	msg[1].addr = ts->client->addr;
	msg[1].flags = I2C_M_RD;	/*operation type */
	msg[1].len = 8;			/*read byte number*/
	msg[1].buf = buf1;			/*read data buf */


	/*read data*/
	while(0<num--){
		/*read data*/
		ret = i2c_transfer(ts->client->adapter, msg, msg_num);
		if (ret < 0) {
			i2c_print_error(KERN_ERR "i2c_transfer failed\n");
			return I2C_TEST_FAILE;
		}

		/*check read data is equal register default value*/
		for(i=0;i<8;i++){
			if(default_val[i]!=buf1[i]){
				i2c_print_error("read bq24161 %d# register data is:0x%x\n",i,buf1[i]);
				return I2C_TEST_FAILE;
			}
		}
	}
	return I2C_TEST_PASS;

}

/************************************************************/
/* write a bytes data to bq24161 03# register	*/

/* test for "i2c_smbus_read_word_data()" interface*/

/************************************************************/
int test_i2c_transfer_byte_send(void)
{
	struct i2c_test_data *ts ;
	uint8_t buf0[4] = {0};
	struct i2c_msg msg[2] = {0};
	int ret = 0;

	int read_back=0;

	ts = (struct i2c_test_data *)kmalloc(sizeof(struct i2c_test_data), GFP_KERNEL);
	if (ts == NULL || NULL == ts->client) {
		i2c_print_error("i2c transfer byte send malloc failed!\n");
		ret = -ENOMEM;
		return ret;
	}
	
	ts->client =g_client;
	
	buf0[0]=0x03;	/*the slave register offset*/
	buf0[1]=0x21;	/*the sending data*/

	/*config the sending message*/
	msg[0].addr = ts->client->addr;	/*slave addr*/
	msg[0].flags = I2C_M_WR;		/*opretion type*/
	msg[0].len = 2;				/*send byte numbers*/
	msg[0].buf = buf0;				/*messge buf*/

	/*send data*/
	ret = i2c_transfer(ts->client->adapter, msg, 1);
	if (ret < 0) {
		i2c_print_error(KERN_ERR "i2c_transfer failed\n");
		return ret;
	}

	/*read data and check it is equal the write data*/
	read_back=i2c_smbus_read_byte_data(g_client,buf0[0]);
	if(read_back!=buf0[1])
	{
		i2c_print_error("read value is 0x%x\n",read_back);
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;

}

/************************************************************/
/* write a word data to bq24161 03# register*/

/* test for "i2c_transfer()" interface*/

/************************************************************/
int test_i2c_transfer_word_send(void)
{
	struct i2c_test_data *ts ;
	uint8_t buf0[4] = {0};
	struct i2c_msg msg[2] = {0};
	int ret = 0;

	int read_back=0;

	ts = (struct i2c_test_data *)kmalloc(sizeof(struct i2c_test_data), GFP_KERNEL);
	if (ts == NULL || NULL == ts->client) {
		ret = -ENOMEM;
		return ret;
	}
	
	ts->client =g_client;
	
	buf0[0]=0x03;	/*the slave register offset*/
	buf0[1]=0x23;
	buf0[2]=0x70;
	
	msg[0].addr = ts->client->addr;
	msg[0].flags = I2C_M_WR;
	msg[0].len = 3;
	msg[0].buf = buf0;

	ret = i2c_transfer(ts->client->adapter, msg, 1);
	if (ret < 0) {
		i2c_print_error(KERN_ERR "i2c_transfer failed\n");
		return I2C_TEST_FAILE;
	}

	read_back=i2c_smbus_read_word_data(g_client,buf0[0]);
	if(read_back!=(buf0[2]<<8|buf0[1]))
	{
		i2c_print_error("read value is 0x%x\n",read_back);
		i2c_print_error("TEST FAIL!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;

}

/************************************************************/
/* write a byte data to bq24161 03# register */

/* test for "i2c_smbus_write_byte_data()" interface */

/************************************************************/
int test_i2c_sumbus_byte_send(void)
{
	int offset =0x03;
	int write_data=0x21;
	int read_back=0;
	int ret=0;
	
	ret=i2c_smbus_write_byte_data(g_client,offset,write_data);
	if(0!=ret){
		i2c_print_error("write data failed!\n");
		return I2C_TEST_FAILE;
	}

	read_back=i2c_smbus_read_byte_data(g_client,offset);
	if(read_back!=write_data)
	{
		i2c_print_error("read data fail!\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;
}

/************************************************************/
/* write a word data to bq24161 02# register and 03#register*/

/* test for "i2c_smbus_write_word_data()" interface*/

/************************************************************/
int test_i2c_sumbus_word_send(void)
{
	int offset =0x02;
	int write_data=0x2170;
	int read_back=0;
	int ret=0;
	
	ret=i2c_smbus_write_word_data(g_client,offset,write_data);
	if(0!=ret){
		i2c_print_error("write data failed!\n");
		return I2C_TEST_FAILE;
	}

	read_back=i2c_smbus_read_word_data(g_client,offset);
	if(read_back!=write_data)
	{
		i2c_print_error("read data fail\n");
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;
}
int test_i2c_master_send(void)
{
	u8 buf[20];/*存放回读结果*/
	int ret = 0;

	buf[0] = 0x0f;

	ret = i2c_master_send(g_client,buf,1);

	ret = i2c_master_recv(g_client,buf,13);

	return ret;
}
int test_i2c_stress(unsigned int times)
{
	u32 i=0;
	u32 success = 0;
	u32 t_sumb_begin;/*i2c_sumbus_byte_send 测试开始时间*/
	u32 t_sumb_over; /*i2c_sumbus_byte_send 测试结束时间*/
	u32 t_sumw_begin;/*i2c_sumbus_word_send 测试开始时间*/
	u32 t_sumw_over; /*i2c_sumbus_word_send 测试结束时间*/
	u32 t_mrev_begin;
	u32 t_mrev_over;


	t_sumb_begin = bsp_get_slice_value();
	for(i=0;i<times;i++){
		if(I2C_TEST_PASS ==test_i2c_sumbus_byte_send()){
			success++;
		}
	}	
	t_sumb_over = bsp_get_slice_value();

	i2c_print_error("i2c sumbus byte send test %d times,success %d times\n",i,success);
	i2c_print_error("i2c sumbus byte send test time_before 0x%x,after: 0x%x \n",t_sumb_begin,t_sumb_over);
	t_sumw_begin = bsp_get_slice_value();
	success = 0;
	for(i=0;i<times;i++){
		
		if(I2C_TEST_PASS ==test_i2c_sumbus_word_send()){
			success++;
		}
	}
	t_sumw_over = bsp_get_slice_value();
	i2c_print_error("i2c sumbus word send test %d times,success %d times\n",i,success);
	i2c_print_error("i2c sumbus word send test 0x%x,after: 0x%x \n",t_sumw_begin,t_sumw_over);

	
	t_mrev_begin = bsp_get_slice_value();
	for(i=0;i<times;i++){
		if(I2C_TEST_PASS ==test_i2c_master_send()){
			success++;
		}
	}	
	t_mrev_over = bsp_get_slice_value();

	i2c_print_error("i2c write 1byte and master read 13bytes %d times,success %d times\n",i,success);
	i2c_print_error("i2c write 1byte and master read 13bytes test time_before 0x%x,after: 0x%x \n",t_mrev_begin,t_mrev_over);

	return I2C_OK;
}
module_init(i2c_test_init);
module_exit(i2c_test_exit);
postcore_initcall(i2c_device_init);

MODULE_DESCRIPTION("I2C Test Driver");
MODULE_LICENSE("GPL");
/*******************************************************************************************/


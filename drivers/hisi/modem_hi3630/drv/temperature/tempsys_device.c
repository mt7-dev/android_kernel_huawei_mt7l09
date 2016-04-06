/*
 * Copyright (c) 2011 Hisilicon Technologies Co., Ltd. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*lint --e{537} */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#include "drv_temp_cfg.h"
#include "bsp_temperature.h"



#define THERMAL_OK          0
#define THERMAL_ERROR      1 /* error单词 */
#define HKADC_NULL      (void*)(0)


#define THERMAL_CLASS_NAME   "thermal"
#define THERMAL_DEVICE_NAME  "temp"

#define TEMPERATOR_DEV_MAX        HKADC_TEMP_BUTT


typedef struct drv_hkadc_map TEMPSYS_DEV; /* tempsys_dev换成大写 */

/* 建议结构体精简 
struct drv_hkadc_map g_hkadc_chan_map[] = {
    {HKADC_TEMP_BATTERY,    0, 0, 0, 0, "battery"},
    {HKADC_TEMP_PA0,        0, 0, 0, 0, "pa0"},
    {HKADC_TEMP_PA1,        0, 0, 0, 0, "pa1"},
    {HKADC_TEMP_DCXO0,      0, 0, 0, 0, "dcxo_0"},
    {HKADC_TEMP_DCXO0_LOW,  0, 0, 0, 0, "dcxo_0_low"},
    {HKADC_TEMP_DCXO1,      0, 0, 0, 0, "dcxo_1"},
    {HKADC_TEMP_DCXO1_LOW,  0, 0, 0, 0, "dcxo_1_low"},
    {HKADC_TEMP_LCD,        0, 0, 0, 0, "lcd"},	
    {HKADC_TEMP_SIM_CARD,   0, 0, 0, 0, "sim_card1"},
    {HKADC_TEMP_BUTT,       0, 0, 0, 0, "buttom"}
};

*/


struct hkadc_chan_name{
	unsigned int logic_chan; 
    char * name;
};

struct hkadc_chan_name g_hkadc_chan_map[] = {
    {HKADC_TEMP_BATTERY,     "battery"},
    {HKADC_TEMP_PA0,         "pa0"},
    {HKADC_TEMP_PA1,         "pa1"},
    {HKADC_TEMP_DCXO0,       "dcxo_0"},
    {HKADC_TEMP_DCXO0_LOW,   "dcxo_0_low"},
    {HKADC_TEMP_DCXO1,       "dcxo_1"},
    {HKADC_TEMP_DCXO1_LOW,   "dcxo_1_low"},
    {HKADC_TEMP_LCD,         "lcd"},	
    {HKADC_TEMP_SIM_CARD,    "sim_card1"},
    {HKADC_TEMP_BUTT,        "buttom"}
};	

/* 该函数用数组代替 
struct drv_hkadc_map *bsp_hkadc_get_chan_map(int logic_chan)
{
    struct drv_hkadc_map *map = &g_hkadc_chan_map[0];

    
    while (HKADC_TEMP_BUTT != map->logic_chan) {
        if (map->logic_chan == logic_chan) {
            return map;
        }
        map++;
     }
        
    return HKADC_NULL;
}
*/

static ssize_t tempsys_val_show(struct device * dev,
                    struct device_attribute * attr,
                    char * buf);

static  ssize_t tempsys_val_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count);



/*定义: dev_attr_val*/
DEVICE_ATTR(val, S_IRUGO, tempsys_val_show, tempsys_val_store);/* 确认这两个属性配置为只读 */



static ssize_t tempsys_val_show(struct device * dev,
                    struct device_attribute * attr,
                    char * buf)
{
    unsigned short volt;
    short temp;
	
	TEMPSYS_DEV* tem_dev = (TEMPSYS_DEV*)dev_get_drvdata(dev);
	if(0 != drv_hkadc_get_temp((HKADC_TEMP_PROTECT_E)tem_dev->logic_chan,&volt,&temp, (HKADC_CONV_MODE_E)0))
	{
		return -1;
	}	

    /*返回字符形式的设备温度值*/
	/*lint -save -e123*/
    return snprintf(buf, PAGE_SIZE, "%d\n",temp);
	/*lint -restore*/
}

static ssize_t tempsys_val_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
    return 0;
}





/*****************************************************************************
 函 数 名  : tempsys_create_device
 功能描述  : 根据hkadc的逻辑通道号列表，创建thermal class下面的sysfs文件节点
 输入参数  : 温保设备类描述

 输出参数  :
 返 回 值  : 成功返回0，失败返回错误号
*****************************************************************************/
int tempsys_create_device(struct class* thermal_class)
{
    struct device * new_dev = NULL;
	int i;
	int ret;
	struct hkadc_chan_name map;
	dev_t devno = 0;
	char dev_name[64];/* 建议减小，注意对齐 */
	
	for (i = 0 ; i < TEMPERATOR_DEV_MAX ; i++) {
		 
		 	map = g_hkadc_chan_map[i];
			memset(dev_name, 0, sizeof(dev_name));
			snprintf(dev_name,64,"%s:%s", THERMAL_DEVICE_NAME, map.name);
			
			/*在设备类别目录创建char型文件节点: /sys/class/thermal/temp:i*/
			new_dev = device_create(thermal_class, NULL, devno, "%s",
									dev_name);
			if (IS_ERR(new_dev)) {
				ret = PTR_ERR(new_dev);
				printk(KERN_ALERT"failed to create tempsys device.\n");
				return ret;
			}
		
			ret = device_create_file(new_dev, &dev_attr_val);
			if (ret < 0) {
				printk(KERN_ALERT"failed to create tempsys file.\n");
				return ret;
			}

			dev_set_drvdata(new_dev, &g_hkadc_chan_map[i]);
		
		 
        }
			


	 
	
	
	

	return 0;


	
}


/*****************************************************************************
 函 数 名  : tempsys_device_init
 功能描述  : 温度sysfs节点设备创建初始化
 输入参数  :

 输出参数  :
 返 回 值  :
*****************************************************************************/
static int __init tempsys_device_init(void)
{

	
	int ret = THERMAL_ERROR;
	struct class * thermal_class;

	/* 创建目录/sys/class/thermal/  */
	thermal_class = class_create(THIS_MODULE, THERMAL_CLASS_NAME);
	if (IS_ERR(thermal_class)) {
        ret = PTR_ERR(thermal_class);
        printk(KERN_ERR "failed to create thermal class.\n");
        goto destory_class;
    }

    printk(KERN_INFO " initialize thermal device.\n");/* 打印级别 */

	
	if (tempsys_create_device(thermal_class))
	{
		goto destory_device;
	}

	printk(KERN_INFO "Succeedded to initialize thermal device.\n");
	
	return 0;
		

destory_device:	
destory_class:

    return ret;
}

static void __exit tempsys_device_exit(void)
{
    
}


module_init(tempsys_device_init);
module_exit(tempsys_device_exit);


MODULE_AUTHOR("  <@huawei.com>");
MODULE_DESCRIPTION("hi6620 tempator sysfs interface");
MODULE_LICENSE("GPL");





#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <asm/uaccess.h>
#include <linux/earlysuspend.h>
#include <linux/leds.h>

#include <bsp_memmap.h>
#include <hi_base.h>
#include <linux/regulator/consumer.h>
#include <hi_syssc.h>
#include <bsp_version.h>
#include <linux/video/lcdfb_balong.h>






#define SPIDEV_MAJOR			150	/* SPI从设备主设备号 */
#define N_SPI_MINORS			32	/* SPI从设备次设备号 */
/*定义一个数组，数组的维数通过N_SPI_MINORS计算得到*/
static DECLARE_BITMAP(minors, N_SPI_MINORS);


#define IMAGEBUFFER (LCD_WIDTH + 1)* (LCD_HIGHT + 1) *8


struct lcd_spidev_data {
	dev_t			devt;
	spinlock_t		spi_lock;
	struct spi_device	*spi;
	struct list_head	device_entry;

	/* buffer is NULL unless this device is open (users > 0) */
	struct mutex		buf_lock;
	unsigned		users;
	u8			*buffer;
};

struct stlcdfb_par {
	struct platform_device *pdev;
	struct fb_info *info;
    u32 *vmem;
    struct spi_driver spidrv;
    struct regulator *dr1_vcc;
    struct regulator *lcd_vcc;

    u32 g_ulLcdPwrOnState;
    struct semaphore	g_screensemmux;

    #ifdef CONFIG_HAS_EARLYSUSPEND
        struct early_suspend earlysus_lcd;
    #endif
	u32 fb_imgType;
    u32 hw_refresh;
    u32 var_pixclock;
    u32 var_xres;
    u32 var_yres;
    u32 fb_page;
    struct lcd_panel_info panel_info;
    int ref_cnt;
    int lcd_backlight_registered;
    struct balong_bl bl;
    struct led_classdev led_cdev;
};

struct v7r2_lcd{
    struct class *lcd_class;
    struct lcd_spidev_data* g_lcd;
};




/*初始化设备链表*/
static LIST_HEAD(device_list);
/*初始化互斥信号量*/
static DEFINE_MUTEX(device_list_lock);

 u8 image[IMAGEBUFFER];
struct v7r2_lcd lcd;



#ifdef CONFIG_FB_2_4_INCH_BALONG
static struct balong_lcd_seq initseq[] =
{
        {'c',0xCF},/**power control B*/
        {'d',0x00},
        {'d',0xD9},
        {'d',0x30},


        {'c',0xED},/*power on sequence control*/
        {'d',0x64},
        {'d',0x03},
        {'d',0x12},
        {'d',0x81},

        {'c',0xE8},/*driver timing control A*/
        {'d',0x85},
        {'d',0x10},
        {'d',0x78},

        {'c',0xCB}, /*power control A*/
        {'d',0x39},
        {'d',0x2C},
        {'d',0x00},
        {'d',0x34},
        {'d',0x02},

        {'c',0xF7},/*pump ratio control*/
        {'d',0x20},

        {'c',0xEA},/*driver timing control B*/
        {'d',0x00},
        {'d',0x00},



        {'c',0xC0},/*power control 1*/
        {'d',0x1B},/*VRH[5:0]*/

        {'c',0xC1},/*power control 2*/
        {'d',0x12},/*SAP[2:0];BT[3:0]*/

        {'c',0xC5},/*VCM control*/
        {'d',0x32},
        {'d',0x3C},


        {'c',0xC7},/*VCM control2*/
        {'d',0x99},

        {'c',0x36},/*memory access control*/
        {'d',0x08},/*RGB-BGR*/

        {'c',0x3A},
        {'d',0x55},/*65k 262k selection*/

        {'c',0xB1},
        {'d',0x00},
        {'d',0x1B},

        {'c',0xB6},/*display function control*/
        {'d',0x0A},
        {'d',0xC2},/*display order*/
        {'d',0x27},/*added by liujia*/

        {'c',0xF6},
        {'d',0x01},
        {'d',0x30},
        {'d',0x20},

        {'c',0xF2},/*3gamma function disable*/
        {'d',0x00},

        {'c',0x26},/*gamma curve selected*/
        {'d',0x01},

        {'c',0xE0}, /*set gamma*/
        {'d',0x0F},
        {'d',0x1D},
        {'d',0x1A},
        {'d',0x0A},
        {'d',0x0D},
        {'d',0x07},
        {'d',0x49},
        {'d',0x66},
        {'d',0x3B},
        {'d',0x07},
        {'d',0x11},
        {'d',0x01},
        {'d',0x09},
        {'d',0x05},
        {'d',0x04},

        {'c',0xE1},
        {'d',0x00},
        {'d',0x18},
        {'d',0x1D},
        {'d',0x02},
        {'d',0x0F},
        {'d',0x04},
        {'d',0x36},
        {'d',0x13},
        {'d',0x4C},
        {'d',0x07},
        {'d',0x13},
        {'d',0x0F},
        {'d',0x2E},
        {'d',0x2F},
        {'d',0x05},

        {'c',0x11},/*SLEEP OUT*/
        {'w',120},

        {'c',0x29},/*DISPLAY ON*/

        {'c',0x2c},



};

static struct balong_lcd_seq initseqtmi[] =
{
};
#elif defined (CONFIG_FB_1_4_5_INCH_BALONG)

struct regulator *dr1_vcc;
struct balong_lcd_seq initseq[] =
{
    {'c',0x0011}, /*唤醒LCD*/
        {'w', 120},

        {'c',0x003A},/*设置像素格式*/
        {'d',0x0055},  /*65K  OR 262k selectiong  12bit control*/

        {'c',0x00C0},/*GVDD VCI1*/
        {'d',0x000C},
        {'d',0x0005},
        {'w', 10},

        {'c',0x00C1},/*Vol supply*/
        {'d',0x0006},
        {'w', 10},

        {'c',0x00C2}, /*set the amount of current in operate放大器电流值*/
        {'d',0x0004},
        {'w', 10},

        {'c',0x00C5},/*vcoml vcomh*/
        {'d',0x0029},
        {'d',0x0035},
        {'w', 10},

        {'c',0x00C7},/*vcom偏移*/
        {'d',0x00D5},

        {'c',0x00F2},/*e0 e1 使能*/
        {'d',0x0001},

        {'c',0x00E0},/*Gamma setting */
        {'d',0x003F},
        {'d',0x001C},
        {'d',0x0018},
        {'d',0x0025},
        {'d',0x001E},
        {'d',0x000C},
        {'d',0x0042},
        {'d',0x00D8},
        {'d',0x002B},
        {'d',0x0013},
        {'d',0x0012},
        {'d',0x0008},
        {'d',0x0010},
        {'d',0x0003},
        {'d',0x0000},

        {'c',0x00E1},
        {'d',0x0000},
        {'d',0x0023},
        {'d',0x0027},
        {'d',0x000A},
        {'d',0x0011},
        {'d',0x0013},
        {'d',0x003D},
        {'d',0x0072},
        {'d',0x0054},
        {'d',0x000C},
        {'d',0x001D},
        {'d',0x0027},
        {'d',0x002F},
        {'d',0x003C},
        {'d',0x003F},

        {'c',0x0036},/*扫描方向*/
        {'d',0x0008},

        {'c',0x00B1},/*帧率*/
        {'d',0x0010},
        {'d',0x000A},

        {'c',0x002A},/*扫描方向BGR*/
        {'d',0x0000},
        {'d',0x0000},
        {'d',0x0000},
        {'d',0x007F},

        {'c',0x002B},
        {'d',0x0000},
        {'d',0x0000},
        {'d',0x0000},
        {'d',0x007F},

        {'c',0x0029},/*打开显示屏*/
        {'w', 5},

        {'c',0x002C},/*写*/
        {'w', 120}

};


static struct balong_lcd_seq initseqtmi[] =
{
    {'c',0x11}, /*唤醒LCD*/
    {'w', 120},

    {'c',0xEC},/*设置像素格式*/
    {'d',0x0C},  /*65K  OR 262k selectiong  16bit control*/

   /*{'c', 0x0026}, Set Default Gamma*/
    /*{'d', 0x0004},*/

    {'c',0x26},/*GVDD VCI1*/
    {'d',0x04},


    {'c',0xB1},/*Vol supply*/
    {'d',0x0A},
    {'d', 0x14},

    {'c',0xC0}, /*set the amount of current in operate放大器电流值*/
    {'d',0x11},
    {'d',0x05},

    {'c',0xC1},/*vcoml vcomh*/
    {'d',0x2},

    {'c',0xC5},/*vcom偏移*/
    {'d',0x40},
    {'d',0x45},

    {'C',0xC7},
    {'d',0xC1},

    {'c',0x2A},/*扫描方向BGR*/
    {'d',0x00},
    {'d',0x00},
    {'d',0x00},
    {'d',0x7F},

    {'c',0x2B},
    {'d',0x00},
    {'d',0x00},
    {'d',0x00},
    {'d',0x7F},

    {'c',0x3A},/*扫描方向*/
    {'d',0x55},


    {'c',0x36},/*扫描方向*/
    {'d',0xC8},


    {'c',0xF2},/*e0 e1 使能*/
    {'d',0x01},

    {'c',0xE0},/*Gamma setting*/
    {'d',0x3F},
    {'d',0x1C},
    {'d',0x18},
    {'d',0x25},
    {'d',0x1E},
    {'d',0x0C},
    {'d',0x42},
    {'d',0xD8},
    {'d',0x2B},
    {'d',0x13},
    {'d',0x12},
    {'d',0x08},
    {'d',0x10},
    {'d',0x03},
    {'d',0x00},

    {'c',0xE1},
    {'d',0x00},
    {'d',0x23},
    {'d',0x27},
    {'d',0x0A},
    {'d',0x11},
    {'d',0x13},
    {'d',0x3D},
    {'d',0x72},
    {'d',0x54},
    {'d',0x0C},
    {'d',0x1D},
    {'d',0x27},
    {'d',0x2F},
    {'d',0x3C},
    {'d',0x3F},

    {'c',0x0029},/*打开显示屏*/
    {'w', 10},

    {'c',0x002C},/*写*/
    {'w', 120}
};
#endif

struct fb_info *g_fbinfo;
unsigned BACKLIGHT;

void set_gpio_updown_time(unsigned gpio,int gpio_status,u32 t_us)
{
    gpio_direction_output(gpio,gpio_status);
    udelay(t_us);
}
#ifdef CONFIG_FB_2_4_INCH_BALONG
 int balong_bl_init(struct balong_bl *lcdbl)
{

    set_gpio_updown_time(lcdbl->gpio,GPIO_UP,35);
    lcdbl->intensity_past = 16;

}

s32 intensity_to_pulse_num(struct balong_bl *lcdbl)
{
    int pulse_num = 0;
    if (lcdbl->intensity_past == lcdbl->intensity_cur)
    {
        hilcd_trace("the brightness set is not changed!\n");

    }
    else if (lcdbl->intensity_past < lcdbl->intensity_cur)
    {
        pulse_num =  MAX_BACKLIGHT_INTENSITY + lcdbl->intensity_past - lcdbl->intensity_cur ;
    }
    else
    {
        pulse_num = lcdbl->intensity_past - lcdbl->intensity_cur;
    }
    return pulse_num;

}

void balong_bl_pulse_output (struct balong_bl *lcdbl,int pulse_num,u32 t_us)
{
    int i;
    for(i = 1;i <= pulse_num;i++)
    {

        set_gpio_updown_time(lcdbl->gpio,GPIO_DOWN,t_us);
        set_gpio_updown_time(lcdbl->gpio,GPIO_UP,t_us);
    }

}
#elif defined CONFIG_FB_1_4_5_INCH_BALONG
/*根据亮度值要求，设置不同电流*/
int balong_bl_current_set(struct balong_bl *lcdbl,int intensity)
{
    int cur = 0;

    cur = regulator_list_voltage(lcdbl->bl_vcc,intensity- 1);
    regulator_set_voltage(lcdbl->bl_vcc,cur,cur);
}
#endif
 int balong_bl_set_intensity(struct led_classdev *led_cdev,enum led_brightness value)
{
    struct stlcdfb_par *par = container_of(led_cdev, struct stlcdfb_par, led_cdev);
    struct balong_bl *lcdbl = &par->bl;
    int pulse_num = 0;

#ifdef CONFIG_FB_2_4_INCH_BALONG
    if(0 == value)
    {

        hilcd_trace("brightness is  0\n");
        set_gpio_updown_time(lcdbl->gpio,GPIO_DOWN,35000);
        lcdbl->intensity_cur = 0;
        goto exit;

    }
    if (0 == lcdbl->intensity_past)
    balong_bl_init(lcdbl);
    hilcd_trace("brightness is not 0\n");

    lcdbl->intensity_cur = (value - 1)/16 + 1;
    hilcd_trace("intensity = %d\n",lcdbl->intensity_cur);

    pulse_num = intensity_to_pulse_num(lcdbl);
    hilcd_trace("pulse_num is %d\n",pulse_num);

    balong_bl_pulse_output(lcdbl,pulse_num,5);
#elif defined CONFIG_FB_1_4_5_INCH_BALONG
    if(0 == value)
    {
        hilcd_trace("brightness is 0,bl will be closed!\n");
        if(regulator_is_enabled(lcdbl->bl_vcc))
            regulator_disable(lcdbl->bl_vcc);
        lcdbl->intensity_cur = 0;
        goto exit;
    }
    if (0 == lcdbl->intensity_past)
    {
        if(!regulator_is_enabled(lcdbl->bl_vcc))
            regulator_enable(lcdbl->bl_vcc);
    }
    lcdbl->intensity_cur = (value - 1)/8 + 1;
    balong_bl_current_set(lcdbl,lcdbl->intensity_cur);
#endif
exit:
    lcdbl->intensity_past = lcdbl->intensity_cur;
    return LCD_OK;
}

struct fb_info g_fbinfo_get(void)
{
    return (struct fb_info )(*g_fbinfo);
}


 s32 spi_write_data(const u8 *buf, size_t len)
{
        struct lcd_spidev_data *lcd_data = lcd.g_lcd;
        struct spi_device	*spi;
        u32 count = 0;
        u16 data;
        u32 *dst = (u32 *)&image[0];

        spin_lock_irq(&lcd_data->spi_lock);
	    spi = spi_dev_get(lcd_data->spi);
	    spin_unlock_irq(&lcd_data->spi_lock);

        if (NULL == spi)
            return LCD_ERROR;

        for(count = 0;count < len; count++){

            data = (u32)((buf[count]) | (1<<8)); /* data  */
            memcpy(dst,&data,4);
            dst++;

        }
        spi_write(spi,(void *)image,len*4);

        return LCD_OK;
}


/* spi write cmd */
 s32 spi_write_cmd(const u8 *buf, size_t len)
{
    struct lcd_spidev_data *lcd_data = lcd.g_lcd;
    struct spi_device	*spi;
    u32 count = 0;
    u32 cmd;
    u32 *dst = (u32 *)&image[0];
    spin_lock_irq(&lcd_data->spi_lock);
	spi = spi_dev_get(lcd_data->spi);
	spin_unlock_irq(&lcd_data->spi_lock);

    if (NULL == spi)
        return LCD_ERROR;

    for(count = 0;(u32)count < len; count++){

        cmd = (u32)(buf[count]) & ~(1 << 8); /* command,d/c=0 */
        memcpy(dst,&cmd,4);
        dst++;

    }
    spi_write(spi,(void *)image,len*4);
    return LCD_OK;

}


 int balong_lcd_toggle(struct balong_lcd_seq *seq, int sz)
{
    s32 ret = 0;
    u32 count;
    u8 buf[1];

    for (count = 0; count < sz; count++)
    {
    	buf[0] = seq[count].value;
    	if(seq[count].format == 'c')
    	{
            /*printk("spi_write_cmd begin!!!\n");*/

    		ret = spi_write_cmd(buf, 1);  /* command */
    		if(ret)
    		{
    		    hilcd_error("spi write command error!");
    		    return ret;
    		}

    	}
    	else if(seq[count].format == 'd')
    	{

    		ret = spi_write_data(buf, 1); /* data */
    		if(ret)
    		{
    		    hilcd_error("spi write data error!");
    		    return ret;
    	    }

    	}
    	else if(seq[count].format=='w')
    	{

    		mdelay(seq[count].value); /* delay */

    	}
    }
return LCD_OK;
}

s32 lcd_vectid_get (u16 *pu16VectId)
{
    #if 0
        u32 u32Data1 = 0;
        u32 u32Data2 = 0;
    	static bool is_init = BSP_FALSE;

        if(!pu16VectId)
        {
        	hilcd_error("invalid parameter\n");
            return LCD_ERROR;
        }

    	if(!is_init)
    	{
    		if(gpio_request(LCD_ID0, "LCD_ID0"))
    		{
    			hilcd_error("Gpio is busy!!! \n");
    		}
    		/* 设置GPIO为输入 */
    		gpio_direction_input(LCD_ID0);

    		if(gpio_request(LCD_ID1, "LCD_ID1"))
    		{
    			hilcd_error("Gpio is busy!!! \n");
    		}

    		is_init = BSP_TRUE;
    	}

     	gpio_direction_input(LCD_ID1);

        /* 获取输入值 */
    	u32Data1 = gpio_get_value(LCD_ID0);
    	u32Data2 = gpio_get_value(LCD_ID1);

	hilcd_trace("LCD_ID0 %x,LCD_ID1 %x\n",u32Data1,u32Data2);
    /* 获取 LCD 厂家ID*/
    *pu16VectId = (unsigned short)((1&u32Data1<<0)\
                     |((1&u32Data2)<<1));
     #endif

    *pu16VectId = 0;
    return LCD_OK;

}

void lcd_init_byid(u32 ucId)
{
    int ret = 0;
    if(LCD_ILITEK == ucId)
	{
	    ret=balong_lcd_toggle(initseqtmi, ARRAY_SIZE(initseqtmi));
        if(ret)
        {
            hilcd_error("lcd init error");
        }
        return;
	}
	else if(LCD_CHIMEI == ucId)
	{

        ret=balong_lcd_toggle(initseq, ARRAY_SIZE(initseq));
        if(ret)
        {
            hilcd_error("lcd init error");
        }
        return;
	}
	else
	{
		hilcd_error("invalid lcd vector id\n");
        return;
	}

}

/*****************************************************************************
* 函 数 名  : lcdInit
*
* 功能描述  :  LCD 初始化
*
* 输入参数  :
*
* 输出参数  : 无
*
* 返 回 值  :
*****************************************************************************/
void lcd_init(void)
{
	u16 ucId = 0;
    s32 retval = 0;

	retval = lcd_vectid_get(&ucId);
    if(retval)
    {
        return;
    }

	lcd_init_byid(ucId);
}

 void lcd_pwron(struct fb_info *info)
{
    struct stlcdfb_par *par = info->par;
    /*已是上电状态，无需重新上电*/
	if (LCD_PWR_ON_STATE == par->g_ulLcdPwrOnState)
	{
		/*printk("lcd has been powered on.\n");*/
		return ;
	}
	msleep(110);/*100ms以上*/

    lcd_init();
    par->g_ulLcdPwrOnState = LCD_PWR_ON_STATE;
	return ;
}


s32 balong_lcd_window_set(struct fb_info *info,u16 x0,u16 y0,u16 x1,u16 y1)
{
    s32 ret = 0;
    u8 buf[1];

    if((x0>(info->var.xres-1))|(x1>(info->var.xres-1))|(y0>(info->var.yres-1))|(y1>(info->var.yres-1)))
    {
        hilcd_error("paramter error,[x0:%d][x1:%d][y0:%d][y1:%d]",
                    x0,x1,y0,y1);
        return -EINVAL;
    }

    buf[0] = COLUMN_ADDRESS_SET;

    ret = spi_write_cmd(buf, 1);

	if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}

    buf[0] = (u8)(x0>>8);

    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = (u8)x0;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = (u8)(x1>>8);
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = (u8)x1;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}


    buf[0] = PAGE_ADDRESS_SET;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}

    buf[0] = (u8)(y0>>8);
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = (u8)y0;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = (u8)(y1>>8);
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = (u8)y1;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = MEMORY_WRITE;

    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
   return LCD_OK;
}


s32 lcd_refresh(struct fb_info *info)
{

    struct stlcdfb_par *par = info->par;
    struct fb_var_screeninfo var = info->var;
    struct fb_fix_screeninfo fix = info->fix;
	u8 *buf = info->screen_base + var.yoffset * fix.line_length +var.xoffset ;
	u32 len = (fix.line_length) * (var.yres);
    s32 ret = 0;
    /*u32 before_write_data_t;
    u32 after_write_data_t;*/
    osl_sem_down(&(par->g_screensemmux));
    /* set window before write image */

    ret = balong_lcd_window_set(info,0, 0, (u16)(var.xres - 1), (u16)(var.yres - 1));
    if(ret){
        hilcd_error("lcd window set error");
        return ret;
    }
    /*before_write_data_t = jiffies;*/
    ret = spi_write_data(buf,len);
    if(ret){

        hilcd_error("lcd refresh error");
        return ret;
    }
    /*after_write_data_t = jiffies - before_write_data_t;
    printk("write data time is %u",after_write_data_t);*/
    osl_sem_up(&(par->g_screensemmux));
    return LCD_OK;
}


 s32 balong_lcd_sleep_in(void)
{
    s32 ret = 0;
    u8 buf[1];
    buf[0] = SLEEP_IN;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
    msleep(120);
    return LCD_OK;
}

/* lcd out of sleep */
 s32 balong_lcd_sleep_out(void)
{
    s32 ret = 0;
    u8 buf[1];
    buf[0] = SLEEP_OUT;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
    msleep(120);
    return LCD_OK;
}
 s32 lcd_light(struct fb_info *info)
{
    struct stlcdfb_par *par = info->par;
    s32 ret = 0;
    u8 buf[1];

     osl_sem_down(&(par->g_screensemmux));

	 ret = balong_lcd_sleep_out();
     if(ret)
	{
	    hilcd_error("balong_lcd_sleep_out error!");
	    return ret;
	}
    buf[0] = DISPLAY_ON;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
    osl_sem_up(&(par->g_screensemmux));
    #ifdef CONFIG_FB_2_4_INCH_BALONG
    /*gpio_set_value(BACKLIGHT,1);*/
	balong_bl_set_intensity((struct led_classdev *)(&par->led_cdev),(enum led_brightness)par->led_cdev.brightness);
    #elif defined CONFIG_FB_1_4_5_INCH_BALONG
    regulator_enable(par->dr1_vcc);
    #endif
	return LCD_OK;
}


 s32 lcd_sleep(struct fb_info *info)
{
    s32 ret = 0;
    u8 buf[1];
    buf[0] = DISPLAY_OFF;
    struct stlcdfb_par *par = (struct stlcdfb_par *)info->par;

    #ifdef CONFIG_FB_2_4_INCH_BALONG
	 /*gpio_set_value(BACKLIGHT,0);*/
	balong_bl_set_intensity((struct led_classdev *)(&par->led_cdev),(enum led_brightness)0);
    #elif defined CONFIG_FB_1_4_5_INCH_BALONG
    regulator_disable(par->dr1_vcc);
    #endif
    osl_sem_down(&(par->g_screensemmux));
	ret = balong_lcd_sleep_in();
     if(ret)
	{
	    hilcd_error("balong_lcd_sleep_in error!");
	    return ret;
	}
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}

    return LCD_OK;
}
 s32 balong_lcd_display_allblack(struct fb_info *info)
{
    struct stlcdfb_par *par = info->par;
    u32 count;
    s32 ret;
    u8 data[1];
    u32 len = info->fix.smem_len; /* pixel format 565 */

    ret = balong_lcd_window_set(info,0,0,(info->var.xres-1),(info->var.yres-1));
    if(ret)
    {
        hilcd_error("lcd window set error");
        return ret;
    }
    osl_sem_down(&(par->g_screensemmux));

    for(count=0;(u32)count<len;count++)
    {
        data[0] = 0x0;     /* black rgb 0 */
        ret = spi_write_data(data,1);
        if(ret)
	    {
	        hilcd_error("spi write data error!");
	        return ret;
	    }
    }

    osl_sem_up(&(par->g_screensemmux));
    return LCD_OK;
}
s32 lcd_pwroff(struct fb_info *info)
{
    s32 ret = 0;
    struct stlcdfb_par *par = info->par;
    /*已是下电状态，无需重新下电*/
	if (LCD_PWR_OFF_STATE == par->g_ulLcdPwrOnState)
	{
		hilcd_error("lcd has been powered off");
		return LCD_OK;
	}
    #ifdef CONFIG_FB_2_4_INCH_BALONG
	gpio_set_value(BACKLIGHT,0);
    #elif defined CONFIG_FB_1_4_5_INCH_BALONG
    regulator_disable(((struct stlcdfb_par*)(info->par))->dr1_vcc);
    #endif
    /*关闭显示*/
	ret = lcd_sleep(info);
	if(ret)
	{
        hilcd_error("lcd sleepin error");
        return LCD_ERROR;
	}
    par->g_ulLcdPwrOnState = LCD_PWR_OFF_STATE;
	return LCD_OK;
}
 s32 stlcdfb_blank(int blank_mode,struct fb_info *fbinfo)
{
   /* struct platform_device *pdev = to_platform_device(fbinfo);*/
    switch (blank_mode)
    {
        case VESA_NO_BLANKING:/*lcd power 1;backlight power1*/
        (void)lcd_light(fbinfo);
        break;
        case VESA_VSYNC_SUSPEND:/*lcd on;backlight off/sleepin*/
        (void)lcd_sleep(fbinfo);
        break;
        case VESA_HSYNC_SUSPEND:
        case VESA_POWERDOWN:/*lcd off;backlight:off*/
        (void)lcd_pwroff(fbinfo);
        break;
        default:
        break;

    }
    return 0;
}


s32 lcd_refresh_bybit(struct fb_info *info,u8 x0,u8 y0,u8 x1,u8 y1,const u8 * pbuffer)
{
    s32 ret = 0;
    u32 bitNum = 0;
    u8 xloop,yloop;
    u8 *buffer = (u8 *)pbuffer;
    struct stlcdfb_par *par = info->par;
    u8 white[2] = {0XFF,0XFF};
    u8 black[2] = {0X00,0X00};

    if(NULL == pbuffer)
    {
        return LCD_ERROR;
    }

    /* set window before write image */
    down (&(par->g_screensemmux));
    ret = balong_lcd_window_set(info,x0, y0, x1, y1);
    if(ret)
    {

        return ret;
    }
    for (yloop = y0; yloop <= y1; yloop++)
	{
		for (xloop = x0; xloop <= x1; xloop++)
		{
            if( bitNum ==8 )
            {
         		buffer++;
         		bitNum=0;/*一个字节输出完毕时指针后移，位标志清零*/
         	}

         	if( (((*buffer)>>(7-bitNum)) & 0x01) == 0x01)/*从高位开始判断*/
         	{
     			ret = spi_write_data(white,2);

                if(ret)
                {

                    return ret;
                }

         	}
         	else
         	{
         		ret = spi_write_data(black,2);

                if(ret)
                {

                    return ret;
                }

             }
            bitNum++;
		}
    }

    up(&(par->g_screensemmux));
    return LCD_OK;
}


/* lcd display white at all window */
s32 balong_lcd_display_allwhite(struct fb_info *info)
{
    u32 count;
    s32 ret = 0;
    u8 data[1];
    u32 len = info->fix.smem_len;
    struct stlcdfb_par *par = info->par;
    ret = balong_lcd_window_set(info,0,0,(info->var.xres-1),(info->var.yres-1));
    if(ret)
    {
        hilcd_error("lcd window set error");
        return ret;
    }
    down(&(par->g_screensemmux));
    for(count=0;(unsigned int)count<len;count++)
    {
        data[0] = 0xff;  /* white rgb ff */
        spi_write_data(data,1);
        if(ret)
	    {
	        hilcd_error("spi write data error!");
	        return ret;
	    }
    }
    up(&(par->g_screensemmux));
    return LCD_OK;
}




static u32 stlcd_fb_line_length(u32 fb_index, u32 xres, u32 bpp)
{   /* The adreno GPU hardware requires that the pitch be aligned to
32 pixels for color buffers, so for the cases where the GPU
is writing directly to fb0, the framebuffer pitch
also needs to be 32 pixel aligned */
    /*if (fb_index == 0)
       return ALIGN(xres, 32) * bpp;
    else  */
        return xres * bpp;
}

int stlcdfb_check_var(struct fb_var_screeninfo *var,
			struct fb_info *info)
{
    u32 len;
    if(var->rotate != FB_ROTATE_UR)
        return -EINVAL;
    if(var->grayscale != info->var.grayscale)
        return -EINVAL;
    switch(var->bits_per_pixel){
    case 16:
        if((var->green.offset != 5)||
            !((var->blue.offset == 11)||(var->blue.offset == 0))||
            !((var->red.offset == 11)||(var->red.offset == 0))||/*suport RGB or BGR*/
            (var->blue.length != 5)||
            (var->green.length !=6)||
            (var->red.length != 5)||/*support 565*/
            (var->blue.msb_right != 0)||
            (var->green.msb_right != 0)||
            (var->red.msb_right != 0)||
            (var->transp.length!= 0)||
            (var->transp.length != 0))
            return -EINVAL;
        break;
    case 24:
        if ((var->blue.offset != 0) ||
            (var->green.offset != 8) ||
            (var->red.offset != 16) ||
            (var->blue.length != 8) ||
            (var->green.length != 8) ||
            (var->red.length != 8) ||
            (var->blue.msb_right != 0) ||
            (var->green.msb_right != 0) ||
            (var->red.msb_right != 0) ||
            !(((var->transp.offset == 0) &&
            (var->transp.length == 0)) ||
            ((var->transp.offset == 24) &&
            (var->transp.length == 8))))
            return -EINVAL;
        break;
    case 32:
        /* Figure out if the user meant RGBA or ARGB
        and verify the position of the RGB components */
        if (var->transp.offset == 24) {
            if ((var->blue.offset != 0) ||
                (var->green.offset != 8) ||
                (var->red.offset != 16))
                return -EINVAL;
            } else if (var->transp.offset == 0) {
            if ((var->blue.offset != 8) ||
                (var->green.offset != 16) ||
                (var->red.offset != 24))
                return -EINVAL;
            } else
            return -EINVAL;
            /* Check the common values for both RGBA and ARGB */
            if ((var->blue.length != 8) ||
                (var->green.length != 8) ||
                (var->red.length != 8) ||
                (var->transp.length != 8) ||
                (var->blue.msb_right != 0) ||
                (var->green.msb_right != 0) ||
                (var->red.msb_right != 0))
                return -EINVAL;
            break;
    default:
            return -EINVAL;

    }
    if ((var->xres_virtual <= 0) || (var->yres_virtual <= 0))
        return -EINVAL;
    len = var->xres_virtual * var->yres_virtual * (var->bits_per_pixel / 8);
    if (len > info->fix.smem_len)
        return -EINVAL;
    if ((var->xres == 0) || (var->yres == 0))
        return -EINVAL;
    /*if ((var->xres > mfd->panel_info.xres) ||
        (var->yres > mfd->panel_info.yres))
        return -EINVAL;  */
    if (var->xoffset > (var->xres_virtual - var->xres))
        return -EINVAL;
    if (var->yoffset > (var->yres_virtual - var->yres))
        return -EINVAL;
    return 0;
}

int stlcdfb_set_par(struct fb_info *info)
{
    struct stlcdfb_par *par = (struct stlcdfb_par *)(info->par);
    struct fb_var_screeninfo *var = &info->var;
    int old_imgType;
    int blank = 0;
    old_imgType = par->fb_imgType;
    switch (var->bits_per_pixel) {
        case 16:
            if (var->red.offset == 0)
                par->fb_imgType = MDP_BGR_565;
            else
                par->fb_imgType = MDP_RGB_565;
            break;
            case 24:
                if ((var->transp.offset == 0) && (var->transp.length == 0))
                    par->fb_imgType = MDP_RGB_888;
                else if ((var->transp.offset == 24) &&
                    (var->transp.length == 8)) {
                    par->fb_imgType = MDP_ARGB_8888;
                    info->var.bits_per_pixel = 32;
                    }
                break;
        case 32:
            if (var->transp.offset == 24)
                par->fb_imgType = MDP_ARGB_8888;
            else
                par->fb_imgType = MDP_RGBA_8888;
            break;
            default:
                return -EINVAL;
        }
    if ((par->var_pixclock != var->pixclock) ||
        (par->hw_refresh && ((par->fb_imgType != old_imgType) ||
        (par->var_pixclock != var->pixclock) ||
        (par->var_xres != var->xres) ||
        (par->var_yres != var->yres)))) {
            par->var_xres = var->xres;
            par->var_yres = var->yres;
            par->var_pixclock = var->pixclock;
            blank = 1;
            }
    par->info->fix.line_length = stlcd_fb_line_length(info->node, var->xres,
        var->bits_per_pixel / 8);
    if (blank) {
        stlcdfb_blank(VESA_POWERDOWN, info);
        lcd_pwron(info);
        }
    return 0;
}

int stlcdfb_pan_display( struct fb_var_screeninfo *var, struct fb_info *info)
{


    if (var->xoffset > (info->var.xres_virtual - info->var.xres))
        return -EINVAL;
    if (var->yoffset > (info->var.yres_virtual - info->var.yres))
        return -EINVAL;
    if (info->fix.xpanstep)
        info->var.xoffset =
        (var->xoffset / info->fix.xpanstep) * info->fix.xpanstep;

    if (info->fix.ypanstep)
        info->var.yoffset =
        (var->yoffset / info->fix.ypanstep) * info->fix.ypanstep;

    else
        hilcd_trace("dma function not set for panel type=%d\n") ;

    lcd_refresh(info);

    return 0;
}



int stlcdfb_open(struct fb_info *info,int user)
{
    struct stlcdfb_par *par = (struct stlcdfb_par *)info->par;
    int ret = 0;
    if(!par->ref_cnt){
            hilcd_trace("first open fb %d\n",info->node);
        }

    par->ref_cnt++;
    return LCD_OK;
}


int stlcdfb_release(struct fb_info *info,int user)
{
    struct stlcdfb_par *par = (struct stlcdfb_par *)info->par;
    int ret = 0;

    if(!par->ref_cnt){
        hilcd_error("try to close unopened fb %d\n",info->node);
        return -EINVAL;
        }
    par->ref_cnt--;
    if(par->ref_cnt){
         hilcd_trace("last close fb %d\n",info->node);
        }

    return LCD_OK;
}



static struct fb_ops stlcdfb_ops = {
    .owner		= THIS_MODULE,
    .fb_open = stlcdfb_open,
    .fb_release = stlcdfb_release,
    .fb_blank   = stlcdfb_blank,
    .fb_check_var = stlcdfb_check_var,
    .fb_set_par = stlcdfb_set_par,
    .fb_pan_display= stlcdfb_pan_display,
    /*.fb_fillrect = cfb_fillrect,*/
    /*.fb_copyarea = cfb_copyarea,*/
    /*.fb_imageblit = cfb_imageblit,*/

};

#if 0
static struct fb_deferred_io stlcdfb_defio = {
	.delay		= HZ,
	.deferred_io	= stlcdfb_deferred_io,
};

#endif

s32 __devinit lcd_spidev_probe(struct spi_device *spi)
{
	struct lcd_spidev_data	*spidev;
	s32			status;
	u32		    minor;
    u32         tmp;
    int         retval=0;
    u8          save;

	dev_info(&spi->dev, " spidev_probe begin! \n");
	/*分配lcd_spidev_data结构 */
	spidev = kzalloc(sizeof(*spidev), GFP_KERNEL);
	if (!spidev)
		return -ENOMEM;
    /*初始化lcd的spi相关的信息*/
	lcd.g_lcd = spidev;

	spidev->spi = spi;
    /*spi总线的锁和互斥信号量初始化*/
	spin_lock_init(&spidev->spi_lock);
    mutex_init(&spidev->buf_lock);
    /*初始化spi设备链表*/
	INIT_LIST_HEAD(&spidev->device_entry);
    /*创建spi从设备，并加入到设备链表中*/
	mutex_lock(&device_list_lock);/*spi设备链表锁*/
	minor = find_first_zero_bit(minors, N_SPI_MINORS);
	if (minor < N_SPI_MINORS) {
		struct device *dev;
        spidev->devt = MKDEV(SPIDEV_MAJOR, minor);
		dev = device_create(lcd.lcd_class, &spi->dev, spidev->devt,
				    spidev, "tftlcd%d.%d",
				    spi->master->bus_num, spi->chip_select);
		status = IS_ERR(dev) ? PTR_ERR(dev) : 0;
	} else {
		dev_dbg(&spi->dev, "no minor number available!\n");
		status = -ENODEV;
	}
	if (status == 0) {
		set_bit(minor, minors);
		list_add(&spidev->device_entry, &device_list);
	}
    mutex_unlock(&device_list_lock);

    if (status == 0)
		spi_set_drvdata(spi, spidev);
	else
		kfree(spidev);

    save = spi->mode;
    tmp = SPI_MODE_0;

    spi->mode = (u8)tmp;
    spi->bits_per_word = 9;
    spi->chip_select = 0;
    spi->max_speed_hz = 24000000;

    /*spi初始化设置，即使能*/
    retval = spi_setup(spi);
	if (retval < 0)
	{
		spi->mode = save;
		dev_info(&spi->dev, "set up failed,now spi mode %02x\n", spi->mode);
	}
	printk("lcd_spidev_probe is over!!!\n");
	return status;
}



static s32 __devexit lcd_spidev_remove(struct spi_device *spi)
{
	struct lcd_spidev_data	*spidev = spi_get_drvdata(spi);

	/* make sure ops on existing fds can abort cleanly */
	spin_lock_irq(&spidev->spi_lock);
	spidev->spi = NULL;
	spi_set_drvdata(spi, NULL);
	spin_unlock_irq(&spidev->spi_lock);

	/* prevent new opens */
	mutex_lock(&device_list_lock);
	list_del(&spidev->device_entry);
	device_destroy(lcd.lcd_class, spidev->devt);
	clear_bit(MINOR(spidev->devt), minors);
	if (spidev->users == 0)
		kfree(spidev);
	mutex_unlock(&device_list_lock);

	return 0;
}



void lcd_reset(void)
{

    set_hi_sc_ctrl3_lcd_rst_n(1);
    mdelay(50);
    set_hi_sc_ctrl3_lcd_rst_n(0);
    mdelay(50);
    set_hi_sc_ctrl3_lcd_rst_n(1);
    mdelay(50);
}


#ifdef CONFIG_HAS_EARLYSUSPEND
void lcd_suspend_early(struct early_suspend *h)
{

    lcd_sleep(g_fbinfo);

}
void lcd_resume_late(struct early_suspend *h)
{
    lcd_light(g_fbinfo);

}


#endif
s32 stlcdfb_fill(struct fb_info *info,struct stlcdfb_par *par)
{
    struct fb_var_screeninfo *var = (struct fb_var_screeninfo *)&info->var;

    switch (par->fb_imgType) {
        case MDP_RGB_565:
            info->fix.type = FB_TYPE_PACKED_PIXELS;
            info->fix.xpanstep = 1;
            info->fix.ypanstep = 1;
            var->vmode = FB_VMODE_NONINTERLACED;
            var->blue.offset = 0;
            var->green.offset = 5;
            var->red.offset = 11;
            var->blue.length = 5;
            var->green.length = 6;
            var->red.length = 5;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.offset = 0;
            var->transp.length = 0;
            var->bits_per_pixel = 2 * 8;
        break;
        case MDP_RGB_888:
            info->fix.type = FB_TYPE_PACKED_PIXELS;
            info->fix.xpanstep = 1;
            info->fix.ypanstep = 1;
            var->vmode = FB_VMODE_NONINTERLACED;
            var->blue.offset = 0;
            var->green.offset = 8;
            var->red.offset = 16;
            var->blue.length = 8;
            var->green.length = 8;
            var->red.length = 8;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.offset = 0;
            var->transp.length = 0;
            var->bits_per_pixel = 3 * 8;
        break;
        case MDP_ARGB_8888:
            info->fix.type = FB_TYPE_PACKED_PIXELS;
            info->fix.xpanstep = 1;
            info->fix.ypanstep = 1;
            var->vmode = FB_VMODE_NONINTERLACED;
            var->blue.offset = 0;
            var->green.offset = 8;
            var->red.offset = 16;
            var->blue.length = 8;
            var->green.length = 8;
            var->red.length = 8;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.offset = 24;
            var->transp.length = 8;
            var->bits_per_pixel = 4 * 8;
            break;
         case MDP_RGBA_8888:
            info->fix.type = FB_TYPE_PACKED_PIXELS;
            info->fix.xpanstep = 1;
            info->fix.ypanstep = 1;
            var->vmode = FB_VMODE_NONINTERLACED;
            var->blue.offset = 8;
            var->green.offset = 16;
            var->red.offset = 24;
            var->blue.length = 8;
            var->green.length = 8;
            var->red.length = 8;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.offset = 0;
            var->transp.length = 8;
            var->bits_per_pixel = 4 * 8;
        break;
        case MDP_YCRYCB_H2V1:
            info->fix.type = FB_TYPE_INTERLEAVED_PLANES;
            info->fix.xpanstep = 2;
            info->fix.ypanstep = 1;
            var->vmode = FB_VMODE_NONINTERLACED;
            /* how about R/G/B offset? */
            var->blue.offset = 0;
            var->green.offset = 5;
            var->red.offset = 11;
            var->blue.length = 5;
            var->green.length = 6;
            var->red.length = 5;
            var->blue.msb_right = 0;
            var->green.msb_right = 0;
            var->red.msb_right = 0;
            var->transp.offset = 0;
            var->transp.length = 0;
            var->bits_per_pixel = 2 * 8;
        break;
        default:
            hilcd_error("stlcd_fb_init: fb %d unkown image type!\n",
                info->node);
            return LCD_ERROR;
        }
    return LCD_OK;
}

static s32 __devinit stlcdfb_probe (struct platform_device *pdev)
{
	struct fb_info *info;
	struct stlcdfb_par *par;
    struct fb_var_screeninfo *var = NULL;
	s32 retval = -ENOMEM;
    size_t size;
    u32 phys = 0;
    u32 product_type = 0xff;

    /*输入参数检查*/
    if(NULL == pdev)
    {
        return -ENOMEM;
    }

    /*分配一个fb_info结构体,结构体中通用指针*par为struct stlcdfb_par结构*/
	info = framebuffer_alloc(sizeof(struct stlcdfb_par), &pdev->dev);
	if (!info)
		return retval;

    /*info 作为pdev的drvdata*/
    platform_set_drvdata(pdev, info);

    /*par 挂接在info->par中*/
    par = info->par;

    /*初始化屏幕互斥信号量*/
    osl_sem_init(LCD_SEM_FULL, &(par->g_screensemmux));

    par->g_ulLcdPwrOnState = LCD_PWR_OFF_STATE;


    /*定义spi设备驱动*/
	par->spidrv.driver.name = "balong_lcd_spi";
	par->spidrv.probe    = lcd_spidev_probe;
	par->spidrv.remove   = __devexit_p(lcd_spidev_remove);
    /*创建lcd类*/
    lcd.lcd_class = class_create(THIS_MODULE, "lcd");
	if (IS_ERR(lcd.lcd_class)) {
        goto dealloc_fb;

	}

    /*注册spi设备驱动,匹配spi设备，构造master匹配boardlist中的boardinfo*/
	retval = spi_register_driver(&par->spidrv);
	if (retval < 0) {
		dev_err(&pdev->dev, "couldn't register SPI Interface\n");
		goto class_exit;

	}

    par->fb_page = FB_NUM;
    par->fb_imgType = MDP_RGB_565;
    par->ref_cnt = 0;
    par->lcd_backlight_registered = 0;

    /*初始化fb_info结构体*/

    strcpy(info->fix.id , "STLCD");

    info->fbops = &stlcdfb_ops;

    stlcdfb_fill(info,par);


    info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;
    var = (struct fb_var_screeninfo *)&info->var  ;
    var->xres = LCD_WIDTH +1;
    var->yres = LCD_HIGHT +1;
    var->xres_virtual = LCD_WIDTH+1;
    var->yres_virtual = var->yres * par->fb_page;


    par->var_xres = var->xres;
    par->var_yres = var->yres;
    par->var_pixclock = var->pixclock;

    info->fix.line_length = stlcd_fb_line_length(info->node, var->xres,var->bits_per_pixel/8);

    size = PAGE_ALIGN(info->fix.line_length * var->yres);
    size *= par->fb_page;

    par->vmem =  dma_alloc_coherent(NULL, size,(dma_addr_t *) &phys, GFP_KERNEL);
	if (!par->vmem){
        hilcd_error("unable to alloc fbmem size = %u\n",size);
		goto spi_exit;
	}

    info->screen_base = (u8 __force __iomem *)(par->vmem);/*(u8 __force __iomem *)*/
    info->fix.smem_start = phys;
    info->fix.smem_len = size;

    info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;
    product_type = bsp_version_get_board_chip_type();
    if((HW_VER_PRODUCT_UDP == product_type)||(HW_VER_PRODUCT_E5379 == product_type))
	  BACKLIGHT = GPIO_2_31;

	else
		BACKLIGHT = GPIO_0_12;

    par->bl.gpio = BACKLIGHT;


    par->led_cdev.name = "lcd-backlight";
    par->led_cdev.brightness = MAX_BACKLIGHT_BRIGNTNESS;
    par->led_cdev.brightness_set = balong_bl_set_intensity;

	par->info = info;
	par->pdev = pdev;
	g_fbinfo = info;

	retval = register_framebuffer(info);
	if (retval < 0)
		goto release_mem;

    if(!par->lcd_backlight_registered){
        if(led_classdev_register(&pdev->dev,&par->led_cdev))
            hilcd_error("led_classdev_register failed!");
        else
            par->lcd_backlight_registered = 1;
    }
    #ifdef CONFIG_FB_2_4_INCH_BALONG
    gpio_request(par->bl.gpio,"lcd_backlight");

    #elif defined CONFIG_FB_1_4_5_INCH_BALONG

     par->dr1_vcc = regulator_get(NULL, "BL-vcc");
    if (IS_ERR(par->dr1_vcc)) {
		dev_err(&pdev->dev, "Get %s fail\n", "BL-vcc");
		goto release_mem;
	}
    #endif
    par->lcd_vcc = regulator_get(NULL,"LCD-vcc");
    if (IS_ERR(par->lcd_vcc)) {
		dev_err(&pdev->dev, "Get %s fail\n", "LCD-vcc");
		goto release_mem;
	}
    #ifdef CONFIG_HAS_EARLYSUSPEND
    #ifdef CONFIG_FB_1_4_5_INCH_BALONG
    dr1_vcc = par->dr1_vcc;
    #endif
    par->earlysus_lcd.suspend = lcd_suspend_early;
    par->earlysus_lcd.resume = lcd_resume_late;
    par->earlysus_lcd.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;

    register_early_suspend(&(par->earlysus_lcd));
    #endif


    lcd_reset();

    lcd_pwron(info);
    balong_bl_init(&par->bl);
	hilcd_trace(
		"fb%d: %s frame buffer device,\n\tusing %d Byte of video memory\n",
		info->node, info->fix.id, info->fix.smem_len);

	return LCD_OK;

release_mem:
    kfree(par->vmem);
spi_exit:
    spi_unregister_driver(&par->spidrv);
class_exit:
     class_destroy(lcd.lcd_class);
dealloc_fb:
    platform_set_drvdata(pdev,NULL);
    framebuffer_release(info);
    osl_sema_delete(&(par->g_screensemmux));


	return retval;
}

static s32 __devexit stlcdfb_remove(struct platform_device *pdev)
{
	struct fb_info *info = platform_get_drvdata(pdev);
    struct stlcdfb_par *par = (struct stlcdfb_par *)info->par;

	platform_set_drvdata(pdev, NULL);
    if (info) {
		unregister_framebuffer(info);
		kfree(info->screen_base);
		framebuffer_release(info);
        osl_sema_delete(&(par->g_screensemmux));
	}
    if(par->lcd_backlight_registered){
        par->lcd_backlight_registered = 0;
        led_classdev_unregister(&par->led_cdev);

    }
    #ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&(par->earlysus_lcd));
    #endif
	return 0;
}
/*驱动注册*/
static struct platform_driver stlcdfb_driver = {
	.driver = {
		.name   = "stlcd_tft_fb",
		.owner  = THIS_MODULE,
	},

	.probe  = stlcdfb_probe,

	.remove = __devexit_p(stlcdfb_remove),
};
/*设备注册*/
static struct resource balong_lcd_resource[] = {
};
struct platform_device balong_device_lcd = {
    .name = "stlcd_tft_fb",
    .id = -1,
    .num_resources = ARRAY_SIZE(balong_lcd_resource),
    .resource = balong_lcd_resource,
};

/*#include "test/lcd_test.c"*/
static int __init stlcdfb_init(void)
{
	 platform_device_register(&balong_device_lcd);
	 platform_driver_register(&stlcdfb_driver);
	 return 0;

}

static void __exit stlcdfb_exit(void)
{
	platform_driver_unregister(&stlcdfb_driver);
	platform_device_unregister(&balong_device_lcd);
}

module_init(stlcdfb_init);
module_exit(stlcdfb_exit);

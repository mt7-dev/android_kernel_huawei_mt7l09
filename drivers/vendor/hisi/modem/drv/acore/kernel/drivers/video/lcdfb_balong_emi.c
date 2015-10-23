
/*lint --e{537,958,529,64,718,746}*/

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
#include <linux/regulator/consumer.h>
#include <linux/earlysuspend.h>
#include <linux/leds.h>
#include <asm/uaccess.h>

#include <bsp_memmap.h>
#include <hi_base.h>
#include <hi_syssc.h>
#include <bsp_edma.h>
#include <hi_emi.h>
#include <linux/emi/emi_balong.h>
#include <bsp_version.h>
#include <bsp_om.h>
#include <linux/video/lcdfb_balong.h>

#define UPDATE_LOCK_SCREEN_DELAY_TIME    50
#define ILI_LCD_EMI_MODE  EMI_TFT_8BIT

struct stlcdfb_par {
    struct platform_device *pdev;
    struct fb_info *info;
    u32 *vmem;
    struct regulator *lcd_vcc;
    u32 g_ulLcdPwrOnState;
    struct semaphore    g_screensemmux;
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

//struct semaphore	g_screen_light_semmux;


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

struct fb_info *g_fbinfo = NULL ;


void lcd_set_trace_level(u32 level)
{
    bsp_mod_level_set(BSP_MODU_LCD, level);
    printk("bsp_mod_level_set(BSP_MODU_LCD=%d, %d)\n",BSP_MODU_LCD,level);
}

struct fb_info g_fbinfo_get(void)
{
    return (struct fb_info )(*g_fbinfo);
}

#ifdef CONFIG_FB_2_4_INCH_BALONG
void set_gpio_updown_time(unsigned gpio,gpio_status status,u32 t_us)
{
    gpio_direction_output(gpio,status);
    udelay(t_us);/*lint !e737*/
}

void balong_bl_init(struct balong_bl *lcdbl)
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
#if 0
void gpio_backlight_test(void)
{
    int i = 0;
    struct fb_info info;
    struct stlcdfb_par *par;

    info = g_fbinfo_get();

    par = (struct stlcdfb_par *)(info.par);
    for(i = 1;i <= 100;i++)
    {
        set_gpio_updown_time(par->bl.gpio,GPIO_DOWN,5);
        set_gpio_updown_time(par->bl.gpio,GPIO_UP,5);
        msleep(300);
    }

}
#endif
#elif defined CONFIG_FB_1_4_5_INCH_BALONG
void balong_bl_init(struct balong_bl *lcdbl)
{
    regulator_enable(lcdbl->bl_vcc);
}
/*根据亮度值要求，设置不同电流*/
int balong_bl_current_set(struct balong_bl *lcdbl)
{
    int cur = 0;
    unsigned selector = 0;
    int iret = LCD_OK;

    if (lcdbl->intensity_past == lcdbl->intensity_cur)
    {
        hilcd_trace("the brightness set is not changed!\n");
    }
    else
    {
        selector = (unsigned)(lcdbl->intensity_cur);
        if(selector > 7)/*selector range : 0-7*/
            selector = 7;
        cur = regulator_list_voltage(lcdbl->bl_vcc,selector);
        iret = regulator_set_voltage(lcdbl->bl_vcc,cur,cur);
    }

    return iret;
}
#endif
int balong_bl_set_intensity(struct led_classdev *led_cladev,enum led_brightness value)
{
    /*lint --e{438,533}*/
    struct stlcdfb_par *par;
    struct balong_bl *lcdbl;
#ifdef CONFIG_FB_2_4_INCH_BALONG
    int pulse_num = 0;
#endif

    par = container_of(led_cladev, struct stlcdfb_par, led_cdev);
    if(NULL == par)
    {
        hilcd_trace("par is null after container_of led_cdev!\n");
        return LCD_ERROR;
    }
    lcdbl = (struct balong_bl *)(&par->bl);/*lint !e413*/
    if(NULL == lcdbl)
    {
        hilcd_trace("lcdbl is null !\n");
        return LCD_ERROR;
    }

    osl_sem_down(&(lcdbl->g_backlightsemmux));

    if(0 == value)
    {

        hilcd_trace("brightness is 0,bl will be closed!\n");
#ifdef CONFIG_FB_2_4_INCH_BALONG
        set_gpio_updown_time(lcdbl->gpio,GPIO_DOWN,35000);
#elif defined CONFIG_FB_1_4_5_INCH_BALONG
        if(regulator_is_enabled(lcdbl->bl_vcc))
            regulator_disable(lcdbl->bl_vcc);
#endif
        lcdbl->intensity_cur = 0;
        goto exit;

    }
#ifdef CONFIG_FB_2_4_INCH_BALONG
    if (0 == lcdbl->intensity_past)
        balong_bl_init(lcdbl);
    hilcd_trace("intensity_past = %d\n",lcdbl->intensity_past);

    lcdbl->intensity_cur = (value - 1)/16 + 1;
    hilcd_trace("intensity_cur = %d\n",lcdbl->intensity_cur);

    pulse_num = intensity_to_pulse_num(lcdbl);
    hilcd_trace("pulse_num is %d\n",pulse_num);

    balong_bl_pulse_output(lcdbl,pulse_num,5);

#elif defined CONFIG_FB_1_4_5_INCH_BALONG

    lcdbl->intensity_cur = (int)(value / 32);/*selector range : 0-7*/
    balong_bl_current_set(lcdbl);

    if (0 == lcdbl->intensity_past)
    {
        if(!regulator_is_enabled(lcdbl->bl_vcc))
            regulator_enable(lcdbl->bl_vcc);
    }
#endif
exit:
    lcdbl->intensity_past = lcdbl->intensity_cur;
    osl_sem_up(&(lcdbl->g_backlightsemmux));
    return LCD_OK;
}


void balong_backlight_set_intensity(struct led_classdev *led_cladev,enum led_brightness value)
{
    struct stlcdfb_par *par;
    struct balong_bl *lcdbl;

    par = container_of(led_cladev, struct stlcdfb_par, led_cdev);
    if(NULL == par)
    {
        hilcd_error("par is null after container_of led_cdev!\n");
        return;
    }
    lcdbl = (struct balong_bl *)(&par->bl);/*lint !e413*/
    if(NULL == lcdbl)
    {
        hilcd_error("lcdbl is null !\n");
        return;
    }
    if(!lcdbl->flag)
    {
        hilcd_error("lcd is not ready to light!\n");
        return;
    }
    (void)balong_bl_set_intensity(led_cladev,value);
}


static void set_bl_flag(struct stlcdfb_par *par);
static void set_bl_flag(struct stlcdfb_par *par)
{
    par->bl.flag = 1;
}

static void clear_bl_flag(struct stlcdfb_par *par);
static void clear_bl_flag(struct stlcdfb_par *par)/*lint !e402 !e752 !e830 */
{
    par->bl.flag = 0;
}

#if 0
static void stlcdfb_deferred_io(struct fb_info *info,
                struct list_head *pagelist)
{
        lcd_refresh(info,0,0,(info->var.xres - 1), (info->var.yres - 1),info->screen_base,info->fix.smem_len);
}
#endif

/* TODO hi_syssc.h*/
void lcd_reset(void)
{
    /*lint --e{737}*/
    set_hi_sc_ctrl3_lcd_rst_n(1);
    mdelay(50);/*lint !e62 */
    set_hi_sc_ctrl3_lcd_rst_n(0);
    mdelay(50);/*lint !e62 */
    set_hi_sc_ctrl3_lcd_rst_n(1);
    mdelay(50);/*lint !e62 */
}

 static int balong_lcd_toggle(struct balong_lcd_seq *seq, int sz)
{
         int count;
    u8 buf[1];

    for (count = 0; count < sz; count++)
    {
        buf[0] = seq[count].value;
        if(seq[count].format == 'c')
        {
            emiCmdOut8(buf[0]);  /* command */
        }
        else if(seq[count].format == 'd')
        {
            emiDataOut8(buf[0]); /* data */
        }
        else if(seq[count].format=='w')
        {
            mdelay((unsigned int)(seq[count].value)); /*lint !e62 !e737*/
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

void lcd_init_byid(u16 ucId)
{
    int ret = 0;
    if(LCD_ILITEK == ucId)
    {
        ret=balong_lcd_toggle(initseqtmi, ARRAY_SIZE(initseqtmi));/*lint !e84 !e30 !e409 !e506 !e656 !e516*/
        if(ret)
        {
            hilcd_error("lcd init error");
        }
        return;
    }
    else if(LCD_CHIMEI == ucId)
    {

        ret=balong_lcd_toggle(initseq, ARRAY_SIZE(initseq));/*lint !e84 !e30 !e409 !e506 !e656 !e516*/
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
void tftlcd_init(void)
{
    u16 ucId = 0;
    s32 retval = 0;

    retval = lcd_vectid_get(&ucId);
    if(retval)
    {
        return;
    }

    lcd_init_byid(ucId);
    return;
}

s32 lcd_init(void)
{
    s32 ret = 0;
    /* 设置时序 */
    if(ILI_LCD_EMI_MODE>= EMI_SCHEDULING_BUTT)
    {
        hilcd_error("EMI SCHDULING MODE IS VALID. \n");
        return LCD_ERROR;
    }
    /* I8080 和M6800 区分.LCD时序选择*/
    if(EMI_STN_8BIT_M6800 == ILI_LCD_EMI_MODE)
    {
        set_hi_sc_ctrl3_emi_sel(1);
    }
    else
    {
        set_hi_sc_ctrl3_emi_sel(0);
    }

    ret = emi_config(ILI_LCD_EMI_MODE);
    if(ret)
    {
       hilcd_error("emi_config failed!\n");
       return LCD_ERROR;
    }
    if((EMI_TFT_8BIT == ILI_LCD_EMI_MODE)||(EMI_TFT_9BIT == ILI_LCD_EMI_MODE))
    {
        tftlcd_init();
    }
    else
    {
        hilcd_error("do not support the kind of lcd\n");
        return LCD_ERROR;
    }
    return LCD_OK;

}


 void lcd_pwron(struct fb_info *info)
{
    struct stlcdfb_par *par = info->par;
    /*已是上电状态，无需重新上电*/
    if (LCD_PWR_ON_STATE == par->g_ulLcdPwrOnState)
    {
        hilcd_trace("lcd has been powered on.\n");
        return ;
    }
    msleep(110);/*100ms以上*/

    lcd_init();
    par->g_ulLcdPwrOnState = LCD_PWR_ON_STATE;
    /*初始化lcd后背光可操作*/
    set_bl_flag(par);
    return ;
}

static s32 balong_lcd_window_set(struct fb_info *info,u16 x0,u16 y0,u16 x1,u16 y1)
{
    u8 buf[1];

    if((x0>(info->var.xres-1))||(x1>(info->var.xres-1))||(y0>(info->var.yres-1))||(y1>(info->var.yres-1)))
    {
        hilcd_error("paramter error,[x0:%d][x1:%d][y0:%d][y1:%d]",
                    x0,x1,y0,y1);
        return -EINVAL;
    }
    hilcd_trace("balong_lcd_window_set:x_start = %d,y_start = %d,x_end = %d,y_end = %d",x0,y0,x1,y1);
    buf[0] = COLUMN_ADDRESS_SET;
    emiCmdOut8(buf[0]);

    buf[0] = (u8)(x0>>8);
    emiDataOut8(buf[0]); /*起始横坐标高8位 */

    buf[0] = (u8)x0;
    emiDataOut8(buf[0]); /* 起始横坐标低8位 */

    buf[0] = (u8)(x1>>8);
    emiDataOut8(buf[0]); /* data */

    buf[0] = (u8)x1;
    emiDataOut8(buf[0]); /* data */


    buf[0] = PAGE_ADDRESS_SET;
    emiCmdOut8(buf[0]);

    buf[0] = (u8)(y0>>8);
    emiDataOut8(buf[0]); /* data */

    buf[0] = (u8)y0;
    emiDataOut8(buf[0]); /* data */

    buf[0] = (u8)(y1>>8);
    emiDataOut8(buf[0]); /* data */

    buf[0] = (u8)y1;
    emiDataOut8(buf[0]); /* data */


    buf[0] = MEMORY_WRITE;
    emiCmdOut8(buf[0]);

    return LCD_OK;
}


s32 lcd_refresh(struct fb_info *info)
{

    struct stlcdfb_par *par = info->par;
    struct fb_var_screeninfo var = info->var;
    struct fb_fix_screeninfo fix = info->fix;
    s32 ret = 0;
    #if 0
    u32 xloop,yloop;
    u8 *pucbuf = info->screen_base + var.yoffset * fix.line_length +var.xoffset ;
    u8 *buffer = pucbuf;
    #endif
    dma_addr_t src_addr =fix.smem_start + var.yoffset * fix.line_length +var.xoffset;
    u32 len = (fix.line_length) * (var.yres);

    osl_sem_down(&(par->g_screensemmux));

    ret = balong_lcd_window_set(info,0, 0, (u16)(var.xres - 1), (u16)(var.yres - 1));
    if(ret){
        hilcd_error("lcd window set error");
        return ret;
    }

    #if 0
    /*CPU传输图像数据*/
    for (yloop = 0; yloop <= (info->var.yres - 1); yloop++)
    {

               pucbuf =buffer ;
              for (xloop = 0; xloop <= (info->var.xres - 1); xloop++)
              {
                  emiDataOut8(*pucbuf++);
                 emiDataOut8(*pucbuf++);
             }
            buffer += fix.line_length;
    }

    #endif
    /*EDMA传输图像数据*/
    emi_edma_transfer(src_addr,len);

    osl_sem_up(&(par->g_screensemmux));
    return LCD_OK;
}

s32 balong_lcd_sleep_in(void)
{
    emiCmdOut8(SLEEP_IN);
    return LCD_OK;
}

/* lcd out of sleep */
 s32 balong_lcd_sleep_out(void)
{
    emiCmdOut8(SLEEP_OUT);
    msleep(120);
    return LCD_OK;

}
 s32 lcd_light(struct fb_info *info)
{

    struct stlcdfb_par *par = info->par;
    s32 ret = 0;

    hilcd_trace("lcd will be light!");

    osl_sem_down(&(par->g_screensemmux));
    ret = balong_lcd_sleep_out();
     if(ret)
    {
        hilcd_error("balong_lcd_sleep_out error!");
        return ret;
    }
    emiCmdOut8(DISPLAY_ON);
    msleep(120);/*如果不sleep，屏幕会闪一下白*/
    osl_sem_up(&(par->g_screensemmux));

    balong_bl_set_intensity((struct led_classdev *)(&par->led_cdev),(enum led_brightness)par->led_cdev.brightness);
    set_bl_flag(par);

    return LCD_OK;

}

 s32 lcd_sleep(struct fb_info *info)
{

    s32 ret = 0;
    struct stlcdfb_par *par = info->par;

    hilcd_trace("lcd will sleep!");

    clear_bl_flag(par);

    balong_bl_set_intensity((struct led_classdev *)(&par->led_cdev),(enum led_brightness)0);

    osl_sem_down(&(par->g_screensemmux));
    ret = balong_lcd_sleep_in();
     if(ret)
    {
        hilcd_error("balong_lcd_sleep_in error!");
        return ret;
    }
    emiCmdOut8(DISPLAY_OFF);
    osl_sem_up(&(par->g_screensemmux));
    return LCD_OK;
}



 s32 balong_lcd_display_allblack(struct fb_info *info)
{
    struct stlcdfb_par *par = info->par;
    u32 count;
    s32 ret;
    u32 len = (info->fix.smem_len)/2; /* pixel format 565 */

    ret = balong_lcd_window_set(info,0,0,(u16)(info->var.xres-1),(u16)(info->var.yres-1));
    if(ret)
    {
        hilcd_error("lcd window set error");
        return ret;
    }
    osl_sem_down(&(par->g_screensemmux));

    for(count=0;(u32)count<len;count++)
    {

        emiDataOut8(0X00);
        emiDataOut8(0X00);
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

   hilcd_trace("blank_mode is %d",blank_mode);

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


s32 lcd_refresh_bybit(struct fb_info *info,u8 x0,u8 y0,u8 x1,u8 y1,const u8 * pbuffer)/*lint !e438 !e529 !e830 */
{
    struct stlcdfb_par *par = info->par;
    s32 ret = 0;
    u32 bitNum = 0;
    u8 xloop,yloop;
    u8 *buffer =(u8 *)pbuffer;

    if(NULL == pbuffer)
    {
        return LCD_ERROR;
    }

    /* set window before write image */
    osl_sem_down (&(par->g_screensemmux));

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
                emiDataOut8(0XFF);
                emiDataOut8(0XFF);
            }
            else
            {
                emiDataOut8(0X00);
                emiDataOut8(0X00);
            }
            bitNum++;
        }
    }

    osl_sem_up(&(par->g_screensemmux));
    return LCD_OK;
}


/* lcd display white at all window */
s32 balong_lcd_display_allwhite(struct fb_info *info)/*lint !e830 !e529 */
{
    u32 count;
    s32 ret = 0;

    u32 len = (info->fix.smem_len)/2;
    struct stlcdfb_par *par = info->par;
    ret = balong_lcd_window_set(info,0,0,(u16)(info->var.xres-1),(u16)(info->var.yres-1));
    if(ret)
    {
        hilcd_error("lcd window set error");
        return ret;
    }
    osl_sem_down(&(par->g_screensemmux));
    for(count=0;(unsigned int)count<len;count++)
    {

        emiDataOut8(0XFF);
        emiDataOut8(0XFF);
    }
    osl_sem_up(&(par->g_screensemmux));
    return LCD_OK;
}



static u32 stlcd_fb_line_length(int fb_index, u32 xres, u32 bpp)
{   /* The adreno GPU hardware requires that the pitch be aligned to
32 pixels for color buffers, so for the cases where the GPU
is writing directly to fb0, the framebuffer pitch
also needs to be 32 pixel aligned */
    /*if (fb_index == 0)
       return ALIGN(xres, 32) * bpp;
    else  */
        return (u32)(xres * bpp);
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
    if ((var->xres_virtual == 0) || (var->yres_virtual == 0))
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


/*根据当前传入的var，来修正当前的par模式，如果，当前的par参数和var参数不一致，则重新设置*/
int stlcdfb_set_par(struct fb_info *info)
{
    struct stlcdfb_par *par = (struct stlcdfb_par *)(info->par);
    struct fb_var_screeninfo *var = &info->var;
    u32 old_imgType;
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
    static int is_first_dispaly = 1;
    struct stlcdfb_par *par = (struct stlcdfb_par *)(info->par);

    if (var->xoffset > (info->var.xres_virtual - info->var.xres)){
        hilcd_trace("var.xoffset is unnormal and the value is %x\n",var->xoffset);
        return -EINVAL;
    }
    if (var->yoffset > (info->var.yres_virtual - info->var.yres)){
        hilcd_trace("var.yoffset is unnormal and the value is %x\n",var->yoffset);
        return -EINVAL;
    }
    if (info->fix.xpanstep)
        info->var.xoffset =
        (var->xoffset / info->fix.xpanstep) * info->fix.xpanstep;

    if (info->fix.ypanstep)
        info->var.yoffset =
        (var->yoffset / info->fix.ypanstep) * info->fix.ypanstep;

    /*为了保持动画连贯性，probe中不初始化lcd，保持fastboot中的图像，一直到应用刷新时才初始化*/
    if(is_first_dispaly)
    {
        //gpio_set_value(par->bl.gpio,0);
        balong_bl_set_intensity((struct led_classdev *)(&par->led_cdev),(enum led_brightness)0);
        lcd_reset();
        lcd_pwron(info);
        balong_lcd_display_allblack(info);
        balong_bl_set_intensity((struct led_classdev *)(&par->led_cdev),(enum led_brightness)255);
        //gpio_set_value(par->bl.gpio,1);

        is_first_dispaly = 0;
        hilcd_trace("LCD:the lcd is display for the first time!!\n");
    }
    lcd_refresh(info);

    return LCD_OK;
}



int stlcdfb_open(struct fb_info *info,int user)
{
    struct stlcdfb_par *par = (struct stlcdfb_par *)info->par;
    if(!par->ref_cnt){
        hilcd_trace("first open fb %d\n",info->node);
    }
    par->ref_cnt++;
    return LCD_OK;
}


int stlcdfb_release(struct fb_info *info,int user)
{
    struct stlcdfb_par *par = (struct stlcdfb_par *)info->par;

    if(!par->ref_cnt){
        hilcd_error("try to close unopened fb %d\n",info->node);
        return -EINVAL;
        }
    par->ref_cnt--;
    if(!par->ref_cnt){
         hilcd_trace("last close fb %d\n",info->node);
    }
    return LCD_OK;
}

static struct fb_ops stlcdfb_ops = {
    .owner      = THIS_MODULE,
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




#ifdef CONFIG_HAS_EARLYSUSPEND
void lcd_suspend_early(struct early_suspend *h)
{
    hilcd_trace("lcd suspend entered!\n");
    lcd_sleep(g_fbinfo);
}
void lcd_resume_late(struct early_suspend *h)
{
    hilcd_trace("lcd resume entered!\n");
	mdelay(UPDATE_LOCK_SCREEN_DELAY_TIME);
    lcd_light(g_fbinfo);
}
#endif


s32 stlcdfb_fill(struct fb_info *info,struct stlcdfb_par *par)
{
    struct fb_var_screeninfo *var = &info->var;


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
            var->bits_per_pixel= 2 * 8;
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
    struct fb_var_screeninfo *var ;//= &info->var
    size_t lcd_size;
    u32 phys = 0;
    s32 retval = -ENOMEM;

#if defined CONFIG_FB_2_4_INCH_BALONG
    u32 product_type = 0xff;
#endif
    int ret = 0;


    /*输入参数检查*/
    if(NULL == pdev)
    {
        return -ENOMEM;
    }

    /*分配一个fb_info结构体,结构体中通用指针*par为struct stlcdfb_par结构*/
    info = framebuffer_alloc(sizeof(struct stlcdfb_par), &pdev->dev);
    if (!info)
        return -ENOMEM;

    /*info 作为pdev的drvdata*/
    platform_set_drvdata(pdev, info);

    /*par 挂接在info->par中*/
    par = info->par;

    /*初始化屏幕互斥信号量*/
    osl_sem_init(LCD_SEM_FULL, &(par->g_screensemmux));

    /*填充用户需要的par参数*/
    par->g_ulLcdPwrOnState = LCD_PWR_OFF_STATE;
    par->fb_page = FB_NUM;/*支持双buffer*/
    par->fb_imgType = MDP_RGB_565;
    par->ref_cnt = 0;
    par->lcd_backlight_registered = 0;

     /*初始化fb_info结构体*/
    strncpy(info->fix.id , "STLCD", sizeof(info->fix.id));

    info->fbops = &stlcdfb_ops;

    /*根据用户需要的颜色格式,填充部分info和par结构的值*/
    retval = stlcdfb_fill(info,par);
    if(retval)
    {
        hilcd_error("stlcdfb_fill error!\n");
        goto exit;
    }

    info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;

    var = (struct fb_var_screeninfo *)&info->var;
    var->xres = LCD_WIDTH +1;
    var->yres = LCD_HIGHT +1;
    var->xres_virtual = LCD_WIDTH+1;
    var->yres_virtual = var->yres * par->fb_page;


    par->var_xres = var->xres;
    par->var_yres = var->yres;
    par->var_pixclock = var->pixclock;


    info->fix.line_length = stlcd_fb_line_length(info->node, var->xres,var->bits_per_pixel/8);

    lcd_size = info->fix.line_length * var->yres *par->fb_page;
    par->vmem =  dma_alloc_coherent(NULL, lcd_size,(dma_addr_t *) &phys, GFP_KERNEL);
    if (!par->vmem){
        hilcd_error("unable to alloc fbmem size = %u\n",lcd_size);
        goto exit;
    }

    info->screen_base = (char *)(par->vmem);/*(u8 __force __iomem *)*/
    info->fix.smem_start = phys;
    info->fix.smem_len = lcd_size;
    hilcd_trace("framebuffer address_virt = %x;address_phys = %x;len = %d",
        info->screen_base,phys,lcd_size);
    info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;
#if defined CONFIG_FB_2_4_INCH_BALONG
    product_type = bsp_version_get_board_chip_type();
    if((HW_VER_PRODUCT_UDP == product_type)||(HW_VER_PRODUCT_E5379 == product_type))
        par->bl.gpio = GPIO_2_31;

    else
        par->bl.gpio = GPIO_0_12;

    ret = gpio_request(par->bl.gpio, "lcd_backlight");
    if(ret < 0)
    {
        hilcd_error("gpio_request failed!\n");
    }
#elif defined CONFIG_FB_1_4_5_INCH_BALONG
    par->bl.bl_vcc = regulator_get(NULL, "BL-vcc");
    ret = IS_ERR(par->bl.bl_vcc);
    if(ret)
    {
        hilcd_error("dr BL-vcc failed!\n");
    }
#endif
    if(ret)
    {
        goto release_mem_exit;
    }
    par->led_cdev.name = "lcd-backlight";
    par->led_cdev.brightness = MAX_BACKLIGHT_BRIGNTNESS;
    //par->led_cdev.brightness_set = balong_bl_set_intensity;
    par->led_cdev.brightness_set = balong_backlight_set_intensity;

    /*初始化背光可操作标志*/
    par->bl.flag = 0;
    /*初始化背光互斥信号量*/
    osl_sem_init(LCD_SEM_FULL, &(par->bl.g_backlightsemmux));

   /*par info pdev 之间的联系定义*/
    par->info = info;
    par->pdev = pdev;
    g_fbinfo = info;
    /*注册framebuffer设备*/
    retval = register_framebuffer(info);
    if (retval < 0)
        goto early_exit;

    if(!par->lcd_backlight_registered){
        if(led_classdev_register(&pdev->dev,&par->led_cdev))
            hilcd_error("led_classdev_register failed!");
        else
            par->lcd_backlight_registered = 1;
        }

    /*低功耗earlysuspend,resumelate函数挂接*/
    #ifdef CONFIG_HAS_EARLYSUSPEND

        par->earlysus_lcd.suspend = lcd_suspend_early;
        par->earlysus_lcd.resume = lcd_resume_late;
        par->earlysus_lcd.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;

        register_early_suspend(&(par->earlysus_lcd));
    #endif

    //lcd_reset();

    //lcd_pwron(info);
    balong_bl_init(&par->bl);

    hilcd_trace(
        "fb%d: %s frame buffer device,\n\tusing %d Byte of video memory\n",
        info->node, info->fix.id, info->fix.smem_len);

    return 0;

early_exit:
    osl_sema_delete(&(par->bl.g_backlightsemmux));
release_mem_exit:/*lint !e563*/
    dma_free_coherent(NULL, lcd_size,par->vmem, phys);
exit:
    osl_sema_delete(&(par->g_screensemmux));
    platform_set_drvdata(pdev,NULL);
    framebuffer_release(info);
    return retval;
}

static s32 __devexit stlcdfb_remove(struct platform_device *pdev)
{
    struct fb_info *info = platform_get_drvdata(pdev);
    struct stlcdfb_par *par;

    if(NULL == info)
    {
        pr_err("%s %d platform_get_drvdata info is NULL\n", __func__, __LINE__);
        return -1;
    }
    par = (struct stlcdfb_par *)info->par;
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
#if 0
    .num_resources = ARRAY_SIZE(balong_lcd_resource),/*lint !e84 !e516 !e30*/
    .resource = balong_lcd_resource,
#endif
};/*lint !e830 !e529 !e813 */

static int __init stlcdfb_init(void)
{
    int ret = 0 ;
    ret = platform_device_register(&balong_device_lcd);
    if(ret)
    {
        hilcd_error("platform_device_register failed");
        return ret;
    }
    ret = platform_driver_register(&stlcdfb_driver);
    if(ret)
    {
        hilcd_error("platform_driver_register failed");
        return ret;
    }

    return 0;
}

static void __exit stlcdfb_exit(void)
{
    platform_driver_unregister(&stlcdfb_driver);
    platform_device_unregister(&balong_device_lcd);
}

module_init(stlcdfb_init);
module_exit(stlcdfb_exit);


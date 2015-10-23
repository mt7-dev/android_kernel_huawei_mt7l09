#include <linux/video/lcd_balong_busstress.h>
#include <linux/video/lcdfb_balong.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <bsp_busstress.h>
#include <bsp_softtimer.h>
#include <osl_thread.h>
#ifndef LCD_ERROR
#define LCD_ERROR (-1)
#endif

#ifndef LCD_OK
#define LCD_OK (0)
#endif

#ifndef TEST_STOP
#define TEST_STOP (0)
#endif

#ifndef TEST_RUN
#define TEST_RUN (1)
#endif


#define LCD_SEM_FULL (1)
#define LCD_SEM_EMPTY (0)

#ifdef CONFIG_FB_2_4_INCH_BALONG
#define LEN 153600
char *gImage = hehua;
#elif defined CONFIG_FB_1_4_5_INCH_BALONG
#define LEN 32768
char *gImage = sprint;
#endif
#ifndef CFG_CONFIG_MODULE_BUSSTRESS
s32 lcd_stress_test_stat = 0;
#endif
u32 lcd_test_interval = 0;
struct fb_info info;

u32 counter;    /* 计数器，压力测试程序正常运行的时候，counter会自加，表示程序正在run */

extern int get_test_switch_stat(int module_id);
extern struct fb_info g_fbinfo_get(void);
extern s32 lcd_refresh(struct fb_info * info);

void set_lcd_test_stat(int run_or_stop)
{
    #ifdef CFG_CONFIG_MODULE_BUSSTRESS
    set_test_switch_stat(LCD,run_or_stop);
    #else
	lcd_stress_test_stat = run_or_stop;
    #endif

}
int get_lcd_test_stat(void)
{
    #ifdef CFG_CONFIG_MODULE_BUSSTRESS
    printk("%d\n", counter);
    return get_test_switch_stat(LCD);
    #else
    return lcd_stress_test_stat;
    #endif
}
void drawRect (char * des, char *src, u32 len)
{
    memcpy(des,src,len);
}

s32 lcd_stress_test_routine(void)
{
    int i= 0;

    info = g_fbinfo_get();

    while(get_lcd_test_stat()){

        msleep(lcd_test_interval);

        drawRect (info.screen_base , gImage,LEN);
        lcd_refresh(&(info));
        msleep(lcd_test_interval);
        drawRect (info.screen_base , gImage_red,LEN);
        lcd_refresh(&(info));
        msleep(lcd_test_interval);
        drawRect (info.screen_base , gImage_green,LEN);
        lcd_refresh(&(info));
        counter++;
    }
    return 0;
}


s32 lcd_emi_edma_stress_test_start(u32 interval)//(s32 task_priority,s32 dtime)
{
    set_lcd_test_stat(TEST_RUN);
    lcd_test_interval = interval;
    counter = 0;
    kthread_run(lcd_stress_test_routine,0,"LcdStressTask");

    return LCD_OK;
}

s32 lcd_emi_edma_stress_test_stop()
{
    set_lcd_test_stat(TEST_STOP);

    msleep(2000);
	return LCD_OK;
}

 void lcd_refresh_test(void)
{
    info = g_fbinfo_get();
    memcpy(info.screen_base,gImage,LEN);
    lcd_refresh(&info);
}

EXPORT_SYMBOL(lcd_emi_edma_stress_test_start);
EXPORT_SYMBOL(lcd_emi_edma_stress_test_stop);



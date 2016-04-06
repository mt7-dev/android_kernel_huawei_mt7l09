

#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/thread_info.h>
#include <linux/syslog.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <asm/current.h>
#include <asm/string.h>
#include <asm/traps.h>
#include "product_config.h"
#include "osl_types.h"
#include "DrvInterface.h"
#include "bsp_dump.h"
#include "bsp_dump_drv.h"
#include "bsp_om_api.h"
#include "bsp_memmap.h"

static char* g_dump_st_buffer;
static int   g_dump_st_len;
extern dump_nv_s g_dump_cfg;

enum exchS
{
    RESET,
    UNDEF,
    SWI,
    PABT,
    DABT,
    DIV,
    LOOP
};

typedef void (*FUNC_VOID)(void);

int exchRESET(void* data)
{
    FUNC_VOID a = 0;/*lint !e611 !e740*/

    a();

    return 0;
}

int exchUNDEF(void* data)
{
    int b = 0;
    FUNC_VOID a = (FUNC_VOID)(&b);/*lint !e611 !e740*/

    b = 0xffffffff;
    a();

    return 0;
}

int exchSWI(void* data)
{
    msleep(20000);
    __asm("        SWI   0x1234   ");

    return 0;
}

int exchPABT(void* data)
{
    FUNC_VOID a = (FUNC_VOID)0xF0000000;

    a();

    return 0;
}

int exchDABT(void* data)
{
    int a;
    a = *(int *)0x0;
    *(int *)0x0 = 0x12345678;

    return 0;
}

int exchLOOP(void* data)
{
    for (;;)
    {
        ;
    }
}

int exchDIV(void* a)
{
    int b = 4;

    return b / (int)a;
}

void exchTest(int type)
{
    switch (type)
    {
    case RESET:
        kthread_run(exchRESET, 0, "exchRESET");
        break;
    case UNDEF:
        kthread_run(exchUNDEF, 0, "exchUNDEF");
        break;
    case SWI:
        kthread_run(exchSWI, 0, "exchSWI");
        break;
    case PABT:
        kthread_run(exchPABT, 0, "exchPABT");
        break;
    case DABT:
        kthread_run(exchDABT, 0, "exchDABT");
        break;
    case DIV:
        kthread_run(exchDIV, 0, "exchDIV");
        break;
    case LOOP:
        {
            u32 pid;
            struct sched_param  param;
            pid = (u32)kthread_run(exchLOOP, 0, "exchLOOP");
        	if (IS_ERR((void*)pid))
        	{
        		printk("dump_init_phase2: create kthread task dump_save failed!\n");
        		return;
        	}

            sched_setscheduler((struct task_struct*)pid, SCHED_FIFO, &param);
        }
        break;
    default:
        break;
    }
}
/* 模式控制 EXC DUMP模式 */
int dump_st_001(void)
{
    system_error(0x72, 1, 2, 0, 0);
    return 0;
}


/* 模式控制 USB DUMP模式 */
int dump_st_002(void)
{
    system_error(0x72, 1, 2, 0, 0);
    return 0;
}

/* RESET错误 */
int dump_st_003(void)
{
//    exchTest(RESET);
    exchRESET(NULL);
    return 0;
}


/* UNDEF错误 */
int dump_st_004(void)
{
//    exchTest(UNDEF);
    exchUNDEF(NULL);
    return 0;
}


/* SWI错误 */
int dump_st_005(void)
{
    exchTest(SWI);
//    exchSWI();
    return 0;
}


/* Prefetch abort错误 */
int dump_st_006(void)
{
//    exchTest(PABT);
    exchPABT(NULL);
    return 0;
}

/* Data abort错误 */
int dump_st_007(void)
{
//    exchTest(DABT);
    exchDABT(NULL);
    return 0;
}

/* 除0错误 */
int dump_st_008(void)
{
    exchDIV(0);
    return 0;
}

/* 核心过流 */
int dump_st_009(void)
{
    return 0;
}

/* 非核心过流 */
int dump_st_010(void)
{
    return 0;
}

/* 看门狗超时错误 */
int dump_st_011(void)
{
    return 0;
}

/* A核软件异常 */
int dump_st_012_013(void)
{
    return 0;
}

/* A核软件异常之后基本信息 */
int dump_st_015_025(void)
{
    return 0;
}

/* A核用户数据 */
int dump_st_026(void)
{
    char *p;
    p = kmalloc(4096, GFP_KERNEL);
    memset(p,1,4096);
    system_error(0x88,1,2,p,4096);
    return 0;
}

void dump_st_hook(void)
{
    s32 ret;
    int len = g_dump_st_len>>1;

    printk("dump_st_hook enter \n");
    memset(g_dump_st_buffer, 0xaa, len);
    memset(g_dump_st_buffer+len, 0xbb, len);
    ret = bsp_dump_save_file(DUMP_SAVE_FILE_OM, g_dump_st_buffer, len, DUMP_SAVE_FILE_MODE_CREATE);
    if(ret)
    {
        printk("save file failed.  test case 28 failed! %d\n", ret);
    }
    ret = bsp_dump_save_file(DUMP_SAVE_FILE_DIAG, g_dump_st_buffer, len, DUMP_SAVE_FILE_MODE_CREATE);
    if(ret)
    {
        printk("save file failed.  test case 28 failed! %d\n", ret);
    }
    ret = bsp_dump_save_file(DUMP_SAVE_FILE_DIAG, g_dump_st_buffer+len, len, DUMP_SAVE_FILE_MODE_APPEND);
    if(ret)
    {
        printk("save file failed.  test case 28 failed! %d\n", ret);
    }
}

/* A核扩展数据 */
int dump_st_028(void)
{
    int ret;

    ret = bsp_dump_get_buffer(DUMP_SAVE_MOD_USB, &g_dump_st_buffer, &g_dump_st_len);
    if(BSP_OK != ret)
    {
        printk("get buffer failed.  test case 28 failed! \n");
        return BSP_ERROR;
    }
    printk("buffer=0x%x, len=0x%x\n", (s32)g_dump_st_buffer, (s32)g_dump_st_len);

    ret = bsp_dump_register_hook(DUMP_SAVE_MOD_USB, (dump_save_hook)dump_st_hook);
    if(BSP_OK != ret)
    {
        printk("register_hook failed.  test case 28 failed! \n");
        return BSP_ERROR;
    }

    msleep(3000);
    system_error(0,0,0,0,0);

    return 0;
}


void dump_st_029(void)
{
    g_dump_cfg.dump_cfg.Bits.appRegSave1= 1;
    g_dump_cfg.dump_cfg.Bits.appRegSave2= 1;
    g_dump_cfg.dump_cfg.Bits.appRegSave3= 1;
    g_dump_cfg.appRegAddr1 = HI_APP_GIC_BASE_ADDR_VIRT;
    g_dump_cfg.appRegSize1 = HI_APP_GIC_SIZE;
    g_dump_cfg.appRegAddr2 = HI_SYSCTRL_BASE_ADDR_VIRT;
    g_dump_cfg.appRegSize2 = HI_SYSCTRL_REG_SIZE;
    g_dump_cfg.appRegAddr3 = HI_WDT_BASE_ADDR_VIRT;
    g_dump_cfg.appRegSize3 = HI_WDT_REG_SIZE;
}


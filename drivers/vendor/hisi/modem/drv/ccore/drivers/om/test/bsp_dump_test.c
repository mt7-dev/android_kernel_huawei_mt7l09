
#include <vxWorks.h>
#include <stdlib.h>
#include <logLib.h>
#include <stdio.h>
#include <errno.h>
#include <errnoLib.h>
#include <memLib.h>
#include <string.h>
#include <taskLib.h>
#include <intLib.h>
#include <vxWind.h>
#include <vxCpuLib.h>
#include <dbgLib.h>
#include <trcLib.h>
#include <sysSymTbl.h>
#include <taskHookLib.h>
#include <private/cplusLibP.h>
#include <private/dbgLibP.h>
#include <private/vxdbgLibP.h>
#include <private/vxdbgBpMsgQLibP.h>
#include <private/excLibP.h>
#include <private/rtpLibP.h>
#include <private/taskLibP.h>
#include <private/trcLibP.h>
#include <private/shellInternalLibP.h>
#include <private/eventLibP.h>
#include <symLib.h>
#include <taskLibCommon.h>
#include <a_out.h>
#include <vmLib.h>

#include "product_config.h"
//#include "osl_io.h"
#include "osl_types.h"
#include "DrvInterface.h"
#include "bsp_om_api.h"
#include "bsp_dump.h"
#include "bsp_dump_drv.h"
#include "bsp_memmap.h"



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

void exchRESET(void);
void exchUNDEF(void);
void exchSWI(void);
void exchPABT(void);
void exchDABT(void);
void exchLOOP(void);
int  exchDIV(int a);
void exchTest(int type);
int dump_st_003(void);
int dump_st_004(void);
int dump_st_005(void);
int dump_st_006(void);
int dump_st_007(void);
int dump_st_008(void);
int dump_st_009(void);
int dump_st_010(void);
int dump_st_011(void);
int dump_st_012_013(void);
int dump_st_015_025(void);
int dump_st_026(void);
void dump_st_hook(void);
int dump_st_028(void);
void dump_st_029(void);
void dump_exc_task_test(void);
void dump_st_030(void);

static char* g_dump_st_buffer;
static int   g_dump_st_len;

void exchRESET(void)
{
    /* coverity[assign_zero] */
    FUNC_VOID a = 0;/*lint !e611 !e740*/

    /* coverity[var_deref_op] */
    a();
}

void exchUNDEF(void)
{
    int b = 0;
    FUNC_VOID a = (FUNC_VOID)(&b);/*lint !e611 !e740*/

    b = 0xffffffff;
    a();
}

void exchSWI(void)
{
    __asm("        SWI   0x1234   ");
}

void exchPABT(void)
{
    FUNC_VOID a = (FUNC_VOID)0xF0000000;

    a();
}

void exchDABT(void)
{
    *(int *)0xF0000000 = 0x12345678;
}

void exchLOOP(void)
{
    /* coverity[no_escape] */
    for (;;)
    {
        ;
    }
}

int exchDIV(int a)
{
    int b = 4;

    return b / a;
}

void exchTest(int type)
{
    switch (type)
    {
    case RESET:
        (BSP_VOID)taskSpawn("exchRESET", 99, 0, 0x1000, (FUNCPTR)exchRESET, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        break;
    case UNDEF:
        (BSP_VOID)taskSpawn("exchUNDEF", 99, 0, 0x1000, (FUNCPTR)exchUNDEF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        break;
    case SWI:
        (BSP_VOID)taskSpawn("exchSWI", 99, 0, 0x1000, (FUNCPTR)exchSWI, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        break;
    case PABT:
        (BSP_VOID)taskSpawn("exchPABT", 99, 0, 0x1000, (FUNCPTR)exchPABT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        break;
    case DABT:
        (BSP_VOID)taskSpawn("exchDABT", 99, 0, 0x1000, (FUNCPTR)exchDABT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        break;
    case DIV:
        (BSP_VOID)taskSpawn("exchDIV", 99, 0, 0x1000, (FUNCPTR)exchDIV, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        break;
    case LOOP:
        (BSP_VOID)taskSpawn("exchLOOP", 253, 0, 0x1000, (FUNCPTR)exchLOOP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        break;
    default:
        break;
    }
}


/* RESET错误 */
int dump_st_003(void)
{
    exchTest(RESET);
//    exchRESET();
    return 0;
}


/* UNDEF错误 */
int dump_st_004(void)
{
    exchTest(UNDEF);
//    exchUNDEF();
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
    exchTest(PABT);
//    exchPABT();
    return 0;
}

/* Data abort错误 */
int dump_st_007(void)
{
    exchTest(DABT);
//    exchDABT();
    return 0;
}

/* 除0错误 */
int dump_st_008(void)
{
    exchTest(DIV);
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
    p = malloc(4096);
    if(p == NULL)
    {
        return 0;
    }
    
    memset(p,1,4096);
    system_error(0x88,1,2,p,4096);
    return 0;
}


void dump_st_hook(void)
{
    int len = g_dump_st_len>>1;

    printf("dump_st_hook enter \n");
    memset(g_dump_st_buffer, 0xcc, len);
    memset(g_dump_st_buffer+len, 0xdd, len);
}

/* C核扩展数据 */
int dump_st_028(void)
{
    int ret;

    ret = bsp_dump_get_buffer(DUMP_SAVE_MOD_UTRACE, &g_dump_st_buffer, (u32*)&g_dump_st_len);
    if(BSP_OK != ret)
    {
        printf("get buffer failed.  test case 28 failed! \n");
        return BSP_ERROR;
    }
    printf("buffer=0x%x, len=0x%x\n", (u32)g_dump_st_buffer, g_dump_st_len);

    ret = bsp_dump_register_hook(DUMP_SAVE_MOD_UTRACE, (dump_save_hook)dump_st_hook);
    if(BSP_OK != ret)
    {
        printf("register_hook failed.  test case 28 failed! \n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

extern dump_nv_s g_dump_cfg;
void dump_st_029(void)
{
    g_dump_cfg.dump_cfg.Bits.commRegSave1= 1;
    g_dump_cfg.dump_cfg.Bits.commRegSave2= 1;
    g_dump_cfg.dump_cfg.Bits.commRegSave3= 1;
    g_dump_cfg.commRegAddr1 = HI_EDMA_REGBASE_ADDR_VIRT;
    g_dump_cfg.commRegSize1 = HI_EDMA_REG_SIZE;
    g_dump_cfg.commRegAddr2 = HI_SYSCTRL_BASE_ADDR_VIRT;
    g_dump_cfg.commRegSize2 = HI_SYSCTRL_REG_SIZE;
    g_dump_cfg.commRegAddr3 = HI_SCI1_REGBASE_ADDR_VIRT;
    g_dump_cfg.commRegSize3 = HI_SCI1_REG_SIZE;
}

void dump_exc_task_test(void)
{
    printf("%s: enter\n", __FUNCTION__);
    system_error(0x100, 0, 0, 0, 0);
}

void dump_st_030(void)
{
    (BSP_VOID)taskSpawn("exc_task_test", 99, 0, 0x1000, (FUNCPTR)dump_exc_task_test, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}


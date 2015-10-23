#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/huawei/rdr.h>
#include <product_config.h>
#include <drv_reset.h>
#include <bsp_reset.h>
#ifdef CONFIG_HIFI_RESET
#ifndef RESET_CBFUNC_PRIO_LEVEL_LOWT
#define RESET_CBFUNC_PRIO_LEVEL_LOWT    (0)
#endif

#ifndef RESET_CBFUNC_PRIO_LEVEL_HIGH
#define RESET_CBFUNC_PRIO_LEVEL_HIGH    (49)
#endif

#define RESET_CBFUN_IGNORE_NAME         "NONAME"
#define BSP_RESET_OK                    0
#define BSP_RESET_ERROR                 (-1)

typedef int (*pdrv_reset_cbfun)(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata);
/*Record information of callback functions*/
typedef struct
{
    char                name[9 + 1];
    int                         priolevel;
    pdrv_reset_cbfun    cbfun;
    int         userdata;
} sreset_mgr_callbackfunc;

/*Link list unit of managing all callback functions*/
typedef struct _sreset_mgr_LLI
{
    sreset_mgr_callbackfunc cbfuninfo;
    struct _sreset_mgr_LLI      *pnext;
} sreset_mgr_LLI;

/*Link used for hifi reset*/
sreset_mgr_LLI  *g_pmgr_hifireset_data = NULL;

/*****************************************************************************
 函 数 名  : reset_link_insert
 功能描述  : 将数据插入链表
 输入参数  : sreset_mgr_LLI *plink, 链表指针
             sreset_mgr_LLI *punit，待插入的节点指针
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
sreset_mgr_LLI * reset_link_insert(sreset_mgr_LLI *plink, sreset_mgr_LLI *punit)
{
    sreset_mgr_LLI    *phead = plink;
    sreset_mgr_LLI    *ppose = plink;
    sreset_mgr_LLI    *ptail = plink;

    if (NULL == plink || NULL == punit)
    {
        return NULL;
    }
    while (NULL != ppose)
    {
        /*根据优先级插入到链表中*/
        if (ppose->cbfuninfo.priolevel > punit->cbfuninfo.priolevel)
        {
            if (phead == ppose)
            {
                punit->pnext = ppose;
                phead = punit;
            }
            else
            {
                ptail->pnext = punit;
                punit->pnext = ppose;
            }
            break;
        }
        ptail = ppose;
        ppose = ppose->pnext;
    }
    if (NULL == ppose)
    {
        ptail->pnext = punit;
    }
    return phead;
}

/*****************************************************************************
 函 数 名  : reset_do_regcbfunc
 功能描述  : 用于其它组件注册回调函数，处理Modem复位前后相关数据。
 输入参数  :
         sreset_mgr_LLI *plink,管理链表，注意，允许为空.
            const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
sreset_mgr_LLI * reset_do_regcbfunc(sreset_mgr_LLI *plink, const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    sreset_mgr_LLI  *phead = plink;
    sreset_mgr_LLI  *pmgr_unit = NULL;

    /*判断入参是否合法，不合法返回错误*/
    if (NULL == pname
        || NULL == pcbfun
        || (priolevel < RESET_CBFUNC_PRIO_LEVEL_LOWT || priolevel > RESET_CBFUNC_PRIO_LEVEL_HIGH))
    {
        printk(KERN_ERR "%s: fail in ccore reset regcb,fail, name 0x%x, cbfun 0x%x, prio %d\n", __FUNCTION__, \
                        (unsigned int)pname, (unsigned int)pcbfun, priolevel);
        return NULL;
    }

    /*分配空间*/
    pmgr_unit = (sreset_mgr_LLI*)kmalloc(sizeof(sreset_mgr_LLI), GFP_KERNEL);
    if (NULL != pmgr_unit)
    {
        memset((void*)pmgr_unit, 0, (sizeof(sreset_mgr_LLI)));
        /*赋值*/
        strncpy(pmgr_unit->cbfuninfo.name, pname, 9);
        pmgr_unit->cbfuninfo.priolevel = priolevel;
        pmgr_unit->cbfuninfo.userdata = userdata;
        pmgr_unit->cbfuninfo.cbfun = pcbfun;
    }

    /*第一次调用该函数，链表为空*/
    if (NULL == phead)
    {
        phead = pmgr_unit;
    }
    else
    {
    /*根据优先级插入链表*/
        phead = reset_link_insert(phead, pmgr_unit);
    }
    return phead;
}

/*****************************************************************************
 函 数 名  : hifireset_regcbfunc
 功能描述  : 用于其它组件注册回调函数，处理HIFI复位前后相关数据。
 输入参数  : const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int hifireset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    g_pmgr_hifireset_data = reset_do_regcbfunc(g_pmgr_hifireset_data, pname, pcbfun, userdata, priolevel);
    printk(KERN_INFO "%s: %s registered a cbfun for hifi reset\n", __FUNCTION__, pname);
    return 0;
}

/*****************************************************************************
 函 数 名  :  hifireset_doruncbfun
 功能描述  : HIFI复位前后调用回调函数的函数。由于圈复杂度超标，所以这里封装函数
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam, 0 表示HIFI复位前；非零表示复位后。

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int hifireset_doruncbfun (const char *pname, DRV_RESET_CALLCBFUN_MOMENT eparam)
{
    int  iresult = BSP_RESET_OK;

    sreset_mgr_LLI  *phead = g_pmgr_hifireset_data;
    BUG_ON(NULL == pname);
    /*不判断模块名字,按顺序执行*/
    if (strcmp(pname, RESET_CBFUN_IGNORE_NAME) == 0)
    {
        while (NULL != phead)
        {
            if (NULL != phead->cbfuninfo.cbfun)
            {
                iresult = phead->cbfuninfo.cbfun(eparam, phead->cbfuninfo.userdata);
                if (BSP_RESET_OK != iresult)
                {
                    /*如果返回失败，记录下组件名字,返回值*/
                    printk(KERN_ERR "%s: fail to run cbfun of %s, at %d return %d\n", __FUNCTION__, phead->cbfuninfo.name, eparam, iresult);
                    break;
                }
                printk(KERN_INFO "%s: run %s cb function 0x%x\n", __FUNCTION__, phead->cbfuninfo.name, (unsigned int)phead->cbfuninfo.cbfun);
            }
            phead = phead->pnext;
        }
    }
    else/*需要判断模块名字，执行指定的回调函数*/
    {
        while (NULL != phead)
        {
            if (strcmp(pname, phead->cbfuninfo.name) == 0
                && NULL != phead->cbfuninfo.cbfun)
            {
                iresult  = phead->cbfuninfo.cbfun(eparam, phead->cbfuninfo.userdata);
                printk(KERN_INFO "%s: run %s cb function 0x%x\n", __FUNCTION__, phead->cbfuninfo.name, (unsigned int)phead->cbfuninfo.cbfun);
                break;
            }
            phead = phead->pnext;
        }
    }

    if (BSP_RESET_OK != iresult)
    {
        if (NULL != phead)
        {
            printk(KERN_ERR "%s: fail to run cbfun of %s, at %d, return %d\n", __FUNCTION__, phead->cbfuninfo.name, eparam ,iresult);

        }
        else
        {
            printk(KERN_ERR "%s: fail to run cbfun, but phead or pname is null\n", __FUNCTION__);
        }
    }
    return iresult;
}

/*****************************************************************************
 函 数 名  :  hifireset _runcbfun
 功能描述  : HIFI复位前后调用回调函数的函数。
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam, 0 表示HIFI复位前；非零表示复位后。

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int hifireset_runcbfun (DRV_RESET_CALLCBFUN_MOMENT eparam)
{
    int  iresult = 0;

    if (DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        /*遍历回调函数链表，调用NAS的回调*/
        iresult = hifireset_doruncbfun("NAS_AT", eparam);
        if (BSP_RESET_OK != iresult)
        {
        /*如果返回失败，记录下组建name, 返回值，保存到文件*/
            goto return_error;
        }
    }
    else
    {
        /*遍历回调函数链表，调用回调函数*/
        iresult = hifireset_doruncbfun(RESET_CBFUN_IGNORE_NAME, eparam);
        if (BSP_RESET_OK != iresult)
        {
            goto return_error;
        }
    }
    printk(KERN_INFO "%s: end of run cb functions for hifi reset at %d, %d\n", __FUNCTION__, eparam, iresult);
    return BSP_RESET_OK;
return_error:
    return BSP_RESET_ERROR;
}
#endif


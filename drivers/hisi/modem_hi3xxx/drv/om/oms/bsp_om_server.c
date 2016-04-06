

/*lint -save -e767 */
#define    THIS_MODU_ID        BSP_MODU_OMS
/*lint -restore +e767 */

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
/*lint -save -e537*/
#include <linux/semaphore.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>
#include "bsp_om_api.h"
#include "bsp_om_server.h"
#include "bsp_socp.h"
#include "drv_om.h"
#include "bsp_softtimer.h"
#include "bsp_om_log.h"
#include "osl_sem.h"
#include "osl_malloc.h"
/*lint -restore*/
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
bsp_om_global_s     g_om_global_info    = {0};

/*lint -save -e512*/
spinlock_t g_st_buf_lock;
spinlock_t g_st_control_lock;
/*lint -restore +e512*/

bsp_om_socp_coder_src_cfg_s g_bsp_om_socp_chan_info=
{
    BSP_OM_SOCP_CHAN_INIT_FAIL,
    BSP_OM_ACPU_CODER_SRC_CHAN,
#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
    SOCP_CODER_DST_LOM_IND,
#else
    SOCP_CODER_DST_OM_IND,
#endif
    SOCP_DATA_TYPE_0,
    SOCP_ENCSRC_CHNMODE_LIST,
    SOCP_CHAN_PRIORITY_2 ,
    0,
    0,
    BSP_OM_CODER_SRC_BDSIZE,
    0,
    0,
    BSP_OM_CODER_SRC_RDSIZE};


bsp_om_socp_buf_ctrl_s g_bsp_om_socp_buf_info[BSP_OM_BUF_NUM] =
{
     {0,0,0,0, 0,BSP_OM_LOG_BUF_SIZE,0,0,BSP_OM_BUF_INIT_FAIL},
    {0,0,0,0, 0,BSP_OM_SOCP_BUF_SIZE,0,0,BSP_OM_BUF_INIT_FAIL}
};

bsp_om_log_buf_ctrl_s  g_bsp_log_buf_info ;

struct softtimer_list *bsp_om_timer[40];

osl_sem_id                socp_opt_sem;
osl_sem_id                om_buf_sem;
osl_sem_id                send_task_sem;

bsp_om_list_s        *g_send_list_head = NULL;
bsp_om_list_debug_s  g_list_debug = {0};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
u32 bsp_om_socp_chan_init(void);
u32 bsp_om_buf_init(void);
u32 bsp_om_socp_clean_rd_buf(u32 chan_id,SOCP_BUFFER_RW_STRU *rd_stru);
u32 bsp_om_clean_rd(void);
void bsp_om_global_init(void);
void bsp_om_get_head_from_list(void**pbuf,u32 *len);
void bsp_om_del_head_froms_list(void);
int bsp_om_send_task(void *);
void bsp_log_send_set(u32 send_type);
u32 bsp_om_free_log_buf(void* buf_addr,u32 len);
#ifdef ENABLE_BUILD_SYSVIEW  
void bsp_om_sysview_swt_reset(void);
#endif

/*****************************************************************************
* 函 数 名  : bsp_om_virt_phy
*
* 功能描述  :虚地址转换成物理地址
*
* 输入参数  : virt_addr:虚地址
*
* 输出参数  :无
*
* 返 回 值  : 物理地址
*****************************************************************************/

static INLINE u32 bsp_om_virt_phy(void* virt_addr)
{
    u32 phy_real_addr;

#ifdef __KERNEL__
    phy_real_addr = (u32)virt_to_phys(virt_addr);
#else
    phy_real_addr = (u32)virt_addr;
#endif

    return (u32)phy_real_addr;
}

/*****************************************************************************
* 函 数 名  : bsp_om_phy_virt
*
* 功能描述  :物理地址转换为虚拟地址
*
* 输入参数  : phy_addr:虚地址
*
* 输出参数  :无
*
* 返 回 值  : 虚拟地址
*****************************************************************************/

static INLINE void* bsp_om_phy_virt(u32 phy_addr)
{
    void* virt_addr;

#ifdef __KERNEL__
    virt_addr = phys_to_virt(phy_addr);
#else
    virt_addr = phy_addr;
#endif

    return virt_addr;
}


/*****************************************************************************
* 函 数 名  : bsp_om_alloc
*
* 功能描述  :申请内存
*
* 输入参数  : size 申请buf的长度
*                        phy_real_addr :返回的对应的物理地址
*
* 输出参数  :
*
* 返 回 值  :虚拟内存地址
*****************************************************************************/

void* bsp_om_alloc(u32 size, u32*phy_real_addr)
{
    void    *virt_addr = NULL;

#ifdef __VXWORKS__
    if(OM_MALLOC_MAX_SIZE < size )
    {
        return NULL;
    }

    virt_addr = cacheDmaMalloc(size);

    *phy_real_addr = (u32)virt_addr;
#endif

#ifdef __KERNEL__
    u32 i = 0;
    u32 index =0;
    if(OM_MALLOC_MAX_SIZE < size )
    {
        return NULL;
    }

    for(i=0;i<=OM_MALLOC_MAX_INDEX;i++)
    {
        if(size <= (u32)(MEMORY_PAGE_SIZE * ((u32)1<<i)))
        {
            index = i;
            break;
        }
    }

    virt_addr = (u8*)__get_free_pages(GFP_KERNEL,index);

    if( virt_addr == NULL)
    {
        return NULL;
    }

    *(phy_real_addr) = (u32)virt_to_phys(virt_addr);
#endif

    return virt_addr;
}



int bsp_om_start_timer(u32 timer_len,void *func,u32 para,struct softtimer_list *timer)
{
    s32 ret = BSP_OK;

    timer->func = func;
    timer->para = para;
    timer->timeout = timer_len*1000;
    timer->wake_type = SOFTTIMER_NOWAKE;

    if(timer->init_flags != TIMER_INIT_FLAG)
    {
        ret = bsp_softtimer_create(timer);

        bsp_softtimer_add(timer);

    }
    else
    {
        ret = bsp_softtimer_modify(timer,timer_len*1000);

        bsp_softtimer_add(timer);
    }
    return ret;
}


int bsp_om_stop_timer(struct softtimer_list *timer)
{
    int ret = BSP_OK;
    if( timer->init_flags == TIMER_INIT_FLAG)
    {
        ret =  bsp_softtimer_delete_sync(timer);

        if(ret <0)
        {
            bsp_om_debug(BSP_LOG_LEVEL_ERROR, "bsp om stop timer fail \n");
			return ret;
        }
    }

    return BSP_OK;
}

int bsp_om_buf_sem_take(void)
{
    if(g_bsp_om_socp_chan_info.init_state== BSP_OM_SOCP_CHAN_INIT_SUCC)
    {
        osl_sem_down(&om_buf_sem);
    }
    else
    {
        return -1;
    }

    return BSP_OK;

}

int bsp_om_buf_sem_give(void)
{
    if(g_bsp_om_socp_chan_info.init_state== BSP_OM_SOCP_CHAN_INIT_SUCC)
    {
        osl_sem_up(&om_buf_sem);
    }
    else
    {
        return -1;
    }

    return BSP_OK;
}


/*****************************************************************************
* 函 数 名  : bsp_om_socp_chan_init
*
* 功能描述  : 底软SOCP通道初始化处理
*
* 输入参数  : 无
*
*
* 输出参数  :无
*
* 返 回 值  :初始化结果
*****************************************************************************/

u32 bsp_om_socp_chan_init(void)
{
    SOCP_CODER_SRC_CHAN_STRU               channle_stu = {0};

    /*编码源通道buf初始化*/
    /* 申请BD空间 */
    g_bsp_om_socp_chan_info.bd_buf = bsp_om_alloc(BSP_OM_CODER_SRC_BDSIZE,&(g_bsp_om_socp_chan_info.bd_buf_phy));

    if(( 0== g_bsp_om_socp_chan_info.bd_buf)||(0 == g_bsp_om_socp_chan_info.bd_buf_phy ))
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR," alloc BD fail\n");
        return BSP_ERR_OMS_MALLOC_FAIL;
    }

    /* 申请RD空间 */
    g_bsp_om_socp_chan_info.rd_buf = bsp_om_alloc(BSP_OM_CODER_SRC_RDSIZE,&(g_bsp_om_socp_chan_info.rd_buf_phy));

    if(( 0== g_bsp_om_socp_chan_info.rd_buf)||(0 == g_bsp_om_socp_chan_info.rd_buf_phy ))
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR," alloc RD fail\n");
        return BSP_ERR_OMS_MALLOC_FAIL;
    }


    channle_stu.u32DestChanID = (u32)g_bsp_om_socp_chan_info.en_dst_chan_id;    /*  目标通道ID */
    channle_stu.eDataType     = g_bsp_om_socp_chan_info.en_data_type;               /*  数据类型，指明数据封装协议，用于复用多平台 */
    channle_stu.eMode         = g_bsp_om_socp_chan_info.en_chan_mode;               /*  通道数据模式 */
    channle_stu.ePriority     = g_bsp_om_socp_chan_info.en_chan_level;              /*  通道优先级 */
    channle_stu.u32BypassEn   = SOCP_HDLC_ENABLE;             /*  通道bypass使能 */
    channle_stu.eDataTypeEn   = SOCP_DATA_TYPE_EN;            /*  数据类型使能位 */
    channle_stu.eDebugEn      = SOCP_ENC_DEBUG_DIS;           /*  调试位使能 */

    channle_stu.sCoderSetSrcBuf.u32InputStart  = (u32)g_bsp_om_socp_chan_info.bd_buf_phy;           /*  输入通道起始地址 */
    channle_stu.sCoderSetSrcBuf.u32InputEnd    = (u32)((g_bsp_om_socp_chan_info.bd_buf_phy
                                                                + g_bsp_om_socp_chan_info.bd_buf_len)-1);   /*  输入通道结束地址 */
    channle_stu.sCoderSetSrcBuf.u32RDStart     = (u32)(g_bsp_om_socp_chan_info.rd_buf_phy);                              /* RD buffer起始地址 */
    channle_stu.sCoderSetSrcBuf.u32RDEnd       = (u32)((g_bsp_om_socp_chan_info.rd_buf_phy
                                                                + g_bsp_om_socp_chan_info.rd_buf_len)-1);    /*  RD buffer结束地址 */
    channle_stu.sCoderSetSrcBuf.u32RDThreshold = 0;                                                                  /* RD buffer数据上报阈值 */

    if (BSP_OK != bsp_socp_coder_set_src_chan(g_bsp_om_socp_chan_info.en_src_chan_id, &channle_stu))
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR,"DRV_SOCP_CORDER_SET_SRC_CHAN  fail\n");
        return BSP_ERR_OMS_SOCP_INIT_ERR;
    }

     /*初始化SOCP通道操作的信号量 */
    osl_sem_init(SEM_FULL,&socp_opt_sem);

    /*通道使能操作在目的通道初始化完成之后进行*/

    g_bsp_om_socp_chan_info.init_state= BSP_OM_SOCP_CHAN_INIT_SUCC;

    return BSP_OK;

}

/*****************************************************************************
* 函 数 名  : bsp_socp_chan_enable
*
* 功能描述  : 底软SOCP通道使能操作
*
* 输入参数  : 无
*
*
* 输出参数  :无
*
* 返 回 值  :初始化结果
*****************************************************************************/

void bsp_socp_chan_enable(void)
{
    if(g_bsp_om_socp_chan_info.init_state == BSP_OM_SOCP_CHAN_INIT_SUCC)
    {
        bsp_socp_start(BSP_OM_ACPU_CODER_SRC_CHAN);
    }
}

/*****************************************************************************
* 函 数 名  : bsp_om_send_coder_src
*
* 功能描述  :发送数据到SOCP的编码源通道
*
* 输入参数  : send_data_virt :需要发送的虚拟地址
*                         send_len:         需要发送的长度
*
* 输出参数  :无
*
* 返 回 值  :写结果
*****************************************************************************/

u32 bsp_om_send_coder_src(u8 *send_data_virt, u32 send_len)
{
    SOCP_BUFFER_RW_STRU                 bd_buf = {0};
    u32                          ulBDNum;
    SOCP_BD_DATA_STRU          bd_data = {0};
    u32                          send_data_phy = 0;
    u32                         ret;

    osl_sem_down(&socp_opt_sem);

    /*  将用户虚拟地址转换成物理地址*/
    send_data_phy = bsp_om_virt_phy((void *)send_data_virt);

    if(send_data_phy == 0)
    {
        ret =  BSP_ERR_OMS_INVALID_PARAM;
        goto fail;
    }

    bsp_socp_get_write_buff(g_bsp_om_socp_chan_info.en_src_chan_id, &bd_buf);

    /* 计算空闲BD的值 */
    ulBDNum = (bd_buf.u32Size + bd_buf.u32RbSize)/ sizeof(SOCP_BD_DATA_STRU);

    /* 判断是否还有空间 */
    if (1 >= ulBDNum)
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR," GET BD fail,ulBDNum = %d\n",ulBDNum);
        ret =  BSP_ERR_OMS_SOCP_GET_BD_NUM_ERR;
        goto fail;
    }

    /*SOCP给出的是物理地址*/
    if(NULL == bd_buf.pBuffer )
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR," GET BD fail,pBuffer = NULL \n");
        ret =  BSP_ERR_OMS_SOCP_BD_ADDR_ERR;
        goto fail;
    }

    bd_data.pucData    = send_data_phy;
    bd_data.usMsgLen   = (BSP_U16)send_len;
    bd_data.enDataType = SOCP_BD_DATA;

    memcpy((void *)bsp_om_phy_virt(((uintptr_t)bd_buf.pBuffer)),(void*)&bd_data,sizeof(SOCP_BD_DATA_STRU));
    /*lint -save -e713*/
    dma_map_single(NULL, (void*)bsp_om_phy_virt(((uintptr_t)bd_buf.pBuffer)), sizeof(SOCP_BD_DATA_STRU), DMA_TO_DEVICE);
    dma_map_single(NULL, (void *)(send_data_virt), send_len, DMA_TO_DEVICE);
    /*lint -restore*/
    ret =  (u32)bsp_socp_write_done(g_bsp_om_socp_chan_info.en_src_chan_id, sizeof(SOCP_BD_DATA_STRU)) ;  /* 当前数据写入完毕 */

    if(ret != BSP_OK)
    {
        goto fail;
    }
    else
    {
        ret = BSP_OK;
        goto successful;
    }

fail:
    osl_sem_up(&socp_opt_sem);
    return ret;
successful:
    osl_sem_up(&socp_opt_sem);
    return ret;
}


/*****************************************************************************
* 函 数 名  : bsp_om_buf_init
*
* 功能描述  :初始化om缓存buf
*
* 输入参数  :无
*
*
* 输出参数  :无
*
* 返 回 值  : 无
*****************************************************************************/

u32 bsp_om_buf_init(void)
{
    u32 i;

    /* LOG BUG 不在此初始化*/
    i = BSP_OM_SOCP_BUF_TYPE;

    g_bsp_om_socp_buf_info[i].start_ptr = bsp_om_alloc(g_bsp_om_socp_buf_info[i].buf_len,(u32*)(&(g_bsp_om_socp_buf_info[i].start_phy_ptr)));

    if(g_bsp_om_socp_buf_info[i].start_ptr == 0)
    {
        return BSP_ERR_OMS_MALLOC_FAIL;
    }
    //g_bsp_om_socp_buf_info.buf_len = BSP_OM_BUF_SIZE;
    g_bsp_om_socp_buf_info[i].read_ptr = g_bsp_om_socp_buf_info[i].start_ptr;    /*  */
    g_bsp_om_socp_buf_info[i].write_ptr = g_bsp_om_socp_buf_info[i].start_ptr;
    g_bsp_om_socp_buf_info[i].send_ptr = g_bsp_om_socp_buf_info[i].start_ptr;
    g_bsp_om_socp_buf_info[i].init_state = BSP_OM_BUF_INIT_SUCC;

    memset((void *)(g_bsp_om_socp_buf_info[i].start_ptr),0,g_bsp_om_socp_buf_info[i].buf_len);

    osl_sem_init(SEM_FULL,&om_buf_sem);

    /* LOG BUG 单独初始化*/
    g_bsp_log_buf_info.start_ptr = bsp_om_alloc(BSP_OM_LOG_BUF_SIZE,(u32*)(&(g_bsp_log_buf_info.start_phy_ptr)));

    if(g_bsp_log_buf_info.start_ptr == 0)
    {
        return BSP_ERR_OMS_MALLOC_FAIL;
    }

    for(i= 0;i<(BSP_OM_LOG_BUF_SIZE/BSP_PRINT_BUF_LEN) ;i++)
    {
        g_bsp_log_buf_info.stnode[i].addr = g_bsp_log_buf_info.start_ptr + ( i* BSP_PRINT_BUF_LEN);
        g_bsp_log_buf_info.stnode[i].is_edle = TRUE;
        g_bsp_log_buf_info.stnode[i].is_valible = FALSE;
    }

    g_bsp_log_buf_info.buys_node_num = 0;
    g_bsp_log_buf_info.valible_node_num = 0;
    g_bsp_log_buf_info.node_num = (BSP_OM_LOG_BUF_SIZE/BSP_PRINT_BUF_LEN);
    g_bsp_log_buf_info.init_state = BSP_OM_BUF_INIT_SUCC;

    g_bsp_om_socp_buf_info[BSP_OM_LOG_BUF_TYPE].start_ptr =g_bsp_log_buf_info.start_ptr;
    g_bsp_om_socp_buf_info[BSP_OM_LOG_BUF_TYPE].start_phy_ptr = g_bsp_log_buf_info.start_phy_ptr;
    g_bsp_om_socp_buf_info[BSP_OM_LOG_BUF_TYPE].buf_len = BSP_OM_LOG_BUF_SIZE;

    return BSP_OK;


}

/*****************************************************************************
* 函 数 名  : bsp_om_socp_clean_rd_buf
*
* 功能描述  :清理SOCP的RD缓存
*
* 输入参数  :chan_id :socp 的通道ID
*                       rd_stru: 入参
*
* 输出参数  :无
*
* 返 回 值  : BSP_OK 成功; 其他 失败
*****************************************************************************/

u32 bsp_om_socp_clean_rd_buf(u32 chan_id,SOCP_BUFFER_RW_STRU *rd_stru)
{
    u32             ret;
    SOCP_BUFFER_RW_STRU    rd_buf_stru = {0};

    osl_sem_down(&socp_opt_sem);

    ret = (u32)bsp_socp_get_rd_buffer(chan_id,&rd_buf_stru);

    if(BSP_OK != ret)
    {
        ret  = BSP_ERR_OMS_SOCP_GET_RD_ERR;

        goto fail;
    }

    /* 无RD需要释放*/
    if( (0 ==rd_buf_stru.u32Size)&&(0 == rd_buf_stru.u32RbSize))
    {
         ret  = BSP_OK;
         goto successful;
    }

    /* 获取到的RD数据异常*/
    if((rd_buf_stru.u32Size + rd_buf_stru.u32RbSize) > BSP_OM_CODER_SRC_RDSIZE)
    {
        ret  = BSP_ERR_OMS_SOCP_CLEAN_RD_ERR;
        goto fail;
    }

    if(0 != (rd_buf_stru.u32Size / sizeof(SOCP_RD_DATA_STRU)))
    {
        /*lint -save -e713*/
        dma_map_single(NULL, (void *)bsp_om_phy_virt(((uintptr_t)rd_buf_stru.pBuffer)), rd_buf_stru.u32Size, DMA_FROM_DEVICE);
        /*lint -restore*/
        rd_stru->pBuffer = (char *)bsp_om_phy_virt(((uintptr_t)rd_buf_stru.pBuffer));
        rd_stru->u32Size = rd_buf_stru.u32Size;
    }
    else
    {
        ret  = BSP_ERR_OMS_SOCP_CLEAN_RD_ERR;

        goto fail;
    }

    ret = (u32)bsp_socp_read_rd_done(chan_id, rd_buf_stru.u32Size);  /* 释放rd缓存 */
    if( BSP_OK !=ret)
    {
        goto fail;
    }
    else
    {
        ret = BSP_OK;
        goto successful;
    }

fail:
    osl_sem_up(&socp_opt_sem);
    return ret;
successful:
    osl_sem_up(&socp_opt_sem);
    return ret;
}


/*****************************************************************************
* 函 数 名  : bsp_om_clean_rd
*
* 功能描述  :清理RD缓存和OM buf缓存
*
* 输入参数  :无
*
*
* 输出参数  :无
*
* 返 回 值  : BSP_OK 成功; 其他 失败
*****************************************************************************/

u32 bsp_om_clean_rd(void)
{
    u32 i;
    u32 ret = BSP_OK;
    SOCP_BUFFER_RW_STRU rd_buf = {0};
    SOCP_RD_DATA_STRU rd_data = {0};

    ret = bsp_om_socp_clean_rd_buf(g_bsp_om_socp_chan_info.en_src_chan_id,&rd_buf);

    if((NULL != rd_buf.pBuffer)&&(ret == BSP_OK))
    {
        bsp_om_buf_sem_take();
        for(i = 0;i < (rd_buf.u32Size /sizeof(SOCP_RD_DATA_STRU));i++)
        {
            memcpy(&rd_data,rd_buf.pBuffer,sizeof(SOCP_RD_DATA_STRU));

            rd_data.pucData =(uintptr_t)bsp_om_phy_virt(rd_data.pucData);

            ret =  bsp_om_free_buf((void*)(uintptr_t)(rd_data.pucData),(u32)(rd_data.usMsgLen)&0xffff);

            if(ret != BSP_OK)
            {
                bsp_om_debug(BSP_LOG_LEVEL_ERROR,"free om buf error, stop socp coder chan\n");
                bsp_socp_stop(g_bsp_om_socp_chan_info.en_src_chan_id);
                bsp_om_buf_sem_give();
                return ret;
            }

            rd_buf.pBuffer = rd_buf.pBuffer +sizeof(SOCP_RD_DATA_STRU);
        }
        bsp_om_buf_sem_give();
    }
    else
    {
        return ret;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_om_get_log_buf
*
* 功能描述  :申请缓存buf,bsp_trace 专用
*
* 输入参数  :buf_len :申请的长度
*
*
* 输出参数  :无
*
* 返 回 值  : NULL 申请失败；其他 内存地址
*****************************************************************************/

void* bsp_om_get_log_buf(u32 get_buf_len)
{
    u32 i;
    unsigned long int_lock_lvl = 0;

    if(get_buf_len > BSP_PRINT_BUF_LEN )
    {
        return  NULL;
    }

    spin_lock_irqsave(&g_st_control_lock, int_lock_lvl);
    for(i= 0;i<(BSP_OM_LOG_BUF_SIZE/BSP_PRINT_BUF_LEN) ;i++)
    {
        if(g_bsp_log_buf_info.stnode[i].is_edle == (u16)TRUE)
        {
            g_bsp_log_buf_info.stnode[i].is_edle = (u16)FALSE;
            g_bsp_log_buf_info.stnode[i].is_valible= (u16)TRUE;
            g_bsp_log_buf_info.buys_node_num++;
            g_bsp_log_buf_info.valible_node_num++;
            spin_unlock_irqrestore(&g_st_control_lock, int_lock_lvl);
            return g_bsp_log_buf_info.stnode[i].addr;
        }
    }
    spin_unlock_irqrestore(&g_st_control_lock, int_lock_lvl);

     return NULL;
}

/*****************************************************************************
* 函 数 名  : bsp_om_free_log_buf
*
* 功能描述  :释放buf,bsp_trace 专用
*
* 输入参数  :buf_len :申请的长度
*
*
* 输出参数  :无
*
* 返 回 值  : NULL 申请失败；其他 内存地址
*****************************************************************************/

u32 bsp_om_free_log_buf(void* buf_addr,u32 len)
{
    u32 i;
    unsigned long int_lock_lvl = 0;

    if(len > BSP_PRINT_BUF_LEN )
    {
        return  BSP_OK;
    }

    for(i= 0;i<(BSP_OM_LOG_BUF_SIZE/BSP_PRINT_BUF_LEN) ;i++)
    {
        if(g_bsp_log_buf_info.stnode[i].addr == buf_addr)
        {
            spin_lock_irqsave(&g_st_control_lock, int_lock_lvl);    
            g_bsp_log_buf_info.stnode[i].is_edle = (u16)TRUE;
            g_bsp_log_buf_info.stnode[i].is_valible= (u16)FALSE;
            g_bsp_log_buf_info.buys_node_num--;
            spin_unlock_irqrestore(&g_st_control_lock, int_lock_lvl);
            return BSP_OK;
        }
    }

    return (u32)BSP_ERROR;
}


/*****************************************************************************
* 函 数 名  : bsp_om_get_buf
*
* 功能描述  :申请缓存buf
*
* 输入参数  :buf_len :申请的长度
*
*
* 输出参数  :无
*
* 返 回 值  : NULL 申请失败；其他 内存地址
*****************************************************************************/

void* bsp_om_get_buf(u32 buf_type,u32 get_buf_len)
{
    u32 free_size = 0;
    void* return_addr = NULL;
    void* read_ptr = NULL;
    void* write_ptr = NULL;
    u32 buf_len = 0;
    void* start_ptr = NULL;

    read_ptr = g_bsp_om_socp_buf_info[buf_type].read_ptr;
    write_ptr = g_bsp_om_socp_buf_info[buf_type].write_ptr;
    buf_len = g_bsp_om_socp_buf_info[buf_type].buf_len;
    start_ptr = g_bsp_om_socp_buf_info[buf_type].start_ptr;

    /* 申请编码源buf*/
    if(read_ptr <= write_ptr)
    {
        free_size = (u32)(buf_len - (((char*)write_ptr - (char*)start_ptr) & 0xFFFFFFFF));

        if(free_size > get_buf_len )
        {
            return_addr = write_ptr;
            g_bsp_om_socp_buf_info[buf_type].write_ptr = (void*)((char*)write_ptr + get_buf_len);
        }
        /*翻转写指针*/
        else
        {
            g_bsp_om_socp_buf_info[buf_type].last_pading_len = free_size;

            free_size = (u32)(((char*)read_ptr - (char*)start_ptr)& 0xFFFFFFFF );

            if(free_size > get_buf_len)
            {
                g_bsp_om_socp_buf_info[buf_type].write_ptr = start_ptr;

                return_addr = start_ptr;

                g_bsp_om_socp_buf_info[buf_type].write_ptr = ((char*)g_bsp_om_socp_buf_info[buf_type].write_ptr + get_buf_len);
            }
            else
            {
                g_bsp_om_socp_buf_info[buf_type].last_pading_len = 0;
                return_addr = NULL;
            }
        }
    }
    /*读写指针翻转*/
    else
    {
        free_size = (u32)(((char*)read_ptr - (char*)write_ptr)&0xFFFFFFFF );

        if(free_size > get_buf_len)
        {
            return_addr = write_ptr;

            g_bsp_om_socp_buf_info[buf_type].write_ptr = (char*)write_ptr + get_buf_len;
        }
        else
        {
            return_addr = NULL;
        }
    }

    if(return_addr != NULL)
    {
        g_bsp_om_socp_buf_info[buf_type].buf_size  += get_buf_len;
    }

    return return_addr;
}

/*****************************************************************************
* 函 数 名  : bsp_om_free_buf
*
* 功能描述  :释放缓存buf
*
* 输入参数  :buf_addr :释放缓存地址
*                       len           : 需要释放的地址
*
* 输出参数  :无
*
* 返 回 值  : BSP_OK 释放成功;其他 释放失败
*****************************************************************************/

u32 bsp_om_free_buf(void* buf_addr,u32 len)
{
    u32 buf_type = 0;
    void* read_ptr = NULL;
    void* write_ptr  = NULL;
    u32 buf_len = 0;
    void* start_ptr  = NULL;
    u32 last_pading_len = 0;
    void* end_ptr  = NULL;;

    for(buf_type = 0;buf_type < BSP_OM_BUF_NUM;buf_type++)
    {
        if((buf_addr>= g_bsp_om_socp_buf_info[buf_type].start_ptr)
            &&( (char*)buf_addr < ((char*)g_bsp_om_socp_buf_info[buf_type].start_ptr + g_bsp_om_socp_buf_info[buf_type].buf_len)))
        {
            break;
        }
    }

    if(buf_type == BSP_OM_BUF_NUM)
    {
        return BSP_ERR_OMS_INVALID_PARAM;
    }

    /*LOG BUF 单独处理*/
    if(buf_type == BSP_OM_LOG_BUF_TYPE)
    {
        return bsp_om_free_log_buf(buf_addr,len);
    }

    read_ptr = g_bsp_om_socp_buf_info[buf_type].read_ptr;
    write_ptr = g_bsp_om_socp_buf_info[buf_type].write_ptr;
    buf_len = g_bsp_om_socp_buf_info[buf_type].buf_len;
    start_ptr = g_bsp_om_socp_buf_info[buf_type].start_ptr;
    last_pading_len = g_bsp_om_socp_buf_info[buf_type].last_pading_len;

    end_ptr = ((char*)start_ptr + buf_len);

    /*顺序申请和释放的情况*/
    if((buf_addr == read_ptr )||( buf_addr == start_ptr))
    {
        if((read_ptr + last_pading_len + len) < end_ptr)
        {
            g_bsp_om_socp_buf_info[buf_type].read_ptr = (char*)read_ptr + len;
        }
        else if(((char*)read_ptr + last_pading_len + len) >(char*)end_ptr)
        {
            g_bsp_om_socp_buf_info[buf_type].read_ptr = (char*)start_ptr + len;
            g_bsp_om_socp_buf_info[buf_type].last_pading_len = 0;
        }
        else
        {
            g_bsp_om_socp_buf_info[buf_type].read_ptr = start_ptr ;
            g_bsp_om_socp_buf_info[buf_type].last_pading_len = 0;
        }

    }
      /*申请之后组包发送错误，直接释放的情况*/
    else if(buf_addr == (write_ptr- len))
    {
        g_bsp_om_socp_buf_info[buf_type].write_ptr  = buf_addr;
        g_bsp_om_socp_buf_info[buf_type].buf_size  -= len;
    }
    else if(( buf_addr + last_pading_len + len) == end_ptr)
    {
        g_bsp_om_socp_buf_info[buf_type].write_ptr  = buf_addr;
        g_bsp_om_socp_buf_info[buf_type].buf_size  -= len;
        g_bsp_om_socp_buf_info[buf_type].last_pading_len = 0;
    }
    /*异常buf*/
    else
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR, " bsp om invalid buf  %p, read_ptr = %p,write_ptr = %p",buf_addr,read_ptr,write_ptr);
    }

    return BSP_OK;
}


u32 bsp_om_send_log_buf(u32 buf_type,u32 packet_len)
{
    u32 return_value = 0;
    u32 send_size = 0;
    u32 i;
    unsigned long int_lock_lvl = 0;

    if(g_bsp_log_buf_info.valible_node_num > 0)
    {
        send_size = packet_len;

        for(i = 0;i<(BSP_OM_LOG_BUF_SIZE/BSP_PRINT_BUF_LEN);i++)
        {
            if(g_bsp_log_buf_info.stnode[i].is_valible == (u16)TRUE)
            {
                return_value = bsp_om_send_coder_src((u8 *)(g_bsp_log_buf_info.stnode[i].addr),send_size);

                if(return_value == BSP_OK)
                {
                    spin_lock_irqsave(&g_st_control_lock, int_lock_lvl);
                    g_bsp_log_buf_info.valible_node_num--;
                    if(g_bsp_log_buf_info.valible_node_num == 0)
                    {
                        spin_unlock_irqrestore(&g_st_control_lock, int_lock_lvl);
                        return BSP_OK;
                    }
                    spin_unlock_irqrestore(&g_st_control_lock, int_lock_lvl);
                }
                else
                {   
                    bsp_om_free_log_buf(g_bsp_log_buf_info.stnode[i].addr, send_size);
                    return return_value;
                }
            }
        }
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_om_global_init
*
* 功能描述  : 初始化OM模块全局信息
*
* 输入参数  :无
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/

void bsp_om_global_init(void)
{
    g_om_global_info.hso_connect_flag = FALSE;
    /* 需要增加一个NV项控制*/
    g_om_global_info.om_cfg.nv_cfg.log_switch = BSP_LOG_SEND_TO_HSO;

    return;
}


void bsp_log_send_set(u32 send_type)
{
    g_om_global_info.om_cfg.nv_cfg.log_switch = send_type;
}

int bsp_om_into_send_list(void* buf_addr,u32 len)
{
    bsp_om_list_s  *ptemp = NULL;
    unsigned long int_lock_lvl = 0;

    ptemp = osl_malloc(sizeof(bsp_om_list_s));

    if(ptemp == NULL)
    {
        return -1;
    }

    ptemp->buf_addr = buf_addr;
    ptemp->buf_len = len;
    ptemp->pnext = NULL;

    spin_lock_irqsave(&g_st_buf_lock, int_lock_lvl);
    if(g_send_list_head != NULL)
    {
        g_send_list_head->ptail->pnext  = ptemp;

        g_send_list_head->ptail =g_send_list_head->ptail->pnext;
    }
    else
    {
        g_send_list_head = ptemp;
        g_send_list_head->ptail = g_send_list_head;

    }

    g_list_debug.list_in++;

    spin_unlock_irqrestore(&g_st_buf_lock, int_lock_lvl);

    osl_sem_up(&send_task_sem);

    return BSP_OK;
}

void bsp_om_get_head_from_list(void** pbuf,u32 *len)
{
    unsigned long int_lock_lvl = 0;

    spin_lock_irqsave(&g_st_buf_lock, int_lock_lvl);
    if(g_send_list_head != NULL)
    {
        *pbuf = g_send_list_head->buf_addr;
        *len   = g_send_list_head->buf_len;
        g_list_debug.list_get++;
    }
    else
    {
        *pbuf = 0;
        *len  = 0;
    }

    spin_unlock_irqrestore(&g_st_buf_lock, int_lock_lvl);
}

void bsp_om_del_head_froms_list(void)
{
    bsp_om_list_s  *ptemp = NULL;
    unsigned long int_lock_lvl = 0;

    spin_lock_irqsave(&g_st_buf_lock, int_lock_lvl);
    if(g_send_list_head == NULL)
    {
        spin_unlock_irqrestore(&g_st_buf_lock, int_lock_lvl);
        return ;
    }

    ptemp = g_send_list_head;

    g_send_list_head = g_send_list_head->pnext;

    if(g_send_list_head != NULL)
    {
        g_send_list_head->ptail = ptemp->ptail;
    }

    g_list_debug.list_del++;

    spin_unlock_irqrestore(&g_st_buf_lock, int_lock_lvl);

    osl_free(ptemp);
    return ;

}

int bsp_om_send_task(void * para)
{
    void* send_addr = 0;
    u32 buf_len = 0;
    u32 ret;

    osl_sem_init(SEM_EMPTY,&send_task_sem);
    /* coverity[no_escape] */
    for(;;)
    {
        /* 清理RD*/
        bsp_om_clean_rd();
        /* coverity[check_return] */
        osl_sem_down(&send_task_sem);

        bsp_om_send_log_buf(BSP_OM_LOG_BUF_TYPE,BSP_PRINT_BUF_LEN);

        do
        {
            bsp_om_get_head_from_list(&send_addr,&buf_len);

            if((send_addr != 0)&&(buf_len != 0))
            {
                ret =  bsp_om_send_coder_src(send_addr,buf_len);

                if(ret == BSP_OK)
                {
                    bsp_om_del_head_froms_list();
                }
                else
                {
                    break;
                }
            }

        }while((send_addr != 0)&&(buf_len != 0));
    }
    /*lint -save -e527 */
    return 0;
    /*lint -restore +e527 */
}

/*****************************************************************************
* 函 数 名  : bsp_om_server_init
*
* 功能描述  : om初始化总入口
*
* 输入参数  :无
*
*
* 输出参数  :无
*
* 返 回 值  :BSP_OK 初始化成功;其他 初始化失败
*****************************************************************************/
s32 bsp_om_server_init(void)
{
    u32 ret ;

    spin_lock_init(&g_st_buf_lock);
    spin_lock_init(&g_st_control_lock);

     /* 初始化 BSP OM SOCP 源缓存buf*/
    ret =  bsp_om_buf_init();

    if(BSP_OK != ret)
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR, "&&&&&&&&bsp_om_buf_init  error!!!ret = 0x%x\n",ret);
        return (s32)ret;
    }

    /* 初始化 BSP使用的SOCP源通道*/
    ret = bsp_om_socp_chan_init();
    if(BSP_OK !=ret )
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR, "bsp_om_server_init  error!!!ret = 0x%x\n",ret);
        return  (s32)ret;
    }

    /*初始化om模块全局信息*/
    bsp_om_global_init();
    /* 初始化log打印级别为default值*/
    bsp_log_level_reset();
    
#ifdef ENABLE_BUILD_SYSVIEW
    /* 初始化sysview全局数据*/
    ret = sys_view_init();
    if(BSP_OK != ret)
    {
        bsp_om_debug(BSP_LOG_LEVEL_ERROR, "&&&&&&&&&sys_view_init  error!!!ret = 0x%x\n",ret);
        return (s32)ret;
    }
#endif

    kthread_run(bsp_om_send_task, NULL, "tAcpuOmTask");

    bsp_om_debug(BSP_LOG_LEVEL_ERROR, "bsp om init ok\n");

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_om_set_hso_conn_flag
*
* 功能描述  :该接口需要MSP的诊断模块在HSO连接或者去连接的时候调用
*
* 输入参数  : flag :连接标志，1表示连接，0表示断开
*
*
* 输出参数  :无
*
* 返 回 值  : 无
*****************************************************************************/

void bsp_om_set_hso_conn_flag(u32 flag)
{
    g_om_global_info.hso_connect_flag = flag;
    bsp_dump_set_hso_conn_flag(flag);
#ifdef ENABLE_BUILD_SYSVIEW   
    bsp_om_sysview_swt_reset();
#endif
}

u32  bsp_om_get_hso_conn_flag(void)
{
    return g_om_global_info.hso_connect_flag;
}

/*lint -save -e19 */
module_init(bsp_om_server_init);
/*lint -restore +e19 */


void show_list_debug(void)
{
    bsp_om_debug(BSP_LOG_LEVEL_ERROR, " list_in =  0x%x\n",g_list_debug.list_in);
    bsp_om_debug(BSP_LOG_LEVEL_ERROR, " list_get =  0x%x\n",g_list_debug.list_get);
    bsp_om_debug(BSP_LOG_LEVEL_ERROR, " list_del =  0x%x\n",g_list_debug.list_del);
}


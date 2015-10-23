
#include "bsp_busstress.h"
#include "bsp_softtimer.h"
#include "bsp_edma.h"
#include "osl_sem.h"
#include "bsp_sram.h"
#include "osl_bio.h"

#define EDMA_HANDLE_NUM (4)
#define EDMA_TEST_LEN ( (AXI_MEM_64_SIZE_FOR_EDMAC)/8)

#ifndef MEM_FOR_EDMAC_BUSSTRESS /* 需要在 bsp_busstress.h中定义 */
#define MEM_FOR_EDMAC_BUSSTRESS SRAM_DRXM_ADDR
#endif

#define EDMA_HANDLE_1_AXI_MEM_64_DST (MEM_FOR_EDMAC_BUSSTRESS)
#define EDMA_HANDLE_2_AXI_MEM_64_SRC (EDMA_HANDLE_1_AXI_MEM_64_DST + EDMA_TEST_LEN)
#define EDMA_HANDLE_2_AXI_MEM_64_DST (EDMA_HANDLE_2_AXI_MEM_64_SRC + EDMA_TEST_LEN)
#define EDMA_HANDLE_3_AXI_MEM_64_SRC (EDMA_HANDLE_2_AXI_MEM_64_DST + EDMA_TEST_LEN)


#define MEM_FOR_EDMAC2_BUSSTRESS (MEM_FOR_EDMAC_BUSSTRESS+4*EDMA_TEST_LEN)

#define EDMA2_HANDLE_1_AXI_MEM_64_DST (MEM_FOR_EDMAC2_BUSSTRESS)
#define EDMA2_HANDLE_2_AXI_MEM_64_SRC (EDMA2_HANDLE_1_AXI_MEM_64_DST + EDMA_TEST_LEN)
#define EDMA2_HANDLE_2_AXI_MEM_64_DST (EDMA2_HANDLE_2_AXI_MEM_64_SRC + EDMA_TEST_LEN)
#define EDMA2_HANDLE_3_AXI_MEM_64_SRC (EDMA2_HANDLE_2_AXI_MEM_64_DST + EDMA_TEST_LEN)



struct edma_busstress_info
{
    struct softtimer_list edma_softtimer_list;
    s32 edma_stress_test_task_id ;
    struct semaphore * edma_send_sem ;
    AXI_DMA_TASK_HANDLE_S  stDMATaskHandle[4];
    u8 EDMA_HANDLE_0_DDR_SRC[EDMA_TEST_LEN];
    u8 EDMA_HANDLE_0_DDR_DST[EDMA_TEST_LEN];
    u8 EDMA_HANDLE_1_DDR_SRC[EDMA_TEST_LEN];
    u8 EDMA_HANDLE_3_DDR_DST[EDMA_TEST_LEN];
};
struct edma_busstress_info g_edma_busstress_info =
{

{0},    /* struct softtimer_list * */
-1,   /* edma_stress_test_task_id */
0,
{    /* AXI_DMA_TASK_HANDLE_S  stDMATaskHandle[EDMA_HANDLE_NUM]  注意个数 是宏定义的  */

    {0, 0,  EDMA_TEST_LEN,  EDMA_CHANNEL_INVALID},
    {0, EDMA_HANDLE_1_AXI_MEM_64_DST,   EDMA_TEST_LEN,  EDMA_CHANNEL_INVALID},
    {EDMA_HANDLE_2_AXI_MEM_64_SRC,  EDMA_HANDLE_2_AXI_MEM_64_DST,   EDMA_TEST_LEN,  EDMA_CHANNEL_INVALID},
    {EDMA_HANDLE_3_AXI_MEM_64_SRC,  0,    EDMA_TEST_LEN,  EDMA_CHANNEL_INVALID}

},    /* SEM_ID edma_send_sem */
{0},
{0},
{0},
{0}
};


extern s32 EDMA_QueryCHNLState(u32 u32ChIdx);
extern s32 EDMA_NormTaskLaunch(AXI_DMA_TASK_HANDLE_S * pstDMAHandle);
extern s32 EDMA_2VecTaskLaunch(AXI_DMA_TASK_HANDLE_S * pstDMAHandle);
extern s32 EDMA_QueryCHNLState(u32 u32ChIdx);
extern s32 EDMA_QueryCHNsLState(u32 u32ChIdxMap);
extern int get_test_switch_stat(int module_id);

u32 g_edma_bustest_count = 0;
s32 edma_stress_test_routine()
{
    u32 count = 0;
    s32 s32ret = 0;
    u32 cur_idx = 0;

    while(get_test_switch_stat(EDMAC))
    {

        /*down(g_edma_busstress_info.edma_send_sem);*/
        semTake(g_edma_busstress_info.edma_send_sem, WAIT_FOREVER);

        if( 0 != EDMA_QueryCHNLState(EDMA_CHANNEL_START))
        {
            continue;
        }

        if(count > 0)
        {
            if(0 != memcmp( (void*)g_edma_busstress_info.stDMATaskHandle[cur_idx].u32DstAddr,\
                             (void*)g_edma_busstress_info.stDMATaskHandle[cur_idx].u32SrcAddr,EDMA_TEST_LEN) )
            {
                logMsg("edma %d task,handle idx:%d,compare data fail.\n",count-1,cur_idx,0,0,0,0);
                return -1;
            }
        }

        cur_idx = count%EDMA_HANDLE_NUM;
        memset((void*)(void*)g_edma_busstress_info.stDMATaskHandle[cur_idx].u32SrcAddr,cur_idx,EDMA_TEST_LEN);
        memset((void*)(void*)g_edma_busstress_info.stDMATaskHandle[cur_idx].u32DstAddr,0xff,EDMA_TEST_LEN);
        s32ret = EDMA_NormTaskLaunch( &(g_edma_busstress_info.stDMATaskHandle[cur_idx]) );
        g_edma_bustest_count++;
        if(s32ret != 0)
        {
            logMsg("EDMA_NormTaskLaunch fail,ret:0x%x.\n",s32ret,0,0,0,0,0);
            continue;
        }

        count++;
    }

    return OK;
}

void edma_timer_event(u32 param)
{
    if(NULL != g_edma_busstress_info.edma_send_sem)
    {
        /*up(g_edma_busstress_info.edma_send_sem);*/
        semGive(g_edma_busstress_info.edma_send_sem);
    }
    bsp_softtimer_add( &g_edma_busstress_info.edma_softtimer_list );
}
s32 edma_stress_test_start(s32 task_priority,s32 test_rate)
{
    s32 ret = 0;
    u32 index = 0;

    s32 chann_id = bsp_edma_channel_init(EDMA_MEMORY, 0, 0, 0);

    for (index=0;index<EDMA_HANDLE_NUM;index++)
    {
        g_edma_busstress_info.stDMATaskHandle[index].ulChIdx = chann_id;
    }
    logMsg("edma_stress: use axi 0x%x \n", MEM_FOR_EDMAC_BUSSTRESS,0,0,0,0,0);
    /*
    g_edma_busstress_info.stDMATaskHandle[0] = {0, 0,  EDMA_TEST_LEN,  chann_id};
    g_edma_busstress_info.stDMATaskHandle[1] = {0, EDMA_HANDLE_1_AXI_MEM_64_DST,   EDMA_TEST_LEN,  chann_id};
    g_edma_busstress_info.stDMATaskHandle[2] = {EDMA_HANDLE_2_AXI_MEM_64_SRC,  EDMA_HANDLE_2_AXI_MEM_64_DST, EDMA_TEST_LEN,  chann_id};
    g_edma_busstress_info.stDMATaskHandle[3] = {EDMA_HANDLE_3_AXI_MEM_64_SRC,  0,    EDMA_TEST_LEN,  chann_id};
    */
    set_test_switch_stat(EDMAC, TEST_RUN);

    /*创建控制发送速率的信号量*/
    /*sema_init(&(g_edma_busstress_info.edma_send_sem) , SEM_EMPTY);*/
    g_edma_busstress_info.edma_send_sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

    if (g_edma_busstress_info.edma_send_sem == NULL)
    {
        printf("semBCreate gmac_send_sem fail.\n");
        return ERROR;
    }
    g_edma_busstress_info.edma_softtimer_list.func = ( softtimer_func )edma_timer_event;
    g_edma_busstress_info.edma_softtimer_list.para = 0;
    g_edma_busstress_info.edma_softtimer_list.timeout = test_rate;
    g_edma_busstress_info.edma_softtimer_list.wake_type = SOFTTIMER_NOWAKE;

    /*创建控制发送速率 的软timer*/
    
    if(bsp_softtimer_create(&g_edma_busstress_info.edma_softtimer_list))
    {
        printf("SOFTTIMER_CREATE_TIMER fail.\n");
        /*sema_delete(g_edma_busstress_info.edma_send_sem);*/
        semDelete(g_edma_busstress_info.edma_send_sem);
        return ERROR;
    }

    g_edma_busstress_info.stDMATaskHandle[0].u32SrcAddr = (u32)cacheDmaMalloc(EDMA_TEST_LEN);
    g_edma_busstress_info.stDMATaskHandle[0].u32DstAddr = (u32)cacheDmaMalloc(EDMA_TEST_LEN);
    g_edma_busstress_info.stDMATaskHandle[1].u32SrcAddr = (u32)cacheDmaMalloc(EDMA_TEST_LEN);
    g_edma_busstress_info.stDMATaskHandle[3].u32DstAddr = (u32)cacheDmaMalloc(EDMA_TEST_LEN);
    /*创建发包测试任务，收包驱动会自行做处理*/
    g_edma_busstress_info.edma_stress_test_task_id \
            = taskSpawn ("edmaBusTask", task_priority, 0, 20000, (FUNCPTR)edma_stress_test_routine, 0,
                                        0, 0, 0,0, 0, 0, 0, 0, 0);
    if(g_edma_busstress_info.edma_stress_test_task_id == ERROR)
    {
        printf("taskSpawn edmaBusTask fail.\n");
        /*sema_delete(g_edma_busstress_info.edma_send_sem);*/
        semDelete(g_edma_busstress_info.edma_send_sem);
        ret = bsp_softtimer_free(&g_edma_busstress_info.edma_softtimer_list);
    }
    else
    {
        bsp_softtimer_add( &g_edma_busstress_info.edma_softtimer_list );
    }
    return g_edma_busstress_info.edma_stress_test_task_id;
}

s32 edma_stress_test_stop()
{
    set_test_switch_stat(EDMAC, TEST_STOP);

    while(OK == taskIdVerify(g_edma_busstress_info.edma_stress_test_task_id))
    {
        taskDelay(100);
    }

    if (NULL != g_edma_busstress_info.edma_send_sem)
    {
        /*sema_delete(g_edma_busstress_info.edma_send_sem);*/
        semDelete(g_edma_busstress_info.edma_send_sem);
    }


    bsp_softtimer_delete_sync(&g_edma_busstress_info.edma_softtimer_list);
    bsp_softtimer_free(&g_edma_busstress_info.edma_softtimer_list);

    cacheDmaFree(g_edma_busstress_info.stDMATaskHandle[0].u32SrcAddr);
    cacheDmaFree(g_edma_busstress_info.stDMATaskHandle[0].u32DstAddr);
    cacheDmaFree(g_edma_busstress_info.stDMATaskHandle[1].u32SrcAddr);
    cacheDmaFree(g_edma_busstress_info.stDMATaskHandle[3].u32DstAddr);
    return OK;
}



#if 1

#define HI_EDMA_CH4_REGBASE_ADDR 0x90026000


struct edma_busstress_info g_edma2_busstress_info =
{
{0},  /* struct softtimer_info   */
//0,    /* struct softtimer_list * */
-1,   /* edma_stress_test_task_id */
0,
{    /* AXI_DMA_TASK_HANDLE_S  stDMATaskHandle[EDMA_HANDLE_NUM]  注意个数 是宏定义的  */

    {0, 0,  EDMA_TEST_LEN,  EDMA_CHANNEL_INVALID},
    {0, EDMA2_HANDLE_1_AXI_MEM_64_DST,   EDMA_TEST_LEN,  EDMA_CHANNEL_INVALID},
    {EDMA2_HANDLE_2_AXI_MEM_64_SRC,  EDMA2_HANDLE_2_AXI_MEM_64_DST,   EDMA_TEST_LEN,  EDMA_CHANNEL_INVALID},
    {EDMA2_HANDLE_3_AXI_MEM_64_SRC,  0,    EDMA_TEST_LEN,  EDMA_CHANNEL_INVALID}

},    /* SEM_ID edma_send_sem */
{0},
{0},
{0},
{0}
};


s32 EDMA2_QueryCHNLState( u32 u32ChIdx )
{
    //s32 ret = -1;
    return (readl(HI_EDMA_CH4_REGBASE_ADDR+0x81c)&0x1);
}
s32 EDMA2_NormTaskLaunch(AXI_DMA_TASK_HANDLE_S * pstDMAHandle)
{
    if(EDMA2_QueryCHNLState(0))
    {
        logMsg("channel %d is_idle failed in EDMA2_NormTaskLaunch",0 ,0,0,0,0,0);
    	return -1;
    }

    writel(EDMA_TEST_LEN, (HI_EDMA_CH4_REGBASE_ADDR+0x810));
    writel(pstDMAHandle->u32SrcAddr, (HI_EDMA_CH4_REGBASE_ADDR+0x814));
    writel(pstDMAHandle->u32DstAddr, (HI_EDMA_CH4_REGBASE_ADDR+0x818));
    writel(0xCFF33000, (HI_EDMA_CH4_REGBASE_ADDR+0x81C));
    writel(0xCFF33001, (HI_EDMA_CH4_REGBASE_ADDR+0x81C));
    return 0;


}

u32 g_edma2_bustest_count = 0;
s32 edma2_stress_test_routine()
{
    u32 count = 0;
    s32 s32ret = 0;
    u32 cur_idx = 0;

    while(get_test_switch_stat(EDMAC))
    {

        /*down(g_edma2_busstress_info.edma_send_sem);*/
        semTake(g_edma2_busstress_info.edma_send_sem, WAIT_FOREVER);

        if( 0 != EDMA2_QueryCHNLState(EDMA_CHANNEL_START))
        {
            continue;
        }

        if(count > 0)
        {
            if(0 != memcmp( (void*)g_edma2_busstress_info.stDMATaskHandle[cur_idx].u32DstAddr,\
                             (void*)g_edma2_busstress_info.stDMATaskHandle[cur_idx].u32SrcAddr,EDMA_TEST_LEN) )
            {
                logMsg("edma2 %d task,handle idx:%d,compare data fail.\n",count-1,cur_idx,0,0,0,0);
                return -1;
            }
        }

        cur_idx = count%EDMA_HANDLE_NUM;
        memset((void*)(void*)g_edma2_busstress_info.stDMATaskHandle[cur_idx].u32SrcAddr,cur_idx,EDMA_TEST_LEN);
        memset((void*)(void*)g_edma2_busstress_info.stDMATaskHandle[cur_idx].u32DstAddr,0xff,EDMA_TEST_LEN);
        s32ret = EDMA2_NormTaskLaunch( &(g_edma2_busstress_info.stDMATaskHandle[cur_idx]) );
        g_edma2_bustest_count++;
        if(s32ret != 0)
        {
            logMsg("EDMA2_NormTaskLaunch fail,ret:0x%x.\n",s32ret,0,0,0,0,0);
            continue;
        }

        count++;
    }

    return OK;
}

void edma2_timer_event(u32 param)
{
    if(NULL != g_edma2_busstress_info.edma_send_sem)
    {
        /*up(g_edma2_busstress_info.edma_send_sem);*/
        semGive(g_edma2_busstress_info.edma_send_sem);
    }
    bsp_softtimer_add( &g_edma2_busstress_info.edma_softtimer_list );
}
s32 edma2_stress_test_start(s32 task_priority,s32 test_rate)
{
    s32 ret = 0;
    u32 index = 0;

    s32 chann_id = 0;/*bsp_edma_channel_init(EDMA_MEMORY, 0, 0, 0);*/

    for (index=0;index<EDMA_HANDLE_NUM;index++)
    {
        g_edma2_busstress_info.stDMATaskHandle[index].ulChIdx = chann_id;
    }
    logMsg("edma2_stress: use axi 0x%x \n", MEM_FOR_EDMAC2_BUSSTRESS,0,0,0,0,0);
    /*
    g_edma2_busstress_info.stDMATaskHandle[0] = {0, 0,  EDMA_TEST_LEN,  chann_id};
    g_edma2_busstress_info.stDMATaskHandle[1] = {0, EDMA2_HANDLE_1_AXI_MEM_64_DST,   EDMA_TEST_LEN,  chann_id};
    g_edma2_busstress_info.stDMATaskHandle[2] = {EDMA2_HANDLE_2_AXI_MEM_64_SRC,  EDMA2_HANDLE_2_AXI_MEM_64_DST, EDMA_TEST_LEN,  chann_id};
    g_edma2_busstress_info.stDMATaskHandle[3] = {EDMA2_HANDLE_3_AXI_MEM_64_SRC,  0,    EDMA_TEST_LEN,  chann_id};
    */
    set_test_switch_stat(EDMAC, TEST_RUN);

    /*创建控制发送速率的信号量*/
    /*sema_init(&(g_edma2_busstress_info.edma_send_sem) , SEM_EMPTY);*/
    g_edma2_busstress_info.edma_send_sem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

    if (g_edma2_busstress_info.edma_send_sem == NULL)
    {
        printf("semBCreate gmac_send_sem fail.\n");
        return ERROR;
    }
    g_edma2_busstress_info.edma_softtimer_list.func = ( softtimer_func )edma2_timer_event;
    g_edma2_busstress_info.edma_softtimer_list.para = 0;
    g_edma2_busstress_info.edma_softtimer_list.timeout = test_rate;
    g_edma2_busstress_info.edma_softtimer_list.wake_type =SOFTTIMER_NOWAKE;

    /*创建控制发送速率 的软timer*/
    if( bsp_softtimer_create(&g_edma2_busstress_info.edma_softtimer_list))
    {
        printf("SOFTTIMER_CREATE_TIMER fail.\n");
        /*sema_delete(g_edma2_busstress_info.edma_send_sem);*/
        semDelete(g_edma2_busstress_info.edma_send_sem);
        return ERROR;
    }

    g_edma2_busstress_info.stDMATaskHandle[0].u32SrcAddr = (u32)cacheDmaMalloc(EDMA_TEST_LEN);
    g_edma2_busstress_info.stDMATaskHandle[0].u32DstAddr = (u32)cacheDmaMalloc(EDMA_TEST_LEN);
    g_edma2_busstress_info.stDMATaskHandle[1].u32SrcAddr = (u32)cacheDmaMalloc(EDMA_TEST_LEN);
    g_edma2_busstress_info.stDMATaskHandle[3].u32DstAddr = (u32)cacheDmaMalloc(EDMA_TEST_LEN);
    /*创建发包测试任务，收包驱动会自行做处理*/
    g_edma2_busstress_info.edma_stress_test_task_id \
            = taskSpawn ("edmaBusTask", task_priority, 0, 20000, (FUNCPTR)edma2_stress_test_routine, 0,
                                        0, 0, 0,0, 0, 0, 0, 0, 0);
    if(g_edma2_busstress_info.edma_stress_test_task_id == ERROR)
    {
        printf("taskSpawn edma2BusTask fail.\n");
        /*sema_delete(g_edma2_busstress_info.edma_send_sem);*/
        semDelete(g_edma2_busstress_info.edma_send_sem);
        ret = bsp_softtimer_free(&g_edma2_busstress_info.edma_softtimer_list);
    }
    else
    {
        bsp_softtimer_add( &g_edma2_busstress_info.edma_softtimer_list );
    }
    return g_edma2_busstress_info.edma_stress_test_task_id;
}

s32 edma2_stress_test_stop()
{
    set_test_switch_stat(EDMAC, TEST_STOP);

    while(OK == taskIdVerify(g_edma2_busstress_info.edma_stress_test_task_id))
    {
        taskDelay(100);
    }

    if (NULL != g_edma2_busstress_info.edma_send_sem)
    {
        /*sema_delete(g_edma2_busstress_info.edma_send_sem);*/
        semDelete(g_edma2_busstress_info.edma_send_sem);
    }


    bsp_softtimer_delete_sync(&g_edma2_busstress_info.edma_softtimer_list);
    bsp_softtimer_free(&g_edma2_busstress_info.edma_softtimer_list);

    cacheDmaFree(g_edma2_busstress_info.stDMATaskHandle[0].u32SrcAddr);
    cacheDmaFree(g_edma2_busstress_info.stDMATaskHandle[0].u32DstAddr);
    cacheDmaFree(g_edma2_busstress_info.stDMATaskHandle[1].u32SrcAddr);
    cacheDmaFree(g_edma2_busstress_info.stDMATaskHandle[3].u32DstAddr);
    return OK;
}




#endif




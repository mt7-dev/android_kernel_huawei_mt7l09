
/*lint --e{944,506,525,64,119,101,132,537,958,438,830,752,762,713,732,569}*/

#include "copyright_wrs.h"
#include "vxWorks.h"
#include "arm_pbxa9.h"
#include <semLib.h>
#include <taskLib.h>
#include <logLib.h>
#include <iv.h>
#include "cacheLib.h"
#include "product_config.h"
#include "osl_types.h"
#include "osl_bio.h"
#include "soc_interrupts.h"
#include "drv_nv_id.h"
#include "drv_timer.h"
#include "drv_sci.h"
#include "bsp_om.h"
#include "bsp_regulator.h"
#include "bsp_edma.h"
#include "bsp_clk.h"
#include "bsp_nvim.h"
#include "bsp_icc.h"
#include "bsp_pmu.h"
#include "bsp_dump.h"
#include "bsp_dump_def.h"
#include "bsp_sci.h"
#include "sci_debug.h"
#include "sci_pl131.h"
#include "sci_apbitops.h"
#include "sci_appl131.h"



#ifndef WAIT_FOREVER
#define WAIT_FOREVER	(-1)
#endif

/*
 * Description:
 * SCI stae
 */
SCI_STATE_STRU g_strSciState = {PL131_STATE_UNINIT};

/*
 * Description:
 * Buffer in driver layer
 */
SCI_REC_DATA g_strRecBuf = {0};

/*
 * Description:
 * Synchronous semphor for SCI Data Receive Process and Call PS CallBack Function
 */
SEM_ID g_SCIRecvSyncSem = 0;

/*
 * Description:
 * Synchronous semphor for SCI Clock Start Process
 */
SEM_ID g_SCIClockStartSem = 0;

/*
 * Description:
 * Synchronous semphor for SCI Clock Stop Process
 */
SEM_ID g_SCIClockStopSem = 0;

/*
 * Description:
 * Synchronous semphor for data receive and power class switch process
 */
SEM_ID g_SCISynSem = 0;



SEM_ID g_SCIDeactiveSyncSem = 0;


/*
 * Description:
 * SCI Data Receive Process and Call back function task ID
 */
s32 g_SCIRxTaskId = 0;

/*
 * Description:
 * Synchronous task ID
 */
s32 g_slSCITaskId = 0;

/*PV400不支持DMA搬移模式，此处用宏隔开，如果以后6910采用DMA
  搬移模式，对PV400编译不会产生影响*/
SCI_RX_STATE_STRU g_sciRxStGblStru = {SCI_RX_CPU_MODE, -1, SCI_RX_DATA_REC_OVER};

/* 回调函数注册事件类型参数*/
u32 g_u32Event;

/* 回调函数注册函数指针*/
Sci_Event_Func g_FuncCb;

/* 可维可测平台注册函数*/
/*  Modified By Yangzhi for V7R2 */
//extern BSPRegExEventFunc g_pBSPRegExEventFunc;

u32 g_SciExcResetTimes = 0;

OMSCIFUNCPTR g_omSciFuncPtr = NULL;

/*
 * Description:
 * Enable WWT Time Out Deactivation Issue
 */
u32 g_ulChTOUTDeactEn  = 0;
u32 g_ulSciGcfStubFlag = 0;

/*
 * Description:
 * Flag of SCI Clock Time Stop Status
 */
apPL131_eClockMode g_ulSciClkTimeStpFlag = apPL131_CLOCK_START;

/*
 * Description:
 * Flag for SIM card deactivation process
 */
BSP_BOOL g_SciDeactivateFlag = BSP_FALSE;

u32 g_u32ResetSuccess = 0;

u32 g_u32SciSetSelect = SIM_SET_0;
/*读取数据标志，保证预读后读取长度满足条件后只调用一次回调函数*/
u32 g_u32ReadFlag = SCI_DEF_READ_FORBID;

/* pmu */
struct regulator *g_sciPmuRegu = NULL;
u32 g_sim_set_nv_select = SIM_SET_0;

SCI_SET_SELECT_STR g_sci_set_select;

/*add detect global var*/
SCI_DETECT_STATE g_sci_detect_state = SIM_CARD_IN;

/* sci vote ctrl flag */

BSP_BOOL g_sci_lp_vote_flag = BSP_TRUE;

/* 定义全局结构体记录SCI的硬件适配信息，放到指定的段中 */
SCI_CFG_STRU __sci_cfg_data g_stSciHwCfg = {0};

/*#pragma section DATA  ".sci.init.data"*/
/* 定义全局的函数指针，记录SCI_Init函数的地址，放到指定的段中*/
void * __sci_init_data g_fp_sci_init = (void *)sci_init;


SCI_VOLTAGE_STATE g_sci_voltage_state = {PL131_SCI_Class_BUTT,SCI_CLASS_SWITCH_NONE,SCI_VOLTAGE_SWITCH_SENSE_CLASS_BUTT,ATR_IND_CLASS_NONE};

/* sci debug global varis */
extern sci_debug_str  g_sci_debug_base;
/*
 * Description:
 * ATR Buffer in driver layer
 */
extern ATR_sBuffer g_strATRData;


#define SIM_NOT_OUT 1
#define SIM_CARD_LEAVE  3


/*
 * Description:
 * Limitted the activation times
 */
extern u8 nActiveT;
extern PL131_PPS_STATE g_Pl131_pps_state ;


/* Debug Regs */
extern u8* g_sciRegGblAddr;

u32 g_detect_leave_time = 0;
u32 g_sci_first_reset = 0;

extern int QueueLoopIn(dump_queue_t *Q, UINT32 element);
void sci_int_handler(void);

void sci_rec_sync_task(void);
void sci_buf_clear(void);

s32 sci_detect_cb(u32 channel_id, u32 len, void *context);
u32 bsp_sci_reg_event(u32 u32Event, Sci_Event_Func pFunc);
s32 sci_buf_add(u8 * pucRecbyte);
s32  sci_rx_dma_start(u32* u32DstAddr, u32 u32TransLength);
u32 sci_count_length(void);
s32 sci_clock_stop_mode_cfg( u32 ucClockStopMode, u32 ucClockStopInd);
void sci_reg_event(u32 u32Event, Sci_Event_Func pFunc);
void sci_class_switch_try(void);
s32 sci_set_voltage(SCI_VOLTAGE_SWITCH_DIRECTION direction);



/******************************************************************
* Function:  pwrctrl_sci_soft_clk_dis
* Description:
*      SCI SOFT CLK DISABLE
* Calls:
*      None
* Called By:
* Table Accessed: NONE
* Table Updated: NONE
* Inputs:
* None
* Outputs:
* None
* Return Value:
*  None
******************************************************************/
void pwrctrl_sci_soft_clk_dis(void)
{

#if 0
    if(SCI_BASE_ADDR == HI_SCI0_REGBASE_ADDR)
    {
        BSP_REG_SETBITS(HI_SYSCTRL_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET, SCI0_CLK_SET_BIT_SHIFT, 1, 1);
    }
    else
    {
        BSP_REG_SETBITS(HI_SYSCTRL_BASE_ADDR, HI_CRG_CLKDIS4_OFFSET, SCI1_CLK_SET_BIT_SHIFT, 1, 1);
    }
#endif
    BSP_REG_SETBITS(HI_SYSCTRL_BASE_ADDR, g_stSciHwCfg.clk_dis_reg, g_stSciHwCfg.clk_dis_bit, 1, 1);
}

/******************************************************************
* Function:  pwrctrl_sci_soft_clk_en
* Description:
*      SCI SOFT CLK ENABLE
* Calls:
*      None
* Called By:
* Table Accessed: NONE
* Table Updated: NONE
* Inputs:
* None
* Outputs:
* None
* Return Value:
*  None
******************************************************************/

void pwrctrl_sci_soft_clk_en(void)
{
#if 0
    if(SCI_BASE_ADDR == HI_SCI0_REGBASE_ADDR)
    {
        BSP_REG_SETBITS(HI_SYSCTRL_BASE_ADDR, HI_CRG_CLKEN4_OFFSET, SCI0_CLK_SET_BIT_SHIFT, 1, 1);
    }
    else
    {
        BSP_REG_SETBITS(HI_SYSCTRL_BASE_ADDR, HI_CRG_CLKEN4_OFFSET, SCI1_CLK_SET_BIT_SHIFT, 1, 1);
    }
#endif

    BSP_REG_SETBITS(HI_SYSCTRL_BASE_ADDR, g_stSciHwCfg.clk_en_reg, g_stSciHwCfg.clk_en_bit, 1, 1);
}

/******************************************************************
* Function:  pwrctrl_sci_low_power_enter
* Description:
*      SCI vote agree sleep.
* Calls:
*      None
* Called By:
* Table Accessed: NONE
* Table Updated: NONE
* Inputs:
* None
* Outputs:
* None
* Return Value:
*  None
******************************************************************/
int pwrctrl_sci_low_power_enter(void)
{
    int result = 0;

	if(BSP_TRUE != g_sci_lp_vote_flag)
	{
		result = BSP_PWRCTRL_SleepVoteUnLock(g_stSciHwCfg.sleep_vote);
	    if(result != 0)
	    {
	        sci_print_error("%d:SCI vote unlock failed, result=%d, line=%d.\n", g_stSciHwCfg.card_id,result,__LINE__,0,0,0);

            SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, result, __LINE__);

	        return BSP_ERROR;
	    }
	    else
	    {
	    	g_sci_lp_vote_flag = BSP_TRUE;/* [false alarm]:  误报*/
	        return BSP_OK;
	    }
	}

	return BSP_ERROR;

}

/******************************************************************
* Function:  pwrctrl_sci_low_power_exit
* Description:
*      SCI vote dont sleep.
* Calls:
*      None
* Called By:
* Table Accessed: NONE
* Table Updated: NONE
* Inputs:
* None
* Outputs:
* None
* Return Value:
*  None
******************************************************************/
int pwrctrl_sci_low_power_exit(void)
{
    int result = 0;

	if(BSP_FALSE != g_sci_lp_vote_flag)
	{
		result = BSP_PWRCTRL_SleepVoteLock(g_stSciHwCfg.sleep_vote);
	    if(result != 0)
	    {
	        sci_print_error("%d:SCI vote lock failed, result=%d, line=%d.\n", g_stSciHwCfg.card_id,result,__LINE__,0,0,0);

            SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, result, __LINE__);

	        return BSP_ERROR;
	    }
	    else
	    {
	    	g_sci_lp_vote_flag = BSP_FALSE;/* [false alarm]: fortify 误报*/
	        return BSP_OK;
	    }
	}

	return BSP_ERROR;

}

/******************************************************************
* Function:  sci_detect_cb

* Description:
* Calls:
*      None
* Called By:
* Table Accessed: NONE
* Table Updated: NONE
* Inputs:
* None
* Outputs:
* None
* Return Value:
*  None
******************************************************************/
s32 sci_detect_cb(u32 channel_id, u32 len, void *context)
{
	u32 sci_detect_status = 0;
	u32 read_len = 0;

	read_len = bsp_icc_read(channel_id, (u8*)&sci_detect_status, len);
	if(read_len != len)
	{
		sci_print_error("readed len(%d) != expected len(%d)\n", read_len, len);
		return BSP_ERROR;
	}

    sci_print("sci_detect_cb: cnt is %d\n",sci_detect_status);


    switch(sci_detect_status)
    {
       
       case SIM_CARD_IN:
           g_sci_detect_state = SIM_CARD_IN;
           SCI_RECORD_EVENT(SCI_EVENT_DETECT_CARD_IN,  omTimerGet() , __LINE__);
           break;
       case SIM_CARD_OUT:
           g_sci_detect_state = SIM_CARD_OUT;
           SCI_RECORD_EVENT(SCI_EVENT_DETECT_CARD_OUT,  omTimerGet() , __LINE__);
           break;
#ifdef BSP_CONFIG_HI3630
       case SIM_CARD_LEAVE:
            g_sci_detect_state = SIM_CARD_LEAVE;
            g_detect_leave_time = omTimerGet();
            SCI_RECORD_EVENT(SCI_EVENT_DETECT_CARD_LEAVE,  g_detect_leave_time , __LINE__);
            break;
#endif
       default:
            break;
            
    }
    
    /* send to usim */
    if(NULL != g_omSciFuncPtr)
	{
	    if(g_sci_detect_state == SIM_CARD_OUT)
        {
            ((OMSCIFUNCPTR)g_omSciFuncPtr)(SIM_CARD_OUT);
            SCI_RECORD_EVENT(SCI_EVENT_DETECT_IND_USIM, SIM_CARD_OUT, __LINE__);
        } 
        if(g_sci_detect_state == SIM_CARD_IN)
        {
            ((OMSCIFUNCPTR)g_omSciFuncPtr)(sci_detect_status);
            SCI_RECORD_EVENT(SCI_EVENT_DETECT_IND_USIM, sci_detect_status, __LINE__);
        }		
	}
	return BSP_OK;

}

int sci_dma_init(void)
{
	int u32DmaChanId = 0;

    /* 初始化DMA通道 */
    if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
    {
		u32DmaChanId = bsp_edma_channel_init(g_stSciHwCfg.dma_chan, NULL, 0, 0);
		if(u32DmaChanId < 0)
		{
			sci_print_error("dma init failed! \n"); /* [false alarm]:*/
			return ERROR;
		}
	    if(BSP_OK!= bsp_edma_channel_set_config(u32DmaChanId, EDMA_P2M, 2, 0))
	    {
			sci_print_error("dma config failed! \n");
			return ERROR;
		}

        g_sciRxStGblStru.DmaChnNo = u32DmaChanId;

        /*Set Default SCI Rx Data Receive Status Flag*/
        g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_OVER;

        /*Create SCI DMA Rx Data Buffer*/
        if (NULL == g_strRecBuf.usOutBuf)
        {
            //when reveived one byte from sci, extend it to one word in usOutBuf
            if (NULL
                == (g_strRecBuf.usOutBuf = (u16 *)cacheDmaMalloc(SCI_MAX_DATA_BUF_LEN * sizeof(u32))))
            {
                sci_print_error("malloc memory error!\n");
                return ERROR;
            }
        }
    }
    else
    {
        /*Set SCI Rx Mode is CPU*/
        g_sciRxStGblStru.RxModeFlag = SCI_RX_CPU_MODE;
    }

    return OK;
}

/*lint +e525*/
/**************************************************
*  Function:  sci_init
*
*  Description:
*      Initialise the SCI.
*
*  Calls:
*      sci_buf_clear
*      intConnect
*      intEnable
*
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         None
*
*  Output:
*         None
*
*  Return:
*         None
*
******************************************************************/
void sci_init(void)
{
    SCI_STATE_STRU *const pstrState = &g_strSciState;

    s32 ret = BSP_OK;
	u32 sci_gcf_flag_nv = 0;

	memset((void *)pstrState, 0, sizeof(SCI_STATE_STRU));
    pstrState->pBase = (SCI_REG *) (g_stSciHwCfg.base_addr);
    pstrState->eCurrentState = PL131_STATE_FIRSTINIT;

    if (!g_SCIRecvSyncSem)
    {
        g_SCIRecvSyncSem = semBCreate(SEM_Q_FIFO, (SEM_B_STATE)SEM_EMPTY);
        if (NULL == g_SCIRecvSyncSem)
        {
            sci_print_error("create Recv Semaphore failed.\n");
            SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, (u32)g_SCIRecvSyncSem, (__LINE__));
            return;
        }
    }

    if (!g_SCIClockStartSem)
    {
        g_SCIClockStartSem = semBCreate(SEM_Q_FIFO, (SEM_B_STATE)SEM_EMPTY );
        if (NULL == g_SCIClockStartSem)
        {
            sci_print_error("create Timer Semaphore failed.\n");
            SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, (u32)g_SCIClockStartSem, (__LINE__));
            return;
        }
    }

    if (!g_SCIClockStopSem)
    {
        g_SCIClockStopSem = semBCreate(SEM_Q_FIFO, (SEM_B_STATE)SEM_EMPTY );
        if (NULL == g_SCIClockStopSem)
        {
            sci_print_error("create Timer Semaphore failed.\n");
            SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, (u32)g_SCIClockStopSem, (__LINE__));
            return;
        }
    }

    if (!g_SCISynSem)
    {
        g_SCISynSem = semBCreate(SEM_Q_FIFO, (SEM_B_STATE)SEM_EMPTY );
        if (NULL == g_SCISynSem)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SCI, "create Sync Semaphore failed.\n", 0, 0, 0, 0, 0, 0);
            return;
        }
    }

	if(!g_SCIDeactiveSyncSem)
    {
        g_SCIDeactiveSyncSem  = semBCreate(SEM_Q_FIFO, (SEM_B_STATE)SEM_EMPTY );
        if (NULL == g_SCIDeactiveSyncSem )
        {
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SCI, "create g_SCIDeactiveSyncSem  Semaphore failed.\n", 0, 0, 0, 0, 0, 0);
            return;
        }
    }

    if (!g_slSCITaskId)
    {
        g_slSCITaskId = taskSpawn(g_stSciHwCfg.syncTaskName, 128, 0, 20000, (FUNCPTR)sci_rec_sync_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    /* init interrupts*/
	pwrctrl_sci_soft_clk_en();
	writel(0, SCI_REG_INTMASK);
	writel(0xffff, SCI_REG_INTCLEAR);

    intConnect((VOIDFUNCPTR*)(g_stSciHwCfg.int_number), (VOIDFUNCPTR)sci_int_handler, 0);
    intEnable(g_stSciHwCfg.int_number);

    sci_dma_init();

    /* init sim card detect icc channel */
	ret = bsp_icc_event_register(g_stSciHwCfg.icc_chnl, sci_detect_cb, NULL, NULL, NULL);/*[false alarm]:*/
	if(ret != BSP_OK)
	{
		sci_print_error("icc register failed\n");
	}

    /* init sci record */
	sci_record_init();

    if(NV_OK != bsp_nvm_read(NV_ID_DRV_SCI_GCF_STUB_FLAG, (u8*)&sci_gcf_flag_nv, sizeof(u32)))
    {
        sci_print_error("bsp_sci_reset: get gcf nv failed!\n");
    }
	g_ulSciGcfStubFlag = sci_gcf_flag_nv;

	sci_print("[sci]: sim%d init OK!\n",__FUNCTION__,g_stSciHwCfg.card_id);

    SCI_RECORD_EVENT(SCI_EVENT_SCI_INIT_SUCCESS, (u32)g_stSciHwCfg.card_id, (__LINE__));

	return ;

}

/******************************************************************
* Function:  sciBufAdd
*
* Description:
*      Add data received by interrupt to driver's buffer
*
* Calls:
*      None
*
* Called By:  Interrupt Service Process
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
* pucRecbyte: the data received by ISR
*
* Outputs:
* None
*
* Return Value:
*  OK  - successful completion
*  ERROR  - failed
*
******************************************************************/
s32 sci_buf_add(u8 * pucRecbyte)
{
	/* buffer overflow Begin: */
    SCI_REG *pstrBase  = (SCI_REG *) (SCI_BASE_ADDR);;
    /* buffer overflow End! */
	
    if ((NULL == pucRecbyte))
    {
        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, 0, __LINE__);

        sci_print_error("para err,NULL == pucRecbyte\n");
        return BSP_ERROR;
    }

	if(g_strRecBuf.usLength >= SCI_BUF_MAX_LEN)
	{
	    SCI_RECORD_EVENT(SCI_EVENT_ERROR_DATA_REC_BUF_OVR,g_strRecBuf.usLength,(u32)pucRecbyte);

        sci_print_error("usLength is larger than SCI_BUF_MAX_LEN, and is %d\n", g_strRecBuf.usLength);

        /* buffer overflow Begin: */
        apBIT_SET(pstrBase->RegCtrl2, PL131_REGCTRL2_FINISH, 1);

        g_SciDeactivateFlag = TRUE;
		/* buffer overflow End! */
		
	}
    else
    {
         /* 将收取的数据保存在buffer中*/
        g_strRecBuf.ucRecBuf[g_strRecBuf.usLength] = *pucRecbyte;

        g_strRecBuf.usLength++;
    }
    

    /* 如果收取的数据长度大于等于预读的数据长度，则释放信号量，在sciRxTask任务中调用回调函数将数据返回给上层*/
    if ((g_strRecBuf.usPrelen != 0) && (g_strRecBuf.usPrelen <= (g_strRecBuf.usLength - g_strRecBuf.usStart))
        && (SCI_DEF_READ_READY == g_u32ReadFlag))
    {
        g_strRecBuf.usPrelen = 0;

        g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_SATISFIED;
        semGive(g_SCIRecvSyncSem);

    }

    return OK;
}

/******************************************************************
* Function:  sci_buf_clear
*
* Description:
*       Clear driver layer's buffer
*
* Calls:
*      None
*
* Called By:  None
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
*  None
*
* Outputs:
*  None
*
* Return Value:
*  None
*
******************************************************************/
void sci_buf_clear(void)
{
    g_strRecBuf.usLength = 0;
    g_strRecBuf.usStart  = 0;
    g_strRecBuf.usPrelen = 0;

    /* added by yangzhi for dma transfer,2010-11-25,begin*/
    if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
    {
        if (NULL != g_strRecBuf.usOutBuf)
        {
            memset((void *)(g_strRecBuf.usOutBuf), 0x00, (SCI_MAX_DATA_BUF_LEN * sizeof(unsigned short)));
        }
    }

    /* added by yangzhi for dma transfer,2010-11-25,end*/
}

void sci_atr_clear(void)
{
    g_strATRData.ulATRLen = 0;
    memset(g_strATRData.ATRData, 0, 256);
}

/******************************************************************
*  Function:  sciRegEvent
*
*  Description:
*      注册回调函数
*
*  Calls:
*      None
*
*  Called By:  PS software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         u32 u32Event      注册事件
*         Sci_Event_Func pFunc  回调函数
*  Outputs:
*
* Return Value:
*         None
*
******************************************************************/
void sci_reg_event(u32 u32Event, Sci_Event_Func pFunc)
{
    g_u32Event = u32Event;
    g_FuncCb = pFunc;
}
/******************************************************************
* Function:  sci_rx_dma_start
*
* Description:
*       start dma transfer
* Calls:
*      None
*
* Called By:  None
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
*  u32DstAddr: buffer address of data received;
*  u32TransLength: data transfer length;
*
* Outputs:
*  None
*
* Return Value:
*  None
*
******************************************************************/
s32  sci_rx_dma_start(u32* u32DstAddr, u32 u32TransLength)
{

	u32 u32SrcAddr = 0;

    /* open DMA clock gating*/

	if(NULL == u32DstAddr)
	{
	    return BSP_ERROR;
	}

	/* 设置EDMA源地址,此处不需要用宏隔 */
	u32SrcAddr = SCI_REG_DATA;

    if(bsp_edma_channel_stop(g_sciRxStGblStru.DmaChnNo) < 0)
	{
		sci_print_error("dma transfer stop failed! \n");
        SCI_RECORD_EVENT(SCI_EVENT_DMA_ERROR,(u32)u32DstAddr,(u32)u32TransLength);
      	return BSP_ERROR;
	}

	/*启动一次EDMA异步数据搬运*/
    if (bsp_edma_channel_async_start(g_sciRxStGblStru.DmaChnNo, u32SrcAddr, (u32)u32DstAddr, u32TransLength))
    {
        SCI_RECORD_EVENT(SCI_EVENT_DMA_ERROR,(u32)u32DstAddr,(u32)u32TransLength);
	    sci_print_error("transfer failed! \n");
        return BSP_ERROR;
    }

    return BSP_OK;
}


/******************************************************************
* Function:  sci_count_length
*
* Description:
*      SCI count receive buffer total length.
*
*
* Calls:
*      None
*
* Called By:
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
* None
*
* Outputs:
* None
*
* Return Value:
*  None
*
******************************************************************/
u32 sci_count_length(void)
{
    u32 ulSciDMADstAddr = 0;
    u32 receivedDataNum = 0;
    u32 ulSciBufAddr = (u32)g_strRecBuf.usOutBuf;

    if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
    {
		ulSciDMADstAddr = bsp_edma_current_transfer_address(g_sciRxStGblStru.DmaChnNo);
		if(ulSciDMADstAddr > 0)
		{
			g_strRecBuf.usLength = (u16)((ulSciDMADstAddr - ulSciBufAddr) / sizeof(u32));

		}
    }

    receivedDataNum = g_strRecBuf.usLength - g_strRecBuf.usStart;
    return receivedDataNum;
}

/******************************************************************
* Function:  sciClockStopModeCfg
*
* Description:
*      start/stop SCI clock
*
* Calls:
*      appl131_data_transfer
*
* Called By:  PS software
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
* ucClockStopMode - If the card supports clock stop mode, this parameter is used
*                              to stop or start the clock.
*                              0: stat the SCI clock
*                              1: stop the SCI clock
*
* Outputs:
* none
*
* Return Value:
*  OK - successful completion
*  ERROR - failed
*  DR_ERR_SCI_PARAERROR - invalid mode specified
*  DR_ERR_SCI_UNSUPPORTED - not support such a operation
*
******************************************************************/
s32 sci_clock_stop_mode_cfg( u32 ucClockStopMode, u32 ucClockStopInd)
{
    s32 staErr;

    staErr = appl131_clock_stop_mode_set(&g_strSciState,
                                       (apPL131_eClockMode)ucClockStopMode, ucClockStopInd);
    if (apERR_UNSUPPORTED == staErr)
    {
        sci_print_error("appl131_clock_stop_mode_set fail = %d.\r\n", staErr, 0, 0, 0, 0, 0);
        return BSP_ERR_SCI_UNSUPPORTED;
    }
    else
    {
        return OK;
    }
}


/******************************************************************
*  Function:  bsp_sci_func_register
*
*  Description:
*      Initialise the SCI.
*
*  Calls:
*      apPL131_ParamsSet
*      sci_buf_clear
*      intConnect
*      intEnable
*      semBCreate
*      taskSpawn
*      apPL131_CardActiveSet
*
*  Called By:  PS software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         None
*
*  Output:
*         None
*
*  Return:
*         None
*
******************************************************************/
void bsp_sci_func_register(OMSCIFUNCPTR omSciFuncPtr)
{
	if (NULL == omSciFuncPtr)
    {
        sci_print_error("the input parameter is null!");
        return ;
    }

    g_omSciFuncPtr = omSciFuncPtr;
}


/******************************************************************
*  Function:  sciReset
*
*  Description:
*      Initialise the SCI.
*
*  Calls:
*      apPL131_ParamsSet
*      sci_buf_clear
*      intConnect
*      intEnable
*      semBCreate
*      taskSpawn
*      apPL131_CardActiveSet
*
*  Called By:  PS software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         None
*
*  Output:
*         None
*
*  Return:
*         None
*
******************************************************************/
s32 bsp_sci_reset(RESET_MODE rstMode)
{
    s32 retvalue   = -1;
    SCI_REG *pstrBase  = BSP_NULL;
    SCI_STATE_STRU *const pstrState = &g_strSciState;
    u32 i = 0;
#ifdef BSP_CONFIG_HI3630
    u32 pSciInOut = SIM_NOT_OUT;
    u32 current_time = 0;
#endif

    SCI_RECORD_EVENT(SCI_EVENT_API_RESET_START, omTimerGet() , __LINE__);

#ifdef BSP_CONFIG_HI3630
    current_time = omTimerGet();
    if((((current_time - g_detect_leave_time)/32) > 500) || g_sci_first_reset == 0)
    {
        if(g_sci_detect_state == SIM_CARD_LEAVE)
        {
            SCI_RECORD_EVENT(SCI_EVENT_DETECT_IND_M3, pSciInOut, __LINE__);
            bsp_icc_send(ICC_CPU_MCU,g_stSciHwCfg.icc_chnl, (u8*)&pSciInOut, sizeof(u32));
            g_detect_leave_time = 0;
        }
    }
    
#endif
    g_sci_first_reset = 1;
    pwrctrl_sci_low_power_exit();
    SCI_RECORD_EVENT(SCI_EVENT_LOW_POWER_EXIT, 0 , __LINE__);
	pwrctrl_sci_soft_clk_en();

#ifdef BSP_CONFIG_HI3630
    /* reset ip*/
    if(CARD_CONNECT_SCI0 == g_stSciHwCfg.sci_id)
    {
        BSP_REG_SETBITS(HI_AP_SYSCTRL_BASE_ADDR, SCI_RESET_REG_ADDR, SCI_RESET_CARD0_BIT_SHIFT, 1, 1);
    }
    else
    {
        BSP_REG_SETBITS(HI_AP_SYSCTRL_BASE_ADDR, SCI_RESET_REG_ADDR, SCI_RESET_CARD1_BIT_SHIFT, 1, 1);
    }
#endif

	memset((void *)pstrState, 0, sizeof(SCI_STATE_STRU));
    pstrState->pBase = (SCI_REG *) (g_stSciHwCfg.base_addr);
    pstrBase = pstrState->pBase;

#ifdef BSP_CONFIG_HI3630
    /* unreset ip*/
    if(CARD_CONNECT_SCI0 == g_stSciHwCfg.sci_id)
    {
        BSP_REG_SETBITS(HI_AP_SYSCTRL_BASE_ADDR, SCI_UNRESET_REG_ADDR, SCI_RESET_CARD0_BIT_SHIFT, 1, 1);
    }
    else
    {
        BSP_REG_SETBITS(HI_AP_SYSCTRL_BASE_ADDR, SCI_UNRESET_REG_ADDR, SCI_RESET_CARD1_BIT_SHIFT, 1, 1);
    }
#endif

    /* 清空buffer */
    sci_buf_clear();
    sci_atr_clear();

    /*mask off & clear all interrupts*/
	writel(SCI_DEF_MASK_ALL_IRQ, SCI_REG_INTMASK);
	writel(SCI_DEF_CLEAR_ALL_IRQ, SCI_REG_INTCLEAR);

    /*set pps related parameters as default value*/
    g_Pl131_pps_state.PL131_PPS_ACTIVATION = 0;
    g_Pl131_pps_state.ucTA1Flag = 0;
    g_Pl131_pps_state.ucPPSFlag = 0;
    g_Pl131_pps_state.PL131_PPS_Response = 0;
    nActiveT = 0;

    /*读取数据标志，保证预读后读取长度满足条件后只调用一次回调函数*/
    g_u32ReadFlag = SCI_DEF_READ_FORBID;

    g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_OVER;

    /* 先下电，保证是cold reset */
    if(g_sciPmuRegu == NULL)
    {
        g_sciPmuRegu = regulator_get(NULL, g_stSciHwCfg.pwr_type);
		if (NULL == g_sciPmuRegu )
		{
			sci_print_error("get pmu device failed.\n");
            SCI_RECORD_EVENT(SCI_EVENT_REGULATOR_ERR,(*(g_stSciHwCfg.pwr_type)) , __LINE__);
	        return ERROR;
		}
    }	
    for(;i < 10 ;i++)
    {
        if(g_SciDeactivateFlag == TRUE)
        {
            taskDelay(1);
        }
        else
        {
            break;
        }
    }

    retvalue = sci_set_voltage(SCI_VOLTAGE_SWITCH_CLASS_ZERO2CB);/*[false alarm]:*/
    
    if(retvalue != BSP_OK)
    {
       SCI_RECORD_EVENT(SCI_EVENT_API_RESET_FAILED,g_sci_voltage_state.sciClassKind, __LINE__);
    }


    /*set default configuration*/
    pstrState->eCurrentState = PL131_STATE_FIRSTINIT;    /*ensures defaults used*/
    pstrState->sSetupParams.ClockFreq = apPL131_DEF_REF_CLOCK;
    pstrState->sSetupParams.eRxHandshake = apPL131_EVEN_HANDSHAKE;
    pstrState->sSetupParams.RxRetries = PL131_DEF_RX_RETRIES;
    pstrState->sSetupParams.TxRetries = PL131_DEF_TX_RETRIES;
    pstrState->sSetupParams.CardFreq = PL131_DEF_CARDFREQ;
    pstrState->sSetupParams.eTxHandshake = apPL131_EVEN_HANDSHAKE;

    retvalue = appl131_params_set(&g_strSciState, NULL, NULL, NULL);/* [false alarm]:误报*/
    if ((retvalue != apERR_NONE) && (retvalue != apERR_PL131_USEDEFAULT))
    {
        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);
        SCI_RECORD_EVENT(SCI_EVENT_API_RESET_FAILED,g_strSciState.eCurrentState, __LINE__);

        sci_print_error("set parameter interface is error.\n");
        return ERROR;
    }

    /*set up other registers if not from power on reset*/
    BIT_SET(pstrBase->RegCtrl0, PL131_REGCTRL0_CONV, PL131_CONVENTION_DIRECT);/*[false alarm]:*/
    BIT_SET(pstrBase->RegDMACtrl, PL131_REGDMA, apPL131_DMA_OFF); /* Disable DMA */    
    BIT_SET(pstrBase->RegCtrl0, PL131_REGCTRL0_PARITYBIT, 1);/*设置奇偶位*/
    
    if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)/* [false alarm]: 误报*/
    {   
        SCI_RECORD_EVENT(SCI_EVENT_DMA_START, g_sciRxStGblStru.RxModeFlag, __LINE__);

        if (OK != sci_rx_dma_start((u32*)(g_strRecBuf.usOutBuf), SCI_MAX_DATA_BUF_LEN * sizeof(u32)))/*lint !e740 !e826*/
        {
            sci_print_error("dma transfer start failed! \n");
            return ERROR;
        }

    }

    if (pstrState->eCurrentState == PL131_STATE_INACTIVECARD)/*[false alarm]:*/
    {
        SCI_RECORD_EVENT(SCI_EVENT_CARD_ACTIVE_START, pstrState->eCurrentState, __LINE__);/*[false alarm]:*/

        retvalue = appl131_card_active_set(pstrState, TRUE, pstrState->sATRParams.pATRBuffer);
        if (retvalue != apERR_NONE)
        {
            SCI_RECORD_EVENT(SCI_EVENT_API_RESET_FAILED,g_strSciState.eCurrentState, __LINE__);

            sci_print_error("card active set interface is error.\n");
            return retvalue;
        }
        SCI_RECORD_EVENT(SCI_EVENT_CARD_ACTIVE_SUCCESS, pstrState->eCurrentState, __LINE__);
    }
    else
    {
        SCI_RECORD_EVENT(SCI_EVENT_ERROR_NOCARD, pstrState->eCurrentState, __LINE__);

        sci_print_error("no card!\n");
    }

	g_ulSciClkTimeStpFlag = apPL131_CLOCK_START;
    g_u32ResetSuccess = SCI_DEF_RESET_SUCCESS;

    if(PL131_STATE_READY == pstrState->eCurrentState)
    {
        g_sci_detect_state = SIM_CARD_IN;
    }
    else
    {
        g_sci_detect_state = SIM_CARD_OUT;
    }
    SCI_RECORD_EVENT(SCI_EVENT_API_RESET_FINISH, pstrState->eCurrentState, __LINE__);
    return OK;
}

/******************************************************************
*  Function:  bsp_sci_card_status_get
*
*  Description:
*      Get the SCI Status.
*
*  Calls:
*      None
*
*  Called By:  PS software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         None
*
*  Outputs:

* pulCardStatus - 0 card ok
*                 1 card error
*
* Return Value:
*  OK  - successful completion
*  ERROR  - failed
*
******************************************************************/
u32 bsp_sci_card_status_get (void)
{
    SCI_STATE_STRU * const pstrState = &g_strSciState;
	u32 u32CardStatus = 0xFF;

    SCI_RECORD_EVENT(SCI_EVENT_API_GET_STATUS, pstrState->eCurrentState, __LINE__);

    if (pstrState->eCurrentState >= PL131_STATE_READY)
    {
        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        u32CardStatus = SCI_CARD_STATE_READY;
    }
    else if ((pstrState->eCurrentState <= PL131_STATE_NOCARD) && \
		(pstrState->eCurrentState != PL131_STATE_FIRSTINIT))
    {
        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        u32CardStatus = SCI_CARD_STATE_NOCARD;
    }
	else
	{
	    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);
		u32CardStatus = SCI_CARD_STATE_BUSY;
	}

	return (u32CardStatus);
}

/******************************************************************
* Function:  sciDataSend
*
* Description:
*      Begins data transfer from the SCI buffer to the card
*      This will send the specified number of bytes from buffer to the
*      card, using an interrupt-driven system.
*
* Calls:
*      appl131_data_transfer
*
* Called By:  PS software
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
* pucDataBuffer - location of data to send
* ulDataLength - number of bytes/characters to send
*
* Outputs:
* none
*
* Return Value:
*  OK - transmission commenced successfully
*  DR_ERR_SCI_NOTINIT - initialisation routine not run
*  DR_ERR_SCI_DISABLED - driver is in a disabled state
*  DR_ERR_SCI_NOCARD - no card present
*  DR_ERR_SCI_NODATA - buffer is NULL or length is zero
*
*
******************************************************************/
s32 bsp_sci_data_send(u32 ulDataLength, unsigned char *pucDataBuffer)
{
       
    SCI_RECORD_EVENT(SCI_EVENT_API_DATA_SEND, (u32)pucDataBuffer, __LINE__);

	if ((0 == ulDataLength) || (NULL == pucDataBuffer) || (ulDataLength > SCI_DEF_SEND_MAX_LENGTH))
    {
        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, (u32)pucDataBuffer, __LINE__);

        sci_print_error("the input parameter ERROR!");
        return BSP_ERR_SCI_INVALIDPARA;
    }

    if (g_u32ResetSuccess != SCI_DEF_RESET_SUCCESS)
    {
        sci_print_error("not initialized\n");
        return BSP_ERR_SCI_NOTINIT;
    }

    sci_buf_clear();


    if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
    {
        SCI_RECORD_EVENT(SCI_EVENT_DMA_START, ulDataLength, __LINE__);

        if (BSP_OK != sci_rx_dma_start((u32*)(g_strRecBuf.usOutBuf), SCI_MAX_DATA_BUF_LEN * sizeof(u32)))/*lint !e740 !e826*/
        {
            sci_print_error("sci_data_send: dma transfer start failed\n");
            return ERROR;
        }
    }

    return appl131_data_transfer (&g_strSciState, apPL131_TX_TO_CARD, pucDataBuffer, ulDataLength);
}



/******************************************************************
* Function:  sciDataReadSync
*
* Description:
*      Read specified length data from the driver.
*      This function wil be called after the PS software get the drivers information.
*
* Calls:
*      memcpy
*
* Called By:  PS Software
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
* pucRecData - The data buffer to be copied to.
* ulLength - number of bytes/characters to receive
*
* Outputs:
* none
*
* Return Value:
*  OK - transmission commenced successfully
*  DR_ERR_SCI_PARAERROR - invalid mode specified
*
******************************************************************/
s32 bsp_sci_data_read_sync(u32 ulLength, u8 *pucRecData)
{
    s32 sciLockKey = 0;
    u32 ulLoopIndex;
    u32 recDataNum = 0;

	SCI_STATE_STRU * const pstrState = &g_strSciState;
    
    SCI_RECORD_EVENT(SCI_EVENT_API_DATA_REC_START, (u32)pucRecData, __LINE__);

	if ((ulLength > SCI_DEF_RECEIVE_MAX_LENGTH) || (NULL == pucRecData))
    {
        sci_print_error("u32Length = %d || pu8Data is NULL!",ulLength,0,0,0,0,0);

        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        return BSP_ERR_SCI_INVALIDPARA;
    }


    if (g_u32ResetSuccess != SCI_DEF_RESET_SUCCESS)
    {
        sci_print_error("not initialized\n");
        return BSP_ERR_SCI_NOTINIT;
    }

    if ((NULL == pucRecData) || (0 == ulLength))
    {
        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        sci_print_error("read data length:%d, data pointer: 0x%x\n",ulLength, pucRecData, 0, 0, 0, 0);

        return BSP_ERR_SCI_INVALIDPARA;
    }

    g_u32ReadFlag = SCI_DEF_READ_READY;
	apBIT_SET((pstrState->pBase->RegIntClear), PL131_REGINTMASK_CHTOUT, 1);
	apBIT_SET((pstrState->pBase->RegIntMask), PL131_REGINTMASK_CHTOUT, 1);
    /* coverity[lock_acquire] */
    sciLockKey = intLock();
    recDataNum = sci_count_length();

    /* 如果当前数据量已经满足，直接拷贝数据返回 */
    if (recDataNum >= (u16)ulLength)
    {
        g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_SATISFIED;

        //added by yangzhi for dma test in 2010-11-25
        if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
        {
            (void)cacheInvalidate(DATA_CACHE, (u8 *)g_strRecBuf.usOutBuf, SCI_MAX_DATA_BUF_LEN * sizeof(u32));
            for (ulLoopIndex = 0; ulLoopIndex < ulLength; ulLoopIndex++)
            {
                //copy data from usOutBuf to ucRecBuf, which is 8-bits allocated compared to 16-bits allocated in the former one
                g_strRecBuf.ucRecBuf[ulLoopIndex] = (u8)(g_strRecBuf.usOutBuf[g_strRecBuf.usStart * sizeof(u16)
                                                                                 + ulLoopIndex * sizeof(u16)] & 0xFF);
            }

            memcpy(pucRecData, (void *)(&(g_strRecBuf.ucRecBuf[0])), ulLength * (sizeof(u8)));
        }
        else
        {
            /* 将buffer中保存的数据返回给上层*/
            memcpy(pucRecData, (void *)(&(g_strRecBuf.ucRecBuf[g_strRecBuf.usStart])), ulLength);
        }

        g_strRecBuf.usStart += (u16)ulLength;

        g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_PROCESS;
        g_u32ReadFlag = SCI_DEF_READ_FORBID;

        intUnlock(sciLockKey);

		apBIT_SET((pstrState->pBase->RegIntMask), PL131_REGINTMASK_CHTOUT, 0);

        
        SCI_RECORD_EVENT(SCI_EVENT_API_DATA_REC_DONE, ulLength, __LINE__);

        SCI_RECORD_DATA(SCI_DATA_RCV_EVENT, (BSP_U8)ulLength, pucRecData);
        
        return BSP_OK;
    }
    /* 否则等待 */
    else
    {
        u32 loop = 135;             /* 从255个字节最慢速计算出来 */
        u32 ulDataLenPre = 0;
        u32 ulDataLenCur = 0;

        /* 打开中断，开始接收数据 */
        g_strRecBuf.usPrelen = (u16)ulLength;
        do
        {
            /* 解锁，接收数据 */
             /* coverity[lock_acquire] */
            intUnlock(sciLockKey);
            /* coverity[sleep] */
            (void)semTake(g_SCIRecvSyncSem, 200);

            /* 接收数据之后再次加锁，保护全局变量 */
            /* coverity[lock_acquire] */
            sciLockKey = intLock();

            recDataNum = sci_count_length();
            if (recDataNum >= (u16)ulLength)
            {
                g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_SATISFIED;
            }

            /* 检查数据是否已经收齐 */
            if (SCI_RX_DATA_REC_SATISFIED == g_sciRxStGblStru.RxDataRecFlag)
            {
                /* 将buffer中保存的数据返回给上层*/
                if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
                {
                    (void)cacheInvalidate(DATA_CACHE, (u8 *)g_strRecBuf.usOutBuf, SCI_MAX_DATA_BUF_LEN * sizeof(u32));
                    for (ulLoopIndex = 0; ulLoopIndex < ulLength; ulLoopIndex++)
                    {
                        g_strRecBuf.ucRecBuf[ulLoopIndex] = (u8)(g_strRecBuf.usOutBuf[g_strRecBuf.usStart
                                                                                         * sizeof(u16)
                                                                                         + ulLoopIndex
                                                                                         * sizeof(u16)] & 0xFF);
                    }

                    memcpy(pucRecData, (void *)(&(g_strRecBuf.ucRecBuf[0])), ulLength * (sizeof(u8)));
                }
                else
                {
                    memcpy(pucRecData, (void *)(&(g_strRecBuf.ucRecBuf[g_strRecBuf.usStart])), ulLength);
                }

                g_strRecBuf.usStart += (u16)ulLength;

                g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_PROCESS;
                g_u32ReadFlag = SCI_DEF_READ_FORBID;

                intUnlock(sciLockKey);

				apBIT_SET((pstrState->pBase->RegIntMask), PL131_REGINTMASK_CHTOUT, 0);

                SCI_RECORD_DATA(SCI_DATA_RCV_EVENT, (BSP_U8)ulLength, pucRecData);
                SCI_RECORD_EVENT(SCI_EVENT_API_DATA_REC_DONE, ulLength, __LINE__);
                
                /* coverity[missing_unlock] */
                return BSP_OK;
            }
            else
            {
                ulDataLenCur = g_strRecBuf.usLength;

                /* 如果2s内数据量没有增加，认为超时 */
                if (ulDataLenCur == ulDataLenPre)
                {
                    g_strRecBuf.usPrelen = 0;
                    g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_OVER;

                    intUnlock(sciLockKey);

					apBIT_SET((pstrState->pBase->RegIntMask), PL131_REGINTMASK_CHTOUT, 0);

                    sci_print_error("no data len=%d cur_len=%d, card id is 0x%d\n", ulLength, g_strRecBuf.usLength,g_stSciHwCfg.card_id,0,0,0);

                    SCI_RECORD_EVENT(SCI_EVENT_ERROR_NODATA, pstrState->eCurrentState, __LINE__);

                    SCI_RECORD_EXCGBL(SCI_EVENT_ERROR_NODATA, pstrState->eCurrentState, omTimerGet());
        
                    /* save regiter */
                    if(BSP_NULL != g_sciRegGblAddr)
                    {
                        memcpy((void *)g_sciRegGblAddr,(void *)SCI_BASE_ADDR,(int)SCI_RECORD_REG_SIZE);
                    }
                    
                    /* coverity[missing_unlock] */
                    return BSP_ERR_SCI_NODATA;
                }

                /* 否则继续接收数据 */
                ulDataLenPre = ulDataLenCur;
            }
        } while (loop--);

        /* 循环完成后，还是没有收完数据 */
        g_strRecBuf.usPrelen = 0;
        g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_OVER;
        intUnlock(sciLockKey);

		apBIT_SET((pstrState->pBase->RegIntMask), PL131_REGINTMASK_CHTOUT, 0);

        if(BSP_NULL != g_sciRegGblAddr)
        {
            memcpy((void *)g_sciRegGblAddr,(void *)SCI_BASE_ADDR,(int)SCI_RECORD_REG_SIZE);
        }

        SCI_RECORD_EVENT(SCI_EVENT_ERROR_NODATA, pstrState->eCurrentState, __LINE__);
        
        return BSP_ERR_SCI_NODATA;
    }
}


/******************************************************************
* Function:  sciAllDataRead
*
* Description:
*      Read all data in the driver's buffer.
*      This function wil be called when the PS software overtime while waiting
*      for driver's callback
*
* Calls:
*      memcpy
*
* Called By:  PS Software
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
* pucRecData - The data buffer to be copied to.
* ulLength - number of bytes/characters received in driver's buffer
*
* Outputs:
* none
*
* Return Value:
*  OK - transmission commenced successfully
*  DR_ERR_SCI_PARAERROR - invalid mode specified
*
******************************************************************/
s32 bsp_sci_all_data_read(u32 *pulLength, u8 *pucRecData)
{
    s32 sciLockKey = 0;
    u32 ulLength = 0;
    u32 ulLoopIndex = 0;

	if ((NULL == pulLength) || (NULL == pucRecData))
    {
        sci_print_error("(NULL == u32DataLength) || (NULL == pu8DataBuffer)!");
        return BSP_ERR_SCI_INVALIDPARA;
    }

    if (g_u32ResetSuccess != SCI_DEF_RESET_SUCCESS)
    {
        sci_print_error("bsp_sci_all_data_read: not initialized\n");
        return BSP_ERR_SCI_NOTINIT;
    }

    sciLockKey = intLock();
    ulLength   = sci_count_length();
    *pulLength = ulLength;

    /* 将buffer中所有数据全部返回给上层，buffer清空*/
    if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
    {
        (void)cacheInvalidate(DATA_CACHE, (u8 *)g_strRecBuf.usOutBuf, SCI_MAX_DATA_BUF_LEN * sizeof(u32));
        for (ulLoopIndex = 0; ulLoopIndex < ulLength; ulLoopIndex++)
        {
            g_strRecBuf.ucRecBuf[ulLoopIndex] = (u8)(g_strRecBuf.usOutBuf[g_strRecBuf.usStart * sizeof(u16)
                                                                             + ulLoopIndex * sizeof(u16)] & 0xFF);
        }

        memcpy(pucRecData, (void *)(&(g_strRecBuf.ucRecBuf[0])), ulLength * (sizeof(u8)));
    }
    else
    {
        /* 将buffer中保存的数据返回给上层*/
        memcpy(pucRecData, (void *)(&(g_strRecBuf.ucRecBuf[g_strRecBuf.usStart])), (u32)ulLength);
    }

    g_strRecBuf.usStart += (u16)ulLength;

    intUnlock(sciLockKey);

    return OK;
}

/******************************************************************
* Function:  sciATRGet
*
* Description:
*      Get the ATR data in the driver's buffer.
*      This function will be called when the PS software want to parse the ATR.
*
* Calls:
*      memcpy
*
* Called By:  PS Software
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
* pucATRData - The data buffer to be copied to.
* ulLength - number of ATR bytes/characters received in driver's buffer
*
* Outputs:
* none
*
* Return Value:
*  OK - transmission commenced successfully
*  ERROR -failed
*
*
******************************************************************/
s32 bsp_sci_atr_get(u8 * pulLen, u8 *pucATRData,SCI_ATRInfo* stSCIATRInfo)
{
    SCI_RECORD_EVENT(SCI_EVENT_API_GET_ATR, g_u32ResetSuccess, __LINE__);

    if((NULL == pulLen) || (NULL == pucATRData))
    {
        sci_print_error("(NULL == u32DataLength) || (NULL == pu8ATR)!");
        return BSP_ERR_SCI_INVALIDPARA;
    }


    if ((TRUE == g_strSciState.sATRParams.HasChecksum) && (0 != g_strATRData.ulATRLen))
    {
        *pulLen = g_strATRData.ulATRLen - 1;
    }
    else
    {
        *pulLen = g_strATRData.ulATRLen;
    }


    /* 将ATR数据返回给上层平台*/
    memcpy(pucATRData, (void *)(g_strATRData.ATRData), ((g_strATRData.ulATRLen) * (sizeof(u8))));

    return OK;
}
/******************************************************************
* Function:  sciRecSyncTask
*
* Description:
*       Demaen Task
*
* Calls:
*      None
*
* Called By:  None
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs:
*  None
*
* Outputs:
*  None
*
* Return Value:
*  None
*
******************************************************************/
void sci_rec_sync_task(void)
{
    SCI_STATE_STRU * const pstrState = &g_strSciState;
    volatile u32 ulPPSDelayIndex = 0;
    int retVal = -1;

    /* coverity[loop_top]*/
    for (;;)
    {
        (void)semTake(g_SCISynSem, WAIT_FOREVER);

        /* Do SIM card deactivation operation */
        if (g_SciDeactivateFlag)
        {
			/* buffer overflow Begin: */
            sci_set_voltage(SCI_VOLTAGE_SWITCH_CLASS_CB2ZERO);

            pwrctrl_sci_soft_clk_dis();
			pwrctrl_sci_low_power_enter();

            if(g_strRecBuf.usLength >= SCI_BUF_MAX_LEN) 
            {
                /*Set Other Global Flags for Safe*/
                nActiveT = 10;
                g_sci_voltage_state.sciClassKind = PL131_SCI_Class_B;
                g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG = SCI_CLASS_SWITCH_NONE;

                sci_buf_clear(); 

        		SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

                pstrState->eCurrentState = PL131_STATE_INACTIVECARD;

                g_u32ResetSuccess = SCI_DEF_NOT_RESET;


            }else if(PL131_STATE_NOCARD != pstrState->eCurrentState)
            {
                semGive(g_SCIDeactiveSyncSem);
            }
			/* buffer overflow End! */
            
            g_SciDeactivateFlag = FALSE;

             continue;
        }

        /*do Vltg switch process*/
        if (g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG == SCI_CLASS_SWITCH_PROCESS)
        {
            switch(g_sci_voltage_state.sci_vlotage_switch_sense)
            {
                case SCI_VOLTAGE_SWITCH_SENSE_CLASS_C2B_IMMEDIATELY:
                    {
                        retVal = sci_set_voltage(SCI_VOLTAGE_SWITCH_CLASS_C2B);
                        nActiveT = 0;
                        break;
                    }
                case SCI_VOLTAGE_SWITCH_SENSE_CLASS_UNKNOW_TRY:
                     {
                        if(g_sci_voltage_state.sciClassKind == PL131_SCI_Class_B)
                        {
                            if(nActiveT <= SCI_MAX_USIM_CARD_DETECT_RETRY_CNT)
                            {
                                retVal = sci_set_voltage(SCI_VOLTAGE_SWITCH_CLASS_B2B);
                            }
                        }
                        if(g_sci_voltage_state.sciClassKind == PL131_SCI_Class_C)
                        {
                            if(nActiveT > SCI_USIM_CLASS_C_DETECT_RETRY_CNT)
                            {
                                retVal = sci_set_voltage(SCI_VOLTAGE_SWITCH_CLASS_C2B);
                            }
                            else
                            {
                                retVal = sci_set_voltage(SCI_VOLTAGE_SWITCH_CLASS_C2C);
                            }
                        }
                    
                        break;
                    }
                 default:
                    g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG = SCI_CLASS_SWITCH_NONE;
                    retVal = BSP_ERROR;
                    break;
                    
            }
            g_sci_voltage_state.sci_vlotage_switch_sense = SCI_VOLTAGE_SWITCH_SENSE_CLASS_BUTT;

            if(retVal == BSP_OK)
            {
                g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG = SCI_CLASS_SWITCH_NONE;
                sci_print_info("RecSyncTask & SCI_CLASS_SWITCH_PROCESS: Start to set Vltg as %ld and do UICC activate\n",
                  (int)g_sci_voltage_state.sciClassKind, 0, 0, 0, 0, 0);

                pstrState->eCurrentState = PL131_STATE_INACTIVECARD;
                SCI_RECORD_EVENT(SCI_EVENT_CARD_ACTIVE_START, pstrState->eCurrentState, __LINE__);
                retVal = appl131_card_active_set(pstrState, TRUE, pstrState->sATRParams.pATRBuffer);
                if(retVal == 0)
                {
                    SCI_RECORD_EVENT(SCI_EVENT_CARD_ACTIVE_SUCCESS, pstrState->eCurrentState, __LINE__);
                }
            }
        }
        /*Changed for SIM Card compatibility. Begin*/
        /*Launch PPS Request*/
        else if ((g_Pl131_pps_state.PL131_PPS_ACTIVATION) && (PPS_RESPONE_READY == g_Pl131_pps_state.PL131_PPS_Response))
        {
            /*Add needed delay*/
            for (ulPPSDelayIndex = 0; ulPPSDelayIndex < PPS_REQUEST_DELAY; ulPPSDelayIndex++)
            {
                /*Delay enough time to ensure SIM Card had been ready to recieve data*/
            }

            sci_print_info("Launch PPS Request Process in SCI Task!\n");

            /*Call PPS Request Function*/
            pl131_pps_request(pstrState);

            SCI_RECORD_EVENT(SCI_EVENT_PPS_REQ, pstrState->eCurrentState, __LINE__);
        }
        /*use cold reset to activate UICC after 10ms delay*/
        else
        {
            taskDelay(1);
            sci_print_info("Not do class switch, but do powerup\n");

            SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

            pstrState->eCurrentState = PL131_STATE_INACTIVECARD;
            SCI_RECORD_EVENT(SCI_EVENT_CARD_ACTIVE_START, pstrState->eCurrentState, __LINE__);
            retVal = appl131_card_active_set(pstrState, TRUE, pstrState->sATRParams.pATRBuffer);
            if(retVal == 0)
            {
                SCI_RECORD_EVENT(SCI_EVENT_CARD_ACTIVE_SUCCESS, pstrState->eCurrentState, __LINE__);
            }
        }
    }
}

/******************************************************************
* Function:  sciIntHandler
*
* Description:
*     intHandle
*
* Calls:
*      memcpy
*
*
* Table Accessed: NONE
*
* Table Updated: NONE
*
* Inputs: NONE
*
* Outputs:NONE
*
* Return Value: NONE
*
*
******************************************************************/
void sci_int_handler(void)
{
    /*Create pointers to the instance data block and the register base*/
    SCI_STATE_STRU * const pstrState = &g_strSciState;
    SCI_REG * const pstrBase = pstrState->pBase;
    u32 IntFlags = 0;               /*the interrupts currently invoked*/
    u32 BytesInFIFO = 0;             /*number of bytes in FIFO*/
    s32 IntID = 0;                  /*the interrupt number*/
    PL131_eStateOptions eInitialState = PL131_STATE_UNINIT;
    u32 FIFOData   = 0;                /*Data in SCI Rx/Tx FIFO, R/W through Data Reg*/
    u32 u32LockKey = 0;
    int retVal = 0;
    //EX_REPORT_SCI_S stReportSci;

    //PROCESS_ENTER(BSP_SCI_TRACE);
	intDisable(g_stSciHwCfg.int_number);

    /*read and clear the interrupt flags*/
    IntFlags = pstrBase->RegIntStatus;
	writel(IntFlags,SCI_REG_INTCLEAR);

    eInitialState = pstrState->eCurrentState;

    /* coverity[overflow_const]*/
    /* coverity[assignment] */
    IntID = 0xFFFFFFFF;

    while (IntFlags)
    {
        /* coverity[incr] */
        IntID++;

        if (IntFlags & 1)
        {
            switch (IntID)
            {
                /*------------------------CARD INSERTION-----------------------*/
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_CARDIN:
            {
                /*sim card 没有这个中断*/
                break;
            }

                /*-----------------------CARD REMOVAL------------------------*/
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_CARDOUT:
            {
                /*NOTIFICATION - card has been removed*/
                /*sim card 没有这个中断*/
                break;
            }

                /*-----------------------CARD ACTIVATION-------------------------------*/
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_CARDUP:
            {
                /* record debug info */
                SCI_RECORD_EVENT(SCI_EVENT_INTR_CARD_UP, pstrState->eCurrentState, __LINE__);

                (void)appl131_clean(pstrState);                      /*terminate transfer*/
                /*NOTIFICATION - card has been activated*/

                pstrState->eCurrentState = PL131_STATE_WAITATR;
                pstrState->sATRParams.Grouping = PL131_ATR_GROUPSTART;               /*ready for ATR*/
                pstrState->sATRParams.BufferAt = 0;
                sci_print_info("\r\n bsPL131_REGINTMASK_CARDUP");

                /*reset pointer for ATR*/
                break;
            }

                /*-----------------------CARD DEACTIVATION---------------------------*/
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_CARDDOWN:
            {

                (void)appl131_clean(pstrState);               /*terminate transfer*/
                (void)pl131_registers_init(pstrState);         /*restore state*/

                SCI_RECORD_EVENT(SCI_EVENT_INTR_CARD_DOWN, pstrState->eCurrentState, __LINE__);

                pstrState->eCurrentState = PL131_STATE_INACTIVECARD;

                /*Do SIM card deactivation operation */
                if (g_SciDeactivateFlag)
                {
                    /* 通知上层平台sim卡已经下电*/
                    if (NULL != g_FuncCb)
                    {
                        (void)g_FuncCb((u32)SCI_EVENT_CARD_DEACTIVE_SUCCESS, NULL);
                    }

                    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

                    semGive(g_SCISynSem);
                    break;
                }

                if (g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG == SCI_CLASS_SWITCH_PROCESS)
                {
                    semGive(g_SCISynSem);

                    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);
                }
                /*Changed for Card Status Type Error, Change Begin*/
                else
                {
                    pstrState->eCurrentState = PL131_STATE_NOCARD;

                    /*Do SIM card deactivation operation*/
                    g_SciDeactivateFlag = TRUE;
                    nActiveT = 10;
                    g_sci_voltage_state.sciClassKind = PL131_SCI_Class_B;
                    g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG = SCI_CLASS_SWITCH_NONE;
                    semGive(g_SCISynSem);

                    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

                    /*Do SIM card deactivation operation*/
                }

                /* Changed for Card Status Type Error, Change End*/
                sci_print_info("apPL131_CARD DEACTIVATION\n");

                g_strRecBuf.usPrelen = 0;
                g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_OVER;

                break;
            }

                /*-----------------------ERRORS AND TIMEOUTS-------------------------*/
                /*If clock stop mode supported the following timeouts can occur*/
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_CLKSTPI:
            {
                apBIT_SET(pstrBase->RegIntMask, PL131_REGINTMASK_CLKSTPI, 0);
                semGive(g_SCIClockStopSem);
                
                SCI_RECORD_EVENT(SCI_EVENT_INTR_CLK_STOP, pstrState->eCurrentState, __LINE__);

                /*NOTIFICATION - clock has been stopped*/

                sci_print_info("\r\n bsPL131_REGINTMASK_CLKSTPI");

                break;
            }
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_CLKACTI:
            {
                apBIT_SET(pstrBase->RegIntMask, PL131_REGINTMASK_CLKACTI, 0);

                /*Set Current SCI CLK Status Flag*/
                semGive(g_SCIClockStartSem);
                SCI_RECORD_EVENT(SCI_EVENT_INTR_CLK_ACTIVE, pstrState->eCurrentState, __LINE__);
                /*NOTIFICATION - clock has been started*/

                sci_print_info("\r\n bsPL131_REGINTMASK_CLKACTI");
                break;
            }

                /*for an ATR start bit timeout*/
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_ATRSTOUT:
            {
                SCI_RECORD_EVENT(SCI_EVENT_INTR_ATRSTOUT, pstrState->eCurrentState, __LINE__);

				SCI_RECORD_EXCGBL(SCI_EVENT_INTR_ATRSTOUT, pstrState->eCurrentState, omTimerGet());

                SCI_RECORD_EVENT(SCI_EVENT_REGISTER_FINISH, pstrState->eCurrentState, __LINE__);

                if (g_sci_voltage_state.sciClassKind == PL131_SCI_Class_C)
                {
                    sci_print_info("bsPL131_REGINTMASK_ATRSTOUT: We will move in class switch right now\n");
                    g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG = SCI_CLASS_SWITCH_PROCESS;
                    pstrState->eCurrentState = PL131_STATE_INACTIVECARD;
                    sci_print_info("bsPL131_REGINTMASK_ATRSTOUT: Do Deactivation Process No1, cause bsPL131_REGINTMASK_ATRSTOUT\n");
                    g_sci_voltage_state.sci_vlotage_switch_sense = SCI_VOLTAGE_SWITCH_SENSE_CLASS_C2B_IMMEDIATELY;
                    SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_SWITCH, pstrState->eCurrentState, __LINE__);

                }
                else
                {
                    /*for future use, for example to report UICC not exist*/
                    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);
                }

                apBIT_SET(pstrBase->RegCtrl2, PL131_REGCTRL2_FINISH, 1);

                break;
            }
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_CHTOUT:
			case bsPL131_REGINTMASK_BLKTOUT:
            {
                BSP_U16 event;

                switch (IntID)
                {
                case bsPL131_REGINTMASK_BLKTOUT:
                    event = SCI_EVENT_INTR_BLKOUT;
                    break;
                /* coverity[equality_cond] */
                case bsPL131_REGINTMASK_CHTOUT:
                default:
                    event = SCI_EVENT_INTR_CHOUT;
                    break;
                }

                SCI_RECORD_EVENT(event, pstrState->eCurrentState, __LINE__);

                /* record debug info */
                sci_print_info("bsPL131_REGINTMASK_CHTOUT\r\n");

                if ((PL131_STATE_READATR == pstrState->eCurrentState))
                {
                    g_strRecBuf.usPrelen = 0;

                    sci_print_info("CHTOUT & SCI_CLASS_SWITCH_PROCESS: Do apPL131_CARDDEACT cause WWT excess in ATR process! RegChTimeLS = %l#x\r\n",
                              (int)pstrBase->RegChTimeLS, 0, 0, 0, 0, 0);
                    
                    pstrState->eCurrentState = PL131_STATE_INACTIVECARD;
                    SCI_RECORD_EVENT(SCI_EVENT_REGISTER_FINISH, pstrState->eCurrentState, __LINE__);
                    SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_SWITCH, pstrState->eCurrentState, __LINE__);

                    sci_class_switch_try();

                     
      
                }
                else if (g_strRecBuf.usPrelen != 0)
                {
					BytesInFIFO = readl(SCI_REG_RXCOUNT);

                    /*Fix SCI FIFO overrun and characeter time out bug, Begin*/
                    sci_print_info("CHTOUT Interrupt, g_strRecBuf.usPrelen = %ld, g_strRecBuf.usStart = %ld, g_strRecBuf.usLength = %ld, BytesInFIFO = %l#x\n\n",
                              (int)g_strRecBuf.usPrelen, (int)g_strRecBuf.usStart,
                              (int)g_strRecBuf.usLength, (int)BytesInFIFO, 0, 0);

                    /*Fix SCI FIFO overrun and characeter time out bug, End*/

                    if (g_ulSciGcfStubFlag)
                    {
                        g_ulChTOUTDeactEn = 1;
                    }

                    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);


                    if (g_ulChTOUTDeactEn)
                    {

                        sci_print_info("Do apPL131_CARDDEACT cause WWT excess when PS is communication with SIM!\n", 0, 0, 0,
                                  0, 0,
                                  0);
                        pstrState->eCurrentState = PL131_STATE_INACTIVECARD;
                        apBIT_SET(pstrBase->RegCtrl2, PL131_REGCTRL2_FINISH, 1);
                        SCI_RECORD_EVENT(SCI_EVENT_REGISTER_FINISH, pstrState->eCurrentState, __LINE__);

                    }
                }
                else if ((g_Pl131_pps_state.PL131_PPS_ACTIVATION) && (PL131_STATE_READATR == pstrState->eCurrentState))
                {
                    sci_print_info("PPS timeout in CHTOUT Interrupt, we set warm reset here, this is the No.%d reset process\n",
                              g_Pl131_pps_state.PL131_PPS_ACTIVATION, 0, 0, 0, 0, 0);

                    SCI_RECORD_EVENT(SCI_EVENT_CARD_ACTIVE_START, pstrState->eCurrentState, __LINE__);

                    retVal = appl131_card_active_set(pstrState, TRUE, pstrState->sATRParams.pATRBuffer);
                    if(retVal == 0)
                    {
                        SCI_RECORD_EVENT(SCI_EVENT_CARD_ACTIVE_SUCCESS, pstrState->eCurrentState, __LINE__);
                    }
      
                }

                break;
            }
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_RORI:
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_TXERR:
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_ATRDTOUT:
            {
                BSP_U16 event;

                /* coverity[dead_error_condition] */
                switch (IntID)
                {
                case bsPL131_REGINTMASK_RORI:
                    event = SCI_EVENT_INTR_RORI;
                    break;
                case bsPL131_REGINTMASK_TXERR:
                    event = SCI_EVENT_INTR_TX_ERR;
                    break;
                case bsPL131_REGINTMASK_ATRDTOUT:
                    event = SCI_EVENT_INTR_ATRDTOUT;
                    break;
                case bsPL131_REGINTMASK_BLKTOUT:
                /* coverity[dead_error_begin] */
                default:
                    event = SCI_EVENT_INTR_BLKOUT;
                    break;
                }

                SCI_RECORD_EVENT(event, pstrState->eCurrentState, __LINE__);

                
                sci_print_info("\r\n bsPL131_REGINTMASK_RORI TXERR ATRDTOUT BLKTOUT.IntID = %d", IntID, 0, 0, 0, 0, 0);
                switch (eInitialState)
                {
                case PL131_STATE_WAITATR:                             /*in ATR mode*/
                case PL131_STATE_READATR:                             /*in ATR mode*/
                {
                    SCI_RECORD_EVENT(SCI_EVENT_REGISTER_FINISH, pstrState->eCurrentState, __LINE__);
                    SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_SWITCH, pstrState->eCurrentState, __LINE__);

                    sci_class_switch_try();
                    break;
                }

                    /*abort a transmission, if any*/
                case PL131_STATE_TX:
                {
                    /*NOTIFICATION - transmission error*/
                    apBIT_SET(pstrBase->RegIntMask, PL131_REGINTMASK_TXTIDE, 0);                   /*disable TX interrupt*/

                    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);
 
                    break;
                }

                    /*abort a receiption, if any*/
                case PL131_STATE_READY:
                case PL131_STATE_RX:
                {
                    g_strRecBuf.usPrelen = 0;
                    g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_OVER;
                    break;
                }
                case PL131_STATE_UNINIT:
                case PL131_STATE_FIRSTINIT:
                case PL131_STATE_DISABLED:
                case PL131_STATE_NOCARD:
                case PL131_STATE_INACTIVECARD:
                default:
                {
                    break;
                }
                }

                IntFlags = 0;                   /*no other interrupts allowed*/
                (void)appl131_clean(pstrState);             /*terminate transfer*/

                /* 通知上层平台发送或者接收发生错误*/
                if (NULL != g_FuncCb)
                {
                    (void)g_FuncCb((u32)SCI_EVENT_CARD_TX_ERR, NULL);

                    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);
                }

                /* 异常状态上报给平台*/
                break;
            }

                /*-------------------------RECEIVE DATA------------------------*/
                /* Modification: Optimize SCI interrupt process*/
            /* coverity[equality_cond] */
            case bsPL131_REGINTMASK_RTOUT:
                
            case bsPL131_REGINTMASK_RXTIDE:
            {
                if(IntID == bsPL131_REGINTMASK_RTOUT)
                {
                    SCI_RECORD_EVENT(SCI_EVENT_INTR_RTOUT, pstrState->eCurrentState, __LINE__);
                }
                /*an inactive card MUST be performing ATR*/
                /*a character moves us into ATR start*/
                if (eInitialState <= PL131_STATE_WAITATR)
                {
                    pstrState->eCurrentState = PL131_STATE_READATR;
                    eInitialState = PL131_STATE_READATR;

                    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);
                }

                //process ATR
                if (eInitialState == PL131_STATE_READATR)
                {  
                    pl131_atr_process(pstrState, pstrBase->RegData);
                    
                }
                else
                {
                    if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
                    {
                        sci_print_error("IntHandler:RXTIDE or RTOUT, transient sci rx interrupt happenned in SCI DMA Rx.\r\n\n",
                                  0,0, 0, 0, 0, 0);
                    }
                    else
                    {
						BytesInFIFO = readl(SCI_REG_RXCOUNT);

                        //receive as many bytes as are available
                        while (BytesInFIFO)
                        {
                            FIFOData = pstrBase->RegData;
                            FIFOData = BIT_GET(FIFOData, PL131_REGDATA_DATA);
                            (void)sci_buf_add((u8*)&FIFOData);
                            BytesInFIFO--;
                        }

                        pstrState->eCurrentState = PL131_STATE_READY;
                    }
                }

                break;
            }

                /*-------------------------TRANSMIT DATA-----------------------------*/
            case bsPL131_REGINTMASK_TXTIDE:
            {
				//modified by yangzhi 20130218 Begin:
    			u32LockKey = (u32)intLock();
				//modified by yangzhi 20130218 End!

                if (eInitialState == PL131_STATE_TX)
                {
                    BytesInFIFO = BIT_GET(pstrBase->RegTxCount, PL131_REGTXCOUNT_DATA);

                    /*transmit as many bytes as there is space*/
                    while ((BytesInFIFO < PL131_FIFO_SIZE_TX) && pstrState->DataLength)
                    {
                        pstrBase->RegData = (u32)(*pstrState->pDataBuffer);
                        pstrState->DataLength--;
                        pstrState->pDataBuffer++;
                        BytesInFIFO++;                            /*track the bytes in the FIFO*/
                    }
                }

                /*check for termination*/
                if ((!pstrState->DataLength) || (eInitialState != PL131_STATE_TX))
                {
                    /*reset to normal state*/
                    pstrState->pDataBuffer = NULL;
                    pstrState->DataLength = 0;
                    pstrState->eCurrentState = PL131_STATE_READY;
                    BIT_SET(pstrBase->RegIntMask, PL131_REGINTMASK_TXTIDE, 0);       /*disable TX interrupt*/
                    BIT_SET(pstrBase->RegTide, PL131_REGTIDE_TX, 0);
                    BIT_SET(pstrBase->RegCtrl1, PL131_REGCTRL1_MODE, apPL131_RX_FROM_CARD);
                    if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
                    {
                        //Set SCI Rx Data Receive Flag is Start
                        g_sciRxStGblStru.RxDataRecFlag = SCI_RX_DATA_REC_START;

                        //Enable SCI DMA Rx
                        appl131_dma_mode_set(&g_strSciState, apPL131_DMA_RX);

                        /*Mask SCI Rx Int*/
                        BIT_SET(pstrBase->RegIntMask, PL131_REGINTMASK_RXTIDE, 0);
                    }
                }

				//modified by yangzhi 20130218 Begin:
    			intUnlock((int)u32LockKey);
				//modified by yangzhi 20130218 End!

                break;
            }

                /*------------------------------------------------------------------*/
            default:
            {
                break;
            }
            } /*end of switch*/
            if (eInitialState != pstrState->eCurrentState)
            {
            }
        }

        IntFlags >>= 1;   /*shift to next bit*/
    } /*end of while*/
	
	
	intEnable(g_stSciHwCfg.int_number);
    //PROCESS_EXIT(BSP_SCI_TRACE);
    return;
}

/******************************************************************
*  Function:  sciDeactive
*
*  Description:
*      deactive sim card
*
*  Calls:
*      None
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         None
*
*  Outputs:
*         None
*
* Return Value:
*   OK
*
******************************************************************/
u32 bsp_sci_deactive( void )
{
    SCI_STATE_STRU * const pstrState = &g_strSciState;
    SCI_REG * const pstrBase = pstrState->pBase;

    SCI_RECORD_EVENT(SCI_EVENT_API_DECATIVE, pstrState->eCurrentState, __LINE__);
    
    if ((pstrState->eCurrentState >= PL131_STATE_RX) && (pstrState->eCurrentState <= PL131_STATE_TX))
    {
        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        sci_print_error("not initialized\n");

        return BSP_ERR_SCI_NOTREADY;
    }
    else if (pstrState->eCurrentState >= PL131_STATE_WAITATR)
    {
        /* unvote and enable clk */
        pwrctrl_sci_low_power_exit();
		pwrctrl_sci_soft_clk_en();

        /*Set SCI Deactivate Flag*/
        g_SciDeactivateFlag = TRUE;

        /*Set Other Global Flags for Safe*/
        nActiveT = 10;
        g_sci_voltage_state.sciClassKind = PL131_SCI_Class_B;
        g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG = SCI_CLASS_SWITCH_NONE;

        sci_buf_clear(); //added by yangzhi in 2010-12-2


		SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        if (SCI_RX_DMA_MODE == g_sciRxStGblStru.RxModeFlag)
        {
            SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, g_sciRxStGblStru.RxModeFlag, __LINE__);

			if(bsp_edma_channel_stop(g_sciRxStGblStru.DmaChnNo) <0)
			{
				sci_print_error("dma transfer stop failede\n");
		      	return BSP_ERR_SCI_DISABLED;
			}
        }

        SCI_RECORD_EVENT(SCI_EVENT_REGISTER_FINISH, pstrState->eCurrentState, __LINE__);

        /*Deactivate SIM Card*/
        pstrState->eCurrentState = PL131_STATE_INACTIVECARD;
        sci_print_info("Deactivate SIM card. g_SciDeactivateFlag = %ld, nActiveT = %ld, sciClassKind = %ld, ucSCI_CLASS_SWITCH_FLAG = %ld.\r\n\n",
                  (int)g_SciDeactivateFlag, (int)nActiveT, (int)g_sci_voltage_state.sciClassKind, (int)g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG, 0, 0);

        apBIT_SET(pstrBase->RegCtrl2, PL131_REGCTRL2_FINISH, 1);

        g_strATRData.ulATRLen = 0;
        
        g_u32ResetSuccess = SCI_DEF_NOT_RESET;

		(BSP_VOID)semTake(g_SCIDeactiveSyncSem, DEACTIVE_SYNC_DELAY);
        
#ifndef BSP_CONFIG_HI3630
        if(SIM_CARD_OUT == g_sci_detect_state)
        {
            if(BSP_OK !=bsp_sim_upres_disable(g_stSciHwCfg.card_id))
            {
                sci_print_error("Card not activation, not need to do deactivation.\r\n\n");
            }
        }
#endif
    } 	/* buffer overflow Begin: */
    else if(pstrState->eCurrentState != PL131_STATE_INACTIVECARD) 
    {
        sci_print_error("Card not activation, not need to do deactivation, satus is 0x%d\r\n\n",pstrState->eCurrentState);
		/* buffer overflow End! */
        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        return OK;
    }

    return OK;
}

/******************************************************************
*  Function:  sciPSClassSwitch
*
*  Description:
*      Do SCI class switch if current voltage is not the highest
*      voltage class.
*
*  Calls:
*      None
*
*  Called By:  PS software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         None
*
*  Outputs:
*         None
*
* Return Value:
*   OK    - Current Voltage is not highest one, do voltage class
*           switch.
*   ERROR - failed
*   1     - Current Voltage is the highest one, not do voltage
*           class switch.
*
******************************************************************/
s32 bsp_sci_vltg_class_switch(void)
{
    /*Create pointers to the instance data block and the register base*/
    SCI_STATE_STRU * const pstrState = &g_strSciState;
    SCI_REG * const pstrBase = pstrState->pBase;
    s32 lErroRet = OK;

    SCI_RECORD_EVENT(SCI_EVENT_API_CLASS_SWITCH, pstrState->eCurrentState, __LINE__);

    if (g_u32ResetSuccess != SCI_DEF_RESET_SUCCESS)
    {
        sci_print_error("not initialized\n");
        return BSP_ERR_SCI_NOTINIT;
    }

    /*busy 状态不支持切换电压*/
    if ((pstrState->eCurrentState >= PL131_STATE_RX) && (pstrState->eCurrentState <= PL131_STATE_TX))
    {
        sci_print_error("eCurrentState busy\n");
        return BSP_ERR_SCI_NOTREADY;
    }

    /*Process according to current voltage is the highest one or not*/
    switch (g_sci_voltage_state.sciClassKind)
    {
        /*current voltage is not the highest one*/
	    case (PL131_SCI_Class_C):
	    {
	        if (0x04 == (pstrState->sATRParams.ClassInd & 0x3F))
	        {
	            sci_print_error("don't support CLASS_B\n");
	            lErroRet = BSP_ERR_SCI_VLTG_HIGHEST;
	            break;
	        }


	        /*Do class switch if current SCI driver state is ready/rx/tx*/
	        if (PL131_STATE_READY <= pstrState->eCurrentState)
	        {
	            pwrctrl_sci_low_power_exit();
                SCI_RECORD_EVENT(SCI_EVENT_LOW_POWER_EXIT, pstrState->eCurrentState, __LINE__);
	        	pwrctrl_sci_soft_clk_en();

	            appl131_dma_mode_set(&g_strSciState, apPL131_DMA_OFF);     //added by yangzhi for dma test
	            g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG  = SCI_CLASS_SWITCH_PROCESS;
	            pstrState->eCurrentState = PL131_STATE_INACTIVECARD;

                SCI_RECORD_EVENT(SCI_EVENT_REGISTER_FINISH, pstrState->eCurrentState, __LINE__);
                g_sci_voltage_state.sci_vlotage_switch_sense = SCI_VOLTAGE_SWITCH_SENSE_CLASS_C2B_IMMEDIATELY;
                
                SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_SWITCH, pstrState->eCurrentState, __LINE__);
  
	            apBIT_SET(pstrBase->RegCtrl2, PL131_REGCTRL2_FINISH, 1);
                
                
	            lErroRet = OK;
	        }
	        else
	        {
	            sci_print_error("The current vltg is PL131_SCI_Class_C, but pstrState->eCurrentState = %ld\n\n",
	                      (int)pstrState->eCurrentState, 0, 0, 0, 0, 0);
	            lErroRet = BSP_ERR_SCI_CURRENT_STATE_ERR;
	        }

	        break;
	    }

        /*current voltage is the highest one*/
	    case (PL131_SCI_Class_B):
	    {
	        sci_print_info("sciPSClassSwitch & PL131_SCI_Class_B: The current vltg is PL131_SCI_Class_B, not do class switch.\n\n",
	                  0, 0, 0, 0, 0, 0);
	        lErroRet = BSP_ERR_SCI_VLTG_HIGHEST;
	        break;
	    }
	    default:
	    {
	        sci_print_error("bsp_sci_vltg_class_switch: Unknow Error Happenned! pstrState->eCurrentState = %ld; Current Vltg is %ld; Current class switch state is %ld!\n\n",
	                  (int)pstrState->eCurrentState, (int)g_sci_voltage_state.sciClassKind, (int)g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG, 0, 0, 0);
	        lErroRet = BSP_ERR_SCI_CURRENT_VLTG_ERR;
	    }
    }

    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, lErroRet, __LINE__);

	if(BSP_ERR_SCI_VLTG_HIGHEST==lErroRet)
	{
		return BSP_SCI_VLTG_SWITCH_NONEED;
	}
	else
	{
		return lErroRet;
	}
}

/******************************************************************
*  Function:  sciShowVltgVal
*
*  Description:
*      Display current SIM vltg supported by LDO 11.
*
*  Calls:
*      None
*
*  Called By:  PS software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         None
*
*  Outputs:
*         None
*
* Return Value:
*         None
*
******************************************************************/
u32 bsp_sci_show_vltg_val(u32 * pu32Vltgval)
{
	if(NULL == pu32Vltgval)
    {
        sci_print_error("NULL == pu32Vltgval!",0,0,0,0,0,0);
        return BSP_ERR_SCI_INVALIDPARA;
    }

    if (g_u32ResetSuccess != SCI_DEF_RESET_SUCCESS)
    {
        sci_print_error("not initialized\n", 0, 0, 0, 0, 0, 0);
        return BSP_ERR_SCI_NOTINIT;
    }

    sci_print_info("Current Working Voltage is %ld, and Current Class Flag is %ld\n\n",
              (int)g_sci_voltage_state.sciClassKind, (int)g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG, 0, 0, 0, 0);

    *pu32Vltgval = g_sci_voltage_state.sciClassKind;

    return OK;
}


/*****************************************************************************
* 函 数 名  : bsp_sci_reg_event
*
* 功能描述  : 本接口用于注册回调函数
*
* 输入参数  : u32 u32Event      事件类型，用掩码区分：
*                                    0x1：插卡；
*                                    0x2：拔卡；
*                                    0x4：卡下电；
*                                    0x8：卡上电；
*                                    0x10：收数据；
*                                    0x20：发送数据错误
*              Sci_Event_Func * pFunc   回调函数
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*          BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
u32 bsp_sci_reg_event(u32 u32Event, Sci_Event_Func pFunc)
{
    if (NULL == pFunc)
    {
        sci_print_error("\r\n bsp_sci_reg_event: pFunc is null");
        return BSP_ERR_SCI_INVALIDPARA;
    }
    sci_reg_event(u32Event, pFunc);

    return OK;
}


/******************************************************************
*  Function:  sciGetClkStatus
*
*  Description:
*      Output current SIM clock status.
*
*  Calls:
*      None
*
*  Called By:  PS software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         None
*
*  Outputs:
*         pulSciClkStatus - 0: SCI clock start
*                           1: SCI clock stop
*
* Return Value:
*         OK - Done successful.
*         ERROR - NULL pointer error.
*
******************************************************************/
s32 bsp_sci_get_clk_status(u32 *pulSciClkStatus)
{
    u32 u32RegVal = 0;

	if (NULL == pulSciClkStatus)
    {
        sci_print_error("bsp_sci_get_clk_status: NULL == pu32SciClkStatus!");
        return BSP_ERR_SCI_INVALIDPARA;
    }

    if (g_u32ResetSuccess != SCI_DEF_RESET_SUCCESS)
    {
        sci_print_error("bsp_sci_get_clk_status: not initialized\n");
        return BSP_ERR_SCI_NOTINIT;
    }

    /* unvote and enable clk */
	pwrctrl_sci_low_power_exit();
	pwrctrl_sci_soft_clk_en();

	u32RegVal = readl(SCI_REG_CTRL0);
    if (u32RegVal & 0x40)
    {
        *pulSciClkStatus = (u32)apPL131_CLOCK_STOP;
    }
    else
    {
        *pulSciClkStatus = (u32)apPL131_CLOCK_START;
    }

    /* SCI在时钟停止的过程中如果发生了CARDDOWN、激活、去激活和warm reset这四种情况，
       会对CLKACT中断造成影响,有可能没有中断上报，所以根据中断状态来记录时钟状态会
       不准确，修改为根据寄存器状态来判断*/

    /*Set Status Value*/

    //*pulSciClkStatus = (u32)g_ulSciClkTimeStpFlag;

    return OK;
}

/******************************************************************
* Description:
* start/stop SCI clock
*
* Inputs:
* ucClkStatusMode - If the card supports clock stop mode, this parameter is used
*                   to set SIM clock voltage high or low status when passed into
*                   SIM clock stop mode.
*
* ulCardType - card type that identify SIM or USIM card.
*
* Outputs:
* none
*
* Return Value:
*  OK - successful completion
*  ERROR - failed
*  DR_ERR_SCI_PARAERROR - invalid mode specified
*  DR_ERR_SCI_UNSUPPORTED - not support such a operation
******************************************************************/
s32 bsp_sci_clk_status_cfg(u32 ucClkStatusMode)
{
    SCI_STATE_STRU * const pstrState = &g_strSciState;
    s32 lSciErroRet = OK;
    
    SCI_RECORD_EVENT(SCI_EVENT_API_CLOCK_STOP, pstrState->eCurrentState, __LINE__);

	if (ucClkStatusMode >= SCI_CLK_STATE_BUTT)
    {
        sci_print_error("bsp_sci_clk_status_cfg: enTimeStopCfg == 0x%x!",ucClkStatusMode,0,0,0,0,0);

        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        return BSP_ERR_SCI_INVALIDPARA;
    }

    if (g_u32ResetSuccess != SCI_DEF_RESET_SUCCESS)
    {
        sci_print_error("bsp_sci_clk_status_cfg: not initialized\n");

        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        return BSP_ERR_SCI_NOTINIT;
    }

    /*busy 状态不支持停止时钟*/
    if ((pstrState->eCurrentState >= PL131_STATE_RX) && (pstrState->eCurrentState <= PL131_STATE_TX))
    {
        sci_print_error("bsp_sci_clk_status_cfg: currentstate busy \n");

        SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

        return BSP_ERR_SCI_NOTREADY;
    }

    // 完全按照ATR的指示来，不关注上层传递的参数

    if ((apPL131_CLOCK_START == g_ulSciClkTimeStpFlag))
    {
        lSciErroRet = sci_clock_stop_mode_cfg(apPL131_CLOCK_STOP, ucClkStatusMode);
        if (lSciErroRet != OK)
        {
            sci_print_error("bsp_sci_clk_status_cfg: Open SIM Clock Stop Mode Failure.lSciErroRet = %ld.\r\n\n",\
                      (int)lSciErroRet,0,0,0,0,0);

            SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, pstrState->eCurrentState, __LINE__);

            return lSciErroRet;
        }

        if (OK != semTake(g_SCIClockStopSem, 100))
        {
            sci_print_error("bsp_sci_clk_status_cfg:wait clock stop interrupt error!\n");
            return BSP_OK;
        }

            /*Set Current SCI CLK Status Flag*/
        g_ulSciClkTimeStpFlag = apPL131_CLOCK_STOP;

        pwrctrl_sci_soft_clk_dis();
        pwrctrl_sci_low_power_enter();
        
        SCI_RECORD_EVENT(SCI_EVENT_LOW_POWER_ENTER, pstrState->eCurrentState, __LINE__);

    }

    SCI_RECORD_EVENT(SCI_EVENT_BEYOND_LOG, lSciErroRet, __LINE__);

    return lSciErroRet;
}


/*****************************************************************************
* 函 数 名  : bsp_sci_excreset_times
*
* 功能描述  : 本接口用于上层设置异常复位次数
*
* 输入参数  : ExcResetTimes 异常复位次数
*
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2010年8月24日   zhouluojun  creat
*
*****************************************************************************/
s32 bsp_sci_excreset_times(u32 ExcResetTimes)
{
    if (ExcResetTimes > 6)
    {
        sci_print_error("Set ExcResetTimes(%d) > 6 %d",ExcResetTimes,0,0,0,0,0);
        return BSP_ERR_SCI_INVALIDPARA;
    }

    g_SciExcResetTimes = ExcResetTimes;

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : bsp_sci_record_data_save
*
* 功能描述  : 本接口用于保存可维可测信息到yaffs中
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*             其他   操作失败
*
* 修改记录  : 2011年5月21日   yangzhi  creat
*
*****************************************************************************/
u32 bsp_sci_record_data_save()
{
    //return ((BSP_U32)sciRecordDataSave());
    return sci_record_data_save();

}

/*****************************************************************************
* 函 数 名  : bsp_sci_record_log_read
*
* 功能描述  : 本接口获取sim卡异常时定位信息
* 输入参数  : pucDataBuff: 获取定位信息的buffer
              ulMaxLength: 最大值，判断使用
*
* 返 回 值  : pulLength: 获取数据长度
*
* 修改记录  : 2011年5月21日   yangzhi  creat
*
*****************************************************************************/
s32 bsp_sci_record_log_read(unsigned char *pucDataBuff, unsigned int * pulLength, unsigned int ulMaxLength)
{
    return sci_record_log_read(pucDataBuff, pulLength, ulMaxLength);
}


s32 sci_set_voltage(SCI_VOLTAGE_SWITCH_DIRECTION direction)
{
    int retVal = 0;

    
    if(direction != SCI_VOLTAGE_SWITCH_CLASS_ZERO2CB)
    {
        regulator_disable(g_sciPmuRegu);
        SCI_RECORD_EVENT(SCI_EVENT_REGULATOR_DOWN, g_sci_voltage_state.sciClassKind, __LINE__);
        taskDelay(1);
    }

    
    switch(direction)
    {
        case SCI_VOLTAGE_SWITCH_CLASS_C2C:
            {
                if(g_sci_voltage_state.sciClassKind == PL131_SCI_Class_B)
                {
                    SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_ERROR, g_sci_voltage_state.sciClassKind, __LINE__);
                    return BSP_ERROR;
                }
            
                retVal = regulator_set_voltage(g_sciPmuRegu,1800000,1800000);/*[false alarm]:*/
                SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_CLASS_C2C, g_sci_voltage_state.sciClassKind, __LINE__);
                g_sci_voltage_state.sciClassKind= PL131_SCI_Class_C;
                break;
            }
        case SCI_VOLTAGE_SWITCH_CLASS_C2B:
            {
                if(g_sci_voltage_state.sciClassKind == PL131_SCI_Class_B)
                {
                    SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_ERROR, g_sci_voltage_state.sciClassKind, __LINE__);
                    return BSP_ERROR;           
                }
                retVal = regulator_set_voltage(g_sciPmuRegu,3000000,3000000);/*[false alarm]:*/
                SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_CLASS_C2B, g_sci_voltage_state.sciClassKind, __LINE__);
                g_sci_voltage_state.sciClassKind= PL131_SCI_Class_B;
                break;
            }
        case SCI_VOLTAGE_SWITCH_CLASS_B2B:
            {
                if(g_sci_voltage_state.sciClassKind == PL131_SCI_Class_C)
                {
                    SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_ERROR, g_sci_voltage_state.sciClassKind, __LINE__);
                    return BSP_ERROR; 
                }
                retVal = regulator_set_voltage(g_sciPmuRegu,3000000,3000000);/*[false alarm]:*/
                SCI_RECORD_EVENT(SCI_EVENT_VOLTAGE_CLASS_B2B, g_sci_voltage_state.sciClassKind, __LINE__);
                g_sci_voltage_state.sciClassKind= PL131_SCI_Class_B;
                break;
            }
       case SCI_VOLTAGE_SWITCH_CLASS_CB2ZERO:
            {        
   
                break;
            }
       case SCI_VOLTAGE_SWITCH_CLASS_ZERO2CB:
            {
                
                 if (g_SciExcResetTimes <= 2)
                 {
                     g_sci_voltage_state.sciClassKind = PL131_SCI_Class_C;

                     retVal = regulator_set_voltage(g_sciPmuRegu,1800000,1800000);
                     if(retVal != 0)
                     {
                        SCI_RECORD_EVENT(SCI_EVENT_REGULATOR_ERR,retVal , __LINE__);
                     }
                 }
                 else
                 {
                    
                     g_sci_voltage_state.sciClassKind = PL131_SCI_Class_B;
               
                     retVal = regulator_set_voltage(g_sciPmuRegu,3000000,3000000);
                     if(retVal != 0)
                     {
                        SCI_RECORD_EVENT(SCI_EVENT_REGULATOR_ERR,retVal , __LINE__);
                     } 
                 }
                break;
            }
        default:
            return BSP_ERROR;
    }
    /*上电需要进行延时*/
    if(direction != SCI_VOLTAGE_SWITCH_CLASS_CB2ZERO)
    {
        if(BSP_OK != regulator_enable(g_sciPmuRegu))
        {
            SCI_RECORD_EVENT(SCI_EVENT_REGULATOR_ERR,0 , __LINE__);
            sci_print_error("get pmu device failed.\n");
	        return ERROR;
        }
        else
        {
            SCI_RECORD_EVENT(SCI_EVENT_REGULATOR_UP,g_sci_voltage_state.sciClassKind , __LINE__);
        }
        taskDelay(2);
    }
    return BSP_OK;
}



void sci_class_switch_try(void)
{
    SCI_STATE_STRU * const pstrState = &g_strSciState;
    SCI_REG *  pBase = pstrState->pBase;
    
    g_sci_voltage_state.ucSCI_CLASS_SWITCH_FLAG = SCI_CLASS_SWITCH_PROCESS;
    g_sci_voltage_state.sci_vlotage_switch_sense = SCI_VOLTAGE_SWITCH_SENSE_CLASS_UNKNOW_TRY;
    apBIT_SET(pBase->RegCtrl2, PL131_REGCTRL2_FINISH, 1);
}


/* T=1 add Begin */

/*****************************************************************************
* 函 数 名  : bsp_sci_set_bwt
*
* 功能描述  : 本接口用于配置块等待时间
*
* 输入参数  : nBWT :配置超时时间为BWT的nBWT倍
*
* 输出参数  : 无
*
* 返 回 值  : OK/ERROR
* 修改记录  : 2013年10月15日   liuyi  creat
*
*****************************************************************************/
u32 bsp_sci_set_bwt(u32 nBWT)
{
    /* stub */
    return 0;
#if 0
#endif

}

/*****************************************************************************
* 函 数 名  : bsp_sci_protocol_switch
*
* 功能描述  : 本接口用于通知底软进行T=0和T=1协议类型切换
*
* 输入参数  : RESET_MODE enSIMProtocolMode:需要切换为的协议类型
*
* 输出参数  : 无
*
* 返 回 值  : OK/ERROR
* 修改记录  : 2013年10月15日   liuyi  creat
*
*****************************************************************************/
u32 bsp_sci_protocol_switch(PROTOCOL_MODE enSIMProtocolMode)
{
    /* stub */
    return 0;
#if 0
#endif
}

/*****************************************************************************
* 函 数 名  : bsp_sci_blk_rcv
* 功能描述  : 本接口用于在T=1时USIM Manager读取来自SCI Driver的卡返回数据；
*             如果出现BWT超时，在数据读取时返回一个超时的值SCI_TIMEOUT
*
* 输入参数  : 无
* 输出参数  : BSP_U8 *pu8Data USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer。
              BSP_U32 *pulLength SCI Driver接收到的数据长度。

* 返 回 值  : OK : 0
              ERROR : -1
              SCI_TIMEOUT : 1
*
* 修改记录  : 2013年10月15日   liuyi  creat
*
*****************************************************************************/
u32 bsp_sci_blk_rcv(unsigned char *pu8Data,u32 *pulLength)
{
    /* stub */
    return 0;
#if 0
#endif
}
/* T=1 add End */





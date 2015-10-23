
#ifndef _BSP_SCI_H_
#define _BSP_SCI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "arm_pbxa9.h"
#include "product_config.h"
#include "osl_types.h"
#include <hi_base.h>
#include "hi_sci.h"
#include "hi_syscrg.h"
#include "drv_sci.h"
#include "drv_dump.h"
#include "drv_wakelock.h"
#include "bsp_om.h"


#ifndef BSP_CONST
#define BSP_CONST               const
#endif

#define BSP_ERR_SCI_NOTINIT              951
#define BSP_ERR_SCI_INVALIDFREQ          952
#define BSP_ERR_SCI_USEDEFAULT           953
#define BSP_ERR_SCI_INVALIDPARA          954
#define BSP_ERR_SCI_NOCARD               955
#define BSP_ERR_SCI_NODATA               956
#define BSP_ERR_SCI_NOTREADY             957
#define BSP_ERR_SCI_DISABLED             958
#define BSP_ERR_SCI_UNSUPPORTED          959
#define BSP_ERR_SCI_INSERROR             960
#define BSP_ERR_SCI_GETAPDU_ERROR        961
#define BSP_ERR_SCI_VLTG_HIGHEST         962
#define BSP_ERR_SCI_CURRENT_STATE_ERR    963
#define BSP_ERR_SCI_CURRENT_VLTG_ERR     964
#define BSP_ERR_SCI_TIMESTOP_TIMEOUT     965


// card event
#define SCI_EVENT_CARD_IN                0x1
#define SCI_EVENT_CARD_OUT               0x2
#define SCI_EVENT_CARD_DEACTIVE_SUCCESS  0x4
#define SCI_EVENT_CARD_RESET_SUCCESS     0x8
#define SCI_EVENT_CARD_READ_DATA         0x10
#define SCI_EVENT_CARD_TX_ERR            0x20

#define SCI_DEF_CARD_OK     0x0
#define SCI_DEF_CARD_ERROR  0x1

#define BSP_SCI_VLTG_SWITCH_NONEED   (1)


typedef unsigned int (*Sci_Event_Func)(unsigned int u32Event, void* para);

#define SCI_DEF_READ_READY     1
#define SCI_DEF_READ_FORBID    0
#define SD_SCI_PROTOCOL_T0_CHAR 0
#define SD_SCI_PROTOCOL_T1_BLOCK 1
#define SD_SCI_PROTOCOL_NONE 2

#define DEACTIVE_SYNC_DELAY (150)

/*Define PPS process flag*/
#define PPS_RESPONE_READY   0xFA
#define PPS_RESPONE_WORK    0xAF
/*Define delay time to match PPS request start time point after ATR as needed*/
#define PPS_REQUEST_DELAY   40000

#define SCI_DEF_USIM_TYPE  0x2
#define SCI_DEF_SIM_TYPE  0x1
#define SCI_DEF_UNKNOWN_TYPE  0x0

/* print pack */
#define  sci_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SCI, "[sci]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  sci_print_info(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_SCI, "[sci]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  sci_print_debug(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_SCI, "[sci]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  sci_print_warning(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_WARNING, BSP_MODU_SCI, "[sci]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

//#define  sci_print               printf

#define  sci_print(fmt, ...)               (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_SCI, "[sci]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define SCI_BASE_ADDR g_stSciHwCfg.base_addr
#define SCI_INT  g_sci_set_select.ulSciIntNum

#define EMI_PAD_DIR_INPUT       (0)
#define EMI_PAD_DIR_OUTPUT      (1)


#define SCI_REG_DATA 		(SCI_BASE_ADDR + HI_SCI_SCI_DATA_OFFSET)
#define SCI_REG_CTRL0 		(SCI_BASE_ADDR + HI_SCI_SCI_CR0_OFFSET)
#define SCI_REG_CTRL1 		(SCI_BASE_ADDR + HI_SCI_SCI_CR1_OFFSET)
#define SCI_REG_CTRL2 		(SCI_BASE_ADDR + HI_SCI_SCI_CR2_OFFSET)
#define SCI_REG_CLKICC 		(SCI_BASE_ADDR + HI_SCI_SCI_CLKICC_OFFSET)
#define SCI_REG_BAUDVALUE 	(SCI_BASE_ADDR + HI_SCI_SCI_VALUE_OFFSET)
#define SCI_REG_BAUD 		(SCI_BASE_ADDR + HI_SCI_SCI_BAUD_OFFSET)
#define SCI_REG_TIDE 		(SCI_BASE_ADDR + HI_SCI_SCI_TIDE_OFFSET)
#define SCI_REG_DMACTRL 	(SCI_BASE_ADDR + HI_SCI_SCI_DMACR_OFFSET)
#define SCI_REG_STABLE 		(SCI_BASE_ADDR + HI_SCI_SCI_STABLE_OFFSET)
#define SCI_REG_ATIME 		(SCI_BASE_ADDR + HI_SCI_SCI_ATIME_OFFSET)
#define SCI_REG_DTIME 		(SCI_BASE_ADDR + HI_SCI_SCI_DTIME_OFFSET)
#define SCI_REG_ATRSTIME 	(SCI_BASE_ADDR + HI_SCI_SCI_ATRSTIME_OFFSET)
#define SCI_REG_ATRDTIME 	(SCI_BASE_ADDR + HI_SCI_SCI_ATRDTIME_OFFSET)
#define SCI_REG_STOPTIME 	(SCI_BASE_ADDR + HI_SCI_SCI_STOPTIME_OFFSET)
#define SCI_REG_STARTTIME 	(SCI_BASE_ADDR + HI_SCI_SCI_STARTTIME_OFFSET)
#define SCI_REG_RETRY 		(SCI_BASE_ADDR + HI_SCI_SCI_RETRY_OFFSET)
#define SCI_REG_CHTIMELS 	(SCI_BASE_ADDR + HI_SCI_SCI_CHTIMELS_OFFSET)
#define SCI_REG_CHTIMEMS 	(SCI_BASE_ADDR + HI_SCI_SCI_CHTIMEMS_OFFSET)
#define SCI_REG_BLKTIMELS 	(SCI_BASE_ADDR + HI_SCI_SCI_BLKTIMELS_OFFSET)
#define SCI_REG_BLKTIMEM 	(SCI_BASE_ADDR + HI_SCI_SCI_BLKTIMEMS_OFFSET)
#define SCI_REG_CHGUARD 	(SCI_BASE_ADDR + HI_SCI_SCI_CHGUARD_OFFSET)
#define SCI_REG_BLKGUARD 	(SCI_BASE_ADDR + HI_SCI_SCI_BLKGUARD_OFFSET)
#define SCI_REG_RXTIME 		(SCI_BASE_ADDR + HI_SCI_SCI_RXTIME_OFFSET)
#define SCI_REG_FLAG 		(SCI_BASE_ADDR + HI_SCI_SCI_FIFOSTATUS_OFFSET)
#define SCI_REG_TXCOUNT 	(SCI_BASE_ADDR + HI_SCI_SCI_TXCOUNT_OFFSET)
#define SCI_REG_RXCOUNT 	(SCI_BASE_ADDR + HI_SCI_SCI_RXCOUNT_OFFSET)
#define SCI_REG_INTMASK 	(SCI_BASE_ADDR + HI_SCI_SCI_IMSC_OFFSET)
#define SCI_REG_INTRAW 		(SCI_BASE_ADDR + HI_SCI_SCI_RIS_OFFSET)
#define SCI_REG_INTSTATUS 	(SCI_BASE_ADDR + HI_SCI_SCI_MIS_OFFSET)
#define SCI_REG_INTCLEAR 	(SCI_BASE_ADDR + HI_SCI_SCI_ICR_OFFSET)
#define SCI_REG_SYNCACT 	(SCI_BASE_ADDR + HI_SCI_SCI_SYNCACT_OFFSET)
#define SCI_REG_SYNCDATA 	(SCI_BASE_ADDR + HI_SCI_SCI_SYNCTX_OFFSET)
#define SCI_REG_SYNCRAW 	(SCI_BASE_ADDR + HI_SCI_SCI_SYNCRX_OFFSET)

/* sci clk regs */
#define SCI0_CLK_SET_BIT_SHIFT       (12)
#define SCI1_CLK_SET_BIT_SHIFT       (13)


/*Define SIM clock stop mode macro*/
#define CLK_STP_NOT_ALLOWED      0x00
#define CLK_STP_NO_PF_LEVEL      0x01
#define CLK_STP_ALLOWED_HIGH     0x04
#define CLK_STP_HIGH_LEVEL       0x05
#define CLK_STP_ALLOWED_LOW      0x08
#define CLK_STP_LOW_LEVEL        0x09

#define SCI_DEF_ATR_MAX_LENGTH      32
#define SCI_DEF_SEND_MAX_LENGTH     256
#define SCI_DEF_RECEIVE_MAX_LENGTH  256

#define SCI_DEF_RESET_SUCCESS       1
#define SCI_DEF_NOT_RESET           0
/* added by yangzhi for dma transfer,2010-11-25,begin*/
#define SCI_MAX_DATA_BUF_LEN    280
/* added by yangzhi for dma transfer,2010-11-25,end*/

/* buffer overflow Begin: */
#define SCI_BUF_MAX_LEN         288  //from 512 to 288 
/* buffer overflow End! */

/* debug info got by usim for card problem 2014-03-03*/

#define SCI_RECORD_ATR_LEN      (40)
#define SCI_RECORD_REG_LEN      (36)
#define SCI_RECORD_DATA_LEN     (72)
#define SCI_RECORD_EVENT_LEN    (32)
#define SCI_RECORD_TOTAL_LEN    (388)
/* when 72 data bytes is needed,  80 bytes is saved really */
#define SCI_RECORD_DATA_REAL_LEN    (80)
#define SCI_RECORD_PER_DATA_LEN     (9)
#define SCI_RECORD_TOTAL_CNT        (8)

#ifdef BSP_CONFIG_HI3630
/* reset bit shift*/
#define SCI_RESET_CARD0_BIT_SHIFT   (23)  
#define SCI_RESET_CARD1_BIT_SHIFT   (24)

/* reset IP Register */
#define SCI_RESET_REG_ADDR          (0x80) 
#define SCI_UNRESET_REG_ADDR        (0x84) 
#endif

/*定义指定段的宏*/
#define __section(S)                    __attribute__((__section__(#S)))
#define __sci_cfg_data                  __section(.sci.cfg.data)
#define __sci_init_data                 __section(.sci.init.data)
#define __sci_record_data               __section(.sci.record.data)

/* card set */
#define SCI_SINGLE                      0  /*卡0*/
#define SCI_SINGLE_MODLEM1              1  /*卡1*/
#define SCI_DUAL                        2  /*双卡*/

/*定义卡0、卡1的宏*/
#define CARD0                           0
#define CARD1                           1

/*定义卡与SCI0、SCI1之间连接关系的宏*/
#define CARD_CONNECT_SCI0               0
#define CARD_CONNECT_SCI1               1

/*异常纪录文件路径的定义*/
#define SCI0_RECORD_LOG_PATH_LOST_CARD   "/modem_log/log/sci0Record_lost_card.txt"
#define SCI1_RECORD_LOG_PATH_LOST_CARD   "/modem_log/log/sci1Record_lost_card.txt"

/*异常纪录文件路径的定义*/
#define SCI0_RECORD_LOG_PATH_CONTROL   "/modem_log/log/sci0Record_control.txt"
#define SCI1_RECORD_LOG_PATH_CONTROL   "/modem_log/log/sci1Record_control.txt"



/*定义任务名*/
#define SCI_CARD0_SYNC_TASK_NAME        "tSCI0SYSNC"
#define SCI_CARD1_SYNC_TASK_NAME        "tSCI1SYSNC"

/*DSDA相关的NV项定义*/
#define NV_SCI_LEN                      4

/* pmu nuem */
#define CARD_PM_TYPE_SIM0               "SIM0-vcc"
#define CARD_PM_TYPE_SIM1               "SIM1-vcc"

/* dump addr id */
#define CARD0_EXCP_RECORD_ADDR_NUM      DUMP_SAVE_MOD_SIM0
#define CARD1_EXCP_RECORD_ADDR_NUM      DUMP_SAVE_MOD_SIM1

/*定义SCI初始化函数的类型*/
typedef void (*SCI_INIT_FUNCP)(void);

/*定义硬件适配的结构*/
typedef struct
{
    unsigned int card_id;               /*0x00: 卡序号*/
    unsigned int sci_id;                /*0x04:SCI的序列号*/
    unsigned int base_addr;             /*0x08: SCI基地址*/
    unsigned int int_number;            /*0x0C: SCI中断号*/
    unsigned int clk_en_reg;            /*0x10: SCI时钟使能寄存器*/
    unsigned int clk_en_bit;            /*0x14: SCI时钟使能比特*/
    unsigned int clk_dis_reg;           /*0x18: SCI时钟禁止寄存器*/
    unsigned int clk_dis_bit;           /*0x30: SCI时钟禁止比特*/
    unsigned int dma_chan;              /*0x34: SCI读操作DMA通道号*/
    unsigned int record_enum;           /*0x38: 卡对应的异常记录区起始地址*/
    unsigned int icc_chnl;              /* sim detect icc channel */
    char *       record_file;           /*0x44: 卡对应的异常记录文件全路径*/
    unsigned int clk_gate;              /*0x48: 时钟门控的序号*/
    unsigned int sleep_vote;            /*0x4C: 睡眠投票的序号*/
    char * pwr_type;                    /*0x50: 供电类型*/
    char *       syncTaskName;          /*0x68: sync任务名*/
}SCI_CFG_STRU;

/*定义NV项的结构*/
typedef union
{
    unsigned int      value;
    struct
    {
    /* bit[0-2]    : 指示支持的多卡的模式:
                        0：单卡0模式；
                        1:     单卡1模式；
                        2：双卡模式；
                        其余：多卡，暂不支持
   */
        unsigned int  card_num      : 3;
        unsigned int  reserved_0    : 5;  /* bit[3-7]    : 保留。 */
        unsigned int  card0         : 3;  /* bit[8-10]   : 卡0使用的SCI接口。
                                                           0：使用SCI0；
                                                           1：使用SCI1；
                                                         其余值:多卡，暂不支持 */
        unsigned int  card1         : 3;  /* bit[11-13]  : 卡1使用的SCI接口。
                                                           0：使用SCI0；
                                                           1：使用SCI1；
                                                         其余值:多卡，暂不支持 */
        unsigned int  reserved_1    : 18;  /* bit[14-31]  : 保留 */
    } cfg;
} NV_SCI_CFG_UNION;

typedef struct RecData
{
    u16 usLength;
    u16 usPrelen;
    u16 usStart;
    u16 *usOutBuf;     /*SCI DMA Rx Data Buffer Pointer*/	// added by yangzhi for dma transfer,2010-11-25
    u8  ucRecBuf[SCI_BUF_MAX_LEN]; /* data block */
} SCI_REC_DATA;


/* sim set select  */
typedef enum
{
    SIM_SET_0 = 0,
    SIM_SET_1 = 1
} SCI_SET_SELECT;

typedef struct
{
    u32 ulSciSetAddr;
	u32 ulSciIntNum;
} SCI_SET_SELECT_STR;
/*
 * SCI Rx Mode Flag
 * SCI data receive working mode flag:
 * SCI_RX_CPU_MODE - use CPU to receive data from SCI Rx FIFO;
 * SCI_RX_DMA_MODE - use DMA to receive data from SCI Rx FIFO;
 */
typedef enum sciRxModeFlag
{
    SCI_RX_CPU_MODE = 0,
    SCI_RX_DMA_MODE
}SCI_RX_MODE_FLAG;

/*
 * SCI Rx Data Receive Flag,
 * SCI data receive path:
 * 1 - data rx start -> satisfied -> process -> satisfied -> process -> over;
 * 2 - data rx start -> process -> satisfied -> process -> over;
 * 3 - data rx start -> satisfied -> process -> over;
 */
typedef enum sciRxDataReceiveFlag
{
    SCI_RX_DATA_REC_START     = 0,
    SCI_RX_DATA_REC_OVER      = 1,
    SCI_RX_DATA_REC_SATISFIED = 2,
    SCI_RX_DATA_REC_PROCESS   = 3,
    SCI_RX_DATA_REC_ERROR     = 4
}SCI_RX_DATA_RECEIVE_FLAG;

/* added by yangzhi for dma transfer,2010-11-25,begin*/
/*SCI Rx Status Structure*/
/*SCI Rx Status Structure*/
typedef struct sciRxStateStru
{
   SCI_RX_MODE_FLAG         RxModeFlag;
   int                      DmaChnNo;
   SCI_RX_DATA_RECEIVE_FLAG RxDataRecFlag;
}SCI_RX_STATE_STRU;


/* debug struct */
typedef struct
{
    u32 sci_debug_base_addr_legth;
    char* sci_debug_base_addr;
}sci_debug_str;


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern SCI_CFG_STRU   g_stSciHwCfg;
extern int _sci_cfg_data_start;
extern int _sci_cfg_data_end;
extern int _sci_init_data_start;
extern int _sci_init_data_end;
extern int _sci_record_data_start;
extern int _sci_record_data_end;

/*****************************************************************************
* functions
*
*****************************************************************************/
void sci_init(void);
void I1_sci_init(void);

//
void bsp_sci_func_register(OMSCIFUNCPTR omSciFuncPtr);
void I1_bsp_sci_func_register(OMSCIFUNCPTR omSciFuncPtr);

//
s32 bsp_sci_reset(RESET_MODE rstMode);
s32 I1_bsp_sci_reset(RESET_MODE rstMode);

//
u32 bsp_sci_card_status_get (void);
u32 I1_bsp_sci_card_status_get (void);

//
s32 bsp_sci_data_send(u32 ulDataLength, unsigned char *pucDataBuffer);
s32 I1_bsp_sci_data_send(u32 ulDataLength, unsigned char *pucDataBuffer);

//
s32 bsp_sci_data_read_sync(u32 ulLength, u8 *pucRecData);
s32 I1_bsp_sci_data_read_sync(u32 ulLength, u8 *pucRecData);

//
s32 bsp_sci_all_data_read(u32 *pulLength, u8 *pucRecData);
s32 I1_bsp_sci_all_data_read(u32 *pulLength, u8 *pucRecData);

//
s32 bsp_sci_atr_get(u8 * pulLen, u8 *pucATRData,SCI_ATRInfo* stSCIATRInfo);
s32 I1_bsp_sci_atr_get(u8 * pulLen, u8 *pucATRData,SCI_ATRInfo* stSCIATRInfo);

//
u32 bsp_sci_deactive( void );
u32 I1_bsp_sci_deactive( void );

//
s32 bsp_sci_vltg_class_switch(void);
s32 I1_bsp_sci_vltg_class_switch(void);

//
u32 bsp_sci_show_vltg_val(u32 * pu32Vltgval);
u32 I1_bsp_sci_show_vltg_val(u32 * pu32Vltgval);

//
u32 bsp_sci_reg_event(u32 u32Event, Sci_Event_Func pFunc);
u32 I1_bsp_sci_reg_event(u32 u32Event, Sci_Event_Func pFunc);

//
s32 bsp_sci_get_clk_status(u32 *pulSciClkStatus);
s32 I1_bsp_sci_get_clk_status(u32 *pulSciClkStatus);

//
s32 bsp_sci_clk_status_cfg(u32 ucClkStatusMode);
s32 I1_bsp_sci_clk_status_cfg(u32 ucClkStatusMode);

//
s32 bsp_sci_excreset_times(u32 ExcResetTimes) ;
s32 I1_bsp_sci_excreset_times(u32 ExcResetTimes) ;

//
u32 bsp_sci_record_data_save(void) ;
u32 I1_bsp_sci_record_data_save(void) ;

//appl131 functions
STATUS appl131_get_clk_freq(u32 *len, unsigned char *pbuf);
STATUS I1_appl131_get_clk_freq(u32 *len, unsigned char *pbuf);

//
STATUS appl131_get_baud_rate(u32 *len, unsigned char *pbuf);
STATUS I1_appl131_get_baud_rate(u32 *len, unsigned char *pbuf);

//
STATUS appl131_get_pcsc_paremeter(unsigned char *pbuf);
STATUS I1_appl131_get_pcsc_paremeter(unsigned char *pbuf);

/* T=1 add Begin */

u32 I1_bsp_sci_set_bwt(u32 nBWT);
u32 bsp_sci_set_bwt(u32 nBWT);

u32 I1_bsp_sci_protocol_switch(PROTOCOL_MODE enSIMProtocolMode);
u32 bsp_sci_protocol_switch(PROTOCOL_MODE enSIMProtocolMode);

u32 I1_bsp_sci_blk_rcv(unsigned char *pu8Data,u32 *pulLength);
u32 bsp_sci_blk_rcv(unsigned char *pu8Data,u32 *pulLength);

s32 bsp_sci_record_log_read(unsigned char *pucDataBuff, unsigned int * pulLength, unsigned int ulMaxLength);
s32 I1_bsp_sci_record_log_read(unsigned char *pucDataBuff, unsigned int * pulLength, unsigned int ulMaxLength);

/* T=1 add End */


int bsp_sci_cfg_init(void);

int bsp_sci_cfg(unsigned int card_id, unsigned int sci_id);

int bsp_sci_parse_nv(NV_SCI_CFG_UNION * nv);

int bsp_sci_get_nv(unsigned int * value);

/*pow and clk*/
void pwrctrl_sci_soft_clk_dis(void);

void pwrctrl_sci_soft_clk_en(void);

int pwrctrl_sci_low_power_enter(void);

int pwrctrl_sci_low_power_exit(void);

#endif /*end #ifndef _BSP_DRV_SCI_H_*/




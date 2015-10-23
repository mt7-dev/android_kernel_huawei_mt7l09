
#ifndef SCI_DEBUG_H
#define SCI_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif
#include "product_config.h"
#include "osl_types.h"
#include "drv_timer.h"
#include "bsp_sci.h"


#if 1
#define SCI_EVENT_RECORD_ON
#endif

/* total size is 0x4000 */
#define SCI_RECORD_BASE_ADDR    g_sci_debug_base.sci_debug_base_addr
#define SCI_RECORD_ATR_SIZE     0x30                    /* 48 */
#define SCI_RECORD_REG_SIZE     0x90                    /* (36*4) */
#define SCI_EXC_GBL_ADDR        (SCI_RECORD_BASE_ADDR+0x100)
#define SCI_EXC_GBL_SIZE 		(0x600)
#define SCI_DATA_BASE_ADDR      (SCI_EXC_GBL_ADDR+SCI_EXC_GBL_SIZE)
#define SCI_DATA_BASE_SIZE 	    (0x1000)
#define SCI_DIR_BASE_ADDR     	(SCI_DATA_BASE_ADDR+SCI_DATA_BASE_SIZE)
#define SCI_DIR_BASE_SIZE 		(0x8C0)                 /* 0xEE0 + 0x100 + 0x20 + 0x20 = 0x1000*/
#define SCI_EVENT_BASE_ADDR     (SCI_DIR_BASE_ADDR+SCI_DIR_BASE_SIZE+0x20)    /* event 总共占0x2000 + 0x20 */
#define SCI_EVENT_HAED_SIZE     0x20
#define SCI_EVENT_RING_SIZE     0x200                   /* 0x200 * 4*4 = 0x2000 */
#define SCI_RECORD_RING_HEAD    0x100

#define SCI_CHAR_CPY_LENGTH     (32)

#define SCI_REGS_NUM 			(36)
#define SCI_EVENT_RECORD_MAGIC  0xAABBCCDD

#define SCI_EXC_LOG_TYPE_NUM    (4)

/* sci event define   */
#define SCI_HISTORY_EVENT_BASE      0
#define SCI_EVENT_RING_INIT_FLAG    0x45545247
#define SCI_EVENT_RING_STOP_FLAG    0x45565248

#define SCI_DATA_SEND_EVENT     0x55
#define SCI_DATA_RCV_EVENT      0xAA

#define SCI_DATA_TOTAL_MAX_LGTH (10)
#define SCI_DATA_PER_MAX_LGTH   (8)
#define SCI_DATA_HALF_MAX_LGTH  (4)

#define SCI_DATA_RING_SIZE 	    (0x800)


#define SCI_RECORD_LOG_PATH_NOCARD_RESET   "/modem_log/log/sciRecord_nocard_reset.txt"
#define SCI_RECORD_LOG_PATH_NOCARD_INIT   "/modem_log/log/sciRecord_nocard_init.txt"
#define SCI_RECORD_LOG_PATH_SEND_ERR   "/modem_log/log/sciRecord_send_err.txt"
#define SCI_RECORD_LOG_PATH_REV_ERR   "/modem_log/log/sciRecord_rev_err.txt"
#define SCI_RECORD_LOG_PATH_LOST_CARD   "/modem_log/log/sciRecord_lost_card.txt"


/* struct */
typedef struct
{
    u32 maxNum;
    u32 front;
    u32 rear;
    u32 num;
    u32 data[1];
}Queue;

#define sci_event_record_get_time_stamp() (omTimerGet())

typedef struct
{
    unsigned int     event;
    u32     param1;
    u32     param2;
    u32     time_stamp;
}SCI_EVENT_RING_STRUCT;

typedef struct
{
	u32                  init_flag;
	u32                  event_pos;  /* the latest event position, so, increate it for the new event.  */
	u32                  version;
	u32                  reserved;
	SCI_EVENT_RING_STRUCT *		event_ring;
}SCI_EVENT_HISTORY_STRUCT;

/* sim struct */
extern SCI_EVENT_HISTORY_STRUCT  * usim_event_history_ptr ;


typedef enum
{
    SCI_EVENT_SCI_INIT_SUCCESS,  /*SCI 模块初始化结束*/
    SCI_EVENT_API_RESET_START,
    SCI_EVENT_API_RESET_FAILED,
    SCI_EVENT_API_RESET_FINISH,
    SCI_EVENT_API_DATA_SEND,
    SCI_EVENT_API_DATA_REC_START,
    SCI_EVENT_API_DATA_REC_DONE,
    SCI_EVENT_API_DECATIVE,
    SCI_EVENT_API_CLASS_SWITCH,
    SCI_EVENT_API_CLOCK_STOP,
    SCI_EVENT_API_GET_STATUS,
    SCI_EVENT_API_GET_ATR,
    SCI_EVENT_CARD_ACTIVE_START,
    SCI_EVENT_CARD_ACTIVE_SUCCESS,
    SCI_EVENT_ATR_REC_START,
    SCI_EVENT_ATR_REC_GLOBAL,
    SCI_EVENT_ATR_REC_OVER,
    SCI_EVENT_ATR_DECODE_ERR,
    SCI_EVENT_ATR_WWT_RESET,
    SCI_EVENT_DATA_TRANS,
    SCI_EVENT_PPS_REQ,
    SCI_EVENT_PPS_PPS0,
    SCI_EVENT_PPS_PPS1,
    SCI_EVENT_PPS_DONE,
    SCI_EVENT_CLK_START,
    SCI_EVENT_CLK_STOP,
    SCI_EVENT_CLK_ERROR,
    SCI_EVENT_REGISTER_COLD_RESET,
    SCI_EVENT_REGISTER_WARM_RESET,
    SCI_EVENT_REGISTER_FINISH,
    SCI_EVENT_REGISTER_CLOCK_STOP,
    SCI_EVENT_REGISTER_CLOCK_START,
    SCI_EVENT_REGULATOR_ERR,
    SCI_EVENT_REGULATOR_UP,
    SCI_EVENT_REGULATOR_DOWN,
    SCI_EVENT_VOLTAGE_SWITCH,
    SCI_EVENT_VOLTAGE_CLASS_C2B,
    SCI_EVENT_VOLTAGE_CLASS_C2C,
    SCI_EVENT_VOLTAGE_CLASS_B2B,
    SCI_EVENT_VOLTAGE_ERROR,
    SCI_EVENT_INTR_CARD_UP,
    SCI_EVENT_INTR_CARD_DOWN,
    SCI_EVENT_INTR_TX_ERR,
    SCI_EVENT_INTR_ATRSTOUT,
    SCI_EVENT_INTR_ATRDTOUT,
    SCI_EVENT_INTR_BLKOUT,
    SCI_EVENT_INTR_CHOUT,
    SCI_EVENT_INTR_RTOUT,
    SCI_EVENT_INTR_RORI,
    SCI_EVENT_INTR_CLK_STOP,
    SCI_EVENT_INTR_CLK_ACTIVE,
    SCI_EVENT_DETECT_CARD_IN,/*M3发送插入*/
    SCI_EVENT_DETECT_CARD_OUT,
    SCI_EVENT_DETECT_CARD_LEAVE,
    SCI_EVENT_DETECT_IND_USIM,/*DETECT上报USIM*/
    SCI_EVENT_DETECT_IND_M3,/*LEAVE 上报M3*/
    SCI_EVENT_ERROR_NODATA,
    SCI_EVENT_ERROR_NOCARD,
    SCI_EVENT_ERROR_DATA_REC_BUF_OVR,
    SCI_EVENT_LOW_POWER_ENTER,
    SCI_EVENT_LOW_POWER_EXIT,
    SCI_EVENT_DMA_START,
    SCI_EVENT_DMA_ERROR,
    SCI_EVENT_BEYOND_LOG,
    SCI_EVENT_MAX,

}SCI_DEBUG_EVENT_TYEP_E;


typedef struct control_log
{
    u32 time;
    u8 sim_no;
    u8 sim_status;
    u8 hpd_level;
    u8 det_level;
    u8 trace;
    u8 sim_mux[3];
} CONTROL_LOG;

void sciEventRecordInit(void);

void sciEventRecordAdd(SCI_EVENT_HISTORY_STRUCT * event_ptr,
                       u32 event,
                       u32 param1,
                       u32 param2 );

void sciEventRecordStop(void);

void sciEventRecordStart(void);

void sciRecordExcGbl(u32 excEvent, u32 u32RecvLen,  u32 u32TimeSamp);

u32 sci_record_data_save(void);

s32 sci_record_log_read(unsigned char *pucDataBuff, unsigned int * pulLength, unsigned int ulMaxLength);

void sci_record_init_para(void);

void sci_record_exc_gbl(u32 excEvent, u32 u32RecvLen,  u32 u32TimeSamp);

void sci_record_cmd_data(u8 tfrFlag, u8 dataLen, u8 *dataAddr);

void sci_record_init(void);
void sci_event_record_add(SCI_EVENT_HISTORY_STRUCT * event_ptr,
                       u32 event,
                       u32 param1,
                       u32 param2 );
void sci_event_record_start(void);
void sci_event_record_stop(void);


#define SCI_CHECK_RECORD_BASE(para) \
do {\
    if (BSP_NULL == para) \
    {\
        sci_print_error("para:0x%x is NULL!\n",para); \
        return ; \
    } \
} while (0)




#ifdef SCI_EVENT_RECORD_ON
    #define SCI_RECORD_EVENT(_event, _p1, _p2 ) \
        sci_event_record_add(usim_event_history_ptr, _event, _p1, _p2 )
    #define SCI_RECORD_DATA(p1,p2,p3)\
    	sci_record_cmd_data(p1,p2,p3)
    #define SCI_RECORD_EXCGBL(_event, _p1, _p2) \
    	sci_record_exc_gbl(_event, _p1, _p2)

#else /* usim event record off  */
    #define SCI_RECORD_EVENT(_event, _p1, _p2 )
    #define SCI_RECORD_DATA(p1,p2,p3)
	#define SCI_RECORD_EXCGBL(_event)
#endif

#define ADD_TO_RING( _event_ptr, _event, _param1, _param2, _pos )   \
    (_event_ptr)->event_ring[ _pos ].event      = _event;           \
    (_event_ptr)->event_ring[ _pos ].param1     = _param1;          \
    (_event_ptr)->event_ring[ _pos ].param2     = _param2;          \
    (_event_ptr)->event_ring[ _pos ].time_stamp = sci_event_record_get_time_stamp();


#ifdef __cplusplus
}
#endif

#endif /* BSP_DRV_SCI_RECORD_H  */


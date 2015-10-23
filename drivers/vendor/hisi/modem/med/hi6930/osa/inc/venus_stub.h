

/*****************************************************************************
1 头文件包含
*****************************************************************************/
#include "vos.h"

#ifndef __VENUS_STUB_H__
#define __VENUS_STUB_H__

#ifdef VOS_VENUS_TEST_STUB


#define VENUS_MAX_POOL_MSG_CNT 100        /*消息池中的最多消息数*/
#define VENUS_MAX_SEQ_MSG_CNT  64         /*一个测试序列中的最多消息数*/
#define VENUS_MAX_FILE_LEN     (10*1024L+VENUS_MAX_POOL_MSG_CNT*2+2)

/*msgid for testdrv,测试消息原语id*/
enum OS_TEST_FLAG_TYPE_ENUM
{
    OS_TEST_FLAG_ENTER_RT_MSGPROC       =0x11111111,
    OS_TEST_FLAG_EXIT_RT_MSGPROC        =0x99999999,
    OS_TEST_FLAG_ENTER_MED_MSGPROC      =0x22222222,
    OS_TEST_FLAG_EXIT_MED_MSGPROC       =0x33333333,
    OS_TEST_FLAG_FILL_MSGQ              =0x66666666,
    OS_TEST_FLAG_INT_REL_SEM            =0x44444444,
    OS_TEST_FLAG_USER_EXEC              =0x55555555,
    OS_TEST_FLAG_TASK_SWITCH_START      =0x55550000,
    OS_TEST_FLAG_MSG_GET_HOOK           =0x55550001,
    OS_TEST_FLAG_MSG_ENTER_INT_HOOK     =0x66660000,
    OS_TEST_FLAG_MSG_EXIT_INT_HOOK      =0x77770000,
    OS_TEST_FLAG_MSG_INTER_CORE         =0x88888888,
    OS_TEST_FLAG_BUTT
};
typedef VOS_UINT16 OS_TEST_FLAG_TYPE_ENUM_UINT16;


/*msgid for testdrv,测试消息原语id*/
enum OS_TEST_MSGID_TYPE_ENUM
{
    OS_TEST_MSGID_START             = 0x8000, //0
    OS_TEST_MSGID_RT_SEM_P          = 0x8001,
    OS_TEST_MSGID_RT_CO_PROC        = 0x8002,
    OS_TEST_MSGID_RT_DIV_BY_ZERO    = 0x8003,
    OS_TEST_MSGID_RT_CHECK_NMI      = 0x8004,
    OS_TEST_MSGID_RT_ALLOC          = 0x8005,
    OS_TEST_MSGID_RT_SEND_TO_LOW    = 0x8006,
    OS_TEST_MSGID_RT_SEM_V          = 0x8007,
    OS_TEST_MSGID_MSG_HOOK          = 0x8008,
    OS_TEST_MSGID_ENTER_INT_HOOK    = 0x8009,
    OS_TEST_MSGID_EXIT_INT_HOOK     = 0x800a,
    OS_TEST_MSGID_INT_STATISTIC     = 0x800b,
    OS_TEST_MSGID_SEM_RESET         = 0x800c,
    OS_TEST_MSGID_STACK_SIZE        = 0x800d,
    OS_TEST_MSGID_LONG_TIME         = 0x800e,

    OS_TEST_MSGID_SEND_TO_RT        = 0x7000,  //(0x10)
    OS_TEST_MSGID_SEND_TO_RT_A      = 0x7001,
    OS_TEST_MSGID_SEM_V             = 0x7002,
    OS_TEST_MSGID_INT_SEM_V         = 0x7003,
    OS_TEST_MSGID_INT_LEVEL1        = 0x7004,
    OS_TEST_MSGID_INT_LEVEL3        = 0x7005,
    OS_TEST_MSGID_INT_LEVEL2        = 0x7006,
    OS_TEST_MSGID_INTER_CORE        = 0x7007,
    OS_TEST_MSGID_SEND_TO_RT_A_INT  = 0x7008,
    OS_TEST_MSGID_INTER_CORE_INT    = 0x7009,
    OS_TEST_MSGID_EMERGENT_MSG      = 0x700a,

    OS_TEST_MSGID_BUTT
};
typedef VOS_UINT16 OS_TEST_MSGID_TYPE_ENUM_UINT16;


typedef struct MSGFILEtag
{
    VOS_UINT32 uwCount;								/*消息池中的消息数*/
    VOS_UINT32 uwIndex[VENUS_MAX_POOL_MSG_CNT]; 	/*各个消息在uhwMsgBytes中的偏移字节数*/
    VOS_UINT16 uhwMsgBytes[VENUS_MAX_FILE_LEN];		/*标准vos消息，逐个连续放置*/
}VENUS_MSG_FILE_STRU;


typedef struct SINGLETESTSEQtag
{
    VOS_UINT32 uwTick; 		/*表示要求在该tick发送一条消息，或者一个中断*/
    VOS_UINT32 uwMsgIndex;	/*需要发送的消息的索引号，或者中断信息
                            消息索引号为>0xf0000000表示为要求发送中断，
                            中断号为uwMsgIndex - 0xf0000000*/

}VENUS_SINGLE_TEST_STRU;



typedef struct TEST_SEQ_STRUtag
{
    VOS_UINT32  uwCount;            /*消息总数*/
    VOS_UINT32  uwLogStartTick;	    /*记录log的开始tick，目前暂时不用*/
    VOS_UINT32  uwLogEndTick; 	    /*记录log的中止tick，到次tick，程序自动结束运行*/
    VENUS_SINGLE_TEST_STRU stSeq[VENUS_MAX_SEQ_MSG_CNT];
}VENUS_TEST_SEQ_STRU;

typedef struct
{
    ZOS_MSG_HEADER
    VOS_UINT16              uhwMsgID;
    VOS_UINT16              uhwReserved;
}VOS_VENUS_MSG_STRU;



VOS_VOID VOS_VenusLogUint32(VOS_UINT32 uwValue);
VOS_VOID VOS_VenusLogInfo(VOS_UINT32 *puwValue, VOS_UINT32  uwDataLen);
VOS_VOID VOS_TrigSendmsg(VOS_UINT32 uwTick);
VOS_VOID VOS_InitTest();
VOS_VOID VOS_IntHandlers_02();
VOS_VOID VOS_IntHandlers_07();

#define VOS_DEBUG_LOG_CPU_INFO(uwValue) VOS_VenusLogUint32(uwValue)
#define VOS_DEBUG_SIM_CLEAR_INTERRUPT(uwIntNo) VOS_VenusLogUint32(0xa5a5a500+(uwIntNo))
#define VOS_DEBUG_LOG_CLK() (*((VOS_UINT32 *)(0x3f000004)) = 0x5a5a5a5a)
#define VOS_DEBUG_GET_TICK() (*((VOS_UINT32 *)(0x3f000000)))
#define VOS_DEBUG_LOG_EXCEPTION(uwExecCauseNo) VOS_VenusLogUint32(0xeeee0000 + (uwExecCauseNo))
#define VOS_DEBUG_LOG_CPU_INFO_A(puwValue,uwSize) VOS_VenusLogInfo(puwValue,uwSize)
#else
#define VOS_DEBUG_LOG_CPU_INFO(uwValue)
#define VOS_DEBUG_SIM_CLEAR_INTERRUPT(uwIntNo)
#define VOS_DEBUG_LOG_CLK()
#define VOS_DEBUG_GET_TICK()
#define VOS_DEBUG_LOG_EXCEPTION(uwExecCauseNo)
#define VOS_DEBUG_LOG_CPU_INFO_A(puwValue,uwSize)
#endif

#endif

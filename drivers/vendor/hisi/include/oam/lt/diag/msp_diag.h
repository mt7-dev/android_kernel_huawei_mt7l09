

#ifndef __MSP_DIAG_H__
#define __MSP_DIAG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "vos.h"
#include "DrvInterface.h"

#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
/*最好PS将MODID 和DIAG_AIR_MSG_LOG_ID的地方都替换成DIAG_ID*/
#define DIAG_ID(module_id, log_type)   (VOS_UINT32)(module_id | (log_type << 12))

#define MODID(module_id, log_type) DIAG_ID(module_id, log_type)

#define DIAG_AIR_MSG_LOG_ID(module_id, is_up_link)  DIAG_ID(module_id, is_up_link) /*module_id对应PID*/


/*is_up_link取值*/
#define OS_MSG_UL                (0x01)/* 表示上行消息*/
#define OS_MSG_DL                (0x02)/* 表示下行消息*/


#define DIAG_SIDE_UE             (0x1)  /* 表示UE接收的空口消息：NET-->UE*/
#define DIAG_SIDE_NET            (0x2)  /* 表示NET接收的空口消息：UE-->NET*/


/* 事件信息打印级别定义*/
#define LOG_TYPE_INFO            0x00000008UL
#define LOG_TYPE_AUDIT_FAILURE   0x00000004UL
#define LOG_TYPE_AUDIT_SUCCESS   0x00000002UL
#define LOG_TYPE_ERROR           0x00000001UL
#define LOG_TYPE_WARNING         0x00000010UL

/*DIAG消息ID定义*/
/*diag fw发给自己的解码消息处理请求*/
#define ID_MSG_DIAG_CMD_REQ_FW_TO_FW                (0x00010001)

/*diag fw发给AGENT模块的请求，以及相应的回复消息*/
#define ID_MSG_DIAG_CMD_REQ_FW_TO_AGENT             (0x00010002)
#define ID_MSG_DIAG_CMD_CNF_AGENT_TO_FW             (0x00010003)
#define ID_MSG_DIAG_HSO_DISCONN_IND                 (0x00010004)

/*diag fw发给APP AGENT模块的请求，以及相应的回复消息*/
#define ID_MSG_DIAG_CMD_REQ_FW_TO_APP_AGENT         (0x00010005)
#define ID_MSG_DIAG_CMD_CNF_APP_AGENT_TO_FW         (0x00010006)

/*diag AGENT发给BBP AGENT模块的请求*/
#define ID_MSG_REQ_DRX_DATA_SAMPLE_START            (0x00010009)
#define ID_MSG_REQ_DRX_DATA_SAMPLE_STOP             (0x0001000A)
#define ID_MSG_REQ_DRX_LOG_SAMPLE_START             (0x0001000B)
#define ID_MSG_REQ_DRX_LOG_SAMPLE_STOP              (0x0001000C)

#define ID_MSG_DIAG_CMD_IND                         (0x0001000D)

/*diag AGENT发给PS模块的HSO回放请求*/
#define ID_MSG_DIAG_CMD_REPLAY_TO_PS    			(0x00010100)

/*发给diag APP AGENT的端口数据处理消息ID*/
#define ID_MSG_DIAG_CMD_PORT_REQ_TO_APP_AGENT       (0x00010200)

/*发起主动复位时MSP传入的参数ID最小值为:0xB000000*/
#define DIAG_CALLED_IN_IRQ          (0xB0000000)
#define DIAG_REALSE_POINTER_ERROR   (0xB0000001)

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/
#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
/* 发送给MSP_PID_DIAG_APP_AGENT的投票请求消息结构体 */
typedef struct
{
    VOS_MSG_HEADER
    SOCP_VOTE_ID_ENUM_U32   ulVoteId;    /* 投票组件 */
    SOCP_VOTE_TYPE_ENUM_U32 ulVoteType;  /* 投票类型 */
}DIAG_MSG_SOCP_VOTE_REQ_STRU;

/* 反对票消息回复 */
typedef struct
{
    VOS_MSG_HEADER
    SOCP_VOTE_ID_ENUM_U32   ulVoteId;    /* 投票组件 */
    SOCP_VOTE_TYPE_ENUM_U32 ulVoteType;  /* 投票类型 */
    VOS_UINT32              ulVoteRst;   /* 投票结果，0-成功, 0xFFFFFFFF-失败 */
}DIAG_MSG_SOCP_VOTE_WAKE_CNF_STRU;
#endif
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*最大允许字节数,包括前面文件名和行号长度，-1是预留\0结束符*/
#define DIAG_PRINTF_MAX_LEN 	(1000-1)

extern VOS_UINT32 DIAG_PrintfV_TDS( VOS_CHAR* pszFmt, ...);

extern VOS_UINT32 DIAG_PrintfV_TDS1( VOS_UINT32 id ,VOS_CHAR* pszFmt, ...);

extern VOS_UINT32 DIAG_PrintfV(VOS_UINT32 id, VOS_CHAR* pszFileName, VOS_UINT32 ulLineNum, VOS_CHAR* pszFmt, ...);

/* 支持的最大参数个数 */
#define LTE_DIAG_PRINTF_PARAM_MAX_NUM   (6)

/* logid生成规则 */
#define DIAG_LOG_ID(FILEID, LINE)       (((FILEID & 0x0000FFFF) << 16 )|(LINE & 0x0000FFFF))

/******************************************************************************
函数名称: LTE_DIAG_LOG
功能描述: 打印点类型的打印接口函数
参数说明:
            ulModuleId[in]  : 模块id，对应PID
            ulSubModId[in]  : 子模块id
            ulLevel[in]     : 打印级别
            ulLogId[in]     : 由文件号和行号根据DIAG_LOG_ID生成
            amount[in]      : 可变参数个数（最大值6，不包括ulModuleId/ulLevel/ulLogId/amout）
            ...             : 可变参数
调用约束:
            1. 绝对禁止对此函数进行二次封装，只能转定义
            2. 支持可变的参数个数，但必须在调用时由参数amount指定参数个数
            3. 可变参数只支持int类型
            4. 目前版本中支持的最大参数个数是6个，超过的参数默认丢弃
******************************************************************************/
extern VOS_INT32 LTE_DIAG_LOG(VOS_UINT32 ulModuleId, VOS_UINT32 ulSubModId, 
                        VOS_UINT32 ulLevel, VOS_UINT32 ulLogId, 
                        VOS_UINT32 amount, ...);


extern VOS_UINT32 DIAG_ReportEventLog(VOS_UINT32 ulId, VOS_UINT32 ulEventID);



typedef struct
{
    VOS_UINT32 ulId;
    VOS_UINT32 ulMessageID;  /* Specify the message id.*/
    VOS_UINT32 ulSideId;     /* 空口消息的方向*/
    VOS_UINT32 ulDestMod;    /* 目的模块*/
    VOS_UINT32 ulDataSize;   /* the data size in bytes.*/
    VOS_VOID* pData;      /* Pointer to the data buffer.*/
} DIAG_AIR_MSG_LOG_STRU;
extern VOS_UINT32 DIAG_ReportAirMessageLog(DIAG_AIR_MSG_LOG_STRU* pRptMessage);
typedef struct
{
    VOS_UINT32 ulId;
    VOS_UINT32 ulMessageID;  /* Specify the message id.*/
    VOS_UINT32 ulSideId;     /* VoLTE消息的方向,0x1:网侧到UE侧 0x2:UE侧到网侧 0xFFFFFFFF:为无效值,代表与方向无关*/
    VOS_UINT32 ulDestMod;    /* 目的模块*/
    VOS_UINT32 ulDataSize;   /* the data size in bytes.*/
    VOS_VOID*  pData;        /* Pointer to the data buffer.*/
} DIAG_VoLTE_LOG_STRU;

extern VOS_UINT32 DIAG_ReportVoLTELog(DIAG_VoLTE_LOG_STRU* pRptMessage);
typedef struct
{
    VOS_UINT8	    ucFuncType;
    VOS_UINT8       ucReserved1;
    VOS_UINT16      usLength;
    VOS_UINT32      ulSn;           /*Sequence Number for Trace, Event, OTA msg.*/
    VOS_UINT32      ulTimeStamp;    /*CPU time coming from ARM.*/
	VOS_UINT16      usPrimId;
    VOS_UINT16      usToolId;
	VOS_UINT16      usOtaMsgID;         /*空口消息的ID号*/
    VOS_UINT8       ucUpDown;           /*空口消息的方向*/
    VOS_UINT8       ucReserved;         /*逻辑信道类型*/
    VOS_UINT32      ulLengthASN;        /*空口消息内容的长度*/
	VOS_VOID*    pData;                 /* Pointer to the data buffer.*/
} DIAG_GU_AIR_MSG_LOG_STRU;
extern VOS_UINT32 DIAG_ReportGUAirMessageLog(VOS_VOID* pRptMessage,VOS_UINT16 ulMsgLen);
typedef DIAG_AIR_MSG_LOG_STRU DIAG_LAYER_MSG_STRU;

extern VOS_UINT32 DIAG_ReportLayerMessageLog(DIAG_LAYER_MSG_STRU* pRptMessage);



typedef struct
{
    VOS_UINT32 ulMessageID;    /* Specify the message id.*/
    VOS_UINT32 ulDataSize;     /* the data size in bytes.*/
    VOS_VOID* pData;        /* Pointer to the data buffer.*/
} DIAG_USERPLANE_MSG_STRU;

extern VOS_UINT32 DIAG_ReportUserPlaneMessageLog(DIAG_USERPLANE_MSG_STRU* pRptMessage);
extern VOS_UINT32 DIAG_ReportCommand(VOS_UINT16 ulID,VOS_UINT32 ulDataSize,VOS_VOID* pData);



typedef VOS_UINT32 (*DIAG_GTR_DATA_RCV_PFN)(VOS_UINT32 ulGtrDtaSize, VOS_VOID* pGtrData);

extern VOS_VOID DIAG_GtrRcvCallBackReg(DIAG_GTR_DATA_RCV_PFN pfnGtrRcv);


/*提供给AT调用*/
VOS_UINT32 diag_LogPortSwitch(VOS_UINT32 ulLogPort);
VOS_UINT32 diag_getLogPort(VOS_VOID);

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)

VOS_UINT32 DIAG_SocpPowerOn(VOS_VOID);


VOS_VOID DIAG_DspInitFinished(VOS_VOID);
#else
static VOS_UINT32 inline DIAG_SocpPowerOn(VOS_VOID)
{
    return 0;
}

static VOS_VOID inline DIAG_DspInitFinished(VOS_VOID)
{
    return;
}
#endif
/*****************************************************************************
  9 OTHERS
*****************************************************************************/










#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of msp_diag.h */

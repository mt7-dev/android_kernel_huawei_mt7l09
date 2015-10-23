/**************************************************************************************************
Copyright (C), 2014-2024, Huawei Tech. Co., Ltd.

File name       : hme_jb_api.h
Version: 1.00
Description     :

Others          :
Function List   :
1: ...
History         :
Date            ver         Modification

**************************************************************************************************/
#ifndef _HME_JB_H_
#define _HME_JB_H_

//对外只提供一个接口文件，接口文件时间数据结构定义
typedef char           int8;
typedef signed short   int16;
typedef signed int     int32;
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

#define HME_JB_MEMSIZE      HME_JB_GetMemorySize()              //分配内存总数， bytes
#define HME_VERSTR_LENGTH   30

//RTP包格式
typedef struct _HME_JB_RTP_INFO
{
    uint16 iCodecType;                 //解码器类型
    uint16 iFrameLength;               //一帧长度(ms)
    uint8* pPayload;                   //RTP负载数据的指针
    uint32 ulPayloadLength;            //负载数据长度，单位字节
    uint16 usSequenceNumber;           //序列号，16比特，RTP包的序列号
    uint16 uFreq;                      //采样率
    uint32 ulTimestamp;                //时戳，32比特
    uint32 ulSSRC;                     //ssrc,源描述符
    uint32 u32Time;                    //系统时间
    uint8  ucPayloadType;              //负载类型，7比特，此域定义了负载的格式，由具体应用决定其解释
    int8   iRes[7];                    //保留使用

    //内部使用
    int32 lNetDelay;                   //网络时延
    int32 lEstTotalDelay;              //目标总时延
    int32 lRealTotalDelay;             //在算法模块中计算的实际总时延
}HME_JB_RTP_INFO;


//设置jitter buf调整模式
typedef enum _HME_JB_MODE_ENUM
{
   HME_JBMODE_AUTOMATIC = 0,            //自动模式，自动调整JB参数设置,适合WiFi网络设置
   HME_JBMODE_MANUAL = 1,               //手动模式,配合使用JB参数设置
   HME_JBMODE_STATIC   = 2              //静态JB
}HME_JB_MODE_ENUM;

//设置jitter buf参数
typedef  struct _HME_JB_PARAMS_STRU
{
    uint16 uiInitJitterBuf;              //初始深度(静态jb有效,动态jb默认40ms，静态建议设置为100ms)
    uint16 uiMinJitterBuf;               //最小JB深度(动态jb有效,ms)
    uint16 uiMaxJitterBuf;               //最大JB深度,外部分配缓冲深度(ms)
    int8   iRes[2];                      //4字节对齐
}HME_JB_PARAMS_STRU;


//解码数据结构
typedef struct _HME_JB_DEC_STRU
{
    int16   ucPayloadType;               //解码器类型或PT值
    int16   bfi;                         //bad帧指示位 0：good帧 1：bad帧/丢包 (当为1时表示要调用解码器的plc功能，pcPayload将为NULL)
    int8    *pcPayload;                  //解码输入码流指针
    uint32  ulInputLength;               //解码输入码流长度，以字节为单位，非传输帧时，长度为0
    int16   *psOutputData;               //解码输出PCM数据指针
    uint32  ulOutputLength;              //解码输出PCM数据长度，以样点为单位
    int8    bPreFrameType;               //最后一帧的帧类型，是语音帧还是非语音帧(3表示语音帧，2表示sid,1表示nodata,0表示丢帧)
    int8    iRes[7];                     //保留使用
} HME_JB_DEC_STRU;

typedef struct _HME_JB_LOG_INFO
{
    int16 usLevel;
    int16 usFileNo;
    int16 usLineNo;
    int16 usFreqency;
    int32 uiLogId;//err code
    int32 idate1;
    int32 idate2;
    int32 idate3;
}HME_JB_LOG_INFO;


typedef struct _HME_JB_HIFI_STATICS
{
    uint32   uiFrameType;            //0/1/2 语音帧，下溢，静音帧
    uint32   uiBufLen;               //当前缓冲深度(ms)
    uint32   uiPacketDelay;          //包缓存时延
    uint32   uiAveBufDelay;          //平均缓存时延
    uint32   uiStartDelay;           //第一包的缓存时延
    uint32   uiBufMaxSize;           //最大的缓存大小(帧数)

    uint32   uiSeq;                  //包序号
    uint32   uiTimeStamp;            //包时戳
    uint32   uiSSRC;                 //ssrc
    uint32   iTimeRecv;              //包接收时候
    int32    iNetDelay;              //当前网络时延
    int32    iEstDelay;              //目标时延
    int32    iTotalDelay;            //计算总时延
    int32    iRealTotalDelay;        //实际总时延
    int32    iAveNetDelay;           //平均网络时延
    int32    iAveNetJitter;          //平均网络抖动

    uint32   uiPacketRecv;           //总的收包数
    uint32   uiPacketLoss;           //总的丢包数
    uint32   uiJBPacketLoss;         //jb主动丢包数
    uint32   uiPlcCount;             //总的plc次数
    uint32   uiTSMCount;             //TSM次数
    uint32   uiTSMDateLen;           //TSM总的处理时间(ms)
    int8     iRes[8];                //保留使用
}HME_JB_HIFI_STATICS;


typedef struct _HME_JB_TRACE_INFO
{
    uint16 usTraceId;
    uint16 usDataLen;
    void * pData;
}HME_JB_TRACE_INFO;


//解码回调函数指针类型定义,HME_JB_DEC_STRU
typedef int32 (*pDecode)(void *pstJBmodule, void *pstDecPrm);

//注册时间获取函数,获取系统时间单位毫秒
typedef int32 (*pSystemTime)(uint32 *piTimeMillisecond);

//维护接口反向注册HME_JB_LOG_INFO
//(1)反向注册写日志  iErrCode 表示错误码，pDate表示要打印的日志信息，iDateLen表示要打印的日志信息的长度
typedef int32  (*pWriteLog)(void *pLogMessage);

//(2)反向注册写Trace数据,pDate表示pcm数据指针，iDateLen表示每次写入的pcm数据长度HME_JB_TRACE_INFO
typedef int32 (*pWriteTrace)(void *pTraceData);


//jb模块初始化使用结构体
typedef struct _HME_JB_CREATE_STRU_
{
    HME_JB_MODE_ENUM           enJBMode;        //jb工作模式
    HME_JB_PARAMS_STRU         stJBParams;      //jb参数信息
    void                       *pMemAddr;       //外部分配JB模块使用的内存地址(HME_JB_MEMSIZE表示)
    pSystemTime                pGetTime;        //系统时间获取反向注册接口
    pWriteLog                  pLog;            //日志回调注册接口
    pWriteTrace                pTrace;          //Trace回调注册接口
    int8                       iRes[8];         //保留使用
}HME_JB_CREATE_STRU;


#ifdef __cplusplus
 extern "C" {
#endif

int32 HME_JB_GetMemorySize(void);

//1、主功能函数
/**************************************************************************************************
Function        : HME_JB_Init
Description     : JB模块的创建和初始化
Input Parameter : pstJBCreate   JB模块需要使用的工作模式/JB参数信息，分配的内存地址，外部接口注册
Output Parameter: pstJBmodule  JB模块句柄
Return Type     :
Other           : 无
**************************************************************************************************/
int32 HME_JB_Init(void** ppstJBmodule, HME_JB_CREATE_STRU *pstJBCreate);

/**************************************************************************************************
Function        : HME_JB_RegDecode
Description     : 将解码器函数反向注册给JB模块
Input Parameter : pstJBmodule                JB模块句柄
                  pStreamRecv                HME自身使用保留，其他产品直接传递NULL
                  pStreamRecvDecode          编解码函数指针
                  uiFrequency                采样频率(8000/16000/24000/48000)
Output Parameter:
Return Type     :
Other           : 通话中切换codec也可以使用
**************************************************************************************************/
int32 HME_JB_RegDecode(void* pstJBmodule, void *pStreamRecv,
                      pDecode pStreamRecvDecode, uint32 uiFreqency);

/**************************************************************************************************
Function        : HME_JB_AddPacket
Description     : 将RTP数据包加入到JB的包缓存中，并更新JB的算法模块。
Input Parameter : pstJBmodule  JB模块句柄
                  pRtpInf      RTP帧信息
Output Parameter:
Return Type     :
Other           : 无
**************************************************************************************************/
int32 HME_JB_AddPacket(void* pstJBmodule, HME_JB_RTP_INFO* pRtpRtcpPacket);

/**************************************************************************************************
Function        : HME_JB_GetPcmData
Description     : 从JB模块获取指定长度的pcm数据
Input Parameter : pstJBmodule  JB模块句柄
                  uinLength    需要获取的数据长度(单位ms,需要是5ms的整数倍)
Output Parameter: pPcmOut      输出pcm数据指针
Return Type     :
Other           : 无
**************************************************************************************************/
int32 HME_JB_GetPcmData(void* pstJBmodule,
                                       uint16             usLength,
                                       void*              pPcmOut,
                                       uint32*            puiOutLen);

//3、维护接口
/**************************************************************************************************
Function        : HME_JB_SetMaintainStatus
Description     : 设置日志和Trace的打印状态
Input Parameter :
                  iFileWrite        日志打印状态，0不打印，非0打印
                  iTraceWrite       Trace数据打印状态，0不打印，非0打印
Output Parameter:
Return Type     :
Other           : 默认不打印日志和Trace数据
**************************************************************************************************/
int32 HME_JB_SetMaintainStatus(int8 iFileWrite,int8 iTraceWrite);


#ifdef __cplusplus
}
#endif
#endif

/**************************************************************************************************
End of File
**************************************************************************************************/

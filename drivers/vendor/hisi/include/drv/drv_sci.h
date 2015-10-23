/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_sci.h
*
*   作    者 :  yangzhi
*
*   描    述 :  
*
*   修改记录 :  2013年1月18日  v1.00  yangzhi创建
*************************************************************************/
#ifndef __DRV_SCI_H__
#define __DRV_SCI_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "drv_comm.h"

typedef enum tagSCI_CLK_STOP_TYPE_E
{
    SCI_CLK_STATE_LOW = 0,        /* 协议侧指示停低*/
    SCI_CLK_STATE_HIGH = 1,       /*协议侧指示停高*/
    SCI_CLK_STATE_UNSUPPORTED = 2,/*协议侧指示不支持*/
    SCI_CLK_STATE_BUTT
}SCI_CLK_STOP_TYPE_E;


/*  SIM  state*/
typedef enum tagSCI_CARD_STATE_E
{
    SCI_CARD_STATE_READY = 0,           	/* Ready */
    SCI_CARD_STATE_NOCARD,                	/* No card */
    SCI_CARD_STATE_BUSY,               		/* In initialization*/
	SCI_CARD_STATE_BUTT               		/* Butt*/
} SCI_CARD_STATE_E;

/* card in out status */
typedef enum
{
    SIM_CARD_OUT = 0,
    SIM_CARD_IN  = 1,
} SCI_DETECT_STATE;

/* T=1 add Begin:*/
/*for reset mode*/
typedef enum
{
    WARM_RESET = 0,     /* warm reset */
    COLD_RESET = 1,     /* cold reset */
    RESET_MODE_BUTTOM
}RESET_MODE;

/*for protocol mode*/
typedef enum
{
    T0_PROTOCOL_MODE = 0,          /*T=0*/
    T1_PROTOCOL_MODE = 1,          /*T=1*/
    PROTOCOL_MODE_BUTTOM
}PROTOCOL_MODE;

/*for EDC mode*/
typedef enum
{
    T1_ERR_DETECT_MODE_LRC = 0,     /*LRC*/
    T1_ERR_DETECT_MODE_CRC = 1,     /*CRC*/
    T1_ERR_DETECT_MODE_BUTTOM
}T1_ERR_DETECT_MODE;


/*for ATR info*/
typedef struct 
{
    PROTOCOL_MODE      Tbyte;   
    BSP_U32            valIFSC;
    BSP_U32            valIFSD;
    T1_ERR_DETECT_MODE errDetectMode;
}SCI_ATRInfo;

#define SCI_RECORD_ATR_LEN      (40)
#define SCI_RECORD_REG_LEN      (36)
#define SCI_RECORD_DATA_LEN     (72)
#define SCI_RECORD_EVENT_LEN    (32)
#define SCI_RECORD_TOTAL_LEN    (388)

typedef struct  stSciRecordInfo
{
    unsigned short  cardClass;   /*卡电压类型：1800或3000*/
    unsigned short  ATRlen;      /*ART数据实际长度*/
    unsigned char   ATRData[SCI_RECORD_ATR_LEN];  /*ART数据记录,不超过40个字节*/
    unsigned int    SciRegData[SCI_RECORD_REG_LEN];/*SCI所有寄存器记录*/
    unsigned char   SciRecordData[SCI_RECORD_DATA_LEN];/*至少记录和卡交互的最后8条数据，每条不超过9个字节*/
    unsigned int    SciEvent[SCI_RECORD_EVENT_LEN];/*SCI 底层事件最后8条记录*/
}SCI_RECORD_INFO;

/* T=1 add End */


typedef void (*OMSCIFUNCPTR)(unsigned int ulVal);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_RST
*
* 功能描述  : 本接口用于复位SCI（Smart Card Interface）驱动和USIM（Universal
*           Subscriber Identity Module）卡
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : OK  复位成功
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_RST(RESET_MODE rstMode);

/********************************************************************************************************************
 函 数 名  : DRV_USIMMSCI_GET_CARD_STAU
 功能描述  : 本接口用于获得卡当前的状态，目前LTE提供接口参数类型不一致
 输入参数  : 无。
 输出参数  :
 返 回 值  : 0:  卡处于Ready；
             -1：卡未处于Ready。
 注意事项  ：有效调用BSP_SCI_Reset()函数后，再调用此函数。
********************************************************************************************************************/
BSP_U32 DRV_USIMMSCI_GET_CARD_STAU(BSP_VOID);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_SND_DATA
*
* 功能描述  : 本接口用于发送一段数据到USIM卡
*
* 输入参数  : BSP_U32 u32DataLength 发送数据的有效长度。取值范围1～256，单位为字节
*             BSP_U8 *pu8DataBuffer 发送数据所在内存的首地址，如是动态分配，调用接口
*                                   后不能立即释放，依赖于硬件发完数据
* 输出参数  : 无
*
* 返 回 值  : OK
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_INVALIDPARA
*           BSP_ERR_SCI_DISABLED
*           BSP_ERR_SCI_NOCARD
*           BSP_ERR_SCI_NODATA
*
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_SND_DATA(BSP_U32 u32DataLength,BSP_U8 * pu8DataBuffer);
/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_RCV
*
* 功能描述  : 本接口用于USIM Manager读取来自SCI Driver的卡返回数据
*             该接口为阻塞接口，只有SCI接收到足够的数据量后才会返回；
*             该接口的超时门限为1s
*
* 输入参数  : BSP_U32 u32DataLength USIM Manager欲从SCI Driver读取的数据长度。
* 输出参数  : BSP_U8 *pu8DataBuffer USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer。
* 返 回 值  : OK
*             BSP_ERR_SCI_NOTINIT
*             BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_RCV(BSP_U32 u32DataLength,BSP_U8 * pu8DataBuffer);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_RCV_ALL
*
* 功能描述  : 本接口用于USIM Manager在读数超时的时候，调用本函数，读取接收数据缓冲中的所有数据
*
* 输入参数  : 无  
* 输出参数  : u32 *u32DataLength Driver读取的数据长度，返回给USIM Manager。取值范围1～256，单位是字节
*           u8 * pu8DataBuffer USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
BSP_U32 DRV_USIMMSCI_RCV_ALL(BSP_U32 *u32DataLength,BSP_U8 * pu8DataBuffer);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_GET_ATR
*
* 功能描述  : 本接口用于将Driver层缓存的ATR数据和数据个数返回给USIM Manager层
*
* 输入参数  : BSP_VOID
* 输出参数  : unsigned long *u8DataLength  Driver读取的ATR数据长度，返回给USIM Manager。
*                                   取值范围0～32，单位是字节
*           BSP_U8 *pu8ATR          USIM Manager指定的Buffer，SCI Driver将ATR
*                                   数据拷贝到本Buffer。一般为操作系统函数动态分配
*                                   或者静态分配的地址
*
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_GET_ATR(unsigned long * u32DataLength, BSP_U8 * pu8ATR,SCI_ATRInfo* stSCIATRInfo);
/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_CLASS_SWITCH
*
* 功能描述  : 本接口用于支持PS对卡的电压类型进行切换，从1.8V切换到3V
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  :  OK    当前电压不是最高的，进行电压切换操作
*           BSP_ERR_SCI_CURRENT_STATE_ERR 切换失败 current SCI driver state is ready/rx/tx
*           BSP_ERR_SCI_VLTG_HIGHEST   当前电压已经是最高电压，没有进行电压切换
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_CURRENT_VLTG_ERR 当前电压值异常（非class B或者C）
*
* 修改记录  : 
*
*****************************************************************************/
BSP_S32  DRV_USIMMSCI_CLASS_SWITCH(BSP_VOID);
/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_SHOW_VLT
*
* 功能描述  : 本接口用于显示当前SIM卡接口电压
*
* 输入参数  : 无  
* 输出参数  :u32 * pu32Vltgval 当前电压
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
BSP_U32  DRV_USIMMSCI_SHOW_VLT(BSP_U32* pu32Vltgval);
/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_TM_STOP
*
* 功能描述  : 本接口用于支持PS关闭SIM卡时钟
*
* 输入参数  :
*           SCI_CLK_STOP_TYPE_E enTimeStopCfg 时钟停止模式
*
* 输出参数  : 无
*
* 返 回 值  : OK - successful completion
*               ERROR - failed
*               BSP_ERR_SCI_NOTINIT
*               BSP_ERR_SCI_INVALIDPARA - invalid mode specified
*               BSP_ERR_SCI_UNSUPPORTED - not support such a operation
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_TM_STOP(SCI_CLK_STOP_TYPE_E enTimeStopCfg); 

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_DEACT
*
* 功能描述  : 本接口用于对SIM卡的去激活操作
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : OK
*             BSP_ERR_SCI_NOTINIT
* 修改记录  : 
*
*****************************************************************************/
BSP_U32 DRV_USIMMSCI_DEACT(BSP_VOID);
/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_GET_CLK_STAU
*
* 功能描述  : 本接口用于获取当前SIM卡时钟状态
*
* 输入参数  : BSP_U32 *pu32SciClkStatus   变量指针，用于返回SIM卡时钟状态：
*                                       0：时钟已打开；
*                                       1：时钟停止
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*          BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 
*
*****************************************************************************/
BSP_U32 DRV_USIMMSCI_GET_CLK_STAU(BSP_U32 * pu32SciClkStatus);
/*****************************************************************************
* 函 数 名  : DRV_PCSC_GET_CLK_FREQ
*
* 功能描述  : 本接口用于获取当前SIM卡时钟频率
*
* 输入参数  : 无
*
* 输出参数  : unsigned long *pLen   时钟频率数据的长度
*             BSP_U8 *pBuf    时钟频率数据
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 
*
*****************************************************************************/
BSP_U32 DRV_PCSC_GET_CLK_FREQ(unsigned long * pLen,BSP_U8 * pBuf); 
/*****************************************************************************
* 函 数 名  : DRV_PCSC_GET_BAUD_RATE
*
* 功能描述  : 本接口用于获取当前SIM卡时钟频率
*
* 输入参数  : 无
*
* 输出参数  : unsigned long *pLen   波特率数据的长度
*             BSP_U8 *pBuf    波特率数据
*
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 
*
*****************************************************************************/
BSP_U32 DRV_PCSC_GET_BAUD_RATE(unsigned long * pLen, BSP_U8 * pBuf);

/*****************************************************************************
* 函 数 名  : DRV_PCSC_GET_SCI_PARA
*
* 功能描述  : 本接口用于获取当前SIM卡的PCSC相关参数
*
* 输入参数  : 无
*
* 输出参数  : BSP_U8 *pBuf    PCSC相关参数
*
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 
*
*****************************************************************************/
BSP_U32 DRV_PCSC_GET_SCI_PARA(BSP_U8 * pBuf); 

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_FUNC_REGISTER
*
* 功能描述  : 本接口用于注册OAM的回调函数
*
* 输入参数  : omSciFuncPtr
* 输出参数  : 无
*
* 返 回 值  : NA
*
* 修改记录  : 
*
*****************************************************************************/
void DRV_USIMMSCI_FUNC_REGISTER(OMSCIFUNCPTR omSciFuncPtr);

/*****************************************************************************
 函 数 名  : DRV_USIMMSCI_RECORD_DATA_SAVE
 功能描述  : 本接口用于获取当前SIM卡交互数据。属于异常处理模块，V7先打桩
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 0:  操作成功；
             -1：操作失败，指针参数为空。
 注意事项  ：
*****************************************************************************/
BSP_U32 DRV_USIMMSCI_RECORD_DATA_SAVE(BSP_VOID);  

/* T=1 add Begin:*/
/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_SETBWT
*
* 功能描述  : 本接口用于配置块等待时间
*
* 输入参数  : BSP_U32 nBWT :配置超时时间为BWT的nBWT倍
*
* 输出参数  : 无
*
* 返 回 值  : OK/ERROR
* 修改记录  : 2013年10月15日   liuyi  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_SETBWT(BSP_U32 nBWT);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_PROTOCOL_SWITCH
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
BSP_S32 DRV_USIMMSCI_PROTOCOL_SWITCH(PROTOCOL_MODE enSIMProtocolMode);

/*****************************************************************************
* 函 数 名  : BSP_SCI_BLK_Receive
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
BSP_S32 DRV_USIMMSCI_BLK_RCV(BSP_U8 *pu8Data,BSP_U32 *pulLength);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_RECORDLOG_READ
*
* 功能描述  : 本接口获取sim卡异常时定位信息
* 输入参数  : pucDataBuff: 获取定位信息的buffer
              ulMaxLength: 最大值，判断使用
*
* 返 回 值  : pulLength: 获取数据长度

*
* 返 回 值  : 无
*
* 修改记录  : 
*
*****************************************************************************/
BSP_S32  DRV_USIMMSCI_RECORDLOG_READ(unsigned char *pucDataBuff, unsigned int * pulLength, unsigned int ulMaxLength);
/* T=1 add End */

// sync V9R1; 2013-05-02
/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_GET_ERRNO
*
* 功能描述  : 本接口获取接口调用返回的错误码
* 输入参数  :  无
* 输出参数  :  无
*
* 返 回 值  : 错误码
*
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_GET_ERRNO(BSP_VOID);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_POWER_ON
*
* 功能描述  : 本接口打开SIM 卡供电电压
* 输入参数  : 无 
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 
*
*****************************************************************************/
BSP_VOID DRV_USIMMSCI_POWER_ON(BSP_VOID);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_POWER_OFF
*
* 功能描述  : 本接口关闭SIM 卡供电电压
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 
*
*****************************************************************************/
BSP_VOID DRV_USIMMSCI_POWER_OFF(BSP_VOID);

#ifdef __cplusplus
}
#endif

#endif


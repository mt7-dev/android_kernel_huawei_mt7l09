/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  wangxiandong
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2013年3月12日  v1.00  wangxiandong修改
*************************************************************************/

#ifndef __DRV_CIPHER_H__
#define __DRV_CIPHER_H__

#include "drv_comm.h"

/*************************CIPHER START**********************************/

/* 错误码 */
#define CIPHER_SUCCESS                  0

#define CHN_BUSY                    0
#define CHN_FREE                    1

/* CIPHER支持的算法种类,注意枚举值与寄存器设置值一致 */
typedef enum tagCIPHER_ALGORITHM_E
{
	CIPHER_ALG_NULL = 0,        /*无算法*/
    CIPHER_ALG_SNOW3G  = 1,     /* SNOW3G算法 */
    CIPHER_ALG_AES_128 = 2,     /* AES128算法 */
    CIPHER_ALG_AES_192 = 3,     /* AES192算法 */
    CIPHER_ALG_AES_256 = 4,     /* AES256算法 */
    CIPHER_ALG_ZUC = 5,	        /* ZUC算法 */
    CIPHER_ALG_BUTTOM
}CIPHER_ALGORITHM_E;

typedef enum tagKDF_SHA_KEY_SOURCE_E
{
    SHA_KEY_SOURCE_DDR,             /*  sha_key来源于input point指定的地址空间 */
    SHA_KEY_SOURCE_KEYRAM,       /*  sha_key来源于内部KeyRam */
    SHA_KEY_SOURCE_FOLLOW,       /*  sha_key来源于上次KDF操作的sha_key */ 
    SHA_KEY_SOURCE_RESULT,        /*  sha_key来源于上一次KDF操作的结果*/
    SHA_KEY_SOURCE_MAX
}KDF_SHA_KEY_SOURCE_E;

typedef enum tagKDF_SHA_S_SOURCE_E
{
    SHA_S_SOURCE_DDR,              /*  sha_s来源于input point指定的地址空间*/
    SHA_S_SOURCE_KEYRAM,        /*  sha_s来源于内部KeyRam */
    SHA_S_SOURCE_MAX
}KDF_SHA_S_SOURCE_E;

/**************************************************************************
  枚举定义
**************************************************************************/
enum CIPHER_SECURITY_CHANNEL_ENUM
{
    CIPHER_SECURITY_CHANNEL_0            = 0,                /*通道0*/
    CIPHER_SECURITY_CHANNEL_1            = 1,                /*通道1*/
    CIPHER_SECURITY_CHANNEL_2            = 2,                /*通道2*/
    CIPHER_SECURITY_CHANNEL_3            = 3,                /*通道3*/

    CIPHER_SECURITY_CHANNEL_BUTT
};

/* 通道号*/
#define LTE_SECURITY_CHANNEL_NAS    CIPHER_SECURITY_CHANNEL_2
#define LTE_SECURITY_CHANNEL_RRC    CIPHER_SECURITY_CHANNEL_2
#define LTE_SECURITY_CHANNEL_UL_SRB CIPHER_SECURITY_CHANNEL_1
#define LTE_SECURITY_CHANNEL_DL_SRB CIPHER_SECURITY_CHANNEL_1
#define LTE_SECURITY_CHANNEL_UL_DRB CIPHER_SECURITY_CHANNEL_0
#define LTE_SECURITY_CHANNEL_DL_DRB CIPHER_SECURITY_CHANNEL_3

/* CIPHER单独操作分类 */
typedef enum tagCIPHER_SINGLE_OPT_E
{
    /* 单独操作类型,单独枚举值与寄存器设置值一致 */
    CIPHER_OPT_ENCRYPT = 0x0,   /* 加密 */
    CIPHER_OPT_DECRYPT = 0x1,   /* 解密 */
    CIPHER_OPT_PROTECT_SMAC = 0x2,   /* 保护操作，输出MAC长度为2Byte */
    CIPHER_OPT_PROTECT_LMAC = 0x3,   /* 保护操作，输出MAC长度为4Byte */
    CIPHER_OPT_CHECK_PRE_SMAC  = 0x4,   /* 检查操作，MAC长度为2Byte，位置在数据前面 */
    CIPHER_OPT_CHECK_POST_SMAC = 0x5,   /* 检查操作，MAC长度为2Byte，位置在数据后面 */
    CIPHER_OPT_CHECK_PRE_LMAC  = 0x6,   /* 检查操作，MAC长度为4Byte，位置在数据前面 */
    CIPHER_OPT_CHECK_POST_LMAC = 0x7,   /* 检查操作，MAC长度为4Byte，位置在数据后面 */
    CIPHER_SINGLE_OPT_BUTTOM
}CIPHER_SINGLE_OPT_E;


/* CIPHER关联操作分类 */     /* PDCP 关联操作随意组合*/
typedef enum tagCHPHER_RELA_OPT_E
{
    /*  关联操作类型 */
    CIPHER_PDCP_PRTCT_ENCY,     /*PDCP保护加密(上行)*/
    CIPHER_PDCP_DECY_CHCK,      /*PDCP解密检查(下行)*/

    CIPHER_NAS_ENCY_PRTCT,      /* NAS层加密保护(上行) */
    CIPHER_NAS_CHCK_DECY,       /* NAS层检查解密(下行) */

	CIPHER_RELA_OPT_BUTTOM
}CIPHER_RELA_OPT_E;


/* CIPHER密钥长度, 注意枚举值与寄存器设置值一致 */
typedef enum tagCIPHER_KEY_LEN_E
{
    CIPHER_KEY_L128 = 0,        /* 密钥长度128 bit */
    CIPHER_KEY_L192 = 1,        /* 密钥长度192 bit */
    CIPHER_KEY_L256 = 2,        /* 密钥长度256 bit */
    CIPHER_KEY_LEN_BUTTOM
}CIPHER_KEY_LEN_E;


/* CIPHER操作提交类型 */
typedef enum tagCIPHER_SUBMIT_TYPE_E
{
    CIPHER_SUBM_NONE = 0,           /* 不作任何通知 */
    CIPHER_SUBM_BLK_HOLD = 1,       /* 循环查询等待完成 */
    CIPHER_SUBM_CALLBACK = 2,       /* 回调函数通知 */
    CIPHER_SUBM_BUTTOM
}CIPHER_SUBMIT_TYPE_E;

/* CIPHER通知类型 */
typedef enum tagCIPHER_NOTIFY_STAT_E
{
    CIPHER_STAT_OK = 0,           /* 成功完成 */
    CIPHER_STAT_CHECK_ERR = 1,    /* 完整性检查错误 */
    CIPHER_STAT_BUTTOM
}CIPHER_NOTIFY_STAT_E;

typedef enum CIPHER_HDR_BIT_ENUM
{
    CIPHER_HDR_BIT_TYPE_0              = 0,     /*对应的头或附加头字节长度为0，即不启动此功能*/
    CIPHER_HDR_BIT_TYPE_5              = 1,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低5位，参与保护*/
    CIPHER_HDR_BIT_TYPE_7              = 2,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低7位，参与保护*/
    CIPHER_HDR_BIT_TYPE_12             = 3,     /*对应的头或附加头字节长度为2，即启动此功能，对应附加头的内容是Count值的低12位，参与保护*/

    CIPHER_APPEND_HDR_BIT_TYPE_5       = 4,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低5位，不参与保护，只搬移*/
    CIPHER_APPEND_HDR_BIT_TYPE_7       = 5,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低7位，不参与保护，只搬移*/
    CIPHER_APPEND_HDR_BIT_TYPE_12      = 6,     /*对应的头或附加头字节长度为2，即启动此功能，对应附加头的内容是Count值的低12位，不参与保护，只搬移*/

    CIPHER_HDR_BIT_TYPE_BUTT
}CIPHER_HDR_E;

/* 算法配置信息 */
typedef struct tagCIHPER_ALG_INFO_S
{
    BSP_U32 u32KeyIndexSec;
    BSP_U32 u32KeyIndexInt;
    CIPHER_ALGORITHM_E enAlgSecurity;          /* 安全操作算法选择，AEC算法还是SNOW3G算法 */
    CIPHER_ALGORITHM_E enAlgIntegrity;         /* 完整性操作算法选择，AEC算法还是SNOW3G算法 */
}CIHPER_ALGKEY_INFO_S;


/*组包加速配置信息*/
typedef struct tagACC_SINGLE_CFG_S
{
    BSP_U32 u32BearId;
    BSP_U32 u32AppdHeaderLen;
    BSP_U32 u32HeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块 */
    BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    BSP_U32 u32Offset;                  /* 使用数据距离数据包起始地址偏移*/
    BSP_U32 u32OutLen;                  /* 使用到的数据长度*/
    BSP_U32 u32Aph;                      /*附加包头域*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 单独操作的算法密钥配置信息 */
} ACC_SINGLE_CFG_S;

/* CIPHER配置信息设置 */
typedef struct tagCIHPER_SINGLE_CFG_S
{
    /*CIPHER_SINGLE_OPT_E*/BSP_U32 enOpt;          /* 单独操作类型(纯DMA操作忽略下面的算法配置) */
    BSP_U8 u8BearId;
    BSP_U8 u8Direction;
    CIPHER_HDR_E enAppdHeaderLen;
    CIPHER_HDR_E enHeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块，不是为０，是为其他数 */
	BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 单独操作的算法密钥配置信息 */
} CIHPER_SINGLE_CFG_S;

typedef struct tagCIHPER_RELA_CFG_S
{
    /*CIPHER_RELA_OPT_E*/BSP_U32 enOpt;             /* 关联操作类型(纯DMA操作忽略下面的算法配置) */
    BSP_U8 u8BearId;
    BSP_U8 u8Direction;
    CIPHER_HDR_E enAppdHeaderLen;
    CIPHER_HDR_E enHeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块 */
	BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 关联操作第一步的算法密钥配置信息 */

} CIHPER_RELA_CFG_S;


typedef struct tagS_CONFIG_INFO_S
{
    KDF_SHA_S_SOURCE_E   enShaSSource;      /* sha_s来源*/
    BSP_U32              u32ShaSIndex;                /* sha_s来源于KeyRam时，其在keyRam中的索引*/
    BSP_U32              u32ShaSLength;              /* sha_s的长度*/
    BSP_VOID             *pSAddr;           
}S_CONFIG_INFO_S;

typedef enum tagCIPHER_KEY_OUTPUT_E
{
    CIPHER_KEY_NOT_OUTPUT,    /*不输出密钥*/
    CIPHER_KEY_OUTPUT,        /*输出密钥*/
    CIPHER_KEY_OUTPUT_BUTTOM
}CIPHER_KEY_OUTPUT_E;

typedef BSP_U32 CIPHER_KEY_LEN_E_U32;
typedef BSP_U32 CIPHER_KEY_OUTPUT_E_U32;

/* 获取Key长度和Key内容的结构体*/
typedef struct
{
    CIPHER_KEY_LEN_E_U32    enKeyLen; /* 要读取的Key的长度，由上层传入*/
    BSP_VOID                *pKeyAddr; /*Key值存放地址 */
    CIPHER_KEY_LEN_E_U32    *penOutKeyLen;/*该地址用于存放实际返回的Key的长度*/
}KEY_GET_S;

/* KeyMake时，获取Key的长度和Key值*/
typedef struct
{
    CIPHER_KEY_OUTPUT_E_U32 enKeyOutput; /*指示是否输出Key到stKeyGet中*/
    KEY_GET_S stKeyGet;
}KEY_MAKE_S;


/* 完成回调函数类型定义 */
/*
u32ChNum   :  通道号;
u32SourAddr:  源地址
u32DestAddr:  目的地址
enStatus   :  通知的状态,比如: 完成 / 数据传输出错 / 数据完成性检查出错
u32Private   :  用户私有数据;
*/
typedef BSP_VOID (*CIPHER_NOTIFY_CB_T)(BSP_U32 u32ChNum, BSP_U32 u32SourAddr,
              BSP_U32 u32DestAddr, CIPHER_NOTIFY_STAT_E enStatus, BSP_U32 u32Private);

typedef BSP_VOID (*CIPHER_FREEMEM_CB_T)(BSP_VOID* pMemAddr);

/**************************************************************************
  KDF相关
**************************************************************************/

typedef struct tagKEY_CONFIG_INFO_S
{
    KDF_SHA_KEY_SOURCE_E enShaKeySource;    /* sha_key来源*/
    BSP_U32              u32ShaKeyIndex;       /* sha_key来源于KeyRam时，其在keyRam中的索引8 */
    BSP_VOID             *pKeySourceAddr;   /* */
}KEY_CONFIG_INFO_S;


/*****************************************************************************
* 函 数 名  : BSP_Accelerator_DMA
*
* 功能描述  : 为指定通道配置描述符，用于单纯的DMA搬移
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*             pInMemMgr    : 输入buffer首地址
*             pOutMemMgr   : 输出buffer首地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_DMA(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,ACC_SINGLE_CFG_S *pstCfg);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_Cipher
*
* 功能描述  : 为指定通道配置描述符，用于需要Cipher进行加密的操作
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*             pInMemMgr    : 输入buffer首地址
*             pOutMemMgr   : 输出buffer首地址
*             pstCfg       : Cipher配置信息
* 输出参数   : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_Cipher(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
               ACC_SINGLE_CFG_S *pstCfg);


/**************************************************************************
 函数声明
**************************************************************************/

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_RegistNotifyCB
*
* 功能描述  : 注册通知回调函数
*
* 输入参数  : pFunNotifyCb: 通知回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistNotifyCB(CIPHER_NOTIFY_CB_T pFunNotifyCb);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_FreeMemCB
*
* 功能描述  : 注册Memory释放回调函数
*
* 输入参数  : u32Chn: 通道号
*             bSrc:   0:输入Buffer / 1:输出Bufffer
*             pFunFreeMemCb: Memory释放回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistFreeMemCB(BSP_U32 u32Chn, BSP_BOOL bSrc, CIPHER_FREEMEM_CB_T pFunFreeMemCb);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_Purge
*
* 功能描述  : 清除所有当前指定通道的CIPHER任务(阻塞接口,完成purge后返回)
*
* 输入参数  : u32Chn: 要清除的通道号
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_Purge(BSP_U32 u32Chn);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_SingleSubmitTask
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于单独操作。
*
* 输入参数  : u32Chn: 通道号
*             pInMemMgr: 输入buffer内存管理结构指针
*             pOutMemMgr:输出buffer内存管理结构指针
*             pstCfg: Cipher操作配置属性
*             pstSubmAttr:提交的属性
*             u32Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_SingleSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
                   CIHPER_SINGLE_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_RelaSubmitTask
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于关联操作。
*
* 输入参数  : u32Chn: 通道号
*             pInMemMgr: 输入buffer内存管理结构指针
*             pOutMemMgr:输出buffer内存管理结构指针
*             pstCfg: Cipher操作配置属性
*             enSubmAttr:提交的属性
*             u32Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RelaSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
                   CIHPER_RELA_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_SetKeyIndex
*
* 功能描述  : 设置密钥值
*
* 输入参数  : pKeyAddr   : 密钥地址
*             enKeyLen   : 密钥长度
*             u32KeyIndex: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_ENUM
*           :      CIPHER_INVALID_KEY
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetKey(const BSP_VOID* pKeyAddr,CIPHER_KEY_LEN_E enKeyLen,BSP_U32 u32KeyIndex);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_GetKey
*
* 功能描述  : 根据KeyIndex获取Key的地址
*
* 输入参数  : u32KeyIndex: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:根据KeyIndex得到的Key的地址
*           : 失败:获取Key值所在地址失败,返回0
*****************************************************************************/
BSP_S32 BSP_CIPHER_GetKey(BSP_U32 u32KeyIndex, KEY_GET_S *pstKeyGet);

/*****************************************************************************
* 函 数 名  : CIPHER_DataBuffPara
*
* 功能描述  : 获取上层数据信息并设置信息
*
* 输入参数  : u32Chn        : 通道号
*             bSrc          : TRUE:源地址的属性配置 / FALSE:目的地址属性配置
*             u32BufOft     : 数据buffer指针偏移
*             u32LenOft     : Buffer长度偏移
*             NextOft       : 下一节点偏移
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetDataBufPara(BSP_U32 u32Chn, BSP_BOOL bSrc, BSP_U32 u32BufOft,
                                 BSP_U32 u32LenOft, BSP_U32 u32NextOft);

/*****************************************************************************
* 函 数 名  : CIPHER_GetCmplSrcBuff
*
* 功能描述  : 获取已经操作完成的源数据地址
*
* 输入参数  : u32ChNum     : 通道号
* 输出参数  : pu32SourAddr : 操作完成的源地址;
*             pu32DestAddr : 操作完成的目的地址
*             penStatus    : 操作完成的状态
*             pu32Private  : 返回的私有数据
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_RD
*****************************************************************************/
BSP_S32 BSP_CIPHER_GetCmplSrcBuff(BSP_U32 u32Chn, BSP_U32 *pu32SourAddr,
             BSP_U32 *pu32DestAddr, CIPHER_NOTIFY_STAT_E *penStatus, BSP_U32 *pu32Private);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_GetBdFifoAddr
*
* 功能描述  : 用于获取当前可用的BDFIFO首地址
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : BDFIFO首地址
*****************************************************************************/
BSP_U32 BSP_Accelerator_GetBdFifoAddr(void);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_GetStatus
*
* 功能描述  : 获取当前通道状态
*
* 输入参数  :  无
* 输出参数   : 无
* 返 回 值  : 通道忙/空闲
*****************************************************************************/
BSP_S32 BSP_Accelerator_GetStatus(void);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_Enable
*
* 功能描述  : 使能组包加速
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_Enable(BSP_U32 u32BdFifoAddr);

/*****************************************************************************
* 函 数 名  : BSP_KDF_KeyRamRead()
*
* 功能描述  : 从Key Ram中读取数据
*
* 输入参数  : enKeyIndex: Key索引，从该索引处开始读取数据
*             u32Length : 读取数据长度，以Byte为单位
* 输出参数  : pDestAddr : 目的地址，将从KeyRam中读取的数据存储到该位置
* 返 回 值  : 成功: BSP_OK
*           : 失败: BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyRamRead(BSP_VOID *pDestAddr, BSP_U32 u32KeyIndex ,BSP_U32 u32ReadLength);

/*****************************************************************************
* 函 数 名  : BSP_KDF_KeyRamWrite
*
* 功能描述  : 向KeyRam中写入数据
*
* 输入参数  : enKeyIndex: Key索引，从该索引处开始写入数据
*             pSourAddr : 源地址，写入到KeyRam中的数据地址
*             u32Length : 数据长度
*
* 输出参数  : 无
* 返 回 值  : 成功: BSP_OK
*           : 失败: BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyRamWrite(BSP_U32 u32KeyIndex, BSP_VOID *pSourAddr, BSP_U32 u32Length);


/*****************************************************************************
* 函 数 名  : BSP_KDF_KeyMake
*
* 功能描述  : KDF计算，用于产生新的Key
*
* 输入参数  : stKeyCfgInfo: KDF运算时参数sha_key属性配置
            : stKeyCfgInfo: KDF运算时参数sha_s属性配置
            : enDestIndex : KDF运算产生的Key放置在KeyRam中的位置
* 输出参数  : 无
* 返 回 值  : 成功:BSP_OK
*           : 失败:BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyMake(KEY_CONFIG_INFO_S *pstKeyCfgInfo, S_CONFIG_INFO_S *pstSCfgInfo,
                        BSP_U32 u32DestIndex, KEY_MAKE_S *pstKeyMake);


BSP_S32 BSP_CIPHER_Disable(void);
BSP_S32 BSP_CIPHER_Enable(void);

/**************************************************************************
  宏定义
**************************************************************************/

/* 注意CIPHER错误码要为负值 */
#define CIPHER_ERROR_BASE               0x80000800

typedef enum tagCIPHER_ERR_CODE_E
{
    CIPHER_ERR_CODE_NULL_PTR = 1,
    CIPHER_ERR_CODE_NO_MEM ,
    CIPHER_ERR_CODE_NOT_INIT ,
    CIPHER_ERR_CODE_FIFO_FULL ,
    CIPHER_ERR_CODE_INVALID_CHN ,
    CIPHER_ERR_CODE_INVALID_OPT ,
    CIPHER_ERR_CODE_ALIGN_ERROR ,
    CIPHER_ERR_CODE_PURGING ,
    CIPHER_ERR_CODE_TIME_OUT,
    CIPHER_ERR_CODE_INVALID_ENUM,
    CIPHER_ERR_CODE_INVALID_RD,
    CIPHER_ERR_CODE_RD_NULL,
    CIPHER_ERR_CODE_INVALID_KEY,
    CIPHER_ERR_CODE_CHECK_ERROR,
    CIPHER_ERR_CODE_BDLEN_ERROR,
    CIPHER_ERR_CODE_INVALID_NUM,
    CIPHER_ERR_CODE_NO_KEY,
    CIPHER_ERR_CODE_KEYLEN_ERROR,
    ACC_FIFOS_WORK,
    CIPHER_ENABLE_FAIL
}CIPHER_ERR_CODE_E;

#define CIPHER_NULL_PTR      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NULL_PTR))
#define CIPHER_NO_MEM        ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_MEM))
#define CIPHER_NOT_INIT      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NOT_INIT))
#define CIPHER_FIFO_FULL     ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_FIFO_FULL))
#define CIPHER_INVALID_CHN   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_CHN))
#define CIPHER_INVALID_OPT   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_OPT))
#define CIPHER_ALIGN_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_ALIGN_ERROR))
#define CIPHER_PURGING       ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_PURGING))
#define CIPHER_TIME_OUT      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_TIME_OUT))
#define CIPHER_INVALID_ENUM  ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_ENUM))
#define CIPHER_INVALID_RD    ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_RD))
#define CIPHER_RDQ_NULL      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_RD_NULL))
#define CIPHER_INVALID_KEY   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_KEY))
#define CIPHER_CHECK_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_CHECK_ERROR))
#define CIPHER_BDLEN_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_BDLEN_ERROR))
#define CIPHER_INVALID_NUM   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_NUM))
#define CIPHER_NO_KEY        ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_KEY))
#define CIPHER_KEYLEN_ERROR  ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_KEYLEN_ERROR))
#define ACC_ALL_FIFO_WORK    ((BSP_S32)(CIPHER_ERROR_BASE|ACC_FIFOS_WORK))
#define CIPHER_ENABLE_FAILED ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ENABLE_FAIL))
#define CIPHER_UNKNOWN_ERROR ((BSP_S32)(CIPHER_ERROR_BASE|0xff))



/*************************CIPHER END **********************************/

#endif


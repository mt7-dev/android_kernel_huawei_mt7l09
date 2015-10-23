/*************************************************************************
*   版权所有(C) 1987-2013, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_dload.h
*
*   作    者 :  吴择淳
*
*   描    述 :  给出底软给OAM提供的升级接口
*
*   修改记录 :  2013-03-13  v1.00  吴择淳 创建
*************************************************************************/

#ifndef __DRV_DLOAD_H__
#define __DRV_DLOAD_H__

#include "drv_comm.h"
#include "drv_rfile.h"

/*************************DLOAD BEGIN*****************************/
/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/
/* 错误码 */
#define DLOAD_OK                    BSP_OK
#define DLOAD_ERROR                 BSP_ERROR
#define DLOAD_ERR_NOT_INITED        BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_MODULE_NOT_INITED)
#define DLOAD_ERR_NULL_PTR          BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_NULL_PTR)
#define DLOAD_ERR_INVALID_PARA      BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_INVALID_PARA)
#define DLOAD_ERR_ALLOC_FAILED      BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_BUF_ALLOC_FAILED)
#define DLOAD_ERR_FREE_FAILED       BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_BUF_FREE_FAILED)
#define DLOAD_ERR_RETRY_TIMEOUT     BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_RETRY_TIMEOUT)

#define DLOAD_ERR_NO_BACKUP         BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 1)  /* 无备份版本 */
#define DLOAD_ERR_OPEN_FAILED       BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 2)  /* 虚拟串口打开失败 */
#define DLOAD_ERR_SEM_CREAT         BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 3)  /* 信号量创建失败 */
#define DLOAD_ERR_ABORT             BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 4)  /* 升级终止 */
#define DLOAD_ERR_MODE_MISMATCH     BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 5)  /* 工作模式不匹配 */
#define DLOAD_ERR_INVALID_ATSTRING  BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 6)  /* AT字符串非法 */


#define MODE_GSM      0x01
#define MODE_CDMA     0x02
#define MODE_WCDMA    0x04


/*--------------------------------------------------------------*
 * 枚举定义                                                     *
 *--------------------------------------------------------------*/
enum UPDATE_STATUS_I
{
	DL_UPDATE = 0,
	DL_NOT_UPDATE = 1
};

typedef enum
{
	NORMAL_DLOAD = 0,	/*正常升级模式*/
	FORCE_DLOAD = 1    /*强制升级模式*/
}DLOAD_TYPE;

/* 下载模式枚举 */
typedef enum tagDLOAD_MODE_E
{
    DLOAD_MODE_DOWNLOAD = 0,
    DLOAD_MODE_NORMAL,
    DLOAD_MODE_DATA,
    DLOAD_MODE_MAX
}DLOAD_MODE_E;

/*--------------------------------------------------------------*
 * 函数指针类型定义                                             *
 *--------------------------------------------------------------*/
typedef BSP_U32 (*BSP_DLOAD_AtCallBack)( BSP_VOID* pDataIn, BSP_U32 ulLen);
typedef BSP_U32 (*BSP_DLOAD_NVGetInfoCb)( BSP_VOID* pDataOut, BSP_U32 ulLen);
typedef BSP_U32 (*BSP_DLOAD_NVSetInfoCb)( BSP_VOID* pDataIn, BSP_U32 ulLen);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetProductId
*
* 功能描述  : 获取产品名称字符串
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERR_INVALID_PARA    :输入参数非法
*             DLOAD_ERROR               :失败
*             其它                      :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetProductId(BSP_CHAR *str, BSP_U32 len);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetDloadType
*
* 功能描述  : 获取下载类型
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : 0 :正常模式
*             1 :强制模式
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetDloadType(BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCurMode
*
* 功能描述  : 设置当前工作模式
*
* 输入参数  : BSP_VOID
* 输出参数  : DLOAD_MODE_NORMAL     :正常模式
*             DLOAD_MODE_DATA       :数据模式
*             DLOAD_MODE_DOWNLOAD   :下载模式
*
* 返 回 值  : 无
*
* 其它说明  : AT模块调用
*             正常模式支持的有:
*                 BSP_DLOAD_GetCurMode(DLOAD_MODE_DOWNLOAD)
*             下载模式支持的有:
*                 BSP_DLOAD_GetCurMode(DLOAD_MODE_DATA)
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetCurMode(DLOAD_MODE_E eDloadMode);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCurMode
*
* 功能描述  : 获取当前工作模式
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : DLOAD_MODE_NORMAL     :正常模式
*             DLOAD_MODE_DATA       :数据模式
*             DLOAD_MODE_DOWNLOAD   :下载模式
*
* 其它说明  : 无
*
*****************************************************************************/
DLOAD_MODE_E BSP_DLOAD_GetCurMode(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SendData
*
* 功能描述  : 通过虚拟串口向PC端发送数据
*
* 输入参数  : pBuf      :数据缓冲区
*             u32Len    :数据缓冲区长度
* 输出参数  : pBuf      :数据缓冲区
*
* 返 回 值  : 无
*
* 其它说明  : pBuf必须保证cache line(32字节)对齐
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_SendData(BSP_CHAR *pBuf, BSP_U32 u32Len);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_AtProcReg
*
* 功能描述  : 注册AT处理函数
*
* 输入参数  : pFun
* 输出参数  : 无
*
* 返 回 值  : DLOAD_OK:成功
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_AtProcReg (BSP_DLOAD_AtCallBack pFun);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetSoftwareVer
* 功能描述  : 获取软件版本号
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
* 返 回 值  : BSP_ERROR     :输入参数非法
*             其它          :返回字符串长度
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetSoftwareVer(BSP_CHAR *str, BSP_U32 len);


/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetDloadVer
*
* 功能描述  : 查询下载协议版本号。该信息BSP固定写为2.0。
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERROR   :输入参数非法
*             其它          :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetDloadVer(BSP_CHAR *str, BSP_U32 len);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SetSoftLoad
*
* 功能描述  : 设置启动标志
*
* 输入参数  : BSP_BOOL bSoftLoad  :
*             BSP_FALSE :从vxWorks启动
*             BSP_TRUE  :从bootrom启动
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
extern BSP_VOID BSP_DLOAD_SetSoftLoad (BSP_BOOL bSoftLoad);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetSoftLoad
*
* 功能描述  : 获取启动标志
*
* 输入参数  : BSP_VOID  
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :从bootrom启动
*             BSP_FALSE :从vxWorks启动
*
* 其它说明  : 无
*
*****************************************************************************/
extern BSP_BOOL BSP_DLOAD_GetSoftLoad (BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SendData
*
* 功能描述  : 通过虚拟串口向PC端发送数据
*
* 输入参数  : pBuf      :数据缓冲区
*             u32Len    :数据缓冲区长度
* 输出参数  : pBuf      :数据缓冲区
*
* 返 回 值  : 无
*
* 其它说明  : pBuf必须保证cache line(32字节)对齐
*
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_SendData(BSP_CHAR *pBuf, BSP_U32 u32Len);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_AtProcReg
*
* 功能描述  : 注册AT处理函数
*
* 输入参数  : pFun  
* 输出参数  : 无
*
* 返 回 值  : DLOAD_OK:成功
*
* 其它说明  : 无
*
*****************************************************************************/

extern BSP_S32 BSP_DLOAD_AtProcReg (BSP_DLOAD_AtCallBack pFun);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetISOVer
*
* 功能描述  : 获取后台版本号字符串
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERROR   :输入参数非法
*             其它          :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetISOVer(BSP_CHAR *str, BSP_U32 len);
#define DRV_GET_CDROM_VERSION(pVersionInfo,ulLength)    BSP_DLOAD_GetISOVer(pVersionInfo, ulLength)

/*****************************************************************************
 函 数 名  : DRV_SET_UPDATA_FLAG
 功能描述  : 设置升级加载标志。
 输入参数  : flag：升级加载标志，
                   0：启动后进入bootrom，进行升级加载。
                   1：启动后不进入bootrom，正常启动。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE int DRV_SET_UPDATA_FLAG(int flag)
{
	return 0;
}

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SetCdromMarker
*
* 功能描述  : 设置设备形态标志
*
* 输入参数  : BSP_BOOL bCdromMarker  :
*             BSP_TRUE  :设置正常设备形态
*             BSP_FALSE :设置bootrom设备形态
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 当升级后台文件时，设置为上报正常设备形态，不恢复NV；
*             当升级前台文件时，设置为上报bootrom设备形态，恢复NV；
*
*****************************************************************************/
static INLINE BSP_VOID BSP_DLOAD_SetCdromMarker(BSP_BOOL bCdromMarker)
{
	return ;
}
#define DRV_SET_CDROM_FLAG(flag)    BSP_DLOAD_SetCdromMarker(flag)

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCdromMarker
*
* 功能描述  : 获取设备形态标志
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :上报正常设备形态
*             BSP_FALSE :上报bootrom设备形态
*
* 其它说明  : 当升级后台文件时，设置为上报正常设备形态，不恢复NV；
*             当升级前台文件时，设置为上报bootrom设备形态，恢复NV；
*
*****************************************************************************/
static INLINE BSP_BOOL BSP_DLOAD_GetCdromMarker (BSP_VOID)
{
	return BSP_TRUE;
}
#define DRV_GET_CDROM_FLAG()    BSP_DLOAD_GetCdromMarker()

#define NV_BACKUP_PARTION	"nvbackgu"
/********************************************************************************************************
 函 数 名  : BSP_DLOAD_NVBackupRead
 功能描述  : 从Flash中的NV项备份区读取数据，实现NV项的恢复功能。
 输入参数  : len：从NV项备份区起始处开始，需要读取的NV项长度（字节数），不超过1Block。
 输出参数  : pRamAddr：目的RAM地址，用于存放读出的NV项数据。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
static INLINE int BSP_DLOAD_NVBackupRead(unsigned char *pRamAddr, unsigned int len)
{
    if((BSP_NULL == pRamAddr) || (0 == len))
    {
        return -1;
    }

//    return BSP_mass_read(NV_BACKUP_PARTION, 0 , len, (void*)pRamAddr);
    return 0;
}
#define DRV_NVBACKUP_READ(pRamAddr,len)    BSP_DLOAD_NVBackupRead(pRamAddr, len)

/********************************************************************************************************
 函 数 名  : BSP_DLOAD_NVBackupWrite
 功能描述  : 将特定数据写入Flash中的NV项备份区，实现NV项的备份功能。
 输入参数  : pRamAddr：源RAM地址，用于存放需要写入的NV项数据。
             len：从NV项备份区起始处开始，需要写入的NV项长度（字节数），不超过1Block。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
static INLINE int BSP_DLOAD_NVBackupWrite(unsigned char *pRamAddr, unsigned int len)
{
    if((BSP_NULL == pRamAddr) || (0 == len))
    {
        return -1;
    }

//    return BSP_mass_write(NV_BACKUP_PARTION ,0, len, (void*)pRamAddr);
    return 0;
}
#define DRV_NVBACKUP_WRITE(pRamAddr, len)   BSP_DLOAD_NVBackupWrite(pRamAddr, len)

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetNVBackupFlag
*
* 功能描述  : 获取升级前是否进行NV备份标志
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :备份NV
*            BSP_FALSE :不备份NV
*
* 其它说明  : 此接口只对非一键式升级方式（SD升级/在线升级）有效，一键式升级会发AT命令设置
*
*****************************************************************************/
#define DRV_DLOAD_GETNVBACKUPFLAG() BSP_DLOAD_GetNVBackupFlag()


/********************************************************************************************************
 函 数 名  : NVBackupFlashDataWrite
 功能描述  : 实现FLASH 中NV备份恢复区从指定位置读功能。
 输入参数  : pRamAddr:源RAM地址
*         offset :从NV备份区0地址开始的偏移
*         len: 需要写入的长度
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
static INLINE int NVBackupFlashDataWrite(unsigned char* pRamAddr, unsigned int offset,unsigned int len)
{
	return 0;
}

#define DRV_NV_FLASH_WRITE(pRamAddr, offset,len) NVBackupFlashDataWrite(pRamAddr, offset,len)


/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetTFUpdateFlag
 功能描述  : 判断是否是TF卡升级
 输入参数  : None
 输出参数  : None
 返 回 值  : 返回1是TF升级，NV恢复成功，需要点蓝灯
             返回0不是TF升级，NV恢复成功，不需要点蓝灯

*****************************************************************************/
extern int BSP_DLOAD_GetTFUpdateFlag(void);
#define DRV_GET_TFUPDATE_FLAG()    BSP_DLOAD_GetTFUpdateFlag()

/*****************************************************************************
 函 数 名  : DRV_GET_DLOAD_VERSION
 功能描述  : Get dload version
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
static INLINE BSP_S32 DRV_GET_DLOAD_VERSION(BSP_U8 *str, int len)
{
	return 0;
}


/*****************************************************************************
 函 数 名  : DRV_GET_DLOAD_INFO
 功能描述  : Get dload infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/

static INLINE BSP_S32 DRV_GET_DLOAD_INFO(unsigned char atCmdBuf[], unsigned int dloadType)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : DRV_GET_AUTHORITY_VERSION
 功能描述  : Get Authority version
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
static INLINE BSP_S32 DRV_GET_AUTHORITY_VERSION(BSP_U8 *str, BSP_S32 len)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : DRV_GET_AUTHORITY_ID
 功能描述  : 获取下载鉴权协议Id
 输入参数  : unsigned char *buf
                          int len
 输出参数  : 无
 返 回 值  : 0:  操作成功；
                      -1：操作失败。
*****************************************************************************/
static INLINE BSP_S32 DRV_GET_AUTHORITY_ID(unsigned char *buf, BSP_S32 len)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetDloadNetMode
 功能描述  : Get net mode
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern BSP_S32  BSP_DLOAD_GetDloadNetMode(BSP_U32 *netMode);
#define DRV_GET_DLOAD_NETMODE(netMode)    BSP_DLOAD_GetDloadNetMode (netMode)

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetDloadFlashInfo
 功能描述  : Get dload flash infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
static INLINE BSP_S32 BSP_DLOAD_GetDloadFlashInfo(DLOAD_FLASH_STRU* pFlashInfo)
{
	return 0;
}

#define DRV_GET_DLOAD_FLASHINFO(pFlashInfo)    BSP_DLOAD_GetDloadFlashInfo(pFlashInfo)


/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetWebUIVersion
 功能描述  : 获得WEBUI 版本信息
 输入参数  : pVersionInfo: 存放返回的版本信息的内存地址
                           ulLength: 存放返回的版本信息的内存长度，目前固定
                           128
 输出参数  : pVersionInfo: 返回的版本信息的内存地址
 返 回 值  :  0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetWebUIVersion(BSP_CHAR *pVersionInfo, BSP_U32 u32Length);
#define DRV_GET_WEBUI_VERSION(pVersionInfo, u32Length) BSP_DLOAD_GetWebUIVersion(pVersionInfo,u32Length)


/*****************************************************************
* 函 数 名  : BSP_OM_SoftReboot
* 功能描述  : 获取软件版本号
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
* 返 回 值  : BSP_ERROR     :输入参数非法
*             其它          :返回字符串长度
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
******************************************************************/
BSP_VOID BSP_OM_SoftReboot(void);

/*****************************************************************************
* 函 数 名  : BSP_TFUP_CompleteDeal
* 功能描述  : TF卡升级完成后处理
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
extern void BSP_TFUP_CompleteDeal(void);
#define DRV_TFUP_COMPLETEDEAL() BSP_TFUP_CompleteDeal()

/*****************************************************************************
* 函 数 名  : BSP_TFUP_CompleteDeal
* 功能描述  : 在线升级完成后处理
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
void BSP_ONUP_CompleteDeal(void);
#define DRV_ONUP_COMPLETEDEAL() BSP_ONUP_CompleteDeal()

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetDashFlag
* 功能描述  : 设置是否只升级后台镜像
* 输入参数  : BSP_DLOAD_GetDashFlag
* 输出参数  : BSP_TRUE  :只升级后台镜像
*             BSP_FALSE :不只升级后台镜像
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetDashFlag(BSP_VOID);
#define DRV_DLOAD_GETDASHFLAG() BSP_DLOAD_GetDashFlag()

/*****************************************************************************
 函 数 名  : drvOnLineUpdateResult
 功能描述  : WebUI模块函数注册。
 输入参数  : 无
 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
extern void drvOnLineUpdateResult(void);
#define DRV_ONLINE_UPDATE_RESULT()    drvOnLineUpdateResult()
/*************************DLOAD END***********************************/

#endif


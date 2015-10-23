/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_MNTN_H__
#define __DRV_MNTN_H__

#include "drv_comm.h"
#include "drv_usb.h"
#include "drv_sd.h"
#include "product_config.h"

/*************************MNTN START**********************************/

#define OM_SAVE_EXCHFILE_ING            0
#define OM_SAVE_EXCHFILE_END            1

enum SECURE_SUPPORT_STATUS_I
{
    SECURE_NOT_SUPPORT = 0,
    SECURE_SUPPORT = 1
};

enum SECURE_ENABLE_STATUS_I
{
    SECURE_DISABLE = 0,
    SECURE_ENABLE = 1
};

typedef enum                /* CACHE_TYPE */
{
    PS_OSAL_INSTRUCTION_CACHE ,
    PS_OSAL_DATA_CACHE
} PS_OSAL_CACHE_TYPE_I;


#ifndef __SUPPORT_MAX_HSPA_CAPA_STRU
#define __SUPPORT_MAX_HSPA_CAPA_STRU
typedef struct
{
    unsigned char enHSDSCHSupport;          /*硬件是否支持HSDPA*/
    unsigned char ucHSDSCHPhyCategory;      /*硬件支持的最大HSDPA能力等级*/
    unsigned char enEDCHSupport;            /*硬件是否支持HSUPA*/
    unsigned char ucEDCHPhyCategory;        /*硬件支持的最大HSUPA能力等级*/
    unsigned int  reserved;                 /*预留*/
}SUPPORT_MAX_HSPA_CAPA_STRU;
#endif
/*****************************BSP_MspProcReg  begin******************************/
typedef enum tagMSP_PROC_ID_E
{
    OM_REQUEST_PROC = 0,
    OM_ADDSNTIME_PROC = 1,
    OM_PRINTF_WITH_MODULE = 2,
    OM_PRINTF = 3,

    OM_PRINTF_GET_MODULE_IDLEV = 4,
    OM_READ_NV_PROC = 5,
    OM_WRITE_NV_PROC = 6,
    OM_MNTN_ERRLOG = 7,

    MSP_PROC_REG_ID_MAX
}MSP_PROC_ID_E;

typedef void (*BSP_MspProc)(void);
/*****************************************************************************
* 函 数 名  : DRV_MSP_PROC_REG
*
* 功能描述  : DRV提供给OM的注册函数
*
* 输入参数  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
extern void DRV_MSP_PROC_REG(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);


/*****************************************************************************
 函 数 名  : BSP_MNTN_UartRecvCallbackRegister
 功能描述  : 提供上层应用程序注册数据接收回调函数指针的API接口函数。
 输入参数  : uPortNo：串口实例号。
             pCallback：回调函数指针。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/



static INLINE int BSP_MNTN_GetMaxFreeblockSize(void)
{

	return 0;
}


/*****************************************************************************
* 函 数 名  : BSP_HwIsSupportWifi
* 功能描述  : 打印硬件版本信息,打桩
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
BSP_BOOL BSP_HwIsSupportWifi(BSP_VOID);


/*****************************************************************************
 函 数 名  : BSP_PMU_LDOOFF
 功能描述  : 本接口用于关闭某路LDO的电压输出。
 输入参数  : ucLDO：指示某路LDO电压。取值范围为1～12。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项　：LDO6由硬件控制，软件不支持对LDO6的操作。如果对LDO1关闭，则系统会关机。
*****************************************************************************/
extern void BSP_PMU_LDOOFF(BSP_U8 u8LDO);
#define DRV_PM_LDO_OFF(u8LDO)                 BSP_PMU_LDOOFF(u8LDO)

/*****************************************************************************
 函 数 名  : BSP_PMU_LDOON
 功能描述  : 本接口用于设置打开某路LDO的电压输出。
 输入参数  : ucLDO：指示某路LDO电压。取值范围为2～12。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项　：不支持对LDO1，LDO6的操作。LDO1是在系统开机时自动打开，一旦关闭则整个系统会下电；LDO6是由硬件管脚控制。
*****************************************************************************/
extern void BSP_PMU_LDOON(BSP_U8 u8LDO);
#define DRV_PM_LDO_ON(u8LDO)                  BSP_PMU_LDOON(u8LDO)




/*****************************************************************************
 函 数 名  : BSP_MNTN_GetGsmPATemperature
 功能描述  : 获取GSM PA温度
 输入参数  : Temprature
             hkAdcTalble
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetGsmPATemperature(int *temperature, unsigned short *hkAdcTable);
#define DRV_GET_PA_GTEMP(temperature, hkAdcTable)   \
                BSP_MNTN_GetGsmPATemperature(temperature, hkAdcTable)

/*****************************************************************************
 函 数 名  : getWcdmaPATemperature
 功能描述  : 获取WCDMA PA温度
 输入参数  : Temprature
             hkAdcTalble
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetWcdmaPATemperature (int *temperature, unsigned short *hkAdcTable);
#define  DRV_GET_PA_WTEMP(temperature, hkAdcTable)  \
              BSP_MNTN_GetWcdmaPATemperature(temperature, hkAdcTable)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSIMTemperature
 功能描述  : 获取SIM卡温度
 输入参数  : pusHkAdcTable：电磁电压表
 输出参数  : pTemp：        指向SIM卡温度的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetSIMTemperature(int *plTemp, unsigned short *pusHkAdcTable);
#define DRV_GET_SIM_TEMP(plTemp, pusHkAdcTable)   \
               BSP_MNTN_GetSIMTemperature(plTemp, pusHkAdcTable)




/*****************************************************************************
 函 数 名  : BSP_MNTN_I2cSelfCheck
 功能描述  : 返回I2c自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
static INLINE int BSP_MNTN_I2cSelfCheck(void)
{
	return 0;
}
#define DRV_I2C_SELFCHECK()    BSP_MNTN_I2cSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_SpiSelfCheck
 功能描述  : 返回spi自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
static INLINE int BSP_MNTN_SpiSelfCheck(void)
{
	return 0;
}

#define DRV_SPI_SELFCHECK()    BSP_MNTN_SpiSelfCheck()

/*****************************************************************************
 函 数 名  : MNTN_RFGLockStateGet
 功能描述  : 读取GSM RF锁定状态。
 输入参数  : 无。
 输出参数  : Status：存放锁定状态的输出值，
                        0：TX或者RX被锁定。
                        1：TX、RX都没有被锁定；

 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE int BSP_MNTN_RFGLockStateGet(unsigned int *Status)
{
	return 0;
}
#define DRV_GET_RF_GLOCKSTATE(Status)    BSP_MNTN_RFGLockStateGet(Status)

/*****************************************************************************
 函 数 名  : BSP_MNTN_BootForceloadModeCheck
 功能描述  : 强制加载模式查询。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：强制加载；
             1：非强制加载。
*****************************************************************************/
static INLINE unsigned int BSP_MNTN_BootForceloadModeCheck(void)
{
    return 0;
}
#define DRV_BOOT_FORCELOAD_MODE_CHECK()    BSP_MNTN_BootForceloadModeCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_BootFlagConfig
 功能描述  : 配置Bootline数据中的启动标志为快速自启动、倒计时7秒自启动或手动启动。
 输入参数  : ulBootFlag：暂未定义。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE  int BSP_MNTN_BootFlagConfig(unsigned int ulBootFlag)
{
	return 0;
}

#define DRV_BOOTFLAG_CFG(ulBootFlag)    BSP_MNTN_BootFlagConfig(ulBootFlag)

/*****************************************************************************
 函 数 名  : BSP_MNTN_CheckArmTCM
 功能描述  : TCM检测
 输入参数  : 无。
 输出参数  : 无。
 返回值：   0xffff0000：检测成功，TCM正常
            地址：检测出错的地址（0－0x5fff）
            0xffffffff:检测失败

*****************************************************************************/
static INLINE unsigned int BSP_MNTN_CheckArmTCM(void)
{
    return 0;
}
#define DRV_CHECK_ARM_TCM()    BSP_MNTN_CheckArmTCM()

/*****************************************************************************
 函 数 名  : BSP_MNTN_Int4ToString
 功能描述  : 将寄存器的值转换为ASCII字符
 输入参数  : 寄存器的值，和转换字符的存储空间,最大空间不超过30byte
 输出参数  : 转换字符
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE int BSP_MNTN_Int4ToString(unsigned int value, char * string)
{
    return 0;
}
#define DRV_MEM_VERCONVERT(value,string)    BSP_MNTN_Int4ToString(value, string)

/*****************************************************************************
 函 数 名  : BSP_MNTN_HeapInfoGet
 功能描述  : 获取单板侧的内存信息。
 输入参数  : allocSize: 已经分配的堆内存大小，单位byte。
             totalSize: 堆内存总尺寸，单位byte。
 输出参数  : None
 返 回 值  : 0: 操作成功；
             -1：  操作失败。
*****************************************************************************/
static INLINE int BSP_MNTN_HeapInfoGet(unsigned int *allocSize, unsigned int *totalSize)
{
	return 0;
}
#define DRV_GET_HEAPINFO(allocSize, totalSize)    BSP_MNTN_HeapInfoGet(allocSize, totalSize)

/********************************************************************************************************
 函 数 名  : BSP_MNTN_WriteSysBackupFile
 功能描述  : 向FLASH备份ZSP、NV数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
static INLINE int BSP_MNTN_WriteSysBackupFile(void)
{
	return 0;
}
#define DRV_BACKUP_SYS_FILE()    BSP_MNTN_WriteSysBackupFile()

/*****************************************************************************
 函 数 名  : BSP_MNTN_TFUPIfNeedNvBackup
 功能描述  : 使用TF卡升级前通过该接口判断是否需要备份NV项.
 输入参数  : None
 输出参数  : None
 返 回 值  : 1:需要备份
             0:无需备份
*****************************************************************************/
static INLINE int BSP_MNTN_TFUPIfNeedNvBackup(void)
{
    return 0;
}
#define DRV_TF_NVBACKUP_FLAG()    BSP_MNTN_TFUPIfNeedNvBackup()

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSDDevName
 功能描述  : 获取设备列表中的SD设备名
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : SD_DEV_TYPE_STR结构指针。
*****************************************************************************/
static INLINE SD_DEV_TYPE_STR * BSP_MNTN_GetSDDevName(void)
{
	return 0;
}

#define DRV_GET_SD_PATH()     BSP_MNTN_GetSDDevName()

/*****************************************************************************
 函 数 名  : BSP_PROD_GetRecoverNvInfo
 功能描述  : 从底软获得当前需要覆盖的NV信息
 输入参数  : N/A

 输出参数  : ppNvInfo  - 指向NV信息结构体的数组
             pulNvNum  - 数组个数

 返 回 值  : 0：正确，非0: 失败
*****************************************************************************/
extern  int BSP_PROD_GetRecoverNvInfo(void **ppNvInfo, unsigned long *pulNvNum);
#define  DRV_GET_RECOVER_NV_INFO(ppNvInfo, pulNvNum)  \
                BSP_PROD_GetRecoverNvInfo(ppNvInfo, pulNvNum)


/*****************************************************************************
 函 数 名  : SDIO_read_write_blkdata
 功能描述  : SD卡读写接口
 输入参数  :    nCardNo     SD卡号
                dwBlkNo     块数
                nBlkCount   读写大小
                pbDataBuff  读写缓冲区
                nFlags      读/写
 输出参数  : 无。
 返回值：   0:  读写成功
            非0:错误码
*****************************************************************************/
static INLINE int BSP_MNTN_SDRWBlkData(int nCardNo,unsigned int dwBlkNo,
                   unsigned int nBlkCount, unsigned char *pbDataBuff,int nFlags)
{
    return 0;
}
#define DRV_SDMMC_BLK_DATA_RW(nCardNo,dwBlkNo,nBlkCount,pbDataBuff,nFlags)   \
             BSP_MNTN_SDRWBlkData(nCardNo,dwBlkNo,nBlkCount,pbDataBuff, nFlags)


/***********************************内存拷贝优化*****************************************/
/*****************************************************************************
 函 数 名  : __rt_memcpy
 功能描述  : 汇编版本的memcpy函数
 输入参数  : Dest :目的地址
             Src :源地址
             Count:拷贝数据的大小
 输出参数  : 无。
 返 回 值  : 目的地址。
*****************************************************************************/
#define DRV_RT_MEMCPY(Dest,Src,Count)       memcpy(Dest,Src,Count)

/*****************************************************************************
 函 数 名  : bsp_memcpy_align32
 功能描述  : 汇编版本的32位数据对齐memcpy函数
 输入参数  : Dest :目的地址
             Src :源地址
             Count:拷贝数据的大小
 输出参数  : 无。
 返 回 值  : 目的地址。
*****************************************************************************/
extern void bsp_memcpy_align32(UINT32 *pDstBuf,  UINT32 *pSrcBuf, UINT32 ulSize);
#define DRV_MEMCPY_ALIGN32(Dest,Src,Count)       bsp_memcpy_align32(Dest,Src,Count)

/*****************************************************************************
 函 数 名  : bsp_memcpy_auto_align64
 功能描述  : 自动按64位对齐版本的memcpy函数
 输入参数  : Dest :目的地址
             Src :源地址
             Count:拷贝数据的大小
 输出参数  : 无。
 返 回 值  : 目的地址。
*****************************************************************************/
extern void bsp_memcpy_auto_align(void *pDstBuf,  void *pSrcBuf, UINT32 ulLen, UINT32 ulAlignSize);
#define DRV_MEMCPY_AUTO_ALIGN64(Dest,Src,Count) bsp_memcpy_auto_align(Dest,Src,Count,8);


/************************************其余宏定义为空的函数******************************************/
/*****************************************************************************
 函 数 名  : UsbStatusCallbackRegister
 功能描述  : 记录pCallBack至全局变量中。SD卡功耗相关接口，V7先打桩
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
 /*extern int  UsbStatusCallbackRegister(pUsbState pCallBack);*/
#define DRV_USB_STATUS_CALLBACK_REGI(pCallBack)              DRV_OK

/*****************************************************************************
 函 数 名  : CicomClkEnable
 功能描述  : This routine enable CICOM IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void CicomClkEnable(void);*/
#define DRV_CICOM_CLK_ENABLE()

/*****************************************************************************
 函 数 名  : CicomClkDisable
 功能描述  : This routine disable CICOM IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void CicomClkDisable(void);*/
#define DRV_CICOM_CLK_DISABLE()

/*****************************************************************************
 函 数 名  : HdlcClkEnable
 功能描述  : This routine enable HDLC IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void HdlcClkEnable(void);*/
#define DRV_HDLC_CLK_ENABLE()

/*****************************************************************************
 函 数 名  : HdlcClkDisable
 功能描述  : This routine disable HDLC IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void HdlcClkDisable(void);*/
#define DRV_HDLC_CLK_DISABLE()

/*****************************************************************************
 函 数 名  : DRV_MEM_READ
 功能描述  : 按32位宽度查询内存，输出4字节数据。可维可测，V7先打桩
 输入参数  : ulAddress：查询地址空间地址，地址需在内存范围内，否则返回-1。
 输出参数  : pulData：指向查询内容的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE int DRV_MEM_READ(unsigned int ulAddress, unsigned int* pulData)
{
	return 0;
}

/*****************************************************************************
 函 数 名  : DrvLogInstall
 功能描述  : 打印函数注册。可维可测，V7先打桩
 输入参数  : fptr 注册的函数指针
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/*extern int DrvLogInstall(PVOIDFUNC fptr, PVOIDFUNC fptr1, PVOIDFUNC fptr2, PVOIDFUNC fptr3, PVOIDFUNC fptr4);*/
#define DRV_LOG_INSTALL(fptr, fptr1, fptr2, fptr3, fptr4)     DRV_OK

/*****************************************************************************
 函 数 名  : pwrctrl_wpa_pwr_up
 功能描述  : RF下电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void pwrctrl_wpa_pwr_up(void);
#define DRV_WPA_POWERUP()


/*****************************************************************************
 函 数 名  : BSP_DMR_ATAnalyze
 功能描述  : 供NAS查询动态内存占用信息，通过AT命令查询
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 成功0，失败-1
*****************************************************************************/
extern int BSP_DMR_ATANALYZE(void * pstMem,unsigned int uiSize, unsigned int * puiModIdSum);

static INLINE void * GET_SYSTEM_RESET_INFO(void)
{
    return 0;
}

#define EXCH_CB_NAME_SIZE           (32)
typedef struct
{
    char   aucName[EXCH_CB_NAME_SIZE];
    unsigned char *pucData;
    unsigned int  ulDataLen;
}cb_buf_t;

typedef int  (*exchCBReg)(cb_buf_t *);
static INLINE int DRV_EXCH_CUST_FUNC_REG(exchCBReg cb)
{
    return 0;
}
/*****************************************************************
Function: free_mem_size_get
Description:
    get mem size
Input:
    N/A
Output:
    N/A
Return:
    free mem size
*******************************************************************/
static INLINE unsigned long free_mem_size_get(void)
{
    return 0;
}
#define FREE_MEM_SIZE_GET() free_mem_size_get()

static INLINE BSP_U32 BSP_CPU_GetTotalUtilize(void)
{
    return 0;
}

static INLINE void BSP_ONOFF_DrvPowerOff(void)
{
    return;/*打桩*/
}

static INLINE unsigned short * BSP_MNTN_MemCheck16(void *pStartAddr, void *pEndAddr)
{
    return 0;
}

static INLINE void BSP_MNTN_StartTimeStamp(void)
{
    return ;
}

static INLINE void BSP_MNTN_VersionAndTimeForm(UINT8 * string, UINT8 length)
{
    return;
}

/*************************MNTN END************************************/

#endif



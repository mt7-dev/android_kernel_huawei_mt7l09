
#ifndef _DRV_NVIM_H_
#define _DRV_NVIM_H_

/*lint -save -e537*/
#include "drv_comm.h"
/*lint -restore +e537*/

#ifdef __cplusplus
extern "C"
{
#endif

/*debug add*/
/*****************************************************************/
/* NV恢复表类型 */
enum NV_RESUME_ITEM
{
    NV_MANUFACTURE_ITEM = 0,
    NV_USER_ITEM,
    NV_SECURE_ITEM,
    NV_ITEM_BUTT
};
typedef unsigned long NV_RESUME_ITEM_ENUM_UINT32;

/*****************************************************************/

typedef struct
{
    BSP_U32 ulfilelen;
    BSP_U8 *pdata;
}NVIM_EXPORT_FILE_INFO_STRU;


typedef enum
{
	EN_NVM_HSO_HNV_FILE = 0,      /*HSO新生成的文件*/
	EN_NVM_VERSION_FILE = 1,      /*当前版本使用的NV文件*/
    EN_NVM_BACKUP_FILE =2,           /*正常升级时做备份的文件*/
    EN_NVM_VERSION_INIT_FILE=3,     /*备份当前版本的初始值*/
    EN_NVM_FILE_LAST
} NVM_FILE_TYPE_ENUM;

/*****************************************************************************
 函 数 名  : NVM_Flush
 功能描述  : 将内存中数据刷新到FLASH或FILE接口
 输入参数  : NULL
 输出参数  :
 返 回 值  :

*****************************************************************************/
BSP_U32 NVM_Flush(BSP_VOID);


/*****************************************************************************
 函 数 名  : NVM_GetItemLength
 功能描述  : 获取NV项长度接口
 输入参数  :
 输出参数  :
 返 回 值  :

*****************************************************************************/
BSP_U32 NVM_GetItemLength(BSP_U16 ulid, BSP_U32 * pusitemlength);


/*****************************************************************************
 函 数 名  : NVM_UpgradeBackup
 功能描述  : 备份NV接口
 输入参数  : NULL
 输出参数  :
 返 回 值  :

*****************************************************************************/
BSP_U32 NVM_UpgradeBackup(BSP_U32 uloption);


/*****************************************************************************
 函 数 名  : NVM_UpgradeRestore
 功能描述  : 恢复NV接口
 输入参数  : NULL
 输出参数  :
 返 回 值  :

*****************************************************************************/
BSP_U32 NVM_UpgradeRestore(BSP_VOID);

/*****************************************************************************
 函 数 名  : NVM_SetFactoryDefault
 功能描述  : 恢复出厂NV接口
 输入参数  : NULL
 输出参数  :
 返 回 值  :

*****************************************************************************/
BSP_U32 NVM_SetFactoryDefault(BSP_VOID);


/*****************************************************************************
 函 数 名  : NVM_RevertFNV
 功能描述  : 恢复出厂NV接口
 输入参数  : NULL
 输出参数  :
 返 回 值  :

*****************************************************************************/
BSP_U32 NVM_RevertFNV(BSP_VOID);
/*****************************************************************************
 函 数 名  : NVM_BackUpFNV
 功能描述  : 备份出厂nv接口
 输入参数  : NULL
 输出参数  :
 返 回 值  :

*****************************************************************************/
BSP_U32 NVM_BackUpFNV(BSP_VOID);


/*****************************************************************************
 结构名    : NV_LIST_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 获取NV列表信息
*****************************************************************************/
typedef struct
{
    unsigned short usNvId;                  /*NVID值*/
    unsigned char  ucNvModemNum;            /*NV是否存在多份*/
    unsigned char  ucRsv;
}NV_LIST_INFO_STRU;

BSP_U32 NV_GetNVIdListNum(BSP_VOID);
BSP_U32 NV_GetNVIdList(NV_LIST_INFO_STRU *pstNvIdList);

/*enModemID :0(卡1) or 1(卡2)*/
BSP_U32 NV_ReadEx(BSP_U16  enModemID, BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength);
BSP_U32 NV_WriteEx(BSP_U16  enModemID,BSP_U16  usID,BSP_VOID   *pItem, BSP_U32  ulLength);
BSP_U32 NV_WritePartEx(BSP_U16   enModemID,BSP_U16   usID,BSP_U32  ulOffset,BSP_VOID *pItem,BSP_U32   ulLength);

BSP_U32 NV_Flush(BSP_VOID);
BSP_U32 NV_GetLength(BSP_U16 usID, BSP_U32 * pulLength);


#if defined (INSTANCE_1)
#define NV_Read(usID,pItem,ulLength)                NV_ReadEx(MODEM_ID_1,(usID),(pItem),(ulLength))

#define NV_ReadPart(usID,ulOffset,pItem,ulLength)   NV_ReadPartEx(MODEM_ID_1,(usID),(ulOffset),(pItem),(ulLength))

#define NV_Write(usID,pItem,ulLength)               NV_WriteEx(MODEM_ID_1,(usID),(pItem),(ulLength))

#define NV_WritePart(usID,ulOffset,pItem,ulLength)  NV_WritePartEx(MODEM_ID_1,(usID),(ulOffset),(pItem),(ulLength))

#define NVM_Read(usID,pItem,ulLength)                NV_ReadEx(MODEM_ID_1,(BSP_U16)(usID),(pItem),(BSP_U32)(ulLength))

#define NVM_Write(usID,pItem,ulLength)               NV_WriteEx(MODEM_ID_1,(BSP_U16)(usID),(pItem),(BSP_U32)(ulLength))

#else
#define NV_Read(usID,pItem,ulLength)                NV_ReadEx(MODEM_ID_0,(usID),(pItem),(ulLength))

#define NV_ReadPart(usID,ulOffset,pItem,ulLength)   NV_ReadPartEx(MODEM_ID_0,(usID),(ulOffset),(pItem),(ulLength))

#define NV_Write(usID,pItem,ulLength)               NV_WriteEx(MODEM_ID_0,(usID),(pItem),(ulLength))

#define NV_WritePart(usID,ulOffset,pItem,ulLength)  NV_WritePartEx(MODEM_ID_0,(usID),(ulOffset),(pItem),(ulLength))

#define NVM_Read(usID,pItem,ulLength)                NV_ReadEx(MODEM_ID_0,(BSP_U16)(usID),(pItem),(BSP_U32)(ulLength))

#define NVM_Write(usID,pItem,ulLength)               NV_WriteEx(MODEM_ID_0,(BSP_U16)(usID),(pItem),(BSP_U32)(ulLength))

#endif


BSP_U32 NV_GetResumeNvIdList(BSP_U32 enNvItem, BSP_U16 * pusNvList, BSP_U32 ulNvNum);
BSP_U32 NV_GetResumeNvIdNum(BSP_U32 enNvItem);

BSP_U32 NV_Backup(BSP_VOID);
BSP_U32 NV_BackupCheck(BSP_VOID);
BSP_U32 NV_RestoreAll(BSP_VOID);
BSP_U32 NV_RestoreManufacture(BSP_VOID);
BSP_U32 NV_RestoreResult(BSP_VOID);
BSP_U32 NV_RestoreManufactureDefault(BSP_VOID);
BSP_U32 NV_QueryRestoreResult(BSP_VOID);
BSP_U32 NV_Init(BSP_VOID);

BSP_U32 NV_SpecialNvIdBackup(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength);


/* gu nv stub */
int DRV_NV_SEC_CHECK(char* path);
int DRV_COPY_NVUSE_TO_NVBACKUP(void);
int DRV_COPY_NVUPGRADE_TO_NVUSE(void);

static INLINE int DRV_NV_FILE_SEC_CHECK(char * pnvFilePath)
{
    pnvFilePath = pnvFilePath;
    return 0;
}

static INLINE int DRV_UPGRADE_NV_SET_RESULT(int status)
{
    status = status;
    return 0;
}

static INLINE unsigned int NV_RestoreManufactureExt(void)
{
    return 1;/*返回1，当前不支持该功能，0表示成功*/
}


#define BSP_NVE_NAME_LENGTH          8       /*NV name maximum length*/
#define BSP_NVE_NV_DATA_SIZE        104     /*NV data maximum length*/

#define BSP_NV_READ                 1       /*NV read  operation*/
#define BSP_NV_WRITE                0       /*NV write operation*/

typedef struct nve_info_stru {
	BSP_U32 nv_operation;              /*0-write,1-read*/
	BSP_U32 nv_number;                 /*NV number you want to visit*/
	BSP_S8 nv_name[BSP_NVE_NAME_LENGTH];
	BSP_U32 valid_size;
	BSP_U8 nv_data[BSP_NVE_NV_DATA_SIZE];
}NVE_INFO_S;


BSP_S32 DRV_NVE_ACCESS(NVE_INFO_S *nve);


#ifdef __cplusplus
}
#endif

#endif  /*_DRV_NVIM_H_*/





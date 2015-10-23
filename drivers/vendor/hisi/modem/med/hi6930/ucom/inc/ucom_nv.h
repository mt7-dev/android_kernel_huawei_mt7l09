

#ifndef __UCOM_NV_H__
#define __UCOM_NV_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "NVIM_Interface.h"
#include "om_log.h"
#include "ucom_config.h"
#include "bsp_shared_ddr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* NV File Backup Info */
#define NV_BACKUP_FILE_MAX_NUM          (20)

#define NV_FILE_NAME_LEN                (28)

/* NV backup Buffer Size */
#define NV_BUFFER_SIZE                  (SHM_MEM_NV_SIZE)
#define NV_MAGIC_NUM                    (0x224e4944)
#if (VOS_CPU_TYPE == VOS_HIFI)

#define UCOM_NV_GetPhyAddr()            (g_ulNVMemAddrPhy)
#define UCOM_NV_DATA_BASE_ADDR          UCOM_NV_GetPhyAddr()
#endif

#define UCOM_SaveNvMemAddr(uwAddr)      (g_ulNVMemAddr = (uwAddr))              /* 保存NV备份区首地址(物理地址) */
#define UCOM_SaveNvMemAddrPhy(uwAddr)   (g_ulNVMemAddrPhy = (uwAddr))              /* 保存NV备份区首地址(物理地址) */

/* 设置全局变量g_pstUcomNvDataBuf指向NV文件块的起始地址 */
#define UCOM_SetNvDataBufPtr(uwAddr)            \
                (g_pstUcomNvDataBuf = (NV_CONTROL_FILE_INFO_STRU *)(uwAddr))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
typedef struct
{
    VOS_UINT32      ulOffset;
    VOS_UINT32      ulSize;
}NV_FILE_HANDLE_STRU;

/* NV Ctrl File Header Struct */
typedef struct
{
    VOS_UINT32  ulMagicNum;
    VOS_UINT32  ulCtrlFileSize;      /* ctrl file size */
    VOS_UINT16  usVersionInfo;       /* ctrl file version */
    VOS_UINT8   ucModemNum;          /* NV Supprot the Modem Number;1:one instance;2: two instance; */
    VOS_UINT8   ucRsv;
    VOS_UINT32  ulFileListOffset;    /* Offset of the File list start address */
    VOS_UINT32  ulFileListNum;       /* File num at file list */
    VOS_UINT32  ulFileListSize;      /* File list size */
    VOS_UINT32  ulNvRefOffset;       /* Offset of the NV reference data start address */
    VOS_UINT32  ulNvRefCount;        /* NV ID num */
    VOS_UINT32  ulNvRefSize;         /* NV reference data size */
    VOS_UINT32  aulRsv[3];
    VOS_UINT32  aulTimeTag[4];       /* the time of the file creating */
    VOS_UINT32  aulProductInfo[8];   /* product version info */
}NV_CONTROL_FILE_INFO_STRU;

/* NV File List Struct */
typedef struct
{
    VOS_UINT32  ulFileId;                      /* NV File ID */
    VOS_UINT8   aucFileName[NV_FILE_NAME_LEN]; /* NV File Name, the max name len is 28 including file type */
    VOS_UINT32  ulFileSize;                    /* NV File size */
}NV_FILE_LIST_INFO_STRU;

/* 函数多参数提取成结构体 */
typedef struct
{
    VOS_UINT16  usFileId;   /* File ID */
    VOS_UINT8   ucModemNum; /* NV Data Number,0: only one value, 1: support dual modem, two values */
    VOS_UINT8   ucRsv;      /* RSV */
    VOS_UINT8   aucFileName[NV_FILE_NAME_LEN]; /* NV id name  */
    VOS_UINT32  ulNvOffset;                    /* Offset of the NV ID start address at the Common NV file */
    VOS_UINT32  ulTotalOffset;                 /* 相对缓存中NV头位置 */
    VOS_UINT16  usProperty;
    VOS_UINT16  usRsv;
}NV_ID_RETUEN_INFO_STRU;

/* NV reference Data Struct */
typedef struct
{
    VOS_UINT16  usNvId;     /* NV ID */
    VOS_UINT16  usNvLength; /* Single NV Length */
    VOS_UINT32  ulNvOffset; /* Offset of the NV ID start address at the Common NV file */
    VOS_UINT16  usFileId;   /* File ID */
    VOS_UINT16  usProperty; /* NV property.just bit0 - bit3 in use. 0:write immediately; 7:write after power off;others reserve */
    VOS_UINT8   ucModemNum; /* NV Supprot the Modem Number;1:one instance;2: two instance; */
    VOS_UINT8   aucRsv[3];  /* RSV */
}NV_REFERENCE_DATA_INFO_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 UCOM_NV_Init(VOS_VOID);
extern VOS_UINT32 UCOM_NV_Read(VOS_UINT16 uhwId, VOS_VOID *pItem, VOS_UINT32 uwLength);

extern VOS_VOID   UCOM_NVIM_MemCpy( VOS_VOID * Dest, const VOS_VOID * Src,  VOS_UINT32 ulnbytes);
extern VOS_VOID   UCOM_NV_MemCpy( VOS_VOID * Dest, const VOS_VOID * Src,  VOS_UINT32 ulnbytes);
extern VOS_UINT32 UCOM_NV_Ctrl_File_Search_InMemory(VOS_UINT16                  usID,
                                                NV_CONTROL_FILE_INFO_STRU   *pstNVCtrlInfo,
                                                NV_FILE_LIST_INFO_STRU      *pstFileListInfo,
                                                NV_REFERENCE_DATA_INFO_STRU *pstNvIdInfo);
extern VOS_UINT32 UCOM_NV_NvIdCheck(MODEM_ID_ENUM_UINT16               enModemID,
                        VOS_UINT16                              usID,
                        VOS_UINT32                              ulOffset,
                        VOS_VOID                               *pItem,
                        VOS_UINT32                              ulLength,
                        NV_ID_RETUEN_INFO_STRU                 *pstNvIdReturnInfo);
extern VOS_UINT32 UCOM_NV_ReadPartEx(MODEM_ID_ENUM_UINT16              enModemID,
                                VOS_UINT16                      usID,
                                VOS_UINT32                      ulOffset,
                                VOS_VOID                        *pItem,
                                VOS_UINT32                      ulLength);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of ucom_nv.h */

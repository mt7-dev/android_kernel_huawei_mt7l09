

/*lint -save -e537*/
#include "bsp_nvim.h"
#include "drv_comm.h"
#include "nv_comm.h"
#include "NVIM_ResumeId.h"
/*lint -restore*/
#ifdef __cplusplus
extern "C"
{
#endif

#define  NV_RESULT_CODE            0xffff
/*lint -save -e958 -e438*/
#ifdef CONFIG_NVIM
/*lint -save -e18*/

BSP_U32 NVM_Flush(BSP_VOID)
{
    return bsp_nvm_flush() & NV_RESULT_CODE;
}



BSP_U32 NVM_UpgradeBackup(BSP_U32 uloption)
{
    /* [false alarm]:uloption is never using */
    uloption = uloption;
    return 0;
}


BSP_U32 NVM_UpgradeRestore(BSP_VOID)
{
    return 0;
}


BSP_U32 NVM_GetItemLength(BSP_U16 ulid, BSP_U32 * pusitemlength)
{
    BSP_U32 len = 0;
    BSP_U32 ret = 0;
    ret = bsp_nvm_get_len(ulid,&len);
    if(ret)
    {
        return NV_ERROR;
    }
    *pusitemlength = len;
    return 0;
}


BSP_U32 NVM_SetFactoryDefault(BSP_VOID)
{
    return 0;
}

BSP_U32 NVM_BackUpFNV(BSP_VOID)
{
    return 0;
}

BSP_U32 NVM_RevertFNV(BSP_VOID)
{
    return 0;
}

BSP_U32 NV_Flush(BSP_VOID)
{
    return bsp_nvm_flush() & NV_RESULT_CODE;
}

BSP_U32 NV_GetLength(BSP_U16 usID, BSP_U32 *pulLength)
{
    BSP_U32 len = 0;
    BSP_U32 ret = 0;
    ret = bsp_nvm_get_len(usID,&len);
    if(ret)
    {
        return NV_ERROR;
    }
    *pulLength = len;
    return 0;
}
BSP_U32 NV_Backup(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_BackupCheck(BSP_VOID)
{
    return 0;/*TO DO*/
}
BSP_U32 NV_RestoreAll(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_RestoreManufactureDefault(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_RestoreManufacture(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_RestoreResult(BSP_VOID)
{
    BSP_U32 ret = NV_ERROR;
    BSP_U16 resume_flag = 0;

    ret = bsp_nvm_read(NV_ID_DRV_RESUME_FLAG,(BSP_U8*)&resume_flag,sizeof(BSP_U16)); /*to do*/
    if(ret)
    {
        return ret;
    }
    if(0 != resume_flag)
    {
        return NV_ERROR;
    }
    return 0;
}
BSP_U32 NV_QueryRestoreResult(BSP_VOID)
{
    BSP_U32 ret = NV_ERROR;
    BSP_U16 resume_flag = 0;

    ret = bsp_nvm_read(NV_ID_DRV_RESUME_FLAG,(BSP_U8*)&resume_flag,sizeof(BSP_U16)); /*to do*/
    if(ret)
    {
        return ret;
    }
    if(0 != resume_flag)
    {
        return NV_ERROR;
    }
    return 0;
}

BSP_U32 NV_Open_ALL_NV_File(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_Close_ALL_NV_File(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_GetResumeNvIdNum(BSP_U32 enNvItem)
{
    return bsp_nvm_getRevertNum( enNvItem);
}
BSP_U32 NV_GetResumeNvIdList(BSP_U32 enNvItem, BSP_U16 *pusNvList, BSP_U32 ulNvNum)
{
    if (NULL == pusNvList)
    {
        return NV_ERROR;
    }

    if (ulNvNum < NV_GetResumeNvIdNum(enNvItem))
    {
        return NV_ERROR;
    }

    if (NV_MANUFACTURE_ITEM == enNvItem)
    {
        memcpy(pusNvList, g_ausNvResumeManufactureIdList,NV_GetResumeNvIdNum(enNvItem)*(sizeof(BSP_U16)));
        return NV_OK;
    }

    if (NV_USER_ITEM == enNvItem)
    {
        memcpy(pusNvList, g_ausNvResumeUserIdList,NV_GetResumeNvIdNum(enNvItem)*(sizeof(BSP_U16)));
        return NV_OK;
    }

    if (NV_SECURE_ITEM == enNvItem)
    {
        memcpy(pusNvList, g_ausNvResumeSecureIdList, NV_GetResumeNvIdNum(enNvItem)*(sizeof(BSP_U16)));
        return NV_OK;
    }

    return NV_OK;
}

int DRV_NV_SEC_CHECK(char* path)
{
    path = path;
    return 0;
}
int DRV_COPY_NVUSE_TO_NVBACKUP(void)
{
    return 0;
}
int DRV_COPY_NVUPGRADE_TO_NVUSE(void)
{
    return 0;
}

BSP_U32 NV_Init(BSP_VOID)
{
    return 0;
}

BSP_U32 NV_SpecialNvIdBackup(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength)
{
     /*[false alarm]:Value Never Read*/
    usID = usID;
     /*[false alarm]:Value Never Read*/
    pItem = pItem;
     /*[false alarm]:Value Never Read*/
    ulLength = ulLength;
    return 0;
}
BSP_U32 NV_GetNVIdListNum(BSP_VOID)
{
    return bsp_nvm_get_nv_num();
}
BSP_U32 NV_GetNVIdList(NV_LIST_INFO_STRU *pusNvIdList)
{
    if(pusNvIdList == NULL)
    {
        return NV_ERROR;
    }
    return bsp_nvm_get_nvidlist(pusNvIdList);
}


BSP_U32 NV_ReadEx(BSP_U16  enModemID, BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength)
{
    BSP_U32 card_id = 0;
    if(enModemID == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(enModemID == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else
        return BSP_ERR_NV_INVALID_PARAM;
    return bsp_nvm_dcread(card_id, usID,(BSP_U8*)pItem,ulLength);
}

BSP_U32 NV_WriteEx(BSP_U16  enModemID,BSP_U16  usID,BSP_VOID   *pItem, BSP_U32  ulLength)
{
    BSP_U32 card_id = 0;
    if(enModemID == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(enModemID == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else
        return BSP_ERR_NV_INVALID_PARAM;

    return bsp_nvm_dcwrite(card_id, usID,(BSP_U8*)pItem,ulLength);
}
BSP_U32 NV_ReadPartEx(BSP_U16 enModemID,BSP_U16 usID,BSP_U32 ulOffset,BSP_VOID *pItem,BSP_U32 ulLength)
{
    BSP_U32 card_id = 0;
    if(enModemID == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(enModemID == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else
        return BSP_ERR_NV_INVALID_PARAM;

    return bsp_nvm_dcreadpart(card_id,usID,ulOffset,(BSP_U8*)pItem,ulLength);
}
BSP_U32 NV_WritePartEx(BSP_U16   enModemID,BSP_U16   usID,BSP_U32  ulOffset,BSP_VOID *pItem,BSP_U32    ulLength)
{
    BSP_U32 card_id = 0;
    if(enModemID == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(enModemID == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else
        return BSP_ERR_NV_INVALID_PARAM;
    return bsp_nvm_dcwritepart(card_id,usID,ulOffset,(BSP_U8*)pItem,ulLength);
}

BSP_S32 DRV_NVE_ACCESS(NVE_INFO_S *nve)
{
    nve = nve;
    return 0;
}
/*lint -restore +e18*/
#else

#include <osl_common.h>
u32 bsp_nvm_read(u32 itemid, u8 * pdata, u32 datalen)
{
    return 0;
}

u32 bsp_nvm_write(u32 itemid, u8 * pdata, u32 datalen)
{
    return 0;
}
BSP_U32 NVM_Read(BSP_U32 ulid, BSP_VOID * pdata, BSP_U32 usdatalen)
{
    return 0;
}


BSP_U32 NVM_Write(BSP_U32 ulid, BSP_VOID * pdata, BSP_U32 usdatalen)
{
    return 0;
}


BSP_U32 NVM_Flush(BSP_VOID)
{
    return 0;
}



BSP_U32 NVM_UpgradeBackup(BSP_U32 uloption)
{
    return bsp_nvm_backup();
}


BSP_U32 NVM_UpgradeRestore(BSP_VOID)
{
    return 0;
}


BSP_U32 NVM_GetItemLength(BSP_U16 ulid, BSP_U32 * pusitemlength)
{
    return 0;
}


BSP_U32 NVM_SetFactoryDefault(BSP_VOID)
{
    return 0;
}

BSP_U32 NVM_BackUpFNV(BSP_VOID)
{
    return 0;
}

BSP_U32 NVM_RevertFNV(BSP_VOID)
{
    return 0;
}


BSP_U32 NV_Read(BSP_U16 ulid, BSP_VOID * pdata, BSP_U32 usdatalen)
{
    return 0;
}

BSP_U32 NV_ReadPart(BSP_U16 usID,BSP_U32 ulOffset,
                                BSP_VOID *pItem,BSP_U32 ulLength)
{
    return 0;
}
BSP_U32 NV_Write(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength)
{
    return 0;
}
BSP_U32 NV_WritePart(BSP_U16 usID,BSP_U32 ulOffset,
                            BSP_VOID *pItem,BSP_U32 ulLength)
{
    return 0;
}

BSP_U32 NV_Flush(BSP_VOID)
{
    return 0;
}

BSP_U32 NV_GetLength(BSP_U16 usID, BSP_U32 *pulLength)
{
    return 0;
}
BSP_U32 NV_Backup(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_BackupCheck(BSP_VOID)
{
    return 0;/*TO DO*/
}
BSP_U32 NV_RestoreAll(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_RestoreManufactureDefault(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_RestoreManufacture(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_RestoreResult(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_QueryRestoreResult(BSP_VOID)
{
    return 0;
}

BSP_U32 NV_Open_ALL_NV_File(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_Close_ALL_NV_File(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_Read_Direct(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength)
{
    return 0;
}
BSP_U32 NV_Write_Direct(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength)
{
    return 0;
}
#if 1
BSP_U32 NV_GetResumeNvIdNum(BSP_U32 enNvItem)
{
    return 0;
}
BSP_U32 NV_GetResumeNvIdList(BSP_U32 enNvItem, BSP_U16 *pusNvList, BSP_U32 ulNvNum)
{
    return 0;
}
#endif



BSP_U32 nvim_DiagDownLoad(BSP_VOID * pfile, BSP_U32 ulfilelen)
{
    return 0;
}

BSP_U32 nvim_GetInitExportFileInfo(NVIM_EXPORT_FILE_INFO_STRU * pexportfileInfo)
{
    if((NULL == pexportfileInfo)
        ||(NULL == pexportfileInfo->pdata))
    {
        return NV_ERROR;
    }
    return 0;
}
BSP_U32 nvim_GetWorkExportFileInfo(NVIM_EXPORT_FILE_INFO_STRU * pexportfileInfo)
{
    if((NULL == pexportfileInfo)
        ||(NULL == pexportfileInfo->pdata))
    {
        return NV_ERROR;
    }
    return 0;
}



int DRV_NV_SEC_CHECK(char* path)
{
    return 0;
}
int DRV_COPY_NVUSE_TO_NVBACKUP(void)
{
    return 0;
}
int DRV_COPY_NVUPGRADE_TO_NVUSE(void)
{
    return 0;
}
BSP_U32 NV_Init(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_SpecialNvIdBackup(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength)
{
    return 0;
}
BSP_U32 NV_GetNVIdListNum(BSP_VOID)
{
    return 0;
}
BSP_U32 NV_GetNVIdList(NV_LIST_INFO_STRU *pusNvIdList)
{
    if(pusNvIdList == NULL)
    {
        return NV_ERROR;
    }
    return 0;
}
#if 0
int  DRV_UPGRADE_NV_SET_RESULT(int status)
{
    return 0;
}
#endif

BSP_U32 NV_ReadEx(BSP_U16  enModemID, BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength)
{
    return 0;
}

BSP_U32 NV_WriteEx(BSP_U16  enModemID,BSP_U16  usID,BSP_VOID   *pItem, BSP_U32  ulLength)
{

    return 0;
}

BSP_U32 NV_WritePartEx(BSP_U16   enModemID,BSP_U16   usID,BSP_U32  ulOffset,BSP_VOID *pItem,BSP_U32    ulLength)
{
    return 0;
}

BSP_S32 DRV_NVE_ACCESS(NVE_INFO_S *nve)
{
    return 0;
}

#endif

/*lint -restore*/

#ifdef __cplusplus
}
#endif


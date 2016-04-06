

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*lint -e767 修改人:甘兰 47350;原因:Log打印 */
#define THIS_FILE_ID PS_FILE_ID_LITTLE_IMAGE_LOG_C
/*lint +e767 修改人:甘兰 47350;*/

#include "product_config.h"

#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)

/********************头文件****************************************************/
#include "vos.h"
#include "pslogadapter.h"
#include "FileSysInterface.h"
#include "omprivate.h"
#include "pslog.h"
#include "DrvInterface.h"
#include "NVIM_Interface.h"
#include "OmLittleImageLog.h"

/******************全局变量声明************************************************/



/* LOG文件保存控制信息 */
LOG_FILE_SAVE_INFO_STRU g_stLogFileSaveInfo;

/* LOG目录 */
#if (VOS_WIN32==VOS_OS_VER)
VOS_CHAR g_acFileSystemRoot[] = "";
VOS_CHAR g_acLogFileSaveDir[LOG_FILE_PATH_MAX_LENGTH] = "/Log";
VOS_CHAR g_acLogFileName[] = "/Log/LOG";
#else
VOS_CHAR g_acFileSystemRoot[] = MANUFACTURE_ROOT_PATH;
VOS_CHAR g_acLogFileSaveDir[LOG_FILE_PATH_MAX_LENGTH] = "/manufacture/Log";
VOS_CHAR g_acLogFileName[] = "/manufacture/Log/LOG";
#endif

/* LOG文件句柄 */
FILE  *g_lFileHandle = VOS_NULL_PTR;

VOS_UINT32  g_ulLittleImgLogErr = LOG_OPERATION_OK;

VOS_UINT32 g_ulLogFileSaveFlg = LOG_FILE_DISABLE;

extern FILE* OM_Open(const VOS_CHAR *pcFileName, VOS_INT lFlag);
VOS_UINT32 OM_LittleImgFilterMsg(VOS_UINT8 *pucMsg)
{
    OM_APP_TRACE_STRU       *pstAppTrace;
    VOS_UINT8               ucFuncType;

    if (VOS_NULL_PTR == pucMsg)
    {
        return VOS_ERR;
    }

    pstAppTrace = (OM_APP_TRACE_STRU*)pucMsg;

    ucFuncType = pstAppTrace->stAppHeader.ucFuncType;

    /* 非信令BT校准，G模下需要上报消息给ATE */
    if ((ucFuncType == OM_TRANS_FUNC) &&(pstAppTrace->usPrimId == OM_APP_TRACE_IND))
    {
        return VOS_OK;
    }

    /* 过滤消息FUNCTYPE */
    if ((ucFuncType == OM_WRF_FUNC) || (ucFuncType == OM_GRF_FUNC) ||
        (ucFuncType == OM_QUERY_FUNC) || (ucFuncType == OM_NV_FUNC) ||
        (ucFuncType == OM_NOSIG_FUNC) || (ucFuncType == OM_AUTH_NV_FUNC) ||
        (ucFuncType == OM_FS_FUNC))
    {
        return VOS_OK;
    }
#if (VOS_WIN32==VOS_OS_VER)
    return VOS_OK;
#endif
    return VOS_ERR;
}


VOS_UINT32 OM_LittleImgCloseLogFile()
{
    if (VOS_NULL_PTR != g_lFileHandle)
    {
        DRV_FILE_CLOSE(g_lFileHandle);

        g_lFileHandle = VOS_NULL_PTR;

        return VOS_OK;
    }

    return VOS_ERR;
}


VOS_VOID OM_LittleImgRemoveLogFile(VOS_UINT32 ulFileId)
{
    VOS_CHAR acFileName[LOG_FILE_PATH_MAX_LENGTH] = {0};
	LOG_FILE_DISK_INFO_STRU stDiskInfo;

    /* 拼接LOG文件名称 */
    VOS_sprintf((VOS_CHAR *)acFileName, "%s_%04d",
            g_acLogFileName,
            ulFileId);

    DRV_FILE_RMFILE(acFileName);

    /* 更新全局信息 */
    g_stLogFileSaveInfo.ulFileMinId++;

    /* 调用底软接口返回 */
    if (DRV_ERROR == DRV_FILE_GET_DISKSPACE(
                    g_acFileSystemRoot,
                    (VOS_UINT*)&stDiskInfo.ulDiskSpace,
                    (VOS_UINT*)&stDiskInfo.ulUsedSpace,
                    (VOS_UINT*)&stDiskInfo.ulValidSpace))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_LittleImgRemoveLogFile: DRV_FILE_GET_DISKSPACE Failed!");
        return;
    }

    g_stLogFileSaveInfo.ulFSFreeSpace = stDiskInfo.ulValidSpace;

    return;
}
VOS_UINT32 OM_LittleImgCreateLogFile(VOS_UINT32 ulFileSize)
{
    VOS_CHAR acFileName[LOG_FILE_PATH_MAX_LENGTH] = {0};
    LOG_FILE_DISK_INFO_STRU stDiskInfo;

    /* 调用底软接口返回 */
    if (DRV_ERROR == DRV_FILE_GET_DISKSPACE(
                    g_acFileSystemRoot,
                    (VOS_UINT*)&stDiskInfo.ulDiskSpace,
                    (VOS_UINT*)&stDiskInfo.ulUsedSpace,
                    (VOS_UINT*)&stDiskInfo.ulValidSpace))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_LittleImgRemoveLogFile: DRV_FILE_GET_DISKSPACE Failed!");
        return VOS_ERR;
    }

    g_stLogFileSaveInfo.ulFSFreeSpace = stDiskInfo.ulValidSpace;

    /*文件系统已满无法创建LOG文件*/
    while ((ulFileSize >= g_stLogFileSaveInfo.ulFSFreeSpace) &&
            (g_stLogFileSaveInfo.ulFileMinId != g_stLogFileSaveInfo.ulFileMaxId))
    {
        /*删除最老的LOG文件*/
        OM_LittleImgRemoveLogFile(g_stLogFileSaveInfo.ulFileMinId);
    }

    /* 以最大的ID创建文件名称 */
    VOS_sprintf((VOS_CHAR *)acFileName, "%s_%04d",
            g_acLogFileName,
            g_stLogFileSaveInfo.ulFileMaxId);

    g_lFileHandle = OM_Open((VOS_CHAR *)acFileName, DRV_O_CREAT|DRV_O_TRUNC|DRV_O_RDWR);

    if (DRV_FILE_NULL == g_lFileHandle)
    {
        g_ulLittleImgLogErr = LOG_OPERATION_OPEN_FILE_FAIL;

        return VOS_ERR;
    }

    /* 更新全局变量信息 */
    g_stLogFileSaveInfo.ulFileMaxId++;
    g_stLogFileSaveInfo.ulCurLogFileSize = 0;

    return VOS_OK;
}
VOS_UINT32 OM_LittleImgWriteLogFile(VOS_CHAR *pcLogData, VOS_UINT32 ulLength)
{
    VOS_UINT32 ulResult;

    /* 过滤消息 */
    ulResult = OM_LittleImgFilterMsg((VOS_UINT8*)pcLogData);

    /* 判断是否使能 */
    if (LOG_FILE_DISABLE == g_ulLogFileSaveFlg)
    {
        /*LogPrint("\r\n OM_LittleImgWriteLogFile: LOG_FILE_SAVE is Disable. \r\n"); */
        return ulResult;
    }

        /* 对临界资源g_astLogFileInfo进行保护 */
    if (VOS_OK != VOS_SmP(g_stLogFileSaveInfo.ulSem, LOG_SEM_TIMEOUT_VALUE))
    {
        g_ulLittleImgLogErr = LOG_OPERATION_POST_SEM_FAIL;
        return ulResult;
    }

    if (VOS_NULL_PTR == g_lFileHandle)
    {
        /*LogPrint("\r\n OM_LittleImgWriteLogFile: The file handle is null. \r\n"); */
        VOS_SmV(g_stLogFileSaveInfo.ulSem);
        return ulResult;
    }

    if (VOS_NULL_PTR == pcLogData)
    {
        VOS_SmV(g_stLogFileSaveInfo.ulSem);
        return ulResult;
    }

    /* 写入长度大于LOG文件的最大值 */
    if (ulLength > (g_stLogFileSaveInfo.ulCurLogFileMaxSize*FILE_SIZE_THRESHOLD_PERCENT))
    {
        LogPrint("\r\n OM_LittleImgWriteLogFile: The file size is too large. \r\n");
        VOS_SmV(g_stLogFileSaveInfo.ulSem);
        return ulResult;
    }

    /* LOG文件写满后 */
    if ((ulLength + g_stLogFileSaveInfo.ulCurLogFileSize) >
                (g_stLogFileSaveInfo.ulCurLogFileMaxSize * FILE_SIZE_THRESHOLD_PERCENT))
    {
        /* 关闭当前LOG文件句柄 */
        OM_LittleImgCloseLogFile();

        /* 如果创建文件失败，停止LOG文件保存功能 */
        if (VOS_OK != OM_LittleImgCreateLogFile(g_stLogFileSaveInfo.ulCurLogFileMaxSize))
        {
            g_ulLogFileSaveFlg = LOG_FILE_DISABLE;

            g_ulLittleImgLogErr = LOG_OPERATION_OPEN_FILE_FAIL;

            VOS_SmV(g_stLogFileSaveInfo.ulSem);

            return ulResult;
        }
    }

    /* 写LOG文件失败 */
    if ( DRV_ERROR == DRV_FILE_WRITE(pcLogData, sizeof(VOS_CHAR), (DRV_SIZE_T)ulLength, g_lFileHandle))
    {
        /* 关闭当前LOG文件 */
        OM_LittleImgCloseLogFile();

        g_ulLogFileSaveFlg = LOG_FILE_DISABLE;

        g_ulLittleImgLogErr = LOG_OPERATION_WRITE_FILE_FAIL;

        VOS_SmV(g_stLogFileSaveInfo.ulSem);

        return ulResult;
    }

    /* 更新当前LOG文件的大小 */
    g_stLogFileSaveInfo.ulCurLogFileSize += ulLength;

    VOS_SmV(g_stLogFileSaveInfo.ulSem);

    return ulResult;

}


VOS_VOID OM_LittleImgClearAllLogFile()
{
    VOS_UINT32 ulFileIndex;

    /* 关闭当前保存的LOG文件 */
    if (VOS_OK != OM_LittleImgCloseLogFile())
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_LittleImgClearAllLogFile: File handle is null.\n");
        /*return; */
    }

    for (ulFileIndex = g_stLogFileSaveInfo.ulFileMinId; ulFileIndex < g_stLogFileSaveInfo.ulFileMaxId; ulFileIndex++)
    {
        OM_LittleImgRemoveLogFile(ulFileIndex);
    }

    /*VOS_MemSet(&g_stLogFileSaveInfo, 0, sizeof(g_stLogFileSaveInfo)); */
}
VOS_UINT32 OM_LittleImgInitLogFile(VOS_UINT32 ulFileSize)
{
    LOG_FILE_DISK_INFO_STRU stDiskInfo;

    /* 创建LOG文件对应的信号量 */
    if (VOS_OK != VOS_SmMCreate("FILE", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_stLogFileSaveInfo.ulSem))
    {
        g_ulLittleImgLogErr = LOG_OPERATION_CREATE_SEM_FAIL;

        return VOS_ERR;
    }

    if (VOS_OK != VOS_SmP(g_stLogFileSaveInfo.ulSem, LOG_SEM_TIMEOUT_VALUE))
    {
        g_ulLittleImgLogErr = LOG_OPERATION_POST_SEM_FAIL;

        return VOS_ERR;
    }

    /* 判断LOG目录是否已经存在 */
    if (VOS_OK != DRV_FILE_ACCESS(g_acLogFileSaveDir, 0))
    {
        /* 如果目录不存在则创建LOG目录 */
        if(DRV_ERROR == DRV_FILE_MKDIR(g_acLogFileSaveDir))
        {
            g_ulLittleImgLogErr = LOG_OPERATION_MKDIR_FAIL;
            VOS_SmV(g_stLogFileSaveInfo.ulSem);
            return VOS_ERR;
        }
    }

    g_stLogFileSaveInfo.ulFileMinId = 1;
    g_stLogFileSaveInfo.ulFileMaxId = 1;
    g_stLogFileSaveInfo.ulCurLogFileMaxSize = ulFileSize * LOG_FILE_UNIT_SIZE;

    /* 调用底软接口返回 */
    if (DRV_ERROR == DRV_FILE_GET_DISKSPACE(
                    g_acFileSystemRoot,
                    (VOS_UINT*)&stDiskInfo.ulDiskSpace,
                    (VOS_UINT*)&stDiskInfo.ulUsedSpace,
                    (VOS_UINT*)&stDiskInfo.ulValidSpace))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_LittleImgInitLogFile: DRV_FILE_GET_DISKSPACE Failed!");
        return VOS_ERR;
    }

    g_stLogFileSaveInfo.ulFSFreeSpace = stDiskInfo.ulValidSpace;

    /* 如果NV项中文件大小不合法，修改为默认值 */
    if (g_stLogFileSaveInfo.ulCurLogFileMaxSize > g_stLogFileSaveInfo.ulFSFreeSpace)
    {
        g_stLogFileSaveInfo.ulCurLogFileMaxSize = LOG_FILE_SIZE * LOG_FILE_UNIT_SIZE;
    }

    /* 如果默认值大于文件系统大小，返回失败 */
    if (g_stLogFileSaveInfo.ulCurLogFileMaxSize > g_stLogFileSaveInfo.ulFSFreeSpace)
    {
        g_ulLittleImgLogErr = LOG_OPERATION_LENGTH_TOOBIG;
        VOS_SmV(g_stLogFileSaveInfo.ulSem);
        return VOS_ERR;
    }

    if (VOS_OK != OM_LittleImgCreateLogFile(g_stLogFileSaveInfo.ulCurLogFileMaxSize))
    {
        LogPrint("\r\n OM_LittleImgInitLogFile: Create Log File Failed.! \r\n");
        VOS_SmV(g_stLogFileSaveInfo.ulSem);
        return VOS_ERR;
    }

    g_ulLogFileSaveFlg = LOG_FILE_ENABLE;

    VOS_SmV(g_stLogFileSaveInfo.ulSem);

    return VOS_OK;

}


VOS_UINT32 OM_StartUpLogSaveReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32                ulResult;
    LOG_FILE_SAVE_CONFIG_STRU stLogFileSaveCfg;

    /* 读取NV项，更新默认文件大小以及校准结果 */
    ulResult = NV_Read(en_NV_Item_LOG_FILE_SAVE, &stLogFileSaveCfg, sizeof(LOG_FILE_SAVE_CONFIG_STRU));

    if (NV_OK != ulResult)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_StartUpLogSaveReq: NV_Read failed.\n");

        /* 返回结果ATE */
        OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);

        return VOS_OK;
    }

    if ( VOS_OK != stLogFileSaveCfg.ulResult )
    {
        /* 返回结果ATE */
        OM_SendResult(pstAppToOmMsg->ucFuncType, stLogFileSaveCfg.ulResult, usReturnPrimId);

        return VOS_OK;
    }

    /*初始化LOG文件保存*/
    ulResult = OM_LittleImgInitLogFile(stLogFileSaveCfg.ulLogFileSize);

    /* 返回结果ATE */
    OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);

    return VOS_OK;
}
VOS_UINT32 OM_StopLogSaveReq(APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                            VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32 ulResult = VOS_ERR;

    /* 获取校准结果 */
    ulResult = *(VOS_UINT32*)(pstAppToOmMsg->aucPara);

    if ( LOG_FILE_DISABLE == g_ulLogFileSaveFlg )
    {
        OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);

        return VOS_OK;
    }

    /* 对临界资源g_stLogFileInfo进行保护 */
    if (VOS_OK != VOS_SmP(g_stLogFileSaveInfo.ulSem, LOG_SEM_TIMEOUT_VALUE))
    {
        g_ulLittleImgLogErr = LOG_OPERATION_POST_SEM_FAIL;

        OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);

        return VOS_ERR;
    }

    /* 更新使能标记 */
    g_ulLogFileSaveFlg = LOG_FILE_DISABLE;

    if (VOS_OK == ulResult)
    {
        /*清除LOG文件*/
        OM_LittleImgClearAllLogFile();
    }
    else
    {
        OM_LittleImgCloseLogFile();
    }

    VOS_SmV(g_stLogFileSaveInfo.ulSem);

    /* 将结果保存到NV项中 */
    ulResult = NV_WritePart(en_NV_Item_LOG_FILE_SAVE, 0, (VOS_UINT32*)(pstAppToOmMsg->aucPara), sizeof(VOS_UINT32));

    OM_SendResult(pstAppToOmMsg->ucFuncType, ulResult, usReturnPrimId);

    /* 释放信号量资源 */
    if (VOS_OK != VOS_SmDelete(g_stLogFileSaveInfo.ulSem))
    {
        g_ulLittleImgLogErr = LOG_OPERATION_DELETE_SEM_FAIL;

        return VOS_ERR;
    }

    return VOS_OK;
}

#endif  /*(FEATURE_ON == FEATURE_MANUFACTURE_LOG)*/

#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif



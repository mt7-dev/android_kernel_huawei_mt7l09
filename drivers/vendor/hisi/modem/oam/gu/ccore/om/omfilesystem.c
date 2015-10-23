
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e767 修改人:甘兰 47350;原因:Log打印 */
#define    THIS_FILE_ID        PS_FILE_ID_OMFS_C
/*lint +e767 修改人:甘兰 47350;*/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "omfilesystem.h"
#include "pslogadapter.h"
#include "pslog.h"
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
VOS_CHAR g_acDumpDir[100];

/*LOG或临终遗言文件所在的目录*/
#if ((VOS_OS_VER == VOS_VXWORKS)||(VOS_OS_VER == VOS_RTOSCK))
#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)
#define OM_FILE_SYSTEM_DUMP_DIR             "/yaffs2/Exc"
#define OM_FILE_SYSTEM_UNITARY_DUMP_DIR     "/modem_log/Exc"
#else
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
#define OM_FILE_SYSTEM_DUMP_DIR             "/data/modemlog/Exc"
#define OM_FILE_SYSTEM_UNITARY_DUMP_DIR     "/modem_log/Exc"
#else
#define OM_FILE_SYSTEM_DUMP_DIR             "/yaffs0/Exc"
#define OM_FILE_SYSTEM_UNITARY_DUMP_DIR     "/modem_log/Exc"
#endif
#endif

#elif (VOS_WIN32 == VOS_OS_VER)
#define OM_FILE_SYSTEM_DUMP_DIR             ""
#define OM_FILE_SYSTEM_UNITARY_DUMP_DIR     ""
#endif
extern VOS_CHAR g_acLogDir[];

#if (FEATURE_ON == FEATURE_BROWSER_NV_FILE_IMEI_PROTECT)
extern VOS_UINT32 OM_QueryPrivilegeLevel(VOS_VOID);
extern VOS_BOOL NV_CheckNVFile(VOS_CHAR  *pcFilePath);
#endif
/* 被操作文件的信息 */
OM_FILE_INFO_STRU g_stOmFileInfo = {DRV_FILE_NULL, OM_FILE_BUTT, VOS_NULL_PTR};

/* 保存最后一条上传给工具的错误值 */
VOS_UINT32        g_ulOmSendToolErrNo = 0;
/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32 OM_QueryDir(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32             ulFolderType;
    OM_APP_MSG_EX_STRU    *pstOmToAppMsg;
    OM_APP_QUERY_DIR_STRU *pstOmQueryDir;
    VOS_UINT32             ulTotalSize;

    ulTotalSize = OM_APP_MSG_EX_LEN + sizeof(OM_APP_QUERY_DIR_STRU);

    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_QueryDir:VOS_MemAlloc fail!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_MEM_ALLOC_ERR, usReturnPrimId);
        return VOS_ERR;
    }
    pstOmQueryDir = (OM_APP_QUERY_DIR_STRU*)(pstOmToAppMsg->aucPara);
    /*判断请求文件夹的类型*/
    ulFolderType = *((VOS_UINT32*)(pstAppToOmMsg->aucPara));

    /*日志文件所在的路径*/
    if (OM_ROOT_FOLDER == ulFolderType)
    {
        /* 调底软接口获取文件系统路径 */
        pstOmQueryDir->ulLength = DRV_NAME_MAX;

        VOS_MemSet(pstOmQueryDir->aucDirPath, 0, DRV_NAME_MAX + 1);
        if(VOS_OK != DRV_GET_FS_ROOT_DIRECTORY((VOS_CHAR*)pstOmQueryDir->aucDirPath,
                                               pstOmQueryDir->ulLength))
        {
            VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
            PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
                    "OM_QueryDir: Get fs root directory from Drv fail!", (VOS_INT32)ulFolderType);
            OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_GET_DIRECTORY_ERR, usReturnPrimId);

            return VOS_ERR;
        }

        g_stOmFileInfo.ulFileType = OM_ROOT_FOLDER;

    }
    else if (OM_LOG_FOLDER == ulFolderType)
    {
        pstOmQueryDir->ulLength = VOS_StrLen((VOS_CHAR*)g_acLogDir);
        VOS_MemCpy(pstOmQueryDir->aucDirPath,
                      g_acLogDir, pstOmQueryDir->ulLength);
        g_stOmFileInfo.ulFileType = OM_LOG_FOLDER;
    }
    /*临终遗言文件所在的路径*/
    else if (OM_DUMP_FOLDER == ulFolderType)
    {
        pstOmQueryDir->ulLength = VOS_StrLen((VOS_CHAR*)g_acDumpDir);
        VOS_MemCpy(pstOmQueryDir->aucDirPath,
                      g_acDumpDir, pstOmQueryDir->ulLength);
        g_stOmFileInfo.ulFileType = OM_DUMP_FOLDER;
    }
    else
    {
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_WARNING,
                  "OM_QueryDir:Unknown ulFolderType fail!", (VOS_INT32)ulFolderType);
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_RECEIVE_FOLDER_TYPE_ERR, usReturnPrimId);

        return VOS_ERR;
    }
    /*由于ulTotalSize在分配内存空间时，使用的是最大值，
      所以实际的长度需要减去多于的字节*/
    ulTotalSize = ulTotalSize - (DRV_NAME_MAX - pstOmQueryDir->ulLength);
    pstOmQueryDir->ulResult = VOS_OK;
    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
    return VOS_OK;
}
/*****************************************************************************
函 数 名  : OM_GetDirInfo
功能描述  : 获取目录下所有的文件和文件夹名字的长度和个数
输入参数  : pstDir - 指向文件系统中目录的指针
输出参数  : pulTotalLen - 文件和文件夹名字的长度
            pulTotalNum - 文件和文件夹的个数
返 回 值  : 无

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_VOID OM_GetDirInfo(DRV_DIR_STRU *pstDir,
                           VOS_UINT32 *pulTotalLen, VOS_UINT32 *pulTotalNum)
{
    DRV_DIRENT_STRU     *pstTmpDirent;
    VOS_UINT32           ulTotalLen = 0;
    VOS_UINT32           ulTotalNum = 0;

    do
    {
        pstTmpDirent = DRV_FILE_READDIR(pstDir);
        /*readdir的返回值为空，表明目录浏览完毕*/
        if (VOS_NULL_PTR != pstTmpDirent)
        {
            ulTotalLen += VOS_StrLen(pstTmpDirent->d_name);
            ulTotalNum++;
        }
    }while (VOS_NULL_PTR != pstTmpDirent);
    *pulTotalLen = ulTotalLen;
    *pulTotalNum = ulTotalNum;
}
/*****************************************************************************
函 数 名  : OM_GetItemInfo
功能描述  : 获取文件或文件夹的基本信息
输入参数  : pstDir        - 指向目录结构的指针
            pstAppToOmMsg - 指向工具侧请求消息包的指针
            pstOmScanDir  - 指向存放基本信息内存的指针
输出参数  : 无
返 回 值  : VOS_ERR - 失败
            VOS_OK  - 成功

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_GetItemInfo(DRV_DIR_STRU *pstDir, APP_OM_MSG_EX_STRU *pstAppToOmMsg,
                                OM_APP_SCAN_DIR_STRU *pstOmScanDir)
{
    VOS_UINT8              *pucOmItemInfo;
    DRV_DIRENT_STRU        *pstTmpDirent;
    DRV_STAT_STRU           stStat;
    VOS_UINT8              *pucItemPath;
    VOS_UINT16              usDirLen;
    VOS_UINT32              ulTotalNum = 0;
    OM_ITEM_INFO_STRU       stOmItemInfo;

    usDirLen = (VOS_UINT16)VOS_StrLen((VOS_CHAR*)(pstAppToOmMsg->aucPara));
    /*分配空间用来存放文件或文件夹的路径，加1是因为后面需要添加斜杠*/
    pucItemPath = (VOS_UINT8*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT,
                                 usDirLen + DRV_NAME_MAX + 1);
    if (VOS_NULL_PTR == pucItemPath)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_GetItemInfo:VOS_MemAlloc fail!");
        return OM_FILE_MEM_ALLOC_ERR;
    }
    /*将目录路径拷贝进文件路径中*/
    VOS_MemCpy(pucItemPath, pstAppToOmMsg->aucPara, usDirLen);
    /*由于目录路径是不以斜杠结束，在与文件名结合时，需要添加斜杠*/
    pucItemPath[usDirLen] = '/';
    usDirLen++;

    pucOmItemInfo = (VOS_UINT8*)(pstOmScanDir->stOmItemInfo);
    do
    {
        /*遍历整个pstDir指向路径中的所有文件和文件夹*/
        pstTmpDirent = DRV_FILE_READDIR(pstDir);
        /*readdir的返回值为空，表明目录浏览完毕*/
        if (VOS_NULL_PTR != pstTmpDirent)
        {
            /*得到文件或文件夹名的长度*/
            stOmItemInfo.ulNameLen = (VOS_UINT16)VOS_StrLen(pstTmpDirent->d_name);
            /*由于文件或文件夹路径需要以'\0'作为结束，所以在Copy时，长度加1*/
            VOS_MemCpy(pucItemPath + usDirLen,
                         pstTmpDirent->d_name, stOmItemInfo.ulNameLen + 1);
            /*通过stat获取文件或文件夹的信息*/
            if (DRV_ERROR == DRV_FILE_STAT((VOS_CHAR*)pucItemPath, &stStat))
            {
                stOmItemInfo.ulItemType = OM_ITEM_FILE;
                stOmItemInfo.ulSize    = 0;
                stOmItemInfo.st_atime  = 0;
                stOmItemInfo.st_mtime  = 0;
                stOmItemInfo.st_ctime  = 0;
            }
            else
            {
                /*目录*/
                if (0 != (DRV_S_IFDIR&stStat.st_mode))
                {
                    stOmItemInfo.ulItemType = OM_ITEM_FOLDER;
                }
                /*文件*/
                else
                {
                    stOmItemInfo.ulItemType = OM_ITEM_FILE;
                }
                stOmItemInfo.ulSize    = (stStat.st_size & 0xFFFFFFFF);/* 目前文件大小不会超过32位大小 */
                stOmItemInfo.st_atime  = stStat.st_atime;
                stOmItemInfo.st_mtime  = stStat.st_mtime;
                stOmItemInfo.st_ctime  = stStat.st_ctime;
            }
            VOS_MemCpy(pucOmItemInfo, &stOmItemInfo, OM_ITEM_INFO_HEAD_LEN);
            pucOmItemInfo += OM_ITEM_INFO_HEAD_LEN;
            /*文件或文件夹名*/
            VOS_MemCpy(pucOmItemInfo, pstTmpDirent->d_name, stOmItemInfo.ulNameLen);
            pucOmItemInfo += stOmItemInfo.ulNameLen;
            /*用来记录文件和文件夹的总个数*/
            ulTotalNum++;
        }
    }while(VOS_NULL_PTR != pstTmpDirent);

    pstOmScanDir->ulResult = VOS_OK;
    pstOmScanDir->ulNumber = ulTotalNum;
    VOS_MemFree(WUEPS_PID_OM, pucItemPath);
    return VOS_OK;
}
/*****************************************************************************
函 数 名  : OM_ScanDir
功能描述  : 遍历整个目录，返回给工具侧当前目录下所有文件和文件夹的信息，
            包括文件名称，属性等。
输入参数  : pstAppToOmMsg  - 指向工具侧发来的请求消息包
            usReturnPrimId - 对应返回的Prim ID.
输出参数  : 无
返 回 值  : VOS_OK  - 操作成功
            VOS_ERR - 操作失败

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_ScanDir(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU     *pstOmToAppMsg;
    DRV_DIR_STRU           *pstTmpDir;
    VOS_UINT32              ulTotalSize;
    VOS_UINT32              ulTotalNameLen;
    VOS_UINT32              ulTotalNum;
    VOS_UINT32              ulRest;

    /*打开目录*/
    pstTmpDir = DRV_FILE_OPENDIR((VOS_CHAR*)(pstAppToOmMsg->aucPara));
    if (VOS_NULL_PTR == pstTmpDir)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_ScanDir:opendir fail!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_OPENDIR_ERR, usReturnPrimId);
        return VOS_ERR;
    }
    /*得到目录内，文件和文件夹的总个数、总的名字长度*/
    OM_GetDirInfo(pstTmpDir, &ulTotalNameLen, &ulTotalNum);
    /*关闭目录*/
    if (DRV_ERROR == DRV_FILE_CLOSEDIR(pstTmpDir))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_ScanDir:closedir fail!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_CLOSEDIR_ERR, usReturnPrimId);
        return VOS_ERR;
    }
    /*计算返回给工具侧消息包的长度*/
    ulTotalSize = OM_APP_MSG_EX_LEN + OM_APP_SCAN_DIR_HEAD_LEN
                  + (ulTotalNum*OM_ITEM_INFO_HEAD_LEN) + ulTotalNameLen;

    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_ScanDir:VOS_MemAlloc fail!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_MEM_ALLOC_ERR, usReturnPrimId);
        return VOS_ERR;
    }

    /*再次打开目录*/
    pstTmpDir = DRV_FILE_OPENDIR((VOS_CHAR*)(pstAppToOmMsg->aucPara));

    /*获取每个文件和文件夹的信息*/
    ulRest = OM_GetItemInfo(pstTmpDir, pstAppToOmMsg,
                         (OM_APP_SCAN_DIR_STRU*)(pstOmToAppMsg->aucPara));
    if (VOS_OK != ulRest)
    {
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        DRV_FILE_CLOSEDIR(pstTmpDir);
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, ulRest, usReturnPrimId);
        return VOS_ERR;
    }
    DRV_FILE_CLOSEDIR(pstTmpDir);
    pstOmToAppMsg->usLength = (VOS_UINT16)(ulTotalSize - VOS_OM_HEADER_LEN);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
    return VOS_OK;
}
/*****************************************************************************
函 数 名  : OM_CloseFile
功能描述  : 关闭文件，并清空全局变量
输入参数  : 无
输出参数  : 无
返 回 值  : 无

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_VOID OM_CloseFile(VOS_VOID)
{
    if (DRV_FILE_NULL != g_stOmFileInfo.lFile)
    {
        DRV_FILE_CLOSE(g_stOmFileInfo.lFile);
        g_stOmFileInfo.lFile = DRV_FILE_NULL;
    }
}

/*****************************************************************************
函 数 名  : OM_OpenFile
功能描述  : 打开指定的文件
输入参数  : pstAppToOmMsg  - 指向工具侧发来的请求消息包
            usReturnPrimId - 对应返回的Prim ID.
输出参数  : 无
返 回 值  : VOS_OK  - 操作成功
            VOS_ERR - 操作失败

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_OpenFile(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    APP_OM_OPEN_INFO_STRU *pstOmOpenInfo;
    VOS_CHAR              *pcPath;
    VOS_UINT32             ulFlag;
    VOS_INT32              lFlags = 0;

    /*不支持操作文件的重入操作*/
    if (DRV_FILE_NULL != g_stOmFileInfo.lFile)
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING,
                               "OM_OpenFile:Last Operation hasn't finished.");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_OPERATE_NOT_FINISH, usReturnPrimId);
        return VOS_ERR;
    }

    pstOmOpenInfo = (APP_OM_OPEN_INFO_STRU*)pstAppToOmMsg->aucPara;

    pcPath = (VOS_CHAR*)(pstOmOpenInfo->aucPara);

#if (FEATURE_ON == FEATURE_BROWSER_NV_FILE_IMEI_PROTECT)
    if (VOS_TRUE == NV_CheckNVFile(pcPath))
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_INFO,
                               "#info: OM_OpenFile: open is nv file.");
        /* IMEI 验证 */
        if (LEVEL_ADVANCED != OM_QueryPrivilegeLevel())
        {
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING,
                               "OM_OpenFile:IMEI verify fail.");
            OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_OPEN_SC_ERR, usReturnPrimId);
            return VOS_ERR;
        }
    }
#endif

    /* 硬加密版本中不能操作加密文件 */
    if (VOS_YES == DRV_SEC_CHECK())
    {
        if (VOS_OK == VOS_StrCmp(OM_CFDNV_FILE_PATH, pcPath))
        {
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_OpenFile:CFD NV File is secured.");
            OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_OPERATE_SEC_FILE_FORBID_CFD, usReturnPrimId);
            return VOS_ERR;
        }

        if (VOS_OK == VOS_StrCmp(OM_ZSP_FILE_PATH, pcPath))
        {
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_OpenFile:ZSP File is secured.");
            OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_OPERATE_SEC_FILE_FORBID_ZSP, usReturnPrimId);
            return VOS_ERR;
        }
    }

    ulFlag = pstOmOpenInfo->ulFlag;
    /*判断是否为可写*/
    if (0 != ISWIRTE(ulFlag))
    {
        lFlags = DRV_O_WRONLY | DRV_O_TRUNC;
    }
    /*判断是否为可读*/
    if (0 != ISREAD(ulFlag))
    {
        if (0 != lFlags)
        {
            lFlags = DRV_O_RDWR;
        }
        else
        {
            lFlags = DRV_O_RDONLY;
        }
    }
    /*判断是否为创建文件*/
    if (0 != ISCREATE(ulFlag))
    {
        lFlags = lFlags | DRV_O_CREAT;
    }
    /*以只读的方式打开文件*/
    g_stOmFileInfo.lFile = OM_Open(pcPath, lFlags);
    if (DRV_FILE_NULL == g_stOmFileInfo.lFile)
    {
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_OpenFile:open file fail.");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_OPEN_ERR, usReturnPrimId);
        return VOS_ERR;
    }
    /*启动定时器*/
    g_stOmFileInfo.hTimer = VOS_NULL_PTR;
    if (VOS_ERR == OM_START_FILETIMER(&g_stOmFileInfo.hTimer))
    {
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_OpenFile:Start Timer failed.");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_START_FILE_TIMER_ERR, usReturnPrimId);
        return VOS_ERR;
    }

    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);
    return VOS_OK;
}
/*****************************************************************************
函 数 名  : OM_ReadFile
功能描述  : 读取指定文件的内容
输入参数  : pstAppToOmMsg  - 指向工具侧发来的请求消息包
            usReturnPrimId - 对应返回的Prim ID.
输出参数  : 无
返 回 值  : VOS_OK  - 操作成功
            VOS_ERR - 操作失败

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_ReadFile(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    OM_APP_MSG_EX_STRU     *pstOmToAppMsg;
    OM_APP_READ_FILE_STRU  *pstOmReadFile;
    VOS_UINT32              ulTotalSize;
    VOS_UINT32              ulReadSize;

    if (DRV_FILE_NULL != g_stOmFileInfo.lFile)
    {
        /*停止定时器*/
        OM_STOP_FILETIMER(&g_stOmFileInfo.hTimer);
    }
    else
    {
        return VOS_ERR;
    }
    /*计算发送给工具侧消息包的总长度*/
    ulTotalSize = OM_APP_MSG_EX_LEN + sizeof(OM_APP_READ_FILE_STRU);

    pstOmToAppMsg = (OM_APP_MSG_EX_STRU*)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, ulTotalSize);
    if (VOS_NULL_PTR == pstOmToAppMsg)
    {
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_ReadFile:VOS_MemAlloc fail!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_MEM_ALLOC_ERR, usReturnPrimId);
        return VOS_ERR;
    }
    pstOmReadFile = (OM_APP_READ_FILE_STRU*)(pstOmToAppMsg->aucPara);
    ulReadSize = (VOS_UINT32)DRV_FILE_READ((VOS_CHAR*)pstOmReadFile->acFileContent, sizeof(VOS_CHAR), OM_READ_MAX_LEN, g_stOmFileInfo.lFile);
    /*读取文件出现错误*/
    if (DRV_ERROR == (VOS_INT32)ulReadSize)
    {
        VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_ReadFile:read fail!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_READ_ERR, usReturnPrimId);
        return VOS_ERR;
    }

    pstOmReadFile->ulResult = VOS_OK;
    pstOmReadFile->ulLength = ulReadSize;
    /*表明已经没有内容可以读取，文件内容全部读完*/
    if (0 == ulReadSize)
    {
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_INFO, "OM_ReadFile:Completed!");
    }
    else
    {
        /*启动定时器*/
        g_stOmFileInfo.hTimer = VOS_NULL_PTR;
        if (VOS_OK != OM_START_FILETIMER(&g_stOmFileInfo.hTimer))
        {
            VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
            OM_CloseFile();
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_ReadFile:OM_START_FILETIMER fail!");
            OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_START_FILE_TIMER_ERR, usReturnPrimId);
            return VOS_ERR;
        }
    }
    pstOmToAppMsg->usLength = (VOS_UINT16)(((ulTotalSize - VOS_OM_HEADER_LEN)
                               - OM_READ_MAX_LEN) + ulReadSize);
    OM_SendContent(pstAppToOmMsg->ucFuncType, pstOmToAppMsg, usReturnPrimId);
    VOS_MemFree(WUEPS_PID_OM, pstOmToAppMsg);
    return VOS_OK;
}
/*****************************************************************************
函 数 名  : OM_WriteFile
功能描述  : 写入指定文件的内容
输入参数  : pstAppToOmMsg  - 指向工具侧发来的请求消息包
            usReturnPrimId - 对应返回的Prim ID.
输出参数  : 无
返 回 值  : VOS_OK  - 操作成功
            VOS_ERR - 操作失败

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_WriteFile(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32  ulWriteSize;
    VOS_INT32   lResult;
    VOS_UINT8  *pucContent;

    if (DRV_FILE_NULL != g_stOmFileInfo.lFile)
    {
        /*停止定时器*/
        OM_STOP_FILETIMER(&g_stOmFileInfo.hTimer);
    }
    else
    {
        return VOS_ERR;
    }

    ulWriteSize = *((VOS_UINT32*)pstAppToOmMsg->aucPara);
    /*写文件完毕，关闭文件*/
    if (0 == ulWriteSize)
    {
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_INFO, "OM_WriteFile:Completed!");
        return VOS_OK;
    }
    /*指向需要写入文件内容的指针*/
    pucContent = (VOS_UINT8*)pstAppToOmMsg->aucPara + sizeof(ulWriteSize);
    lResult = DRV_FILE_WRITE((VOS_CHAR*)pucContent, sizeof(VOS_CHAR), ulWriteSize, g_stOmFileInfo.lFile);
    /*写文件操作失败*/
    if (DRV_ERROR == lResult)
    {
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_WriteFile:DRV_FILE_WRITE fail!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_WRITE_ERR, usReturnPrimId);
        return VOS_ERR;
    }
    /*写入长度不正确*/
    if (lResult != ulWriteSize)
    {
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR,
               "OM_WriteFile: the written size is not correct!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_WRITE_LEN_ERR, usReturnPrimId);
        return VOS_ERR;
    }
    /*启动定时器*/
    if (VOS_OK != OM_START_FILETIMER(&g_stOmFileInfo.hTimer))
    {
        OM_CloseFile();
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_WriteFile:OM_START_FILETIMER fail!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_START_FILE_TIMER_ERR, usReturnPrimId);
        return VOS_ERR;
    }
    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);
    return VOS_OK;
}
/*****************************************************************************
函 数 名  : OM_MakeDir
功能描述  : 创建指定目录
输入参数  : pstAppToOmMsg  - 指向工具侧发来的请求消息包
            usReturnPrimId - 对应返回的Prim ID.
输出参数  : 无
返 回 值  : VOS_OK  - 操作成功
            VOS_ERR - 操作失败

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_MakeDir(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    if (DRV_ERROR == DRV_FILE_MKDIR((VOS_CHAR*)(pstAppToOmMsg->aucPara)))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_MakeDir: DRV_FILE_MKDIR failed!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_MKDIR_ERR, usReturnPrimId);

        return VOS_ERR;
    }
    OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);
    return VOS_OK;
}
/*****************************************************************************
函 数 名  : OM_DeleteFile
功能描述  : 创建指定目录
输入参数  : pstAppToOmMsg  - 指向工具侧发来的请求消息包
            usReturnPrimId - 对应返回的Prim ID.
输出参数  : 无
返 回 值  : VOS_OK  - 操作成功
            VOS_ERR - 操作失败

修改历史      :
    1.日    期   : 2008年9月9日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_DeleteFile(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    VOS_UINT32  ulItemType;
    VOS_CHAR   *pcPath;
    /*得到当前操作的是文件还是文件夹*/
    ulItemType = *((VOS_UINT32*)pstAppToOmMsg->aucPara);
    /*获得路径*/
    pcPath = (VOS_CHAR*)pstAppToOmMsg->aucPara + sizeof(ulItemType);
    if (OM_ITEM_FOLDER == ulItemType)
    {
        if (DRV_ERROR == DRV_FILE_RMDIR(pcPath))
        {
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_DeleteFile: DRV_FILE_RMDIR failed!");
            OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_RMDIR_ERR, usReturnPrimId);
            return VOS_ERR;
        }
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);
        return VOS_OK;
    }
    else if (OM_ITEM_FILE == ulItemType)
    {
        /* 硬加密版本中不能操作加密文件 */
        if (VOS_YES == DRV_SEC_CHECK())
        {
            if (VOS_OK == VOS_StrCmp(OM_CFDNV_FILE_PATH, pcPath))
            {
                PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_OpenFile:CFD NV File is secured.");
                OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_OPERATE_SEC_FILE_FORBID_CFD, usReturnPrimId);
                return VOS_ERR;
            }

            if (VOS_OK == VOS_StrCmp(OM_ZSP_FILE_PATH, pcPath))
            {
                PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_OpenFile:ZSP File is secured.");
                OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_OPERATE_SEC_FILE_FORBID_ZSP, usReturnPrimId);
                return VOS_ERR;
            }
        }

        if (DRV_ERROR == DRV_FILE_RMFILE(pcPath))
        {
            PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_DeleteFile: DRV_FILE_RMFILE failed!");
            OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_RMFILE_ERR, usReturnPrimId);
            return VOS_ERR;
        }
        OM_SendResult(pstAppToOmMsg->ucFuncType, VOS_OK, usReturnPrimId);
        return VOS_OK;
    }
    else
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_DeleteFile: Unknown type!");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_RECEIVE_ITEM_TYPE_ERR, usReturnPrimId);
    }
    return VOS_ERR;
}
/*****************************************************************************
函 数 名  : OM_QuerySpace
功能描述  : 查询文件系统的内存空间使用情况
输入参数  : pstAppToOmMsg  - 指向工具侧发来的请求消息包
            usReturnPrimId - 对应返回的Prim ID.
输出参数  : 无
返 回 值  : VOS_OK  - 操作成功
            VOS_ERR - 操作失败

修改历史      :
    1.日    期   : 2009年5月11日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_QuerySpace(APP_OM_MSG_EX_STRU *pstAppToOmMsg, VOS_UINT16 usReturnPrimId)
{
    OM_APP_QUERY_DISKSPACE_STRU     stOmDiskSpace;

    if (DRV_ERROR == DRV_FILE_GET_DISKSPACE(
                    (VOS_CHAR*)(pstAppToOmMsg->aucPara),
                    (VOS_UINT*)&stOmDiskSpace.ulDiskSpace,
                    (VOS_UINT*)&stOmDiskSpace.ulUsedSpace,
                    (VOS_UINT*)&stOmDiskSpace.ulValidSpace))
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_ERROR, "OM_QuerySpace: DRV_FILE_GET_DISKSPACE");
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_DRV_FILE_GET_DISKSPACE_ERR, usReturnPrimId);

        return VOS_ERR;
    }

    stOmDiskSpace.ulResult = VOS_OK;
    stOmDiskSpace.usLength = (VOS_UINT16)(sizeof(OM_APP_QUERY_DISKSPACE_STRU)
                            - VOS_OM_HEADER_LEN);

    OM_SendContent(pstAppToOmMsg->ucFuncType, (OM_APP_MSG_EX_STRU*)&stOmDiskSpace, usReturnPrimId);
    return VOS_OK;
}

/* 保存工具侧发来的请求消息和对应处理函数的关系*/
static OM_MSG_FUN_STRU g_astOmFSMsgTbl[] =
{
    {APP_OM_QUERY_DIR_REQ,      OM_QueryDir,     OM_APP_QUERY_DIR_IND},
    {APP_OM_SCAN_DIR_REQ,       OM_ScanDir,      OM_APP_SCAN_DIR_IND},
    {APP_OM_OPEN_FILE_REQ,      OM_OpenFile,     OM_APP_OPEN_FILE_CNF},
    {APP_OM_READ_FILE_REQ,      OM_ReadFile,     OM_APP_READ_FILE_IND},
    {APP_OM_WRITE_FILE_REQ,     OM_WriteFile,    OM_APP_WRITE_FILE_CNF},
    {APP_OM_MAKE_DIR_REQ,       OM_MakeDir,      OM_APP_MAKE_DIR_CNF},
    {APP_OM_DELETE_FILE_REQ,    OM_DeleteFile,   OM_APP_DELETE_FILE_CNF},
    {APP_OM_QUERY_SPACE_REQ,    OM_QuerySpace,   OM_APP_QUERY_SPACE_CNF}
};

/*****************************************************************************
函 数 名  : OM_FileSystemMsgProc
功能描述  : 处理OM对文件系统的操作消息
输入参数  : pstAppToOmMsg - 指向工具侧发来的消息包指针
输出参数  : 无
返 回 值  : VOS_OK

修改历史      :
    1.日    期   : 2008年9月8日
      作    者   : 甘兰
      修改内容   : 创建此函数
*****************************************************************************/
VOS_VOID OM_FileSystemMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr)
{
    APP_OM_MSG_EX_STRU      *pstAppToOmMsg;
    VOS_UINT32               ulIndex;
    VOS_UINT32               ulTotalNum;
    VOS_UINT16               usPrimId;
    VOS_UINT16               usReturnPrimId;
    VOS_UINT32               ulResult = VOS_ERR;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pRspPacket;
    ulTotalNum = sizeof(g_astOmFSMsgTbl)/sizeof(OM_MSG_FUN_STRU);
    usPrimId = pstAppToOmMsg->usPrimId;
    /*Search the corresponding function and return PrimId*/
    for (ulIndex = 0; ulIndex < ulTotalNum; ulIndex++)
    {
        if (usPrimId == (VOS_UINT16)(g_astOmFSMsgTbl[ulIndex].ulPrimId))
        {
            usReturnPrimId = (VOS_UINT16)(g_astOmFSMsgTbl[ulIndex].ulReturnPrimId);
            ulResult = g_astOmFSMsgTbl[ulIndex].pfFun(pstAppToOmMsg, usReturnPrimId);
            if (VOS_ERR == ulResult)
            {
                g_stOmFileInfo.ulFileType = OM_FILE_BUTT;
            }
            break;
        }
    }
    /*Can't find the function handles the usPrimId.*/
    if (ulIndex == ulTotalNum)
    {
        PS_LOG1(WUEPS_PID_OM, 0, PS_PRINT_INFO,
          "OM_FileSystemMsgProc: PrimId can't be handled", (VOS_INT32)usPrimId);
        OM_FILE_SEND_RESULT(pstAppToOmMsg->ucFuncType, OM_FILE_RECEIVE_REQ_FIND_FUNC_ERR, usPrimId);
    }
    return;
}

/*****************************************************************************
函 数 名  : OM_Open
功能描述  : 适配Open方式打开文件，通过解析打开方式，调用对应的fopen的模式
输入参数  : pcFileName  - 文件路径
            lFlag       - 文件打开方式.
输出参数  : 无
返 回 值  : 打开文件的句柄

修改历史      :
    1.日    期   : 2012年4月11日
      作    者   : 祝锂
      修改内容   : 创建此函数
*****************************************************************************/
FILE* OM_Open(const VOS_CHAR *pcFileName, VOS_INT lFlag)
{
    VOS_CHAR    *pucMode;
    FILE        *fp;

    switch(lFlag)
    {
        case (DRV_O_CREAT|DRV_O_APPEND|DRV_O_RDWR):
            pucMode = "ab+";
            break;

        case (DRV_O_CREAT|DRV_O_APPEND|DRV_O_WRONLY):
            pucMode = "ab";
            break;

        case (DRV_O_CREAT|DRV_O_TRUNC|DRV_O_RDWR):
            pucMode = "wb+";
            break;

        case (DRV_O_CREAT|DRV_O_TRUNC|DRV_O_WRONLY):
            pucMode = "wb";
            break;

        case DRV_O_RDWR:
            pucMode = "rb+";
            break;

        case DRV_O_RDONLY:
            pucMode = "rb";
            break;

        default:
            LogPrint1("\r\nOM_Open: The File Mode 0x%x is Error", lFlag);
            return DRV_FILE_NULL;
    }

    fp = DRV_FILE_OPEN(pcFileName, pucMode);

    return fp;
}


/*****************************************************************************
函 数 名  : OM_FileSystemInit
功能描述  : 文件系统相关变量初始化
输入参数  : 无
输出参数  : 无
返 回 值  : VOS_ERR/VOS_OK

修改历史      :
    1.日    期   : 2014年3月11日
      作    者   : jinni
      修改内容   : 创建此函数
*****************************************************************************/
VOS_UINT32 OM_FileSystemInit(VOS_VOID)
{
    /* 初始化文件系统路径信息 */
    if (VOS_OK != OM_GetLogPath(g_acDumpDir, OM_FILE_SYSTEM_DUMP_DIR, OM_FILE_SYSTEM_UNITARY_DUMP_DIR))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


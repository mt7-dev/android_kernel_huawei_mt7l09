/******************************************************************************

            版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : PsLog.c
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2007年4月23日
  最近修改   :
  功能描述   : Log功能实现
  函数列表   : Log_BufInput
               Log_BuildId
               Log_BuildStr
               Log_FileClose
               Log_FileOpen
               Log_FileWrite
               Log_GetFileName
               Log_GetTableIndex
               Log_IdNParam
               Log_Init
               Log_Output
               Log_SelfTask
               Log_SetAsPrintLev
               Log_SetHpaPrintLev
               Log_SetModuleIdLev
               Log_SetNasLev
               Log_SetOutputType
               Log_SetPartLev
               Log_SetTtfPrintLev
               Log_StrNParam
               OM_Log
               OM_Log1
               OM_Log2
               OM_Log3
               OM_Log4
               OM_LogId
               OM_LogId1
               OM_LogId2
               OM_LogId3
               OM_LogId4

  修改历史   :
  1.日    期   : 2007年4月23日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/
#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/********************头文件****************************************************/

#include "vos.h"
#include "pslogadapter.h"
#include "omfilesystem.h"
#include "NVIM_Interface.h"
#include "pslog.h"
#include "DrvInterface.h"
#include "FileSysInterface.h"
#include "omnvinterface.h"

/******************全局变量声明************************************************/
#define THIS_FILE_ID PS_FILE_ID_PS_LOG_C

/* ErrLog实体 */
MNTN_ERRLOG_STRU g_stErrLogStru;

/* Log实体 */
LOG_ENTITY_ST  g_stLogEnt =
{LOG_FALSE,  OM_OUTPUT_SHELL, LOG_NULL_PTR, LOG_NULL_PTR};
/*用来对RingBuffer进行互斥访问*/
LOG_SEM        g_logBuffSem;
/*全局变量，用来保存每个模块的打印级别*/
LOG_LEVEL_EN   g_aulLogPrintLevPsTable[LOG_PS_MODULE_MAX_NUM] = {LOG_LEVEL_OFF};
LOG_LEVEL_EN   g_aulLogPrintLevDrvTable[LOG_DRV_MODULE_MAX_NUM] = {LOG_LEVEL_OFF};
/*保存在文件系统中的LOG文件信息*/

#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
LOG_FILE_INFO_STRU g_astLogFileInfo[LOG_SOURCE_BUTT]
               = {{VOS_FALSE, LOG_FILE_1, 0, LOG_FILE_MAX_SIZE, 0, "/data/modemlog/Log/PsLog0"},
                  {VOS_FALSE, LOG_FILE_1, 0, LOG_FILE_MAX_SIZE, 0, "/data/modemlog/Log/Printf0"},
                  {VOS_FALSE, LOG_FILE_1, 0, (LOG_FILE_MAX_SIZE*2), 0, "/data/modemlog/Log/OmLog0"}};
#else
LOG_FILE_INFO_STRU g_astLogFileInfo[LOG_SOURCE_BUTT]
               = {{VOS_FALSE, LOG_FILE_1, 0, LOG_FILE_MAX_SIZE, 0, "/yaffs0/Log/PsLog0"},
                  {VOS_FALSE, LOG_FILE_1, 0, LOG_FILE_MAX_SIZE, 0, "/yaffs0/Log/Printf0"},
                  {VOS_FALSE, LOG_FILE_1, 0, (LOG_FILE_MAX_SIZE*2), 0, "/yaffs0/Log/OmLog0"}};
#endif

/* 用于LOG写入FLASH接口的定位信息 */

#if ((VOS_VXWORKS == VOS_OS_VER)||(VOS_RTOSCK == VOS_OS_VER))
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
VOS_CHAR g_acLogDir[LOG_FILE_PATH_MAX_LENGTH] = "/data/modemlog/Log";
#else
/*LOG File Directory */
VOS_CHAR g_acLogDir[LOG_FILE_PATH_MAX_LENGTH] = "/yaffs0/Log";
#endif
#elif (VOS_WIN32 == VOS_OS_VER)
/*LOG File Directory */
VOS_CHAR g_acLogDir[] = "";
#endif

UINT32  g_ulLogErrFlag = LOG_OPERATION_OK;
VOS_UINT32                   g_ulPslogStatus = VOS_OK;

VOS_UINT32                   g_ulImmediatelyWriteFlg = VOS_FALSE;

extern VOS_UINT32 USIMM_GetCardIMSI(VOS_UINT8 *pucImsi);

/*****************************************************************************
 函 数 名  : OM_InitLogPath
 功能描述  : 根据是否支持SD卡来确定LOG保存路径
 输入参数  : VOID
 输出参数  : VOID
 返 回 值  : VOID
 修改历史      :
  1.日    期   : 2011年1月3日
    作    者   :
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID OM_InitLogPath(void)
{
    OM_PORT_CFG_STRU    stPortCfg;
    VOS_UINT32          ulIndex;
    SD_DEV_TYPE_STR*    pstPath;
    VOS_CHAR            acDir[] = "/Log";
    VOS_CHAR            acPath[LOG_SOURCE_BUTT][LOG_FILE_PATH_MAX_LENGTH]
                                ={"/Log/PsLog0","/Log/Printf0","/Log/OmLog0"};

    if (NV_OK != NV_Read(en_NV_Item_Om_Port_Type, (VOS_VOID *)&stPortCfg, sizeof(OM_PORT_CFG_STRU)))
    {
        vos_printf("OM_InitLogPath: read NV fail\n");
        return;
    }

    if(OMRL_SD != stPortCfg.enPortType)
    {
        return;                 /* 不支持SD卡 ,默认路径为/yaffs0/Log */
    }

    pstPath = DRV_GET_SD_PATH();

    if((VOS_NULL_PTR == pstPath)||(0 == pstPath->devNameLen))
    {
        g_ulPslogStatus = VOS_ERR;
        vos_printf("OM_InitLogPath: SD card initialize fail\n");
        return;
    }

    VOS_MemCpy(g_acLogDir, pstPath->devName, pstPath->devNameLen);
    VOS_MemCpy(&g_acLogDir[pstPath->devNameLen], acDir, VOS_StrLen(acDir)+1);/*多COPY一个字节确保以\0结束*/

    for(ulIndex=0; ulIndex<LOG_SOURCE_BUTT; ulIndex++)
    {
        VOS_MemCpy(g_astLogFileInfo[ulIndex].acName, pstPath->devName, pstPath->devNameLen);
        VOS_MemCpy(&g_astLogFileInfo[ulIndex].acName[pstPath->devNameLen],
                   acPath[ulIndex], VOS_StrLen(acPath[ulIndex])+1);/*多COPY一个字节确保以\0结束*/
    }

    return;
}
VOS_VOID MNTN_GetHeadVersion(VOS_VOID * pVersion )
{
    MNTN_VERSION_INFO_EVENT_STRU    *pstVerInfo ;

    /*   入参检查   */
    if(VOS_NULL_PTR == pVersion)
    {
        vos_printf("MNTN_GetHeadVersion:Input parameter error\n");
        return ;
    }

    /*   填充记录头   */
    pstVerInfo = (MNTN_VERSION_INFO_EVENT_STRU *)pVersion;
    memset(pstVerInfo, 0, sizeof(MNTN_VERSION_INFO_EVENT_STRU));

    pstVerInfo->stRecordHead.ulInfoLen   = sizeof(MNTN_VERSION_INFO_EVENT_STRU);
    pstVerInfo->stRecordHead.ulErrNo     = MNTN_VERSION_INFO_EVENT;
    pstVerInfo->stRecordHead.ulSliceTime = VOS_GetSlice();

    /*    获取boardtime   */
    if ( NV_OK != NV_Read(en_NV_Item_LiveTime,
            (VOS_VOID *)&(pstVerInfo->stRecordHead.ulBoardtime), sizeof(VOS_UINT32)))
    {
        vos_printf("MNTN_GetHeadVersion:Get boardtime error.\n");
        pstVerInfo->stRecordHead.ulBoardtime = 0xFFFFFFFF;
    }

    /*    获取IMEI   */
    if (NV_OK != NV_Read(en_NV_Item_IMEI,
            (VOS_VOID *)&pstVerInfo->stIMEI, MNTN_ERRLOG_IMEI_SIZE))
    {
        vos_printf("MNTN_GetHeadVersion:GET imei ERROR\n");
    }

    /*    获取硬件版本   */
    if (VOS_OK != DRV_GET_FULL_HW_VER(pstVerInfo->acHardVersion,
            MNTN_ERRLOG_HARDVERSION_SIZE))
    {
        vos_printf("MNTN_GetHeadVersion:get hardversion error\n");
    }

    /*    获取软件版本   */
    if (VOS_OK != DRV_MEM_VERCTRL((VOS_INT8*)pstVerInfo->acSoftVersion,
            MNTN_ERRLOG_SOFTVERSION_SIZE, VER_SOFTWARE, VERIONREADMODE))
    {
        vos_printf("MNTN_GetHeadVersion:get softversion error\n");
    }

    return ;
}


FILE* MNTN_OpenErrorLog(VOS_UINT32 ulFileId, VOS_INT32 lFileFlag)
{
    FILE            *pLogFile;
    VOS_CHAR        aucErrLogPath[100];

    switch(ulFileId)
    {
        case MNTN_ERRLOR0_ID:
            if (VOS_OK != OM_GetLogPath(aucErrLogPath, MNTN_ERRLOG0_PATH, MNTN_ERRLOG0_UNITARY_PATH))
            {
                return DRV_FILE_NULL;
            }

            pLogFile = OM_Open(aucErrLogPath, lFileFlag);
            break;

        case MNTN_ERRLOR1_ID:
            if (VOS_OK != OM_GetLogPath(aucErrLogPath, MNTN_ERRLOG1_PATH, MNTN_ERRLOG0_UNITARY_PATH))
            {
                return DRV_FILE_NULL;
            }

            pLogFile = OM_Open(aucErrLogPath, lFileFlag);
            break;

        default:
            pLogFile = DRV_FILE_NULL;
            vos_printf("MNTN_OpenErrorLog:Parameter Error\n");
            break;
    }

    return pLogFile;
}


void  MNTN_RecordVersionInfo(void)
{
    FILE*                           lLogFile ;
    VOS_INT                         lFileSize;
    MNTN_VERSION_INFO_EVENT_STRU    stVerInfo;

    if(ERRLOG_DISABLE == g_stErrLogStru.ucInitSucc)
    {
        vos_printf("MNTN_RecordVersionInfo:Errlog uninitialize\n");
        return ;
    }

    /*Errlog.bin文件不存在，建立*/
    lLogFile = MNTN_OpenErrorLog(g_stErrLogStru.ucFileId, DRV_O_CREAT|DRV_O_APPEND|DRV_O_RDWR);
    if(DRV_FILE_NULL == lLogFile)
    {
        vos_printf("MNTN_RecordVersionInfo:fopen fd file error\n");
        return;
    }

    /*获取该文件长度*/
    lFileSize = (VOS_INT)DRV_FILE_LSEEK(lLogFile, 0, DRV_SEEK_END);
    if(DRV_ERROR == lFileSize)
    {
        DRV_FILE_CLOSE(lLogFile);
        vos_printf("MNTN_RecordVersionInfo:LSEEK error\n");
        return;
    }

    if(VOS_OK != Log_SmP(&g_stErrLogStru.semFileId))
    {
        DRV_FILE_CLOSE(lLogFile);
        vos_printf("MNTN_RecordVersionInfo:Take semphore Error\n");
        return;
    }

    if (MNTN_ERRLOG_MAXSIZE < lFileSize)
    {
        DRV_FILE_CLOSE(lLogFile);
        lLogFile = DRV_FILE_NULL;

        /* 切换文件ID */
        g_stErrLogStru.ucFileId = (g_stErrLogStru.ucFileId + 1)%2;

        /*重新建立Errlog.bin文件*/
        lLogFile = MNTN_OpenErrorLog(g_stErrLogStru.ucFileId, DRV_O_CREAT|DRV_O_TRUNC|DRV_O_RDWR);
        if(DRV_FILE_NULL == lLogFile)
        {
            vos_printf("MNTN_RecordVersionInfo:fopen fd file error\n");
            Log_SmV(&g_stErrLogStru.semFileId);
            return;
        }
    }

    Log_SmV(&g_stErrLogStru.semFileId);

    /*获取版本信息*/
    MNTN_GetHeadVersion((VOS_VOID *)&stVerInfo);

    /*写flash*/
    if(DRV_ERROR == DRV_FILE_WRITE((VOS_CHAR *)&stVerInfo, sizeof(VOS_CHAR), stVerInfo.stRecordHead.ulInfoLen, lLogFile))
    {
        vos_printf("MNTN_RecordVersionInfo:write version info error\n");
    }

    DRV_FILE_CLOSE(lLogFile);

    return;
}


VOS_UINT32 MNTN_ErrLog_BufInput(VOS_CHAR * pcLogStr, VOS_UINT32 ulLen)
{
    VOS_UINT32              ulRecordLen ;

    /* 判断输出参数 */
    if((VOS_NULL_PTR == pcLogStr)||(0 == ulLen))
    {
        return VOS_ERR ;
    }

    if(VOS_OK != Log_SmP(&g_stErrLogStru.semErrlog))
    {
        vos_printf("MNTN_ErrLog_BufInput:Take semphore error\n");
        return VOS_ERR ;
    }

    ulRecordLen = g_stErrLogStru.aulRecordLen[g_stErrLogStru.ucBufId];

    /* 判断buf中空间是否足以存放该记录 */
    if(MAX_ERRLOG_BUF_LEN < (ulLen + ulRecordLen))
    {
        Log_SmV(&g_stErrLogStru.semErrlog);
        return VOS_ERR ;
    }

    /* 将记录拷贝到buf中 */
    memcpy(g_stErrLogStru.acbuf[g_stErrLogStru.ucBufId] + ulRecordLen,
                pcLogStr, ulLen);
    g_stErrLogStru.aulRecordLen[g_stErrLogStru.ucBufId] = ulRecordLen + ulLen;

    Log_SmV(&g_stErrLogStru.semErrlog);

    return VOS_OK;
}
unsigned int MNTN_ErrorLog(char * cFileName, unsigned int ulFileId, unsigned int ulLine,
                unsigned int ulErrNo, void *pRecord, unsigned int ulLen)
{
    MNTN_HEAD_INFO_STRU *       pstHeadInfo;
    VOS_UINT32                  ulInfolen;
    VOS_INT32                   ret;
    DRV_MNTN_SYSTEM_RESET_STRU  *pstSysReset;
    static VOS_UINT32            ulfistCallFlag = VOS_TRUE;

    /* 判断输出参数 */
    if((VOS_NULL_PTR == pRecord)||(0 == ulLen))
    {
        return VOS_ERR;
    }

    /* 判断ERRLOG是否使能 */
    if(ERRLOG_DISABLE == g_stErrLogStru.ucInitSucc)
    {
        return VOS_ERR;
    }

    /* 填充记录头 */
    pstHeadInfo = (MNTN_HEAD_INFO_STRU *)pRecord;
    pstHeadInfo->ulInfoLen   = ulLen;
    pstHeadInfo->ulErrNo     = ulErrNo;
    pstHeadInfo->ulSliceTime = VOS_GetSlice();

    if (NV_OK != NV_Read(en_NV_Item_LiveTime,
                    (VOS_VOID *)(&pstHeadInfo->ulBoardtime), sizeof(VOS_UINT32)))
    {
        vos_printf("MNTN_ErrorLog:Get boardtime error\n");
        pstHeadInfo->ulBoardtime = 0xFFFFFFFF;
    }

    if(VOS_TRUE == ulfistCallFlag)
    {
        pstSysReset = (DRV_MNTN_SYSTEM_RESET_STRU *)GET_SYSTEM_RESET_INFO();

        if(VOS_NULL_PTR != pstSysReset)
        {
            pstSysReset->stRecordHead.ulErrNo = MNTN_DRV_SYSTEM_RESET_RECORD;
            pstSysReset->stRecordHead.ulInfoLen = sizeof(DRV_MNTN_SYSTEM_RESET_STRU);
            pstSysReset->stRecordHead.ulBoardtime = pstHeadInfo->ulBoardtime;

            pstSysReset->ImsiStatus = (VOS_UINT8)USIMM_GetCardIMSI(pstSysReset->Imsi);

            MNTN_ErrLog_BufInput((VOS_CHAR *)pstSysReset, sizeof(DRV_MNTN_SYSTEM_RESET_STRU));
        }
        else
        {
            vos_printf("INFO :MNTN_ErrorLog:Can't Get Reset Info.\r\n");
        }

        ulfistCallFlag = VOS_FALSE;
    }
    /* 将记录保存到buf中 */
    MNTN_ErrLog_BufInput((char *)pRecord,ulLen);

    /* 将长度转换成长整形字节的个数 */
    ulInfolen = ((ulLen + sizeof(long)) - 1)>>2 ;

    /* 将记录通过omlog输出 */
    #ifdef __LOG_BBIT__
    ret = Log_StrNParam(cFileName, ulLine, LOG_ERRLOG_ID, (LOG_SUBMOD_ID_EN)0,
                LOG_LEVEL_ERROR, "Errlog:", (unsigned char)ulInfolen ,(long *)pRecord);
    #endif
    #ifdef __LOG_RELEASE__
    ret = Log_IdNParam(LOG_ERRLOG_ID, 0, LOG_LEVEL_ERROR,
                PsLogId(ulFileId, ulLine), ulInfolen , (long *)pRecord);
    #endif

    if(MNTN_USB_ENUMERATE_STATUS_EVENT == ulErrNo)
    {
        g_ulImmediatelyWriteFlg = VOS_TRUE;
    }

    if (VOS_OK != ret)
    {
        Log_SmV(&(g_stLogEnt.semOmPrint));
    }
    return VOS_OK ;
}


VOS_UINT8 MNTN_GetFileId(VOS_VOID)
{
    FILE*                   lLogFile0;     /* File0 handle */
    FILE*                   lLogFile1;     /* File1 handle */
    VOS_INT                 lFile0Size;    /* File0 size */
    VOS_UINT8               ucFileId;      /* 函数返回的文件句柄 */
    MNTN_HEAD_INFO_STRU     stHeadInfo0;   /* File0的记录头部 */
    MNTN_HEAD_INFO_STRU     stHeadInfo1;   /* File1的记录头部 */
    VOS_UINT32              ulReadSize0;   /* File0的记录头部的字节数(文件系统函数返回值) */
    VOS_UINT32              ulReadSize1;   /* File1的记录头部的字节数(文件系统函数返回值) */
    VOS_CHAR                aucErrLogPath[100];

    if (VOS_OK != OM_GetLogPath(aucErrLogPath, MNTN_ERRLOG0_PATH, MNTN_ERRLOG0_UNITARY_PATH))
    {
        return DRV_FILE_NULL;
    }

    lLogFile0 = OM_Open(aucErrLogPath, DRV_O_RDONLY);
    if(DRV_FILE_NULL == lLogFile0)
    {
        vos_printf("MNTN_GetFileId:open Errorlog0 error\n");
        ucFileId = MNTN_ERRLOR0_ID;
        return ucFileId;
    }

    /*获取版本信息*/
    ulReadSize0 = (VOS_UINT32)DRV_FILE_READ((VOS_CHAR*)&stHeadInfo0, sizeof(VOS_CHAR), sizeof(MNTN_HEAD_INFO_STRU), lLogFile0);
    if(DRV_ERROR == (VOS_INT32)ulReadSize0)
    {
        vos_printf("MNTN_GetFileId:read error\n");
        ucFileId = MNTN_ERRLOR0_ID;
        DRV_FILE_CLOSE(lLogFile0);
        return ucFileId;
    }

    if (VOS_OK != OM_GetLogPath(aucErrLogPath, MNTN_ERRLOG1_PATH, MNTN_ERRLOG1_UNITARY_PATH))
    {
        return DRV_FILE_NULL;
    }

    lLogFile1 = OM_Open(aucErrLogPath, DRV_O_RDONLY);

    ulReadSize1 = (VOS_UINT32)DRV_FILE_READ((VOS_CHAR*)&stHeadInfo1, sizeof(VOS_CHAR), sizeof(MNTN_HEAD_INFO_STRU), lLogFile1);

/*lint -e774*/
    if ((DRV_FILE_NULL == lLogFile1) || (DRV_ERROR == (VOS_INT32)ulReadSize1))
/*lint +e774*/
    {
        lFile0Size = (VOS_INT)DRV_FILE_LSEEK(lLogFile0, 0, DRV_SEEK_END);

        /* File0没有满或者读取File0大小失败时, 选择File0作为要写入的文件 */
        if ((lFile0Size < MNTN_ERRLOG_MAXSIZE) || (DRV_ERROR == lFile0Size))
        {
            ucFileId = MNTN_ERRLOR0_ID;
        }
        else
        {
            ucFileId = MNTN_ERRLOR1_ID;
        }
    }
    else    /* 当读取文件信息成功时, 查找出较晚生成的文件作为要写入的文件 */
    {
        /*slice计数是递减的，所有判断使用的是大于等于*/
        if ((stHeadInfo0.ulBoardtime > stHeadInfo1.ulBoardtime)
            || ((stHeadInfo0.ulBoardtime == stHeadInfo1.ulBoardtime)
            && (stHeadInfo1.ulSliceTime >=  stHeadInfo0.ulSliceTime)))
        {
            ucFileId = MNTN_ERRLOR0_ID;
        }
        else
        {
            ucFileId = MNTN_ERRLOR1_ID;
        }
    }

    DRV_FILE_CLOSE(lLogFile0);
    DRV_FILE_CLOSE(lLogFile1);
    return ucFileId;
}


VOS_VOID  MNTN_ErrLogInit(VOS_VOID)
{
    VOS_UINT32      ulBoardtime;
    VOS_CHAR        aucErrLogDir[100];

    g_stErrLogStru.ucInitSucc = ERRLOG_DISABLE;

    /*   获取使能ERRLOG的NV项   */
    if (NV_OK != NV_Read(en_NV_Item_ERRORLOG_ENABLEFLAG,
             (VOS_VOID *)&g_stErrLogStru.stEnableFlag, sizeof(OM_ERRORLOG_ENABLE_FLAG_STRU)))
    {
        vos_printf("MNTN_ErrLogInit:GET EnableFlag ERROR\n");
        return;
    }

    /*   获取flash刷写间隔的NV项   */
    if (NV_OK != NV_Read(en_NV_Item_ERRORLOG_FLUSHBUFINTERVAL,
            (VOS_VOID *)&g_stErrLogStru.stFlushInterval, sizeof(OM_ERRORLOG_FLUSHBUFINTER_STRU)))
    {
        vos_printf("MNTN_ErrLogInit:GET FlushInterval ERROR\n");
        g_stErrLogStru.stFlushInterval.ulErrorFlushInter = ERRLOG_WRITE_INTERVAL_DEFAULT_VALUE ;
    }

    /*   获取ERRLOG最大记录时间的NV项   */
    if (NV_OK != NV_Read(en_NV_Item_ERRORLOG_RECORDPERIOD,
            (VOS_VOID *)&g_stErrLogStru.stRecordPeriod, sizeof(OM_ERRORLOG_RECORDPERIOD_STRU)))
    {
        vos_printf("MNTN_ErrLogInit:GET RecordPeriod ERROR\n");
        g_stErrLogStru.stRecordPeriod.ulErrorRecord = ERRLOG_TOTAL_RECORD_TIME ;
    }

    /*   获取当前boardtime的值   */
    if ( VOS_OK != NV_Read(en_NV_Item_LiveTime,
            (VOS_VOID *)(&ulBoardtime), sizeof(VOS_UINT32)))
    {
        vos_printf("MNTN_ErrLogInit:Get boardtime error\n");
        ulBoardtime = 0;
    }
    /*   根据nv项的设置判断当前是否使能errlog机制,ulBoardtime单位为分钟，
    g_stErrLogStru.ulRecordPeriod单位为5分钟  */
    if((ERRLOG_DISABLE == g_stErrLogStru.stEnableFlag.usErrorLogEnable)
        || (ulBoardtime > (g_stErrLogStru.stRecordPeriod.ulErrorRecord * 5)))
    {
        vos_printf("MNTN_ErrLogInit:Errlog disable;ucEnableFlag=%d;curBoadtime=%d;NVBoadtime=%d\n",
             g_stErrLogStru.stEnableFlag.usErrorLogEnable,ulBoardtime,g_stErrLogStru.stRecordPeriod.ulErrorRecord);
        return ;
    }

    g_stErrLogStru.ulLastTick = VOS_GetTick();
    g_stErrLogStru.ucBufId    = 0;

    VOS_MemSet(g_stErrLogStru.aulRecordLen, 0, (sizeof(VOS_UINT32)*ERRLOG_BUF_COUNT));

    if (VOS_OK != OM_GetLogPath(aucErrLogDir, MNTN_ERRLOG_DIR, MNTN_ERRLOG_UNITARY_DIR))
    {
        return;
    }

    /* 创建Errlog文件存放目录 */
    if (VOS_OK != DRV_FILE_ACCESS(aucErrLogDir, 0))
    {
        if (DRV_ERROR == DRV_FILE_MKDIR(aucErrLogDir))
        {
            vos_printf("MNTN_ErrLogInit:Log folder create Error\n");
            return ;
        }
    }

    /*   创建互斥信号量，控制buf的互斥操作   */
    if (LOG_OK != LOG_CreateMutex(&g_stErrLogStru.semErrlog))
    {
        vos_printf("MNTN_ErrLogInit:Log: Error, semCreate Fail\n");
        return;
    }

    /*   创建互斥信号量，控制FileId   */
    if (LOG_OK != LOG_CreateMutex(&g_stErrLogStru.semFileId))
    {
        vos_printf("MNTN_ErrLogInit:Log: Error, semCreate Fail\n");
        return;
    }

    /*  获取当前写入的文件ID    */
    g_stErrLogStru.ucFileId = MNTN_GetFileId();

    g_stErrLogStru.ucInitSucc = ERRLOG_ENABLE;
    return;
}


void MNTN_WriteErrorLog(void)
{
    FILE*                           lLogFile;
    VOS_INT                         lFileSize;
    VOS_UINT32                      ulRecordLen;
    VOS_UINT8                       ucBufId;
    MNTN_VERSION_INFO_EVENT_STRU    stVerInfo;

    if(VOS_OK != Log_SmP(&g_stErrLogStru.semErrlog))
    {
        vos_printf("MNTN_WriteErrorLog:Take semphore Error\n");
        return;
    }

    ulRecordLen = g_stErrLogStru.aulRecordLen[g_stErrLogStru.ucBufId];

    /*buf中没有记录则直接返回*/
    if (0 == ulRecordLen)
    {
        Log_SmV(&g_stErrLogStru.semErrlog);
        return;
    }

    ucBufId = g_stErrLogStru.ucBufId;

    /*切换buf ID*/
    g_stErrLogStru.ucBufId = (g_stErrLogStru.ucBufId + 1)%2;

    Log_SmV(&g_stErrLogStru.semErrlog);

    /*Errlog.bin文件不存在，则创建*/
    lLogFile = MNTN_OpenErrorLog(g_stErrLogStru.ucFileId, DRV_O_CREAT|DRV_O_APPEND|DRV_O_RDWR);

    if(DRV_FILE_NULL == lLogFile)
    {
        vos_printf("MNTN_WriteErrorLog:fopen fd file error\n");
        return;
    }

    /*获取该文件长度*/
    lFileSize = (VOS_INT)DRV_FILE_LSEEK(lLogFile, 0, DRV_SEEK_END);
    if (DRV_ERROR == lFileSize)
    {
        DRV_FILE_CLOSE(lLogFile);
        vos_printf("MNTN_WriteErrorLog:LSEEK error\n");
        return;
    }

    lFileSize = (VOS_INT)DRV_FILE_TELL(lLogFile);

    /*如果长度为0，则写入版本信息*/
    if (0 == lFileSize)
    {
        /*获取版本信息,写入flash*/
        MNTN_GetHeadVersion(&stVerInfo);
        if (DRV_ERROR == DRV_FILE_WRITE((VOS_CHAR *)&stVerInfo, sizeof(VOS_CHAR),
                            stVerInfo.stRecordHead.ulInfoLen, lLogFile))
        {
            vos_printf("MNTN_WriteErrorLog:write version info error\n");
        }
    }

    if(VOS_OK != Log_SmP(&g_stErrLogStru.semFileId))
    {
        DRV_FILE_CLOSE(lLogFile);
        vos_printf("MNTN_WriteErrorLog:Take semphore Error\n");
        return;
    }

    if (MNTN_ERRLOG_MAXSIZE < lFileSize)    /* 当前文件满 */
    {
        DRV_FILE_CLOSE(lLogFile);
        lLogFile = DRV_FILE_NULL;

        /* 交换文件ID */
        g_stErrLogStru.ucFileId = (g_stErrLogStru.ucFileId + 1)%2;

        /* 若文件不存在, 则创建文件 */
        lLogFile = MNTN_OpenErrorLog(g_stErrLogStru.ucFileId, DRV_O_CREAT|DRV_O_TRUNC|DRV_O_RDWR);
        if (DRV_FILE_NULL == lLogFile)
        {
            vos_printf("MNTN_WriteErrorLog:fopen fd file error\n");
            Log_SmV(&g_stErrLogStru.semFileId);
            return;
        }
        else
        {
            /*文件创建后, 获取版本信息, 写入flash*/
            MNTN_GetHeadVersion(&stVerInfo);
            if (DRV_ERROR == DRV_FILE_WRITE((VOS_CHAR *)&stVerInfo, sizeof(VOS_CHAR),
                               stVerInfo.stRecordHead.ulInfoLen, lLogFile))
            {
                vos_printf("MNTN_WriteErrorLog:write version info error\n");
            }
        }
    }
    Log_SmV(&g_stErrLogStru.semFileId);

    /* 将buf中的记录全部写入文件中 */
    if (DRV_ERROR == DRV_FILE_WRITE(g_stErrLogStru.acbuf[ucBufId], sizeof(VOS_CHAR), ulRecordLen, lLogFile))
    {
        vos_printf("MNTN_WriteErrorLog:write Errlog.bin error\n");
    }

    DRV_FILE_CLOSE(lLogFile);

    g_stErrLogStru.aulRecordLen[ucBufId] = 0;
    g_stErrLogStru.ulLastTick = VOS_GetTick();

    return;
}
void MNTN_ErrorLogWriteFile(void)
{
    if(ERRLOG_DISABLE == g_stErrLogStru.ucInitSucc)
    {
        return;
    }

    if(VOS_TRUE == g_ulImmediatelyWriteFlg)
    {
        g_ulImmediatelyWriteFlg = VOS_FALSE;
        MNTN_WriteErrorLog();

        return;
    }

    /*与上一次写Flash时间间隔小于NV项设置，则本次不写Flash*/
    if(VOS_GetTick() < (g_stErrLogStru.stFlushInterval.ulErrorFlushInter + g_stErrLogStru.ulLastTick))
    {
        return;
    }

    MNTN_WriteErrorLog();
    return;
}

/*****************************************************************************
 函 数 名  : LOG_GetTick
 功能描述  : 获取当前CPU的Tick时间
 输入参数  : 无
 输出参数  : 无
 返 回 值  : LOG_ERR - 操作失败
             LOG_OK  - 操作成功


 修改历史      :
  1.日    期   : 2008年7月17日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
unsigned long LOG_GetTick(void)
{
#if ((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
/*lint -e718*/
/*lint -e746*/

    /*在ASIC/FPGA平台下，需要将Slice值转换成tick值，保证和SDT的显示一致*/
    unsigned long   ulSlice;
    ulSlice = OM_GetSlice();
    /*Slice值每隔一秒增加32768，通过以下计算转换成10ms的tick值
    而先右移7位，再乘以100，是为了防止数据过大而溢出*/
    ulSlice >>= 7;
    ulSlice *= 100;
    ulSlice >>= 8;
    return ulSlice;

/*lint +e718*/
/*lint +e746*/

#elif(VOS_OS_VER == VOS_WIN32)
    return (unsigned long)GetTickCount();
#else
    return 0;
#endif
}

/*****************************************************************************
 函 数 名  : Log_GetPrintLevel
 功能描述  : 得到模块Id、子模块Id在打印级别记录表中的索引号
 输入参数  : LOG_MODULE_ID_EN enModuleId
             LOG_SUBMOD_ID_EN enSubModId
 输出参数  : 无
 返 回 值  : unsigned long
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数

  2.日    期   : 2008年9月12日
    作    者   : 甘兰
    修改内容   : 可维可测第三阶段需求

*****************************************************************************/
unsigned long Log_GetPrintLevel(VOS_UINT32 ulModuleId)
{
    /*在配置为OM输出时，不输出OM模块的打印信息，否则有可能会造成死循环*/
    if ((WUEPS_PID_MUX == ulModuleId) && (OM_OUTPUT_SDT == g_stLogEnt.ulLogOutput))
    {
        return LOG_LEVEL_OFF;
    }
    /*判断模块ID是否在CCPU支持的PS范围内*/
    if ((VOS_PID_DOPRAEND <= ulModuleId) && (VOS_PID_BUTT > ulModuleId))
    {
        return g_aulLogPrintLevPsTable[ulModuleId - VOS_PID_DOPRAEND]; /* [false alarm]: 屏蔽Fortify 错误 */
    }
    /*判断模块ID是否在CCPU支持的DRV范围内*/
    if ((LOG_MIN_MODULE_ID_DRV <= ulModuleId) && (LOG_MAX_MODULE_ID_DRV >= ulModuleId))
    {
        return g_aulLogPrintLevDrvTable[ulModuleId - LOG_MIN_MODULE_ID_DRV]; /* [false alarm]: 屏蔽Fortify 错误 */
    }
    return LOG_LEVEL_OFF;
}
/*****************************************************************************
 函 数 名  : Log_GetPathOffset
 功能描述  : 得到文件路径名的偏移值
 输入参数  : char* pcFileName
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数

*****************************************************************************/
long Log_GetPathOffset(char* pcFileName)
{
    long     lOffset;
    long     lOffset1;
    long     lOffset2;
    char    *pcPathPos;

    lOffset1  = 0;
    lOffset2  = 0;

    /* 操作系统可能使用'\'来查找路径 */
    pcPathPos = (char*)strrchr(pcFileName, '\\');
    if (LOG_NULL_PTR != pcPathPos)
    {
        lOffset1 = (pcPathPos - pcFileName) + 1;
    }

    /* 操作系统可能使用'/'来查找路径 */
    pcPathPos = (char*)strrchr(pcFileName, '/');
    if (LOG_NULL_PTR != pcPathPos)
    {
        lOffset2 = (pcPathPos - pcFileName) + 1;
    }

    lOffset = (lOffset1 > lOffset2) ? lOffset1 : lOffset2;

    return lOffset;
}

/*****************************************************************************
 函 数 名  : Log_Init
 功能描述  : 打印初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数

*****************************************************************************/
void Log_Init(void)
{
    g_stLogEnt.ulPrintSwitch = LOG_FALSE;

    if (LOG_OK != LOG_CreateMutex(&g_logBuffSem))
    {
        vos_printf("Log: Error, semMCreate Fail");
        return;
    }

    if (LOG_OK != LOG_CreateSemaphore(&(g_stLogEnt.semOmPrint)))
    {
        LOG_DeleteSemaphore(&g_logBuffSem);
        vos_printf("Log: Error, semCCreate Fail");
        return;
    }

    g_stLogEnt.rngOmRbufId = OM_RingBufferCreate(LOG_BUF_VOLUMN);
    if (LOG_NULL_PTR == g_stLogEnt.rngOmRbufId)
    {
        LOG_DeleteSemaphore(&g_logBuffSem);
        LOG_DeleteSemaphore(&g_stLogEnt.semOmPrint);
        vos_printf("Log: Error, rngCreate Fail");
        return;
    }

    g_stLogEnt.ulPrintSwitch = LOG_TRUE;

    /* 注册底软的回调函数，定义OM_Log...... */
#ifdef __LOG_BBIT__
    LOG_RegisterDrv((LOG_PFUN)OM_Log, (LOG_PFUN)OM_Log1, (LOG_PFUN)OM_Log2,
                     (LOG_PFUN)OM_Log3, (LOG_PFUN)OM_Log4);
#endif

#ifdef __LOG_RELEASE__
    LOG_RegisterDrv((LOG_PFUN)OM_LogId, (LOG_PFUN)OM_LogId1, (LOG_PFUN)OM_LogId2,
                    (LOG_PFUN)OM_LogId3, (LOG_PFUN)OM_LogId4);
#endif

    MNTN_ERRLOG_REG_FUNC((RECORD_ERRLOG_PFUN)MNTN_ErrorLog);

    return;
}

#ifdef __LOG_BBIT__

/*****************************************************************************
 函 数 名  : OM_Log
 功能描述  : 字符串类型的打印接口函数（无参数）
 输入参数  : char             *cFileName
             unsigned long      ulLineNum
             LOG_MODULE_ID_EN  enModuleId
             LOG_SUBMOD_ID_EN   enSubModId
             LOG_LEVEL_EN      enLevel
             char              *pcString
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数

*****************************************************************************/
void OM_Log(char             *cFileName,  unsigned long      ulLineNum,
            LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
            LOG_LEVEL_EN      enLevel,    char              *pcString)
{
    Log_StrNParam(cFileName, ulLineNum, enModuleId, enSubModId, enLevel,
            pcString, 0, LOG_NULL_PTR);
    return;
}

/*****************************************************************************
 函 数 名  : OM_Log1
 功能描述  : 字符串类型的打印接口函数（1 个参数）
 输入参数  : char             *cFileName
             unsigned long      ulLineNum
             LOG_MODULE_ID_EN  enModuleId
             LOG_SUBMOD_ID_EN   enSubModId
             LOG_LEVEL_EN      enLevel
             char              *pcString
             long              lPara1
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数

*****************************************************************************/
void OM_Log1(char             *cFileName,  unsigned long      ulLineNum,
             LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
             LOG_LEVEL_EN      enLevel,    char              *pcString,
             long              lPara1)
{
    long alParam[4];
    alParam[0] = lPara1;

    Log_StrNParam(cFileName, ulLineNum, enModuleId, enSubModId, enLevel,
            pcString, 1, alParam);
    return;
}

/*****************************************************************************
 函 数 名  : OM_Log2
 功能描述  : 字符串类型的打印接口函数（2个参数）
 输入参数  : char             *cFileName
             unsigned long      ulLineNum
             LOG_MODULE_ID_EN  enModuleId
             LOG_SUBMOD_ID_EN   enSubModId
             LOG_LEVEL_EN      enLevel
             char              *pcString
             long              lPara1
             long               lPara2
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数

*****************************************************************************/
void OM_Log2(char             *cFileName,  unsigned long      ulLineNum,
             LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
             LOG_LEVEL_EN      enLevel,    char              *pcString,
             long              lPara1,     long               lPara2)
{
    long alParam[4];

    alParam[0] = lPara1;
    alParam[1] = lPara2;

    Log_StrNParam(cFileName, ulLineNum, enModuleId, enSubModId, enLevel,
            pcString, 2, alParam);

    return;
}

/*****************************************************************************
 函 数 名  : OM_Log3
 功能描述  : 字符串类型的打印接口函数（3 个参数）
 输入参数  : char             *cFileName
             unsigned long      ulLineNum
             LOG_MODULE_ID_EN  enModuleId
             LOG_SUBMOD_ID_EN   enSubModId
             LOG_LEVEL_EN      enLevel
             char              *pcString
             long              lPara1
             long               lPara2
             long              lPara3
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数

*****************************************************************************/
void OM_Log3(char             *cFileName,  unsigned long      ulLineNum,
             LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
             LOG_LEVEL_EN      enLevel,    char              *pcString,
             long              lPara1,     long               lPara2,
             long              lPara3)
{
    long alParam[4];

    alParam[0] = lPara1;
    alParam[1] = lPara2;
    alParam[2] = lPara3;

    Log_StrNParam(cFileName, ulLineNum, enModuleId, enSubModId, enLevel,
            pcString, 3, alParam);

    return;
}

/*****************************************************************************
 函 数 名  : OM_Log4
 功能描述  : 字符串类型的打印接口函数（4 个参数）
 输入参数  : char             *cFileName
                         unsigned long      ulLineNum
                         LOG_MODULE_ID_EN  enModuleId
                         LOG_SUBMOD_ID_EN   enSubModId
                         LOG_LEVEL_EN      enLevel
                         char              *pcString
                         long              lPara1
                         long               lPara2
                         long              lPara3
                         long               lPara4
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void OM_Log4(char             *cFileName,  unsigned long      ulLineNum,
             LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
             LOG_LEVEL_EN      enLevel,    char              *pcString,
             long              lPara1,     long               lPara2,
             long              lPara3,     long               lPara4)
{
    long alParam[4];

    alParam[0] = lPara1;
    alParam[1] = lPara2;
    alParam[2] = lPara3;
    alParam[3] = lPara4;

    Log_StrNParam(cFileName, ulLineNum, enModuleId, enSubModId, enLevel,
            pcString, 4, alParam);

    return;
}

/*****************************************************************************
 函 数 名  : Log_BuildStr
 功能描述  : 构建字符串类型的打印输出信息
 输入参数  : char          *pcFileName
                         unsigned long  ulLineNum
                         LOG_LEVEL_EN   enPrintLev
                         char          *pcOriStr
                         unsigned char  ucParaCnt
                         long          *plPara
                         char          *pcDstStr
                         unsigned long *pulLen
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void Log_BuildStr(char          *pcFileName,  unsigned long  ulLineNum,
                  LOG_LEVEL_EN   enPrintLev,  char          *pcOriStr,
                  unsigned char  ucParaCnt,   long          *plPara,
                  char          *pcDstStr,    unsigned long *pulLen)
{
    long            lTmpLen;
    long            lStrLen;
    long            lOffset;
    long            lParamLen;
    long            lSpareLen;
    long            lOccupyLen;
    unsigned char   i;

    *pulLen = 0;

    switch ( enPrintLev )
    {
        case LOG_LEVEL_ERROR :
            lTmpLen    = LOG_SNPRINTF( pcDstStr, LOG_MAX_COLUMN_VAL, "\nERROR,F: ");
            break;
        case LOG_LEVEL_WARNING :
            lTmpLen    = LOG_SNPRINTF( pcDstStr, LOG_MAX_COLUMN_VAL, "\nWARNING,F: ");
            break;
        case LOG_LEVEL_NORMAL :
            lTmpLen    = LOG_SNPRINTF( pcDstStr, LOG_MAX_COLUMN_VAL, "\nNORMAL,F: ");
            break;
        case LOG_LEVEL_INFO :
            lTmpLen    = LOG_SNPRINTF( pcDstStr, LOG_MAX_COLUMN_VAL, "\nINFO,F: ");
            break;
        default:
            lTmpLen    = LOG_SNPRINTF( pcDstStr, LOG_MAX_COLUMN_VAL, "\nF: ");
            break;
    }

    lOccupyLen = lTmpLen;
    LOG_AFFIRM(LOG_MAX_COLUMN_VAL >= lOccupyLen)

    lSpareLen  = LOG_MAX_COLUMN_VAL - lOccupyLen;
    LOG_AFFIRM(lSpareLen >= LOG_MAX_FILENAME_LEN)

        /* 将路径去除,只保留文件名称 */
    lOffset = Log_GetPathOffset(pcFileName);
    lTmpLen = (long)strlen(pcFileName + lOffset);
    if (lTmpLen < LOG_MAX_FILENAME_LEN)
    {
        memcpy(pcDstStr + lOccupyLen, pcFileName + lOffset, (unsigned int)lTmpLen);
        lOccupyLen += lTmpLen;
    }
    else
    {
        memcpy(pcDstStr + lOccupyLen, pcFileName + lOffset, LOG_MAX_FILENAME_LEN);
        lOccupyLen += LOG_MAX_FILENAME_LEN;
    }

    lSpareLen   = LOG_MAX_COLUMN_VAL - lOccupyLen;

    lTmpLen = LOG_SNPRINTF( pcDstStr + lOccupyLen, (unsigned int)lSpareLen,
                           ", Line: %u, Tick: %u, ", ulLineNum, LOG_GetTick());

    lOccupyLen += lTmpLen;
    lSpareLen   = LOG_MAX_COLUMN_VAL - lOccupyLen;

        /*=======================*/ /* 计算输出参数需要的长度 */
    lParamLen = ucParaCnt * LOG_MAX_PARA_LEN;
    lStrLen   = lSpareLen - lParamLen;
    LOG_AFFIRM(0 <= lStrLen)

        /*=======================*/ /* 加入字符串信息 */
    lTmpLen = (long)strlen(pcOriStr);
    if (lTmpLen <= lStrLen)
    {
        memcpy(pcDstStr + lOccupyLen, pcOriStr, (unsigned int)lTmpLen);
        lOccupyLen += lTmpLen;
    }
    else
    {
        memcpy(pcDstStr + lOccupyLen, pcOriStr, (unsigned int)lStrLen);
        lOccupyLen += lStrLen;
    }

    if (0 == ucParaCnt)
    {
        if (LOG_MAX_COLUMN_VAL == lOccupyLen)
        {
            *(pcDstStr + (lOccupyLen - 1)) = '\0';
        }
        else
        {
            *(pcDstStr + lOccupyLen) = '\0';
            lOccupyLen = lOccupyLen + 1;
        }

        LOG_AFFIRM(LOG_MAX_COLUMN_VAL >= lOccupyLen)
        *pulLen = (unsigned long)lOccupyLen;
        return;
    }

    *(pcDstStr + lOccupyLen) = ':';
    lOccupyLen = lOccupyLen + 1;
    LOG_AFFIRM(LOG_MAX_COLUMN_VAL > lOccupyLen)

    lSpareLen = LOG_MAX_COLUMN_VAL - lOccupyLen;

        /*=======================*/ /* 加入参数 */
    for (i = 0; i < ucParaCnt; i++)
    {
        lTmpLen = LOG_SNPRINTF( pcDstStr + lOccupyLen, (unsigned int)lSpareLen, " %d.", *(plPara + i));
        lOccupyLen += lTmpLen;
        LOG_AFFIRM(LOG_MAX_COLUMN_VAL >= lOccupyLen)

        lSpareLen  = LOG_MAX_COLUMN_VAL - lOccupyLen;
    }

    *pulLen = (unsigned long)(LOG_MAX_COLUMN_VAL - lSpareLen);
    return;
}

/*****************************************************************************
 函 数 名  : Log_StrNParam
 功能描述  : 字符串类型的打印函数（N 个参数）
 输入参数  : char             *cFileName
                         unsigned long      ulLineNum
                         LOG_MODULE_ID_EN  enModuleId
                         LOG_SUBMOD_ID_EN   enSubModId
                         LOG_LEVEL_EN      enLevel
                         char              *pcOriStr
                         unsigned char     ucParaCnt
                         long              *plPara
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
int Log_StrNParam(char             *cFileName,  unsigned long      ulLineNum,
             LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN   enSubModId,
             LOG_LEVEL_EN      enLevel,    char              *pcOriStr,
             unsigned char     ucParaCnt,  long              *plPara)
{
    unsigned long     ulLen;
    int               ret = VOS_ERR;

    /*lint -e813*/
    char              acLogStr[LOG_MAX_COLUMN_VAL_EX + 1];
    /*lint +e813*/

    if (LOG_TRUE != g_stLogEnt.ulPrintSwitch)
    {
        return ret;
    }

    if (enLevel > Log_GetPrintLevel(enModuleId))
    {
        return ret;
    }

#if(VOS_OS_VER == VOS_WIN32)

    switch ( enLevel )
    {
        case LOG_LEVEL_ERROR :
            vos_printf("\r\nERROR,");
            break;
        case LOG_LEVEL_WARNING :
            vos_printf("\r\nWARNING,");
            break;
        case LOG_LEVEL_NORMAL :
            vos_printf("\r\nNORMAL,");
            break;
        case LOG_LEVEL_INFO :
            vos_printf("\r\nINFO,");
            break;
        default:
            break;
    }
    vos_printf("%s, L: %d, T: %u, %s", (cFileName + Log_GetPathOffset(cFileName)), ulLineNum, LOG_GetTick(), pcOriStr);

    for (ulLen = 0; ulLen < ucParaCnt; ulLen++)
    {
        vos_printf(",%d ", plPara[ulLen]);
    }
#else
    *((VOS_UINT32*)acLogStr) = (VOS_UINT32)enModuleId;
    *(VOS_UINT32*)(acLogStr + sizeof(VOS_UINT32)) = (VOS_UINT32)enLevel;
    /*加上LOG_MODULE_ID_LEN是由于在LOG内容的头部添加了Module ID
      而造成的偏移*/
    Log_BuildStr(cFileName, ulLineNum, enLevel,  pcOriStr,
                 ucParaCnt, plPara,    acLogStr + LOG_MODULE_ID_LEN, &ulLen);
    ret = Log_BufInput(acLogStr, ulLen + LOG_MODULE_ID_LEN);
#endif

    return ret;
}
#endif

#ifdef __LOG_RELEASE__

/*****************************************************************************
 函 数 名  : OM_LogId
 功能描述  : 打印点类型的打印接口函数（无参数）
 输入参数  : LOG_MODULE_ID_EN  enModuleId
                         LOG_SUBMOD_ID_EN  enSubModId
                         LOG_LEVEL_EN      enLevel
                         unsigned long     ulLogId
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void OM_LogId(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
              LOG_LEVEL_EN      enLevel,    unsigned long     ulLogId)
{
    Log_IdNParam(enModuleId, enSubModId, enLevel, ulLogId, 0, LOG_NULL_PTR);
    return;
}

/*****************************************************************************
 函 数 名  : OM_LogId1
 功能描述  : 打印点类型的打印接口函数（1 个参数）
 输入参数  : LOG_MODULE_ID_EN  enModuleId
                         LOG_SUBMOD_ID_EN  enSubModId
                         LOG_LEVEL_EN      enLevel
                         unsigned long     ulLogId
                         long              lPara1
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void OM_LogId1(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
               LOG_LEVEL_EN      enLevel,    unsigned long     ulLogId,
               long              lPara1)
{
    long alParam[4];
    alParam[0] = lPara1;

    Log_IdNParam(enModuleId, enSubModId, enLevel, ulLogId, 1, alParam);
    return;
}

/*****************************************************************************
 函 数 名  : OM_LogId2
 功能描述  : 打印点类型的打印接口函数（2 个参数）
 输入参数  : LOG_MODULE_ID_EN  enModuleId
                         LOG_SUBMOD_ID_EN  enSubModId
                         LOG_LEVEL_EN      enLevel
                         unsigned long     ulLogId
                         long              lPara1
                         long              lPara2
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void OM_LogId2(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
               LOG_LEVEL_EN      enLevel,    unsigned long     ulLogId,
               long              lPara1,     long              lPara2)
{
    long alParam[4];

    alParam[0] = lPara1;
    alParam[1] = lPara2;

    Log_IdNParam(enModuleId, enSubModId, enLevel, ulLogId, 2, alParam);

    return;
}

/*****************************************************************************
 函 数 名  : OM_LogId3
 功能描述  : 打印点类型的打印接口函数（3 个参数）
 输入参数  : LOG_MODULE_ID_EN  enModuleId
                         LOG_SUBMOD_ID_EN  enSubModId
                         LOG_LEVEL_EN      enLevel
                         unsigned long     ulLogId
                         long              lPara1
                         long              lPara2
                         long              lPara3
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void OM_LogId3(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
               LOG_LEVEL_EN      enLevel,    unsigned long     ulLogId,
               long              lPara1,     long              lPara2,
               long              lPara3)
{
    long alParam[4];

    alParam[0] = lPara1;
    alParam[1] = lPara2;
    alParam[2] = lPara3;

    Log_IdNParam(enModuleId, enSubModId, enLevel, ulLogId, 3, alParam);

    return;
}

/*****************************************************************************
 函 数 名  : OM_LogId4
 功能描述  : 打印点类型的打印接口函数（4 个参数）
 输入参数  : LOG_MODULE_ID_EN  enModuleId
                         LOG_SUBMOD_ID_EN  enSubModId
                         LOG_LEVEL_EN      enLevel
                         unsigned long     ulLogId
                         long              lPara1
                         long              lPara2
                         long              lPara3
                         long              lPara4
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void OM_LogId4(LOG_MODULE_ID_EN  enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
               LOG_LEVEL_EN      enLevel,    unsigned long     ulLogId,
               long              lPara1,     long              lPara2,
               long              lPara3,     long              lPara4)
{
    long alParam[4];

    alParam[0] = lPara1;
    alParam[1] = lPara2;
    alParam[2] = lPara3;
    alParam[3] = lPara4;

    Log_IdNParam(enModuleId, enSubModId, enLevel, ulLogId, 4, alParam);

    return;
}

/*****************************************************************************
 函 数 名  : Log_BuildId
 功能描述  : 构建打印点类型的打印输出信息
 输入参数  : unsigned long  ulLogId
                         unsigned char  ucParaCnt
                         long *plPara
                         char          *pcDst
                         unsigned long *pulLen
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void Log_BuildId(unsigned long  ulLogId, unsigned char  ucParaCnt, long *plPara,
                    char          *pcDst,   unsigned long *pulLen)
{
    long             lTmpLen;
    long             lSpareLen;
    long             lOccupyLen;
    unsigned char    i;

    *pulLen = 0;

    if (ucParaCnt > LOG_MAX_PARA_CNT)
    {
        ucParaCnt = LOG_MAX_PARA_CNT;
    }

    lTmpLen    = LOG_SNPRINTF( pcDst, LOG_MAX_COLUMN_VAL, "%d, ", ulLogId);
    LOG_AFFIRM (0 <= lTmpLen)

    lOccupyLen = lTmpLen;
    LOG_AFFIRM(LOG_MAX_COLUMN_VAL >= lOccupyLen)
    lSpareLen  = LOG_MAX_COLUMN_VAL - lOccupyLen;

    lTmpLen    = LOG_SNPRINTF( pcDst + lOccupyLen, lSpareLen, "%u, ", LOG_GetTick());
    LOG_AFFIRM(0 <= lTmpLen)

    lOccupyLen += lTmpLen;
    LOG_AFFIRM(LOG_MAX_COLUMN_VAL >= lOccupyLen)
    lSpareLen  = LOG_MAX_COLUMN_VAL - lOccupyLen;

    for (i = 0; i < ucParaCnt; i ++)
    {
        lTmpLen = LOG_SNPRINTF( pcDst + lOccupyLen, lSpareLen, "%d, ", *plPara);
        LOG_AFFIRM(0 <= lTmpLen)

        lOccupyLen += lTmpLen;
        LOG_AFFIRM(LOG_MAX_COLUMN_VAL >= lOccupyLen)
        lSpareLen  = LOG_MAX_COLUMN_VAL - lOccupyLen;
        plPara++;
    }

    *pulLen = (unsigned long)(LOG_MAX_COLUMN_VAL - lSpareLen);

    return;
}

/*****************************************************************************
 函 数 名  : Log_IdNParam
 功能描述  : 打印点类型的打印函数（N 个参数）
 输入参数  : LOG_MODULE_ID_EN enModuleId
                         LOG_SUBMOD_ID_EN  enSubModId
                         LOG_LEVEL_EN     enLevel
                         unsigned long     ulLogId
                         unsigned char    ucParaCnt
                         long             *plPara
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
int Log_IdNParam(LOG_MODULE_ID_EN enModuleId, LOG_SUBMOD_ID_EN  enSubModId,
               LOG_LEVEL_EN     enLevel,    unsigned long     ulLogId,
               unsigned char    ucParaCnt,  long             *plPara)
{
    unsigned long     ulLen;
    int               ret = VOS_ERR;

/*lint -e813*/
    char              acLogStr[LOG_MAX_COLUMN_VAL_EX + 1];
/*lint +e813*/

    if (LOG_TRUE != g_stLogEnt.ulPrintSwitch)
    {
        return ret;
    }

    if (enLevel > Log_GetPrintLevel(enModuleId))
    {
        return ret;
    }

    *((VOS_UINT32*)acLogStr) = (VOS_UINT32)enModuleId;
    *(VOS_UINT32*)(acLogStr + sizeof(VOS_UINT32)) = (VOS_UINT32)enLevel;
    /*加上LOG_MODULE_ID_LEN是由于在LOG内容的头部添加了Module ID
      而造成的偏移*/
    Log_BuildId(ulLogId, ucParaCnt, plPara, acLogStr + LOG_MODULE_ID_LEN, &ulLen);
    ret = Log_BufInput(acLogStr, ulLen + LOG_MODULE_ID_LEN);

    return ret;
}
#endif

/*****************************************************************************
 函 数 名  : Log_BufInput
 功能描述  : 将打印信息写入缓存
 输入参数  : char *pcLogStr
                         unsigned long ulLen
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
int Log_BufInput(char *pcLogStr, unsigned long ulLen)
{
    int     sRet;

    if (0 < ulLen)
    {
        if(VOS_OK != Log_SmP(&g_logBuffSem))
        {
            return VOS_ERR;
        }

        if ((ulLen + sizeof(unsigned long)) > (unsigned long)OM_RingBufferFreeBytes(g_stLogEnt.rngOmRbufId))
        {
            Log_SmV(&g_logBuffSem);
            return VOS_ERR;
        }

        sRet = OM_RingBufferPut(g_stLogEnt.rngOmRbufId, (char*)(&ulLen), sizeof(unsigned long));
        if ((int)sizeof(unsigned long) == sRet)
        {
            sRet = OM_RingBufferPut(g_stLogEnt.rngOmRbufId, pcLogStr, (long)ulLen);
            Log_SmV(&g_logBuffSem);

            if (ulLen == (unsigned long)sRet)
            {
                Log_SmV(&(g_stLogEnt.semOmPrint));
                return VOS_OK ;
            }
            else
            {
                vos_printf("\nLog, Error, rngBufPut Data not OK");
            }
        }
        else
        {
            Log_SmV(&g_logBuffSem);
            vos_printf("\nLog, Error, rngBufPut DataLen not OK");
        }
    }
    return VOS_ERR;
}

/*****************************************************************************
 函 数 名  : Log_SelfTask
 功能描述  : 打印自处理任务
 输入参数  : unsigned long ulPara1
                         unsigned long ulPara2
                         unsigned long ulPara3
                         unsigned long ulPara4
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
        作    者   : 李霄 46160
        修改内容   : 新生成函数

*****************************************************************************/
void Log_SelfTask(unsigned long ulPara1, unsigned long ulPara2,
                  unsigned long ulPara3, unsigned long ulPara4)
{
    int             sRet;
    unsigned long   ulLen;
/*lint -e813*/
    char            acTmpStr[LOG_MAX_COLUMN_VAL_EX + 1];
/*lint +e813*/
    Log_Init();

    for(;;)
    {
        if(VOS_OK != Log_SmP(&g_stLogEnt.semOmPrint))
        {
            continue;
        }

        MNTN_ErrorLogWriteFile();

        if (LOG_NULL_PTR == g_stLogEnt.rngOmRbufId)
        {
            continue;
        }

        if (LOG_TRUE == OM_RingBufferIsEmpty(g_stLogEnt.rngOmRbufId))
        {
            continue;
        }

        sRet = OM_RingBufferGet(g_stLogEnt.rngOmRbufId, (char*)(&ulLen), sizeof(unsigned long));
        if (sizeof(unsigned long) != (unsigned long)sRet)
        {
            continue;
        }

        if(LOG_MAX_COLUMN_VAL_EX < ulLen)
        {
            OM_RingBufferFlush(g_stLogEnt.rngOmRbufId);

            continue;
        }

        sRet = OM_RingBufferGet(g_stLogEnt.rngOmRbufId, acTmpStr, (long)ulLen);
        if (ulLen == (unsigned long)sRet)
        {
            Log_Output(g_stLogEnt.ulLogOutput, acTmpStr, ulLen);
        }
    }
}
/*****************************************************************************
 函 数 名  : Log_Output
 功能描述  : 打印输出函数（输出到串口 、写入Flash）
 输入参数  : LOG_OUTPUT_EN enOutputType
             char *pcStr
             unsigned long ulLen
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数

*****************************************************************************/
void Log_Output(unsigned long ulOutputType, char *pcStr, unsigned long ulLen)
{
    if (ulLen < LOG_MAX_COLUMN_VAL_EX)
    {
        pcStr[ulLen] = '\0';
    }
    else
    {
        pcStr[LOG_MAX_COLUMN_VAL_EX] = '\0';
    }

    switch (ulOutputType)
    {
        case OM_OUTPUT_SHELL:
#if (VOS_WIN32 == VOS_OS_VER)
            vos_printf("%s\n", pcStr + LOG_MODULE_ID_LEN);
#else
                        /*=======================*/ /* 将字符串输出到串口 */
            vos_printf("%s\n", pcStr + LOG_MODULE_ID_LEN);
#endif

            break;

        case OM_OUTPUT_SDT:
            OM_SendLog((unsigned char*)pcStr, ulLen);
            break;
        default:
            break;
    }

    return;
}

/*****************************************************************************
 函 数 名  : Log_SetOutputType
 功能描述  : 调整打印输出位置（串口、FileSystem, OM）的接口控制函数
 输入参数  : OM_OUTPUT_PORT_ENUM_UINT32 enOutputType
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年4月24日
    作    者   : 李霄 46160
    修改内容   : 新生成函数
  2.日    期   : 2008年9月9日
    作    者   : 甘兰 47350
    修改内容   : 添加可维可测第三阶段需求
*****************************************************************************/
VOS_UINT32 Log_SetOutputType(OM_OUTPUT_PORT_ENUM_UINT32 enOutputType)
{
    /*参数合法性检查*/
    if (OM_OUTPUT_BUTT <= enOutputType)
    {
        return VOS_ERR;
    }
/*由于工具侧不支持字符串输出方式，需要返回失败*/
#ifdef __LOG_BBIT__
    if (OM_OUTPUT_SDT == enOutputType)
    {
        return VOS_ERR;
    }
#endif
    g_stLogEnt.ulLogOutput = enOutputType;
    return VOS_OK;
}
/*****************************************************************************
 函 数 名  : Log_CheckPara
 功能描述  : 检测设置打印级别参数的合法性
 输入参数  : pstLogIdLevel
             ulLength
 输出参数  : 无
 返 回 值  : VOS_OK  - 成功
             VOS_ERR - 失败

 修改历史      :
  1.日    期   : 2008年9月9日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数,添加可维可测第三阶段需求
*****************************************************************************/
VOS_UINT32 Log_CheckPara(LOG_ID_LEVEL_STRU *pstLogIdLevel, VOS_UINT32 ulLength)
{
    VOS_UINT32 ulModuleNum;

    ulModuleNum = pstLogIdLevel->ulModuleNum;

    /*判断个数和长度间的关系是否正确*/
    if (ulLength != (sizeof(ulModuleNum)
             + (ulModuleNum*sizeof(LOG_MODULE_LEVEL_STRU))))
    {
        vos_printf("Log_CheckPara: Received length isn't correct.\n");
        return VOS_ERR;
    }

    return VOS_OK;
}
/*****************************************************************************
 函 数 名  : Log_SetModuleIdLev
 功能描述  : 设置模块的打印级别
 输入参数  : enModuleId - 模块ID号
             enSubModId - 子模块ID号，目前没有用到
             enLevel    - 打印级别
 输出参数  : 无
 返 回 值  : void

 修改历史      :
  1.日    期   : 2008年9月9日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数,添加可维可测第三阶段需求
*****************************************************************************/
void Log_SetModuleIdLev(LOG_MODULE_ID_EN enModuleId, LOG_SUBMOD_ID_EN enSubModId,
                                LOG_LEVEL_EN enLevel)
{
    /*lint -e662 -e661*/
    /*当打印级别不合法时，返回失败*/
    if (LOG_LEVEL_BUTT <= enLevel)
    {
        vos_printf("\r\n  Log_SetModuleIdLev: Print level isn't corret. \r\n");
        return;
    }

    if (LOG_MIN_MODULE_ID_DRV <= enModuleId && LOG_MAX_MODULE_ID_DRV >= enModuleId)
    {
        g_aulLogPrintLevDrvTable[enModuleId - LOG_MIN_MODULE_ID_DRV] = enLevel;
    }
    else if ((VOS_PID_DOPRAEND <= enModuleId) && (VOS_PID_BUTT > enModuleId))
    {
        g_aulLogPrintLevPsTable[enModuleId - VOS_PID_DOPRAEND] = enLevel;
    }
    else
    {
        vos_printf("Log_SetModuleIdLev: enModuleId isn't correct. ModuleId is %d.\r\n", enModuleId);
    }
	/*lint +e662 +e661*/
}

/*****************************************************************************
 函 数 名  : Log_SetPrintLev
 功能描述  : 设置模块的打印级别
 输入参数  : pstLogIdLevel - 指向工具侧发来的模块级别结构体的指针
             ulLength      - 数据的长度
 输出参数  : 无
 返 回 值  : VOS_OK  - 成功
             VOS_ERR - 失败

 修改历史      :
  1.日    期   : 2008年9月9日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数,添加可维可测第三阶段需求
*****************************************************************************/
VOS_UINT32 Log_SetPrintLev(LOG_ID_LEVEL_STRU *pstLogIdLevel, VOS_UINT32 ulLength)
{
    VOS_UINT32         ulIndex;
    VOS_UINT32         ulModuleId;
    VOS_UINT32         ulModuleNum;
    LOG_LEVEL_EN       enPrintLev;

    /*检测参数的合法性*/
    if (VOS_ERR == Log_CheckPara(pstLogIdLevel, ulLength))
    {
        return VOS_ERR;
    }

    /*清空上次配置*/
    VOS_MemSet(g_aulLogPrintLevPsTable, 0, LOG_PS_MODULE_MAX_NUM*sizeof(LOG_LEVEL_EN));
    VOS_MemSet(g_aulLogPrintLevDrvTable, 0, LOG_DRV_MODULE_MAX_NUM*sizeof(LOG_LEVEL_EN));

    ulModuleNum = pstLogIdLevel->ulModuleNum;

    /*将每个模块的打印级别填入到全局过滤表中*/
    for (ulIndex = 0; ulIndex < ulModuleNum; ulIndex++)
    {
        ulModuleId = pstLogIdLevel->astModuleLev[ulIndex].ulModuleId;
        enPrintLev = pstLogIdLevel->astModuleLev[ulIndex].enPrintLev;

        Log_SetModuleIdLev((LOG_MODULE_ID_EN)ulModuleId, (LOG_SUBMOD_ID_EN)0, enPrintLev);
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : Log_OmMsgProc
 功能描述  : 处理工具侧发来的消息包
 输入参数  : pRspPacket - 消息包的指针
 输出参数  : 无
 返 回 值  : void

 修改历史      :
  1.日    期   : 2008年9月11日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID Log_OmMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr)
{
    APP_OM_MSG_EX_STRU             *pstAppToOmMsg;
    VOS_UINT16                      usReturnPrimId;
    VOS_UINT32                      ulRet = VOS_ERR;
    VOS_UINT32                      ulOutputType;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pRspPacket;
    /*设置日志输出方式的原语消息*/
    if (APP_OM_SET_PRINT_OUTPUT_REQ == pstAppToOmMsg->usPrimId)
    {
        usReturnPrimId = OM_APP_SET_PRINT_OUTPUT_CNF;
        ulOutputType = *((VOS_UINT32*)pstAppToOmMsg->aucPara);
        ulRet = Log_SetOutputType((OM_OUTPUT_PORT_ENUM_UINT32)ulOutputType);
    }
    /*设置日志打印级别的原语消息*/
    else if (APP_OM_SET_PRINT_LEV_REQ == pstAppToOmMsg->usPrimId)
    {
        usReturnPrimId = OM_APP_SET_PRINT_LEV_CNF;
        ulRet = Log_SetPrintLev((LOG_ID_LEVEL_STRU*)(pstAppToOmMsg->aucPara),
          pstAppToOmMsg->usLength - (OM_APP_MSG_EX_LEN - VOS_OM_HEADER_LEN));
    }
    /*删除文件系统中的日志文件*/
    else if (APP_OM_EXPORT_LOG_END_CNF == pstAppToOmMsg->usPrimId)
    {
        if(PS_LOG_FILE_PROTECT == *(VOS_UINT32*)(pstAppToOmMsg->aucPara))
        {
        }

        return;
    }
    else
    {
        vos_printf("Undefined PrimId : %d", pstAppToOmMsg->usPrimId);
        return;
    }

    OM_GreenChannel(OM_LOG_FUNC, usReturnPrimId,
                    (VOS_UINT8*)&ulRet, sizeof(ulRet));

    return;
}
/*****************************************************************************
 函 数 名  : Log_InitFile
 功能描述  : 初始化LOG文件
 输入参数  : void
 输出参数  : 无
 返 回 值  : void

 修改历史      :
  1.日    期   : 2008年9月11日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 Log_InitFile(VOS_VOID)
{
    OM_PORT_CFG_STRU    stPortCfg;

    MNTN_ErrLogInit();

    /* 从NV项中读出LOG的输出方式和文件支持的最大大小 */
    if(NV_OK != NV_Read(en_NV_Item_Om_PsLog_Port,
                                &stPortCfg,
                                sizeof(OM_PORT_CFG_STRU)))
    {
        vos_printf("Log_InitFile: NV_Read fail, NV Id :%d\n", en_NV_Item_Om_PsLog_Port);
        stPortCfg.ulMaxFileSize = 0;
    }
    else
    {
        /*参数检测*/
        if (OM_OUTPUT_BUTT > stPortCfg.enPortType)
        {
            g_stLogEnt.ulLogOutput = (LOG_ULONG)stPortCfg.enPortType;
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : LogShow
 功能描述  : 显示日志上报当前状态
 输入参数  : VOID
 输出参数  : 无
 返 回 值  : VOID
 修改历史      :
  1.日    期   : 2008年9月11日
    作    者   : 甘兰 47350
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID LogShow(VOS_VOID)
{
    VOS_UINT32      ulBufferLen;

    if (LOG_TRUE == g_stLogEnt.ulPrintSwitch)
    {
        vos_printf("The Current Log State:      ON\n");
    }
    else
    {
        vos_printf("The Current Log State:      OFF\n");
    }

#ifdef __LOG_BBIT__
    vos_printf("The Data Type:              STRING\n");
#else
    vos_printf("The Data Type:              DOT\n");
#endif

    if (OM_OUTPUT_SHELL == g_stLogEnt.ulLogOutput)
    {
        vos_printf("The Output Type:            COM\n");
    }
    else if (OM_OUTPUT_FS == g_stLogEnt.ulLogOutput)
    {
        vos_printf("The Output Type:            File System\n");
    }
    else if (OM_OUTPUT_SDT == g_stLogEnt.ulLogOutput)
    {
        vos_printf("The Output Type:            SDT\n");
    }
    else
    {
        /*Make pclint happy*/
    }

    if (VOS_TRUE == g_astLogFileInfo[LOG_FROM_PS_LOG].bIsWritten)
    {
        vos_printf("The Log File:               OK\n");
    }
    else
    {
        vos_printf("The Log File:               ERROR\n");
    }

    ulBufferLen = (VOS_UINT32)OM_RingBufferFreeBytes(g_stLogEnt.rngOmRbufId);
    vos_printf("Remainder Len of Log buffer:%ld\n", ulBufferLen);
}
VOS_UINT32 OM_InitLogFile(VOS_UINT32 ulModuleId, VOS_UINT32 ulFileSize)
{
    VOS_UINT32      ulStringLen;
    FILE*           lTmpFile;
    VOS_UINT32      ulIndex;
    VOS_INT         lFileSize;
    VOS_CHAR        acSemName[LOG_SOURCE_BUTT][LOG_SEM_NAME_LEN]= {"pslog","prinf","omlog"};

    g_astLogFileInfo[ulModuleId].bIsWritten = VOS_FALSE;

    /* 设置LOG文件支持的最大大小，等于0时采用全局变量中的初始值*/
    if(( 0 != ulFileSize) && ( VOS_OK == g_ulPslogStatus))
    {
        g_astLogFileInfo[ulModuleId].lFileMaxSize = (VOS_INT)ulFileSize;
    }

    /* 创建LOG文件对应的信号量 */
    if (VOS_OK != VOS_SmMCreate(acSemName[ulModuleId], VOS_SEMA4_FIFO, &g_astLogFileInfo[ulModuleId].ulSem))
    {
        g_ulLogErrFlag = LOG_OPERATION_CREATE_SEM_FAIL;
        return VOS_ERR;
    }

    /* 判断LOG目录是否已经存在 */
    if (VOS_OK != DRV_FILE_ACCESS(g_acLogDir, 0))
    {
        /* 如果目录不存在则创建LOG目录 */
        if(DRV_ERROR == DRV_FILE_MKDIR(g_acLogDir))
        {
            g_ulLogErrFlag = LOG_OPERATION_MKDIR_FAIL;
            return VOS_ERR;
        }
    }

    ulStringLen = VOS_StrLen(g_astLogFileInfo[ulModuleId].acName);
    for (ulIndex = 0; ;ulIndex++)
    {
        /*由于LOG文件采用乒乓方式进行操作，两个LOG文件分别为Log1, Log2*/
        g_astLogFileInfo[ulModuleId].acName[ulStringLen - 1] = (VOS_CHAR)('1' + ulIndex);

        /*打开LOG文件*/
        lTmpFile = OM_Open(g_astLogFileInfo[ulModuleId].acName, DRV_O_CREAT|DRV_O_APPEND|DRV_O_RDWR);
        if (DRV_FILE_NULL == lTmpFile)
        {
            g_ulLogErrFlag = LOG_OPERATION_OPEN_FILE_FAIL;
            return VOS_ERR;
        }

        /*获取当前日志文件的大小*/
        lFileSize = (VOS_INT)DRV_FILE_LSEEK(lTmpFile, 0, DRV_SEEK_END);
        if (DRV_ERROR == lFileSize)
        {
            DRV_FILE_CLOSE(lTmpFile);
            g_ulLogErrFlag = LOG_OPERATION_LSEEK_FILE_FAIL;
            return VOS_ERR;
        }

        /*假如为最后一个LOG文件或者当前LOG文件没有被写满，
          则选用此文件为初始化LOG文件*/
        if ((LOG_FILE_BUTT == (ulIndex + 1))
            || (g_astLogFileInfo[ulModuleId].lFileMaxSize > (lFileSize + LOG_MAX_COLUMN_VAL)))
        {
            /*保存当前日志文件的句柄和大小*/
            g_astLogFileInfo[ulModuleId].enLogFileNum = (LOG_NUM_ENUM)ulIndex;
            g_astLogFileInfo[ulModuleId].lFileSize = lFileSize;
            DRV_FILE_CLOSE(lTmpFile);
            break;
        }
        DRV_FILE_CLOSE(lTmpFile);
    }

    g_astLogFileInfo[ulModuleId].bIsWritten = VOS_TRUE;
    return VOS_OK;
}
VOS_UINT32 OM_WriteLogFile(VOS_UINT32 ulModuleId, VOS_CHAR *pcLogData, VOS_UINT32 ulLength)
{
    VOS_UINT32      ulStringLen;
    VOS_UINT32      ulIndex;
    FILE*           lLogFile;

    /*判断写入长度是否超过了文件的最大长度*/
    if (g_astLogFileInfo[ulModuleId].lFileMaxSize < (VOS_INT)ulLength)
    {
        g_ulLogErrFlag = LOG_OPERATION_LENGTH_TOOBIG;
        return VOS_ERR;
    }

    /* 对临界资源g_astLogFileInfo进行保护 */
    if (VOS_OK != VOS_SmP(g_astLogFileInfo[ulModuleId].ulSem, 0))
    {
        g_ulLogErrFlag = LOG_OPERATION_POST_SEM_FAIL;
        return VOS_ERR;
    }

    /*判断当前文件是否可写*/
    if (VOS_FALSE == g_astLogFileInfo[ulModuleId].bIsWritten)
    {
        VOS_SmV(g_astLogFileInfo[ulModuleId].ulSem);
        g_ulLogErrFlag = LOG_OPERATION_WRITE_PROTECT;
        return VOS_ERR;
    }

    /* 判断LOG文件是否已经被删除 */
    if (VOS_OK != DRV_FILE_ACCESS(g_astLogFileInfo[ulModuleId].acName, 0))
    {
        g_astLogFileInfo[ulModuleId].lFileSize = 0;
    }

    /*得到当前日志文件的序号和大小*/
    ulIndex = (VOS_UINT32)g_astLogFileInfo[ulModuleId].enLogFileNum;
    ulStringLen = VOS_StrLen(g_astLogFileInfo[ulModuleId].acName);

    /*判断当前写入长度是否使得LOG溢出*/
    if (g_astLogFileInfo[ulModuleId].lFileMaxSize < ((VOS_INT)ulLength + g_astLogFileInfo[ulModuleId].lFileSize))
    {
        /*由于采用乒乓机制，需要对0或1进行反转*/
        ulIndex++;
        ulIndex = (ulIndex & 0x01);
        g_astLogFileInfo[ulModuleId].acName[ulStringLen - 1] = (VOS_CHAR)('1' + ulIndex);

        /*以只写的方式打开LOG文件，并且初始化为空，
          如果此文件不存在，则创建此文件*/
        lLogFile = OM_Open(g_astLogFileInfo[ulModuleId].acName,DRV_O_CREAT|DRV_O_RDWR|DRV_O_TRUNC);

        if (DRV_FILE_NULL != lLogFile)
        {
            g_astLogFileInfo[ulModuleId].enLogFileNum = (LOG_NUM_ENUM)ulIndex;
            g_astLogFileInfo[ulModuleId].lFileSize = 0;
        }
    }
    else
    {
        g_astLogFileInfo[ulModuleId].acName[ulStringLen - 1] = (VOS_CHAR)('1' + ulIndex);
        lLogFile = OM_Open(g_astLogFileInfo[ulModuleId].acName,DRV_O_CREAT|DRV_O_RDWR|DRV_O_APPEND);

        /*由于Balong版本的O_APPEND没有效果，需要手动进行设置*/
        if(VOS_NULL_PTR != lLogFile)
        {
            if (DRV_ERROR == DRV_FILE_LSEEK(lLogFile, 0, DRV_SEEK_END))
            {
                vos_printf("OM_WriteLogFile: Seek Fail\r\n");
            }
        }
    }

    /*打开失败*/
    if (DRV_FILE_NULL == lLogFile)
    {
        g_astLogFileInfo[ulModuleId].bIsWritten = VOS_FALSE;
        g_ulLogErrFlag = LOG_OPERATION_OPEN_FILE_FAIL;
        VOS_SmV(g_astLogFileInfo[ulModuleId].ulSem);
        return VOS_ERR;
    }

    /*写入LOG内容*/
    if (DRV_ERROR == DRV_FILE_WRITE(pcLogData, sizeof(VOS_CHAR), (DRV_SIZE_T)ulLength, lLogFile))
    {
        DRV_FILE_CLOSE(lLogFile);
        g_astLogFileInfo[ulModuleId].bIsWritten = VOS_FALSE;
        g_ulLogErrFlag = LOG_OPERATION_WRITE_FILE_FAIL;
        VOS_SmV(g_astLogFileInfo[ulModuleId].ulSem);
        return VOS_ERR;
    }

    DRV_FILE_CLOSE(lLogFile);

    /*更新LOG文件实际大小*/
    g_astLogFileInfo[ulModuleId].lFileSize += (VOS_INT32)ulLength;

    VOS_SmV(g_astLogFileInfo[ulModuleId].ulSem);
    return VOS_OK;
}


VOS_VOID OM_RemoveLogFile(VOS_UINT32 ulModuleId)
{
    VOS_UINT32 ulIndex;
    VOS_UINT32 ulStringLen;

    /* 对临界资源g_astLogFileInfo进行保护 */
    if (VOS_OK != VOS_SmP(g_astLogFileInfo[ulModuleId].ulSem, 0))
    {
        g_ulLogErrFlag = LOG_OPERATION_POST_SEM_FAIL;

        return;
    }
    ulStringLen = VOS_StrLen(g_astLogFileInfo[ulModuleId].acName);

    /* 删除对应模块的两个乒乓LOG文件 */
    for (ulIndex = 0; ulIndex < LOG_FILE_BUTT; ulIndex++)
    {
        g_astLogFileInfo[ulModuleId].acName[ulStringLen - 1] = (VOS_CHAR)('1' + ulIndex);
        DRV_FILE_RMFILE(g_astLogFileInfo[ulModuleId].acName);
    }

    /* 更新LOG文件状态控制块*/
    g_astLogFileInfo[ulModuleId].enLogFileNum = LOG_FILE_1;
    g_astLogFileInfo[ulModuleId].lFileSize = 0;

    VOS_SmV(g_astLogFileInfo[ulModuleId].ulSem);
    return ;
}
VOS_VOID OM_PauseLogFile(VOS_UINT32 ulModuleId)
{
    /* 对临界资源g_astLogFileInfo进行保护 */
    if (VOS_OK != VOS_SmP(g_astLogFileInfo[ulModuleId].ulSem, 0))
    {
        g_ulLogErrFlag = LOG_OPERATION_POST_SEM_FAIL;

        return ;
    }

    /* 设置日志文件不可写 */
    g_astLogFileInfo[ulModuleId].bIsWritten = VOS_FALSE;

    VOS_SmV(g_astLogFileInfo[ulModuleId].ulSem);

    return ;
}


VOS_VOID OM_ResumeLogFile(VOS_UINT32 ulModuleId)
{
    /* 对临界资源g_astLogFileInfo进行保护 */
    if (VOS_OK != VOS_SmP(g_astLogFileInfo[ulModuleId].ulSem, 0))
    {
        g_ulLogErrFlag = LOG_OPERATION_POST_SEM_FAIL;

        return ;
    }

    /* 设置日志文件可写 */
    g_astLogFileInfo[ulModuleId].bIsWritten = VOS_TRUE;

    VOS_SmV(g_astLogFileInfo[ulModuleId].ulSem);

    return;
}


#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif



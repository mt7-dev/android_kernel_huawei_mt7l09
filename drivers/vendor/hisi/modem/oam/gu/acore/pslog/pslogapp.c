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
#include "PsLogAdapterApp.h"
#include "omprivate.h"
#include "NVIM_Interface.h"
#include "omnvinterface.h"


/******************全局变量声明************************************************/
#define THIS_FILE_ID PS_FILE_ID_PS_LOG_APP_C

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

#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
/*保存在文件系统中的LOG文件信息*/
LOG_FILE_INFO_STRU g_astLogFileInfo[LOG_SOURCE_BUTT]
               = {{VOS_FALSE, LOG_FILE_1, 0, LOG_FILE_MAX_SIZE, 0, "/modem_log/Log/PsLog0"},
                  {VOS_FALSE, LOG_FILE_1, 0, LOG_FILE_MAX_SIZE, 0, "/modem_log/Log/Printf0"},
                  {VOS_FALSE, LOG_FILE_1, 0, (LOG_FILE_MAX_SIZE*2), 0, "/modem_log/Log/OmLog0"}};
#else
/*保存在文件系统中的LOG文件信息*/
LOG_FILE_INFO_STRU g_astLogFileInfo[LOG_SOURCE_BUTT]
               = {{VOS_FALSE, LOG_FILE_1, 0, LOG_FILE_MAX_SIZE, 0, "/yaffs0/Log/PsLog0"},
                  {VOS_FALSE, LOG_FILE_1, 0, LOG_FILE_MAX_SIZE, 0, "/yaffs0/Log/Printf0"},
                  {VOS_FALSE, LOG_FILE_1, 0, (LOG_FILE_MAX_SIZE*2), 0, "/yaffs0/Log/OmLog0"}};
#endif

/* 用于LOG写入FLASH接口的定位信息 */
VOS_UINT32  g_ulLogErrFlag = LOG_OPERATION_OK;
#if (VOS_VXWORKS == VOS_OS_VER)
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
VOS_CHAR    g_acLogDir[LOG_FILE_PATH_MAX_LENGTH] = "/modem_log/Log";
#else
/*LOG File Directory */
VOS_CHAR    g_acLogDir[LOG_FILE_PATH_MAX_LENGTH] = "/yaffs0/Log";
#endif

#elif (VOS_WIN32 == VOS_OS_VER)

/*LOG File Directory */
VOS_CHAR    g_acLogDir[] = "";

#endif

extern VOS_UINT32 OM_AcpuSendLog(VOS_UINT8 *pucLogData, VOS_UINT32 ulLength);

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
#if ((VOS_OS_VER == VOS_VXWORKS)||(VOS_OS_VER == VOS_LINUX))
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
    return LOG_OK;
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
    if ((VOS_PID_BUTT == ulModuleId) && (OM_OUTPUT_SDT == g_stLogEnt.ulLogOutput))
    {
        return LOG_LEVEL_OFF;
    }
    /*判断模块ID是否在ACPU支持的PS范围内*/
    if ((VOS_PID_DOPRAEND <= ulModuleId)
         && (VOS_PID_BUTT > ulModuleId))
    {
        return g_aulLogPrintLevPsTable[ulModuleId - VOS_PID_DOPRAEND];
    }
    /*判断模块ID是否在ACPU支持的DRV范围内*/
    if ((LOG_MIN_MODULE_ID_ACPU_DRV <= ulModuleId)
         && (LOG_MAX_MODULE_ID_ACPU_DRV >= ulModuleId))
    {
        return g_aulLogPrintLevDrvTable[ulModuleId - LOG_MIN_MODULE_ID_ACPU_DRV];
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
    OM_PORT_CFG_STRU    stPortCfg;

    g_stLogEnt.ulPrintSwitch = LOG_FALSE;

    if (VOS_OK != VOS_SmMCreate("LOG_MSem", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_logBuffSem))
    {
        vos_printf("Log: Error, semMCreate Fail");
        return ;
    }


    if (VOS_OK != VOS_SmCCreate("LOG_CSem", 0, VOS_SEMA4_FIFO, &(g_stLogEnt.semOmPrint)))
    {
        VOS_SmDelete(g_logBuffSem);
        vos_printf("Log: Error, semCCreate Fail");
        return ;
    }

    g_stLogEnt.rngOmRbufId = OM_RingBufferCreate(LOG_BUF_VOLUMN);
    if (LOG_NULL_PTR == g_stLogEnt.rngOmRbufId)
    {
        VOS_SmDelete(g_logBuffSem);
        VOS_SmDelete(g_stLogEnt.semOmPrint);
        vos_printf("Log: Error, rngCreate Fail");
        return;
    }

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

#if (VOS_OS_VER == VOS_LINUX)
    if( in_interrupt() )
    {
        return VOS_ERR;
    }
#endif

    if (0 < ulLen)
    {
        if(VOS_OK != VOS_SmP(g_logBuffSem, 0))
        {
            return VOS_ERR;
        }

        if ((ulLen + sizeof(unsigned long)) > (unsigned long)OM_RingBufferFreeBytes(g_stLogEnt.rngOmRbufId))
        {
            VOS_SmV(g_logBuffSem);
            return VOS_ERR;
        }

        sRet = OM_RingBufferPut(g_stLogEnt.rngOmRbufId, (char*)(&ulLen), sizeof(unsigned long));
        if ((int)sizeof(unsigned long) == sRet)
        {
            sRet = OM_RingBufferPut(g_stLogEnt.rngOmRbufId, pcLogStr, (long)ulLen);
            VOS_SmV(g_logBuffSem);

            if (ulLen == (unsigned long)sRet)
            {
                VOS_SmV(g_stLogEnt.semOmPrint);
                return VOS_OK ;
            }
            else
            {
                vos_printf("\nLog, Error, rngBufPut Data not OK");
            }
        }
        else
        {
            VOS_SmV(g_logBuffSem);
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
        if(VOS_OK != VOS_SmP(g_stLogEnt.semOmPrint, 0))
        {
            continue;
        }

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
            vos_printf("%s\n", pcStr + LOG_MODULE_ID_LEN);/* 将字符串输出到串口 */
            break;

        case OM_OUTPUT_SDT:
            OM_AcpuSendLog((unsigned char*)pcStr, ulLen);
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
        vos_printf("Log_SetModuleIdLev: ModuleId is %d, Level is %d.\r\n", enModuleId, enLevel);
        return;
    }

    if (LOG_MIN_MODULE_ID_ACPU_DRV <= enModuleId && LOG_MAX_MODULE_ID_ACPU_DRV >= enModuleId)
    {
        g_aulLogPrintLevDrvTable[enModuleId - LOG_MIN_MODULE_ID_ACPU_DRV] = enLevel;
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
VOS_VOID Log_AcpuOmMsgProc(OM_REQ_PACKET_STRU *pRspPacket, OM_RSP_FUNC *pRspFuncPtr)
{
    APP_OM_MSG_EX_STRU             *pstAppToOmMsg;
    VOS_UINT32                      ulOutputType;

    pstAppToOmMsg = (APP_OM_MSG_EX_STRU*)pRspPacket;
    /*设置日志输出方式的原语消息*/
    if (APP_OM_SET_PRINT_OUTPUT_REQ == pstAppToOmMsg->usPrimId)
    {
        ulOutputType = *((VOS_UINT32*)pstAppToOmMsg->aucPara);
        Log_SetOutputType((OM_OUTPUT_PORT_ENUM_UINT32)ulOutputType);
    }
    /*设置日志打印级别的原语消息*/
    else if (APP_OM_SET_PRINT_LEV_REQ == pstAppToOmMsg->usPrimId)
    {
        Log_SetPrintLev((LOG_ID_LEVEL_STRU*)(pstAppToOmMsg->aucPara),
        pstAppToOmMsg->usLength - (OM_APP_MSG_EX_LEN - VOS_OM_HEADER_LEN));
    }
    /*删除文件系统中的日志文件*/
    else if (APP_OM_EXPORT_LOG_END_CNF == pstAppToOmMsg->usPrimId)
    {
        return;
    }
    else
    {
        vos_printf("Undefined PrimId : %d", pstAppToOmMsg->usPrimId);
        return;
    }

    return;
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
VOS_VOID LogAcpuShow(VOS_VOID)
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

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif



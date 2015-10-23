/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : RNIC.c
  版 本 号   : 初稿
  作    者   : 王蓉
  生成日期   : 2011年11月23日
  最近修改   :
  功能描述   : 用户空间获取SDT配置信息，并按照配置进行信息输出
  函数列表   :

  修改历史   :
  1.日    期   : 2011年11月23日
    作    者   : 王蓉
    修改内容   : 创建文件

******************************************************************************/

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C" {
  #endif
#endif


/******************************************************************************
   1 头文件包含
******************************************************************************/
#if defined(__UT_CENTER__) || defined(_lint)
#include "LinuxStub.h"
#include "skbuff.h"
#else
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <linux/fs.h>
#include <sys/select.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NetworkInquire.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/


#define THIS_FILE_ID PS_FILE_ID_NETWORK_INQUIRE

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/
/*lint -e960*/
extern unsigned long OM_Printf(char * pcformat, ... );
/*lint +e960*/
#ifdef __UT_CENTER__
extern int read(int fd, char *p, int len);
#endif
/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
*****************************************************************************/
NETWORK_INQUIRE_ENTITY_STRU  g_stNwInquireEntity = {0};

/* network inquire映射开关表 */
NW_INQUERY_MASK_FUNC_STRU g_stNwInquireMaskOps[] =
{
    {
        NW_INQUIRE_IFCONFIG_ON_MASK,
        NW_InquireIfconfig
    },
    {
        NW_INQUIRE_ARP_ON_MASK,
        NW_InquireArp
    },
    {
        NW_INQUIRE_ROUTE_ON_MASK,
        NW_InquireRoute
    },
    {
        NW_INQUIRE_NETSTATE_ON_MASK,
        NW_InquireNetstate
    },
    {
        NW_INQUIRE_NAT_ON_MASK,
        NW_InquireNat
    }
};


/*****************************************************************************
 函 数 名  : NW_ShellExc
 功能描述  : 将cmd查询到的对应信息输出
 输入参数  : cmd 要查询的命令
 输出参数  : int
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
void NW_ShellExc(char* cmd)
{
    FILE*           fp                  = NULL;
    int             lRet                = 0;
    char            buf[MAX_RST_BUF]    = {0};

    /*lint -e64*/
    fp = popen(cmd, "r");
    /*lint +e64*/

    if ( NULL != fp )
    {
        while ( (lRet = fread(buf, READ_SIZE_PER_TIME, MAX_RST_BUF - 1, fp)) != 0 )
        {
            buf[lRet + 1] = '\0';
            OM_Printf("%s", buf);
        };

        /*lint -e64*/
        pclose(fp);
        /*lint +e64*/
    }

}/* exc */

/*****************************************************************************
 函 数 名  : NW_DeviceInit
 功能描述  : 设备初始化，通过open打开文件系统
 输入参数  : fd 文件句柄
 输出参数  : int
 返 回 值  : NW_INQUIRE_RETURN_OK/NW_INQUIRE_RETURN_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 唐刚
    修改内容   : 新生成函数

*****************************************************************************/
int NW_DeviceInit(int *fd)
{
    char aucDevPath[DEVICE_PATH_LEN] = {0};
    if ( NULL == fd )
    {
        printf("NW_InquireInit,error, fd is NULL_PTR!\n ");

        return NW_INQUIRE_ERROR;
    }

    /* 打开文件(虚拟设备) */
    sprintf(aucDevPath, "/dev/%s", DEVICE_NAME);

    (*fd) = open(aucDevPath, O_RDWR);

    if ( -1 == *fd )
    {
        printf("NW_InquireInit,error, open file failed!\n ");

        close(*fd);

        return NW_INQUIRE_ERROR;
    }

    return NW_INQUIRE_OK;
}

/*****************************************************************************
 函 数 名  : NW_DeviceRelease
 功能描述  : 关闭设备
 输入参数  : fd 文件句柄
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 唐刚
    修改内容   : 新生成函数

*****************************************************************************/
void NW_DeviceRelease(int *fd)
{
    close(*fd);
}

/*****************************************************************************
 函 数 名  : NW_InquireInit
 功能描述  : Network Inquire模块初始化
 输入参数  : fd 文件句柄
 输出参数  : 无
 返 回 值  : NW_INQUIRE_OK/NW_INQUIRE_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
int NW_InquireInit(int *fd)
{
    if ( NW_INQUIRE_ERROR == NW_DeviceInit(fd) )
    {
        return NW_INQUIRE_ERROR;
    }

    return NW_INQUIRE_OK;
}/* NW_InquireInit */

/*****************************************************************************
 函 数 名  : NW_InquireUnInit
 功能描述  : NW_InquireUnInit模块注销
 输入参数  : fd 文件句柄
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 唐刚
    修改内容   : 新生成函数

*****************************************************************************/
void NW_InquireUnInit(int *fd)
{
    NW_DeviceRelease(fd);
}

/*****************************************************************************
 函 数 名  : NW_GetConfigPara
 功能描述  : 获取配置信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 唐刚
    修改内容   : 新生成函数

*****************************************************************************/
void NW_GetConfigPara(int fd)
{
    int readret;

    /* 读取文件中配置信息 ,保存在g_stNwInquireCfg中 */
    /*lint -e718 -e746*/
    readret = read(fd, (char *)&g_stNwInquireEntity.stInquireCfg, DEV_BUFF_LEN);
    /*lint +e718 +e746*/

    /* 实际读取的长度和要求读取的长度不符时，跳过本次处理 */
    if ( (int)DEV_BUFF_LEN != readret )
    {
        printf("NW_GetConfigPara,error,read len error!\n");

        return;
    }

    /* 如果配置关闭上报，将定时器停掉，这里只是设置一个不会超时的定时器 */
    if ( (0 == g_stNwInquireEntity.stInquireCfg.ulTimerLen) || (0 == g_stNwInquireEntity.stInquireCfg.ulOnOffMask) )
    {
        g_stNwInquireEntity.stInquireCfg.ulTimerLen = MAX_TIMER_LEN;
    }
}


/*****************************************************************************
 函 数 名  : NW_ResetTimer
 功能描述  : 重新设置时间参数
 输入参数  : struct timeval  *timeout
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 唐刚
    修改内容   : 新生成函数

*****************************************************************************/
void NW_ResetTimer(struct timeval  *timeout)
{
    timeout->tv_sec     = (int)g_stNwInquireEntity.stInquireCfg.ulTimerLen;
    timeout->tv_usec    = 0;
}


/*****************************************************************************
 函 数 名  : NW_GetConfigPara
 功能描述  : 获取配置信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
void NW_InquireProcess(int fd)
{
    int             ret = 0;
    fd_set          fds = {0};
    struct  timeval timeout = {MAX_TIMER_LEN, 0};

    FD_SET((unsigned int)fd, &fds);

#ifdef __UT_CENTER__
/*lint -e506 -e774*/
    if (1)
/*lint +e506 +e774*/
#else
    for(;;)
#endif
    {
        FD_ZERO(&fds);
        FD_SET((unsigned int)fd, &fds);

        /*lint -e64*/
        ret = select(fd + 1, &fds, NULL, NULL, &timeout);
        /*lint +e64*/
        switch ( ret )
        {
            case -1:
                printf("Error occur in NW_InquireProcess");

                break;
            case 0: /* 定时器超时处理 */
                /* 重设置定时器信息 */
                NW_ResetTimer(&timeout);
                /* 做一次查询 */
                NW_DoInquire();

                break;
            default:
                /* 如果文件可读 */
                if (FD_ISSET((unsigned int)fd, &fds))
                {
                    /* 读取配置参数 */
                    NW_GetConfigPara(fd);
                    /* 重新设置时间参数 */
                    NW_ResetTimer(&timeout);
                    /* 做一次查询 */
                    NW_DoInquire();
                }

                break;
        }
    }

}/* NW_GetConfigPara */


/*****************************************************************************
 函 数 名  : NW_DoInquire
 功能描述  : 根据掩码配置生效的函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
void NW_DoInquire(void)
{
    unsigned int i;
    u_int32_t    ulMask     = g_stNwInquireEntity.stInquireCfg.ulOnOffMask;

    if ( NW_INQUIRE_CFG_ALL_MASK < ulMask )
    {
        printf("NW_DoInquire,error,Mask is invalid!\n");

        return;
    }

    for ( i = 0; i < NW_INQUIRE_ARRAY_SIZE(g_stNwInquireMaskOps); i++ )
    {
        if ( g_stNwInquireMaskOps[i].ulMask == ( ulMask & g_stNwInquireMaskOps[i].ulMask ))
        {
            g_stNwInquireMaskOps[i].pInquireFunc();
        }
    }
}/* NW_DoInquire */


/*****************************************************************************
 函 数 名  : NW_InquireIfconfig
 功能描述  : 查询Ifconfig的函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
void NW_InquireIfconfig(void)
{
    EXC("ifconfig");

}/* NW_InquireIfconfig */

/*****************************************************************************
 函 数 名  : NW_InquireArp
 功能描述  : 查询Arp的函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
void NW_InquireArp(void)
{
    EXC("arp");

}/* NW_InquireArp */

/*****************************************************************************
 函 数 名  : NW_InquireRoute
 功能描述  : 查询Route的函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
void NW_InquireRoute(void)
{
    EXC("route");

}/* NW_InquireRoute */

/*****************************************************************************
 函 数 名  : NW_InquireNetstate
 功能描述  : 查询Netstate的函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
void NW_InquireNetstate(void)
{
    /* 只查询tcp,udp,icmp连接其他协议不查询 */
    EXC("netstat -t -u -w -n");

}/* NW_InquireNetstate */

/*****************************************************************************
 函 数 名  : NW_InquireNat
 功能描述  : 查询Nat的函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
void NW_InquireNat(void)
{
    EXC("cat /proc/net/ip_conntrack");

}/* NW_InquireNat */



/*****************************************************************************
 函 数 名  : NW_InquireNat
 功能描述  : 主函数，入口函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月24日
    作    者   : 王蓉
    修改内容   : 新生成函数

*****************************************************************************/
#ifndef __UT_CENTER__
int main(void)
{
    int fd;
    int lWaitKoCnt = 0;

    /*===========================================*//* 模块初始化 */
    /* 打开设备文件，失败则返回 */
    /* 由于内核加载需要一段时间，用户程序与内核程序交互需要等待内核程序加载完成之后 */
    do
    {
        if ( NW_INQUIRE_OK != NW_InquireInit(&fd) )
        {
            lWaitKoCnt++;

            printf("NW_ProcessConfig,warning, NW_InquireInit fail times %d!\n ", lWaitKoCnt);
        }
        else
        {
            break;
        }

        if ( lWaitKoCnt >= MAX_WAIT_KO_READY_TIMES )
        {
            printf("NW_ProcessConfig,error, NW_InquireInit failed!\n ");

            return NW_INQUIRE_ERROR;
        }

        /*lint -e718 -e746*/
        sleep(WAIT_KO_READY_ONE_TIME_LEN);
        /*lint +e718 +e746*/
    /*lint -e506*/
    }while(1);
    /*lint -e506*/

    /* 读取配置等处理程序运行在一个线程中 */
    NW_InquireProcess(fd);

    NW_InquireUnInit(&fd);

    return 0;
}
#endif


#ifdef __cplusplus
    #if __cplusplus
            }
    #endif
#endif



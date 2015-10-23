

#ifndef __NETWORK_INQUIRE_H__
#define __NETWORK_INQUIRE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "NetWInterface.h"


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*================================================*/
/* 数值宏定义 */
/*================================================*/
#define MAX_RST_BUF                          (1011)         /*一次最大的读取长度*/
#define READ_SIZE_PER_TIME                   (1)            /*单个数据元素的大小*/
#define MAX_TIMER_LEN                        (0x7FFFFFFF)
#define WAIT_KO_READY_ONE_TIME_LEN           (5)
#define MAX_WAIT_KO_READY_TIMES              (10)

/*================================================*/
/* 功能函数宏定义 */
/*================================================*/
#define NW_INQUIRE_ARRAY_SIZE(x)    (sizeof(x)/sizeof(x[0]))
#define EXC(x)                      NW_ShellExc(x)
typedef void NW_InquireFunc(void);

/*******************************************************************************
  3 枚举定义
*******************************************************************************/
enum NW_INQUIRE_RETURN_ENUM
{
        NW_INQUIRE_ERROR          = 0,
        NW_INQUIRE_OK             = 1,
        NW_INQUIRE_BUTT
};

typedef int NW_INQUIRE_RETURN;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
typedef struct
{
    NETWORK_INQUIRE_CONFIG_STRU stInquireCfg;
}NETWORK_INQUIRE_ENTITY_STRU;

typedef struct
{
    unsigned int             ulMask;
    NW_InquireFunc          *pInquireFunc;
} NW_INQUERY_MASK_FUNC_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern int NW_InquireInit(int *fd);
extern int NW_EstHandlerForTimer(struct sigaction *sa);
extern void NW_DoInquire(void);
extern void NW_InquireIfconfig(void);
extern void NW_InquireArp(void);
extern void NW_InquireRoute(void);
extern void NW_InquireNetstate(void);
extern void NW_InquireNat(void);
extern void NW_ShellExc(char* cmd);
extern void NW_ProcessThread(void);
extern int NW_DeviceInit(int *fd);
extern void NW_DeviceRelease(int *fd);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* NetworkInquire.h */


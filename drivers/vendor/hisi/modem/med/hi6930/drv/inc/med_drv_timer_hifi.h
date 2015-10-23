

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "med_drv_interface.h"
#include "soc_timer_interface.h"

#ifndef __DRV_TIMER_HIFI_H__
#define __DRV_TIMER_HIFI_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define DRV_TIMER_OM_ADDR               DRV_TIMER_SC_SLICE_ADDR                 /* 系统 Slice计数器所在的地址 */
#define DRV_TIMER_OM_FREQ               DRV_TIMER_SC_SLICE_FREQ

#define DRV_TIMER_UNLINK_TIMER_ADDR     DRV_TIMER_UNLINK_HIFI_ADDR              /* PC VOICE使用TIMER8 */
#define DRV_TIMER_UNLINK_TIMER_IDX      (VOS_NULL)                              /* 该参数V7R2中不使用 */
#define DRV_TIMER_UNLINK_TIMER_FREQ     DRV_TIMER_UNLINK_HIFI_FREQ              /* 计算方法:定时器频率(Hz)/10000 */
#define DRV_TIMER_UNLINK_MAX_NUM        (1)                                     /* 独立定时器数目       */
#define DRV_TIMER_UNLINK_MAX_TIMEOUT    (1000000)                               /* 独立定时器支持的最大定时时长，单位0.1ms, =(10000*2P32)/(DRV_TIMER_UNLINK_TIMER_FREQ*2) */

#define DRV_TIMER_LINKED_TIMER_ADDR     DRV_TIMER_DWAPB_HIFI_ADDR               /* 链式定时器使用TIMER0 */
#define DRV_TIMER_LINKED_TIMER_IDX      (VOS_NULL)                              /* 该参数V7R2中不使用 */
#define DRV_TIMER_LINKED_TIMER_FREQ     DRV_TIMER_DWAPB_HIFI_FREQ               /* HIFI工作时钟，计算方法:定时器频率(Hz)/10000 */
#define DRV_TIMER_LINKED_MAX_NUM        (9)                                     /* 支持的链接定时器数目 */
#define DRV_TIMER_LINKED_MAX_TIMEOUT    (1000000)                               /* 链接定时器支持的最大定时时长，单位0.1ms, =(10000*2P32)/(266000000*2) */

#define DRV_TIMER_MAX_NUM               (DRV_TIMER_LINKED_MAX_NUM       \
                                         + DRV_TIMER_UNLINK_MAX_NUM)            /* 最多支持定时器数目 */

#define DRV_TIMER_PERIPH_CLK_EN_ADDR SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(VOS_NULL)

#define DRV_TIMER_CONTROL(base, i)       \
                SOC_TIMER_TIMERN_CONTROL_ADDR(base, i)                          /* 定时器i控制寄存器         */
#define DRV_TIMER_EOI(base, i)           \
                SOC_TIMER_TIMERN_INTCLR_ADDR(base, i)                           /* 定时器i清中断寄存器       */
#define DRV_TIMER_INTSTATUS(base, i)     \
                SOC_TIMER_TIMERN_RIS_ADDR(base, i)                              /* 定时器i中断状态寄存器     */
#define DRV_TIMER_LOADCOUNT(base, i)     \
                SOC_TIMER_TIMERN_LOAD_ADDR(base, i)                             /* 定时器i初始值低32位寄存器 */
#define DRV_TIMER_CURRENTVALUE(base, i)  \
                SOC_TIMER_TIMERN_VALUE_ADDR(base, i)                            /* 定时器i当前值低32位寄存器 */

#define DRV_TIMER_GetTimerCtrlObjPtr()  (&g_stDrvTimerCtrlObj)                  /* 获取Timer控制全局结构指针 */

#define DRV_TIMER_StartLinkedTimer(uwLoadCnt)                         \
        DRV_TIMER_StartDwApbTimer(DRV_TIMER_LINKED_TIMER_ADDR,    \
                                  DRV_TIMER_LINKED_TIMER_IDX,     \
                                  uwLoadCnt,                          \
                                  DRV_TIMER_MODE_ONESHOT)
#define DRV_TIMER_StopLinkedTimer()                                   \
        DRV_TIMER_StopDwApbTimer(DRV_TIMER_LINKED_TIMER_ADDR,     \
                                 DRV_TIMER_LINKED_TIMER_IDX)
#define DRV_TIMER_ReadLinkedTimer()                                   \
        DRV_TIMER_ReadDwApbTimer(DRV_TIMER_LINKED_TIMER_ADDR,     \
                                 DRV_TIMER_LINKED_TIMER_IDX,      \
                                 VOS_NULL,                            \
                                 VOS_NULL)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : DRV_TIMER_MODE_ENUM
 功能描述  : 定时器模式枚举定义
*****************************************************************************/
enum DRV_TIMER_MODE_ENUM
{
    DRV_TIMER_MODE_ONESHOT              = 0,                                    /* 一次性定时器 */
    DRV_TIMER_MODE_PERIODIC             = 1,                                    /* 周期性定时器 */
    DRV_TIMER_MODE_BUTT
};
typedef VOS_UINT16 DRV_TIMER_MODE_ENUM_UINT16;

/*****************************************************************************
 实体名称  : DRV_TIMER_DEVICE_ENUM
 功能描述  : HIFI可用定时器设备标志枚举定义
*****************************************************************************/
enum DRV_TIMER_DEVICE_ENUM
{
    DRV_TIMER_DEVICE_DUALTIMER5_0       = 0,                                    /* DUALTimer定时器5_0 */
    DRV_TIMER_DEVICE_DUALTIMER5_1,                                              /* DUALTimer定时器5_1 */
    DRV_TIMER_DEVICE_BUTT
};
typedef VOS_UINT32 DRV_TIMER_DEVICE_ENUM_UINT32;

/*****************************************************************************
 实体名称  : DRV_TIMER_DEVICE_STATUS_ENUM
 功能描述  : 定时器设备状态枚举定义
*****************************************************************************/
enum DRV_TIMER_DEVICE_STATUS_ENUM
{
    DRV_TIMER_DEVICE_STATUS_IDLE          = 0,                                  /* 定时器停止 */
    DRV_TIMER_DEVICE_STATUS_RUNNING,                                            /* 定时器运行 */
    DRV_TIMER_DEVICE_STATUS_BUTT
};
typedef VOS_UINT32 DRV_TIMER_DEVICE_STATUS_ENUM_UINT32;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
 函 数 名  : DRV_TIMER_INT_FUNC
 功能描述  : 定时器中断处理回调函数
 输入参数  : uwTimer   - 定时器句柄
             uwPara    - 用户参数
 输出参数  : 无
 返 回 值  : VOS_VOID
*****************************************************************************/
typedef VOS_VOID (*DRV_TIMER_INT_FUNC)(VOS_UINT32 uwTimer, VOS_UINT32 uwPara);

/*****************************************************************************
 实体名称  : DRV_TIMER_CTRL_BLOCK_STRU
 功能描述  : 定时器控制块
*****************************************************************************/
typedef struct _DRV_TIMER_CTRL_BLOCK_TAG
{
    VOS_UINT16                          uhwUsedFlag;                            /* 0-未使用、1-使用 */
    DRV_TIMER_MODE_ENUM_UINT16          enMode;                                 /* 定时器模式       */
    VOS_UINT32                          uwLength;                               /* 定时器时长,0.1ms */
    VOS_UINT32                          uwLoadCnt;                              /* 定时器计数值     */
    VOS_UINT32                          uwPara;                                 /* 超时回调参数     */
    DRV_TIMER_INT_FUNC                  pfFunc;                                 /* 超时回调函数     */
    DRV_TIMER_DEVICE_ENUM_UINT32        uwDevice;                               /* 物理定时器标志   */
    struct _DRV_TIMER_CTRL_BLOCK_TAG   *pstNext;
    struct _DRV_TIMER_CTRL_BLOCK_TAG   *pstPrevious;
}DRV_TIMER_CTRL_BLOCK_STRU;

/*****************************************************************************
 实体名称  : DRV_TIMER_CTRL_BLOCK_STRU
 功能描述  : 定时器控制实体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwIdleNum;                              /* 未使用的控制块数 */
    VOS_UINT16                          uhwLinkedNum;                           /* 链表定时器个数   */
    VOS_UINT16                          uhwUnlinkNum;                           /* 独立定时器个数   */
    DRV_TIMER_CTRL_BLOCK_STRU          *pstIdleBlk;                             /* 空闲控制块,仅对链式定时器使用 */
    DRV_TIMER_CTRL_BLOCK_STRU          *pstLinkedBlk;                           /* 链表定时器控制块 */
    DRV_TIMER_CTRL_BLOCK_STRU           astUnlinkBlk[DRV_TIMER_UNLINK_MAX_NUM]; /* 独立定时器控制块 */
    DRV_TIMER_CTRL_BLOCK_STRU           astLinkedBlk[DRV_TIMER_LINKED_MAX_NUM]; /* 定时器控制块缓存 */
}DRV_TIMER_CTRL_OBJ_STRU;

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
extern VOS_VOID DRV_TIMER_AddToLink(
                       DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimer,
                       DRV_TIMER_CTRL_OBJ_STRU                    *pstTimers);
extern VOS_UINT32 DRV_TIMER_CheckLinkedBlk(
                       DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimer,
                       DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimerLst);
extern VOS_VOID DRV_TIMER_DelFromLink(
                       DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimer,
                       DRV_TIMER_CTRL_OBJ_STRU                    *pstTimers);
extern VOS_VOID DRV_TIMER_FreeLinkedBlk(
                       DRV_TIMER_CTRL_BLOCK_STRU                  *pstTimer,
                       DRV_TIMER_CTRL_OBJ_STRU                    *pstTimers);
extern DRV_TIMER_CTRL_BLOCK_STRU *DRV_TIMER_GetLinkedBlk(DRV_TIMER_CTRL_OBJ_STRU *pstTimers);
extern VOS_UINT32 DRV_TIMER_GetOmFreq(VOS_VOID);
extern DRV_TIMER_CTRL_BLOCK_STRU *DRV_TIMER_GetUnlinkBlk(DRV_TIMER_CTRL_OBJ_STRU *pstTimers);
extern VOS_VOID DRV_TIMER_Init(VOS_VOID);
extern VOS_VOID DRV_TIMER_IsrUnLinkedTimer(VOS_VOID);
extern VOS_VOID DRV_TIMER_IsrLinkedTimer(VOS_VOID);
extern VOS_UINT32 DRV_TIMER_ReadDwApbTimer(
                       VOS_UINT32              uwBaseAddr,
                       VOS_UINT32              uwTimerIndx,
                       VOS_UINT32             *puwCntLow,
                       VOS_UINT32             *puwCntHigh);
extern VOS_VOID DRV_TIMER_RestartDwApbTimer(
                       VOS_UINT32              uwBaseAddr,
                       VOS_UINT32              uwTimerIndx);
extern VOS_UINT32 DRV_TIMER_ReadSysTimeStamp(VOS_VOID);
extern VOS_UINT32 DRV_TIMER_Start(
                       VOS_UINT32                                 *puwTimer,
                       VOS_UINT32                                  uwTimeOutLen,
                       DRV_TIMER_MODE_ENUM_UINT16                  enMode,
                       DRV_TIMER_INT_FUNC                          pfFunc,
                       VOS_UINT32                                  uwPara);
extern VOS_VOID DRV_TIMER_StartDwApbTimer(
                       VOS_UINT32              uwBaseAddr,
                       VOS_UINT32              uwTimerIndx,
                       VOS_UINT32              uwLoadCnt,
                       VOS_UINT16              uhwMode);
extern VOS_UINT32 DRV_TIMER_StartPrecise(
                       VOS_UINT32                                 *puwTimer,
                       VOS_UINT32                                  uwTimeOutLen,
                       DRV_TIMER_MODE_ENUM_UINT16                  enMode,
                       DRV_TIMER_INT_FUNC                          pfFunc,
                       VOS_UINT32                                  uwPara);
extern VOS_UINT32 DRV_TIMER_Stop(VOS_UINT32 *puwTimer);
extern VOS_VOID DRV_TIMER_StopDwApbTimer(
                       VOS_UINT32              uwBaseAddr,
                       VOS_UINT32              uwTimerIndx);
extern VOS_UINT32 DRV_TIMER_StopPrecise(VOS_UINT32 *puwTimer);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_drv_timer_hifi.h */

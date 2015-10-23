

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef __CPU_C_H__
#define __CPU_C_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MAX_PID_USED 3
#define OS_PID_MASK  (0xf00)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : OS_HOOK_TYPE_ENUM
 功能描述  : 钩子函数类型枚举定义
*****************************************************************************/
enum OS_HOOK_TYPE_ENUM
{
    OS_HOOK_TYPE_ENTER_INTR=0,
    OS_HOOK_TYPE_EXIT_INTR,
    OS_HOOK_TYPE_TASK_SWITCH,
    OS_HOOK_TYPE_MSG_GET,
    OS_HOOK_TYPE_NMI,
    OS_HOOK_TYPE_EXCEPTION,
    OS_HOOK_TYPE_APPINIT,
    OS_HOOK_TYPE_IDLE,
    OS_HOOK_TYPE_BUTT
};
typedef VOS_UINT16 OS_HOOK_TYPE_ENUM_UINT16;

/*****************************************************************************
 实体名称  : OS_INTR_CONNECT_ENUM
 功能描述  : 中断处理钩子函数类型枚举定义
*****************************************************************************/
enum OS_INTR_CONNECT_TYPE_ENUM
{
    OS_INTR_CONNECT_00=0,        /*Int 0   type / priority level NMI / 6          */
    OS_INTR_CONNECT_01,          /*Int 1   type / priority level Software / 3  */
    OS_INTR_CONNECT_02,          /*Int 2   type / priority level ExtLevel / 2  */
    OS_INTR_CONNECT_03,          /*Int 3   type / priority level ExtLevel / 2  */
    OS_INTR_CONNECT_04,          /*Int 4   type / priority level ExtLevel / 2  */
    OS_INTR_CONNECT_05,          /*Int 5   type / priority level Timer / 3     */
    OS_INTR_CONNECT_06,          /*Int 6   type / priority level Timer / 4     */
    OS_INTR_CONNECT_07,          /*Int 7   type / priority level ExtLevel / 3  */
    OS_INTR_CONNECT_08,          /*Int 8   type / priority level ExtLevel / 3  */
    OS_INTR_CONNECT_09,          /*Int 9   type / priority level ExtLevel / 3  */
    OS_INTR_CONNECT_10,          /*Int 10   type / priority level ExtLevel / 2 */
    OS_INTR_CONNECT_11,          /*Int 11   type / priority level ExtLevel / 2 */
    OS_INTR_CONNECT_12,          /*Int 12   type / priority level ExtLevel / 2 */
    OS_INTR_CONNECT_13,          /*Int 13   type / priority level ExtLevel / 2 */
    OS_INTR_CONNECT_14,          /*Int 14   type / priority level ExtLevel / 2 */
    OS_INTR_CONNECT_15,          /*Int 15   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_16,          /*Int 16   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_17,          /*Int 17   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_18,          /*Int 18   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_19,          /*Int 19   type / priority level Timer / 2    */
    OS_INTR_CONNECT_20,          /*Int 20   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_21,          /*Int 21   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_22,          /*Int 22   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_23,          /*Int 23   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_24,          /*Int 24   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_25,          /*Int 25   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_26,          /*Int 26   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_27,          /*Int 27   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_28,          /*Int 28   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_29,          /*Int 29   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_30,          /*Int 30   type / priority level ExtLevel / 1 */
    OS_INTR_CONNECT_31,          /*Int 31   type / priority level ExtEdge / 1  */
    OS_INTR_CONNECT_BUTT
};
typedef VOS_UINT16 OS_INTR_CONNECT_TYPE_ENUM_UINT16;


#define SET_STKREG(r,v) sp[(r) >> 2] = (VOS_STK)(v)

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
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : HOOK_FUN_TYPE
 功能描述  : 钩子函数类型定义
*****************************************************************************/
typedef VOS_VOID (*HOOK_FUN_TYPE)(VOS_VOID);

/*****************************************************************************
 实体名称  : VOS_IDLE_HOOK_FUN_TYPE
 功能描述  : IDLE任务钩子函数类型定义
*****************************************************************************/
typedef VOS_VOID (*VOS_IDLE_HOOK_FUN_TYPE)(VOS_UINT32 ulVosCtxSw);

/*****************************************************************************
 实体名称  : INTR_HOOK_FUN_TYPE
 功能描述  : 中断进入/退出钩子函数类型定义
*****************************************************************************/
typedef VOS_VOID  (*INTR_HOOK_FUN_TYPE )(VOS_UINT32 uwIntNo);

/*****************************************************************************
 实体名称  : VOS_TASK_SWITCH_HOOK
 功能描述  : 任务切换钩子函数类型定义
*****************************************************************************/
typedef VOS_VOID (*VOS_TASK_SWITCH_HOOK)( VOS_VOID *tcb_old, VOS_VOID *tcb_new);

/*****************************************************************************
 实体名称  : VOS_MSG_HOOK_FUNC
 功能描述  : 消息获取钩子函数类型定义
*****************************************************************************/
typedef VOS_VOID (*VOS_MSG_HOOK_FUNC)( VOS_VOID *pMsg);

/*****************************************************************************
 实体名称  : VOS_EXCEPTION_HOOK_FUNC
 功能描述  : 异常处理钩子函数类型定义
*****************************************************************************/
typedef VOS_VOID (*VOS_EXCEPTION_HOOK_FUNC)( VOS_UINT32 uwExceptionNo);




/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOS_VOID *g_pfVosHookFuncTable[OS_HOOK_TYPE_BUTT];

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID VOS_RegisterEnterIntrHook(INTR_HOOK_FUN_TYPE  pfnEnterIntrHook);
extern VOS_VOID VOS_RegisterExitIntrHook(INTR_HOOK_FUN_TYPE  pfnExitIntrHook);
extern VOS_VOID VOS_RegTaskSwitchHook(VOS_TASK_SWITCH_HOOK pfnTaskSwitchHook);
extern VOS_VOID VOS_RegisterMsgGetHook(VOS_MSG_HOOK_FUNC pfnMsgGetHook);
extern VOS_VOID VOS_ConnectInterrupt(VOS_UINT32 uwIntrNo, HOOK_FUN_TYPE pfnInterruptHook);
extern VOS_VOID VOS_RegisterExceptionHandler(VOS_EXCEPTION_HOOK_FUNC pfnExceptionHook);
extern VOS_VOID VOS_RegisterNMIHook(HOOK_FUN_TYPE  pfnNmiHook);
extern VOS_VOID VOS_EnableInterrupt(VOS_UINT32 uwIntNo);
extern VOS_VOID VOS_DisableInterrupt(VOS_UINT32 uwIntNo);
extern VOS_VOID VOS_RegAppInitFuncHook(HOOK_FUN_TYPE  pfnAppInitFuncHook);
extern VOS_VOID VOS_SoftIntTriger(VOS_UINT32 uwInfo);
extern VOS_VOID VOS_RegIdleHook(VOS_IDLE_HOOK_FUN_TYPE pfnIdleHook);

extern VOS_VOID  OS_UserExit(VOS_VOID);
extern VOS_VOID VOS_SetInterruptMask(VOS_UINT32 uwBitEnable);
extern VOS_UINT32 VOS_GetInterruptMask(VOS_VOID);
extern VOS_UINT32 VOS_GetInterrupt( VOS_VOID );


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of cpu_c.h */

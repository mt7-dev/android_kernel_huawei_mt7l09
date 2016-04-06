

#ifndef SRECORDER_MODEM_LOG_H
#define SRECORDER_MODEM_LOG_H


/*----includes-----------------------------------------------------------------------*/

#include "srecorder_common.h"


/*----c++ support--------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


/*----export macroes-----------------------------------------------------------------*/


/*----export prototypes---------------------------------------------------------------*/


/*----global variables-----------------------------------------------------------------*/


/*----export function prototypes--------------------------------------------------------*/

/**
    @function: int srecorder_get_modem_log(srecorder_reserved_mem_info_for_log_t *pmem_info)
    @brief: 读取modem死机堆栈和运行日志

    @param: pmem_info SRecorder的保留内存信息
    
    @return: 0 - 成功；-1-失败

    @note: 
*/
int srecorder_get_modem_log(srecorder_reserved_mem_info_t *pmem_info);


/**
    @function: int srecorder_init_modem_log(srecorder_module_init_params_t *pinit_params)
    @brief: 初始化modem日志模块

    @param: pinit_params 模块初始化数据
    
    @return: 0 - 成功；-1-失败

    @note: 
*/
int srecorder_init_modem_log(srecorder_module_init_params_t *pinit_params);


/**
    @function: void srecorder_exit_modem_log(void)
    @brief: 退出modem日志模块

    @param: none
    
    @return: none

    @note: 
*/
void srecorder_exit_modem_log(void);


#ifdef __cplusplus
}
#endif
#endif /* SRECORDER_MODEM_LOG_H */


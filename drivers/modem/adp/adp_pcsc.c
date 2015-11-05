#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "drv_pcsc.h"



/*****************************************************************************
* 函 数 名  : pcsc_usim_ctrl_cmd
*
* 功能描述  : USIMM模块处理PCSC下发命令，完成后，调此函数通知PCSC
*
* 输入参数  : cmd_type:USIMM处理命令
*             status:  处理的处理结果，是否成功
*             buf:   命令处理后的回应数据
*             length:回应数据的长度
* 输出参数  : 无
* 返 回 值  : 0:命令处理完成; <0:命令处理错误
*****************************************************************************/
unsigned long pcsc_usim_ctrl_cmd(unsigned long cmd_type, unsigned long status, 
                unsigned char *buf, unsigned long length)
{
    int ret;
    ret = gusim_ctrl_cmd(0, (u32)cmd_type, (u32)status, (u8*)buf, (u32)length);

    return (unsigned long)ret;
}


/*****************************************************************************
* 函 数 名  : pcsc_usim_int
*
* 功能描述  : USIM模块注册回调函数接口
*
* 输入参数  : pFun: USIMM模块处理函数指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
void pcsc_usim_int(pFunAPDUProcess pFun1, GetCardStatus pFun2)
{
    gusim_reg_cb(0, pFun1, pFun2);
}



/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_V9R1_BBIT_H__
#define __DRV_V9R1_BBIT_H__

#include "drv_comm.h"


/************************V9R1BBIT START***********************/
typedef enum
{
    DBLED_DRV1 = 0,        /* DRV调试灯1 */
    DBLED_DRV2,            /* DRV调试灯2 */
    DBLED_DSP1 = 2,        /* DSP调试灯1 */
    DBLED_DSP2,            /* DSP调试灯2 */
    DBLED_RESERVED1 = 4,   /* 保留调试灯1 */
    DBLED_RESERVED2,       /* 保留调试灯2 */
    DBLED_MAX
}DBLED_ID_E;
extern int DRV_DBLED_ON(unsigned int ledId);


extern int DRV_DBLED_OFF(unsigned int ledId);

/*V9R1SFT SC基地址，BBIT平台无SC*/
#define SC_BASE    SOC_SCTRL_BASE_ADDR
extern int DRV_COPY_NVUSE_TO_NVBACKUP ( void );



extern int DRV_COPY_NVUPGRADE_TO_NVUSE( void );
/*************************V9R1BBIT END************************/

#endif


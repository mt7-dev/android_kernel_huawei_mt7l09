
#ifndef _CIPHER_ACC_H
#define _CIPHER_ACC_H

#include "bsp_cipher.h"
#include "cipher_balong_common.h"
#include "cipher_balong.h"/*为了使用BD/RD结构体*/

#ifndef CHN_BUSY
#define CHN_BUSY                    0
#endif
#ifndef CHN_FREE
#define CHN_FREE                    1
#endif

#define ACC_FIFO_ERROR    0
#define ACC_FIFO_STAT_NUM ACC_STAT_BOTTOM
#define ACC_CHN_NUM       0	/*ACC使用的通道号*/
#define ACC_FIFO_NUM      2	/*ACC使用的FIFO队列数*/

/*组包加速寄存器*/
#define CHN_STATBIT          0x40000000    /*取通道使能寄存器的第30位*/
#define CHN_PACK_ENBITS      0xBFFFFFFF    /*禁止pack_en位，通道使能寄存器的第30位   */
#define CHN_ENBITS           0x1           /*使能ch_en位，通道使能寄存器的第0位 */
#define CHN_BDRESET          0x2           /*通道复位寄存器，清空BD队列，复位读指针*/
#define CHN_WPT              0xFFFFFC00    /*通道写指针复位*/
#define CHN_DENBITS          0xFFFFFFFE    /*通道使能寄存器最高位清0，暂停通道*/
#define CIPHER_INT0_CH0      0x3F          /*中断寄存器0中，对应的通道0的低6位清1*/
#define CIPHER_INT1_CH0      0xD           /*中断寄存器1中，对应的通道0的0、2、3位清1*/
#define CHN_CFGUSR           0x60          /*将通道0的配置寄存器的userfiled域配成3*/

#define ALIN_BYTES         8                          /* 8字节对齐要求 */
#define CIPHER_DESC_SIZE   sizeof(struct cipher_bd_s) /* 硬件所需每个描述符长度(Byte) */
#define ACC_FIFO_MAX_DEEP  200                        /* BDFIFO的最大深度*/


#define ACC_ERROR_PRINT(errno) \
do{\
    {\
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ACC, "[ACC API ERROR]:\n\t\t <func>: %s;  <line>: %d;  <Errno>: %s (0x%08x)\n\n", \
               (int)__FUNCTION__, (int)__LINE__, (int)#errno, errno);\
    }\
}while(0)
#define ACC_INFOR_PRINT(fmt, ...)  (bsp_trace(BSP_LOG_LEVEL_INFO,  BSP_MODU_ACC, "[ACC API INFOR]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

/*BDFIFO状态集*/
enum ACC_FIFO_STATUS_E
{
    ACC_STAT_WORK = 0,           /* 正在被使用 */
    ACC_STAT_CFG,                /* 已经被配置 */
    ACC_STAT_IDLE,               /* 空闲状态 */
    ACC_STAT_BOTTOM
};

/* 组包加速模块管理全局结构体 */
struct acc_fifo_mgr_s
{
	u32                     bd_fifo_addr;    /* FIFO描述符基地址 */
    enum ACC_FIFO_STATUS_E  fifo_stat;       /* BDFIFO状态*/
    u32                     bd_fifo_cfg_num; /* 软件维护的待写地址 */
};

/*ACC Debug 结构体*/
 struct acc_debug_s
{
    u32 get_fifo_times;
    u32 cfg_dma_times;
    u32 cfg_cipher_times;
    u32 enable_times;
    u32 drop_pak_times;
};

/*ACC模块专用通道管理结构体*/
struct acc_chx_mgr_s
{
	struct acc_fifo_mgr_s acc_fifo_mgr[ACC_FIFO_NUM];
	struct acc_debug_s acc_debug;
	u32	acc_fifo_deep;
	u32 init_flag;
	u32 cur_used_fifo_num;
};


/*****************************************************************************
* 函 数 名  : bsp_acc_init
*
* 功能描述  : 初始化组包加速模块
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 成功/失败
*
* 修改记录  : 2011年1月8日   wangjing  creat
*****************************************************************************/
s32 bsp_acc_init ();
/*****************************************************************************
* 函 数 名  : bsp_acc_get_bdfifo_addr
*
* 功能描述  : 用于获取当前可用的BDFIFO首地址
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : BDFIFO首地址
*****************************************************************************/
u32 bsp_acc_get_bdfifo_addr();
/*****************************************************************************
* 函 数 名  : bsp_acc_dma
*
* 功能描述  : 为指定通道配置描述符，用于单纯的DMA搬移
*
* 输入参数  : bd_fifo_addr: BDFIFO首地址
*                          p_in_mem_mgr    : 输入buffer数据块的首地址
*                          p_out_mem_mgr   : 输出buffer数据块的首地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_dma(u32 bd_fifo_addr, const void* p_in_mem_mgr, const void* p_out_mem_mgr,
                            ACC_SINGLE_CFG_S *p_cfg);
/*****************************************************************************
* 函 数 名  : bsp_acc_cipher
*
* 功能描述  : 为指定通道配置描述符，用于需要Cipher进行加密的操作
*
* 输入参数  : bd_fifo_addr: BDFIFO首地址
*                          p_in_mem_mgr    : 输入buffer数据块的首地址
*                          p_out_mem_mgr   : 输出buffer数据块的首地址
*                          p_cfg       : 组包加速配置信息
* 输出参数   : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_cipher(u32 bd_fifo_addr, const void* p_in_mem_mgr, const void* p_out_mem_mgr,
                               ACC_SINGLE_CFG_S *p_cfg);
/*****************************************************************************
* 函 数 名  : bsp_acc_get_status
*
* 功能描述  : 获取当前通道状态
*
* 输入参数  :  无
* 输出参数   : 无
* 返 回 值  : 通道忙/空闲
*****************************************************************************/
s32 bsp_acc_get_status();
/*****************************************************************************
* 函 数 名  : bsp_acc_enable
*
* 功能描述  : 使能组包加速
*
* 输入参数  : bd_fifo_addr : BDFIFO首地址
*
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_enable(u32 bd_fifo_addr);

void bsp_acc_debug_show();

#endif

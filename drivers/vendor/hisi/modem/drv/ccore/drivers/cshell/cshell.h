/* cshell.h -  balong cshell driver header file */

/* Copyright (C) 2012 Huawei Corporation */

#ifndef __CSHELL__
#define __CSHELL__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
                                                Cshell  Header files
**************************************************************************/
#include <ioLib.h>
#include <sioLib.h>
#include <selectLib.h>

#include "osl_types.h"
#include <osl_malloc.h>
#include <osl_sem.h>
#include <bsp_om.h>

/**************************************************************************
                                                Cshell Macro Definitions
**************************************************************************/
#define SIO_IRDA_MODE               0x1040
#define UART_DMA_MODE               0x1041
#define IO_BUFFER_MAX               8192     /* 接收缓冲区大小 */
#define IO_BUFFER_LEVEL             2048     /* 接收缓冲区处理水位线 */
#define TTY_BUFFER_MAX              2048     /* tty设备缓冲区大小 */
#define CSHELL_ICC_FIFO_DEPTH       1024*3
#define CSHELL_ICC_CHANNEL_ID       31
#define CSHELL_TASK_PRI             (20)
#define CSHELL_TASK_STACK_SIZE      (0x400)
#define CSHELL_OK                   0
#define CSHELL_ERROR                (-1)

#ifndef min
#define min(__x,__y)                ((__x)<(__y)?(__x):(__y))
#endif

#define cshell_safe_free(p) \
do \
{ \
	if(p) \
	{ \
		osl_free(p); \
		p = NULL; \
	} \
} \
while(0)

#define cshell_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_CSHELL, "[cshell]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define cshell_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_INFO,  BSP_MODU_CSHELL, "[cshell]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define cshell_print_debug(fmt, ...) \
do {                               \
    if (g_cshell_dbg.print_sw)    \
        cshell_print_error(fmt, ##__VA_ARGS__);\
} while (0)

/**************************************************************************
                                                Cshell types Definitions
**************************************************************************/
typedef enum cshell_mode_tpye
{
    CSHELL_MODE_UART,
    CSHELL_MODE_USB,
}cshell_mode_tpye;

typedef struct
{
    SIO_DRV_FUNCS   * ptr_drv_funcs;            /* driver functions */
    int             (*get_tx_char) ();          /* pointer to xmitr function */
    int             (*put_rx_char) ();          /* pointer tp rcvr function */
    void *          get_tx_arg;
    void *          put_rx_arg;
    int             baud_rate;
    int             channel_mode;               /*int or poll*/
    int             options;
    u16             send_wt;                    /* Shell发送缓冲区写指针  */
    u16             send_rd;                    /* Shell发送缓冲区读指针  */
    u8  *           ptr_send_buf;               /* Shell发送缓冲区指针    */
    u8  *           ptr_back_buf;               /* 底层通道发送缓冲区指针 */
    u8  *           ptr_recv_buf;
    osl_sem_id      cshell_recv_sem;            /* Shell准备发送数据      */
    s32             cshell_send_permission;     /* 使能向A核发送log标志       */
    SEL_WAKEUP_LIST sel_wakeup_list;            /* telling the upp layer that the data comes*/
    u32 icc_channel_id;
    int shell_send_tid;
    int shell_mode;
} SHELL_IO_CHAN;

struct cshell_debug
{
	int state;
	int print_sw;
	int lost_data_cnt;
	int recv_data_cnt;
	int send_fail_cnt;
	int send_succ_cnt;
	int send_busy_cnt;
};

typedef void (*cprint_hook)(void);


/*****************************************************************************
* 函 数 名     :  cshell_init
*
* 功能描述  :  cshell初始化
*
* 输入参数  :  void
* 输出参数  :  无
*
* 返 回 值     :  无
*
* 修改记录  :
*****************************************************************************/
int  cshell_init(void);
void cshell_send_data(char* data, int length);
void cshell_register_hook(cprint_hook hook);

#ifdef __cplusplus
}
#endif

#endif



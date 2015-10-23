/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  sys.c
*
*   作    者 :  wuzechun
*
*   描    述 :  系统公用函数
*
*   修改记录 :  2011年6月29日  v1.00  wuzechun  创建
*
*************************************************************************/

#include "OnChipRom.h"
#include "OcrShare.h"
#include "sys.h"

/*****************************************************************************
* 函 数 名  : delay
*
* 功能描述  : 使用Timer6定时器精准微秒延时,最大可支持223,696,213us
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void delay(UINT32 us)
{
    /* 防止小数被舍弃/溢出 */
    volatile UINT32 ulDelayClks = (TIMER6_FREQ * 10 / US_PER_SECEND) * us / 10;

    /* disable */
    OUTREG32(TIMER6_REGOFF_CTRL, TIMER_CTRL_INTMASK);

    /* 设置超时时间 */
    OUTREG32(TIMER6_REGOFF_LOAD, ulDelayClks);

    /* 等待设置完成 */
    while(TIMER_CTRL_EN_ACK != (INREG32(TIMER6_REGOFF_CTRL)&TIMER_CTRL_EN_ACK));

    /* enable中断,开始计时 */
    OUTREG32(TIMER6_REGOFF_CTRL, 1);

    /* 等待超时 */
    while(TIMER_INS_INT != (TIMER_INS_INT & (INREG32(TIMER6_REGOFF_INTSTATUS))));

    /* disable */
    OUTREG32(TIMER6_REGOFF_CTRL, TIMER_CTRL_INTMASK);
}

/*****************************************************************************
* 函 数 名  : print_info
*
* 功能描述  : 字符串打印，换行需要加\r\n
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void print_info(const void *pucBuffer)
{
#if PLATFORM!=PLATFORM_EDA
    UINT32 ulStrLen = 0x00;
    UINT8 *pucTmp = (UINT8 *)pucBuffer;
    UINT32 u32Loop;

    while('\0' != *pucTmp++ )
    {
        ulStrLen++;
    }
    pucTmp = (UINT8 *)pucBuffer;
    while(ulStrLen--)
    {
        /*check tx fifo is empty*/
        u32Loop = 10000;
        while(0x00 == ((*(volatile UINT32 *)(UART0_BASE+UART_USR)) & 0x04))
        {
            /* 超时直接返回,这里没有更好的方法 */
            if(0 == --u32Loop)
            {
                return;
            }
        }
        *(UINT32 *)(UART0_BASE + UART_THR) = *pucTmp;
        pucTmp++;
    }
#endif
}

/*****************************************************************************
* 函 数 名  : print_info_with_u32
*
* 功能描述  : print information and a value
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void print_info_with_u32( const void *msg, UINT32 ulValue )
{
#if PLATFORM!=PLATFORM_EDA

    static const unsigned char hex_value[] = "0123456789ABCDEF";
    UINT8 curr_msg [ 16 ];
    UINT8 *p;
    print_info( msg );
    p = curr_msg;
    *p++ = hex_value[ (ulValue >> 28) & 0x0F ];
    *p++ = hex_value[ (ulValue >> 24) & 0x0F ];
    *p++ = hex_value[ (ulValue >> 20) & 0x0F ];
    *p++ = hex_value[ (ulValue >> 16) & 0x0F ];
    *p++ = hex_value[ (ulValue >> 12) & 0x0F ];
    *p++ = hex_value[ (ulValue >>   8) & 0x0F ];
    *p++ = hex_value[ (ulValue >>   4) & 0x0F ];
    *p++ = hex_value[ (ulValue >>   0) & 0x0F ];
    *p++ = 0;

    print_info( curr_msg );
#endif
}

/*****************************************************************************
* 函 数 名  : waitBitSetWithTimeout
*
* 功能描述  : 超时等待寄存器某位被设置成期望的值
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  : OK 成功等到位被设置
*                ERROR 超时未等到位被设置
*
* 其它说明  :
*
*****************************************************************************/
INT32 waitBitSetWithTimeout(UINT32 reg, UINT32 bitmask, UINT32 value, UINT32 timeout)
{
    volatile UINT32 ulLoop = timeout;
//print_info_with_u32("ulLoop:", ulLoop);
    while(value != (INREG32(reg) & bitmask)
          && (0 != --ulLoop));

//print_info_with_u32("ulLoop:", ulLoop);
    return ((0 == ulLoop) ? ERROR : OK);
}

/*****************************************************************************
* 函 数 名  : wdtReboot
*
* 功能描述  : 使用看门狗复位
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void wdtReboot( void )
{
    /*设置系统控制器,配置看门狗计数时钟使能*/
    OUTREG32(SC_WDTCTRL, 0x69200009);

    /* 寄存器解锁 */
    OUTREG32(WDG_LOCK, WDT_LOCK_VALUE);

    /*设置看门狗计数器初值*/
    OUTREG32(WDG_LOAD, 1000);

    /* 使能复位和中断 */
    OUTREG32(WDG_CONTROL, 0x3);

    /* 寄存器上锁 */
    OUTREG32(WDG_LOCK, 0);

    /* while循环等待看门狗超时复位 */
    while(1);   /*lint !e716*/

}

/*****************************************************************************
* 函 数 名  : wdtRebootDelayMs
*
* 功能描述  : 延时复位
*
* 输入参数  : us,延时毫秒数
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void wdtRebootDelayMs( UINT32 ms )
{
    delay(ms * 1000);

    wdtReboot();
}

/*****************************************************************************
* 函 数 名  : timer0_get_current_tick
*
* 功能描述  : 获取当前tick
*
* 输入参数  : VOID
* 输出参数  : 无
*
* 返 回 值  : 当前tick值
*
* 其它说明  : 无
*
*****************************************************************************/
UINT32 timer0_get_current_tick(VOID)
{
    volatile UINT32 ulTick;

    ulTick = INREG32(TIMER0_REGOFF_VALUE);

    return ulTick;
}

/*****************************************************************************
* 函 数 名  : setErrno
*
* 功能描述  : 在AXI Mem中设置错误码
*
* 输入参数  : errno,错误码
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
void setErrno( INT32 errno )
{
    volatile tOcrShareData *pShareData = (tOcrShareData*)M3_SRAM_SHARE_DATA_ADDR;
    pShareData->errno = errno;
}


/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  sys.h
*
*   作    者 :  wuzechun
*
*   描    述 :  系统公用头文件
*
*   修改记录 :  2011年4月22日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __SYS_H__
#define __SYS_H__

#include "types.h"

#define DATA_LARGE  1
#define DATA_SMALL  (-1)
#define DATA_EQUAL  0

/* register control */
#define READ_REGISTER_UCHAR(addr) (*(volatile UINT8 *)(addr))
#define READ_REGISTER_USHORT(addr) (*(volatile UINT16 *)(addr))
#define READ_REGISTER_ULONG(addr) (*(volatile UINT32 *)(addr))

#define WRITE_REGISTER_UCHAR(addr,val) (*(volatile UINT8 *)(addr) = (val))
#define WRITE_REGISTER_USHORT(addr,val) (*(volatile UINT16 *)(addr) = (val))
#define WRITE_REGISTER_ULONG(addr,val) (*(volatile UINT32 *)(addr) = (val))

#define INREG8(x)           READ_REGISTER_UCHAR(x)
#define OUTREG8(x, y)       WRITE_REGISTER_UCHAR(x, (UINT8)(y))
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))
#define SETBITVALUE8(addr, mask, value) \
                OUTREG8(addr, (INREG8(addr)&(~mask)) | (value&mask))

#define INREG16(x)          READ_REGISTER_USHORT(x)
#define OUTREG16(x, y)      WRITE_REGISTER_USHORT(x,(UINT16)(y))
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))
#define SETBITVALUE16(addr, mask, value) \
                OUTREG16(addr, (INREG16(addr)&(~mask)) | (value&mask))

#define INREG32(x)          READ_REGISTER_ULONG(x)
#define OUTREG32(x, y)      WRITE_REGISTER_ULONG(x, (UINT32)(y))
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))
#define SETBITVALUE32(addr, mask, value) \
                OUTREG32((addr), (INREG32(addr)&(~(mask))) | ((value)&(mask)))

#define inline
#define IN
#define OUT

/*PLL CLK 800M, EMMC need 25M, crg div 15+1*/
#define EMMC_SET_CLK()\
{\
    OUTREG32(SC_CRG_DIV3, (INREG32(SC_CRG_DIV3)|0xF)&~(0x1<<4)); \
}

/* timer0 operation, 用于超时计数,不会同时运行的模块可重复使用 */
#define TIMER0_INIT()\
{\
    OUTREG32(TIMER0_REGOFF_CTRL, TIMER_CTRL_INTMASK);       /* disable */\
    OUTREG32(TIMER0_REGOFF_LOAD, 0xFFFFFFFF); /* init-count */\
    while(TIMER_CTRL_EN_ACK != (INREG32(TIMER0_REGOFF_CTRL)&TIMER_CTRL_EN_ACK));\
    OUTREG32(TIMER0_REGOFF_CTRL, 0x5);       /* enable, free-running */\
}

#define TIMER0_DEINIT()\
{\
    OUTREG32(TIMER0_REGOFF_CTRL, TIMER_CTRL_INTMASK);       /* disable */\
}

/*
    We use the time0 as timeout check, the input clock is 19.2M.
    one tick = 1/(19.2*1000*1000) second.and the timeout time is 0xFFFFFFFF/(19.2*1000*1000)=223.69621328125s
*/
#define TIMER0_VAR_DECLAR          volatile UINT32 ulStartTick = 0, ulTickTimeout = 0
#define TIMER0_INIT_TIMEOUT( _ms )  { ulStartTick = timer0_get_current_tick();  ulTickTimeout = (_ms) * MS_TICKS; }  /* ms to tick */
#define TIMER0_IS_TIMEOUT()         ( (ulStartTick - timer0_get_current_tick()) > ulTickTimeout )

void delay(UINT32 us);
void print_info(const void *pucBuffer);
void print_info_with_u32( const void *msg, UINT32 ulValue );
INT32 waitBitSetWithTimeout(UINT32 reg, UINT32 bitmask, UINT32 value, UINT32 timeout);
void wdtReboot( void );
void wdtRebootDelayMs( UINT32 ms );
void setErrno( INT32 errno );
UINT32 timer0_get_current_tick( void );

#endif


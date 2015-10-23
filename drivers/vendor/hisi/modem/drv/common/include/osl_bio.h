

#ifndef __OSL_BIO_H
#define __OSL_BIO_H

#ifdef __KERNEL__
#ifndef __ASSEMBLY__
#include <linux/io.h>
#include <asm/barrier.h>
static __inline__  void cache_sync(void)
{
	mb();
}
#endif
#include <bsp_memmap.h>

#elif defined(__VXWORKS__) && !defined(__ASSEMBLY__)

static __inline__ void DWB(void) /* drain write buffer */
{
//    asm volatile ( "mcr p15, 0, r0, c7, c10, 4;");
    asm volatile ( "dsb;" );
}

extern void (* cache_sync_ops)(void);
static __inline__  void cache_sync(void)
{
    DWB();
    if(cache_sync_ops)
        cache_sync_ops();
}

static __inline__ void writel(unsigned val, unsigned addr)
{
    cache_sync();
    (*(volatile unsigned *) (addr)) = (val);
}

static __inline__ void writeb(unsigned val, unsigned addr)
{
    cache_sync();
    (*(volatile unsigned char *) (addr)) = (val);
}

static __inline__ unsigned readl(unsigned addr)
{
    return (*(volatile unsigned *) (addr));
}

static __inline__ unsigned readb(unsigned addr)
{
	return (*(volatile unsigned char*)(addr));
}
static __inline__ void writel_relaxed(unsigned val, unsigned addr)
{
    (*(volatile unsigned *) (addr)) = (val);
}
static __inline__ void writeb_relaxed(unsigned val, unsigned addr)
{	
    (*(volatile unsigned char *) (addr)) = (val);
}

#elif defined(__CMSIS_RTOS)
 /* drain write buffer */
static inline void DWB(void)
{
	  //asm volatile ( "dsb;" );
}

static inline void ISB(void) /* drain write buffer */
{
	asm volatile ( "isb;");
}

static inline void WFI(void)
{
	asm volatile ( "wfi;");
}

#ifndef writel
static inline void writel(unsigned val, unsigned addr)
{
    DWB();
    (*(volatile unsigned *) (addr)) = (val);
    DWB();
}
#endif

#ifndef writeb
static inline void writeb(unsigned val, unsigned addr)
{
    DWB();
    (*(volatile unsigned char *) (addr)) = (val);
    DWB();
}
#endif

#ifndef readl
static inline unsigned readl(unsigned addr)
{
    return (*(volatile unsigned *) (addr));
}
#endif

#ifndef readb
static inline unsigned readb(unsigned addr)
{
	return (*(volatile unsigned char*)(addr)); 
}
#endif

static __inline__  void cache_sync(void)
{
	return;
}


#else

#endif	/* __KERNEL__ */

#ifndef IO_ADDRESS
#define IO_ADDRESS(x)	    ((((x) & 0xF0000000) >> 4) | ((x) & 0x00FFFFFF) | 0xF0000000)
#endif

#endif	/* __OSL_BIO_H */

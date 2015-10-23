/* sysL2Cache.c - ARM Versatile Express A9x4 L2 Cache Routines */

/*
 * Copyright (c) 2010 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01b,29jun10, my_  update for first release.
01a,28jun10, z_l  created.
*/

/*
DESCRIPTION
This file supports the external L2 cache, which is connected to the ARM Cortex
A9 processor via the L2 cache controller(PL310). Some APIs for cache operation,
such as flush/clear/invalidate, are implemented in this file. These APIs are
used in VxWorks arch library to support L2 cache.
*/

#include <vxWorks.h>
#include "config.h"
#include <cacheLib.h>
#include <arm_pbxa9.h>
#include <intLib.h>

#define L2_CTRL_BASE             HI_MDMA9_L2_REGBASE_ADDR_VIRT
#define L2_CACHE_ID              (L2_CTRL_BASE + 0x0)
#define L2_CACHE_TYPE            (L2_CTRL_BASE + 0x4)
#define L2_CONTROL               (L2_CTRL_BASE + 0x100)
#define L2_AUX_CONTROL           (L2_CTRL_BASE + 0x104)
#define L2_INT_MASK              (L2_CTRL_BASE + 0x214)
#define L2_INT_CLEAR             (L2_CTRL_BASE + 0x220)

#define L2_CACHE_SYNC            (L2_CTRL_BASE + 0x730)
#define L2_INVALIDATE_PA         (L2_CTRL_BASE + 0x770)
#define L2_INVALIDATE_WAY        (L2_CTRL_BASE + 0x77c)
#define L2_CLEAN_PA              (L2_CTRL_BASE + 0x7b0)
#define L2_CLEAN_WAY             (L2_CTRL_BASE + 0x7bc)
#define L2_CLEAN_INVALIDATE_PA   (L2_CTRL_BASE + 0x7f0)
#define L2_CLEAN_INVALIDATE_WAY  (L2_CTRL_BASE + 0x7fc)

#define L2_PREFETCH_CTRL        (L2_CTRL_BASE + 0xF60)
#define L2_POWER_CTRL           (L2_CTRL_BASE + 0xF80)

#define AUX_EARLY_BRESP         (0x1<<30)
#define AUX_INSTR_PREFETCH      (0x1<<29)
#define AUX_DATA_PREFETCH       (0x1<<28)
#define AUX_FULL_LINE_OF_ZERO   (0x1<<0)

#define PREF_DOUBLE_LINEFILL    (0x1<<30)
#define PREF_INSTR_PREFETCH     (0x1<<29)   /*the same to aux bit29*/
#define PREF_DATA_PREFETCH      (0x1<<28)  /*the same to aux bit28*/
#define PREF_PREFETCH_DROP      (0x1<<24)
#define PREF_PREFETCH_OFFSET    (0x7)

#define POWER_CLK_GATING        (0x1<<1)
#define POWER_STANDBY_MODE      (0x1<<0)


#define L2_CONTROL_ENABLE        (0x1)
#define L2_WAY_ALL               (0xff) /* 8-ways */
#define L2_OPERATION_IN_PROGRESS (0x1)
#define L2_INT_MASK_ALL          (0x0)
#define L2_INT_CLEAR_ALL         (0x1ff)

#define L2_CACHE_SIZE            (0x80000)
#define L2_CACHE_LINE_SIZE       (0x20)
#define L2_CACHE_LINE_MASK       (L2_CACHE_LINE_SIZE - 1)

/*
 * The following five function pointers are defined and used in VxWorks ARM
 * arch library. To support L2 cache, customer BSP should initialize them with
 * concrete implementation.
 */

IMPORT void (*_pSysL2CacheEnable)(CACHE_TYPE cacheType);
IMPORT void (*_pSysL2CacheDisable)(CACHE_TYPE cacheType);
IMPORT void (*_pSysL2CacheInvFunc)(CACHE_TYPE cacheType, void * start, void * end);
IMPORT void (*_pSysL2CacheFlush)(CACHE_TYPE cacheType, void * start, void * end);
IMPORT void (*_pSysL2CacheClear)(CACHE_TYPE cacheType, void * start, void * end);
static unsigned int l2cache_init_flag = 0;
/*******************************************************************************
*
* sysL2Invalidate - invalidate L2 Cache
*
* This routine invalidates L2 cache ranging from 'pMemStart' to 'pMemEnd'
*
* RETURNS: N/A
*
* ERRNO
*/

LOCAL void cacheSync(void)
{
	ARMA9CTX_REGISTER_WRITE(L2_CACHE_SYNC, 0);
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory"); 
}

extern void (* cache_sync_ops)(void);
extern void l2_cache_sync(void);


LOCAL void sysL2Invalidate
    (
    CACHE_TYPE cacheType,
    void * pMemStart,
    void * pMemEnd
    )
    {
    int phyAddr, tempSize, invalidateSize,lockKey;

	lockKey = intLock();

    if ((UINT32)pMemEnd == ENTIRE_CACHE) /* invalidate the whole L2 cache */
        {
        ARMA9CTX_REGISTER_WRITE(L2_INVALIDATE_WAY, L2_WAY_ALL);

        /* poll state until the background invalidate operation is complete */

        while (ARMA9CTX_REGISTER_READ(L2_INVALIDATE_WAY) & L2_WAY_ALL);

		cacheSync();

		intUnlock(lockKey);
		
        return;
        }
    else
        {
        if ((UINT32)pMemEnd > (UINT32)pMemStart) /* check the parameters */
            invalidateSize = (UINT32)pMemEnd - (UINT32)pMemStart;
        else
	    	{
				intUnlock(lockKey);
	        	return;
	    	}
        }

    /* use the physical address for L2 cache operation */

    phyAddr = mmuVirtToPhys((VIRT_ADDR)pMemStart);
    phyAddr &= ~L2_CACHE_LINE_MASK;

    for (tempSize = 0; tempSize < invalidateSize; tempSize += L2_CACHE_LINE_SIZE)
        {
        ARMA9CTX_REGISTER_WRITE(L2_INVALIDATE_PA, (phyAddr + tempSize)); /* invalidate operation */
        }

    /* check the boundary cache line */

    if (((L2_CACHE_LINE_SIZE - ((UINT32)pMemStart % L2_CACHE_LINE_SIZE)) +
        invalidateSize % L2_CACHE_LINE_SIZE) > L2_CACHE_LINE_SIZE)
        {
        ARMA9CTX_REGISTER_WRITE(L2_INVALIDATE_PA, (phyAddr + tempSize)); /* invalidate operation */
        }

    /*
     * There are the following comments in PL310 manual:
     *
     * All writes to registers automatically perform an initial Cache Sync
     * operation before proceeding.
     *
     * So at the end of operation, we perform a "write to register" action to
     * ensure that the operation is complete.
     */

    ARMA9CTX_REGISTER_WRITE(L2_INT_MASK, L2_INT_MASK_ALL);

	cacheSync();
	
	intUnlock(lockKey);

    }

/*******************************************************************************
*
* sysL2Flush - flush L2 Cache
*
* This routine flushes L2 cache ranging from 'pMemStart' to 'pMemEnd'
*
* RETURNS: N/A
*
* ERRNO
*/

LOCAL void sysL2Flush
    (
    CACHE_TYPE cacheType,
    void * pMemStart,
    void * pMemEnd
    )
    {
    int phyAddr, tempSize, flushSize,lockKey;

	lockKey = intLock();

    if ((UINT32)pMemEnd == ENTIRE_CACHE) /* flush the whole L2 cache */
        {
        ARMA9CTX_REGISTER_WRITE(L2_CLEAN_WAY, L2_WAY_ALL);

        /* poll state until the background flush operation  is complete */

        while (ARMA9CTX_REGISTER_READ(L2_CLEAN_WAY) & L2_WAY_ALL);

		cacheSync();
		intUnlock(lockKey);

        return;
        }
    else
        {
        if ((UINT32)pMemEnd > (UINT32)pMemStart) /* check the parameters */
            flushSize = (UINT32)pMemEnd - (UINT32)pMemStart;
        else
	        {      	
	        	intUnlock(lockKey);
	            return;
	        }
        }

    /* use the physical address for L2 cache operation */

    phyAddr = mmuVirtToPhys((VIRT_ADDR)pMemStart);
    phyAddr &= ~L2_CACHE_LINE_MASK;

    for (tempSize = 0; tempSize < flushSize; tempSize += L2_CACHE_LINE_SIZE)
        {
        ARMA9CTX_REGISTER_WRITE(L2_CLEAN_PA, (phyAddr + tempSize)); /* flush operation */
        }

    /* check the boundary cache line */

    if (((L2_CACHE_LINE_SIZE - ((UINT32)pMemStart % L2_CACHE_LINE_SIZE)) +
        flushSize % L2_CACHE_LINE_SIZE) > L2_CACHE_LINE_SIZE)
        {
        ARMA9CTX_REGISTER_WRITE(L2_CLEAN_PA, (phyAddr + tempSize)); /* flush operation */
        }

    /*
     * There are the following comments in PL310 manual:
     *
     * All writes to registers automatically perform an initial Cache Sync
     * operation before proceeding.
     *
     * So at the end of operation, we perform a "write to register" action to
     * ensure that the operation is completed.
     */

    ARMA9CTX_REGISTER_WRITE(L2_INT_MASK, L2_INT_MASK_ALL);

	cacheSync();

	intUnlock(lockKey);

    }

/*******************************************************************************
*
* sysL2Clear - clear L2 Cache
*
* This routine clears L2 cache ranging from 'pMemStart' to 'pMemEnd'
*
* NOTE Clear operation is flush + invalidate
*
* RETURNS: N/A
*
* ERRNO
*/

LOCAL void sysL2Clear
    (
    CACHE_TYPE cacheType,
    void * pMemStart,
    void * pMemEnd
    )
    {
    int phyAddr, tempSize, clearSize,lockKey;

	lockKey = intLock();

    if ((UINT32)pMemEnd == ENTIRE_CACHE) /* clear the whole L2 cache */
        {
        ARMA9CTX_REGISTER_WRITE(L2_CLEAN_INVALIDATE_WAY, L2_WAY_ALL);

        /* poll state until the background clear operation  is complete */

        while (ARMA9CTX_REGISTER_READ(L2_CLEAN_INVALIDATE_WAY) & L2_WAY_ALL);

		/* cache sync */
		cacheSync();

		intUnlock(lockKey);

        return;
        }
    else
        {
        if ((UINT32)pMemEnd > (UINT32)pMemStart) /* check the parameters */
            clearSize = (UINT32)pMemEnd - (UINT32)pMemStart;
        else
	        {
	        	intUnlock(lockKey);
	            return;
	        }
        }

    /* use the physical address for L2 cache operation */

    phyAddr = mmuVirtToPhys((VIRT_ADDR)pMemStart);
    phyAddr &= ~L2_CACHE_LINE_MASK;

    for (tempSize = 0; tempSize < clearSize; tempSize += L2_CACHE_LINE_SIZE)
        {
        ARMA9CTX_REGISTER_WRITE(L2_CLEAN_INVALIDATE_PA, (phyAddr + tempSize)); /* clear operation */
        }

    /* check the boundary cache line */

    if (((L2_CACHE_LINE_SIZE - ((UINT32)pMemStart % L2_CACHE_LINE_SIZE)) +
        clearSize % L2_CACHE_LINE_SIZE) > L2_CACHE_LINE_SIZE)
        {
        ARMA9CTX_REGISTER_WRITE(L2_CLEAN_INVALIDATE_PA, (phyAddr + tempSize)); /* clear operation */
        }

    /*
     * There are the following comments in PL310 manual:
     *
     * All writes to registers automatically perform an initial Cache Sync
     * operation before proceeding.
     *
     * So at the end of operation, we perform a "write to register" action to
     * ensure that the operation is complete.
     */

    ARMA9CTX_REGISTER_WRITE(L2_INT_MASK, L2_INT_MASK_ALL);

	cacheSync();
	intUnlock(lockKey);

    }


#define BIT_A9_FULL_LINE 3
#define BIT_A9_L1_PREFETCH 2
#define BIT_A9_L2_PREFETCH_HINTS 1

static void  set_a9_aux_ctrl( unsigned int bit , unsigned int bool)
{
/* c1 - 0 - c0 - 1 */
    unsigned int reg = 0;
    __asm__ __volatile__  ("mrc p15, 0, %0, c1, c0, 1" : "=r" (reg) );

    if(bool==1)
        { reg = reg|(0x1<<bit);}
    else
        { reg = reg &(~(0x1<<bit)) ;}
    __asm__ __volatile__  ("mcr p15, 0, %0, c1, c0, 1" : : "r" (reg));
    
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory"); 

}

#define  L2CACHE_OPT 1

#ifdef L2CACHE_OPT
#define  L2CACHE_WITH_L1 1
#endif

#define SCU_CTRL_OFFSET 0x0
#define SCU_IC_STANDBY      (0x1<<6)
#define SCU_STANDBY     (0x1<<5)
#define SCU_SPECUL_LINEFILL (0x1<<3)
#define SCU_SCU_ENABLE      (0x1<<0)

#define SCU_CTRL_CONFIG (SCU_IC_STANDBY|SCU_STANDBY|SCU_SPECUL_LINEFILL|SCU_SCU_ENABLE)

LOCAL void scu_enable(void)
{
    unsigned int scu_reg_base = 0;
    unsigned int reg = 0;

    __asm__ __volatile__  ("mrc p15, 4, %0, c15, c0, 0" : "=r" (scu_reg_base) );
    scu_reg_base &= 0xffffe000; /* get scu reg base - bit[31:13]*/

    reg = ARMA9CTX_REGISTER_READ(scu_reg_base+SCU_CTRL_OFFSET);
    reg= reg|SCU_CTRL_CONFIG;
    ARMA9CTX_REGISTER_WRITE(scu_reg_base+SCU_CTRL_OFFSET, reg);
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory"); 
}


/*******************************************************************************
*
* sysL2Enable - enable L2 Cache
*
* This routine enables the external L2 cache connected to Cortex A9 processor
*
* RETURNS: N/A
*
* ERRNO
*/

LOCAL void sysL2Enable
    (
    CACHE_TYPE cacheType
    )
    {
    int  lockKey;
    lockKey = intLock();
#ifdef L2CACHE_OPT
    unsigned int reg = 0;
    /* l2cache config for optimize*/
    reg = ARMA9CTX_REGISTER_READ(L2_AUX_CONTROL);
    reg = reg|AUX_INSTR_PREFETCH|AUX_DATA_PREFETCH|AUX_EARLY_BRESP;
#ifdef L2CACHE_WITH_L1
    reg = reg|AUX_FULL_LINE_OF_ZERO; /*AUX_FULL_LINE_OF_ZERO*/
#endif
    ARMA9CTX_REGISTER_WRITE(L2_AUX_CONTROL, reg);
    
    reg = ARMA9CTX_REGISTER_READ(L2_PREFETCH_CTRL);
    reg = reg|PREF_DOUBLE_LINEFILL|PREF_INSTR_PREFETCH|PREF_DATA_PREFETCH|PREF_PREFETCH_OFFSET|PREF_PREFETCH_DROP;
    ARMA9CTX_REGISTER_WRITE(L2_PREFETCH_CTRL, reg);

    reg = ARMA9CTX_REGISTER_READ(L2_POWER_CTRL);
    reg = reg|POWER_CLK_GATING|POWER_STANDBY_MODE;
    ARMA9CTX_REGISTER_WRITE(L2_POWER_CTRL, reg);

    scu_enable();

#endif
    ARMA9CTX_REGISTER_WRITE(L2_CONTROL, L2_CONTROL_ENABLE); /* enable L2 cache */
	__asm__ __volatile__ ("dsb" : : : "memory");

#ifdef L2CACHE_WITH_L1
    /* after l2 full line zero enabled */
    set_a9_aux_ctrl(BIT_A9_FULL_LINE, 1); /*AUX_FULL_LINE_OF_ZERO*/

    set_a9_aux_ctrl(BIT_A9_L2_PREFETCH_HINTS, 1); /*  */
    set_a9_aux_ctrl(BIT_A9_L1_PREFETCH, 1); /* config anywhere */
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory"); 

#endif

    cache_sync_ops = l2_cache_sync;
    intUnlock(lockKey);
    }

/*******************************************************************************
*
* sysL2Enable - disable L2 Cache
*
* This routine disables the external L2 cache connected to Cortex A9 processor
*
* RETURNS: N/A
*
* ERRNO
*/

LOCAL void sysL2Disable
    (
    CACHE_TYPE cacheType
    )
    {
    int  lockKey;
    lockKey = intLock();
#ifdef L2CACHE_WITH_L1
    set_a9_aux_ctrl(BIT_A9_FULL_LINE, 0); /*AUX_FULL_LINE_OF_ZERO*/
    set_a9_aux_ctrl(BIT_A9_L2_PREFETCH_HINTS, 1); /*  */
#endif
    ARMA9CTX_REGISTER_WRITE(L2_CONTROL, 0x0); /* disable L2 cache */
	__asm__ __volatile__ ("dsb" : : : "memory");

    cache_sync_ops = NULL;
	intUnlock(lockKey);

    }

/*******************************************************************************
*
* sysL2CacheInit - init L2 Cache on Cortex A9
*
* This routine initializes L2 cache support for Cortex A9
*
* RETURNS: N/A
*
* ERRNO
*/
LOCAL void sysL2CacheInit(void)
    {

    /*
     * 512KB L2 cache RAM. 8-way associativity. way-size is 64KB
     */
    int  lockKey;
    lockKey = intLock();

    ARMA9CTX_REGISTER_WRITE(L2_CONTROL, 0x0); /* disable L2 cache */

    /*
     * We use the default configuration, so here we do not need use the
     * auxiliary control register to configure associativity, way size and
     * latency of RAM access.
     */

    /* invalidate the whole L2 cache through way operation  */

    ARMA9CTX_REGISTER_WRITE(L2_INVALIDATE_WAY, L2_WAY_ALL);

    /* poll state until the background invalidate operation  is complete */

    while (ARMA9CTX_REGISTER_READ(L2_INVALIDATE_WAY) & L2_WAY_ALL);

	cacheSync();

    /* clear all pending interrupts */

    ARMA9CTX_REGISTER_WRITE(L2_INT_CLEAR, L2_INT_CLEAR_ALL);

    /* mask all interrupts */

    ARMA9CTX_REGISTER_WRITE(L2_INT_MASK, L2_INT_MASK_ALL);
    _pSysL2CacheInvFunc  = sysL2Invalidate;
    _pSysL2CacheEnable   = sysL2Enable;
    _pSysL2CacheDisable  = sysL2Disable;
    _pSysL2CacheFlush    = sysL2Flush;
    _pSysL2CacheClear    = sysL2Clear;
	l2cache_init_flag = 1;
    cache_sync_ops = NULL;
	intUnlock(lockKey);
    }


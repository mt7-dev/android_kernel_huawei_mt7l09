/* romStart.c - ROM initialization module */

/*
 * Copyright (c) 1989-2005, 2008-2009, 2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */



/*
DESCRIPTION
This module provides a generic ROM startup facility.  The target-specific
romInit.s module performs the minimal preliminary board initialization and
then jumps to the C routine romStart(). It is assumed that the romInit.s
module has already set the initial stack pointer to STACK_ADRS.
This module optionally clears memory to avoid parity errors, initializes
the RAM data area, and then jumps to the routine "relocEntry".
This module is a replacement for config/all/bootInit.c.

CLEARING MEMORY
One feature of this module is to optionally clear memory to avoid parity
errors.
Generally, we clear for the bottom of memory (the BSP's LOCAL_MEM_LOCAL_ADRS
macro), to the top of memory (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE),
but avoiding the stack area. In order to avoid trashing the stack area,
the BSP's romInit.s must have set the stack pointer to the macro STACK_ADRS.
There are three configuration macros which can be overridden in the
BSP's config.h file to change how memory is cleared:
ROMSTART_BOOT_CLEAR undefine to not clear memory when booting
USER_RESERVED_MEM   area at the top of memory to not clear
RESERVED        area at the bottom of memory to not clear
.bS 22
    RAM
    --------------  0x00100000 = LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE
    |            |
    |------------|      = " - USER_RESERVED_MEM
    |            |
    |  0 filled  |
    |            |
    |------------|  0x00001000  = RAM_DATA_ADRS
    | initial sp |
    |------------|      = RAM_DATA_ADRS - STACK_SAVE
    |            |
    | 0 filled   |
    |            |  exc vectors, bp anchor, exc msg, bootline
    |            |
    |------------|      = LOCAL_MEM_LOCAL_ADRS + RESERVED
    |            |
    --------------  0x00000000  = LOCAL_MEM_LOCAL_ADRS
.bE
Note: The STACK_ADRS macro defaults to either RAM_DATA_ADRS, or
RAM_DATA_ADRS - STACK_SAVE, depending on if the stack grows up
or down for the given processor.
Note: The RAM_DATA_ADRS macro is passed to us from the Make system.
It represents the address to which the data segment is linked
and defaults to the BSP Makefile's RAM_LOW_ADRS macro. For details on
how to change the addresses to which VxWorks is linked, refer to the
documentation in h/make/defs.vxWorks.

INITIALIZING DATA
The other main feture of this module is to initialize the RAM data
area.
The way this is done depends on the kind of image you are building.
There are three ROM image types that are supported by the Make
system:
  ROM_RESIDENT  run from ROM, with only the data segment placed in RAM.
  ROM_COPY  copy the main image from ROM to RAM and jump to it.
  ROM_COMPRESS  uncompress the main image from ROM to RAM and jump to it.
The make system will define one of these macros for you, depending on the
name of the target you are building (%.rom_res, %.rom_cpy, or %.rom_cmp).
There are separate version of romStart for each of these cases.
Let's start with the simplest case; ROM_RESIDENT.

ROM_RESIDENT IMAGES
When the board is powered on, the processor starts executing instructions
from ROM. The ROM contains some text (code) and a copy of the data
segment. The pictured below is a typical example:
.bS 6
    ROM
    --------------
    |            |
    |------------|
    |    data    |
    |------------|  0xff8xxxxx  = ROM_DATA_ADRS
    |    text    |
    |            |  0xff800008  = ROM_TEXT_ADRS
    --------------  0xff800000  = ROM_BASE_ADRS
.bE
romStart() copies the data segment from ROM_DATA_ADRS to RAM_DATA_ADRS -
the address to which it is really linked.
The picture below is an example of RAM addresses on a 1 meg board.
.bS 22
    RAM
    --------------  0x00100000 = LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE
    |            |
    |            |
    |------------|      = RAM_DATA_ADRS + data segment size
    |            |
    |data segment|
    |            |
    |------------|  0x00001000  = RAM_DATA_ADRS
    | initial sp |
    |------------|      = RAM_DATA_ADRS - STACK_SAVE
    |            |
    |            |
    --------------  0x00000000  = LOCAL_MEM_LOCAL_ADRS
.bE

ROM_COPY and ROM_COMPRESS IMAGES
These images are slightly more complex than ROM_RESIDENT images.
The Make system produces an intermediate image, called %.ram_reloc,
which runs from RAM. It then embeds the binary for this subimage within
a ROM image. The subimage is located between the global symbols
"binArrayStart" and "binArrayEnd". The main difference between
the ROM_COPY and the ROM_COMPRESS images is how the subimage is
stored.
For ROM_COMPRESS, the subimage is compressed to save ROM space.
A modified version of the Public Domain \f3zlib\fP library is used to
uncompress the VxWorks boot ROM executable linked with it.  Compressing
object code typically achieves over 55% compression, permitting much
larger systems to be burned into ROM.  The only expense is the added few
seconds delay while the first two stages complete.
For ROM_COPY, the subimage is not compressed to make the
startup sequence shorter.
In both cases, the ROM image initially looks like this, with
binArrayStart and binArrayEnd at the end of the ROM data area.
The pictured below is a typical example:
.bS 6
    ROM
    --------------
    |            |
    |------------|  0xff8xxxxx  = binArrayEnd
    |  subImage  |
    |------------|  0xff8xxxxx  = binArrayStart
    |    data    |
    |------------|  0xff8xxxxx  = ROM_DATA_ADRS
    |    text    |
    |            |  0xff800008  = ROM_TEXT_ADRS
    --------------  0xff800000  = ROM_BASE_ADRS
.bE
For ROM_COPY images, romStart() copies the subimage from binArrayStart to
RAM_DST_ADRS
For ROM_UNCOMPRESS images, romStart() first copies the uncompression
code's data segment to RAM_DATA_ADRS, and then runs the uncompression
routine to place the subimage at RAM_DST_ADRS.
Both the RAM_DST_ADRS and RAM_DATA_ADRS macros are link addressed
defined by the Make system, which default to the BSP Makefile's
RAM_LOW_ADRS and RAM_HIGH_ADRS respectively.
For information on how to change link addresses, refer to the file
target/h/make/rules.bsp.
The picture below is an example of RAM addresses on a 1 meg board.
"temp data" below refers to the uncompression code's data segment.
.bS 22
        RAM
    --------------  0x00100000 = LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE
    |            |
    |            |
    |------------|
    | temp data  |
    |------------|  0x00090000  = RAM_DATA_ADRS
    | initial sp |
    |------------|              = RAM_DATA_ADRS - STACK_SAVE
    |            |
    |            |
    |------------|
    |            |
    |  subimage  |
    |            |
    |------------|  0x00001000  = RAM_DST_ADRS (for non-resident images)
    |            |
    |            |
    --------------  0x00000000  = LOCAL_MEM_LOCAL_ADRS
.bE

SEE ALSO:
inflate(), romInit(), and deflate.

AUTHOR
The original compression software was written by Jean-loup Gailly
and Mark Adler. See the manual pages of inflate and deflate for
more information on their freely available compression software.

SUMMARY OF CONFIGURATION MACROS
Macro       defined     synopsys
-----       -------     --------
ROM_BASE_ADRS   config.h    Start of ROM
ROM_TEXT_ADRS   Makefile    Start of text segment within ROM, typically
                a CPU-dependant offset from ROM_BASE_ADRS.
ROM_DATA_ADRS   Here        Location in ROM containing a copy of the
                data segment. This must eventually be copied
                to the RAM address to which it is linked.
RAM_DATA_ADRS   rules.bsp   Location in RAM to which the data segment is
                really linked.
STACK_ADRS  configAll.h Initial stack pointer.
ROMSTART_BOOT_CLEAR configAll.h Undefine to not clear memory when booting
USER_RESERVED_MEM configAll.h   Area at the top of memory to not clear
RESERVED    configAll.h Area at the bottom of memory to not clear
LOCAL_MEM_LOCAL_ADRS config.h
LOCAL_MEM_SIZE  config.h

CHANGES from bootInit.c:
Three separate versions of romStart to make it readable.
One each for ROM_COMPRESS, ROM_RESIDENT, and ROM_COPY images.
* The ROM_COPY image copies the main image from binArrayStart to
  RAM_DST_ADRS and then jumps to it.
  The array binArrayStart and the macro RAM_DST_ADRS are provided by the
  Make system.
* The ROM_COMPRESS image uncompresses the main image from binArrayStart to
  RAM_DST_ADRS and then jumps to it.
  Before the uncompression can run, this image needs to initialize the
  uncompression code's data segment. It does this by copying data from
  ROM_DATA to RAM_DATA_ADRS.
  The array binArrayStart and the macros RAM_DST_ADRS and RAM_DATA_ADRS
  are provided by the Make system. The macro ROM_DATA is a toolchain
  dependant macro, which defaults to the global symbol "etext".
* The ROM_RESIDENT image copys its data segment from ROM to RAM. As with
  the ROM_COMPRESS image, it does this by copying from ROM_DATA to
  RAM_DATA_ADRS.
*/

#include "vxWorks.h"
#include "sysLib.h"
#include "config.h"
#include "usrConfig.h"
#include "modem_l2_test_case.h"

/*
 * BSP_BOOT_CACHE_SYNC is an optionally defined macro that a BSP
 * can use to provide a cache synchronization routine for data
 * cache flush and instruction cache invalidate after text space
 * has been copied/extracted from flash to memory (cacheTextUpdate).
 *
 * The BSP provided function has no arguments as it will
 * flush/invalidate all data/instruction cache.
 *
 * .eg:
 * #define BSP_BOOT_CACHE_SYNC sysBspBootCacheSync()
 */

#ifdef BSP_BOOT_CACHE_SYNC
void BSP_BOOT_CACHE_SYNC;
#endif /* BSP_BOOT_CACHE_SYNC */

/* default uncompression routine for compressed ROM images */

#ifndef UNCMP_RTN
#   define  UNCMP_RTN   inflate
#endif

/* toolchain dependant location of the data segment within ROM */

#ifndef ROM_DATA_ADRS
#   define  ROM_DATA_ADRS   MCORE_TEXT_START_ADDR
#endif

/* MIPS needs to write to uncached memory, others just do normal writes */

#ifndef UNCACHED
#    define UNCACHED(adr)   adr
#endif

/* USER_RESERVED_MEM must be set to 0 if not defined */
#ifndef USER_RESERVED_MEM
#    define USER_RESERVED_MEM 0
#endif

/* handy macros */

#define ROM_DATA(adrs) ((UINT)adrs + ((UINT)ROM_DATA_ADRS - RAM_DATA_ADRS))

#ifdef  INCLUDE_EDR_PM
#    define SYS_MEM_TOP \
        (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE - USER_RESERVED_MEM \
         - PM_RESERVED_MEM)
#else
#    define SYS_MEM_TOP \
        (LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE - USER_RESERVED_MEM)
#endif

#define SYS_MEM_BOTTOM \
    (LOCAL_MEM_LOCAL_ADRS + RESERVED)

#define BINARRAYEND_ROUNDOFF    (ROUND_DOWN(binArrayEnd, sizeof(long)))

#if defined (ROMSTART_BOOT_CLEAR) && !defined (ROM_RESIDENT)
# define LD_IMAGE_END  ((UINT)RAM_DST_ADRS + \
               (UINT)(ROUND_UP(binArrayEnd - binArrayStart, sizeof(long))))
#endif /* defined (ROMSTART_BOOT_CLEAR) && !defined (ROM_RESIDENT) */

/* forward declarations */

/*
 * BSP_BOOT_CACHE_SYNC_POST is an optional defined macro that a BSP
 * can use. BSP_BOOT_CACHE_SYNC_POST is used before the final jump of
 * romStart(). BSP developer can add any codes in this definition. For example,
 * if MMU/cache is enabled in romInit.s, and the developer does not want the
 * routines after romStart() to be effected by romInit.s, then operation to
 * disable MMU/cache can be implemented by BSP in BSP_BOOT_CACHE_SYNC_POST.
 *
 * The BSP provided function has no arguments.
 *
 * .eg:
 * #define BSP_BOOT_CACHE_SYNC_POST sysBspBootCacheSyncPost()
 */

#ifdef BSP_BOOT_CACHE_SYNC_POST
void BSP_BOOT_CACHE_SYNC_POST;
#endif /* BSP_BOOT_CACHE_SYNC_POST */

/*
 * copyLongs() is only used for some image types.
 * This prevents compiler warnings.
 */
#if (defined(ROM_COMPRESS) || defined(ROM_COPY) || defined(ROM_RESIDENT))
LOCAL void copyLongs (FAST UINT *source, FAST UINT *destination, UINT nlongs);
#endif /* ROM_xxx */

#ifdef  ROMSTART_BOOT_CLEAR
LOCAL void fillLongs (FAST UINT *buf, UINT nlongs, FAST UINT val);
LOCAL void bootClear (void);
#endif

/* imports */

IMPORT STATUS   UNCMP_RTN ();
IMPORT void     relocEntry ();
IMPORT UCHAR    binArrayStart [];       /* compressed binary image */
IMPORT UCHAR    binArrayEnd [];     /* end of compressed binary image */
IMPORT char     etext [];               /* defined by the loader */
IMPORT char     end [];                 /* defined by the loader */
IMPORT char     wrs_kernel_data_end []; /* defined by the loader */

#ifdef INCLUDE_UEFI_BOOT_SUPPORT
#include <private/efi/uefiVxWorksTypes.h>  /* UEFI types in VxWorksish form */
#include <private/efi/efiVxWorks.h> /* UEFI definitions */
IMPORT VxWorksOSLoaderMemoryMapHeader *pRomUefiMemAddr;
IMPORT UINT32 *pRomUefiAcpiAddr;
#endif


#if (CPU_FAMILY == PPC) || (CPU_FAMILY == MIPS)
IMPORT void vxSdaInit (void);
#endif	/* (CPU_FAMILY == PPC) || (CPU_FAMILY == MIPS) */

#ifdef  ROM_COMPRESS
#include <osl_bio.h>
#include <bsp_shared_ddr.h>
void rom_mmu_record_time(void)
{
    unsigned int value = 0;
    static unsigned int address = SHM_MEM_HIFI_ADDR;
#ifdef BSP_CONFIG_HI3630
    value = readl(0xFFF0A000 + 0x534);
#else
    value = readl(0x90000000 + 0x640);
#endif

    writel(value, address);
    address += 4;
}

void rom_mmu_enable(void);
void rom_mmu_disable(void);
void rom_mmu_l2cache_enable(void);
void rom_mmu_l2cache_disable(void);
void rom_mmu_l2cache_init(void);
void rom_mmu_l2cache_flush(void);
void rom_mmu_cache_clean(void);

/*******************************************************************************
*
* romStart - generic ROM initialization for compressed images
*
* This is the first C code executed after reset.
*
* This routine is called by the assembly start-up code in romInit().
* It clears memory, copies ROM to RAM, and invokes the uncompressor.
* It then jumps to the entry point of the uncompressed object code.
*
* RETURNS: N/A
*/

#if 0
#define MODEM_L2CACHE_TEST
#endif

#define RAM_DST_ADRS (MCORE_TEXT_START_ADDR)

#ifdef ATE_L2CACHE_TEST
void ate_l2cache_test_case(void)
{
    unsigned int test_count = 0;

    for(test_count = 0; test_count < 5; test_count++)
    {
        memcpy((void*)(ATE_l2_TEST_LOW_ADDR + ATE_L2_TEST_BUFFER_SIZE * test_count),
                (void*)MCORE_TEXT_START_ADDR_COMPRESSED,
                       ATE_L2_TEST_BUFFER_SIZE);

        if (memcmp( (void*)(ATE_l2_TEST_LOW_ADDR + ATE_L2_TEST_BUFFER_SIZE * test_count),
                     (void*)MCORE_TEXT_START_ADDR_COMPRESSED,
                      ATE_L2_TEST_BUFFER_SIZE))
        {
            while (1)
            {
                writel(0xFFFFFFFF, ATE_L2_TEST_FLAG);
                rom_mmu_cache_clean();
                rom_mmu_l2cache_flush();
            }
        }
        memcpy((void*)(ATE_L2_TEST_HIGH_ADDR - test_count * ATE_L2_TEST_BUFFER_SIZE),
               (void*)(ATE_l2_TEST_LOW_ADDR + ATE_L2_TEST_BUFFER_SIZE * test_count),
                        ATE_L2_TEST_BUFFER_SIZE);
        if (memcmp((void*)(ATE_L2_TEST_HIGH_ADDR - test_count * ATE_L2_TEST_BUFFER_SIZE),
                    (void*)(ATE_l2_TEST_LOW_ADDR + ATE_L2_TEST_BUFFER_SIZE * test_count),
                            ATE_L2_TEST_BUFFER_SIZE))
        {
            while (1)
            {
                writel(0xFFFFFFFF, ATE_L2_TEST_FLAG);
                rom_mmu_cache_clean();
                rom_mmu_l2cache_flush();
            }
        }

    }
    for(test_count = 5; test_count < 10; test_count++)
    {
        memset((void*)(ATE_l2_TEST_LOW_ADDR + ATE_L2_TEST_BUFFER_SIZE * test_count),
                       0x5a5a5a5a,
                       ATE_L2_TEST_BUFFER_SIZE);
        memcpy((void*)(ATE_L2_TEST_HIGH_ADDR - test_count * ATE_L2_TEST_BUFFER_SIZE),
               (void*)(ATE_l2_TEST_LOW_ADDR + ATE_L2_TEST_BUFFER_SIZE * test_count),
                        ATE_L2_TEST_BUFFER_SIZE);
        if (memcmp((void*)(ATE_L2_TEST_HIGH_ADDR - test_count * ATE_L2_TEST_BUFFER_SIZE),
                    (void*)(ATE_l2_TEST_LOW_ADDR + ATE_L2_TEST_BUFFER_SIZE * test_count),
                            ATE_L2_TEST_BUFFER_SIZE))
        {
            while (1)
            {
                writel(0xFFFFFFFF, ATE_L2_TEST_FLAG);
                rom_mmu_cache_clean();
                rom_mmu_l2cache_flush();
            }
        }
    }
    while (1)
    {
        writel(0xA5A5A5A5, ATE_L2_TEST_FLAG);
        rom_mmu_cache_clean();
        rom_mmu_l2cache_flush();
    }
}
#endif
void romStart
    (
    FAST int startType      /* start type */
    )
    {
    volatile FUNCPTR absEntry;

#ifdef MODEM_L2CACHE_TEST
    /* A9 -- Modem A9 */
    writel(0xA9A9A9A9, SHM_MEM_LOADM_ADDR);
#endif

#if (CPU_FAMILY == PPC) || (CPU_FAMILY == MIPS)

    /*
     * For PPC and MIPS, the call to vxSdaInit() must be the first operation
     * in sysStart(). This is because vxSdaInit() sets the SDA registers
     * (r2 and r13 on PPC, gp on MIPS) to the SDA base values. No C code
     * must be placed before this call.
     */

    _WRS_ASM ("");   /* code barrier to prevent compiler moving vxSdaInit() */
    vxSdaInit ();    /* this MUST be the first operation in usrInit() for PPC */
    _WRS_ASM ("");   /* code barrier to prevent compiler moving vxSdaInit() */

#endif	/* (CPU_FAMILY == PPC) || (CPU_FAMILY == MIPS) */

    absEntry = (volatile FUNCPTR) RAM_DST_ADRS;

    /* relocate the data segment of the decompression stub */
#if 0
    copyLongs ((UINT *)ROM_DATA_ADRS, (UINT *)UNCACHED(RAM_DST_ADRS),
              ((UINT)binArrayStart - (UINT)DDR_MCORE_ADDR) / sizeof (long));

    copyLongs ((UINT *)((UINT)ROM_DATA_ADRS + ((UINT)BINARRAYEND_ROUNDOFF -
        (UINT)RAM_DATA_ADRS)), (UINT *)UNCACHED(BINARRAYEND_ROUNDOFF),
    ((UINT)wrs_kernel_data_end - (UINT)binArrayEnd) / sizeof (long));
#endif

#ifdef BSP_BOOT_CACHE_SYNC

    /* Text has been copied from flash, call BSP provided cacheTextUpdate */

    BSP_BOOT_CACHE_SYNC;
#endif /* BSP_BOOT_CACHE_SYNC */

    /* if cold booting, start clearing memory to avoid parity errors */

#ifdef  ROMSTART_BOOT_CLEAR
    if (startType & BOOT_CLEAR)
    /* low memory is cleared up to the stack */
    bootClear();
#endif

    /* rom_mmu_record_time(); */

    rom_mmu_enable();

#ifdef BSP_CONFIG_HI3630
    rom_mmu_l2cache_init();
    rom_mmu_l2cache_enable();
#endif

#ifdef ATE_L2CACHE_TEST
    ate_l2cache_test_case();

#elif defined (MODEM_L2CACHE_TEST)

#define MODEM_L2CACHE_TEST_BUFFER_SIZE (0x100000)

    memcpy( (void*)MCORE_TEXT_START_ADDR,
            (void*)MCORE_TEXT_START_ADDR_COMPRESSED,
            MODEM_L2CACHE_TEST_BUFFER_SIZE);

    if (memcmp( (void*)MCORE_TEXT_START_ADDR,
                (void*)MCORE_TEXT_START_ADDR_COMPRESSED,
                MODEM_L2CACHE_TEST_BUFFER_SIZE))
    {
        while (1)
            writel(0xFFFFFFFF, SHM_MEM_LOADM_ADDR);
    }
    else
    {
        while (1)
            writel(0xA5A5A5A5, SHM_MEM_LOADM_ADDR);
    }

    return; /* return to dead loop */
#else
    /* decompress the main image */
    if (UNCMP_RTN (UNCACHED(binArrayStart),
        UNCACHED(RAM_DST_ADRS),
        binArrayEnd - binArrayStart) != OK)
    return;

#endif

#ifdef BSP_CONFIG_HI3630
    rom_mmu_l2cache_disable();
#endif

    rom_mmu_disable();

    /* rom_mmu_record_time(); */

#ifdef BSP_BOOT_CACHE_SYNC

    /* Text has been copied from flash, call BSP provided cacheTextUpdate */

    BSP_BOOT_CACHE_SYNC;
#endif /* BSP_BOOT_CACHE_SYNC */

    /* if cold booting, finish clearing memory */

#ifdef  ROMSTART_BOOT_CLEAR
    if (startType & BOOT_CLEAR)
    /* clear past the stack to the top of memory */
# if (RAM_DATA_ADRS > SYS_MEM_TOP)
#error Bad size, RAM_DATA_ADRS > SYS_MEM_TOP, check LOCAL_MEM SIZE and RAM_HIGH ADRS settings
# endif	/* RAM_DATA_ADRS > SYS_MEM_TOP */
    fillLongs ((UINT *)UNCACHED(RAM_DATA_ADRS),
        ((UINT)SYS_MEM_TOP - (UINT)RAM_DATA_ADRS) / sizeof(long), 0);
#endif

#ifdef BSP_BOOT_CACHE_SYNC_POST
    BSP_BOOT_CACHE_SYNC_POST;
#endif /* BSP_BOOT_CACHE_SYNC_POST */

#if     ((CPU_FAMILY == ARM) && ARM_THUMB)
     absEntry = (FUNCPTR)((UINT32)absEntry | 1);         /* force Thumb state */
#endif  /* CPU_FAMILY == ARM */

#if (CPU_FAMILY == MIPS)
    /* mapped ROMs have start address in kseg2 so it needs to be modified
     * so we jump to unmapped space
     */
    absEntry = (FUNCPTR)KX_TO_K0(absEntry);
#endif /* (CPU_FAMILY == MIPS) */

/* and jump to the entry */

#ifdef INCLUDE_UEFI_BOOT_SUPPORT
    /* For UEFI we must pass the UEFI memory map and ACPI pointer */
    absEntry (startType, pRomUefiMemAddr, pRomUefiAcpiAddr);
#else
    absEntry (startType);
#endif

    }
#endif  /* ROM_COMPRESS */

#ifdef  ROM_COPY
/*******************************************************************************
*
* romStart - generic ROM initialization for uncompressed ROM images
*
* This is the first C code executed after reset.
*
* This routine is called by the assembly start-up code in romInit().
* It clears memory, copies ROM to RAM, and then jumps to the entry
* point of the copied object code.
*
* RETURNS: N/A
*/

void romStart
    (
    FAST int startType      /* start type */
    )
    {
    volatile FUNCPTR absEntry;

#if (CPU_FAMILY == PPC) || (CPU_FAMILY == MIPS)

    /*
     * For PPC and MIPS, the call to vxSdaInit() must be the first operation
     * in sysStart(). This is because vxSdaInit() sets the SDA registers
     * (r2 and r13 on PPC, gp on MIPS) to the SDA base values. No C code
     * must be placed before this call.
     */

    _WRS_ASM ("");   /* code barrier to prevent compiler moving vxSdaInit() */
    vxSdaInit ();    /* this MUST be the first operation in usrInit() for PPC */
    _WRS_ASM ("");   /* code barrier to prevent compiler moving vxSdaInit() */

#endif	/* (CPU_FAMILY == PPC) || (CPU_FAMILY == MIPS) */

    absEntry = (volatile FUNCPTR) RAM_DST_ADRS;

    /* copy the main image into RAM */

    copyLongs ((UINT *)ROM_DATA(binArrayStart),
        (UINT *)UNCACHED(RAM_DST_ADRS),
        (binArrayEnd - binArrayStart) / sizeof (long));

#ifdef BSP_BOOT_CACHE_SYNC

    /* Text has been copied from flash, call BSP provided cacheTextUpdate */

    BSP_BOOT_CACHE_SYNC;
#endif /* BSP_BOOT_CACHE_SYNC */

#if     ((CPU_FAMILY == ARM) && ARM_THUMB)
     absEntry = (FUNCPTR)((UINT32)absEntry | 1);         /* force Thumb state */
#endif  /* CPU_FAMILY == ARM */

#ifdef BSP_BOOT_CACHE_SYNC

    /* Text has been copied from flash, call BSP provided cacheTextUpdate */

    BSP_BOOT_CACHE_SYNC;
#endif /* BSP_BOOT_CACHE_SYNC */

    /* If cold booting, clear memory to avoid parity errors */

#ifdef ROMSTART_BOOT_CLEAR
    if (startType & BOOT_CLEAR)
        bootClear();
#endif

#ifdef BSP_BOOT_CACHE_SYNC_POST
    BSP_BOOT_CACHE_SYNC_POST;
#endif /* BSP_BOOT_CACHE_SYNC_POST */

#if (CPU_FAMILY == MIPS)
    /* mapped ROMs have start address in kseg2 so it needs to be modified
     * so we jump to uncached space
     */
    absEntry = (FUNCPTR)KX_TO_K0(absEntry);
#endif /* (CPU_FAMILY == MIPS) */

    /* and jump to the entry */


#ifdef INCLUDE_UEFI_BOOT_SUPPORT
    /* For UEFI we must pass the UEFI memory map and ACPI pointer */
    absEntry (startType, pRomUefiMemAddr, pRomUefiAcpiAddr);
#else
    absEntry (startType);
#endif

    }
#endif  /* ROM_COPY */

#ifdef  ROM_RESIDENT
/*******************************************************************************
*
* romStart - generic ROM initialization for ROM resident images
*
* This is the first C code executed after reset.
*
* This routine is called by the assembly start-up code in romInit().
* It clears memory, copies ROM to RAM, and invokes the uncompressor.
* It then jumps to the entry point of the uncompressed object code.
*
* RETURNS: N/A
*/

void romStart
    (
    FAST int startType      /* start type */
    )
    {
#if (CPU_FAMILY == PPC) || (CPU_FAMILY == MIPS)

    /*
     * For PPC and MIPS, the call to vxSdaInit() must be the first operation
     * in sysStart(). This is because vxSdaInit() sets the SDA registers
     * (r2 and r13 on PPC, gp on MIPS) to the SDA base values. No C code
     * must be placed before this call.
     */

    _WRS_ASM ("");   /* code barrier to prevent compiler moving vxSdaInit() */
    vxSdaInit ();    /* this MUST be the first operation in usrInit() for PPC */
    _WRS_ASM ("");   /* code barrier to prevent compiler moving vxSdaInit() */

#endif	/* (CPU_FAMILY == PPC) || (CPU_FAMILY == MIPS) */

    /* relocate the data segment into RAM */

    copyLongs ((UINT *)ROM_DATA_ADRS, (UINT *)UNCACHED(RAM_DATA_ADRS),
        ((UINT)wrs_kernel_data_end - (UINT)RAM_DATA_ADRS) / sizeof (long));

#ifdef BSP_BOOT_CACHE_SYNC

    /* Text has been copied from flash, call BSP provided cacheTextUpdate */

    BSP_BOOT_CACHE_SYNC;
#endif /* BSP_BOOT_CACHE_SYNC */

    /* If cold booting, clear memory to avoid parity errors */

#ifdef ROMSTART_BOOT_CLEAR
    if (startType & BOOT_CLEAR)
        bootClear();
#endif

#ifdef BSP_BOOT_CACHE_SYNC_POST
    BSP_BOOT_CACHE_SYNC_POST;
#endif /* BSP_BOOT_CACHE_SYNC_POST */

    /* and jump to the entry */

#ifdef INCLUDE_UEFI_BOOT_SUPPORT
    /* For UEFI we must pass the UEFI memory map and ACPI pointer */
    usrInit (startType, pRomUefiMemAddr, pRomUefiAcpiAddr);
#else
    usrInit (startType);
#endif

    }
#endif  /* ROM_RESIDENT */

#ifdef  ROMSTART_BOOT_CLEAR
/******************************************************************************
*
* bootClear - clear memory
*
* If cold booting, clear memory not loaded with text & data.
*
* We are careful about initializing all memory (except
* STACK_SAVE bytes) due to parity error generation (on
* some hardware) at a later stage.  This is usually
* caused by read accesses without initialization.
*/

LOCAL void bootClear (void)
    {
#ifdef  ROM_RESIDENT

  /* fill from the bottom of memory to the load image */

# if ((STACK_SAVE+SYS_MEM_BOTTOM) > RAM_DST_ADRS)
#error Bad size, (STACK_SAVE+SYS_MEM_BOTTOM) > RAM_DST_ADRS, check RAM_*_ADRS settings
# endif	/* (STACK_SAVE+SYS_MEM_BOTTOM) > RAM_DST_ADRS */
    fillLongs ((UINT *)SYS_MEM_BOTTOM,
       ((UINT)RAM_DST_ADRS - STACK_SAVE - (UINT)SYS_MEM_BOTTOM) / sizeof (long),
        0);

    /* fill from the load image to the top of memory */

    fillLongs ((UINT *)end, ((UINT)SYS_MEM_TOP - (UINT)end) / sizeof(long), 0);

#else /* ROM_RESIDENT */

    /* fill from the bottom of memory to the load image */

# if (SYS_MEM_BOTTOM > RAM_DST_ADRS)
#error Bad size, (STACK_SAVE+SYS_MEM_BOTTOM) > RAM_DST_ADRS, check RAM_*_ADRS settings
# endif	/* SYS_MEM_BOTTOM > RAM_DST_ADRS */
    fillLongs ((UINT *)UNCACHED(SYS_MEM_BOTTOM),
        ((UINT)RAM_DST_ADRS - (UINT)SYS_MEM_BOTTOM) / sizeof (long), 0);

    /*
    * fill from the end of the load image to the stack
    * (end of a decompressed image isn't known, but this is ok as long as
    *  clearing is done before decompression is performed)
    */
    fillLongs ((UINT *)UNCACHED(LD_IMAGE_END),
        ((UINT)RAM_DATA_ADRS - STACK_SAVE - LD_IMAGE_END) / sizeof (long),
        0);

#  ifndef  ROM_COMPRESS
    /*
     * fill past the stack to the top of memory
     * (this section is cleared later with compressed images)
     */

# if (RAM_DATA_ADRS > SYS_MEM_TOP)
#error Bad size, RAM_DATA_ADRS > SYS_MEM_TOP, check LOCAL_MEM SIZE and RAM_HIGH ADRS settings
# endif	/* RAM_DATA_ADRS > SYS_MEM_TOP */
    fillLongs ((UINT *)UNCACHED(RAM_DATA_ADRS),
	((UINT)SYS_MEM_TOP - (UINT)RAM_DATA_ADRS) / sizeof(long), 0);

#  endif /* ROM_COMPRESS */

#endif /* ROM_RESIDENT */

    /*
     * Ensure the boot line is null. This is necessary for those
     * targets whose boot line is excluded from cleaning.
     */

    *(BOOT_LINE_ADRS) = EOS;
    }

/*******************************************************************************
*
* fillLongs - fill a buffer with a value a long at a time
*
* This routine fills the first <nlongs> longs of the buffer with <val>.
*/

LOCAL void fillLongs
    (
    FAST UINT * buf,    /* pointer to buffer              */
    UINT    nlongs, /* number of longs to fill        */
    FAST UINT   val /* char with which to fill buffer */
    )
    {
    FAST UINT *bufend = buf + nlongs;
    FAST UINT nchunks;

    /* Hop by chunks of longs, for speed. */
    for (nchunks = nlongs / 8; nchunks; --nchunks)
    {
#if (CPU_FAMILY == MC680X0)
    *buf++ = val;   /* 0 */
    *buf++ = val;   /* 1 */
    *buf++ = val;   /* 2 */
    *buf++ = val;   /* 3 */
    *buf++ = val;   /* 4 */
    *buf++ = val;   /* 5 */
    *buf++ = val;   /* 6 */
    *buf++ = val;   /* 7 */
#else
    buf[0] = val;
    buf[1] = val;
    buf[2] = val;
    buf[3] = val;
    buf[4] = val;
    buf[5] = val;
    buf[6] = val;
    buf[7] = val;
    buf += 8;
#endif
    }

    /* Do the remainder one long at a time. */
    while (buf < bufend)
    *buf++ = val;
    }
#endif

/*
 * copyLongs() is only used for some image types.
 * This prevents compiler warnings.
 */
#if (defined(ROM_COMPRESS) || defined(ROM_COPY) || defined(ROM_RESIDENT))
/*******************************************************************************
*
* copyLongs - copy one buffer to another a long at a time
*
* This routine copies the first <nlongs> longs from <source> to <destination>.
*/

LOCAL void copyLongs
    (
    FAST UINT * source,     /* pointer to source buffer      */
    FAST UINT * destination,    /* pointer to destination buffer */
    UINT    nlongs      /* number of longs to copy       */
    )
    {
    FAST UINT *dstend = destination + nlongs;
    FAST UINT nchunks;

    /* Hop by chunks of longs, for speed. */
    for (nchunks = nlongs / 8; nchunks; --nchunks)
    {
#if (CPU_FAMILY == MC680X0)
    *destination++ = *source++; /* 0 */
    *destination++ = *source++; /* 1 */
    *destination++ = *source++; /* 2 */
    *destination++ = *source++; /* 3 */
    *destination++ = *source++; /* 4 */
    *destination++ = *source++; /* 5 */
    *destination++ = *source++; /* 6 */
    *destination++ = *source++; /* 7 */
#else
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
    destination[3] = source[3];
    destination[4] = source[4];
    destination[5] = source[5];
    destination[6] = source[6];
    destination[7] = source[7];
    destination += 8, source += 8;
#endif
    }

    /* Do the remainder one long at a time. */
    while (destination < dstend)
    *destination++ = *source++;
    }
#endif /* ROM_xxx */

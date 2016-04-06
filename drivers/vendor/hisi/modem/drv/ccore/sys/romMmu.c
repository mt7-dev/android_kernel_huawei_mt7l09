
#include <osl_bio.h>

#include <bsp_memmap.h>
#include <product_config.h>
#include "modem_l2_test_case.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MMUCR_M_ENABLE	 (1<<0)  /* MMU enable */
#define MMUCR_A_ENABLE	 (1<<1)  /* Address alignment fault enable */
#define MMUCR_C_ENABLE	 (1<<2)  /* (data) cache enable */
#define MMUCR_W_ENABLE	 (1<<3)  /* write buffer enable */
#define MMUCR_PROG32	 (1<<4)  /* PROG32 */
#define MMUCR_DATA32	 (1<<5)  /* DATA32 */
#define MMUCR_L_ENABLE	 (1<<6)  /* Late abort on earlier CPUs */
#define MMUCR_BIGEND	 (1<<7)  /* Big-endian (=1), little-endian (=0) */
#define MMUCR_SYSTEM	 (1<<8)  /* System bit, modifies MMU protections */
#define MMUCR_ROM	 (1<<9)  /* ROM bit, modifies MMU protections */
#define MMUCR_F		 (1<<10) /* Should Be Zero */
#define MMUCR_Z_ENABLE	 (1<<11) /* Branch prediction enable on 810 */
#define MMUCR_I_ENABLE	 (1<<12) /* Instruction cache enable */
#define MMUCR_V_ENABLE	 (1<<13) /* Exception vectors remap to 0xFFFF0000 */
#define MMUCR_ALTVECT    MMUCR_V_ENABLE /* alternate vector select */
#define MMUCR_RR_ENABLE	 (1<<14) /* Round robin cache replacement enable */
#define MMUCR_ROUND_ROBIN MMUCR_RR_ENABLE  /* round-robin placement */
#define MMUCR_DISABLE_TBIT   (1<<15) /* disable TBIT */
#define MMUCR_ENABLE_DTCM    (1<<16) /* Enable Data TCM */
#define MMUCR_ENABLE_ITCM    (1<<18) /* Enable Instruction TCM */
#define MMUCR_UNALIGNED_ENABLE (1<<22) /* Enable unaligned access */
#define MMUCR_EXTENDED_PAGE (1<<23)  /* Use extended PTE format */
#define MMUCR_VECTORED_INTERRUPT (1<<24) /* Enable VIC Interface */


#define MMU_MUST_SET_VALUE      ( MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
                                 MMUCR_W_ENABLE )
#define MMUCR_MODE MMUCR_ROM

#define MMU_INIT_VALUE ( MMU_MUST_SET_VALUE | MMUCR_MODE)


#define MMU_BUFFERABLE   (1<<2)
#define MMU_BUFFERABLE_NOT   (0<<2)
#define MMU_CACHEABLE  (1<<3)
#define MMU_CACHEABLE_NOT  (0<<3)
#define MMU_WRITE_ALLOC (1<<12)
#define MMU_WRITEABLE ((1<<10)|(0<<11))
#define MMU_WRITEABLE_NOT  ((0<<10)|(0<<11))
#define MMU_SECT_SIZE (0x100000)

#define ROUND_UP(x, align)	(((int) (x) + (align - 1)) & ~(align - 1))

#ifdef BSP_CONFIG_HI3630

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

#define BIT_A9_FULL_LINE 3
#define BIT_A9_L1_PREFETCH 2
#define BIT_A9_L2_PREFETCH_HINTS 1

#define SCU_CTRL_OFFSET 0x0
#define SCU_IC_STANDBY      (0x1<<6)
#define SCU_STANDBY     (0x1<<5)
#define SCU_SPECUL_LINEFILL (0x1<<3)
#define SCU_SCU_ENABLE      (0x1<<0)

#define SCU_CTRL_CONFIG (SCU_IC_STANDBY|SCU_STANDBY|SCU_SPECUL_LINEFILL|SCU_SCU_ENABLE)

#define ARMA9CTX_REGISTER_READ(reg) \
    *(volatile unsigned int *)(reg)

#define ARMA9CTX_REGISTER_WRITE(reg, data)\
    *(volatile unsigned int *)(reg) = (data)


#endif

typedef struct
{
    unsigned int viraddr;
    unsigned int phyaddr;
    unsigned int memsize;
    unsigned int bitconfig;
}MMU_DES_CONFIG;


#define BUFFERABLE_ADDR_START       DDR_MCORE_ADDR
#ifdef ATE_L2CACHE_TEST
const MMU_DES_CONFIG g_mmu_desc_cfg[]=
{
    {
          L2_CTRL_BASE,
          L2_CTRL_BASE,
          ROUND_UP(MMU_SECT_SIZE, MMU_SECT_SIZE),
          (MMU_WRITEABLE)
    },
    {
        ATE_l2_TEST_LOW_ADDR,
        ATE_l2_TEST_LOW_ADDR,
        ROUND_UP (ATE_L2_TEST_SIZE, MMU_SECT_SIZE),
        (MMU_WRITEABLE) | (MMU_CACHEABLE) | (MMU_BUFFERABLE) | (MMU_WRITE_ALLOC)
    },
    {
        HI_MDM_GIC_BASE_ADDR, /* MPCore functions including SCU */
        HI_MDM_GIC_BASE_ADDR,
        ROUND_UP(MMU_SECT_SIZE, MMU_SECT_SIZE),
        (MMU_WRITEABLE)
    },
};
#else
const MMU_DES_CONFIG g_mmu_desc_cfg[]=
{
#ifdef BSP_CONFIG_HI3630
    {
          L2_CTRL_BASE,
          L2_CTRL_BASE,
          ROUND_UP(MMU_SECT_SIZE, MMU_SECT_SIZE),
          (MMU_WRITEABLE)
    },
#endif
    {
        DDR_SHARED_MEM_ADDR,
        DDR_SHARED_MEM_ADDR,
        ROUND_UP(DDR_SHARED_MEM_SIZE, MMU_SECT_SIZE),
        (MMU_WRITEABLE)
    },
    {
        DDR_MCORE_ADDR,
        DDR_MCORE_ADDR,
        ROUND_UP (DDR_MCORE_SIZE, MMU_SECT_SIZE),
        (MMU_WRITEABLE) | (MMU_CACHEABLE) | (MMU_BUFFERABLE) | (MMU_WRITE_ALLOC)
    },
    {
        HI_MDM_GIC_BASE_ADDR, /* MPCore functions including SCU */
        HI_MDM_GIC_BASE_ADDR,
        ROUND_UP(MMU_SECT_SIZE, MMU_SECT_SIZE),
        (MMU_WRITEABLE)
    },
};
#endif

#ifdef BSP_CONFIG_HI3630
/* for compile writel */
void (* cache_sync_ops)(void) = 0;

void rom_mmu_l2cache_sync(void)
{
	ARMA9CTX_REGISTER_WRITE(L2_CACHE_SYNC, 0);
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory");
}

void rom_mmu_l2cache_invalidate(void)
{
    ARMA9CTX_REGISTER_WRITE(L2_INVALIDATE_WAY, L2_WAY_ALL);

    /* poll state until the background invalidate operation is complete */

    while (ARMA9CTX_REGISTER_READ(L2_INVALIDATE_WAY) & L2_WAY_ALL);

    rom_mmu_l2cache_sync();
}

void rom_mmu_l2cache_flush(void)
{
    ARMA9CTX_REGISTER_WRITE(L2_CLEAN_WAY, L2_WAY_ALL);

    /* poll state until the background flush operation  is complete */

    while (ARMA9CTX_REGISTER_READ(L2_CLEAN_WAY) & L2_WAY_ALL);

    rom_mmu_l2cache_sync();
}

void rom_mmu_l2cache_clear(void)
{

    ARMA9CTX_REGISTER_WRITE(L2_CLEAN_INVALIDATE_WAY, L2_WAY_ALL);

    /* poll state until the background clear operation  is complete */

    while (ARMA9CTX_REGISTER_READ(L2_CLEAN_INVALIDATE_WAY) & L2_WAY_ALL);

    /* cache sync */
    rom_mmu_l2cache_sync();
}

void rom_mmu_l2cache_init(void)
{

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

	rom_mmu_l2cache_sync();

    /* clear all pending interrupts */

    ARMA9CTX_REGISTER_WRITE(L2_INT_CLEAR, L2_INT_CLEAR_ALL);

    /* mask all interrupts */

    ARMA9CTX_REGISTER_WRITE(L2_INT_MASK, L2_INT_MASK_ALL);
}
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

void rom_mmu_l2cache_enable(void)
{

    unsigned int reg = 0;
    unsigned int scu_reg_base = 0;

  /* l2cache config for optimize*/
    reg = ARMA9CTX_REGISTER_READ(L2_AUX_CONTROL);
    reg = reg|AUX_INSTR_PREFETCH|AUX_DATA_PREFETCH|AUX_EARLY_BRESP;
    reg = reg|AUX_FULL_LINE_OF_ZERO; /*AUX_FULL_LINE_OF_ZERO*/
    ARMA9CTX_REGISTER_WRITE(L2_AUX_CONTROL, reg);

    reg = ARMA9CTX_REGISTER_READ(L2_PREFETCH_CTRL);
    reg = reg|PREF_DOUBLE_LINEFILL|PREF_INSTR_PREFETCH|PREF_DATA_PREFETCH|PREF_PREFETCH_OFFSET|PREF_PREFETCH_DROP;
    ARMA9CTX_REGISTER_WRITE(L2_PREFETCH_CTRL, reg);

    reg = ARMA9CTX_REGISTER_READ(L2_POWER_CTRL);
    reg = reg|POWER_CLK_GATING|POWER_STANDBY_MODE;
    ARMA9CTX_REGISTER_WRITE(L2_POWER_CTRL, reg);

    /*enable SCU*/
    __asm__ __volatile__  ("mrc p15, 4, %0, c15, c0, 0" : "=r" (scu_reg_base) );
    scu_reg_base &= 0xffffe000; /* get scu reg base - bit[31:13]*/

    reg = ARMA9CTX_REGISTER_READ(scu_reg_base+SCU_CTRL_OFFSET);
    reg= reg|SCU_CTRL_CONFIG;
    ARMA9CTX_REGISTER_WRITE(scu_reg_base+SCU_CTRL_OFFSET, reg);
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory");
    ARMA9CTX_REGISTER_WRITE(L2_CONTROL, L2_CONTROL_ENABLE); /* enable L2 cache */
	__asm__ __volatile__ ("dsb" : : : "memory");

    /* after l2 full line zero enabled */
    set_a9_aux_ctrl(BIT_A9_FULL_LINE, 1); /*AUX_FULL_LINE_OF_ZERO*/

    set_a9_aux_ctrl(BIT_A9_L2_PREFETCH_HINTS, 1); /*  */
    set_a9_aux_ctrl(BIT_A9_L1_PREFETCH, 1); /* config anywhere */
	__asm__ __volatile__ ("dsb" : : : "memory");
	__asm__ __volatile__ ("isb" : : : "memory");

}

void rom_mmu_l2cache_disable(void)
{
    set_a9_aux_ctrl(BIT_A9_FULL_LINE, 0); /*AUX_FULL_LINE_OF_ZERO*/
    set_a9_aux_ctrl(BIT_A9_L2_PREFETCH_HINTS, 1); /*  */

    ARMA9CTX_REGISTER_WRITE(L2_CONTROL, 0x0); /* disable L2 cache */
	__asm__ __volatile__ ("dsb" : : : "memory");
}
#endif

void rom_mmu_cache_invalidate(void)
{
    unsigned int value = 0;

    unsigned int current_way = 0;
    unsigned int current_set = 0;
    unsigned int current_level = 0;

    unsigned int total_way = 0;
    unsigned int total_set = 0;
    unsigned int line_length = 0;

    unsigned int way_offset = 0;
    unsigned int set_offset = 0;

    /* select current cache level in cssr */
    asm volatile("MCR	p15, 2, %0, c0, c0, 0 "::"r"(current_level));
    asm volatile("ISB");
    /* read the new csidr */
    asm volatile("MRC	p15, 1, %0, c0, c0, 0 ":"=r"(value));

    line_length = value & 0x7;
    total_way = (value >> 3) & 0x3FF;
    total_set = (value >> 13) & 0x7FFF;

    asm volatile("CLZ %0, %1":"=r"(way_offset):"r"(total_way):"memory");
    set_offset = line_length + 4;


    for (current_way = 0; current_way <= total_way; current_way++)
    {
        for (current_set = 0; current_set <= total_set; current_set++)
        {
            /*
             * bit[31:32-A] -- Way      A = log2(ASSOCIATIVITY)
             * bit[31-A:B]  -- SBZ      B = L + S
             * bit[B-1:L]   -- Set      L = log2(LINELEN)
             * bit[L-1:4]   -- SBZ      S = log2(number of sets)
             * bit[3:1]     -- Level
             * bit[0:0]     -- 0
             */
            value = 0;
            value |= (current_level << 1);
            value |= (current_set << (set_offset));
            value |= (current_way << way_offset);
            asm volatile("MCR	p15, 0, %0, c7, c6, 2 "::"r"(value));
        }
    }

    /* invalidate entire branch predictor array */
    asm volatile("MCR	p15, 0, r0, c7, c5, 6");
    /* invalidate I cahce */
    asm volatile("MCR   p15, 0, r2, c7, c5, 0");

    /* invalidate the TLB*/
    asm volatile("MOV	r2, #0 ");
    asm volatile("MCR	p15, 0, r2, c8, c7, 0 ");

    /*drain write buffer*/
    asm volatile("MOV	r2, #0 ");
    asm volatile("MCR	p15, 0, r2, c7, c10, 4 ");
    asm volatile("DSB");
    asm volatile("ISB");
}

void rom_mmu_cache_clean(void)
{
    unsigned int value = 0;

    unsigned int current_way = 0;
    unsigned int current_set = 0;
    unsigned int current_level = 0;

    unsigned int total_way = 0;
    unsigned int total_set = 0;
    unsigned int line_length = 0;

    unsigned int way_offset = 0;
    unsigned int set_offset = 0;

    /* select current cache level in cssr */
    asm volatile("MCR	p15, 2, %0, c0, c0, 0 "::"r"(current_level));
    asm volatile("ISB");
    /* read the new csidr */
    asm volatile("MRC	p15, 1, %0, c0, c0, 0 ":"=r"(value));

    line_length = value & 0x7;
    total_way = (value >> 3) & 0x3FF;
    total_set = (value >> 13) & 0x7FFF;

    asm volatile("CLZ %0, %1":"=r"(way_offset):"r"(total_way):"memory");
    set_offset = line_length + 4;


    for (current_way = 0; current_way <= total_way; current_way++)
    {
        for (current_set = 0; current_set <= total_set; current_set++)
        {
            /*
             * bit[31:32-A] -- Way      A = log2(ASSOCIATIVITY)
             * bit[31-A:B]  -- SBZ      B = L + S
             * bit[B-1:L]   -- Set      L = log2(LINELEN)
             * bit[L-1:4]   -- SBZ      S = log2(number of sets)
             * bit[3:1]     -- Level
             * bit[0:0]     -- 0
             */
            value = 0;
            value |= (current_level << 1);
            value |= (current_set << (set_offset));
            value |= (current_way << way_offset);
            asm volatile("MCR	p15, 0, %0, c7, c10, 2 "::"r"(value));
        }
    }
}

void rom_mmu_cache_clear(void)
{
    unsigned int value = 0;

    unsigned int current_way = 0;
    unsigned int current_set = 0;
    unsigned int current_level = 0;

    unsigned int total_way = 0;
    unsigned int total_set = 0;
    unsigned int line_length = 0;

    unsigned int way_offset = 0;
    unsigned int set_offset = 0;

    /* select current cache level in cssr */
    asm volatile("MCR	p15, 2, %0, c0, c0, 0 "::"r"(current_level));
    asm volatile("ISB");
    /* read the new csidr */
    asm volatile("MRC	p15, 1, %0, c0, c0, 0 ":"=r"(value));

    line_length = value & 0x7;
    total_way = (value >> 3) & 0x3FF;
    total_set = (value >> 13) & 0x7FFF;

    asm volatile("CLZ %0, %1":"=r"(way_offset):"r"(total_way):"memory");
    set_offset = line_length + 4;


    for (current_way = 0; current_way <= total_way; current_way++)
    {
        for (current_set = 0; current_set <= total_set; current_set++)
        {
            /*
             * bit[31:32-A] -- Way      A = log2(ASSOCIATIVITY)
             * bit[31-A:B]  -- SBZ      B = L + S
             * bit[B-1:L]   -- Set      L = log2(LINELEN)
             * bit[L-1:4]   -- SBZ      S = log2(number of sets)
             * bit[3:1]     -- Level
             * bit[0:0]     -- 0
             */
            value = 0;
            value |= (current_level << 1);
            value |= (current_set << (set_offset));
            value |= (current_way << way_offset);
            asm volatile("MCR	p15, 0, %0, c7, c14, 2 "::"r"(value));
        }
    }

    /* invalidate entire branch predictor array */
    asm volatile("MCR	p15, 0, r0, c7, c5, 6");
    /* invalidate I cahce */
    asm volatile("MCR   p15, 0, r2, c7, c5, 0");

    /* invalidate the TLB*/
    asm volatile("MOV	r2, #0 ");
    asm volatile("MCR	p15, 0, r2, c8, c7, 0 ");

    /*drain write buffer*/
    asm volatile("MOV	r2, #0 ");
    asm volatile("MCR	p15, 0, r2, c7, c10, 4 ");
    asm volatile("DSB");
    asm volatile("ISB");
}

void rom_mmu_table_set(void)
{
    unsigned int i,j,k;
    unsigned int tablenum;
    unsigned int sectdescriptor,pagedescriptor;
    unsigned int *desaddr;
    unsigned int *pagedesaddr;
    unsigned int secttblsize = 0x4000;
    unsigned int seconddescnum = 256;
    unsigned int pagesize = 0x1000;
    unsigned int ulMmuTlbBaseAddr;
    unsigned int ulMmuSectDesNum = sizeof(g_mmu_desc_cfg)/sizeof(MMU_DES_CONFIG);

    /* tlb addr must be 16KB aligned */
    ulMmuTlbBaseAddr = (unsigned int )BUFFERABLE_ADDR_START;

    /*clear the baseaddr area to 0*/
    desaddr= (unsigned int *)ulMmuTlbBaseAddr;
    for (i = 0; i < ((secttblsize+pagesize)/4); i++) {
        *(desaddr+i)=0;
    }

    for (i = 0; i < ulMmuSectDesNum; i++) {
        tablenum = g_mmu_desc_cfg[i].memsize/MMU_SECT_SIZE;

        for (j = 0; j < tablenum; j++) {

            /*use page mode to protect the area that is used to reserve the sect table*/
            if(0 == ((ulMmuTlbBaseAddr&0xfff00000) -
                    (g_mmu_desc_cfg[i].phyaddr + (j * MMU_SECT_SIZE)) )) {
                desaddr = (unsigned int *)(ulMmuTlbBaseAddr | (((g_mmu_desc_cfg[i].viraddr +
                          (j*MMU_SECT_SIZE))>>18) & 0x00003ffc));
                sectdescriptor=(ulMmuTlbBaseAddr+secttblsize) | 0x11;
                * desaddr=sectdescriptor;

                /*config the pagetable,using small page mode*/
                for (k = 0; k < seconddescnum; k++) {
                    pagedesaddr = (unsigned int *)(ulMmuTlbBaseAddr+secttblsize+(k*4));
                    pagedescriptor = (g_mmu_desc_cfg[i].phyaddr +
                                      (j*MMU_SECT_SIZE) + (k*pagesize)) ;

                    if((0 == ( ulMmuTlbBaseAddr - pagedescriptor)) || \
                        (0 == ((ulMmuTlbBaseAddr + pagesize) - pagedescriptor )) || \
                        (0 == ((ulMmuTlbBaseAddr + (2*pagesize)) - pagedescriptor)) || \
                        (0 == ((ulMmuTlbBaseAddr + (3*pagesize)) - pagedescriptor)) || \
                        (0 == ((ulMmuTlbBaseAddr + (4*pagesize)) - pagedescriptor))) {
                        /*these 5 pages are set to unwriteable */
                        pagedescriptor |= 0x00A;
                    }
                    else {
                        pagedescriptor |= 0x55A;
                    }
                    *pagedesaddr = pagedescriptor;
                }

            }
            else
            {
                desaddr = (unsigned int *)(ulMmuTlbBaseAddr |
                          (((g_mmu_desc_cfg[i].viraddr +
                          (j*MMU_SECT_SIZE))>>18) & 0x00003ffc));
                sectdescriptor = (g_mmu_desc_cfg[i].phyaddr +
                                 (j*MMU_SECT_SIZE)) |
                                 g_mmu_desc_cfg[i].bitconfig | 0x02;
                *desaddr = sectdescriptor;
            }
        }
    }
}

void rom_mmu_table_enable(unsigned int tlb_address)
{
    /*mmuTLBIDFlushAll,clear the TBL*/
    asm volatile("MOV    r2, #0 ");
    asm volatile("MCR    p15, 0, r2, c8, c7, 0 ");
    asm volatile("ISB ");

    /*disable cache*/
    asm volatile("  MRC   p15,  0, r2, c1, c0, 0   ");
    asm volatile("  BIC   r2, r2, #0x1000  ");
    asm volatile("  BIC   r2, r2, #0x05  ");
    asm volatile("  MCR   p15,  0, r2, c1, c0, 0  ");
    asm volatile("  ISB ");

    /*set the descriptor base address to c2*/

    asm volatile("  MCR   p15,  0, r0, c2, c0, 0 ");
    asm volatile("  ISB ");

    /*set the domain bit 0001*/
    asm volatile("  MRC   p15,  0, r2, c3, c0, 0   ");
    asm volatile("  BIC r2,r2, #0x0E ");
    asm volatile("  ORR r2,r2, #0x01 ");
    asm volatile("  MCR p15,0,r2,c3,c0,0 ");
    asm volatile("  ISB ");

    /*enable MMU*/
    asm volatile("  MRC   p15,  0, r2, c1, c0, 0 ");
    asm volatile("  ORR   r2, r2, #0x01  ");
    asm volatile("  MCR   p15,  0, r2, c1, c0, 0 ");
    asm volatile("  ISB ");

    /*enable cache*/
    asm volatile("  MRC   p15,  0, r2, c1, c0, 0 ");
    asm volatile("  ORR   r2, r2, #0x1000  ");
    asm volatile("  ORR   r2, r2, #0x04  ");
    asm volatile("  MCR   p15,  0, r2, c1, c0, 0 ");
    asm volatile("  ISB ");
}

void rom_mmu_enable(void)
{
    rom_mmu_cache_invalidate();
    rom_mmu_table_set();
    rom_mmu_table_enable(BUFFERABLE_ADDR_START);
}

void rom_mmu_disable(void)
{
    rom_mmu_cache_clear();

    asm volatile("MRC	p15, 0, r2, c1, c0, 0 ");/* Read control reg into ARM r2 */
    asm volatile("BIC	r2, r2, #0x1000 ");     /* disable I cache */
    asm volatile("BIC	r2, r2, #0x0005 ");     /* disable MMU and D cache*/
    asm volatile("MCR	p15, 0, r2, c1, c0, 0");
    asm volatile("DSB");
    asm volatile("ISB");
}

#ifdef __cplusplus
}
#endif


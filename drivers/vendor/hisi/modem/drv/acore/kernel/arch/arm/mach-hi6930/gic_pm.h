#ifndef __GIC_DRX_H__
#define __GIC_DRX_H__
#include <bsp_memmap.h>
#ifdef __KERNEL__
/*GIC base addr */
#define GIC_CPU_BASE 	           (HI_APP_GIC_BASE_ADDR_VIRT+0x100)
#define GIC_DIST_BASE              (HI_APP_GIC_BASE_ADDR_VIRT+0x1000)
#elif defined(__VXWORKS__)
/*GIC base addr*/
#define GIC_CPU_BASE               (HI_MDM_GIC_BASE_ADDR_VIRT+0x100)  /*(0x8000100)*/
#define GIC_DIST_BASE              (HI_MDM_GIC_BASE_ADDR_VIRT+0x1000)/*(0x8001000)*/
#endif
#define GIC_REG_BAK_NUM 72

/* GIC */
#define GIC_CPUICR                 (GIC_CPU_BASE + 0x000)
#define GIC_CPUPMR                 (GIC_CPU_BASE + 0x004)
#define GIC_CPUBPR                 (GIC_CPU_BASE + 0x008)
#define GIC_CPUIACK                (GIC_CPU_BASE + 0x00C)
#define GIC_CPUEOIR                (GIC_CPU_BASE + 0x010)
#define GIC_CPURPR                 (GIC_CPU_BASE + 0x014)
#define GIC_CPUHPIR                (GIC_CPU_BASE + 0x018)
#define GIC_ICABPR                 (GIC_CPU_BASE + 0x01C)

#define GIC_ICDDCR                 (GIC_DIST_BASE + 0x000)
#define GIC_ICDISR                 (GIC_DIST_BASE + 0x080)
#define GIC_ICDISER                (GIC_DIST_BASE + 0x100)
#define GIC_ICDICER                (GIC_DIST_BASE + 0x180)
#define GIC_ICDISR1                (GIC_DIST_BASE + 0x200)
#define GIC_ICDISR1_OFFSET(x)      (GIC_ICDISR1  + ((x)*0x4))

#define GIC_ICDICPR                (GIC_DIST_BASE + 0x280)

#define GIC_ICDISPR                (GIC_DIST_BASE + 0x300)
#define GIC_ICDIPR                 (GIC_DIST_BASE + 0x400)
#define GIC_ICDIPR_OFFSET(x)       (GIC_ICDIPR  + ((x)*0x4))

#define GIC_ICDTARG                (GIC_DIST_BASE + 0x800)
#define GIC_ICDICFR                (GIC_DIST_BASE + 0xC00)
#define GIC_ICDSGIR                (GIC_DIST_BASE + 0xF00)
#define GIC_ICDISPR_OFFSET(x)      (GIC_ICDISPR  + ((x)*0x4))
#endif

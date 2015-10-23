
#ifndef __HI_ABB_H__
#define __HI_ABB_H__

#include <osl_bio.h>
#include <bsp_memmap.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ABB_K3V3

static __inline__ void hi_abb_control_enable(int id)
{
    unsigned int hi_value = 0;

    if (1 == id)
    {
        hi_value = readl(HI_AP_SYSCTRL_BASE_ADDR_VIRT + 0x10);

        hi_value |= (1UL << 7);

        writel(hi_value, HI_AP_SYSCTRL_BASE_ADDR_VIRT + 0x10);
    }
}

static __inline__ void hi_abb_control_disable(int id)
{
    unsigned int hi_value = 0;

    if (1 == id)
    {
        hi_value = readl(HI_AP_SYSCTRL_BASE_ADDR_VIRT + 0x10);

        hi_value &= ~(1UL << 7);

        writel(hi_value, HI_AP_SYSCTRL_BASE_ADDR_VIRT + 0x10);
    }
}

static __inline__ int hi_abb_control_status(int id)
{
    unsigned int hi_value = 0;

    hi_value = readl(HI_AP_SYSCTRL_BASE_ADDR_VIRT + 0x10);

    hi_value = (hi_value >> 7) & 0x01;

    return (int)hi_value;
}

static __inline__ int hi_abb_get_buffer_status(int id)
{
    unsigned int hi_value = 0;

    hi_value = readl(HI_AP_SYSCTRL_BASE_ADDR_VIRT + 0x1C);
    hi_value = (hi_value >> 11) & 0x01;

    return (int)hi_value;
}

static __inline__ int hi_abb_get_finish_status(int id)
{
    unsigned int hi_value = 0;

    hi_value = readl(HI_AP_SYSCTRL_BASE_ADDR_VIRT + 0x1C);
    hi_value = (hi_value >> 13) & 0x01;

    return (int)hi_value;
}


#ifdef __cplusplus
}
#endif

#endif

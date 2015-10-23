


#ifndef __BSP_HKADC_H__
#define __BSP_HKADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <product_config.h>

#include <hi_hkadc.h>



/* APB clock, 100 MHz, 10us and 3us */
#define HKADC_DELAY_CYCLES (0x03E8012C)

#define HKADC_INIT_DELAY_CYCLES (0x03E803E8)



#if defined(CONFIG_HKADC)

void bsp_hkadc_init(void);
int bsp_hkadc_convert(enum HKADC_CHANNEL_ID channel, u16* value);

void bsp_hkadc_resume(void);

int bsp_hkadc_convert_test(int channel);

int bsp_hkadc_test(u32 count);

#else

#ifdef BSP_HKADC_K3V3
int bsp_hkadc_convert(enum HKADC_CHANNEL_ID channel, u16* value);
#else
static void bsp_hkadc_init(void)
{
    return;
}

static int bsp_hkadc_convert(enum HKADC_CHANNEL_ID channel, u16* value)
{
    return 0;
}

static void bsp_hkadc_resume(void)
{
    return;
}
#endif


#endif

#ifdef __cplusplus
}
#endif

#endif



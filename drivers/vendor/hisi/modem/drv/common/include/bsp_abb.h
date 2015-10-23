
#ifndef __BSP_ABB_H__
#define __BSP_ABB_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <product_config.h>
#include <osl_types.h>
#include <hi_abb.h>
#include <bsp_dpm.h>



/* only this version is supported */
#define COMSTARV500 (0xA0)
#define COMSTARV510 (0xA4)
#define COMSTARV511 (0xA5)
#define COMSTARV530 (0xAC)
#define COMSTARV532 (0xAE)
#define COMSTARV550 (0xB4)
#define V7R2_ABB    COMSTARV530
#define K3V3_ABB    COMSTARV550

struct abb_config
{
    u32 addr;
    u32 value;
};

enum ABB_BUFFER_ID
{
    ABB_BUFFER_ID_0 = 0,
    ABB_BUFFER_ID_1,
    ABB_BUFFER_ID_MAX,
};

enum ABB_BUFFER_VOTE
{
    ABB_BUFFER_VOTE_G_PLL = 0,
    ABB_BUFFER_VOTE_W_PLL,
    ABB_BUFFER_VOTE_BBP_PLL,
    ABB_BUFFER_VOTE_TCXO,
    ABB_BUFFER_VOTE_MAX,
};


int bsp_abb_read(u32 addr, u32 *value);
int bsp_abb_write(u32 addr, u32 value);

int bsp_abb_buffer_enable(enum ABB_BUFFER_ID id, enum ABB_BUFFER_VOTE vote);
int bsp_abb_buffer_disable(enum ABB_BUFFER_ID id, enum ABB_BUFFER_VOTE vote);
int bsp_abb_buffer_status(enum ABB_BUFFER_ID id, enum ABB_BUFFER_VOTE vote);

void bsp_abb_v500_init_later(void);
void bsp_abb_v510_init_later(void);
void bsp_abb_v511_init_later(void);

void abb_v530_tcxo_sel(void);

void bsp_abb_v530_init_later(void);
void bsp_abb_v550_init_later(void);

extern int bsp_abb_suspend(struct dpm_device *dev);
extern int bsp_abb_resume(struct dpm_device *dev);

extern void bsp_abb_unreset(void);
extern void bsp_abb_init(void);


#ifdef __cplusplus
}
#endif

#endif


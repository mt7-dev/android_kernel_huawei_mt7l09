

/*lint --e{537} */
#include <stdio.h>

#include <osl_types.h>
#include <osl_bio.h>
#include <osl_spinlock.h>

#include <soc_memmap.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>

#include <bsp_hardtimer.h>

#include <bsp_om.h>
#include <bsp_dpm.h>
#include <bsp_abb.h>
#include <bsp_nvim.h>




#ifdef CONFIG_ABB
static u32 g_abb_is_inited = 0;

spinlock_t g_abb_spin_lock;
u32 g_abb_buffer_vote_map[ABB_BUFFER_ID_MAX];

static struct abb_config g_abb_cfg_v500[] =
{
    {0x2D    ,0xAA},
    {0x31    ,0x56},
    {0x39    ,0xFD},
    {0xD9    ,0x20},
    {0x71    ,0x00},
    {0xD1    ,0x00},
    {0x03    ,0x23},
    {0x08    ,0x23},
    {0x0D    ,0x23},
    {0x12    ,0x23},
    {0x14    ,0x23},
    {0x18    ,0x23},
    {0x1D    ,0x23},
    {0x183    ,0x23},
    {0x24    ,0xFE},
    {0x28    ,0xFA},
    {0x2C    ,0xF8},
    {0x30    ,0xFC},
    {0x38    ,0xF8},
    {0x185    ,0xFA},
    {0x7A    ,0x00},
    {0xDA    ,0x03},
    {0x90    ,0x30},
    {0x89    ,0x00},
    {0x72    ,0x04},
    {0x130    ,0x01},
    {0x8C    ,0x10},
    {0x71    ,0x00},
    {0xD1    ,0x08},
};

static struct abb_config g_abb_cfg_v510[] =
{
    {0x2D    ,0xAA},
    {0x31    ,0x56},
    {0x39    ,0xFD},
    {0xD9    ,0x00},
    {0x03    ,0x23},
    {0x08    ,0x23},
    {0x0D    ,0x23},
    {0x12    ,0x23},
    {0x14    ,0x23},
    {0x18    ,0x23},
    {0x1D    ,0x23},
    {0x183    ,0x23},
    {0x24    ,0xFE},
    {0x28    ,0xFA},
    {0x2C    ,0xF8},
    {0x30    ,0xFC},
    {0x38    ,0xF8},
    {0x185    ,0xFA},
    {0x7A    ,0x00},
    {0xDA    ,0x03},
    {0x71    ,0x00},
    {0xD1    ,0x08},
    {0x130    ,0x00},
    {0x129    ,0x00},
    {0x18E    ,0x00},
    {0x105    ,0x00},
    {0x117    ,0x00},
    {0x89    ,0x00},
    {0x72    ,0x0C},
    {0x6B    ,0x04},
    {0x96    ,0x60},
    {0x8D    ,0x07},
    {0x9A    ,0x49},
    {0x0B    ,0x23},
    {0x1B    ,0x23},
    {0x06    ,0x23},
    {0x181    ,0x23},
    {0x01    ,0x33},
    {0x10    ,0x73},
};

static struct abb_config g_abb_cfg_v511[] =
{
    {0x2D    ,0xAA},
    {0x31    ,0x56},
    {0x39    ,0x02},
    {0x3a    ,0xCC},
    {0x1a    ,0x42},
    {0xD9    ,0x60},
    {0x79    ,0x60},
    {0x03    ,0x33},
    {0x08    ,0x33},
    {0x0D    ,0x33},
    {0x12    ,0x33},
    {0x14    ,0x33},
    {0x18    ,0x33},
    {0x1D    ,0x33},
    {0x183    ,0x33},
    {0x24    ,0xFE},
    {0x28    ,0xFA},
    {0x2C    ,0xF8},
    {0x30    ,0xFC},
    {0x185    ,0xFA},
    {0x7A    ,0x00},
    {0xDA    ,0x00},
    {0xCB    ,0x44},
    {0x6B    ,0x04},
    {0xE9    ,0x01},
    {0xEF    ,0x06},
    {0x8F    ,0x0C},
    {0x71    ,0x00},
    {0xD1    ,0x00},
    {0x38    ,0xF8},
    {0x89    ,0x01},
    {0x2A    ,0x5d},
    {0x187    ,0x5d},
};

static struct abb_config g_abb_cfg_v530[] =
{
    {0x2D    ,0xAA},
    {0x31    ,0x56},
    {0x39    ,0xAA},
    {0x71    ,0x20},
    {0xD1    ,0x20},
    {0x3     ,0x23},
    {0x8     ,0x23},
    {0xD     ,0x23},
    {0x12    ,0x23},
    {0x1D    ,0x23},
    {0x183    ,0x23},
    {0x24    ,0xF6},
    {0x28    ,0xF2},
    {0x2C    ,0xF0},
    {0x30    ,0xF4},
    {0x38    ,0xF0},
    {0x185    ,0xF2},
    {0x25    ,0x22},
    {0x103    ,0x08},
    {0x7A    ,0x00},
    {0xDA    ,0x00},
    {0x105    ,0x80},
    {0x117    ,0x82},
    {0x129    ,0x82},
    {0x18E    ,0x82},
    {0x111    ,0x82},
    {0x10B    ,0x82},
    {0x189    ,0x00},
    {0x106    ,0x00},
    {0x112    ,0x00},
    {0x100    ,0x00},
    {0x18A    ,0x0D},
    {0x107    ,0x0D},
    {0x113    ,0x0D},
    {0x101    ,0x0D},
    {0x10C    ,0x00},
    {0x10D    ,0x0D},
    {0x104    ,0x80},
    {0x10A    ,0x80},
    {0x116    ,0x80},
    {0x18D    ,0x80},
    {0x110    ,0x80},
    {0x128    ,0x80},
    {0x12F    ,0x02},
    {0x26    ,0x0F},
    {0x19B    ,0x40},
    {0x195    ,0x40},
    {0x18F    ,0x40},
    {0x1A7    ,0x40},
    {0x1A1    ,0x40},
    {0x1A    ,0x40},
    {0x1C    ,0x40},
    {0x3A    ,0x4C},
    {0x3B    ,0x15},
    {0x124    ,0x00},
    {0x125    ,0x0D},
    {0xF     ,0x26},
    {0x30    ,0xF2},
    {0x31    ,0x02},
    {0x11    ,0x60},
    {0x32    ,0x0D},
    {0x7     ,0x20},
    {0x2A    ,0x0D},
    {0x182    ,0x20},
    {0x187    ,0x0D},
    {0x1E    ,0x00},
};

static struct abb_config g_abb_cfg_v550[] =
{
    {0x2D    ,0xAA},
    {0x31    ,0x56},
    {0x39    ,0xAA},
    {0x71    ,0x20},
    {0xD1    ,0x20},
    {0x3     ,0x23},
    {0x8     ,0x23},
    {0xD     ,0x23},
    {0x12    ,0x23},
    {0x1D    ,0x23},
    {0x183    ,0x23},
    {0x24    ,0xF6},
    {0x28    ,0xF2},
    {0x2C    ,0xF0},
    {0x30    ,0xF4},
    {0x38    ,0xF0},
    {0x185    ,0xF2},
    {0x7A    ,0x00},
    {0xDA    ,0x00},
    {0x25    ,0x22},
    {0x103    ,0x08},
    {0x105    ,0x80},
    {0x117    ,0x82},
    {0x129    ,0x82},
    {0x18E    ,0x82},
    {0x111    ,0x82},
    {0x10B    ,0x82},
    {0x189    ,0x00},
    {0x106    ,0x00},
    {0x112    ,0x00},
    {0x100    ,0x00},
    {0x18A    ,0x0D},
    {0x107    ,0x0D},
    {0x113    ,0x0D},
    {0x101    ,0x0D},
    {0x10C    ,0x00},
    {0x10D    ,0x0D},
    {0x104    ,0x80},
    {0x10A    ,0x80},
    {0x116    ,0x80},
    {0x18D    ,0x80},
    {0x110    ,0x80},
    {0x128    ,0x80},
    {0x12F    ,0x02},
    {0x26    ,0x0F},
    {0x19B    ,0x40},
    {0x195    ,0x40},
    {0x18F    ,0x40},
    {0x1A7    ,0x40},
    {0x1A1    ,0x40},
    {0x1A    ,0x40},
    {0x1C    ,0x40},
    {0x3A    ,0x4C},
    {0x3B    ,0x15},
    {0x124    ,0x00},
    {0x125    ,0x0D},
    {0xF     ,0x26},
    {0x30    ,0xF2},
    {0x31    ,0x02},
    {0x11    ,0x60},
    {0x32    ,0x0D},
    {0x7     ,0x20},
    {0x2A    ,0x0D},
    {0x182    ,0x20},
    {0x187    ,0x0D},
    {0x190    ,0x40},
    {0x196    ,0x40},
    {0x19C    ,0x40},
    {0x1A2    ,0x40},
    {0x1A8    ,0x40},
    {0x1E    ,0x00},
};

/* used for abb suspend */
static u32 g_abb_data_0x16;
static u32 g_abb_data_v530[0x304-0x2C6];


int bsp_abb_read(u32 addr, u32 *value)
{
    *value = readl(HI_ABB_REG_BASE_ADDR + (addr << 2));
    return 0;
}

int bsp_abb_write(u32 addr, u32 value)
{
    writel(value, HI_ABB_REG_BASE_ADDR + (addr << 2));
    return 0;
}

int bsp_abb_buffer_enable(enum ABB_BUFFER_ID id, enum ABB_BUFFER_VOTE vote)
{
#ifdef ABB_K3V3
    unsigned long irq_flags;

    if ((id >= ABB_BUFFER_ID_MAX) || (vote >= ABB_BUFFER_VOTE_MAX))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ABB, "error args, id %d, vote %d\n", id, vote);
        return -1;
    }

    if (ABB_BUFFER_ID_0 == id)
        return 0;

    spin_lock_irqsave(&g_abb_spin_lock, irq_flags);

    if (!g_abb_buffer_vote_map[id])
    {
        if (!hi_abb_control_status(id))
        {
            while (!hi_abb_get_finish_status(id)) ;

            hi_abb_control_enable(id);

            if (vote != ABB_BUFFER_VOTE_TCXO)
                while (!hi_abb_get_buffer_status(id)) ;
        }
    }
    g_abb_buffer_vote_map[id] |= (1UL << vote);

    spin_unlock_irqrestore(&g_abb_spin_lock, irq_flags);
#endif

    return 0;
}

int bsp_abb_buffer_disable(enum ABB_BUFFER_ID id, enum ABB_BUFFER_VOTE vote)
{
#ifdef ABB_K3V3
    unsigned long irq_flags;

    if ((id >= ABB_BUFFER_ID_MAX) || (vote >= ABB_BUFFER_VOTE_MAX))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ABB, "error args, id %d, vote %d\n", id, vote);
        return -1;
    }

    if (ABB_BUFFER_ID_0 == id)
        return 0;

    spin_lock_irqsave(&g_abb_spin_lock, irq_flags);

    g_abb_buffer_vote_map[id] &= ~(1UL << vote);
    if (!g_abb_buffer_vote_map[id])
    {
        if (hi_abb_control_status(id))
        {
            while (hi_abb_get_finish_status(id)) ;

            hi_abb_control_disable(id);

            while (!hi_abb_get_finish_status(id)) ;
        }
    }

    spin_unlock_irqrestore(&g_abb_spin_lock, irq_flags);
#endif

    return 0;
}

void bsp_abb_v500_init_later(void)
{
    u32 i;

    for (i = 0; i < sizeof(g_abb_cfg_v500) / sizeof(g_abb_cfg_v500[0]); i++)
    {
        bsp_abb_write(g_abb_cfg_v500[i].addr, g_abb_cfg_v500[i].value);
    }

    /* ??????ê???è?CA??ê?(ABB LTE Mode Config) */
    bsp_abb_write(0x79, 0x00);
    bsp_abb_write(0x73, 0x40);
    bsp_abb_write(0x74, 0x40);
    bsp_abb_write(0x75, 0x60);
    bsp_abb_write(0x99, 0x00);
    bsp_abb_write(0x8D, 0x0F);
    bsp_abb_write(0x9A, 0x49);

    /* ABB PLL Config */
    bsp_abb_write(0xD0, 0x03);  /* CH0 */
    bsp_abb_write(0x70, 0x03);  /* CH1 */

    /* ABB Clk Config */
    bsp_abb_write(0x0B, 0x23);
}

void bsp_abb_v510_init_later(void)
{
    u32 i;
    u32 temp = 0;

    bsp_abb_read(0xCB, &temp);
    temp &= 0xDF;
    bsp_abb_write(0xCB, temp);

    bsp_abb_read(0x6B, &temp);
    temp &= 0xDF;
    bsp_abb_write(0x6B, temp);


    bsp_abb_read(0xDC, &temp);
    bsp_abb_write(0xF6, temp);

    for (i = 0; i < sizeof(g_abb_cfg_v510) / sizeof(g_abb_cfg_v510[0]); i++)
    {
        bsp_abb_write(g_abb_cfg_v510[i].addr, g_abb_cfg_v510[i].value);
    }
}

void bsp_abb_v511_init_later(void)
{
    u32 i;
    u32 temp = 0;

    bsp_abb_read(0xCB, &temp);
    temp &= 0xDF;
    bsp_abb_write(0xCB, temp);

    bsp_abb_read(0x6B, &temp);
    temp &= 0xDF;
    bsp_abb_write(0x6B, temp);

    for (i = 0; i < sizeof(g_abb_cfg_v511) / sizeof(g_abb_cfg_v511[0]); i++)
    {
        bsp_abb_write(g_abb_cfg_v511[i].addr, g_abb_cfg_v511[i].value);
    }
}

void abb_v530_tcxo_sel(void)
{
    u32 ret = 0;
    u32 temp = 0;
    static DRV_TCXO_SEL_PARA_STRU tcxo_sel;

    if (!g_abb_is_inited)
    {
        ret = bsp_nvm_read(NV_ID_DRV_TCXO_SEL, (u8*)&tcxo_sel, sizeof(DRV_TCXO_SEL_PARA_STRU));
    }

    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ABB, "[%s]: bsp_nvm_read error, ret %x\n", __FUNCTION__, ret);
    }
    else
    {
        if (0 == tcxo_sel.ucABBCh0TcxoSel)
        {
            bsp_abb_read(0xF4, &temp);
            temp &= ~(1<<2);
            bsp_abb_write(0xF4, temp);
        }
        else
        {
            bsp_abb_read(0xF4, &temp);
            temp |= 1<<2;
            bsp_abb_write(0xF4, temp);
        }

        if (0 == tcxo_sel.ucABBCh1TcxoSel)
        {
            bsp_abb_read(0x94, &temp);
            temp |= 1<<2;
            bsp_abb_write(0x94, temp);
        }
        else
        {
            bsp_abb_read(0x94, &temp);
            temp &= ~(1<<2);
            bsp_abb_write(0x94, temp);
        }
    }
}

void bsp_abb_v530_init_later(void)
{
    u32 i = 0;
    u32 temp = 0;

    abb_v530_tcxo_sel();

    bsp_abb_write(0xCB, 0x44);
    bsp_abb_write(0x6B, 0x44);

    bsp_abb_read(0x7C, &temp);
    bsp_abb_write(0x96, temp);

    bsp_abb_read(0xDC, &temp);
    bsp_abb_write(0xF6, temp);
#if 0
    /* ET calibration */
    bsp_abb_write(0x34, 0x18);
    bsp_abb_write(0x12F, 0x02);
    bsp_abb_write(0xD0, 0x07);

    udelay(10);

    bsp_abb_write(0x1D1, 0x08);
    bsp_abb_write(0x21, 0x40);
    bsp_abb_write(0x22, 0x08);
    bsp_abb_write(0x3E, 0x40);
    bsp_abb_write(0x17, 0x0F);
    bsp_abb_write(0xCF, 0x08);
    bsp_abb_write(0x16, 0x09);

    do
    {
        bsp_abb_read(0x19, &temp);
        temp &= 0x80;
    }while (0 == temp);

    bsp_abb_write(0x16, 0x08);
    bsp_abb_write(0xCF, 0x00);
    bsp_abb_write(0x17, 0x00);
    bsp_abb_write(0x1D1, 0x00);
    bsp_abb_write(0x22, 0x0B);
    bsp_abb_write(0x3E, 0x4C);
    bsp_abb_write(0x16, 0x40);
#endif
    /* CH0 CA CK Tuning */
    bsp_abb_write(0xD0, 0x07);
    udelay(10);
    bsp_abb_write(0x1D1, 0x08);
    bsp_abb_write(0x1F, 0xA0);
    bsp_abb_write(0x3D, 0xFD);
    bsp_abb_write(0x20, 0x10);
    bsp_abb_write(0x3C, 0x00);
    bsp_abb_write(0xFA, 0x40);
    bsp_abb_write(0xFA, 0xC0);
    udelay(1);
    bsp_abb_read(0xDF, &temp);
    if (temp <= 0x06)
    {
        bsp_abb_write(0x10C, 0x02);
        bsp_abb_write(0x10D, 0x07);
    }
    else if (0x07 == temp)
    {
        bsp_abb_write(0x10C, 0x05);
        bsp_abb_write(0x10D, 0x09);
    }
    else if (0x08 == temp)
    {
        bsp_abb_write(0x10C, 0x06);
        bsp_abb_write(0x10D, 0x0A);
    }
    else if (0x09 == temp)
    {
        bsp_abb_write(0x10C, 0x0A);
        bsp_abb_write(0x10D, 0x0D);
    }
    else if (0x0A == temp)
    {
        bsp_abb_write(0x10C, 0x0B);
        bsp_abb_write(0x10D, 0x0D);
    }
    else /* temp >= 0x0B */
    {
        bsp_abb_write(0x10C, 0x0D);
        bsp_abb_write(0x10D, 0x10);
    }

    bsp_abb_write(0x1D1, 0x00);
    bsp_abb_write(0x20, 0x13);
    bsp_abb_write(0x3C, 0xF0);
    bsp_abb_write(0xD0, 0x0F);

    /* CH1 CA CK Tuning */
    bsp_abb_write(0x70, 0x07);
    udelay(10);
    bsp_abb_write(0x1C1, 0x08);
    bsp_abb_write(0x1F, 0xA0);
    bsp_abb_write(0x3D, 0xFD);
    bsp_abb_write(0x20, 0x10);
    bsp_abb_write(0x3C, 0x00);
    bsp_abb_write(0x9A, 0x40);
    bsp_abb_write(0x9A, 0xC0);
    udelay(1);
    bsp_abb_read(0x7F, &temp);
    if (temp <= 0x06)
    {
        bsp_abb_write(0x124, 0x02);
        bsp_abb_write(0x125, 0x07);
    }
    else if (0x07 == temp)
    {
        bsp_abb_write(0x124, 0x05);
        bsp_abb_write(0x125, 0x09);
    }
    else if (0x08 == temp)
    {
        bsp_abb_write(0x124, 0x06);
        bsp_abb_write(0x125, 0x0A);
    }
    else if (0x09 == temp)
    {
        bsp_abb_write(0x124, 0x0A);
        bsp_abb_write(0x125, 0x0D);
    }
    else if (0x0A == temp)
    {
        bsp_abb_write(0x124, 0x0B);
        bsp_abb_write(0x125, 0x0D);
    }
    else /* temp >= 0x0B */
    {
        bsp_abb_write(0x124, 0x0D);
        bsp_abb_write(0x125, 0x10);
    }

    bsp_abb_write(0x1C1, 0x00);
    bsp_abb_write(0x20, 0x13);
    bsp_abb_write(0x3C, 0xF0);
    bsp_abb_write(0x70, 0x0F);

    for (i = 0; i < sizeof(g_abb_cfg_v530) / sizeof(g_abb_cfg_v530[0]); i++)
    {
        bsp_abb_write(g_abb_cfg_v530[i].addr, g_abb_cfg_v530[i].value);
    }
}

void bsp_abb_v550_init_later(void)
{
    u32 i = 0;
    u32 temp = 0;

    abb_v530_tcxo_sel();

    bsp_abb_write(0xCB, 0x44);
    bsp_abb_write(0x6B, 0x44);

    bsp_abb_read(0x7C, &temp);
    bsp_abb_write(0x96, temp);

    bsp_abb_read(0xDC, &temp);
    bsp_abb_write(0xF6, temp);
#if 0
    /* ET calibration */
    bsp_abb_write(0x34, 0x18);
    bsp_abb_write(0x12F, 0x02);
    bsp_abb_write(0xD0, 0x07);

    udelay(10);

    bsp_abb_write(0x1D1, 0x08);
    bsp_abb_write(0x21, 0x40);
    bsp_abb_write(0x22, 0x08);
    bsp_abb_write(0x3E, 0x40);
    bsp_abb_write(0x17, 0x0F);
    bsp_abb_write(0xCF, 0x08);
    bsp_abb_write(0x16, 0x09);

    do
    {
        bsp_abb_read(0x19, &temp);
        temp &= 0x80;
    }while (0 == temp);

    bsp_abb_write(0x16, 0x08);
    bsp_abb_write(0xCF, 0x00);
    bsp_abb_write(0x17, 0x00);
    bsp_abb_write(0x1D1, 0x00);
    bsp_abb_write(0x22, 0x0B);
    bsp_abb_write(0x3E, 0x4C);
    bsp_abb_write(0x16, 0x40);
#endif
    /* CH0 CA CK Tuning */
    bsp_abb_write(0xD0, 0x07);
    udelay(10);
    bsp_abb_write(0x1D1, 0x08);
    bsp_abb_write(0x1F, 0xA0);
    bsp_abb_write(0x3D, 0xFD);
    bsp_abb_write(0x20, 0x10);
    bsp_abb_write(0x3C, 0x00);
    bsp_abb_write(0xFA, 0x40);
    bsp_abb_write(0xFA, 0xC0);
    udelay(1);
    bsp_abb_read(0xDF, &temp);
    if (temp <= 0x06)
    {
        bsp_abb_write(0x10C, 0x02);
        bsp_abb_write(0x10D, 0x07);
    }
    else if (0x07 == temp)
    {
        bsp_abb_write(0x10C, 0x05);
        bsp_abb_write(0x10D, 0x09);
    }
    else if (0x08 == temp)
    {
        bsp_abb_write(0x10C, 0x06);
        bsp_abb_write(0x10D, 0x0A);
    }
    else if (0x09 == temp)
    {
        bsp_abb_write(0x10C, 0x0A);
        bsp_abb_write(0x10D, 0x0D);
    }
    else if (0x0A == temp)
    {
        bsp_abb_write(0x10C, 0x0B);
        bsp_abb_write(0x10D, 0x0D);
    }
    else /* temp >= 0x0B */
    {
        bsp_abb_write(0x10C, 0x0D);
        bsp_abb_write(0x10D, 0x10);
    }

    bsp_abb_write(0x1D1, 0x00);
    bsp_abb_write(0x20, 0x13);
    bsp_abb_write(0x3C, 0xF0);
    bsp_abb_write(0xD0, 0x0F);

    /* CH1 CA CK Tuning */
    bsp_abb_write(0x70, 0x07);
    udelay(10);
    bsp_abb_write(0x1C1, 0x08);
    bsp_abb_write(0x1F, 0xA0);
    bsp_abb_write(0x3D, 0xFD);
    bsp_abb_write(0x20, 0x10);
    bsp_abb_write(0x3C, 0x00);
    bsp_abb_write(0x9A, 0x40);
    bsp_abb_write(0x9A, 0xC0);
    udelay(1);
    bsp_abb_read(0x7F, &temp);
    if (temp <= 0x06)
    {
        bsp_abb_write(0x124, 0x02);
        bsp_abb_write(0x125, 0x07);
    }
    else if (0x07 == temp)
    {
        bsp_abb_write(0x124, 0x05);
        bsp_abb_write(0x125, 0x09);
    }
    else if (0x08 == temp)
    {
        bsp_abb_write(0x124, 0x06);
        bsp_abb_write(0x125, 0x0A);
    }
    else if (0x09 == temp)
    {
        bsp_abb_write(0x124, 0x0A);
        bsp_abb_write(0x125, 0x0D);
    }
    else if (0x0A == temp)
    {
        bsp_abb_write(0x124, 0x0B);
        bsp_abb_write(0x125, 0x0D);
    }
    else /* temp >= 0x0B */
    {
        bsp_abb_write(0x124, 0x0D);
        bsp_abb_write(0x125, 0x10);
    }

    bsp_abb_write(0x1C1, 0x00);
    bsp_abb_write(0x20, 0x13);
    bsp_abb_write(0x3C, 0xF0);
    bsp_abb_write(0x70, 0x0F);

    for (i = 0; i < sizeof(g_abb_cfg_v550) / sizeof(g_abb_cfg_v550[0]); i++)
    {
        bsp_abb_write(g_abb_cfg_v550[i].addr, g_abb_cfg_v550[i].value);
    }
}

static int bsp_abb_v530_resume_later(void)
{
    u32 i;

    bsp_abb_write(0x304, 0x01);
    bsp_abb_write(0x16, g_abb_data_0x16);
    for (i = 0; i < (0x304 - 0x2C6); i++)
    {
        bsp_abb_write(i + 0x2C6, g_abb_data_v530[i]);
    }
    return 0;
}

#ifdef CONFIG_CCORE_PM
static struct dpm_device g_abb_dpm_device={
    .device_name = "abb_dpm",
    .prepare = NULL,
    .suspend_early= NULL,
    .suspend = bsp_abb_suspend,
    .suspend_late = NULL,   /* this is for dsp subsustem */
    .resume_early = NULL,   /* this is for dsp subsustem */
    .resume = bsp_abb_resume,
    .resume_late = NULL,
    .complete = NULL,
};
#endif

void bsp_abb_unreset(void)
{
#ifdef BSP_CONFIG_HI3630
    set_hi_crg_srstdis3_abb_srst_dis(1);
#endif
}

void bsp_abb_init(void)
{

    u32 calibration_ok = 0;
    u32 channel0_state = 0;
    u32 channel1_state = 0;

    u32 abb_version = 0;

    bsp_abb_unreset();

    /* read the version of ComStar */
    bsp_abb_read(0x40, &abb_version);
	if (!g_abb_is_inited)
	{
	    if ((COMSTARV500 != abb_version) &&
	        (COMSTARV510 != abb_version) &&
	        (COMSTARV511 != abb_version) &&
	        (COMSTARV530 != abb_version) &&
	        (COMSTARV550 != abb_version))
	    {
	        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ABB, "don't support this ComStar version: %x\r\n", abb_version);
	        return;
	    }
	}
#ifdef CONFIG_CCORE_PM
    if (!g_abb_is_inited)
    {
	    int ret;
        ret = bsp_device_pm_add(&g_abb_dpm_device);
        if(ret)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ABB, "fail to add abb dpm device\r\n");
            return;
        }
    }
#endif

    /* reset VREF */
    bsp_abb_write(0x85, 0x80);
    udelay(3);
    bsp_abb_write(0x85, 0x00);

    bsp_abb_write(0xE5, 0x80);
    udelay(3);
    bsp_abb_write(0xE5, 0x00);

    /* start ABB calibration */
    if (COMSTARV500 != abb_version)
    {
        bsp_abb_write(0x72, 0x0C);
        bsp_abb_write(0x72, 0x0D);
        bsp_abb_write(0xD2, 0x0C);
        bsp_abb_write(0xD2, 0x0D);

        do
        {
            /* wait for calibration complete */
            bsp_abb_read(0x7B, &channel0_state);
            bsp_abb_read(0xDB, &channel1_state);

            calibration_ok = channel0_state & channel1_state & 0x20;
        }while (!calibration_ok);
    }


    if (COMSTARV500 == abb_version)
    {
        bsp_abb_v500_init_later();
    }
    else if (COMSTARV510 == abb_version)
    {
        bsp_abb_v510_init_later();
    }
    else if (COMSTARV511 == abb_version)
    {
        bsp_abb_v511_init_later();
    }
    else if (COMSTARV530 == abb_version)
    {
        bsp_abb_v530_init_later();
    }
    else if (COMSTARV550 == abb_version)
    {
        bsp_abb_v550_init_later();
    }
    else
    {
        /* for pclint */
    }

    if (!g_abb_is_inited)
    {
        g_abb_is_inited = 1;

        spin_lock_init(&g_abb_spin_lock);

        (void)memset((void*)&g_abb_buffer_vote_map[0], 0, sizeof(g_abb_buffer_vote_map));

        printf("abb init ok, version: 0x%x\r\n", abb_version);
    }
}

int bsp_abb_suspend(struct dpm_device *dev)
{
    u32 i;
    u32 abb_version = 0;

    /* read the version of ComStar */
    bsp_abb_read(0x40, &abb_version);

    if (COMSTARV500 == abb_version)
    {
        for (i = 0; i < sizeof(g_abb_cfg_v500) / sizeof(g_abb_cfg_v500[0]); i++)
        {
            bsp_abb_read(g_abb_cfg_v500[i].addr, &(g_abb_cfg_v500[i].value));
        }
    }
    else if (COMSTARV510 == abb_version)
    {
        for (i = 0; i < sizeof(g_abb_cfg_v510) / sizeof(g_abb_cfg_v510[0]); i++)
        {
            bsp_abb_read(g_abb_cfg_v510[i].addr, &(g_abb_cfg_v510[i].value));
        }
    }
    else if (COMSTARV511 == abb_version)
    {
        for (i = 0; i < sizeof(g_abb_cfg_v511) / sizeof(g_abb_cfg_v511[0]); i++)
        {
            bsp_abb_read(g_abb_cfg_v511[i].addr, &(g_abb_cfg_v511[i].value));
        }
    }
    else if ((COMSTARV530 == abb_version) ||(COMSTARV550 == abb_version))
    {
        for (i = 0; i < sizeof(g_abb_cfg_v530) / sizeof(g_abb_cfg_v530[0]); i++)
        {
            bsp_abb_read(g_abb_cfg_v530[i].addr, &(g_abb_cfg_v530[i].value));
        }
        bsp_abb_read(0x16, &g_abb_data_0x16);
        for (i = 0; i < (0x304 - 0x2C6); i++)
        {
            bsp_abb_read(i + 0x2C6, &(g_abb_data_v530[i]));
        }
    }
    else
    {
        /*bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ABB, "don't support this ComStar version: %x", abb_version);*/
        return -1;
    }

    return 0;
}

int bsp_abb_resume(struct dpm_device *dev)
{
    int ret = 0;
    u32 abb_version = 0;

    bsp_abb_read(0x40, &abb_version);

    bsp_abb_init();

    if ((COMSTARV530 == abb_version) || (COMSTARV550 == abb_version))
    {
        ret = bsp_abb_v530_resume_later();
    }

    return ret;
}
#else

void bsp_abb_init(void)
{
    return;
}

int bsp_abb_read(u32 addr, u32 *value)
{
    return 0;
}

int bsp_abb_write(u32 addr, u32 value)
{
    return 0;
}

int bsp_abb_suspend(struct dpm_device *dev)
{
    return 0;
}

int bsp_abb_resume(struct dpm_device *dev)
{
    return 0;
}

#endif


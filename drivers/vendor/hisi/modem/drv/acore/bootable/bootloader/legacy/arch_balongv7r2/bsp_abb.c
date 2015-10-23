


#include <soc_memmap.h>
#include <boot/boot.h>
#include <balongv7r2/types.h>


struct abb_config
{
    u16 addr;
    u16 value;
};

static struct abb_config g_abb_cfg_v530[] =
{
    {0x2D    ,0xAA},
    {0x31    ,0x56},
    {0x39    ,0xAA},
    {0x71    ,0x40},
    {0xD1    ,0x40},
    {0x3     ,0x23},
    {0x8     ,0x23},
    {0xD     ,0x23},
    {0x12    ,0x23},
    {0x1D    ,0x23},
    {0x183    ,0x23},
    {0x24    ,0xFE},
    {0x28    ,0xFA},
    {0x2C    ,0xF8},
    {0x30    ,0xFC},
    {0x38    ,0xF8},
    {0x185    ,0xFA},
    {0x7A    ,0x00},
    {0xDA    ,0x00},
    {0x105    ,0x80},
    {0x117    ,0x82},
    {0x129    ,0x82},
    {0x18E    ,0x82},
    {0x111    ,0x82},
    {0x10B    ,0x82},
    {0x189    ,0x09},
    {0x106    ,0x09},
    {0x112    ,0x09},
    {0x100    ,0x09},
    {0x18A    ,0x0F},
    {0x107    ,0x0F},
    {0x113    ,0x0F},
    {0x101    ,0x0F},
    {0x10C    ,0x09},
    {0x10D    ,0x0F},
    {0x104    ,0x80},
    {0x10A    ,0x80},
    {0x116    ,0x80},
    {0x18D    ,0x80},
    {0x110    ,0x80},
    {0x128    ,0x80},
    {0x12F    ,0x02},
    {0x26    ,0x0F},
    {0x1A    ,0x40},
    {0x1C    ,0x40},
    {0x3A    ,0x4C},
    {0x3B    ,0x15},
    {0x124    ,0x09},
    {0x125    ,0x0F},
    {0xF     ,0x26},
    {0x30    ,0xFA},
    {0x31    ,0x02},
    {0x11    ,0x62},
    {0x32    ,0x1D},
    {0x7     ,0x22},
    {0x2A    ,0x1D},
    {0x182    ,0x22},
    {0x187    ,0x1D},
    {0xD0    ,0x07},
    {0x70    ,0x07},
};

/* used for abb suspend */
static u16 g_abb_data_0x16;
static u16 g_abb_data_v530[0x304-0x2C6];


void bsp_abb_read(u16 addr, u16 *value)
{
    *value = readl(HI_ABB_REG_BASE_ADDR + (addr << 2));
}

void bsp_abb_write(u16 addr, u16 value)
{
    writel(value, HI_ABB_REG_BASE_ADDR + (addr << 2));
}

void bsp_abb_v530_init_later()
{
    u32 i, nCount = 20000;
    u16 temp = 0;

    bsp_abb_write(0xCB, 0x44);
    bsp_abb_write(0x6B, 0x44);

    bsp_abb_read(0x7C, &temp);
    bsp_abb_write(0x96, temp);

    bsp_abb_read(0xDC, &temp);
    bsp_abb_write(0xF6, temp);

    /* ET calibration */
    bsp_abb_write(0x12F, 0x02);
    bsp_abb_write(0xD0, 0x07);

    udelay(10);

    bsp_abb_write(0x1D1, 0x08);
    bsp_abb_write(0x21, 0x40);
    bsp_abb_write(0x22, 0x08);
    bsp_abb_write(0x3E, 0x40);
    bsp_abb_write(0x17, 0x07);
    bsp_abb_write(0xCF, 0x08);
    bsp_abb_write(0x16, 0x01);
    do
    {
        bsp_abb_read(0x19, &temp);
        temp &= 0x80;
        nCount--;
    }while((0 == temp) && (nCount));
    if (!nCount)
    {
        cprintf("ABB CT calibration timeout\r\n");
        return;
    }
    bsp_abb_write(0x16, 0x00);
    bsp_abb_write(0xCF, 0x00);
    bsp_abb_write(0x17, 0x00);
    bsp_abb_write(0x1D1, 0x00);
    bsp_abb_write(0x22, 0x0B);
    bsp_abb_write(0x3E, 0x4C);
    bsp_abb_write(0x16, 0x50);

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
    bsp_abb_write(0x10C, temp);
    bsp_abb_write(0x10D, temp * 2);
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
    bsp_abb_write(0x124, temp);
    bsp_abb_write(0x125, temp * 2);
    bsp_abb_write(0x1C1, 0x00);
    bsp_abb_write(0x20, 0x13);
    bsp_abb_write(0x3C, 0xF0);
    bsp_abb_write(0x70, 0x0F);

    for (i = 0; i < sizeof(g_abb_cfg_v530) / sizeof(g_abb_cfg_v530[0]); i++)
    {
        bsp_abb_write(g_abb_cfg_v530[i].addr, g_abb_cfg_v530[i].value);
    }
}

void bsp_abb_init(void)
{
    int nCount = 20000;
    u16 calibration_ok = 0;
    u16 channel0_state = 0;
    u16 channel1_state = 0;

    u16 abb_version = 0;

    /* read the version of ComStar */
    bsp_abb_read(0x40, &abb_version);

#if 0
    if (0xAC != abb_version)
    {
        return;
    }

    /* reset VREF */
    bsp_abb_write(0x85, 0x80);
    udelay(3);
    bsp_abb_write(0x85, 0x00);

    bsp_abb_write(0xE5, 0x80);
    udelay(3);
    bsp_abb_write(0xE5, 0x00);

    /* start ABB calibration */
    if (0xA0 != abb_version)
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
            nCount--;
        }while (!calibration_ok && nCount);

        if (!nCount)
        {
            return;
        }

    }

    bsp_abb_v530_init_later();
    return;
#endif
}


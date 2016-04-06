/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
*/

#include <linux/clk.h>
#include <linux/clk-private.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/hardware/arm_timer.h>
#include <linux/module.h>
#include <linux/debugfs.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/of_address.h>
#include <bsp_icc.h>
#include <bsp_socp.h>
#include <product_config.h>

#ifdef CONFIG_MODEM_PERF
#define MODULE_NAME                            "bandwidth_dump_irq"

#define IRQ_CPU_CORE 7

/*use dual timer5 which clock name is clk_timclk6 in dts*/
#define RES_SIZE    0x1000
#define TIMER5_BASE 0xE8A01000
#define TIMER5_IRQ  78
#define TIMER_NAME "clk_timclk6"

/*timing for 1 s */
#define BW_TIMER_FREQ_48M                  (4800000)
/*timer_value = time * freq*/
#define BW_TIMER_SET_VALUE(x)              ((BW_TIMER_FREQ_48M) / 100000 * (x) / 10)

/*timer2 register offsets and masks */
#define BW_TIMER_DEFAULT_LOAD              (0xFFFFFFFF)
/*max data of int32*/
#define MAX_DATA_OF_32BIT                  (0xFFFFFFFF)
/*max time that timer2 can be timing(useconds)*/
#define BW_TIMER_MAX_TIMING                ((MAX_DATA_OF_32BIT / BW_TIMER_FREQ_48M) * 1000000)

static char __iomem * file_addr = NULL;
#define FILE_DUMP_ADDR 0x38000000
#define FILE_MAX_SIZE    0x4000000

static char __iomem *ddrc_base = NULL;
static char __iomem *noc_dss_base = NULL;
static char __iomem *vdec_base = NULL;
static char __iomem *asp_base = NULL;
static char __iomem *noc_modem_base = NULL;

static u64 cur_pos = 0;
static struct timeval hrtv;

// default monitor CPU
static int ddrc_port_monitor = 0x7;
// default monitor dss osd
static int dss_latency = 0;

#define MAX_MOUDLE_NUM 16
static int module_mask = (1<<0 | 1<<2 | 1<<3 | 1<<4);

#define outp32(addr, val) writel(val, addr)

void set_reg_dump(char __iomem *addr, u32 val, u8 bw, u8 bs)
{
    u32 mask = (1 << bw) - 1;
    u32 tmp = 0;

    tmp = readl(addr);
    tmp &= ~(mask << bs);
    outp32(addr, tmp | ((val & mask) << bs));
}

void ddrc_bandwidth_init(void)
{
    outp32(ddrc_base + 0x8010, 0);
    outp32(ddrc_base + 0x9010, 0);
    //0x6--- GPU  0x7 --- CPU
    outp32(ddrc_base + 0x8274, ddrc_port_monitor);
    outp32(ddrc_base + 0x8278, 0xf);
    //0x6--- GPU  0x7 --- CPU
    outp32(ddrc_base + 0x9274, ddrc_port_monitor);
    outp32(ddrc_base + 0x9278, 0xf);

    outp32(ddrc_base + 0x8010, 1);
    outp32(ddrc_base + 0x9010, 1);

    outp32(ddrc_base + 0x4120, 0x20000000);
}

void print_DMC_info(void)
{
    cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1,
                        "%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t",
                        /* DMC info */
                        readl(ddrc_base + 0x8380),
                        readl(ddrc_base + 0x8384),
                        readl(ddrc_base + 0x9380),
                        readl(ddrc_base + 0x9384),

                        readl(ddrc_base + 0x8390),
                        readl(ddrc_base + 0x8394),
                        readl(ddrc_base + 0x9390),
                        readl(ddrc_base + 0x9394),

                        readl(ddrc_base + 0x4124),
                        readl(ddrc_base + 0x41E4),
                        readl(ddrc_base + 0x41E8));
}

void ddrc_timeout_init(void)
{
    outp32(ddrc_base + 0x41F0, 0);
    outp32(ddrc_base + 0x40D0, 0x03020100);
    outp32(ddrc_base + 0x40AC, 0x00400000);
}

void ddrc_latency_init(void)
{
    //ddrc_timeout_init();
    outp32(ddrc_base + 0x8010, 0);
    outp32(ddrc_base + 0x9010, 0);
    outp32(ddrc_base + 0x8274, 0x2);
    outp32(ddrc_base + 0x8278, 0xf);
    outp32(ddrc_base + 0x9274, 0x2);
    outp32(ddrc_base + 0x9278, 0xf);
    outp32(ddrc_base + 0x8010, 1);
    outp32(ddrc_base + 0x9010, 1);
}

void print_ddrc_latency_info(void)
{
#if 0
    cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1,
                        "%08x\t%08x\t",
                        /* DDRC latency */
                        ((readl(ddrc_base + 0x83A8)>>16) & 0xffff),
                        ((readl(ddrc_base + 0x93A8)>>16) & 0xffff));
#endif
}

void Noc_dss_init(void)
{
    set_reg_dump(noc_dss_base + 0x080C, 0, 1, 0);
    set_reg_dump(noc_dss_base + 0x0808, 0, 1, 3);
    set_reg_dump(noc_dss_base + 0x0808, 1, 1, 3);
    outp32(noc_dss_base + 0x0938, 0x14);
    outp32(noc_dss_base + 0x094c, 0x10);
    outp32(noc_dss_base + 0x0960, 0xc);
    outp32(noc_dss_base + 0x0974, 0x10);
    outp32(noc_dss_base + 0x0988, 0x8);
    outp32(noc_dss_base + 0x099c, 0x10);
    outp32(noc_dss_base + 0x0824, 0);
    outp32(noc_dss_base + 0x093c, 0x2);
    outp32(noc_dss_base + 0x0950, 0);
    outp32(noc_dss_base + 0x0964, 0x2);
    outp32(noc_dss_base + 0x0978, 0);
    outp32(noc_dss_base + 0x098c, 0x2);
    outp32(noc_dss_base + 0x09a0, 0);
    outp32(noc_dss_base + 0x0830, 1);

    outp32(noc_dss_base + 0x0844, 0x0200);
    outp32(noc_dss_base + 0x0848, 0x3f80);

    outp32(noc_dss_base + 0x084c, 0);
    outp32(noc_dss_base + 0x0854, 0x20);

    outp32(noc_dss_base + 0x0860, 0xf);
    outp32(noc_dss_base + 0x0864, 0x3);
    outp32(noc_dss_base + 0x0868, 0x8);
    outp32(noc_dss_base + 0x086c, 0);

    set_reg_dump(noc_dss_base + 0x080C, 1, 1, 0);
}

void Noc_dss_stop(void)
{
    set_reg_dump(noc_dss_base + 0x0828, 1, 1, 0);
    //set_reg_dump(noc_dss_base + 0x080c, 0, 1, 0);
}

void print_noc_dss_info(void)
{
#if 0
    cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1,
                        "%04x%04x\t%04x%04x\t%04x%04x\t",
                        /* NOC dss info */
                        readl(noc_dss_base + 0x0954),
                        readl(noc_dss_base + 0x0940),
                        readl(noc_dss_base + 0x097c),
                        readl(noc_dss_base + 0x0968),
                        readl(noc_dss_base + 0x09a4),
                        readl(noc_dss_base + 0x0990));
#endif
}

void Noc_dss_start(void)
{
    set_reg_dump(noc_dss_base + 0x080C, 0, 1, 0);
    outp32(noc_dss_base + 0x0838, 1);
    set_reg_dump(noc_dss_base + 0x0808, 0, 1, 3);
    set_reg_dump(noc_dss_base + 0x0808, 1, 1, 3);
    set_reg_dump(noc_dss_base + 0x080C, 1, 1, 0);
}

void Noc_vdec_init(void)
{
    set_reg_dump(vdec_base + 0x080C, 0, 1, 0);
    set_reg_dump(vdec_base + 0x0808, 0, 1, 3);
    set_reg_dump(vdec_base + 0x0808, 1, 1, 3);
    outp32(vdec_base + 0x0938, 0x14);
    outp32(vdec_base + 0x094c, 0x10);
    outp32(vdec_base + 0x0960, 0x12);
    outp32(vdec_base + 0x0974, 0x10);
    outp32(vdec_base + 0x0988, 0xD);
    outp32(vdec_base + 0x099c, 0x10);
    outp32(vdec_base + 0x09b0, 0x8);
    outp32(vdec_base + 0x09c4, 0x10);
    outp32(vdec_base + 0x0824, 0);
    outp32(vdec_base + 0x093c, 0x2);
    outp32(vdec_base + 0x0950, 0);
    outp32(vdec_base + 0x0964, 0x2);
    outp32(vdec_base + 0x0978, 0);
    outp32(vdec_base + 0x098c, 0x2);
    outp32(vdec_base + 0x09a0, 0);
    outp32(vdec_base + 0x09b4, 0x2);
    outp32(vdec_base + 0x09c8, 0);
    outp32(vdec_base + 0x0830, 1);

    outp32(vdec_base + 0x0844, 0x400);
    outp32(vdec_base + 0x0848, 0xFC0);
    outp32(vdec_base + 0x084c, 0x0);
    outp32(vdec_base + 0x0854, 0x20);
    outp32(vdec_base + 0x0860, 0xD);
    outp32(vdec_base + 0x0864, 0x3);
    outp32(vdec_base + 0x0868, 0x8);
    outp32(vdec_base + 0x086c, 0x0);

    outp32(vdec_base + 0x0880, 0x400);
    outp32(vdec_base + 0x0884, 0xFC0);
    outp32(vdec_base + 0x0888, 0x0);
    outp32(vdec_base + 0x0890, 0x20);
    outp32(vdec_base + 0x089c, 0xE);
    outp32(vdec_base + 0x08a0, 0x3);
    outp32(vdec_base + 0x08a4, 0x8);
    outp32(vdec_base + 0x08a8, 0x0);

    outp32(vdec_base + 0x0814, 0xc);

    set_reg_dump(vdec_base + 0x080C, 1, 1, 0);
}

void Noc_vdec_stop(void)
{
    set_reg_dump(vdec_base + 0x0828, 1, 1, 0);
}

void print_noc_vdec_info(void)
{
#if 0
    cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1,
                        "%08x\t%08x\t%08x\t%08x\t",
                        /* NOC VDEC info */
                        ((readl(vdec_base + 0x0954) & 0x3ff) << 10) | (readl(vdec_base + 0x0940) & 0x3ff),
                        ((readl(vdec_base + 0x097c) & 0x3ff) << 10) | (readl(vdec_base + 0x0968) & 0x3ff),
                        ((readl(vdec_base + 0x09a4) & 0x3ff) << 10) | (readl(vdec_base + 0x0990) & 0x3ff),
                        ((readl(vdec_base + 0x09cc) & 0x3ff) << 10) | (readl(vdec_base + 0x09b8) & 0x3ff));
#endif
}

void Noc_vdec_start(void)
{
    set_reg_dump(vdec_base + 0x080C, 0, 1, 0);
    outp32(vdec_base + 0x0838, 1);
    set_reg_dump(vdec_base + 0x0808, 0, 1, 3);
    set_reg_dump(vdec_base + 0x0808, 1, 1, 3);
    set_reg_dump(vdec_base + 0x080C, 1, 1, 0);
}

void dss_latency_osd_init(void)
{
    set_reg_dump(noc_dss_base + 0x240c, 0, 1, 0);
    set_reg_dump(noc_dss_base + 0x2408, 0, 1, 3);
    set_reg_dump(noc_dss_base + 0x2408, 1, 1, 3);
    outp32(noc_dss_base + 0x2088, 0x1);

    outp32(noc_dss_base + 0x208c, 0x0);
    outp32(noc_dss_base + 0x2094, 0x20);
    outp32(noc_dss_base + 0x20a0, 0x3);
    outp32(noc_dss_base + 0x20a8, 0x0);
    outp32(noc_dss_base + 0x20b0, 0x0);

    if (dss_latency) {
        outp32(noc_dss_base + 0x2078, 0x8); // for latency only
    } else {
        outp32(noc_dss_base + 0x2074, 1); // for outstanding only
    }

    outp32(noc_dss_base + 0x2008, 0x1);

    if (dss_latency) {
        outp32(noc_dss_base + 0x200c, 0x0);// for latency only
    } else {
        outp32(noc_dss_base + 0x200c, 1); // for outstanding only
    }

    outp32(noc_dss_base + 0x202c, 0x0);
    outp32(noc_dss_base + 0x2030, 0x1);
    outp32(noc_dss_base + 0x2034, 0x2);
    outp32(noc_dss_base + 0x2038, 0x3);

    outp32(noc_dss_base + 0x2538, 0x20);
    outp32(noc_dss_base + 0x254c, 0x21);
    outp32(noc_dss_base + 0x2560, 0x22);
    outp32(noc_dss_base + 0x2574, 0x23);
    outp32(noc_dss_base + 0x2588, 0x24);

    outp32(noc_dss_base + 0x2424, 0x0);

    outp32(noc_dss_base + 0x253c, 0x2);
    outp32(noc_dss_base + 0x2550, 0x2);
    outp32(noc_dss_base + 0x2564, 0x2);
    outp32(noc_dss_base + 0x2578, 0x2);
    outp32(noc_dss_base + 0x258c, 0x2);

    outp32(noc_dss_base + 0x2430, 0x1);
    outp32(noc_dss_base + 0x240c, 0x1);
}

void dss_latency_osd_stop(void)
{
    outp32(noc_dss_base + 0x2428, 0x1);
}

void print_dss_latency_osd_info(void)
{
#if 0
    cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1,
                        "%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t",
                        /* DSS latency info */
                        readl(noc_dss_base + 0x2540),
                        readl(noc_dss_base + 0x2554),
                        readl(noc_dss_base + 0x2568),
                        readl(noc_dss_base + 0x257c),
                        readl(noc_dss_base + 0x2590),
                        readl(noc_dss_base + 0x2078),
                        readl(noc_dss_base + 0x206c));
#endif
}

void dss_latency_osd_start(void)
{
    outp32(noc_dss_base + 0x240c, 0x0);
    outp32(noc_dss_base + 0x2438, 1);

    if (readl(noc_dss_base + 0x206c)) {
        outp32(noc_dss_base + 0x2070, 1);
    }

    set_reg_dump(noc_dss_base + 0x2408, 0, 1, 3);
    set_reg_dump(noc_dss_base + 0x2408, 1, 1, 3);
    outp32(noc_dss_base + 0x240c, 0x1);
}

void Noc_asp_init(void)
{
    set_reg_dump(asp_base + 0x0C0C, 0, 1, 0);
    set_reg_dump(asp_base + 0x0C08, 0, 1, 3);
    set_reg_dump(asp_base + 0x0C08, 1, 1, 3);
    outp32(asp_base + 0x0D38, 0x14);
    outp32(asp_base + 0x0D4c, 0x10);
    outp32(asp_base + 0x0D60, 0x12);
    outp32(asp_base + 0x0D74, 0x10);
    outp32(asp_base + 0x0D88, 0xD);
    outp32(asp_base + 0x0D9c, 0x10);
    outp32(asp_base + 0x0Db0, 0x8);
    outp32(asp_base + 0x0Dc4, 0x10);
    outp32(asp_base + 0x0C24, 0);
    outp32(asp_base + 0x0D3c, 0x2);
    outp32(asp_base + 0x0D50, 0);
    outp32(asp_base + 0x0D64, 0x2);
    outp32(asp_base + 0x0D78, 0);
    outp32(asp_base + 0x0D8c, 0x2);
    outp32(asp_base + 0x0Da0, 0);
    outp32(asp_base + 0x0Db4, 0x2);
    outp32(asp_base + 0x0Dc8, 0);
    outp32(asp_base + 0x0C30, 1);

    outp32(asp_base + 0x0C44, 0x0A4000);
    outp32(asp_base + 0x0C48, 0x3FF000);
    outp32(asp_base + 0x0C4c, 0x0);
    outp32(asp_base + 0x0C54, 0x20);
    outp32(asp_base + 0x0C60, 0xD);
    outp32(asp_base + 0x0C64, 0x3);
    outp32(asp_base + 0x0C68, 0x8);
    outp32(asp_base + 0x0C6c, 0x0);

    outp32(asp_base + 0x0C80, 0x0A4000);
    outp32(asp_base + 0x0C84, 0x3FF000);
    outp32(asp_base + 0x0C88, 0x0);
    outp32(asp_base + 0x0C90, 0x20);
    outp32(asp_base + 0x0C9c, 0xE);
    outp32(asp_base + 0x0Ca0, 0x3);
    outp32(asp_base + 0x0Ca4, 0x8);
    outp32(asp_base + 0x0Ca8, 0x0);

    outp32(asp_base + 0x0C14, 0xc);

    set_reg_dump(asp_base + 0x0C0C, 1, 1, 0);
}

void Noc_asp_stop(void)
{
    set_reg_dump(asp_base + 0x0C28, 1, 1, 0);
}

void print_noc_asp_info(void)
{
#if 0
    cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1,
                        "%08x\t%08x\t%08x\t%08x\t",
                        /* ASP bandwidth */
                        ((readl(asp_base + 0x0D54) & 0x7FFF) << 15) | (readl(asp_base + 0x0D40) & 0x7FFF),
                        ((readl(asp_base + 0x0D7c) & 0x7FFF) << 15) | (readl(asp_base + 0x0D68) & 0x7FFF),
                        ((readl(asp_base + 0x0Da4) & 0x7FFF) << 15) | (readl(asp_base + 0x0D90) & 0x7FFF),
                        ((readl(asp_base + 0x0Dcc) & 0x7FFF) << 15) | (readl(asp_base + 0x0Db8) & 0x7FFF));
#endif
}

void Noc_asp_start(void)
{
    set_reg_dump(asp_base + 0x0C0C, 0, 1, 0);
    outp32(asp_base + 0x0C38, 1);
    set_reg_dump(asp_base + 0x0C08, 0, 1, 3);
    set_reg_dump(asp_base + 0x0C08, 1, 1, 3);
    set_reg_dump(asp_base + 0x0C0C, 1, 1, 0);
}

void Noc_modem_init(void)
{
    set_reg_dump(asp_base + 0x0C0C, 0, 1, 0);
    set_reg_dump(asp_base + 0x0C08, 0, 1, 3);
    set_reg_dump(asp_base + 0x0C08, 1, 1, 3);
    outp32(asp_base + 0x0D38, 0x14);
    outp32(asp_base + 0x0D4c, 0x10);
    outp32(asp_base + 0x0D60, 0x12);
    outp32(asp_base + 0x0D74, 0x10);
    outp32(asp_base + 0x0D88, 0xD);
    outp32(asp_base + 0x0D9c, 0x10);
    outp32(asp_base + 0x0Db0, 0x8);
    outp32(asp_base + 0x0Dc4, 0x10);
    outp32(asp_base + 0x0C24, 0);
    outp32(asp_base + 0x0D3c, 0x2);
    outp32(asp_base + 0x0D50, 0);
    outp32(asp_base + 0x0D64, 0x2);
    outp32(asp_base + 0x0D78, 0);
    outp32(asp_base + 0x0D8c, 0x2);
    outp32(asp_base + 0x0Da0, 0);
    outp32(asp_base + 0x0Db4, 0x2);
    outp32(asp_base + 0x0Dc8, 0);
    outp32(asp_base + 0x0C30, 1);

    outp32(asp_base + 0x0C44, 0x194000);
    outp32(asp_base + 0x0C48, 0x3FF000);
    outp32(asp_base + 0x0C4c, 0x0);
    outp32(asp_base + 0x0C54, 0x20);
    outp32(asp_base + 0x0C60, 0xD);
    outp32(asp_base + 0x0C64, 0x3);
    outp32(asp_base + 0x0C68, 0x8);
    outp32(asp_base + 0x0C6c, 0x0);

    outp32(asp_base + 0x0C80, 0x194000);
    outp32(asp_base + 0x0C84, 0x3FF000);
    outp32(asp_base + 0x0C88, 0x0);
    outp32(asp_base + 0x0C90, 0x20);
    outp32(asp_base + 0x0C9c, 0xE);
    outp32(asp_base + 0x0Ca0, 0x3);
    outp32(asp_base + 0x0Ca4, 0x8);
    outp32(asp_base + 0x0Ca8, 0x0);

    outp32(asp_base + 0x0C14, 0xc);

    set_reg_dump(asp_base + 0x0C0C, 1, 1, 0);
}


void dss_latency_modem_init(void)
{
    set_reg_dump(noc_modem_base + 0x0C0C, 0, 1, 0);
    set_reg_dump(noc_modem_base + 0x0C08, 0, 1, 3);
    set_reg_dump(noc_modem_base + 0x0C08, 1, 1, 3);
    outp32(noc_modem_base + 0x0888, 0x1);

    outp32(noc_modem_base + 0x088C, 0x0);
    outp32(noc_modem_base + 0x0894, 0x20);
    outp32(noc_modem_base + 0x08A0, 0x3);
    outp32(noc_modem_base + 0x08A8, 0x0);
    outp32(noc_modem_base + 0x08B0, 0x0);

    outp32(noc_modem_base + 0x0878, 0x20); // for latency only************************************与文档不同

    outp32(noc_modem_base + 0x0808, 0x1);

    outp32(noc_modem_base + 0x080C, 0x0);// for latency only

    outp32(noc_modem_base + 0x080C, 0x0);
    outp32(noc_modem_base + 0x0830, 0x1);
    outp32(noc_modem_base + 0x0834, 0x2);
    outp32(noc_modem_base + 0x0838, 0x3);

    outp32(noc_modem_base + 0x0D38, 0x20);
    outp32(noc_modem_base + 0x0D4C, 0x21);
    outp32(noc_modem_base + 0x0D60, 0x22);
    outp32(noc_modem_base + 0x0D74, 0x23);
    outp32(noc_modem_base + 0x0D88, 0x24);

    outp32(noc_modem_base + 0x0C24, 0x0);

    outp32(noc_modem_base + 0x0D3C, 0x2);
    outp32(noc_modem_base + 0x0D50, 0x2);
    outp32(noc_modem_base + 0x0D64, 0x2);
    outp32(noc_modem_base + 0x0D78, 0x2);
    outp32(noc_modem_base + 0x0D8C, 0x2);

    outp32(noc_modem_base + 0x0C30, 0x1);
    outp32(noc_modem_base + 0x0C0C, 0x1);
}

void dss_latency_modem_stop(void)
{
    outp32(noc_modem_base + 0x0c28, 0x1);
}


void print_dss_latency_modem_info(void)
{
    cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1,
                        "%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t%08x\t",
                        /* DSS latency info */
                        readl(noc_modem_base + 0x0D40),
                        readl(noc_modem_base + 0x0D54),
                        readl(noc_modem_base + 0x0D68),
                        readl(noc_modem_base + 0x0D7C),
                        readl(noc_modem_base + 0x0D90),
                        readl(noc_modem_base + 0x0878),
                        readl(noc_modem_base + 0x086C));
}


void dss_latency_modem_start(void)
{
    outp32(noc_modem_base + 0x0C0C, 0x0);
    outp32(noc_modem_base + 0x0C38, 1);

    if (readl(noc_modem_base + 0x086C)) {
        outp32(noc_modem_base + 0x0870, 1);
    }

    set_reg_dump(noc_modem_base + 0x0C08, 0, 1, 3);
    set_reg_dump(noc_modem_base + 0x0C08, 1, 1, 3);
    outp32(noc_modem_base + 0x0C0C, 0x1);
}


struct hisi_bw_dump {
    int mask_bit;
    void (*hisi_module_dump_init)(void);
    void (*hisi_moudle_dump_stop)(void);
    /* hisi_moudle_dump_print should be end with "\t" to separate next moudle info */
    void (*hisi_moudle_dump_print)(void);
    void (*hisi_moudle_dump_start)(void);
};

static struct hisi_bw_dump hisi_module_map[MAX_MOUDLE_NUM] = {
    /* ddrc_bandwidth and ddrc_latency is mutex, choose only one */
    {1<<0, ddrc_bandwidth_init, NULL, print_DMC_info, NULL},
    {1<<1, ddrc_latency_init, NULL, print_ddrc_latency_info, NULL},

    {1<<2, Noc_dss_init, Noc_dss_stop, print_noc_dss_info, Noc_dss_start},
    {1<<3, Noc_vdec_init, Noc_vdec_stop, print_noc_vdec_info, Noc_vdec_start},
    {1<<4, dss_latency_osd_init, dss_latency_osd_stop,
            print_dss_latency_osd_info, dss_latency_osd_start},
    {1<<5, Noc_asp_init, Noc_asp_stop, print_noc_asp_info, Noc_asp_start},
    {1<<6, Noc_modem_init, Noc_asp_stop, print_noc_asp_info, Noc_asp_start},
    {1<<7, dss_latency_modem_init, dss_latency_modem_stop,
            print_dss_latency_modem_info, dss_latency_modem_start},
};

struct k3v3_bw_timer {
    spinlock_t                lock;
    void __iomem              *base;
    struct clk                *clk;
    struct clk                *pclk;
    int                       irq;
 };

/* local variables */
static struct k3v3_bw_timer bandwidth_timer;

/*local functions*/
static void k3v3_bw_timer_set_time(unsigned int time);
static void k3v3_bw_timer_init_config(void);
static int  k3v3_bw_timer_clk_get(void);
static int  k3v3_bw_timer_clk_enable(void);
static void k3v3_bw_timer_clk_disable(void);

static void k3v3_bw_timer_set_time(unsigned int time)
{
    unsigned long ctrl, flags;

    spin_lock_irqsave(&bandwidth_timer.lock, flags);

    ctrl = readl(bandwidth_timer.base + TIMER_CTRL);

    /*first disable timer*/
    ctrl &= (~TIMER_CTRL_ENABLE);
    writel(ctrl, bandwidth_timer.base + TIMER_CTRL);

    /*set TIME_LOAD register together*/
    writel(time, bandwidth_timer.base + TIMER_LOAD);

    writel(time, bandwidth_timer.base + TIMER_BGLOAD);

    /*then enable timer again*/
    ctrl |= (TIMER_CTRL_ENABLE | TIMER_CTRL_IE);

    writel(ctrl, bandwidth_timer.base + TIMER_CTRL);

    spin_unlock_irqrestore(&bandwidth_timer.lock, flags);
}

/*
*func: do timer initializtion
*
*description:
*1. set timer clk to 4.8MHz
*2. if mode is periodic and period is not the same as timer value, set bgload value
*3. set timer control: mode, 32bits, no prescale, interrupt enable, timer enable
*
*/
static void k3v3_bw_timer_init_config(void)
{
    unsigned long flags, ctrl = 0;
    //unsigned int reg = 0;
    int ret = 0;
    struct clk  *tempclk;

    spin_lock_irqsave(&bandwidth_timer.lock, flags);

    /*1. timer select 4.8M*/
    tempclk = clk_get_parent_by_index(bandwidth_timer.pclk, 1);
    if (IS_ERR_OR_NULL(tempclk)) {
        printk("fail to get parent clk. \n");
        return;
    }

    ret = clk_set_parent(bandwidth_timer.pclk, tempclk);
    if (ret < 0) {
    	pr_err("clk_set_parent :Failed to set parent!\n");
    	return;
    }

    /*2. clear the interrupt */
    writel(1, bandwidth_timer.base + TIMER_INTCLR);

    /*3. set timer2 control reg: 32bit, interrupt disable, timer_value, oneshot mode and disable wakeup_timer*/
    ctrl = TIMER_CTRL_32BIT;

    writel(BW_TIMER_DEFAULT_LOAD, bandwidth_timer.base + TIMER_LOAD);

    //ctrl |= TIMER_CTRL_ONESHOT;
    ctrl |= TIMER_CTRL_PERIODIC;

    writel(ctrl, bandwidth_timer.base + TIMER_CTRL);

    spin_unlock_irqrestore(&bandwidth_timer.lock, flags);
}

static irqreturn_t k3v3_bw_timer_interrupt(int irq, void *dev_id)
{
    int i;

    if ((readl(bandwidth_timer.base + TIMER_RIS)) & 0x1) {
        /* clear the interrupt */
        writel(1, bandwidth_timer.base + TIMER_INTCLR);

        do_gettimeofday(&hrtv);

        for (i=0; i<MAX_MOUDLE_NUM; i++) {
            if ((module_mask & (1<<i)) && (hisi_module_map[i].hisi_moudle_dump_stop))
                hisi_module_map[i].hisi_moudle_dump_stop();
        }

        cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1,
                    "%d.%06d\t",
                    (int)hrtv.tv_sec,(int)hrtv.tv_usec);

        for (i=0; i<MAX_MOUDLE_NUM; i++) {
            if ((module_mask & (1<<i)) && (hisi_module_map[i].hisi_moudle_dump_print))
                hisi_module_map[i].hisi_moudle_dump_print();
        }

        cur_pos += snprintf(file_addr + cur_pos, FILE_MAX_SIZE - cur_pos - 1, "\n");

        for (i=0; i<MAX_MOUDLE_NUM; i++) {
            if ((module_mask & (1<<i)) && (hisi_module_map[i].hisi_moudle_dump_start))
                hisi_module_map[i].hisi_moudle_dump_start();
        }
    }

    return IRQ_HANDLED;
}

/*
*func: used for wakeup on timer for S/R
*
*decs: set timer2 count value = (seconds*1000+milliseconds)*32.768 ms
*mode is used for user to set timer work in periodic or oneshot mode
*mode: 0 for periodic
*          1 for oneshot
*/
void k3v3_pm_bw_on_timer(unsigned int seconds, unsigned int useconds)
{
    unsigned int set_time;

    if (!seconds && !useconds) {
        printk("k3v3_pm_bw_on_timer: input time error!\n");
        return;
    }

    /*change time to milliseconds format*/
    set_time = 1000 * 1000 * seconds + useconds;
    if (BW_TIMER_MAX_TIMING < set_time) {
        printk("k3v3_pm_bw_on_timer: input timing overflow!\n");
        return;
    }

    /*enable clk*/
    if(k3v3_bw_timer_clk_enable()) {
        printk("k3v3_pm_bw_on_timer: clk enable error!\n");
        return;
    }

    /*add for the case ICS4.0 system changed the timer clk to 6.5MHz
    here changed back to 4.8MHz.
    */
    k3v3_bw_timer_init_config();
    k3v3_bw_timer_set_time(BW_TIMER_SET_VALUE(set_time));
}
EXPORT_SYMBOL(k3v3_pm_bw_on_timer);

void k3v3_bw_timer_disable(void)
{
    unsigned long ctrl, flags;

    spin_lock_irqsave(&bandwidth_timer.lock, flags);

    ctrl = readl(bandwidth_timer.base + TIMER_CTRL);

    ctrl &= ~(TIMER_CTRL_ENABLE | TIMER_CTRL_IE);

    writel(ctrl, bandwidth_timer.base + TIMER_CTRL);

    spin_unlock_irqrestore(&bandwidth_timer.lock, flags);

    /*disbale clk*/
    k3v3_bw_timer_clk_disable();
}
EXPORT_SYMBOL(k3v3_bw_timer_disable);

static int k3v3_bw_timer_clk_get(void)
{
    int ret = 0;

    /*get timer clk*/
    bandwidth_timer.clk = clk_get(NULL,TIMER_NAME);
    if (IS_ERR(bandwidth_timer.clk)) {
        printk("k3v2_wakeup_timer_clk_get: clk not found\n");
        ret = PTR_ERR(bandwidth_timer.clk);
        return ret;
    }

    bandwidth_timer.pclk = clk_get_parent(bandwidth_timer.clk);
    if (IS_ERR(bandwidth_timer.pclk)) {
        printk("k3v2_wakeup_timer_clk_get: pclk not found\n");
        ret = PTR_ERR(bandwidth_timer.pclk);
        return ret;
    }

    return ret;
}

static int k3v3_bw_timer_clk_enable(void)
{
    int ret = 0;

    ret = clk_prepare_enable(bandwidth_timer.clk);
    if (ret ) {
        printk("k3v2_wakeup_timer_clk_enable :clk prepare enable failed !\n");
    }

    return ret;
}

static void k3v3_bw_timer_clk_disable(void)
{
    clk_disable_unprepare(bandwidth_timer.clk);
}

static void k3v3_bw_timer_clk_put(void)
{
    clk_put(bandwidth_timer.clk);
}

static int k3v3_bw_timer_init(void)
{
    int ret = 0;

    if (bandwidth_timer.clk)
        return 0;

    if (k3v3_bw_timer_clk_get()) {
        printk("clk get failed\n");
        goto err_clk_get;
    }

    bandwidth_timer.base = ioremap(TIMER5_BASE, RES_SIZE);
    if (!bandwidth_timer.base) {
        ret = -ENOMEM;
        printk("ioremap fail\n");
        goto err_ioremap;
    }

    spin_lock_init(&bandwidth_timer.lock);

    if(k3v3_bw_timer_clk_enable()) {
        printk("clk enable failed\n");
        goto err_clk_enable;
    }

    bandwidth_timer.irq = TIMER5_IRQ;

    /*do timer init configs: disable timer ,mask interrupt, clear interrupt and set clk to 32.768KHz*/
    k3v3_bw_timer_init_config();

    /*register timer2 interrupt*/
    if(request_irq(bandwidth_timer.irq, k3v3_bw_timer_interrupt, IRQF_NO_SUSPEND, MODULE_NAME, NULL)) {
        printk("request irq for timer2 error\n");
        goto err_clk_enable;
    }

    irq_set_affinity(bandwidth_timer.irq, cpumask_of(IRQ_CPU_CORE));

    /*when init config finished, disable the clk and pclk for timer and enable them when needed.*/
    k3v3_bw_timer_clk_disable();

    printk("hisi bandwidth timer init has been finished\n");

    return 0;

err_clk_enable:
    k3v3_bw_timer_clk_disable();
    iounmap(bandwidth_timer.base);
err_ioremap:
    k3v3_bw_timer_clk_put();
err_clk_get:
    bandwidth_timer.clk = NULL;
    bandwidth_timer.pclk = NULL;
    //release_mem_region(res->start, resource_size(res));
    printk("hisi bandwidth timer init failed!\n");
    return ret;
}

static int k3v3_bw_timer_deinit(void)
{
    if (!bandwidth_timer.clk)
        return 0;

    iounmap(bandwidth_timer.base);
    k3v3_bw_timer_clk_disable();
    k3v3_bw_timer_clk_put();
    free_irq(bandwidth_timer.irq, NULL);
    bandwidth_timer.clk = NULL;
    bandwidth_timer.pclk = NULL;

    return 0;
}

int hisi_bw_dump_open(struct inode *node, struct file *fp)
{
    return 0;
}

ssize_t hisi_bw_dump_read(struct file *fp, char __user *ch, size_t count, loff_t *ppos)
{
    return 0;
}

ssize_t hisi_bw_dump_write(struct file *fp, const char __user *ch, size_t count, loff_t *ppos)
{
    char buf[32] = {0};
    int temp;
    static struct file* fd = NULL;
    static mm_segment_t oldfs;
    static struct timeval hrtv;
    loff_t pos = 0;
    ssize_t write_len = 0;
    char fileName[256] = {0};
    int time_tick_u = 0;
    int time_tick_s = 0;
    int i = 0;

    if (count >= sizeof(buf)) {
        return 0;
    }

    if (copy_from_user(buf, ch, count)) {
        return -EFAULT;
    }
	/*配置timer时间，启动计时，初始化需要记录的东东*/
    if (buf[0] == '0') {
        if (sscanf(buf, "%d %d %d", &temp, &time_tick_s, &time_tick_u) != 3) {
            printk("Invalid input\n");
            return -EINVAL;										
        }
        pr_info("time_tick: second = %d, usecond = %d\n", time_tick_s,time_tick_u);
        if (!time_tick_s && !time_tick_u) {
            printk("Invalid tick\n");
            return -EINVAL;
        }

        oldfs = get_fs();
        set_fs(KERNEL_DS);

        if (fd != NULL) {
            pr_info("last timer is undergoing, stop it first\n");
            set_fs(oldfs);
            return count;
        }

        do_gettimeofday(&hrtv);
        snprintf(fileName, sizeof(fileName), "/data/hwcdump/%d.txt",(int)hrtv.tv_sec);
        fd = filp_open(fileName, O_CREAT|O_RDWR, 0644);
        if (!fd || IS_ERR(fd)) {
            pr_info("open failed\n");
            set_fs(oldfs);
            fd = NULL;
            return count;
        }

        k3v3_bw_timer_init();
        k3v3_pm_bw_on_timer(time_tick_s, time_tick_u);

        for (i=0; i<MAX_MOUDLE_NUM; i++) {
            if ((module_mask & (1<<i)) && (hisi_module_map[i].hisi_module_dump_init))
                hisi_module_map[i].hisi_module_dump_init();
        }
    } else if (buf[0] == '1') {/*停止timer记录数据到文件*/
        k3v3_bw_timer_deinit();
        if (fd && !IS_ERR(fd) && (0 == write_len)) {
            write_len = vfs_write(fd,(char __user*)file_addr, cur_pos, &pos);
            filp_close(fd, NULL);							
            set_fs(oldfs);
            fd = NULL;
            cur_pos = 0;
        }
    } else if (buf[0] == '2') {/*配置初始化哪些模块需要测试*/
        if (sscanf(buf, "%d %d", &i, &temp) != 2) {
            printk("Invalid input\n");
            return -EINVAL;
        }

        if (temp == 0) {
            pr_info("Invalid module_mask\n");
            return -EINVAL;
        }

        module_mask = temp;												
        pr_info("set module_mask = 0x%x\n", module_mask);
    } else if (buf[0] == '3') {/*配置DDR需要检测的端口*/
        if (sscanf(buf, "%d %d", &i, &temp) != 2) {
            printk("Invalid input\n");
            return -EINVAL;
        }

        if (temp == 0) {
            pr_info("Invalid ddrc_port_monitor\n");
            return -EINVAL;
        }

        ddrc_port_monitor = temp;
        pr_info("set ddrc_port_monitor = 0x%x\n", ddrc_port_monitor);
    } else if (buf[0] == '4') {/*配置是latency还是outstandy统计0:outstanding 1:latency*/
        if (sscanf(buf, "%d %d", &i, &temp) != 2) {
            printk("Invalid input\n");
            return -EINVAL;
        }
        dss_latency = !!temp;
        pr_info("set dss_latency = 0x%x\n", dss_latency);
    } else {
        pr_info("unknow cmd: %s\n", buf);
    }

    return count;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = hisi_bw_dump_open,
	.read = hisi_bw_dump_read,
	.write = hisi_bw_dump_write,
};

int  hisi_bandwidth_dump_init(void)
{
    struct dentry * rootdir = debugfs_create_dir("hisi_bw_dump", NULL);
    debugfs_create_file("state", 0440, rootdir, NULL, &fops);

    file_addr = ioremap(FILE_DUMP_ADDR, FILE_MAX_SIZE);
    if (!file_addr) {
        printk("file_addr ioremap err\n");
        return -1;
    }

    ddrc_base = ioremap(0xfff20000, 0x10000);
    if (!ddrc_base) {
        printk("ddrc_base ioremap err\n");
        return -1;
    }

    noc_dss_base = ioremap(0xE85C0000, 0x10000);
    if (!noc_dss_base) {
        printk("noc_dss_base ioremap err\n");
        return -1;
    }

    vdec_base = ioremap(0xE8920000, 0x10000);
    if (!vdec_base) {
        printk("vdec_base ioremap err\n");
        return -1;
    }

    asp_base = ioremap(0xE8100000, 0x10000);
    if (!asp_base) {
        printk("asp_base ioremap err\n");
        return -1;
    }

    noc_modem_base = ioremap(0xE8080000, 0x10000);
    if (!noc_modem_base) {
        printk("noc_modem_base ioremap err\n");
        return -1;
    }
    return 0;
}

static void __exit hisi_bandwidth_dump_exit(void)
{
}

//module_init(hisi_bandwidth_dump_init);
module_exit(hisi_bandwidth_dump_exit);
#endif


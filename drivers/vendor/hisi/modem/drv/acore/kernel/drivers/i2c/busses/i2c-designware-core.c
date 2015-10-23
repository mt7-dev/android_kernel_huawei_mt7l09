/*
 * Synopsys DesignWare I2C adapter driver (master only).
 *
 * Based on the TI DAVINCI I2C adapter driver.
 *
 * Copyright (C) 2006 Texas Instruments.
 * Copyright (C) 2007 MontaVista Software Inc.
 * Copyright (C) 2009 Provigent Ltd.
 *
 * ----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 *
 */
/*lint --e{64,124,438,530,537,550,701,713,730,731,732,734,737,747,830,958} */
#include <linux/clk.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/pm_runtime.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <bsp_edma.h>
#include "i2c-designware-core.h"

/*
 * Registers offset
 */
#define DW_IC_CON		0x0
#define DW_IC_TAR		0x4
#define DW_IC_DATA_CMD		0x10
#define DW_IC_SS_SCL_HCNT	0x14
#define DW_IC_SS_SCL_LCNT	0x18
#define DW_IC_FS_SCL_HCNT	0x1c
#define DW_IC_FS_SCL_LCNT	0x20

#ifdef CONFIG_I2C_DESIGNWARE_HI6930
#define DW_IC_HS_SCL_HCNT	0x24
#define DW_IC_HS_SCL_LCNT	0x28
#define IC_SDA_HOLD             0x7C
#define IC_FS_SPKLEN            0xA0
#define IC_HS_SPKLEN            0xA4
#endif

#define DW_IC_INTR_STAT		0x2c
#define DW_IC_INTR_MASK		0x30
#define DW_IC_RAW_INTR_STAT	0x34
#define DW_IC_RX_TL		0x38
#define DW_IC_TX_TL		0x3c
#define DW_IC_CLR_INTR		0x40
#define DW_IC_CLR_RX_UNDER	0x44
#define DW_IC_CLR_RX_OVER	0x48
#define DW_IC_CLR_TX_OVER	0x4c
#define DW_IC_CLR_RD_REQ	0x50
#define DW_IC_CLR_TX_ABRT	0x54
#define DW_IC_CLR_RX_DONE	0x58
#define DW_IC_CLR_ACTIVITY	0x5c
#define DW_IC_CLR_STOP_DET	0x60
#define DW_IC_CLR_START_DET	0x64
#define DW_IC_CLR_GEN_CALL	0x68
#define DW_IC_ENABLE		0x6c
#define DW_IC_STATUS		0x70
#define DW_IC_TXFLR		0x74
#define DW_IC_RXFLR		0x78
#define DW_IC_TX_ABRT_SOURCE	0x80
#define DW_IC_ENABLE_STATUS	0x9c

#define DW_IC_COMP_TYPE		0xfc

#define DW_IC_DMA_CR	0x88
#define DW_IC_DMA_TDLR	0x8c
#define DW_IC_DMA_RDLR	0x90
#define DW_IC_COMP_TYPE_VALUE	0x44570140

#define DW_IC_INTR_RX_UNDER	0x001
#define DW_IC_INTR_RX_OVER	0x002
#define DW_IC_INTR_RX_FULL	0x004
#define DW_IC_INTR_TX_OVER	0x008
#define DW_IC_INTR_TX_EMPTY	0x010
#define DW_IC_INTR_RD_REQ	0x020
#define DW_IC_INTR_TX_ABRT	0x040
#define DW_IC_INTR_RX_DONE	0x080
#define DW_IC_INTR_ACTIVITY	0x100
#define DW_IC_INTR_STOP_DET	0x200
#define DW_IC_INTR_START_DET	0x400
#define DW_IC_INTR_GEN_CALL	0x800

#ifdef CONFIG_I2C_DESIGNWARE_HI6930
#define DW_IC_DATACMD_READ_FLAG         0x100
#define DW_IC_DATACMD_STOP_FLAG         0x200
#define DW_IC_DATACMD_RESTART_FLAG      0x400
#define RX_TX_LAST_BYTE                 0x1
#define I2C_DELAY_70NS                  70
#define I2C_DELAY_300NS                 300
#endif

#define DW_IC_TXDMAE		(1 << 1)	/* enable transmit dma */
#define DW_IC_RXDMAE		(1 << 0)	/* enable receive dma */
#define DW_IC_INTR_DEFAULT_MASK		(DW_IC_INTR_RX_FULL | \
					 DW_IC_INTR_TX_EMPTY | \
					 DW_IC_INTR_TX_ABRT | \
					 DW_IC_INTR_STOP_DET)

#define DW_IC_STATUS_ACTIVITY	0x1

#define DW_IC_ERR_TX_ABRT	0x1

#define DW_IC_IC_EN		0x1

/*
 * status codes
 */
#define STATUS_IDLE			0x0
#define STATUS_WRITE_IN_PROGRESS	0x1
#define STATUS_READ_IN_PROGRESS		0x2

#define TIMEOUT			20 /* ms */

#define WAIT_FOR_COMPLETION		((HZ*totallen)/400+HZ)   /* wait 1s add 2.5ms every byte. */

#define I2C_POLL_DISABLE		40   /* this value must be 10 times the signaling
					      * period for the highest I2C transfer speed, if the
					      * highest I2C transfer mode is 400 kb/s, then this
					      * ti2c_poll is 25us. */
#define MAX_T_POLL_COUNT		100
/*
 * hardware abort codes from the DW_IC_TX_ABRT_SOURCE register
 *
 * only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK	0
#define ABRT_10ADDR1_NOACK	1
#define ABRT_10ADDR2_NOACK	2
#define ABRT_TXDATA_NOACK	3
#define ABRT_GCALL_NOACK	4
#define ABRT_GCALL_READ		5
#define ABRT_HS_ACKDET		6  //
#define ABRT_SBYTE_ACKDET	7
#define ABRT_HS_NORSTRT		8  //
#define ABRT_SBYTE_NORSTRT	9
#define ABRT_10B_RD_NORSTRT	10
#define ABRT_MASTER_DIS		11
#define ARB_LOST		12
#define ABRT_USER_ABRT		16 //

#define DW_IC_TX_ABRT_7B_ADDR_NOACK	(1UL << ABRT_7B_ADDR_NOACK)
#define DW_IC_TX_ABRT_10ADDR1_NOACK	(1UL << ABRT_10ADDR1_NOACK)
#define DW_IC_TX_ABRT_10ADDR2_NOACK	(1UL << ABRT_10ADDR2_NOACK)
#define DW_IC_TX_ABRT_TXDATA_NOACK	(1UL << ABRT_TXDATA_NOACK)
#define DW_IC_TX_ABRT_GCALL_NOACK	(1UL << ABRT_GCALL_NOACK)
#define DW_IC_TX_ABRT_GCALL_READ	(1UL << ABRT_GCALL_READ)
#define DW_IC_TX_ABRT_SBYTE_ACKDET	(1UL << ABRT_SBYTE_ACKDET)
#define DW_IC_TX_ABRT_SBYTE_NORSTRT	(1UL << ABRT_SBYTE_NORSTRT)
#define DW_IC_TX_ABRT_10B_RD_NORSTRT	(1UL << ABRT_10B_RD_NORSTRT)
#define DW_IC_TX_ABRT_MASTER_DIS	(1UL << ABRT_MASTER_DIS)
#define DW_IC_TX_ARB_LOST		(1UL << ARB_LOST)

#define DW_IC_TX_ABRT_NOACK		(DW_IC_TX_ABRT_7B_ADDR_NOACK | \
					 DW_IC_TX_ABRT_10ADDR1_NOACK | \
					 DW_IC_TX_ABRT_10ADDR2_NOACK | \
					 DW_IC_TX_ABRT_TXDATA_NOACK | \
					 DW_IC_TX_ABRT_GCALL_NOACK)

#define I2C_DW_MAX_DMA_BUF_LEN          (60*1024)

#ifndef CONFIG_I2C_DESIGNWARE_HI6930
static char *abort_sources[] = {
	[ABRT_7B_ADDR_NOACK] =
		"slave address not acknowledged (7bit mode)",
	[ABRT_10ADDR1_NOACK] =
		"first address byte not acknowledged (10bit mode)",
	[ABRT_10ADDR2_NOACK] =
		"second address byte not acknowledged (10bit mode)",
	[ABRT_TXDATA_NOACK] =
		"data not acknowledged",
	[ABRT_GCALL_NOACK] =
		"no acknowledgement for a general call",
	[ABRT_GCALL_READ] =
		"read after general call",
	[ABRT_SBYTE_ACKDET] =
		"start byte acknowledged",
	[ABRT_SBYTE_NORSTRT] =
		"trying to send start byte when restart is disabled",
	[ABRT_10B_RD_NORSTRT] =
		"trying to read when restart is disabled (10bit mode)",
	[ABRT_MASTER_DIS] =
		"trying to use disabled adapter",
	[ARB_LOST] =
	"lost arbitration",
	[ABRT_USER_ABRT] =
	"master has detected the transfer abort(IC_ENABLE(1))",
};
#else
static char *abort_sources[] = {
	[ABRT_7B_ADDR_NOACK] =
	"slave address not acknowledged (7bit mode)",
	[ABRT_10ADDR1_NOACK] =
	"first address byte not acknowledged (10bit mode)",
	[ABRT_10ADDR2_NOACK] =
	"second address byte not acknowledged (10bit mode)",
	[ABRT_TXDATA_NOACK] =
	"data not acknowledged",
	[ABRT_GCALL_NOACK] =
	"no acknowledgement for a general call",
	[ABRT_GCALL_READ] =
	"read after general call",
	[ABRT_HS_ACKDET] =
	"the high-speed master code was achnowledged",
	[ABRT_SBYTE_ACKDET] =
	"start byte acknowledged",
	[ABRT_HS_NORSTRT] =
	"the restart is disabled and the user is trying to use the master to transfer data in high-speed mode",
	[ABRT_SBYTE_NORSTRT] =
	"trying to send start byte when restart is disabled",
	[ABRT_10B_RD_NORSTRT] =
	"trying to read when restart is disabled (10bit mode)",
	[ABRT_MASTER_DIS] =
	"trying to use disabled adapter",
	[ARB_LOST] =
	"lost arbitration",
	[ABRT_USER_ABRT] =
	"master has detected the transfer abort(IC_ENABLE(1))",
};
#endif


static int i2c_dw_xfer_msg_dma(struct dw_i2c_dev *dev, int *alllen);
void i2c_dw_dma_tx_complete(u32 data,u32 int_flag);
void i2c_dw_dma_rx_complete(u32 data,u32 int_flag);
#define HI_I2c0_DATA_ADDR             0x90023000
#define EDMA_I2C0_TX             24
#define EDMA_I2C0_RX             25

static int i2c_dw_dma_tx(struct dw_i2c_dev *dev)
{
    struct dw_i2c_dma_data *dmatx = &dev->dmatx;
    s32   ret = 0;
    dmatx->buf = devm_kzalloc(dev->dev, dmatx->length, GFP_KERNEL);
    if (!dmatx->buf)
    {
        dev_err(dev->dev, "%s: no memory for DMA buffer, length: %lu\n",
        		__func__, dmatx->length);
        return -ENOMEM;
    }
    dmatx->dma_mapaddr = dma_map_single(dev->dev, (void *)dmatx->buf, dmatx->length,DMA_TO_DEVICE);

    dmatx->dma_channel = bsp_edma_channel_init(EDMA_I2C0_TX,i2c_dw_dma_tx_complete,dev,EDMA_INT_DONE|EDMA_INT_TRANSFER_ERR|EDMA_INT_CONFIG_ERR|EDMA_INT_READ_ERR);
    if(dmatx->dma_channel<0)
    {
        dev_warn(dev->dev, "Dma dma_channel failed.\n");
        return ENOMEM;
    }
    bsp_edma_channel_set_config (dmatx->dma_channel, EDMA_M2P, EDMA_TRANS_WIDTH_16, EDMA_BUR_LEN_16);

    ret = bsp_edma_channel_async_start(dmatx->dma_channel, dmatx->dma_mapaddr, HI_I2c0_DATA_ADDR, dmatx->length);

    if(ret)
    {
        dev_warn(dev->dev, "Dma bsp_edma_channel_async_start failed.\n");
        return ret;
    }
    return ret;
}

void i2c_dw_dma_tx_complete(u32 data,u32 int_flag)
{
    struct dw_i2c_dev *dev = data;
    dev_dbg(dev->dev, "%s: entry.\n", __func__);

    if(int_flag&EDMA_INT_DONE)
    {
        dev->dmacr &= ~DW_IC_TXDMAE;
        writew(dev->dmacr, dev->base + DW_IC_DMA_CR);
        dev->using_tx_dma = false;
        dev_warn(dev->dev, "Dma i2c_dw_dma_tx_complete success.\n");
        if(!(dev->using_tx_dma) && !(dev->using_rx_dma))
        {
            complete(&dev->dma_complete);
        }
    }
    else
    {
        dev_warn(dev->dev, "Dma i2c_dw_dma_tx_complete failed.\n");
    }

}

/*
 * Returns:
 *	1 if we queued up a RX DMA buffer.
 *	0 if we didn't want to handle this by DMA
 */
static int i2c_dw_dma_rx(struct dw_i2c_dev *dev)
{
	struct dw_i2c_dma_data *dmarx = &dev->dmarx;
    s32   ret = 0;

	dev_warn(dev->dev, "i2c_dw_dma_rx_trigger_dma, %d bytes to read\n",
			dmarx->length);

    dmarx->buf = devm_kzalloc(dev->dev, dmarx->length, GFP_KERNEL);
	if (!dmarx->buf) {
		dev_err(dev->dev, "%s: no memory for DMA buffer, length: %lu\n",
				__func__, dmarx->length);
		return -ENOMEM;
	}
    dmarx->dma_mapaddr = dma_map_single(dev->dev, (void *)dmarx->buf, dmarx->length,DMA_FROM_DEVICE);

    dmarx->dma_channel = bsp_edma_channel_init(EDMA_I2C0_RX,i2c_dw_dma_rx_complete,dev,EDMA_INT_DONE|EDMA_INT_TRANSFER_ERR|EDMA_INT_CONFIG_ERR|EDMA_INT_READ_ERR);
    if(dmarx->dma_channel<0)
    {
        dev_warn(dev->dev, "Dma dma_channel rx failed.\n");
        return ENOMEM;
    }
    bsp_edma_channel_set_config (dmarx->dma_channel, EDMA_P2M, EDMA_TRANS_WIDTH_8, EDMA_BUR_LEN_16);

    ret = bsp_edma_channel_async_start(dmarx->dma_channel, dmarx->dma_mapaddr, HI_I2c0_DATA_ADDR, dmarx->length);

    if(ret)
    {
        dev_warn(dev->dev, "Dma bsp_edma_channel_async_start rx failed.\n");
        return ret;
    }
	return ret;
}

void i2c_dw_dma_rx_complete(u32 data,u32 int_flag)
{
    struct dw_i2c_dev *dev = data;
    struct i2c_msg *msgs = dev->msgs;
    struct dw_i2c_dma_data *dmarx = &dev->dmarx;
    int rx_valid = dmarx->length;
    int rd_idx = 0;
    u32 len;
    u8 *buf;

    dev_dbg(dev->dev, "%s: entry.\n", __func__);
    if(int_flag&EDMA_INT_DONE)
    {
    	dev->dmacr &= ~DW_IC_RXDMAE;
    	writew(dev->dmacr, dev->base + DW_IC_DMA_CR);

    	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++)
        {
    		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD))
    			continue;

    		len = msgs[dev->msg_read_idx].len;
    		buf = msgs[dev->msg_read_idx].buf;

    		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
    			*buf++ = dmarx->buf[rd_idx++];
    		}

    	}
    	dev->using_rx_dma = false;
    	if(!(dev->using_tx_dma) && !(dev->using_rx_dma))
    	{
    		complete(&dev->dma_complete);
    	}
    }
    else
    {
        dev_warn(dev->dev, "Dma i2c_dw_dma_rx_complete failed.\n");
    }
}


static void i2c_dw_dma_clear(struct dw_i2c_dev *dev)
{
	if(dev->dmatx.buf) {

		dma_unmap_single(dev->dev, (void *)dev->dmatx.buf, dev->dmatx.length,DMA_TO_DEVICE);
        devm_kfree(dev->dev,dev->dmatx.buf);
		dev->dmatx.buf = NULL;
		dev->using_tx_dma = false;
	}
	if(dev->dmarx.buf) {
		dma_unmap_single(dev->dev, (void *)dev->dmarx.buf, dev->dmarx.length,DMA_FROM_DEVICE);
        devm_kfree(dev->dev,dev->dmarx.buf);
		dev->dmarx.buf = NULL;
		dev->using_rx_dma = false;
	}

	dev->using_dma = false;
	dev->dmacr = 0;
	writew(dev->dmacr, dev->base + DW_IC_DMA_CR);
}


u32 dw_readl(struct dw_i2c_dev *dev, int offset)
{
	u32 value;

	if (dev->accessor_flags & ACCESS_16BIT)
		value = readw(dev->base + offset) |
			(readw(dev->base + offset + 2) << 16);
	else
		value = readl(dev->base + offset);

	if (dev->accessor_flags & ACCESS_SWAP)
		return swab32(value);
	else
		return value;
}

void dw_writel(struct dw_i2c_dev *dev, u32 b, int offset)
{
	if (dev->accessor_flags & ACCESS_SWAP)
		b = swab32(b);

	if (dev->accessor_flags & ACCESS_16BIT) {
		writew((u16)b, dev->base + offset);
		writew((u16)(b >> 16), dev->base + offset + 2);
	} else {
		writel(b, dev->base + offset);
	}
}

static u32
i2c_dw_scl_hcnt(u32 ic_clk, u32 tSYMBOL, u32 tf, int cond, int offset)
{
	/*
	 * DesignWare I2C core doesn't seem to have solid strategy to meet
	 * the tHD;STA timing spec.  Configuring _HCNT based on tHIGH spec
	 * will result in violation of the tHD;STA spec.
	 */
	if (cond)
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		 *
		 * This is based on the DW manuals, and represents an ideal
		 * configuration.  The resulting I2C bus speed will be
		 * faster than any of the others.
		 *
		 * If your hardware is free from tHD;STA issue, try this one.
		 */
		return (ic_clk * tSYMBOL + 5000) / 10000 - 8 + offset;
	else
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		 *
		 * This is just experimental rule; the tHD;STA period turned
		 * out to be proportinal to (_HCNT + 3).  With this setting,
		 * we could meet both tHIGH and tHD;STA timing specs.
		 *
		 * If unsure, you'd better to take this alternative.
		 *
		 * The reason why we need to take into account "tf" here,
		 * is the same as described in i2c_dw_scl_lcnt().
		 */
		return (ic_clk * (tSYMBOL + tf) + 5000) / 10000 - 3 + offset;
}

static u32 i2c_dw_scl_lcnt(u32 ic_clk, u32 tLOW, u32 tf, int offset)
{
	/*
	 * Conditional expression:
	 *
	 *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	 *
	 * DW I2C core starts counting the SCL CNTs for the LOW period
	 * of the SCL clock (tLOW) as soon as it pulls the SCL line.
	 * In order to meet the tLOW timing spec, we need to take into
	 * account the fall time of SCL signal (tf).  Default tf value
	 * should be 0.3 us, for safety.
	 */
	return ((ic_clk * (tLOW + tf) + 5000) / 10000) - 1 + offset;
}

static void __i2c_dw_enable(struct dw_i2c_dev *dev, bool enable)
{
	int timeout = 100;
	do{
        dw_writel(dev, enable, DW_IC_ENABLE);
        if ((dw_readl(dev, DW_IC_ENABLE_STATUS) & 1) == enable)
        	return;

        /*
         * Wait 10 times the signaling period of the highest I2C
         * transfer supported by the driver (for 400KHz this is
         * 25us) as described in the DesignWare I2C databook.
         */
        usleep_range(25, 250);
	} while (timeout--);

	dev_warn(dev->dev, "timeout in %sabling adapter\n",
		 enable ? "en" : "dis");
}

/**
 * i2c_dw_init() - initialize the designware i2c master hardware
 * @dev: device private data
 *
 * This functions configures and enables the I2C master.
 * This function is called during I2C init function, and in case of timeout at
 * run time.
 */
int i2c_dw_init(struct dw_i2c_dev *dev)
{
	u32 input_clock_khz;
	u32 hcnt, lcnt;
	u32 reg;
#ifdef CONFIG_I2C_DESIGNWARE_HI6930
	u32 sda_delay_count;
#endif
	input_clock_khz = dev->get_clk_rate_khz(dev);

	if (!dev->accessor_flags) {
		reg = dw_readl(dev, DW_IC_COMP_TYPE);
		if (reg == ___constant_swab32(DW_IC_COMP_TYPE_VALUE)) {
			/* Configure register endianess access */
			dev->accessor_flags |= ACCESS_SWAP;
		} else if (reg == (DW_IC_COMP_TYPE_VALUE & 0x0000ffff)) {
			/* Configure register access mode 16bit */
			dev->accessor_flags |= ACCESS_16BIT;
		} else if (reg != DW_IC_COMP_TYPE_VALUE) {
			dev_err(dev->dev, "Unknown Synopsys component type: "
					"0x%08x\n", reg);
			return -ENODEV;
		}
	}

	/* Disable the adapter */
	__i2c_dw_enable(dev, false);

	/* set standard and fast speed deviders for high/low periods */

	/* Standard-mode */
	hcnt = i2c_dw_scl_hcnt(input_clock_khz,
				40,	/* tHD;STA = tHIGH = 4.0 us */
				3,	/* tf = 0.3 us */
				0,	/* 0: DW default, 1: Ideal */
				0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(input_clock_khz,
				47,	/* tLOW = 4.7 us */
				3,	/* tf = 0.3 us */
				0);	/* No offset */
	dw_writel(dev, hcnt, DW_IC_SS_SCL_HCNT);
	dw_writel(dev, lcnt, DW_IC_SS_SCL_LCNT);
	dev_dbg(dev->dev, "Standard-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	/* Fast-mode */
	hcnt = i2c_dw_scl_hcnt(input_clock_khz,
				6,	/* tHD;STA = tHIGH = 0.6 us */
				3,	/* tf = 0.3 us */
				0,	/* 0: DW default, 1: Ideal */
				0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(input_clock_khz,
				13,	/* tLOW = 1.3 us */
				3,	/* tf = 0.3 us */
				0);	/* No offset */
	dw_writel(dev, hcnt, DW_IC_FS_SCL_HCNT);
	dw_writel(dev, lcnt, DW_IC_FS_SCL_LCNT);
	dev_dbg(dev->dev, "Fast-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	/* High-mode */
	hcnt = i2c_dw_scl_hcnt(input_clock_khz,
						   1,	/* tHD;STA = tHIGH = 0.1 us */
						   3,	/* tf = 0.3 us */
						   0,	/* 0: DW default, 1: Ideal */
						   0);	/* No offset */
	lcnt = i2c_dw_scl_lcnt(input_clock_khz,
						   2,	/* tLOW = 0.2 us */
						   3,	/* tf = 0.3 us */
						   0);	/* No offset */
	dw_writel(dev, hcnt, DW_IC_HS_SCL_HCNT);
	dw_writel(dev, lcnt, DW_IC_HS_SCL_LCNT);
	dev_dbg(dev->dev, "high-mode HCNT:LCNT = %d:%d\n", hcnt, lcnt);

	/* Spike Suppression*/
	dw_writel(dev, 1, IC_FS_SPKLEN);
	dw_writel(dev, 1, IC_HS_SPKLEN);
#ifdef CONFIG_I2C_DESIGNWARE_HI6930
	/* SDA HOLD TIME */
	if(I2C_DELAY_70NS == dev->delay_off) {
		sda_delay_count = (input_clock_khz * 70)/1000000;
		dw_writel(dev, sda_delay_count, IC_SDA_HOLD);
	} else {
		sda_delay_count = (input_clock_khz * 300)/1000000;
		dw_writel(dev, sda_delay_count, IC_SDA_HOLD);
	}
#endif
	/* Configure Tx/Rx FIFO threshold levels */
	dw_writel(dev, dev->tx_fifo_depth - 1, DW_IC_TX_TL);
	dw_writel(dev, 16, DW_IC_RX_TL);

	dw_writel(dev, dev->tx_fifo_depth - 16, DW_IC_DMA_TDLR);
	dw_writel(dev, 15, DW_IC_DMA_RDLR);
	/* configure the i2c master */
	dw_writel(dev, dev->master_cfg , DW_IC_CON);
	return 0;
}
EXPORT_SYMBOL_GPL(i2c_dw_init);

#ifdef CONFIG_I2C_HI6930_LOWPOWER
int devm_pinctrl_state_select(struct dw_i2c_dev *dev,const char *name)
{
	struct pinctrl_state *s;
	int ret;
	if(0 == dev->pinctrl_flag) {
		dev->pinctrl= devm_pinctrl_get(dev->platform_dev);
		if (IS_ERR(dev->pinctrl))
			return -1;
		dev->pinctrl_flag = 1;
	}

	s = pinctrl_lookup_state(dev->pinctrl, name);
	if (IS_ERR(s)) {
		devm_pinctrl_put(dev->pinctrl);
		dev->pinctrl_flag = 0;
		return -1;
	}

	ret = pinctrl_select_state(dev->pinctrl, s);
	if (ret < 0) {
		devm_pinctrl_put(dev->pinctrl);
		dev->pinctrl_flag = 0;
		return -1;
	}
	return 0;
}
#endif

#ifdef CONFIG_I2C_DESIGNWARE_HI6930
/* print reg*/
void i2c_print_controller_reg(struct dw_i2c_dev *dev)
{
	u32 i;
	dev_err(dev->dev, "I2C DUMP REGISTER\n");
	for(i = 0; i <= DW_IC_TX_ABRT_SOURCE; i += 0x10) {
		/* can't dump  DW_IC_DATA_CMD */
		if(i != DW_IC_DATA_CMD) {
			printk(KERN_ERR "%08x: %08x, %08x, %08x, %08x\n", (unsigned)(dev->mapbase + i), readl(dev->base + i),
				   readl(dev->base + i + 4), readl(dev->base + i + 8), readl(dev->base + i + 12));
		} else {
			printk(KERN_ERR "%08x: ........, %08x, %08x, %08x\n", (unsigned)(dev->mapbase + i),
				   readl(dev->base + i + 4), readl(dev->base + i + 8), readl(dev->base + i + 12));
		}
	}
}
#endif

/* reset i2c controller */
static void reset_i2c_controller(struct dw_i2c_dev *dev)
{
#ifdef CONFIG_ARCH_HI3630
	int r;
	r = devm_pinctrl_state_select(dev, PINCTRL_STATE_IDLE);
	if (r<0)
		dev_warn(dev->platform_dev,
				 "pins are not configured from the driver\n");
#endif

	if(dw_readl(dev,  DW_IC_ENABLE_STATUS)&DW_IC_IC_EN)
        /* ABORT the adapter */
	    dw_writel(dev, 0x03, DW_IC_ENABLE);

	if(dev && dev->reset_controller)
		dev->reset_controller(dev);

	i2c_dw_init(dev);

	i2c_dw_disable_int(dev);

#ifdef CONFIG_ARCH_HI3630
	r = devm_pinctrl_state_select(dev, PINCTRL_STATE_DEFAULT);
	if (r<0)
		dev_warn(dev->platform_dev,
				 "pins are not configured from the driver\n");
#endif
}

/*
 * Waiting for bus not busy
 */
static int i2c_dw_wait_bus_not_busy(struct dw_i2c_dev *dev)
{
	int timeout = TIMEOUT;

	while (dw_readl(dev, DW_IC_STATUS) & DW_IC_STATUS_ACTIVITY) {
		if (timeout <= 0) {
			dev_warn(dev->dev, "timeout waiting for bus ready\n");
			reset_i2c_controller(dev);
			if(readl(dev->base + DW_IC_STATUS) &
			   DW_IC_STATUS_ACTIVITY) {
				dev_err(dev->dev, "controller failed after reset.\n");
				return -EAGAIN;
			} else {
				return 0;
			}
		}
		timeout--;
		usleep_range(1000, 1100);
	}

	return 0;
}

static void i2c_dw_xfer_init(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 ic_con;
	int retry_num = MAX_T_POLL_COUNT;

	/* Disable the adapter */
	dw_writel(dev, 0, DW_IC_ENABLE);
	while((--retry_num != 0) && (dw_readl(dev,  DW_IC_ENABLE_STATUS)&DW_IC_IC_EN)) {
		udelay(I2C_POLL_DISABLE);
	}

	if(retry_num <= 0) {
		dev_err(dev->dev, "disable i2c controller error.\n");
		reset_i2c_controller(dev);
	}

	/* set the slave (target) address */
	dw_writel(dev, msgs[dev->msg_write_idx].addr, DW_IC_TAR);

	/* if the slave address is ten bit address, enable 10BITADDR */
	ic_con = dw_readl(dev, DW_IC_CON);
	if (msgs[dev->msg_write_idx].flags & I2C_M_TEN)
		ic_con |= DW_IC_CON_10BITADDR_MASTER;
	else
		ic_con &= ~DW_IC_CON_10BITADDR_MASTER;
	dw_writel(dev, ic_con, DW_IC_CON);

	/* Enable the adapter */
	__i2c_dw_enable(dev, true);

	/* Clear and enable interrupts */
	i2c_dw_clear_int(dev);

	/* Enable interrupts */
	dw_writel(dev, DW_IC_INTR_TX_ABRT | DW_IC_INTR_STOP_DET, DW_IC_INTR_MASK);
}

/*
 * Initiate (and continue) low level master read/write transaction.
 * This function is only called from i2c_dw_isr, and pumping i2c_msg
 * messages into the tx buffer.  Even if the size of i2c_msg data is
 * longer than the size of the tx buffer, it handles everything.
 */
static void
i2c_dw_xfer_msg(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 intr_mask;
	int tx_limit, rx_limit;
	u32 addr = msgs[dev->msg_write_idx].addr;
	u32 buf_len = dev->tx_buf_len;
	u8 *buf = dev->tx_buf;

	intr_mask = DW_IC_INTR_DEFAULT_MASK;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		/*
		 * if target address has changed, we need to
		 * reprogram the target address in the i2c
		 * adapter when we are done with this transfer
		 */
		if (msgs[dev->msg_write_idx].addr != addr) {
			dev_err(dev->dev,
				"%s: invalid target address\n", __func__);
			dev->msg_err = -EINVAL;
			break;
		}

		if (msgs[dev->msg_write_idx].len == 0) {
			dev_err(dev->dev,
				"%s: invalid message length\n", __func__);
			dev->msg_err = -EINVAL;
			break;
		}

		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			/* new i2c_msg */
			buf = msgs[dev->msg_write_idx].buf;
			buf_len = msgs[dev->msg_write_idx].len;
		}

		tx_limit = dev->tx_fifo_depth - dw_readl(dev, DW_IC_TXFLR);
		rx_limit = dev->rx_fifo_depth - dw_readl(dev, DW_IC_RXFLR);

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			u32 cmd = 0;

			/*
			 * If IC_EMPTYFIFO_HOLD_MASTER_EN is set we must
			 * manually set the stop bit. However, it cannot be
			 * detected from the registers so we set it always
			 * when writing/reading the last byte.
			 */
			if (dev->msg_write_idx == dev->msgs_num - 1 &&
			    buf_len == 1)
				cmd |= BIT(9);

			if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {

				/* avoid rx buffer overrun */
				if (rx_limit - dev->rx_outstanding <= 0)
					break;
#ifdef CONFIG_I2C_DESIGNWARE_HI6930
				dw_writel(dev, cmd | 0x100, DW_IC_DATA_CMD);
#else
				dw_writel(dev, 0x100, DW_IC_DATA_CMD);
#endif
				rx_limit--;
				dev->rx_outstanding++;

			} else {
#ifdef CONFIG_I2C_DESIGNWARE_HI6930
				dw_writel(dev, cmd | *buf++, DW_IC_DATA_CMD);
#else
				dw_writel(dev, *buf++, DW_IC_DATA_CMD);
#endif
			}
			tx_limit--;
			buf_len--;
		}

		dev->tx_buf = buf;
		dev->tx_buf_len = buf_len;

		if (buf_len > 0) {
			/* more bytes to be written */
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			dev_dbg(dev->dev,
				"%s: buffer length is over fifo. \n", __func__);
			break;
		} else
			dev->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	/*
	 * If i2c_msg index search is completed, we don't need TX_EMPTY
	 * interrupt any more.
	 */
	if (dev->msg_write_idx == dev->msgs_num)
		intr_mask &= ~DW_IC_INTR_TX_EMPTY;

	if (dev->msg_err)
		intr_mask = 0;

	dw_writel(dev, intr_mask,  DW_IC_INTR_MASK);
}

static void
i2c_dw_read(struct dw_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	int rx_valid;

	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		u32 len;
		u8 *buf;

		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD))
			continue;

		if (!(dev->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dev->msg_read_idx].len;
			buf = msgs[dev->msg_read_idx].buf;
		} else {
			len = dev->rx_buf_len;
			buf = dev->rx_buf;
		}

		rx_valid = dw_readl(dev, DW_IC_RXFLR);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			*buf++ = dw_readl(dev, DW_IC_DATA_CMD);
			dev->rx_outstanding--;
		}

		if (len > 0) {
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		} else
			dev->status &= ~STATUS_READ_IN_PROGRESS;
	}
}


static int i2c_dw_handle_tx_abort(struct dw_i2c_dev *dev)
{
	unsigned long abort_source = dev->abort_source;
	int i;

	if (abort_source & DW_IC_TX_ABRT_NOACK) {
		for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
			dev_dbg(dev->dev,
				"%s: %s\n", __func__, abort_sources[i]);
		return -EREMOTEIO;
	}

	for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
		dev_err(dev->dev, "%s: %s\n", __func__, abort_sources[i]);

	if (abort_source & DW_IC_TX_ARB_LOST)
		return -EAGAIN;
	else if (abort_source & DW_IC_TX_ABRT_GCALL_READ)
		return -EINVAL; /* wrong msgs[] data */
	else
		return -EIO;
}

static int i2c_dw_xfer_msg_dma(struct dw_i2c_dev *dev, int *alllen)
{
	struct i2c_msg *msgs = dev->msgs;
	int rx_len = 0;
	int tx_len;
	int i;
	int total_len = 0;
	u32 buf_len;
	u8 *buf = dev->tx_buf;
	u16 *dma_txbuf;
	int ret = -EPERM;

	if (!dev->support_dma) {
		return -EPERM;

	}

	/* If total date length less than a fifodepth, not use DMA */
	for (i = dev->msg_write_idx; i < dev->msgs_num; i++)
		total_len += dev->msgs[i].len;

	dev_dbg(dev->dev, "%s: msg num: %d, total length: %d\n",
			__func__, dev->msgs_num, total_len);

	*alllen = total_len;

	if (total_len < dev->tx_fifo_depth)
		return -EPERM;

	tx_len = total_len * sizeof(unsigned short);

	if (tx_len > I2C_DW_MAX_DMA_BUF_LEN) {
		dev_err(dev->dev, "Too long to send with DMA: %d\n", tx_len);
		dev->msg_err = -EINVAL;
		return -EPERM;
	}

	dev_dbg(dev->dev, "use DMA transfer, len=%d\n", tx_len);

	INIT_COMPLETION(dev->dma_complete);
	dev->using_dma = true;
	dev->using_tx_dma = true;
    if (i2c_dw_dma_tx(dev) == 0)
		dev->dmacr |= DW_IC_TXDMAE;
	else {
		dev_warn(dev->dev, "Dma tx failed.\n");
		goto error;
	}

	dma_txbuf = (u16 *)dev->dmatx.buf;
    dev->dmatx.length=tx_len;

#ifdef CONFIG_I2C_DESIGNWARE_HI6930
	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		u32 cmd = 0;
		buf = msgs[dev->msg_write_idx].buf;
		buf_len = msgs[dev->msg_write_idx].len;

		if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {
			for (i = 0; i < buf_len; i++) {
				if (dev->msg_write_idx == dev->msgs_num - 1 &&
					i == buf_len - 1)
					cmd |= BIT(9);

				*dma_txbuf++ = cmd | 0x100;
				rx_len++;
			}
		} else {
			for (i = 0; i < buf_len; i++) {
				if (dev->msg_write_idx == dev->msgs_num - 1 &&
					i == buf_len - 1)
					cmd |= BIT(9);

				*dma_txbuf++ = cmd | *buf++;
			}
		}
	}
#else
    for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {

		buf = msgs[dev->msg_write_idx].buf;
		buf_len = msgs[dev->msg_write_idx].len;

		if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {
			for (i = 0; i < buf_len; i++) {
				*dma_txbuf++ = 0x100;
				rx_len++;
			}
		} else {
			for (i = 0; i < buf_len; i++)
				*dma_txbuf++ = *buf++;
		}
	}
#endif

	if (rx_len > 0) {

		dev->using_rx_dma = true;
        dev->dmarx.length=rx_len;
		if (i2c_dw_dma_rx(dev) >= 0)
		{
			dev->dmacr |= DW_IC_RXDMAE;
		}
		else {
			dev_warn(dev->dev, "Dma rx failed.\n");
			goto error;
		}
	}

	writew(dev->dmacr, dev->base + DW_IC_DMA_CR);

	ret = 0;
error:
	if (ret < 0) {
		dev->using_dma = false;
		/* Restore for CPU transfer */
		dev->msg_write_idx = 0;
		dev->msg_read_idx = 0;
		dev->status &= ~STATUS_WRITE_IN_PROGRESS;
		dev_err(dev->dev, "i2c_dw_xfer_msg_dma return erron %d.\n",
				ret);
	}
	return ret;
}

/*
 * Prepare controller for a transaction and call i2c_dw_xfer_msg
 */
int
i2c_dw_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	struct dw_i2c_dev *dev = i2c_get_adapdata(adap);
	int totallen = 0;
	int ret;
	int r;


	dev_dbg(dev->dev, "%s: msgs: %d\n", __func__, num);

	mutex_lock(&dev->lock);
	//pm_runtime_get_sync(dev->dev);

	r = clk_prepare_enable(dev->clk);
	if(r) {
		dev_warn(dev->platform_dev,"Unable to enable clock!\n");
		return  -EINVAL;
	}
#ifdef CONFIG_ARCH_HI3630
	r = devm_pinctrl_state_select(dev, PINCTRL_STATE_DEFAULT);
	if (r<0)
		dev_warn(dev->platform_dev,
				 "pins are not configured from the driver\n");
#endif
	INIT_COMPLETION(dev->cmd_complete);
	dev->msgs = msgs;
	dev->msgs_num = num;
	dev->cmd_err = 0;
	dev->msg_write_idx = 0;
	dev->msg_read_idx = 0;
	dev->msg_err = 0;
	dev->status = STATUS_IDLE;
	dev->abort_source = 0;
	dev->rx_outstanding = 0;

	dev->using_tx_dma = false;
	dev->using_rx_dma = false;
	dev->dmarx.buf = NULL;
	dev->dmatx.buf = NULL;
	dev->using_dma = false;

	ret = i2c_dw_wait_bus_not_busy(dev);
	if (ret < 0)
		goto done;

	/* start the transfers */
	i2c_dw_xfer_init(dev);

	if((0x4 != (dw_readl(dev, DW_IC_STATUS) & 0xC))
			|| (dw_readl(dev , DW_IC_TXFLR) & 0x3F)
			|| (dw_readl(dev, DW_IC_RXFLR) & 0x3F)) {
		dev_err(dev->dev,  "rx or tx fifo is not zero.\n");
		reset_i2c_controller(dev);
		ret = -EAGAIN;
		goto done;
	}

	if (i2c_dw_xfer_msg_dma(dev, &totallen) < 0) {
		if (dev->msg_err < 0) {
			ret = dev->msg_err;
			goto done;
		}
		writel(DW_IC_INTR_DEFAULT_MASK, dev->base + DW_IC_INTR_MASK);
	}

	/* wait for tx to complete */
	ret = wait_for_completion_timeout(&dev->cmd_complete, WAIT_FOR_COMPLETION);
	if (ret == 0) {
		int retry_num = 20;
		dev_err(dev->dev, "controller timed out\n");

		do {
			writel(0, dev->base + DW_IC_INTR_MASK);
			msleep(5);
		}while((dev->irq_is_run != 0) && (retry_num-- != 0));
		if(dev->irq_is_run != 0){
			dev_err(dev->dev, "Isr is runing, can't reset I2C IP.\n");
		}else{
			reset_i2c_controller(dev);
		}

		ret = -ETIMEDOUT;
		goto done;
	} else if (ret < 0)
		goto done;

	if (dev->msg_err) {
		ret = dev->msg_err;
		goto done;
	}

	if((!dev->cmd_err) && (dev->using_dma)) {
		ret = wait_for_completion_timeout(&dev->dma_complete, WAIT_FOR_COMPLETION);
		if (ret == 0) {
			dev_err(dev->dev, "wait for dma complete timed out, transfer %d, len = "
					"%d, slave_addr = 0x%x, clk = %lu.\n", WAIT_FOR_COMPLETION,
					totallen, msgs[0].addr, clk_get_rate(dev->clk));
			reset_i2c_controller(dev);
			ret = -ETIMEDOUT;
			goto done;
		}
	}

	/* no error */
	if (likely(!dev->cmd_err)) {
		/* Disable the adapter */
		//__i2c_dw_enable(dev, false);
		if((dev->status & (STATUS_READ_IN_PROGRESS|STATUS_WRITE_IN_PROGRESS))
				|| (DW_IC_INTR_TX_EMPTY == (readl(dev->base + DW_IC_INTR_MASK)&DW_IC_INTR_TX_EMPTY))){
			dev_err(dev->dev, "write or read is not complete, status 0x%x, len = %d.\n", dev->status, totallen);
			ret = -EAGAIN;
		}
		else{
			ret = num;
		}
		goto done;
	}

	/* We have an error */
	if (dev->cmd_err == DW_IC_ERR_TX_ABRT) {
		ret = i2c_dw_handle_tx_abort(dev);
		goto done;
	}
	ret = -EIO;

done:
	i2c_dw_dma_clear(dev);
    clk_disable_unprepare(dev->clk);

#ifdef CONFIG_ARCH_HI3630
	r = devm_pinctrl_state_select(dev, PINCTRL_STATE_IDLE);
	if (r<0)
		dev_warn(dev->platform_dev,
				 "pins are not configured from the driver\n");
#endif
	//pm_runtime_mark_last_busy(dev->dev);
	//pm_runtime_put_autosuspend(dev->dev);
	mutex_unlock(&dev->lock);
	if(ret < 0){
		dev_err(dev->dev, "error info %d, slave addr 0x%x.\n", ret, msgs[0].addr);
	}
	return ret;
}
EXPORT_SYMBOL_GPL(i2c_dw_xfer);

u32 i2c_dw_func(struct i2c_adapter *adap)
{
	struct dw_i2c_dev *dev = i2c_get_adapdata(adap);
	return dev->functionality;
}
EXPORT_SYMBOL_GPL(i2c_dw_func);

static u32 i2c_dw_read_clear_intrbits(struct dw_i2c_dev *dev)
{
	u32 stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * Ths unmasked raw version of interrupt status bits are available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = dw_readl(IC_INTR_STAT);
	 * equals to,
	 *   stat = dw_readl(IC_RAW_INTR_STAT) & dw_readl(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = dw_readl(dev, DW_IC_INTR_STAT);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * dw_readl(IC_INTR_STAT) to dw_readl(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & DW_IC_INTR_RX_UNDER)
		dw_readl(dev, DW_IC_CLR_RX_UNDER);
	if (stat & DW_IC_INTR_RX_OVER)
		dw_readl(dev, DW_IC_CLR_RX_OVER);
	if (stat & DW_IC_INTR_TX_OVER)
		dw_readl(dev, DW_IC_CLR_TX_OVER);
	if (stat & DW_IC_INTR_RD_REQ)
		dw_readl(dev, DW_IC_CLR_RD_REQ);
	if (stat & DW_IC_INTR_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		dev->abort_source = dw_readl(dev, DW_IC_TX_ABRT_SOURCE);
		dw_readl(dev, DW_IC_CLR_TX_ABRT);
	}
	if (stat & DW_IC_INTR_RX_DONE)
		dw_readl(dev, DW_IC_CLR_RX_DONE);
	if (stat & DW_IC_INTR_ACTIVITY)
		dw_readl(dev, DW_IC_CLR_ACTIVITY);
	if (stat & DW_IC_INTR_STOP_DET)
		dw_readl(dev, DW_IC_CLR_STOP_DET);
	if (stat & DW_IC_INTR_START_DET)
		dw_readl(dev, DW_IC_CLR_START_DET);
	if (stat & DW_IC_INTR_GEN_CALL)
		dw_readl(dev, DW_IC_CLR_GEN_CALL);

	return stat;
}

/*
 * Interrupt service routine. This gets called whenever an I2C interrupt
 * occurs.
 */
irqreturn_t i2c_dw_isr(int this_irq, void *dev_id)
{
	struct dw_i2c_dev *dev = dev_id;
	u32 stat, raw_stat, enabled;

	dev->irq_is_run = 1;
	stat = i2c_dw_read_clear_intrbits(dev);
	enabled = dw_readl(dev, DW_IC_ENABLE);
	raw_stat = dw_readl(dev, DW_IC_RAW_INTR_STAT);
	dev_dbg(dev->dev, "%s:  %s enabled= 0x%x raw_stat=0x%x\n", __func__,
		dev->adapter.name, enabled, raw_stat);
	if (!enabled || !(raw_stat & ~DW_IC_INTR_ACTIVITY))
		return IRQ_HANDLED;

	if (stat & DW_IC_INTR_TX_ABRT) {
		dev->cmd_err |= DW_IC_ERR_TX_ABRT;
		dev->status = STATUS_IDLE;

		/*
		 * Anytime TX_ABRT is set, the contents of the tx/rx
		 * buffers are flushed.  Make sure to skip them.
		 */
		dw_writel(dev, 0, DW_IC_INTR_MASK);
		goto tx_aborted;
	}

	if (stat & DW_IC_INTR_RX_FULL)
		i2c_dw_read(dev);

	if ((stat & DW_IC_INTR_STOP_DET)){
		if(!(dev->using_dma))
			i2c_dw_read(dev);
		goto tx_aborted;
	}

	if (stat & DW_IC_INTR_TX_EMPTY)
		i2c_dw_xfer_msg(dev);

	/*
	 * No need to modify or disable the interrupt mask here.
	 * i2c_dw_xfer_msg() will take care of it according to
	 * the current transmit status.
	 */

tx_aborted:
	if ((stat & (DW_IC_INTR_TX_ABRT | DW_IC_INTR_STOP_DET)) || dev->msg_err) {
		writel(0, dev->base + DW_IC_INTR_MASK);
		complete(&dev->cmd_complete);
	}
	dev->irq_is_run = 0;
	return IRQ_HANDLED;
}
EXPORT_SYMBOL_GPL(i2c_dw_isr);

void i2c_dw_enable(struct dw_i2c_dev *dev)
{
       /* Enable the adapter */
	__i2c_dw_enable(dev, true);
}
EXPORT_SYMBOL_GPL(i2c_dw_enable);

u32 i2c_dw_is_enabled(struct dw_i2c_dev *dev)
{
	return dw_readl(dev, DW_IC_ENABLE);
}
EXPORT_SYMBOL_GPL(i2c_dw_is_enabled);

void i2c_dw_disable(struct dw_i2c_dev *dev)
{
	/* Disable controller */
	__i2c_dw_enable(dev, false);

	/* Disable all interupts */
	dw_writel(dev, 0, DW_IC_INTR_MASK);
	dw_readl(dev, DW_IC_CLR_INTR);
}
EXPORT_SYMBOL_GPL(i2c_dw_disable);

void i2c_dw_clear_int(struct dw_i2c_dev *dev)
{
	dw_readl(dev, DW_IC_CLR_INTR);
}
EXPORT_SYMBOL_GPL(i2c_dw_clear_int);

void i2c_dw_disable_int(struct dw_i2c_dev *dev)
{
	dw_writel(dev, 0, DW_IC_INTR_MASK);
}
EXPORT_SYMBOL_GPL(i2c_dw_disable_int);

u32 i2c_dw_read_comp_param(struct dw_i2c_dev *dev)
{
	return dw_readl(dev, DW_IC_COMP_PARAM_1);
}
EXPORT_SYMBOL_GPL(i2c_dw_read_comp_param);

MODULE_DESCRIPTION("Synopsys DesignWare I2C bus adapter core");
MODULE_LICENSE("GPL");


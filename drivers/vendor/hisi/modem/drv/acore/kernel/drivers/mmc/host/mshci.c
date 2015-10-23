/*
 * linux/drivers/mmc/host/mshci.c
 *
 * Mobile Storage Host Controller Interface driver
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Based on linux/drivers/mmc/host/sdhci.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
 /*lint -save -e19 -e62 -e64 -e123 -e514 -e530 -e537 -e547 -e548 -e648 -e681 -e701 -e713 -e718 -e730 -e732 -e737 -e746 -e747 -e767 -e778 -e830 -e958 -e1564*/
#include <linux/delay.h>
#include <linux/highmem.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/scatterlist.h>
#include <linux/leds.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/core.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/mshci.h>
#include <linux/clk.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/debugfs.h>
#include <linux/workqueue.h>
#define HI_MMC_DEBUG


static unsigned int debug = 1;
static unsigned int quirks;
/*lint --e{457 } */
#define DBG(f, x...) \
do { \
		if (debug & 0x1) {\
			dev_dbg(host->mmc->parent, "%s "f, __func__, ## x); \
		} else if (debug & 0x2) {\
			dev_info(host->mmc->parent, "%s "f, __func__, ## x); \
		} else { }\
} while(0)

#define ERR(f, x...) \
do { \
		dev_err(host->mmc->parent, "%s "f, __func__, ## x); \
} while(0)

#define INFO(f, x...) \
do { \
		dev_info(host->mmc->parent, "%s "f, __func__, ## x); \
} while(0)

	
#define SDHC_CLK_ON 1
#define SDHC_CLK_OFF 0
#define HI_MMC_OPT_WAITBUSY

#define DMA_SG_NUM 256

#define DELAY_TIME     2000

static ssize_t mshci_show_debug(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", debug);
}

static ssize_t mshci_show_quirks(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	
	return snprintf(buf, PAGE_SIZE, "%d\n", quirks);
}
static ssize_t mshci_store_debug(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	ssize_t		rc;
	rc = kstrtouint(buf, 16, &debug);
	if (rc)
		return rc;
	rc = count;
	return rc;
}
static ssize_t mshci_store_quirks(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	ssize_t		rc;
	rc = kstrtouint(buf, 16, &quirks);
	if (rc)
		return rc;
	rc = count;
	return rc;
}
static DEVICE_ATTR(debug, 0644, mshci_show_debug, mshci_store_debug);
static DEVICE_ATTR(quirks, 0644, mshci_show_quirks, mshci_store_quirks);
static void mshci_prepare_data(struct mshci_host *, struct mmc_data *);
static void mshci_finish_data(struct mshci_host *);

static void mshci_send_command(struct mshci_host *, struct mmc_command *);
static void mshci_finish_command(struct mshci_host *);
static void mshci_set_clock(struct mshci_host *host, unsigned int clock);
static void mshci_fifo_init(struct mshci_host *host);
static void mshci_request(struct mmc_host *mmc, struct mmc_request *mrq);

static void mshci_dumpregs(struct mshci_host *host)
{
	pr_debug(" ============== REGISTER DUMP =======================\n");
	pr_debug(" MSHCI_CTRL:	0x%08x\n",
		mshci_readl(host, MSHCI_CTRL));
	pr_debug(" MSHCI_PWREN:	0x%08x\n",
		mshci_readl(host, MSHCI_PWREN));
	pr_debug(" MSHCI_CLKDIV:	0x%08x\n",
		mshci_readl(host, MSHCI_CLKDIV));
	pr_debug(" MSHCI_CLKSRC:	0x%08x\n",
		mshci_readl(host, MSHCI_CLKSRC));
	pr_debug(" MSHCI_CLKENA:	0x%08x\n", 
		mshci_readl(host, MSHCI_CLKENA));
	pr_debug(" MSHCI_TMOUT:	0x%08x\n",
		mshci_readl(host, MSHCI_TMOUT));
	pr_debug(" MSHCI_CTYPE:	0x%08x\n",
		mshci_readl(host, MSHCI_CTYPE));
	pr_debug(" MSHCI_BLKSIZ:	0x%08x\n",
		mshci_readl(host, MSHCI_BLKSIZ));
	pr_debug(" MSHCI_BYTCNT:	0x%08x\n",
		mshci_readl(host, MSHCI_BYTCNT));
	pr_debug(" MSHCI_INTMSK:	0x%08x\n",
		mshci_readl(host, MSHCI_INTMSK));
	pr_debug(" MSHCI_CMDARG:	0x%08x\n",
		mshci_readl(host, MSHCI_CMDARG));
	pr_debug(" MSHCI_CMD:	0x%08x\n",
		mshci_readl(host, MSHCI_CMD));
	pr_debug(" MSHCI_MINTSTS:	0x%08x\n",
		mshci_readl(host, MSHCI_MINTSTS));
	pr_debug(" MSHCI_RINTSTS:	0x%08x\n",
		mshci_readl(host, MSHCI_RINTSTS));
	pr_debug(" MSHCI_STATUS:	0x%08x\n",
		mshci_readl(host, MSHCI_STATUS));
	pr_debug(" MSHCI_FIFOTH:	0x%08x\n",
		mshci_readl(host, MSHCI_FIFOTH));
	pr_debug(" MSHCI_CDETECT:	0x%08x\n",
		mshci_readl(host, MSHCI_CDETECT));
	pr_debug(" MSHCI_WRTPRT:	0x%08x\n",
		mshci_readl(host, MSHCI_WRTPRT));
	pr_debug(" MSHCI_GPIO:	0x%08x\n",
		mshci_readl(host, MSHCI_GPIO));
	pr_debug(" MSHCI_TCBCNT:	0x%08x\n",
		mshci_readl(host, MSHCI_TCBCNT));
	pr_debug(" MSHCI_TBBCNT:	0x%08x\n",
		mshci_readl(host, MSHCI_TBBCNT));
	pr_debug(" MSHCI_DEBNCE:	0x%08x\n",
		mshci_readl(host, MSHCI_DEBNCE));
	pr_debug(" MSHCI_USRID:	0x%08x\n",
		mshci_readl(host, MSHCI_USRID));
	pr_debug(" MSHCI_VERID:	0x%08x\n",
		mshci_readl(host, MSHCI_VERID));
	pr_debug(" MSHCI_HCON:	0x%08x\n",
		mshci_readl(host, MSHCI_HCON));
	pr_debug(" MSHCI_UHS_REG:	0x%08x\n",
		mshci_readl(host, MSHCI_UHS_REG));
	pr_debug(" MSHCI_BMOD:	0x%08x\n",
		mshci_readl(host, MSHCI_BMOD));
	pr_debug(" MSHCI_PLDMND:	0x%08x\n",
		mshci_readl(host, MSHCI_PLDMND));
	pr_debug(" MSHCI_DBADDR:	0x%08x\n",
		mshci_readl(host, MSHCI_DBADDR));
	pr_debug(" MSHCI_IDSTS:	0x%08x\n",
		mshci_readl(host, MSHCI_IDSTS));
	pr_debug(" MSHCI_IDINTEN:	0x%08x\n",
		mshci_readl(host, MSHCI_IDINTEN));
	pr_debug(" MSHCI_DSCADDR:	0x%08x\n",
		mshci_readl(host, MSHCI_DSCADDR));
	pr_debug(" MSHCI_BUFADDR:	0x%08x\n",
		mshci_readl(host, MSHCI_BUFADDR));
	pr_debug(" MSHCI_WAKEUPCON:	0x%08x\n",
		mshci_readl(host, MSHCI_WAKEUPCON));
	pr_debug(" MSHCI_CLOCKCON:	0x%08x\n",
		mshci_readl(host, MSHCI_CLOCKCON));
	pr_debug(" MSHCI_CARDTHRCTl:	0x%08x\n",
		mshci_readl(host, MSHCI_CARDTHRCTl));
	pr_debug(" ================================================\n");
}

/* Low level functions */

static void mshci_clear_set_irqs(struct mshci_host *host, u32 clear, u32 set)
{
	u32 ier;

	ier = mshci_readl(host, MSHCI_INTMSK);
	ier &= ~clear;
	ier |= set;
	mshci_writel(host, ier, MSHCI_INTMSK);
}

static void mshci_unmask_irqs(struct mshci_host *host, u32 irqs)
{
	mshci_clear_set_irqs(host, 0, irqs);
}

static void mshci_mask_irqs(struct mshci_host *host, u32 irqs)
{
	mshci_clear_set_irqs(host, irqs, 0);
}

static void mshci_set_card_detection(struct mshci_host *host, bool enable)
{
	u32 irqs = INTMSK_CDETECT;

	if (enable)
		mshci_unmask_irqs(host, irqs);
	else
		mshci_mask_irqs(host, irqs);
}

#if 0
static void mshci_enable_card_detection(struct mshci_host *host)
{
	mshci_set_card_detection(host, true);
}
#endif
static void mshci_disable_card_detection(struct mshci_host *host)
{
	mshci_set_card_detection(host, false);
}

static void mshci_reset_fifo(struct mshci_host *host)
{
	u32 timeout = 100;
	u32 ier;

	ier = mshci_readl(host, MSHCI_CTRL);
	ier |= FIFO_RESET;

	mshci_writel(host, ier, MSHCI_CTRL);
	while (mshci_readl(host, MSHCI_CTRL) & FIFO_RESET) {
		if (timeout == 0) {
			ERR(" Reset FIFO never completed.\n");
			mshci_dumpregs(host);
			return;
		}
		timeout--;
		mdelay(1);
	}
}


static void mshci_reset_dma(struct mshci_host *host)
{
	u32 timeout = 1000;
	u32 ier;

	ier = mshci_readl(host, MSHCI_CTRL);
	ier |= DMA_RESET;

	mshci_writel(host, ier, MSHCI_CTRL);
	while (mshci_readl(host, MSHCI_CTRL) & DMA_RESET) {
		if (timeout == 0) {
			DBG(" Reset DMA never completed.\n");
			mshci_dumpregs(host);
			return;
		}
		timeout--;
		udelay(10);
	}
}

static void mshci_reset_all(struct mshci_host *host)
{
	u32 timeout = 1000;
	u32 ier;

	ier = mshci_readl(host, MSHCI_CTRL);
	ier |= DMA_RESET | FIFO_RESET | CTRL_RESET;

	mshci_writel(host, ier, MSHCI_CTRL);
	while (mshci_readl(host, MSHCI_CTRL) & (DMA_RESET | FIFO_RESET | CTRL_RESET)) {
		if (timeout == 0) {
			DBG(" Reset never completed.\n");
			mshci_dumpregs(host);
			return;
		}
		timeout--;
		udelay(10);
	}
}

static void mshci_init(struct mshci_host *host)
{
	mshci_reset_all(host);

	/* clear interrupt status */
	mshci_writel(host, INTMSK_ALL, MSHCI_RINTSTS);

	mshci_clear_set_irqs(host, INTMSK_ALL,
		/*INTMSK_CDETECT | */
		INTMSK_RE |
		INTMSK_CDONE | INTMSK_DTO | INTMSK_TXDR | INTMSK_RXDR |
		INTMSK_RCRC | INTMSK_DCRC | INTMSK_RTO | INTMSK_DRTO |
		INTMSK_HTO | INTMSK_FRUN | INTMSK_HLE | INTMSK_SBE |
		INTMSK_EBE);
}

static void mshci_reinit(struct mshci_host *host)
{
	mshci_init(host);
	mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) & ~INT_ENABLE),
					MSHCI_CTRL);
}

/* Core functions */

static void mshci_read_block_pio(struct mshci_host *host)
{
	unsigned long flags;
	size_t blksize, len, chunk;
	u32 scratch = 0;
	u8 *buf;

	DBG("PIO reading\n");

	blksize = host->data->blksz;
	blksize = min(blksize, host->data_transfered << 2);
	chunk = 0;
	local_irq_save(flags);

	while (blksize) {
		if (!sg_miter_next(&host->sg_miter))
			BUG();

		len = min(host->sg_miter.length, blksize);

		blksize -= len;
		host->sg_miter.consumed = len;

		buf = host->sg_miter.addr;

		while (len) {
			if (chunk == 0) {
				scratch = mshci_readl(host, MSHCI_FIFODAT);
				chunk = 4;
			}

			*buf = scratch & 0xFF;

			buf++;
			scratch >>= 8;
			chunk--;
			len--;
		}
	}
	host->data->blksz -= blksize;
	if (!host->data->blksz) {
		host->blocks--;
	}
	sg_miter_stop(&host->sg_miter);

	local_irq_restore(flags);
}

static void mshci_write_block_pio(struct mshci_host *host)
{
	unsigned long flags;
	size_t blksize, len, chunk;
	u32 scratch;
	u8 *buf;
	DBG("PIO writing\n");

	blksize = host->data->blksz;
	chunk = 0;
	scratch = 0;
	local_irq_save(flags);
	
	while (blksize) {
		if (!sg_miter_next(&host->sg_miter))
			BUG();
		len = min(host->sg_miter.length, blksize);
		blksize -= len;
		host->sg_miter.consumed = len;

		buf = host->sg_miter.addr;

		while (len) {
			scratch |= (u32)*buf << (chunk * 8);

			buf++;
			chunk++;
			len--;

			if ((chunk == 4) || ((len == 0) && (blksize == 0))) {
				mshci_writel(host, scratch, MSHCI_FIFODAT);
				chunk = 0;
				scratch = 0;
			}
		}

	}
	host->blocks --;
	sg_miter_stop(&host->sg_miter);

	local_irq_restore(flags);
}

static void mshci_transfer_pio(struct mshci_host *host)
{
	WARN_ON(!host->data);

	if (host->blocks == 0 || !host->data)
		return;

	if (host->data->flags & MMC_DATA_READ) {
		unsigned int reg;
		reg = 0X1FFF & (mshci_readl(host, MSHCI_STATUS) >> 17);
		if (reg) {
			host->data_transfered = reg;
			mshci_read_block_pio(host);
		}
	}
	else {
		mshci_write_block_pio(host);
	}

	DBG("PIO transfer complete.\n");
}

static void mshci_set_mdma_desc(u8 *desc_vir, u8 *desc_phy,
				u32 des0, u32 des1, u32 des2)
{
	((struct mshci_idmac *)(desc_vir))->des0 = des0;
	((struct mshci_idmac *)(desc_vir))->des1 = des1;
	((struct mshci_idmac *)(desc_vir))->des2 = des2;
	((struct mshci_idmac *)(desc_vir))->des3 = (u32)desc_phy +
					sizeof(struct mshci_idmac);
}

static int mshci_mdma_table_pre(struct mshci_host *host,
				struct mmc_data *data)
{
	int direction;

	u8 *desc_vir, *desc_phy;
	dma_addr_t addr;
	int len;

	struct scatterlist *sg;
	int i;
	u32 des_flag;
	u32 size_idmac = sizeof(struct mshci_idmac);

	if (data->flags & MMC_DATA_READ)
		direction = DMA_FROM_DEVICE;
	else
		direction = DMA_TO_DEVICE;

	host->sg_count = dma_map_sg(mmc_dev(host->mmc),
		data->sg, data->sg_len, direction);
	if (host->sg_count == 0)
		goto fail;

	desc_vir = host->idma_desc;

	/* to know phy address */
	#if 0
	host->idma_addr = dma_map_single(mmc_dev(host->mmc),
				host->idma_desc,
				DMA_SG_NUM * size_idmac,
				DMA_TO_DEVICE);
	
	if (dma_mapping_error(mmc_dev(host->mmc), host->idma_addr))
		goto unmap_entries;
	#else
	host->idma_addr = virt_to_phys(host->idma_desc);
	#endif
	BUG_ON((host->idma_addr & 0x3) != 0);

	desc_phy = (u8 *)host->idma_addr;

	for_each_sg(data->sg, sg, host->sg_count, i) {
		addr = sg_dma_address(sg);
		len = sg_dma_len(sg);

		/* tran, valid */
		des_flag = (MSHCI_IDMAC_OWN | MSHCI_IDMAC_CH);
		des_flag |= (i == 0) ? MSHCI_IDMAC_FS : 0;

		mshci_set_mdma_desc(desc_vir, desc_phy, des_flag, len, addr);
		desc_vir += size_idmac;
		desc_phy += size_idmac;

		/* If this triggers then we have a calculation bug somewhere */
		WARN_ON((desc_vir - host->idma_desc) > DMA_SG_NUM * size_idmac);/*lint !e574*/
	}

	/* Add a terminating flag */
	((struct mshci_idmac *)(desc_vir-size_idmac))->des0 |= MSHCI_IDMAC_LD;

	/* It has to dma map again to resync vir data to phy data */
	host->idma_addr = dma_map_single(mmc_dev(host->mmc),
				host->idma_desc,
				host->sg_count * size_idmac,
				DMA_TO_DEVICE);
	if (dma_mapping_error(mmc_dev(host->mmc), host->idma_addr))
		goto unmap_entries;
	BUG_ON((host->idma_addr & 0x3) != 0);

	return 0;

unmap_entries:
	dma_unmap_sg(mmc_dev(host->mmc), data->sg, data->sg_len, direction);
fail:
	return -EINVAL;
}

static void mshci_idma_table_post(struct mshci_host *host,
				  struct mmc_data *data)
{
	int direction;

	if (data->flags & MMC_DATA_READ)
		direction = DMA_FROM_DEVICE;
	else
		direction = DMA_TO_DEVICE;

	dma_unmap_single(mmc_dev(host->mmc), host->idma_addr,
			 DMA_SG_NUM * sizeof(struct mshci_idmac), DMA_TO_DEVICE);

	dma_unmap_sg(mmc_dev(host->mmc), data->sg, data->sg_len, direction);
}

static u32 mshci_calc_timeout(struct mshci_host *host, struct mmc_data *data)
{
	/* this value SHOULD be optimized */
	return 0xffffffff;
}

static void mshci_set_transfer_irqs(struct mshci_host *host)
{
	u32 uhs_reg;
	u32 dma_irqs = INTMSK_DMA;
	u32 pio_irqs = INTMSK_TXDR | INTMSK_RXDR;

	uhs_reg = mshci_readl(host, MSHCI_UHS_REG);

	if (host->flags & MSHCI_REQ_USE_DMA) {
		/* Next codes are the W/A for DDR */
		if ((uhs_reg & (0x1 << 16))
			&& (host->data->flags & MMC_DATA_WRITE))
			dma_irqs |= INTMSK_DCRC;

		/* clear interrupts for PIO */
		mshci_clear_set_irqs(host, dma_irqs, 0);
	} else {
		/* Next codes are the W/A for DDR */
		if ((uhs_reg & (0x1 << 16))
			&& (host->data->flags & MMC_DATA_WRITE))
			mshci_clear_set_irqs(host, INTMSK_DCRC, pio_irqs);
		else
			mshci_clear_set_irqs(host, 0, pio_irqs);
	}
}

static void mshci_prepare_data(struct mshci_host *host, struct mmc_data *data)
{
	u32 count;
#ifndef HI_MMC_OPT_WAITBUSY
	u32 ret;
#endif
	if (data == NULL)
		return;

	BUG_ON(data->blksz * data->blocks > (host->mmc->max_req_size *
					host->mmc->max_segs));
	BUG_ON(data->blksz > host->mmc->max_blk_size);
	BUG_ON(data->blocks > 400000);

	host->data = data;
	host->data_early = 0;

	count = mshci_calc_timeout(host, data);
	mshci_writel(host, count, MSHCI_TMOUT);

	mshci_reset_fifo(host);

	if (host->flags & (MSHCI_USE_IDMA))
		host->flags |= MSHCI_REQ_USE_DMA;

	/*
	 * FIXME: This doesn't account for merging when mapping the
	 * scatterlist.
	 */
	if (host->flags & MSHCI_REQ_USE_DMA) {
		/*
		 *  mshc's IDMAC can't transfer data that is not aligned
		 * or has length not divided by 4 byte.
		 */
		unsigned int i = 0;
		struct scatterlist *sg = NULL;
			/* coverity[var_deref_model] */
			for_each_sg(data->sg, sg, data->sg_len, i) {
			#if 0
				if (sg && (sg->length & 0x3) ) {
					DBG("Reverting to PIO because of "
						"transfer size (%d)\n",
						sg->length);
					host->flags &= ~MSHCI_REQ_USE_DMA;
					break;
				} else
			#else
				if (sg && (sg->offset & 0x3)) { /*lint !e115*/
			#endif
					DBG("Reverting to PIO because of "
						"bad alignment\n");
					host->flags &= ~MSHCI_REQ_USE_DMA;
					break;
				}
			}
	}

#ifndef HI_MMC_OPT_WAITBUSY
	/* remove to mshci_send_command for optimised */
	if (host->flags & MSHCI_REQ_USE_DMA) {
		ret = mshci_mdma_table_pre(host, data);
		if (ret) {
			/*
			 * This only happens when someone fed
			 * us an invalid request.
			 */
			WARN_ON(1);
			host->flags &= ~MSHCI_REQ_USE_DMA;
		} else {
			mshci_writel(host, host->idma_addr,
				MSHCI_DBADDR);
		}
	}
#endif

	if (host->flags & MSHCI_REQ_USE_DMA) {
		/* enable DMA, IDMA interrupts and IDMAC */
		mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) |
					ENABLE_IDMAC|DMA_ENABLE), MSHCI_CTRL);
		mshci_writel(host, (mshci_readl(host, MSHCI_BMOD) |
					(BMOD_IDMAC_ENABLE|BMOD_IDMAC_FB)),
					MSHCI_BMOD);
		mshci_writel(host, INTMSK_IDMAC_ERROR, MSHCI_IDINTEN);
	} else {
		/* disable DMA, IDMA interrupts and IDMAC */
		mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) &
					~(ENABLE_IDMAC|DMA_ENABLE)), MSHCI_CTRL);
		mshci_writel(host, (mshci_readl(host, MSHCI_BMOD) &
					~(BMOD_IDMAC_ENABLE|BMOD_IDMAC_FB)),
					MSHCI_BMOD);
		//mshci_writel(host, INTMSK_IDMAC_ERROR, MSHCI_IDINTEN);
	}

	if (!(host->flags & MSHCI_REQ_USE_DMA)) {
		int flags;

		flags = SG_MITER_ATOMIC;
		if (host->data->flags & MMC_DATA_READ)
			flags |= SG_MITER_TO_SG;
		else
			flags |= SG_MITER_FROM_SG;

		sg_miter_start(&host->sg_miter, data->sg, data->sg_len, flags);
		host->blocks = data->blocks;
	}
	/* set transfered data as 0. this value only uses for PIO write */
	host->data_transfered = 0;
	mshci_set_transfer_irqs(host);
	
	mshci_writel(host, data->blksz, MSHCI_BLKSIZ);
	if(data->blksz <= 512)
		mshci_writel(host, data->blksz<<16|0x1, MSHCI_CARDTHRCTl);
	else
		mshci_writel(host, 512<<16|0x1, MSHCI_CARDTHRCTl);

	mshci_writel(host, (data->blocks * data->blksz), MSHCI_BYTCNT);
}

static u32 mshci_set_transfer_mode(struct mshci_host *host,
				   struct mmc_data *data)
{
	u32 ret = 0;

	if (data == NULL)
		return ret;

	WARN_ON(!host->data);

	/* this cmd has data to transmit */
	ret |= CMD_DATA_EXP_BIT;

	if (data->flags & MMC_DATA_WRITE)
		ret |= CMD_RW_BIT;
	if (data->flags & MMC_DATA_STREAM)
		ret |= CMD_TRANSMODE_BIT;

	return ret;
}

static void mshci_finish_data(struct mshci_host *host)
{
	struct mmc_data *data;

	WARN_ON(!host->data);
	if (!host->data) {
		return;
	}
	data = host->data;
	host->data = NULL;

	if (host->flags & MSHCI_REQ_USE_DMA) {
		/* mshci_idma_table_post(host, data); */
		host->flags |= MSHCI_POST_UNMAP;
		/* disable IDMAC and DMA interrupt */
		mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) &
				~(DMA_ENABLE|ENABLE_IDMAC)), MSHCI_CTRL);

		/* mask all interrupt source of IDMAC */
		mshci_writel(host, 0x0, MSHCI_IDINTEN);
	}


	if (data->error) {
		mshci_reset_dma(host);
		data->bytes_xfered = 0;
	} else
		data->bytes_xfered = data->blksz * data->blocks;

	if (data->stop)
		mshci_send_command(host, data->stop);
	else
		tasklet_schedule(&host->finish_tasklet);
}

static void mshci_clock_onoff(struct mshci_host *host, bool val)
{
	u32 loop_count;
	u32 reg = 0;

	loop_count = 0x100000;
	
	mshci_writel(host, val, MSHCI_CLKENA);
	mshci_writel(host, 0, MSHCI_CMD);
	mshci_writel(host,INTMSK_HLE,MSHCI_RINTSTS);
	mshci_writel(host, CMD_ONLY_CLK, MSHCI_CMD);
	
	do {
		if (!(mshci_readl(host, MSHCI_CMD) & CMD_STRT_BIT))
			break;
		loop_count--;
	} while (loop_count);
	reg = mshci_readl(host, MSHCI_RINTSTS);
	if (reg & INTMSK_HLE) {
		ERR(" HLE in Clock %s. RINTSTS = 0x%x\n ", val ? "ON" : "OFF", reg);
		mshci_writel(host, 0, MSHCI_CMD);
		mshci_writel(host,INTMSK_HLE,MSHCI_RINTSTS);
	} else if (loop_count == 0) {
		ERR(" Clock %s has been failed.\n ",	 val ? "ON" : "OFF");
	}

	mshci_writel(host, 0, MSHCI_CMD);
}

static void mshci_send_command(struct mshci_host *host, struct mmc_command *cmd)
{
	int flags, ret;
	int ctype_flags, uhs_flags;


	WARN_ON(!cmd);
	if (!cmd) {
		return;
	}
	/* disable interrupt before issuing cmd to the card. */
	mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) & ~INT_ENABLE),
					MSHCI_CTRL);

	mod_timer(&host->timer, jiffies + 30 * HZ);

	host->cmd = cmd;

	mshci_prepare_data(host, cmd->data);

	mshci_writel(host, cmd->arg, MSHCI_CMDARG);

	flags = mshci_set_transfer_mode(host, cmd->data);

	if ((cmd->flags & MMC_RSP_136) && (cmd->flags & MMC_RSP_BUSY)) {
		ERR(" Unsupported response type!\n");
		cmd->error = (unsigned int)-EINVAL;
		tasklet_schedule(&host->finish_tasklet);
		return;
	}

	if (cmd->flags & MMC_RSP_PRESENT) {
		flags |= CMD_RESP_EXP_BIT;
		if (cmd->flags & MMC_RSP_136)
			flags |= CMD_RESP_LENGTH_BIT;
	}
	if (cmd->flags & MMC_RSP_CRC) {
		flags |= CMD_CHECK_CRC_BIT;
	}
#ifdef CONFIG_MMC_MSHCI_USE_HOLD_REG
		flags |= CMD_USE_HOLD_REG;
#endif
	flags |= (cmd->opcode | CMD_STRT_BIT | CMD_WAIT_PRV_DAT_BIT);

	if (MMC_STOP_TRANSMISSION == cmd->opcode) {
		flags &= ~CMD_WAIT_PRV_DAT_BIT;
		flags |= CMD_STOP_ABORT_CMD;
	}



	if (cmd->opcode == SD_SWITCH_VOLTAGE) {
		int loop_count;
		flags |= CMD_VOLT_SWITCH;

		/* disable clock low power */
		mshci_writel(host, (0x1<<0), MSHCI_CLKENA);
		mshci_writel(host, CMD_ONLY_CLK | CMD_VOLT_SWITCH, MSHCI_CMD);
		loop_count = 0x100000;
		do {
			if (!(mshci_readl(host, MSHCI_CMD) & CMD_STRT_BIT))
				break;
			loop_count--;
		} while (loop_count);
		if (!loop_count) 
			ERR("disable clk low power failed\n");

	}

	ret = mshci_readl(host, MSHCI_CMD);
	if (ret & CMD_STRT_BIT) {
		ERR("CMD busy. current cmd %d. last cmd reg 0x%x\n",
			cmd->opcode, ret);
		ctype_flags = mshci_readl(host, MSHCI_CTYPE);
		uhs_flags = mshci_readl(host, MSHCI_UHS_REG);
		mshci_reinit(host);
		host->clock_to_restore = host->clock;
		host->clock = 0;
		mshci_set_clock(host, host->clock_to_restore);
		mshci_writel(host, ctype_flags, MSHCI_CTYPE);
		mshci_writel(host, uhs_flags, MSHCI_UHS_REG);
	}

	mshci_writel(host, flags, MSHCI_CMD);

	/* enable interrupt upon it sends a command to the card. */
	mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) | INT_ENABLE),
					MSHCI_CTRL);
}

static void mshci_finish_command(struct mshci_host *host)
{
	int i;

	WARN_ON(host->cmd == NULL);
	if (!host->cmd) {
		return;
	}
	if (host->cmd->flags & MMC_RSP_PRESENT) {
		if (host->cmd->flags & MMC_RSP_136) {
			/* response data are overturned */
			for (i = 0; i < 4; i++) {
				host->cmd->resp[0] = mshci_readl(host,
								MSHCI_RESP3);
				host->cmd->resp[1] = mshci_readl(host,
								MSHCI_RESP2);
				host->cmd->resp[2] = mshci_readl(host,
								MSHCI_RESP1);
				host->cmd->resp[3] = mshci_readl(host,
								MSHCI_RESP0);
			}
		} else {
			host->cmd->resp[0] = mshci_readl(host, MSHCI_RESP0);
		}
	}

	host->cmd->error = 0;

	/* if data interrupt occurs earlier than command interrupt */
	if (host->data && host->data_early)
		mshci_finish_data(host);

	if (!host->cmd->data)
		tasklet_schedule(&host->finish_tasklet);

	host->cmd = NULL;
}

static void mshci_set_clock(struct mshci_host *host, unsigned int clock)
{
	unsigned int div;
	u32 loop_count, reg;
	
	DBG("max clk %d ,changing clock %d\n", host->max_clk, clock); 
	if (clock >= host->max_clk) {
		div = 0;
	} else {
		for (div = 1; div < 255; div++) {
			if ((host->max_clk / (div << 1)) <= clock)
				break;
		}
	}
	
	DBG("Clock div %d\n", div); 
	
	loop_count = 0x100000;
	
	mshci_clock_onoff(host, CLK_DISABLE);
	
	mshci_writel(host, div, MSHCI_CLKDIV);
	mshci_writel(host, 0, MSHCI_CLKSRC);
	mshci_writel(host, 0, MSHCI_CMD);
	mshci_writel(host,INTMSK_HLE,MSHCI_RINTSTS);
	mshci_writel(host, CMD_ONLY_CLK, MSHCI_CMD);

	do {
		if (!(mshci_readl(host, MSHCI_CMD) & CMD_STRT_BIT))
			break;
		loop_count--;
	} while (loop_count);

	reg = mshci_readl(host, MSHCI_RINTSTS);
	if (reg & INTMSK_HLE) {
		DBG(" HLE in Changing clock. RINTSTS = 0x%x\n", reg);
		mshci_writel(host, 0, MSHCI_CMD);
		mshci_writel(host,INTMSK_HLE,MSHCI_RINTSTS);
	} else if (loop_count == 0) {
		DBG(" Changing clock has been failed.\n");
	}

	mshci_writel(host, 0, MSHCI_CMD);

	mshci_clock_onoff(host, CLK_ENABLE);

	host->clock = clock;
}


/* MMC callbacks */
static void mshci_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct mshci_host *host;
	unsigned long flags;
	unsigned long timeout;
	int ret;
	u32 intmask;
	host = mmc_priv(mmc);

	WARN_ON(host->mrq != NULL);
	host->working = MMC_HOST_BUSY;
	host->mrq = mrq;
	/* Wait max 2s */
	timeout = 200000;

	#ifdef HI_MMC_OPT_WAITBUSY
	/* copy from mshci_prepare_data for optimised */
	if (mrq->cmd->data) {
		if (host->flags & (MSHCI_USE_IDMA))
			host->flags |= MSHCI_REQ_USE_DMA;

		if (host->flags & MSHCI_REQ_USE_DMA) {
			ret = mshci_mdma_table_pre(host, mrq->cmd->data);
			if (ret) {
				/*
				 * This only happens when someone fed
				 * us an invalid request.
				 */
				WARN_ON(1);
				host->flags &= ~MSHCI_REQ_USE_DMA;
				DBG("  NO use idma\n");

			} else {
				mshci_writel(host, host->idma_addr,
					MSHCI_DBADDR);
				intmask = mshci_readl(host, MSHCI_IDSTS);
				intmask = ((intmask >>13) & 0xf);
				if(1 == intmask)
				mshci_writel(host, 0x1,MSHCI_PLDMND);
			}
		}
	}
	#endif
	
	/*
	 * We shouldn't wait for data inihibit for stop commands,
	 * even though they might use busy signaling
	 */
	while (mshci_readl(host, MSHCI_STATUS) & (1 << 9)) {
		if (timeout == 0) {
			ERR(" Controller never released "
				"data0.\n");
			mshci_dumpregs(host);
			mrq->cmd->error = (unsigned int)-EIO;
			tasklet_schedule(&host->finish_tasklet);
			return;
		}
		timeout--;
		udelay(10);
	}
	spin_lock_irqsave(&host->lock, flags);
	mshci_send_command(host, mrq->cmd);
	spin_unlock_irqrestore(&host->lock, flags);
}

static void mshci_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct mshci_host *host;
	host = mmc_priv(mmc);
	if (host->ops && host->ops->set_ios) {
		host->ops->set_ios(host, ios);
	}
	if (host->clock != ios->clock && ios->clock) {
		host->clock = ios->clock;
		mshci_set_clock(host, host->clock);
	}
	if (ios->clock) {
		if (ios->bus_width == MMC_BUS_WIDTH_8) {
			mshci_writel(host, (0x1<<16), MSHCI_CTYPE);
		} else if (ios->bus_width == MMC_BUS_WIDTH_4) {
			mshci_writel(host, (0x1<<0), MSHCI_CTYPE);
		} else {
			mshci_writel(host, 0, MSHCI_CTYPE);
		}
		
		if (MMC_TIMING_UHS_DDR50 == ios->timing) {
			mshci_writel(host, mshci_readl(host, MSHCI_UHS_REG) | 0x1<<16, MSHCI_UHS_REG);
		}
	}

}
static int mshci_get_ro(struct mmc_host *mmc)
{
	struct mshci_host *host;
	unsigned long flags;
	int wrtprt;

	host = mmc_priv(mmc);
	spin_lock_irqsave(&host->lock, flags);
	host->working = MMC_HOST_BUSY;
	if (host->quirks & MSHCI_QUIRK_ALWAYS_WRITABLE)
		wrtprt = 0;
	else if (host->quirks & MSHCI_QUIRK_NO_WP_BIT)
		wrtprt = host->ops->get_ro(mmc) ? 0 : WRTPRT_ON;
	else if (host->flags & MSHCI_DEVICE_DEAD)
		wrtprt = 0;
	else 
		wrtprt = mshci_readl(host, MSHCI_WRTPRT);
	host->working = MMC_HOST_FREE;
	spin_unlock_irqrestore(&host->lock, flags);

	return wrtprt & WRTPRT_ON;
}

static void mshci_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	struct mshci_host *host;
	unsigned long flags;

	host = mmc_priv(mmc);
	spin_lock_irqsave(&host->lock, flags);

	host->working = MMC_HOST_BUSY;
	if (host->flags & MSHCI_DEVICE_DEAD)
		goto out;
	if (enable) {
		mshci_unmask_irqs(host, SDIO_INT_ENABLE);
		/* enable interrupt */
		mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) | INT_ENABLE),
					MSHCI_CTRL);
	}
	else
		mshci_mask_irqs(host, SDIO_INT_ENABLE);
out:
	mmiowb();
	host->working = MMC_HOST_FREE;
	spin_unlock_irqrestore(&host->lock, flags);
}

static int mshci_get_cd(struct mmc_host *mmc)
{
	struct mshci_host *host;
	unsigned ret = 0;
	host = mmc_priv(mmc);
	
	if (host->quirks & MSHCI_QUIRK_WLAN_DETECTION) {
		ret = host->flags & MSHCI_DEVICE_DEAD ? 0 : 1;
		return ret;
	}
	if (host->ops->get_present_status) {
		return host->ops->get_present_status(host);
	}
	else {
		return 0;
	}
}

static int mshci_start_signal_voltage_switch(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct mshci_host *host;
	unsigned long loop_count = 0x100000;
	int ret = -1;
	int intrs;
	unsigned long flags;
	
	host = mmc_priv(mmc);
	spin_lock_irqsave(&host->lock, flags);
	host->working = MMC_HOST_BUSY;
	spin_unlock_irqrestore(&host->lock, flags);

	/* voltage switch only happen to MMC_SIGNAL_VOLTAGE_180 */

	if (ios->signal_voltage != MMC_SIGNAL_VOLTAGE_330) {

		/* disable interrupt upon voltage switch. handle interrupt here 
		   and DO NOT triggle irq */
		mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) & ~INT_ENABLE),
					MSHCI_CTRL);

		/* stop clock */
		mshci_writel(host, (0x0<<0), MSHCI_CLKENA);
		mshci_writel(host, CMD_ONLY_CLK | CMD_VOLT_SWITCH, MSHCI_CMD);
		do {
			if (!(mshci_readl(host, MSHCI_CMD) & CMD_STRT_BIT))
				break;
			loop_count--;
		} while (loop_count);

		if (!loop_count) 
			ERR(" Disable clock failed in voltage switch\n");
		mmiowb();
	}

	mshci_writel(host, 0x0, MSHCI_UHS_REG);
	if (host->ops->start_signal_voltage_switch)
		ret = host->ops->start_signal_voltage_switch(host, ios);

	usleep_range(5000, 5500);

	if (ios->signal_voltage != MMC_SIGNAL_VOLTAGE_330) {

		/* voltage switch cont. */
		mshci_writel(host, 0x1, MSHCI_UHS_REG);

		/* start clock */
		mshci_writel(host, (0x1<<0), MSHCI_CLKENA);
		mshci_writel(host, CMD_ONLY_CLK | CMD_VOLT_SWITCH, MSHCI_CMD);
		loop_count = 0x100000;
		do {
			if (!(mshci_readl(host, MSHCI_CMD) & CMD_STRT_BIT))
				break;
			loop_count--;
		} while (loop_count);

		if (!loop_count) 
			ERR(" Enable clock failed in voltage switch\n");

		/* poll cd interrupt */
		loop_count = 0x100000;
		do {
			intrs = mshci_readl(host, MSHCI_RINTSTS); 
			if (intrs & INTMSK_CDONE) {
				DBG("command done in voltage switch\n");
				mshci_writel(host, intrs, MSHCI_RINTSTS);
				break;
			}
			loop_count--;
		} while (loop_count);		

		if (!loop_count) 
			ERR(" NO CD in voltage switch\n");
		/*during cmd11 do not operate mci clock*/
		host->voltage_int_count = 0;
		/* enable interrupt */
		mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) | INT_ENABLE),
					MSHCI_CTRL);			
	
		mmiowb();
	}
	spin_lock_irqsave(&host->lock, flags);
	host->working = MMC_HOST_FREE;
	spin_unlock_irqrestore(&host->lock, flags);
	return ret;
}

int mshci_execute_tuning(struct mmc_host *host, u32 opcode)
{
	struct mshci_host *ms_host;
	/*bit[30-29] * bit[28-24]*/
	int sample;
	unsigned int clk;
	
	ms_host = mmc_priv(host);
	if(ms_host->ops && ms_host->ops->tuning_find_condition) {
		sample = ms_host->ops->tuning_find_condition(ms_host);
		if(sample > -1 && ms_host->ops 
			&& ms_host->ops->tuning_set_current_state) {
			clk = (((sample / 6)  < 4) | (sample % 6));
			ms_host->ops->tuning_set_current_state(ms_host, clk);
		}
		
	}
	return 0;
}
static void mshci_hw_reset(struct mmc_host *host)
{
	struct mshci_host *ms_host;
	ms_host = mmc_priv(host);
	if(ms_host->ops && ms_host->ops->hw_reset)
		ms_host->ops->hw_reset(ms_host);
}

static struct mmc_host_ops mshci_ops = {
	.request	= mshci_request,
	.set_ios	= mshci_set_ios,
	.get_ro		= mshci_get_ro,
	.enable_sdio_irq = mshci_enable_sdio_irq,
	.get_cd     = mshci_get_cd,
	.start_signal_voltage_switch = mshci_start_signal_voltage_switch,
	.hw_reset = mshci_hw_reset,
	.execute_tuning = mshci_execute_tuning,
};

/* Tasklets */



static void mshci_tasklet_card(unsigned long param)
{
	struct mshci_host *host;
	host = (struct mshci_host *)param;
	cancel_delayed_work(&host->mmc->detect);
	mmc_detect_change(host->mmc, msecs_to_jiffies(150));
}

static void mshci_tasklet_finish(unsigned long param)
{
	struct mshci_host *host;
	unsigned long flags;
	struct mmc_request *mrq;
	u32 count = 100;

	host = (struct mshci_host *)param;

	spin_lock_irqsave(&host->lock, flags);
	del_timer(&host->timer);

	mrq = host->mrq;

	/*
	 * The controller needs a reset of internal state machines
	 * upon error conditions.
	 */
	if (!(host->flags & MSHCI_DEVICE_DEAD) && (mrq) &&
		((mrq->cmd && mrq->cmd->error) ||
		 (mrq->data && (mrq->data->error ||
		  (mrq->data->stop && mrq->data->stop->error))))) {

		if (host->flags & MSHCI_USE_IDMA) {
			mshci_writel(host, BMOD_IDMAC_RESET, MSHCI_BMOD);
			count = 100;
			while ((mshci_readl(host, MSHCI_BMOD) & BMOD_IDMAC_RESET)
				&& --count)
				; /* nothing to do */

			mshci_writel(host, (mshci_readl(host, MSHCI_BMOD) |
					(BMOD_IDMAC_ENABLE|BMOD_IDMAC_FB)), MSHCI_BMOD);

			/* clear idmac interrupts */
			mshci_writel(host, 0x3FF, MSHCI_IDSTS);
		}

		/*
		 * Spec says we should do both at the same time,
		 * but Ricoh controllers do not like that
		 */
		mshci_reset_fifo(host);
	}

	host->mrq = NULL;
	host->cmd = NULL;
	host->data = NULL;
	host->working = MMC_HOST_FREE;
	mmiowb();
	spin_unlock_irqrestore(&host->lock, flags);

	if (host->flags & MSHCI_POST_UNMAP) {
		host->flags &= ~MSHCI_POST_UNMAP;

		if (mrq && mrq->cmd)
			mshci_idma_table_post(host, mrq->cmd->data);
		else
			ERR(" idma table post error\n");
	}

	if(mrq && mrq->cmd)
		mmc_request_done(host->mmc, mrq);
}

static void mshci_timeout_timer(unsigned long data)
{
	struct mshci_host *host;
	unsigned long flags;

	host = (struct mshci_host *)data;

	spin_lock_irqsave(&host->lock, flags);

	if (host->mrq) {
		DBG(" Timeout waiting for hardware interrupt.\n");
		mshci_dumpregs(host);

		if (host->data) {
			host->data->error = (unsigned int)-ETIMEDOUT;
			mshci_finish_data(host);
		} else {
			if (host->cmd)
				host->cmd->error = (unsigned int)-ETIMEDOUT;
			else
				host->mrq->cmd->error = (unsigned int)-ETIMEDOUT;

			tasklet_schedule(&host->finish_tasklet);
		}
	}

	mmiowb();
	spin_unlock_irqrestore(&host->lock, flags);
}

/* Interrupt handling */

static void mshci_cmd_irq(struct mshci_host *host, u32 intmask)
{
	BUG_ON(intmask == 0);

	if (!host->cmd) {
		DBG(" Got command interrupt 0x%08x even "
			"though no command operation was in progress.\n",
			 (unsigned)intmask);
		return;
	}
	
	if (intmask & INTMSK_RTO) {
		DBG("CMD%d REP time out\n", host->cmd->opcode);
		host->cmd->error = (unsigned int)-ETIMEDOUT;
	} else if (intmask & INTMSK_RCRC) {
		ERR("CMD%d REP CRC error\n", host->cmd->opcode);
		host->cmd->error = (unsigned int)-EILSEQ;
	} else if (intmask & INTMSK_RE) {
		ERR("CMD%d REP error\n", host->cmd->opcode);
		host->cmd->error = (unsigned int)-EILSEQ;
	}

	if (host->cmd->error) {
		tasklet_schedule(&host->finish_tasklet);
		return;
	}

	if (intmask & INTMSK_CDONE) {
		DBG("CMD%d done\n", host->cmd->opcode);
		mshci_finish_command(host);
	}
}

static void mshci_data_irq(struct mshci_host *host, u32 intmask, u8 intr_src)
{
	BUG_ON(intmask == 0);

	if (!host->data) {
		/*
		 * The "data complete" interrupt is also used to
		 * indicate that a busy state has ended. See comment
		 * above in mshci_cmd_irq().
		 */
		if (host->cmd && (host->cmd->flags & MMC_RSP_BUSY)) {
			if (intmask & INTMSK_DTO) {
				mshci_finish_command(host);
				return;
			}
		}

		DBG(" Got data interrupt 0x%08x from %s "
			"even though no data operation was in progress.\n",
			 (unsigned)intmask,intr_src ? "MINT" : "IDMAC");
		return;
	}
	if (intr_src == INT_SRC_MINT) {
		if (intmask & INTMSK_HTO) {
			ERR(" Host timeout error\n");
			host->data->error = (unsigned int)-ETIMEDOUT;
		} else if (intmask & INTMSK_DRTO) {
			ERR(" Data read timeout error\n");
			host->data->error = (unsigned int)-ETIMEDOUT;
		} else if (intmask & INTMSK_SBE) {
			ERR(" Start bit error\n");
			host->data->error = (unsigned int)-EIO;
		} else if (intmask & INTMSK_EBE) {
			ERR(" Endbit/Write no CRC error\n");
			host->data->error = (unsigned int)-EIO;
		} else if (intmask & INTMSK_DCRC) {
			ERR(" Data CRC error\n");
			host->data->error = (unsigned int)-EIO;
		} else if (intmask & INTMSK_FRUN) {
			ERR(" FIFO underrun/overrun error\n");
			host->data->error = (unsigned int)-EIO;
		} 
	} else {
		if (intmask & IDSTS_FBE) {
			ERR(" Fatal Bus error on DMA\n");
			host->data->error = (unsigned int)-EIO;
		} else if (intmask & IDSTS_CES) {
			ERR(" Card error on DMA\n");
			host->data->error = (unsigned int)-EIO;
		} else if (intmask & IDSTS_DU) {
			ERR(" Description error on DMA\n");
			host->data->error = (unsigned int)-EIO;
		}
	}

	if (host->data->error) {
		mshci_finish_data(host);
	} else {
		if (!(host->flags & MSHCI_REQ_USE_DMA) &&
				(((host->data->flags & MMC_DATA_READ) &&
				(intmask & (INTMSK_RXDR | INTMSK_DTO))) ||
				((host->data->flags & MMC_DATA_WRITE) &&
					(intmask & (INTMSK_TXDR))))) {
					mshci_transfer_pio(host);
			}
		if (intmask & INTMSK_DTO) {
			if (host->cmd) {
				/*
				 * Data managed to finish before the
				 * command completed. Make sure we do
				 * things in the proper order.
				 */
				host->data_early = 1;
			} else {
				mshci_finish_data(host);
			}
		}
	}
}

static irqreturn_t mshci_irq(int irq, void *dev_id)
{
	irqreturn_t result;
	struct mshci_host *host = dev_id;
	u32 intmask;
	int cardint = 0;
	int timeout = 0x10000;

	spin_lock(&host->lock);
	intmask = mshci_readl(host, MSHCI_MINTSTS);
	if (!intmask || intmask == 0xffffffff) {
		/* check if there is a interrupt for IDMAC  */
		intmask = mshci_readl(host, MSHCI_IDSTS);
		if (intmask) {
			mshci_writel(host, intmask, MSHCI_IDSTS);
			mshci_data_irq(host, intmask, INT_SRC_IDMAC);
			result = IRQ_HANDLED;
			goto out;
			}
		result = IRQ_NONE;
		goto out;
	}

	mshci_writel(host, intmask, MSHCI_RINTSTS);

	if (intmask & (INTMSK_CDETECT))
		tasklet_schedule(&host->card_tasklet);

	intmask &= ~INTMSK_CDETECT;

	if (intmask & CMD_STATUS) {
		if (!(intmask & INTMSK_CDONE) && (intmask & INTMSK_RTO)) {
			/*
			 * when a error about command timeout occurs,
			 * cmd done intr comes together.
			 * cmd done intr comes later than error intr.
			 * so, it has to wait for cmd done intr.
			 */
			while (--timeout && !(mshci_readl(host, MSHCI_MINTSTS)
				& INTMSK_CDONE))
				; /* Nothing to do */
			if (!timeout)
				ERR(" time out for CDONE intr\n");
			else
				mshci_writel(host, INTMSK_CDONE,
					MSHCI_RINTSTS);
			mshci_cmd_irq(host, intmask & CMD_STATUS);
		} else {
			mshci_cmd_irq(host, intmask & CMD_STATUS);
		}
	}
	if (host->cmd && (host->cmd->opcode == SD_SWITCH_VOLTAGE) 
		&& (intmask & INTMSK_VOLT_SWITCH)) {
		host->voltage_int_count++;
		mshci_finish_command(host);
		intmask &= ~INTMSK_VOLT_SWITCH; 
	}

	if (intmask & DATA_STATUS) {
		if (!(intmask & INTMSK_DTO) && (intmask & INTMSK_DRTO)) {
			/*
			 * when a error about data timout occurs,
			 * DTO intr comes together.
			 * DTO intr comes later than error intr.
			 * so, it has to wait for DTO intr.
			 */
			while (--timeout && !(mshci_readl(host, MSHCI_MINTSTS)
				& INTMSK_DTO))
				; /* Nothing to do */
			if (!timeout)
				ERR(" time out for CDONE intr\n");
			else
				mshci_writel(host, INTMSK_DTO,
					MSHCI_RINTSTS);
			mshci_data_irq(host, intmask & DATA_STATUS,
							INT_SRC_MINT);
		} else {
			mshci_data_irq(host, intmask & DATA_STATUS,
							INT_SRC_MINT);
		}
	}

	intmask &= ~(CMD_STATUS | DATA_STATUS);

	if (intmask & SDIO_INT_ENABLE)
		cardint = 1;

	intmask &= ~SDIO_INT_ENABLE;

	if (intmask) {
		ERR(" Unexpected interrupt 0x%08x.\n", intmask);
		mshci_dumpregs(host);
	}

	result = IRQ_HANDLED;

	mmiowb();
out:
	spin_unlock(&host->lock);

	/* We have to delay this as it calls back into the driver */
	if (cardint)
		mmc_signal_sdio_irq(host->mmc);

	return result;
}

/* Suspend and Resume */

#ifdef CONFIG_PM

int mshci_suspend_host(struct mshci_host *host)
{
	int ret;
	ret = mmc_suspend_host(host->mmc);
	return ret;
}
EXPORT_SYMBOL_GPL(mshci_suspend_host);

static void mshci_resume_init(struct mshci_host *host);
int mshci_resume_host(struct mshci_host *host)
{
	int ret;
	if (host->ops->enable_clock) {
		host->ops->enable_clock(host, 1);
	}
	mshci_resume_init(host);
	if (host->ops->enable_clock) {
		host->ops->enable_clock(host, 0);
	}
	ret = mmc_resume_host(host->mmc);
	host->flags &= ~MSHCI_IN_TUNING;
	host->flags &= ~MSHCI_TUNING_DONE;
	return ret;
}
EXPORT_SYMBOL_GPL(mshci_resume_host);

#endif /* CONFIG_PM */

/* Device allocation and registration */

struct mshci_host *mshci_alloc_host(struct device *dev,
					size_t priv_size)
{
	struct mmc_host *mmc;
	struct mshci_host *host;

	WARN_ON(dev == NULL);

	mmc = mmc_alloc_host(sizeof(struct mshci_host) + priv_size, dev);
	if (!mmc)
		return ERR_PTR(-ENOMEM);

	host = mmc_priv(mmc);
	host->mmc = mmc;

	return host;
}

EXPORT_SYMBOL_GPL(mshci_alloc_host);

static void mshci_fifo_init(struct mshci_host *host)
{
	int fifo_val, fifo_depth, fifo_threshold;

	fifo_val = mshci_readl(host, MSHCI_FIFOTH);
	fifo_depth = ((fifo_val & RX_WMARK)>>16)+1+(fifo_val & TX_WMARK);
	
	fifo_threshold = FIFO_TH;
	host->fifo_threshold = fifo_threshold;
	host->fifo_depth = fifo_depth;

	fifo_val &= ~(RX_WMARK | TX_WMARK | MSIZE_MASK);

	fifo_val |= (fifo_threshold | ((fifo_threshold - 1) << 16));
	fifo_val |= MSIZE_8;

	mshci_writel(host, fifo_val, MSHCI_FIFOTH);

	DBG("fifo depth %d wmark for rx 0x%x wx 0x%x.\n", fifo_depth, 
		 ((fifo_val & RX_WMARK) >> 16) + 1,
		fifo_val & TX_WMARK);
}

static void mshci_resume_init(struct mshci_host *host)
{
	int count;

	mshci_init(host);

	mshci_fifo_init(host);

	/* set debounce filter value */
	mshci_writel(host, 0xfffff, MSHCI_DEBNCE);

	/* clear card type. set 1bit mode */
	mshci_writel(host, 0x0, MSHCI_CTYPE);

	/* set bus mode register for IDMAC */
	if (host->flags & MSHCI_USE_IDMA) {
		mshci_writel(host, BMOD_IDMAC_RESET, MSHCI_BMOD);
		count = 100;
		while ((mshci_readl(host, MSHCI_BMOD) & BMOD_IDMAC_RESET)
			&& --count)
			; /* nothing to do */

		mshci_writel(host, (mshci_readl(host, MSHCI_BMOD) |
				(BMOD_IDMAC_ENABLE|BMOD_IDMAC_FB)), MSHCI_BMOD);
	}
}



int mshci_add_host(struct mshci_host *host)
{
	struct mmc_host *mmc;
	int ret, count;

	WARN_ON(host == NULL);
	if (host == NULL)
		return -EINVAL;

	mmc = host->mmc;

	if (quirks)
		host->quirks = quirks;
	host->working = MMC_HOST_BUSY;
	
	mshci_reset_all(host);

	host->version = mshci_readl(host, MSHCI_VERID);

	host->flags &= ~MSHCI_IN_TUNING;
	host->flags &= ~MSHCI_TUNING_DONE;

	/* there are no reasons not to use DMA */
	host->flags |= MSHCI_USE_IDMA;

	if (host->flags & MSHCI_USE_IDMA) {
		/* We need to allocate descriptors for all sg entries
		 * DMA_SG_NUM transfer for each of those entries. */
		host->idma_desc = kmalloc(DMA_SG_NUM * sizeof(struct mshci_idmac),
					GFP_KERNEL);
		if (!host->idma_desc) {
			pr_warning(" Unable to allocate IDMA "
				"buffers. Falling back to standard DMA.\n");
			host->flags &= ~MSHCI_USE_IDMA;
		}
	}

	/*
	 * If we use DMA, then it's up to the caller to set the DMA
	 * mask, but PIO does not need the hw shim so we set a new
	 * mask here in that case.
	 */
	if (!(host->flags & (MSHCI_USE_IDMA))) {
		host->dma_mask = DMA_BIT_MASK(64);
		mmc_dev(host->mmc)->dma_mask = &host->dma_mask;
	}

	/* Set host parameters */
	if (host->ops->get_ro)
		mshci_ops.get_ro = host->ops->get_ro;

	if (!(host->quirks & MSHCI_QUIRK_TUNING_ENABLE)) {
		mshci_ops.execute_tuning = NULL;
	}

	mmc->ops = &mshci_ops;

	mmc->f_min = host->max_clk / 510;
	mmc->f_max = host->max_clk;

	if (mmc->ocr_avail == 0) {
		ERR(" Hardware doesn't report any support voltages.\n");
		return -ENODEV;
	}

	spin_lock_init(&host->lock);

	/* Maximum number of segments */
	mmc->max_segs = DMA_SG_NUM;

	/*
	 * Maximum number of sectors in one transfer. Limited by DMA boundary
	 * size (4KiB).
	 * Limited by CPU I/O boundry size (0xfffff000 KiB)
	 */

	/*
	 * to prevent starvation of a process that want to access SD device
	 * it should limit size that transfer at one time.
	 */
	mmc->max_req_size = 0x80000;

	/*
	 * Maximum segment size. Could be one segment with the maximum number
	 * of bytes. When doing hardware scatter/gather, each entry cannot
	 * be larger than 4 KiB though.
	 */
	if (host->flags & MSHCI_USE_IDMA)
		mmc->max_seg_size = 0x1000;
	else
		mmc->max_seg_size = mmc->max_req_size;

	/*
	 * from SD spec 2.0 and MMC spec 4.2, block size has been
	 * fixed to 512 byte
	 */
	mmc->max_blk_size = 0;

	mmc->max_blk_size = 512 << mmc->max_blk_size;

	/* Maximum block count */
	mmc->max_blk_count = 0xffff;

	/* Init tasklets */
	tasklet_init(&host->card_tasklet,
		mshci_tasklet_card, (unsigned long)host);
	tasklet_init(&host->finish_tasklet,
		mshci_tasklet_finish, (unsigned long)host);

	setup_timer(&host->timer, mshci_timeout_timer, (unsigned long)host);

	mshci_init(host);

	mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) & ~INT_ENABLE),
					MSHCI_CTRL);

	mshci_fifo_init(host);

	/* set debounce filter value */
	mshci_writel(host, 0xfffff, MSHCI_DEBNCE);

	/* clear card type. set 1bit mode */
	mshci_writel(host, 0x0, MSHCI_CTYPE);

	/* set bus mode register for IDMAC */
	if (host->flags & MSHCI_USE_IDMA) {
		mshci_writel(host, BMOD_IDMAC_RESET, MSHCI_BMOD);
		count = 100;
		while ((mshci_readl(host, MSHCI_BMOD) & BMOD_IDMAC_RESET)
			&& --count)
			; /* nothing to do */

		mshci_writel(host, (mshci_readl(host, MSHCI_BMOD) |
				(BMOD_IDMAC_ENABLE|BMOD_IDMAC_FB)), MSHCI_BMOD);

		/* clear idmac interrupts */
		mshci_writel(host, 0x3FF, MSHCI_IDSTS);
	}
	if(host->quirks & MSHCI_QUIRK_EXTERNAL_CARD_DETECTION){
		host->flags &= ~MSHCI_CARD_NEED_ENUM;
	}

	ret = request_irq(host->irq, mshci_irq, IRQF_SHARED,
		mmc_hostname(mmc), host);
	if (ret)
		goto untasklet;

	mmiowb();

	mmc_add_host(mmc);
	
#ifdef CONFIG_DEBUG_FS
	debugfs_create_x32("cap", (S_IRUSR | S_IWUSR),
				mmc->debugfs_root, (u32*)(&mmc->caps));
#endif

	host->working = MMC_HOST_FREE;

	device_create_file(mmc->parent, &dev_attr_debug);
	device_create_file(mmc->parent, &dev_attr_quirks);
	
	return 0;

untasklet:
	tasklet_kill(&host->card_tasklet);
	tasklet_kill(&host->finish_tasklet);
	host->working = MMC_HOST_FREE;

	return ret;
}
EXPORT_SYMBOL_GPL(mshci_add_host);

void mshci_remove_host(struct mshci_host *host, int dead)
{
	unsigned long flags;

	if (dead) {
		spin_lock_irqsave(&host->lock, flags);

		host->flags |= MSHCI_DEVICE_DEAD;

		if (host->mrq) {
			ERR(" Controller removed during transfer!\n");

			host->mrq->cmd->error = (unsigned int)-ENOMEDIUM;
			host->flags &= ~MSHCI_IN_TUNING;
			host->flags &= ~MSHCI_TUNING_DONE;
			tasklet_schedule(&host->finish_tasklet);
		}

		spin_unlock_irqrestore(&host->lock, flags);
	}

	mshci_disable_card_detection(host);

	mmc_remove_host(host->mmc);

	if (!dead)
		mshci_reset_all(host);

	free_irq(host->irq, host);

	del_timer_sync(&host->timer);
	tasklet_kill(&host->card_tasklet);
	tasklet_kill(&host->finish_tasklet);

	kfree(host->idma_desc);

	host->idma_desc = NULL;
	host->align_buffer = NULL;
	device_remove_file(host->mmc->parent, &dev_attr_debug);
	device_remove_file(host->mmc->parent, &dev_attr_quirks);
}
EXPORT_SYMBOL_GPL(mshci_remove_host);

void mshci_free_host(struct mshci_host *host)
{
	mmc_free_host(host->mmc);
}
EXPORT_SYMBOL_GPL(mshci_free_host);

/* Driver init and exit */

static int __init mshci_drv_init(void)
{
	return 0;
}

static void __exit mshci_drv_exit(void)
{
}

module_init(mshci_drv_init);
module_exit(mshci_drv_exit);


#ifdef MODULE
MODULE_AUTHOR("Hyunsung Jang <hs79.jang@samsung.com>");
MODULE_AUTHOR("Hyuk Lee <hyuk1.lee@samsung.com>");
MODULE_DESCRIPTION("Mobile Storage Host Controller Interface core driver");
MODULE_LICENSE("GPL");
#endif
 /*lint -restore*/

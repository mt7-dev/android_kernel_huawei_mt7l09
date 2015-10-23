/*
 * Designware SPI core controller driver (refer pxa2xx_spi.c)
 *
 * Copyright (c) 2009, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/suspend.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_balong.h>
#include <linux/scatterlist.h>
#include "bsp_edma.h"
#include <product_config.h>
#include <bsp_memmap.h>

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#endif

#define START_STATE ((void *)0)
#define RUNNING_STATE   ((void *)1)
#define DONE_STATE  ((void *)2)
#define ERROR_STATE ((void *)-1)

#define QUEUE_RUNNING   0
#define QUEUE_STOPPED   1

#define MRST_SPI_DEASSERT   0
#define MRST_SPI_ASSERT     1

#define SPI_OK   0
#define SPI_ERROR -1

#define DRIVER_NAME "balong_spi_v7r2"

struct balong_spi_v7r2 {
    struct balong_spi  balongs;
    struct clk     *clk;
    unsigned int        suspended:1;        /*add for pm*/
};

/* Slave spi_dev related */
struct chip_data {
    u16 cr0;
    u8 cs;          /* chip select pin */
    u8 n_bytes;     /* current is a 1/2/4 byte op */
    u8 tmode;       /* TR/TO/RO/EEPROM */
    u8 type;        /* SPI/SSP/MicroWire */

    u8 poll_mode;       /* 1 means use poll mode */

    u32 dma_width;
    u32 rx_threshold;
    u32 tx_threshold;
    u8 enable_dma;
    u8 bits_per_word;
    u16 clk_div;        /* baud rate divider */
    u32 speed_hz;       /* baud rate */
    int (*write)(struct dw_spi *dws);
    int (*read)(struct dw_spi *dws);
    void (*cs_control)(u32 command);
};

#ifdef CONFIG_DEBUG_FS
#define SPI_REGS_BUFSIZE    1024
static ssize_t  spi_show_regs(struct file *file, char __user *user_buf,
                size_t count, loff_t *ppos)
{
    struct balong_spi *balongs;
    char *buf;
    u32 len = 0;
    ssize_t ret;

    balongs = file->private_data;

    buf = kzalloc(SPI_REGS_BUFSIZE, GFP_KERNEL);
    if (!buf)
        return 0;

    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "MRST SPI0 registers:\n");
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "=================================\n");
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "CTRL0: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_CTRLR0_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "CTRL1: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_CTRLR1_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "SSIENR: \t0x%08x\n", balong_readl(balongs, HI_SPI_SSIENR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "SER: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_SER_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "BAUDR: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_BAUDR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "TXFTLR: \t0x%08x\n", balong_readl(balongs, HI_SPI_TXFTLR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "RXFTLR: \t0x%08x\n", balong_readl(balongs, HI_SPI_RXFTLR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "TXFLR: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_TXFLR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "RXFLR: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_RXFLR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "SR: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_SR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "IMR: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_IMR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "ISR: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_ISR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "DMACR: \t\t0x%08x\n", balong_readl(balongs, HI_SPI_DMACR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "DMATDLR: \t0x%08x\n", balong_readl(balongs, HI_SPI_DMATDLR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "DMARDLR: \t0x%08x\n", balong_readl(balongs, HI_SPI_DMARDLR_OFFSET));
    len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
            "=================================\n");

    ret =  simple_read_from_buffer(user_buf, count, ppos, buf, len);
    kfree(buf);
    return ret;
}

static const struct file_operations mrst_spi_regs_ops = {
    .owner      = THIS_MODULE,
    .open       = simple_open,
    .read       = spi_show_regs,
    .llseek     = default_llseek,
};

static int mrst_spi_debugfs_init(struct balong_spi *balongs)
{
    balongs->debugfs = debugfs_create_dir("mrst_spi", NULL);
    if (!balongs->debugfs)
        return -ENOMEM;

    debugfs_create_file("registers", S_IFREG | S_IRUGO,
        balongs->debugfs, (void *)balongs, &mrst_spi_regs_ops);
    return 0;
}

static void mrst_spi_debugfs_remove(struct balong_spi *balongs)
{
    if (balongs->debugfs)
        debugfs_remove_recursive(balongs->debugfs);
}

#else
static inline int mrst_spi_debugfs_init(struct balong_spi *balongs)
{
    return 0;
}

static inline void mrst_spi_debugfs_remove(struct balong_spi *balongs)
{
}
#endif /* CONFIG_DEBUG_FS */

/* Return the max entries we can fill into tx fifo */
static inline u32 tx_max(struct balong_spi *balongs)
{
    u32 tx_left, tx_room, rxtx_gap;

    tx_left = (balongs->tx_end - balongs->tx) / balongs->n_bytes;
    tx_room = balongs->fifo_len - balong_readw(balongs, HI_SPI_TXFLR_OFFSET);

    /*
     * Another concern is about the tx/rx mismatch, we
     * though to use (balongs->fifo_len - rxflr - txflr) as
     * one maximum value for tx, but it doesn't cover the
     * data which is out of tx/rx fifo and inside the
     * shift registers. So a control from sw point of
     * view is taken.
     */
    rxtx_gap =  ((balongs->rx_end - balongs->rx) - (balongs->tx_end - balongs->tx))
            / balongs->n_bytes;

    return min3(tx_left, tx_room, (u32) (balongs->fifo_len - rxtx_gap));
}

/* Return the max entries we should read out of rx fifo */
static inline u32 rx_max(struct balong_spi *balongs)
{
    u32 rx_left = (balongs->rx_end - balongs->rx) / balongs->n_bytes;

    return min(rx_left, (u32)balong_readw(balongs, HI_SPI_RXFLR_OFFSET));
}

//static void balong_writer(struct balong_spi *balongs)
void balong_writer(struct balong_spi *balongs)
{
    u32 max = tx_max(balongs);
    u16 txw = 0;

    while (max--) {
        /* Set the tx word if the transfer's original "tx" is not null */
        if (balongs->tx_end - balongs->len) {
        //if (balongs->tx_end - balongs->tx) {
            if (balongs->n_bytes == 1)
                txw = *(u8 *)(balongs->tx);
            else
                txw = *(u16 *)(balongs->tx);
        }
        balong_writew(balongs, HI_SPI_DR_OFFSET, txw);
        balongs->tx += balongs->n_bytes;
    }
}
//static void balong_reader(struct balong_spi *balongs)
void balong_reader(struct balong_spi *balongs)
{
    u32 max = rx_max(balongs);
    u16 rxw;

    while (max--) {
        rxw = balong_readw(balongs, HI_SPI_DR_OFFSET);
        /* Care rx only if the transfer's original "rx" is not null */
        if (balongs->rx_end - balongs->len) {
        //if (balongs->rx_end - balongs->rx) {
            if (balongs->n_bytes == 1)
                *(u8 *)(balongs->rx) = rxw;
            else
                *(u16 *)(balongs->rx) = rxw;
        }
        balongs->rx += balongs->n_bytes;
    }
}

static void *next_transfer(struct balong_spi *balongs)
{
    struct spi_message *msg = balongs->cur_msg;
    struct spi_transfer *trans = balongs->cur_transfer;

    /* Move to next transfer */
    if (trans->transfer_list.next != &msg->transfers) {
        balongs->cur_transfer =
            list_entry(trans->transfer_list.next,
                    struct spi_transfer,
                    transfer_list);
        return RUNNING_STATE;
    } else
        return DONE_STATE;
}

/*
 * Note: first step is the protocol driver prepares
 * a dma-capable memory, and this func just need translate
 * the virt addr to physical
 */
static int map_dma_buffers(struct balong_spi *balongs)
{
    if (/*!balongs->cur_msg->is_dma_mapped
        || */!balongs->dma_inited
        || !balongs->cur_chip->enable_dma
        || !balongs->dma_ops)
        return 0;

    if (balongs->cur_transfer->tx_dma)
        balongs->tx_dma = balongs->cur_transfer->tx_dma;

    if (balongs->cur_transfer->rx_dma)
        balongs->rx_dma = balongs->cur_transfer->rx_dma;

    return 1;
}

/* Caller already set message->status; dma and pio irqs are blocked */
static void giveback(struct balong_spi *balongs)
{
    struct spi_transfer *last_transfer;
    unsigned long flags;
    struct spi_message *msg;

    spin_lock_irqsave(&balongs->lock, flags);
    msg = balongs->cur_msg;
    balongs->cur_msg = NULL;
    balongs->cur_transfer = NULL;
    balongs->prev_chip = balongs->cur_chip;
    balongs->cur_chip = NULL;
    balongs->dma_mapped = 0;
    queue_work(balongs->workqueue, &balongs->pump_messages);
    spin_unlock_irqrestore(&balongs->lock, flags);

    last_transfer = list_entry(msg->transfers.prev,
                    struct spi_transfer,
                    transfer_list);

    if (!last_transfer->cs_change && balongs->cs_control)
    //if (!last_transfer->cs_change)
        balongs->cs_control(MRST_SPI_DEASSERT);

    msg->state = NULL;
    if (msg->complete)
        msg->complete(msg->context);
}

static void int_error_stop(struct balong_spi *balongs, const char *msg)
{
    /* Stop the hw */
    spi_enable_chip(balongs, 0);

    dev_err(&balongs->master->dev, "%s\n", msg);
    balongs->cur_msg->state = ERROR_STATE;
    tasklet_schedule(&balongs->pump_transfers);
}

void balong_spi_xfer_done(struct balong_spi *balongs)
{
    /* Update total byte transferred return count actual bytes read */
    balongs->cur_msg->actual_length += balongs->len;

    /* Move to next transfer */
    balongs->cur_msg->state = next_transfer(balongs);

    /* Handle end of message */
    if (balongs->cur_msg->state == DONE_STATE) {
        balongs->cur_msg->status = 0;
        giveback(balongs);
    } else
        tasklet_schedule(&balongs->pump_transfers);
}
EXPORT_SYMBOL_GPL(balong_spi_xfer_done);

static irqreturn_t interrupt_transfer(struct balong_spi *balongs)
{
    u16 irq_status = balong_readw(balongs, HI_SPI_ISR_OFFSET);

    /* Error handling */
    if (irq_status & (SPI_INT_TXOI | SPI_INT_RXOI | SPI_INT_RXUI)) {
        balong_readw(balongs, HI_SPI_TXOICR_OFFSET);
        balong_readw(balongs, HI_SPI_RXOICR_OFFSET);
        balong_readw(balongs, HI_SPI_RXUICR_OFFSET);
        int_error_stop(balongs, "interrupt_transfer: fifo overrun/underrun");
        return IRQ_HANDLED;
    }

    balong_reader(balongs);
    if (balongs->rx_end == balongs->rx) {
        spi_mask_intr(balongs, SPI_INT_TXEI);
        balong_spi_xfer_done(balongs);
        return IRQ_HANDLED;
    }
    if (irq_status & SPI_INT_TXEI) {
        spi_mask_intr(balongs, SPI_INT_TXEI);
        balong_writer(balongs);
        /* Enable TX irq always, it will be disabled when RX finished */
        spi_umask_intr(balongs, SPI_INT_TXEI);
    }

    return IRQ_HANDLED;
}

s32 balong_spi_edma_init(struct balong_spi *balongs)
{
    s32 ret = 0;

    balongs->dma_inited = 1;

    return SPI_OK;
}



s32 balong_spi_edma_transfer(struct balong_spi *balongs)
{
    edma_addr_t  edma_addr    = 0;
    s32         ret          = 0;
    u32      i            = 0;
    u32 ilinum = 0;
    u32 ililast = 0;
    struct edma_cb *pstNode   = NULL;
    struct edma_cb *psttemp   = NULL;
    struct edma_cb *FirstNode   = NULL;
    if(balongs->len <= 0)
    {
        dev_err(&balongs->master->dev,
            "balong SPI: balong_edma_transfer  failed，lens is wrong!\n");
        return SPI_ERROR;
    }
     balongs->dma_channel = bsp_edma_channel_init(EDMA_SPI1_TX,NULL,NULL,NULL);
    if(balongs->dma_channel<0)
    {
        dev_err(&balongs->master->dev,
        "balong SPI: bsp_edma_channel_init failed!\n");
        return SPI_ERROR;
    }

    balongs->tx_dma = dma_map_single(&balongs->cur_dev->dev, (void *)balongs->tx, balongs->len,DMA_TO_DEVICE);

    if( balongs->len < 0x10000 )
    {/*若一次需传输的长度小于等于一维传输长度最大值0X10000-1，则进行一次一维传输*/

        bsp_edma_channel_set_config (balongs->dma_channel, EDMA_M2P, EDMA_TRANS_WIDTH_32, EDMA_BUR_LEN_16);

        ret = bsp_edma_channel_async_start(balongs->dma_channel, balongs->tx_dma, balongs->dma_addr, balongs->len);
        if(ret){
            dev_err(&balongs->master->dev,
                "balong SPI: bsp_edma_channel_async_start  failed!\n");
            goto early_exit;
        }
        while(EDMA_CHN_BUSY == bsp_edma_channel_is_idle(balongs->dma_channel))
        {
            ;
        }
   }
    else
   {/*若一次需传输的长度大于一维传输长度最大值，则进行链表传输*/

       ilinum = (balongs->len) / (0xf000) + 1;/*链表节点数目*/
       ililast = (balongs->len) % (0xf000);/*最后一个节点传输长度*/

       /*创建链表*/
       pstNode = (struct edma_cb *)dma_alloc_coherent(NULL,(ilinum * sizeof(struct edma_cb)),
                                                                       &edma_addr, GFP_DMA|__GFP_WAIT);

       if (NULL == pstNode)
       {
           ret = SPI_ERROR;
           goto early_exit;
       }

       FirstNode = pstNode;
       psttemp = pstNode;
       for (i = 0; i <= ilinum - 1; i++)
       {
           psttemp->lli = EDMA_SET_LLI(edma_addr + (i+1) * sizeof(struct edma_cb), ((i < ilinum - 1)?0:1));
           psttemp->config = EDMA_SET_CONFIG( EDMA_SPI1_TX,  EDMA_M2P,  EDMA_TRANS_WIDTH_32 , EDMA_BUR_LEN_16);//& (~(1<<30));
           psttemp->src_addr = balongs->tx_dma +i * (0xf000);
           psttemp->des_addr = balongs->dma_addr;
           psttemp->cnt0 = ((i < ilinum - 1)?(0xf000):ililast);
           psttemp->bindx = 0;
           psttemp->cindx = 0;
           psttemp->cnt1  = 0;

           psttemp++;
       }
        /*获取首节点寄存器地址*/
       psttemp = bsp_edma_channel_get_lli_addr((u32)balongs->dma_channel);
       if (NULL == psttemp)
       {
         ret = SPI_ERROR;
         goto early_exit;
       }


        /*配置首节点寄存器*/
        psttemp->lli = FirstNode->lli;
        psttemp->config = FirstNode->config & 0xFFFFFFFE;
        psttemp->src_addr = FirstNode->src_addr;  /*物理地址*/
        psttemp->des_addr = FirstNode->des_addr;  /*物理地址*/
        psttemp->cnt0 = FirstNode->cnt0;
        psttemp->bindx = 0;
        psttemp->cindx = 0;
        psttemp->cnt1  = 0;


        /*启动EDMA传输后即返回，通过查询通道是否busy来确定传输是否完成*/
        if (bsp_edma_channel_lli_async_start((u32)balongs->dma_channel))
        {
            ret = SPI_ERROR;
            goto early_exit0;
        }

        while(EDMA_CHN_BUSY == bsp_edma_channel_is_idle(balongs->dma_channel))
        {
            ;
        }

    }

    balong_spi_xfer_done(balongs);

early_exit0:
    dma_free_coherent(NULL,(ilinum * sizeof(struct edma_cb)),
                              (void*)pstNode, edma_addr);
early_exit:
    dma_unmap_single(&balongs->cur_dev->dev, (void *)balongs->tx, balongs->len,DMA_TO_DEVICE);
    return ret;

}

void balong_spi_edma_exit(struct balong_spi *balongs)
{
    s32 ret = 0;
    ret = bsp_edma_channel_free (balongs->dma_channel);
    if(ret < 0 )
       dev_err(&balongs->master->dev,
        "balong SPI: bsp_edma_channel_free_spi1 failed!\n");

    return ;
}
static irqreturn_t balong_spi_irq(int irq, void *dev_id)
{
    struct balong_spi *balongs = dev_id;
    u16 irq_status = balong_readw(balongs, HI_SPI_ISR_OFFSET) & 0x3f;

    if (!irq_status)
        return IRQ_NONE;

    if (!balongs->cur_msg) {
        spi_mask_intr(balongs, SPI_INT_TXEI);
        return IRQ_HANDLED;
    }

    return balongs->transfer_handler(balongs);
}

/*add begin*/
static void wait_till_not_busy(struct balong_spi *dws)
{
    unsigned long end = jiffies + 1 + usecs_to_jiffies(1000);

    while (time_before(jiffies, end)) {
        if (!(balong_readw(dws, HI_SPI_SR_OFFSET) & SR_BUSY))
            return;
    }
    dev_err(&dws->master->dev,
        "balong SPI: Status keeps busy for 1000us after a read/write!\n");
}
int null_writer(struct balong_spi *dws)
{
    u8 n_bytes = dws->n_bytes;

    if (!(balong_readw(dws, HI_SPI_SR_OFFSET) & SR_TF_NOT_FULL)
        || (dws->tx == dws->tx_end))
        return 0;
    //balong_writew(dws, dr, 0);
    dws->tx += n_bytes;

    wait_till_not_busy(dws);
    return 1;
}

int null_reader(struct balong_spi *dws)
{
    u8 n_bytes = dws->n_bytes;

    while ((balong_readw(dws, HI_SPI_SR_OFFSET) & SR_RF_NOT_EMPT)
        && (dws->rx < dws->rx_end)) {
    //  balong_readw(dws, dr);
        dws->rx += n_bytes;
    }
    wait_till_not_busy(dws);
    return dws->rx == dws->rx_end;
}

int u8_writer(struct balong_spi *dws)
{
    /*printk(KERN_ERR "u8_writer begin!\n");*/
    if (!(balong_readw(dws, HI_SPI_SR_OFFSET) & SR_TF_NOT_FULL)
        || (dws->tx == dws->tx_end))
        return 0;

    balong_writew(dws, HI_SPI_DR_OFFSET, *(u8 *)(dws->tx));
    ++dws->tx;

    wait_till_not_busy(dws);
    return 1;
}

int u8_reader(struct balong_spi *dws)
{
    while ((balong_readw(dws, HI_SPI_SR_OFFSET) & SR_RF_NOT_EMPT)
        && (dws->rx < dws->rx_end)) {
        *(u8 *)(dws->rx) = balong_readw(dws, HI_SPI_DR_OFFSET);
        ++dws->rx;
    }

    wait_till_not_busy(dws);
    return dws->rx == dws->rx_end;
}


int u16_writer(struct balong_spi *dws)
{
    if (!(balong_readw(dws, HI_SPI_SR_OFFSET) & SR_TF_NOT_FULL)
        || (dws->tx == dws->tx_end))
        return 0;

    if(dws->bus_num){
        balong_writew(dws, HI_SPI_DR_OFFSET, *((u16 *)dws->tx));
        dws->tx += 4;
        wait_till_not_busy(dws);
    }
    else{

        /*balong_writew(dws, dr, *(u16 *)(dws->tx));*/
        balong_writew(dws, HI_SPI_DR_OFFSET, *(u16 *)(dws->tx));
        dws->tx += 2;
        wait_till_not_busy(dws);
    }


    return 1;
}

int u16_reader(struct balong_spi *dws)
{
    u16 temp;

    while ((balong_readw(dws, HI_SPI_SR_OFFSET) & SR_RF_NOT_EMPT)
        && (dws->rx < dws->rx_end)) {
        temp = balong_readw(dws, HI_SPI_DR_OFFSET);
        *(u16 *)(dws->rx) = temp;
        dws->rx += 2;
    }

    wait_till_not_busy(dws);
    return dws->rx == dws->rx_end;
}
/*add end*/
/* Must be called inside pump_transfers() */
//static void poll_transfer(struct balong_spi *balongs)
void poll_transfer(struct balong_spi *balongs)
{

#if 0
    do {
        balong_writer(balongs);
        balong_reader(balongs);
        cpu_relax();
    } while (balongs->rx_end > balongs->rx);
#endif

    if(balongs->bus_num == 1)/*如果是SPI1，LCD模块使用，只写不读*/
    {
        while (balongs->write(balongs))
            {;}
        /*写完所有transfer中需要发送的信息后，再检查总线忙闲状态,会导致SPI FIFO满*/

    }
    else
    {

        while (balongs->write(balongs))
        balongs->read(balongs);
    }

    balong_spi_xfer_done(balongs);
}
//static void pump_transfers(unsigned long data)
void pump_transfers(unsigned long data)
{
    struct balong_spi *balongs = (struct balong_spi *)data;
    struct spi_message *message = NULL;
    struct spi_transfer *transfer = NULL;
    struct spi_transfer *previous = NULL;
    struct spi_transfer *trans_read = NULL;
    struct spi_device *spi = NULL;
    struct chip_data *chip = NULL;
    u32 ret = 0;
    u8 bits = 0;
    u8 imask = 0;
    u8 cs_change = 0;
    u16 txint_level = 0;
    u16 clk_div = 0;
    u32 speed = 0;
    u32 cr0 = 0;
    u32 rx_len = 0x01;
    u16 dma_level = 0;


    /* Get current state information */
    message = balongs->cur_msg;
    transfer = balongs->cur_transfer;
    chip = balongs->cur_chip;
    spi = message->spi;

    if (unlikely(!chip->clk_div))
        chip->clk_div = balongs->max_freq / chip->speed_hz;

    if (message->state == ERROR_STATE) {
        message->status = -EIO;
        goto early_exit;
    }

    /* Handle end of message */
    if (message->state == DONE_STATE) {
        message->status = 0;
        goto early_exit;
    }

    /* Delay if requested at end of transfer*/
    if (message->state == RUNNING_STATE) {
        previous = list_entry(transfer->transfer_list.prev,
                    struct spi_transfer,
                    transfer_list);
        if (previous->delay_usecs)
            udelay(previous->delay_usecs);
    }

    balongs->n_bytes = chip->n_bytes;
    balongs->dma_width = chip->dma_width;
    balongs->cs_control = chip->cs_control;

    balongs->rx_dma = transfer->rx_dma;
    balongs->tx_dma = transfer->tx_dma;
    balongs->tx = (void *)transfer->tx_buf;
    balongs->tx_end = balongs->tx + transfer->len;
    balongs->rx = transfer->rx_buf;
    balongs->rx_end = balongs->rx + transfer->len;
/*add begin*/
    balongs->write = balongs->tx ? chip->write : null_writer;
    balongs->read = balongs->rx ? chip->read : null_reader;
/*add end*/
    balongs->cs_change = transfer->cs_change;
    balongs->len = balongs->cur_transfer->len;
    if (chip != balongs->prev_chip)
        cs_change = 1;


    cr0 = chip->cr0;
    /* Handle per transfer options for bpw and speed */
    if (transfer->speed_hz) {
        speed = chip->speed_hz;

        if (transfer->speed_hz != speed) {
            speed = transfer->speed_hz;
            if (speed > balongs->max_freq) {
                printk(KERN_ERR "MRST SPI0: unsupported"
                    "freq: %dHz\n", speed);
                message->status = -EIO;
                goto early_exit;
            }

            /* clk_div doesn't support odd number */
            clk_div = balongs->max_freq / speed;
            clk_div = (clk_div + 1) & 0xfffe;

            chip->speed_hz = speed;
            chip->clk_div = clk_div;
        }
    }
    if (transfer->bits_per_word) {
        bits = transfer->bits_per_word;
        printk(KERN_ERR "transfer->bits_per_word: %02x!\n",  bits);
        switch (bits) {
        case 8:
            balongs->read = (balongs->read != null_reader) ?
                    u8_reader : null_reader;
            balongs->write = (balongs->write != null_writer) ?
                    u8_writer : null_writer;
            balongs->n_bytes = balongs->dma_width = bits >> 3;
            break;
        case 16:
            balongs->read = (balongs->read != null_reader) ?
                    u16_reader : null_reader;
            balongs->write = (balongs->write != null_writer) ?
                    u16_writer : null_writer;
            balongs->n_bytes = balongs->dma_width = bits >> 3;
            break;
        default:
            printk(KERN_ERR "MRST SPI0: unsupported bits:"
                "%db\n", bits);
            message->status = -EIO;
            goto early_exit;
        }

        cr0 = (bits - 1)
            | (chip->type << SPI_FRF_OFFSET)
            | (spi->mode << SPI_MODE_OFFSET)
            | (chip->tmode << SPI_TMOD_OFFSET);
    }

        /* search the transfer of rx,get the rx_len*/
    if (message->state == START_STATE) {

        list_for_each_entry(trans_read,&message->transfers,transfer_list)
        {
            if(trans_read->rx_buf)
            {
                rx_len = trans_read->len / balongs->n_bytes;
                /*balongs->cur_chip->tmode =  SPI_TMOD_TR;
                balongs->cur_chip->cr0  &= ~SPI_TMOD_MASK;
                balongs->cur_chip->cr0 |= (balongs->cur_chip->tmode << SPI_TMOD_OFFSET);*/
                chip->tmode = SPI_TMOD_TR;
                cr0 &= ~SPI_TMOD_MASK;
                cr0 |= (chip->tmode << SPI_TMOD_OFFSET);
                /*printk(KERN_ERR "rx_len: %02x!\n",  rx_len);
                printk(KERN_ERR "chip->tmode: %02x!\n",  chip->tmode);
                printk(KERN_ERR "cr0: %02x!\n",  cr0);*/
                break;
            }

        }
    }



    /*
     * Adjust transfer mode if necessary. Requires platform dependent
     * chipselect mechanism.
     */
    if (balongs->cs_control) {
        if (balongs->rx && balongs->tx)
            chip->tmode = SPI_TMOD_TR;
        else if (balongs->rx)
            chip->tmode = SPI_TMOD_RO;
        else
            chip->tmode = SPI_TMOD_TO;

        cr0 &= ~SPI_TMOD_MASK;
        cr0 |= (chip->tmode << SPI_TMOD_OFFSET);
        printk(KERN_ERR "balongs->cs_control not null!\n");
    }

    /* Check if current transfer is a DMA transaction */
    balongs->dma_mapped = map_dma_buffers(balongs);

    /*
     * Interrupt mode
     * we only need set the TXEI IRQ, as TX/RX always happen syncronizely
     */
    if (!balongs->dma_mapped && !chip->poll_mode) {
        int templen = balongs->len / balongs->n_bytes;
        txint_level = balongs->fifo_len / 2;
        txint_level = (templen > txint_level) ? txint_level : templen;

        imask |= SPI_INT_TXEI | SPI_INT_TXOI | SPI_INT_RXUI | SPI_INT_RXOI;
        balongs->transfer_handler = interrupt_transfer;
    }

    /*
     * Reprogram registers only if
     *  1. chip select changes
     *  2. clk_div is changed
     *  3. control value changes
     */
    if (message->state == START_STATE && (balong_readw(balongs, HI_SPI_CTRLR0_OFFSET) != cr0 || cs_change || clk_div || imask)){
        spi_enable_chip(balongs, 0);

        if (balong_readw(balongs, HI_SPI_CTRLR0_OFFSET) != cr0)
            balong_writew(balongs, HI_SPI_CTRLR0_OFFSET, cr0);

        /*set the len of tx*/
        if ((rx_len >= 1) && (balong_readw(balongs, HI_SPI_CTRLR1_OFFSET) != (rx_len -1)))
            balong_writew(balongs, HI_SPI_CTRLR1_OFFSET, (rx_len-1));

        spi_set_clk(balongs, clk_div ? clk_div : chip->clk_div);
        spi_chip_sel(balongs, spi->chip_select);

        /* Set the interrupt mask, for poll mode just disable all int */
        spi_mask_intr(balongs, 0xff);


        if (balongs->dma_mapped){


            dma_level = 10;
            balong_writew(balongs,HI_SPI_DMATDLR_OFFSET,dma_level);
            ret = balong_readw(balongs, HI_SPI_DMACR_OFFSET) |(1<<1);
            balong_writew(balongs, HI_SPI_DMACR_OFFSET,ret);


        }
        else{
        if (imask)
            spi_umask_intr(balongs, imask);
        if (txint_level)
            balong_writew(balongs, HI_SPI_TXFTLR_OFFSET, txint_level);
        }
        spi_enable_chip(balongs, 1);
        if (cs_change)
            balongs->prev_chip = chip;
    }
    message->state = RUNNING_STATE;
    /*printk(KERN_ERR "HI_SPI_CTRLR0_OFFSET: %02x!\n", balong_readw(balongs, HI_SPI_CTRLR0_OFFSET));
    printk(KERN_ERR "HI_SPI_CTRLR1_OFFSET: %02x!\n", balong_readw(balongs, HI_SPI_CTRLR1_OFFSET));*/

    if ((balongs->dma_mapped)&&(balongs->len >=100)){


        ret = balongs->dma_ops->dma_transfer(balongs);
        if(ret){
            printk(KERN_ERR "dma_transfer failed\n");
            goto edma_transfer_exit;
        }
        balongs->dma_ops->dma_exit(balongs);
        return;
    }

    if (chip->poll_mode)
        poll_transfer(balongs);

    return;
edma_transfer_exit:
    balongs->dma_ops->dma_exit(balongs);
early_exit:
    giveback(balongs);
    return;
}

static void pump_messages(struct work_struct *work)
{
    struct balong_spi *balongs =
        container_of(work, struct balong_spi, pump_messages);
    unsigned long flags;

    /* Lock queue and check for queue work */
    spin_lock_irqsave(&balongs->lock, flags);
    if (list_empty(&balongs->queue) || balongs->run == QUEUE_STOPPED) {
        balongs->busy = 0;
        spin_unlock_irqrestore(&balongs->lock, flags);
        return;
    }

    /* Make sure we are not already running a message */
    if (balongs->cur_msg) {
        spin_unlock_irqrestore(&balongs->lock, flags);
        return;
    }

    /* Extract head of queue */
    balongs->cur_msg = list_entry(balongs->queue.next, struct spi_message, queue);
    list_del_init(&balongs->cur_msg->queue);

    /* Initial message state*/
    balongs->cur_msg->state = START_STATE;
    balongs->cur_transfer = list_entry(balongs->cur_msg->transfers.next,
                        struct spi_transfer,
                        transfer_list);
    balongs->cur_chip = spi_get_ctldata(balongs->cur_msg->spi);

    /* Mark as busy and launch transfers */
    tasklet_schedule(&balongs->pump_transfers);

    balongs->busy = 1;
    spin_unlock_irqrestore(&balongs->lock, flags);
}

/* spi_device use this to queue in their spi_msg */
static int balong_spi_transfer(struct spi_device *spi, struct spi_message *msg)
{
    struct balong_spi *balongs = spi_master_get_devdata(spi->master);
    unsigned long flags;

    if(NULL == balongs)
    {
        printk(KERN_ERR "balong_spi_transfer:balongs is null! \n");
        return -1;
    }

    spin_lock_irqsave(&balongs->lock, flags);

    if (balongs->run == QUEUE_STOPPED) {
        spin_unlock_irqrestore(&balongs->lock, flags);
        return -ESHUTDOWN;
    }

    msg->actual_length = 0;
    msg->status = -EINPROGRESS;
    msg->state = START_STATE;

    list_add_tail(&msg->queue, &balongs->queue);

    if (balongs->run == QUEUE_RUNNING && !balongs->busy) {

        if (balongs->cur_transfer || balongs->cur_msg)
            queue_work(balongs->workqueue,
                    &balongs->pump_messages);
        else {
            /* If no other data transaction in air, just go */
            spin_unlock_irqrestore(&balongs->lock, flags);
            pump_messages(&balongs->pump_messages);
            return 0;
        }
    }

    spin_unlock_irqrestore(&balongs->lock, flags);
    return 0;
}

/* This may be called twice for each spi dev */
static int balong_spi_setup(struct spi_device *spi)
{
    struct balong_spi_chip *chip_info = NULL;
    struct chip_data *chip;
    int first_flag = 0;
    int status = 0;

    /* Only alloc on first setup */
    chip = spi_get_ctldata(spi);
    if (!chip) {
        chip = kzalloc(sizeof(struct chip_data), GFP_KERNEL);
        first_flag = 1;
        if (!chip)
            return -ENOMEM;
    }

    /*
     * Protocol drivers may change the chip settings, so...
     * if chip_info exists, use it
     */
    chip_info = spi->controller_data;

    /* chip_info doesn't always exist */
    if (chip_info) {
        if (chip_info->cs_control)
            chip->cs_control = chip_info->cs_control;

        chip->poll_mode = chip_info->poll_mode;
        chip->type = chip_info->type;

        chip->rx_threshold = 0;
        chip->tx_threshold = 0;

        chip->enable_dma = chip_info->enable_dma;
    }

    if (spi->bits_per_word <= 8) {
        chip->n_bytes = 1;
        chip->dma_width = 1;
        chip->read = u8_reader;
        chip->write = u8_writer;
    } else if (spi->bits_per_word <= 16) {
        chip->n_bytes = 2;
        chip->dma_width = 2;
        chip->read = u16_reader;
        chip->write = u16_writer;
    } else {
        /* Never take >16b case for MRST SPIC */
        dev_err(&spi->dev, "invalid wordsize\n");
        /*return -EINVAL;*/
        status = -EINVAL;
        goto exit_early;
    }
    chip->bits_per_word = spi->bits_per_word;

    if (!spi->max_speed_hz) {
        dev_err(&spi->dev, "No max speed HZ parameter\n");
        /*return -EINVAL;*/
        status = -EINVAL;
        goto exit_early;
    }
    chip->speed_hz = spi->max_speed_hz;

    //chip->tmode = 0x00; /* Tx & Rx */
    chip->tmode = 0x01; /* Tx*/
    /* Default SPI mode is SCPOL = 0, SCPH = 0 */
    chip->cr0 = (chip->bits_per_word - 1)
            | (chip->type << SPI_FRF_OFFSET)
            | (spi->mode  << SPI_MODE_OFFSET)
            | (chip->tmode << SPI_TMOD_OFFSET);

    spi_set_ctldata(spi, chip);
    /*return 0;*/
    goto exit;

exit_early :
    if(first_flag)
        kfree(chip);
exit:
    return status;
}

static void balong_spi_cleanup(struct spi_device *spi)
{
    struct chip_data *chip = spi_get_ctldata(spi);
    kfree(chip);
}

static int __devinit init_queue(struct balong_spi *balongs)
{
    INIT_LIST_HEAD(&balongs->queue);
    spin_lock_init(&balongs->lock);

    balongs->run = QUEUE_STOPPED;
    balongs->busy = 0;

    tasklet_init(&balongs->pump_transfers,
            pump_transfers, (unsigned long)balongs);

    INIT_WORK(&balongs->pump_messages, pump_messages);
    balongs->workqueue = create_singlethread_workqueue(
                    dev_name(balongs->master->dev.parent));
    if (balongs->workqueue == NULL)
        return -EBUSY;

    return 0;
}

static int start_queue(struct balong_spi *balongs)
{
    unsigned long flags;

    spin_lock_irqsave(&balongs->lock, flags);

    if (balongs->run == QUEUE_RUNNING || balongs->busy) {
        spin_unlock_irqrestore(&balongs->lock, flags);
        return -EBUSY;
    }

    balongs->run = QUEUE_RUNNING;
    balongs->cur_msg = NULL;
    balongs->cur_transfer = NULL;
    balongs->cur_chip = NULL;
    balongs->prev_chip = NULL;
    spin_unlock_irqrestore(&balongs->lock, flags);

    queue_work(balongs->workqueue, &balongs->pump_messages);

    return 0;
}

static int stop_queue(struct balong_spi *balongs)
{
    unsigned long flags;
    unsigned limit = 50;
    int status = 0;

    spin_lock_irqsave(&balongs->lock, flags);
    balongs->run = QUEUE_STOPPED;
    while ((!list_empty(&balongs->queue) || balongs->busy) && limit--) {
        spin_unlock_irqrestore(&balongs->lock, flags);
        msleep(10);
        spin_lock_irqsave(&balongs->lock, flags);
    }

    if (!list_empty(&balongs->queue) || balongs->busy)
        status = -EBUSY;
    spin_unlock_irqrestore(&balongs->lock, flags);

    return status;
}

static int destroy_queue(struct balong_spi *balongs)
{
    int status;

    status = stop_queue(balongs);
    if (status != 0)
        return status;
    destroy_workqueue(balongs->workqueue);
    return 0;
}

static void flush(struct dw_spi *dws)
{
    while (balong_readw(dws, HI_SPI_SR_OFFSET) & SR_RF_NOT_EMPT)
        balong_readw(dws, HI_SPI_DR_OFFSET);

    wait_till_not_busy(dws);
}

/* Restart the controller, disable all interrupts, clean rx fifo */
static void spi_hw_init(struct balong_spi *balongs)
{
    spi_enable_chip(balongs, 0);
    spi_mask_intr(balongs, 0xff);
    spi_enable_chip(balongs, 1);
    flush(balongs);
    /*
     * Try to detect the FIFO depth if not set by interface driver,
     * the depth could be from 2 to 256 from HW spec
     */
    if (!balongs->fifo_len) {
        u32 fifo;
        for (fifo = 2; fifo <= 257; fifo++) {
            balong_writew(balongs, HI_SPI_TXFTLR_OFFSET, fifo);
            if (fifo != balong_readw(balongs, HI_SPI_TXFTLR_OFFSET))
                break;
        }

        balongs->fifo_len = (fifo == 257) ? 0 : fifo;
        balong_writew(balongs, HI_SPI_TXFTLR_OFFSET, 0);
    }
}

int __devinit balong_spi_add_host(struct balong_spi *balongs)
{
    struct spi_master *master;
    static struct balong_spi_dma_ops dma_ops;
    int ret;

    if(NULL == balongs)
    {
        ret = -1;
        printk(KERN_ERR "balong_spi_add_host:balongs is NULL! \n");
        goto exit;
    }

    master = spi_alloc_master(balongs->parent_dev, 0);
    if (!master) {
        ret = -ENOMEM;
        goto exit;
    }

    balongs->master = master;
    balongs->type = SSI_MOTO_SPI;
    balongs->prev_chip = NULL;
    balongs->dma_inited = 0;
    balongs->dma_addr = (dma_addr_t)(balongs->paddr + 0x60);
    snprintf(balongs->name, sizeof(balongs->name), "balong_spi%d",
            balongs->bus_num);


    master->mode_bits = SPI_CPOL | SPI_CPHA;
    master->bus_num = balongs->bus_num;
    master->num_chipselect = balongs->num_cs;
    master->cleanup = balong_spi_cleanup;
    master->setup = balong_spi_setup;
    master->transfer = balong_spi_transfer;
    dma_ops.dma_init = balong_spi_edma_init;
    dma_ops.dma_transfer = balong_spi_edma_transfer;
    dma_ops.dma_exit = balong_spi_edma_exit;
    balongs->dma_ops = &dma_ops;

    /* Basic HW init */
    spi_hw_init(balongs);
    ret = request_irq(balongs->irq, balong_spi_irq, IRQF_SHARED,
            balongs->name, balongs);
    if (ret < 0) {
        dev_err(&master->dev, "can not get IRQ\n");
        goto err_free_master;
    }


    if (balongs->dma_ops && balongs->dma_ops->dma_init) {
        ret = balongs->dma_ops->dma_init(balongs);
        if (ret) {
            dev_warn(&master->dev, "DMA init failed\n");
            balongs->dma_inited = 0;
        }
    }

    /* Initial and start queue */
    ret = init_queue(balongs);
    if (ret) {
        dev_err(&master->dev, "problem initializing queue\n");
        goto err_diable_hw;
    }
    ret = start_queue(balongs);
    if (ret) {
        dev_err(&master->dev, "problem starting queue\n");
        goto err_diable_hw;
    }

    spi_master_set_devdata(master, balongs);
    ret = spi_register_master(master);
    if (ret) {
        dev_err(&master->dev, "problem registering spi master\n");
        goto err_queue_alloc;
    }

    mrst_spi_debugfs_init(balongs);

    return 0;

err_queue_alloc:
    destroy_queue(balongs);
    if (balongs->dma_ops && balongs->dma_ops->dma_exit)
        balongs->dma_ops->dma_exit(balongs);
err_diable_hw:
    spi_enable_chip(balongs, 0);
    free_irq(balongs->irq, balongs);
err_free_master:
    spi_master_put(master);
exit:
    return ret;
}
EXPORT_SYMBOL_GPL(balong_spi_add_host);

void __devexit balong_spi_remove_host(struct balong_spi *balongs)
{
    int status = 0;

    if (!balongs)
        return;
    mrst_spi_debugfs_remove(balongs);

    /* Remove the queue */
    status = destroy_queue(balongs);
    if (status != 0)
        dev_err(&balongs->master->dev, "balong_spi_remove: workqueue will not "
            "complete, message memory not freed\n");

    if (balongs->dma_ops && balongs->dma_ops->dma_exit)
        balongs->dma_ops->dma_exit(balongs);
    spi_enable_chip(balongs, 0);
    /* Disable clk */
    spi_set_clk(balongs, 0);
    free_irq(balongs->irq, balongs);

    /* Disconnect from the SPI framework */
    spi_unregister_master(balongs->master);
}
EXPORT_SYMBOL_GPL(balong_spi_remove_host);

int balong_spi_suspend_host(struct balong_spi *balongs)
{
    int ret = 0;

    ret = stop_queue(balongs);
    if (ret)
        return ret;
    spi_enable_chip(balongs, 0);
    spi_set_clk(balongs, 0);
    return ret;
}
EXPORT_SYMBOL_GPL(balong_spi_suspend_host);

int balong_spi_resume_host(struct balong_spi *balongs)
{
    int ret;

    spi_hw_init(balongs);
    ret = start_queue(balongs);
    if (ret)
        dev_err(&balongs->master->dev, "fail to start queue (%d)\n", ret);
    return ret;
}
EXPORT_SYMBOL_GPL(balong_spi_resume_host);

static int __devinit balong_spi_v7r2_probe(struct platform_device *pdev)
{
    struct balong_spi_v7r2 *balongsv7r2;
    struct balong_spi *balongs;
    struct resource *mem, *ioarea;
    int ret;
    u32 ioshare;

    balongsv7r2 = kzalloc(sizeof(struct balong_spi_v7r2), GFP_KERNEL);
    if (!balongsv7r2) {
        dev_err(&pdev->dev, "spi kzalloc failed\n");
        ret = -ENOMEM;
        goto err_end;
    }

    balongs = &balongsv7r2->balongs;

    /* Get basic io resource and map it */
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!mem) {
        dev_err(&pdev->dev, "no mem resource?\n");
        ret = -EINVAL;
        goto err_kfree;
    }

    ioarea = request_mem_region(mem->start, resource_size(mem),
            pdev->name);
    if (!ioarea) {
        dev_err(&pdev->dev, "SPI region already claimed\n");
        ret = -EBUSY;
        goto err_kfree;
    }

    balongs->irq = platform_get_irq(pdev, 0);
    if (balongs->irq < 0) {
        dev_err(&pdev->dev, "no irq resource?\n");
        ret = balongs->irq; /* -ENXIO */
        goto err_release_reg;
    }

    /*balongsv7r2->clk = clk_get(&pdev->dev, NULL);*/
    if(pdev->id == 0)
    {
        balongs->regs = HI_SPI_MST0_REGBASE_ADDR_VIRT;
        balongsv7r2->clk = clk_get(NULL, "dw_ssi0_clk");
    }
    else
    {
        balongs->regs = HI_SPI_MST1_REGBASE_ADDR_VIRT;
        balongsv7r2->clk = clk_get(NULL, "dw_ssi1_clk");
        balongs->paddr = HI_SPI_MST1_REGBASE_ADDR;
    }
    if (IS_ERR(balongsv7r2->clk)) {
        ret = PTR_ERR(balongsv7r2->clk);
        goto err_release_reg;
    }
    clk_enable(balongsv7r2->clk);

    balongs->parent_dev = &pdev->dev;
    balongs->bus_num = pdev->id;
    balongs->num_cs = 2;
    balongs->max_freq = clk_get_rate(balongsv7r2->clk);

    ret = balong_spi_add_host(balongs);
    if (ret)
        goto err_clk;

    platform_set_drvdata(pdev, balongsv7r2);
    printk(KERN_ERR "balong_spi_v7r2_probe ok! \n");
    return 0;

err_clk:
    clk_disable(balongsv7r2->clk);
    clk_put(balongsv7r2->clk);
    balongsv7r2->clk = NULL;
err_release_reg:
    release_mem_region(mem->start, resource_size(mem));
err_kfree:
    kfree(balongsv7r2);
err_end:
    return ret;
}

static int __devexit balong_spi_v7r2_remove(struct platform_device *pdev)
{
    struct balong_spi_v7r2 *balongsv7r2 = platform_get_drvdata(pdev);
    struct resource *mem;

    if (NULL == balongsv7r2) {
        pr_err("%s %d platform_get_drvdata NULL\n", __func__, __LINE__);
        return -1;
    }

    platform_set_drvdata(pdev, NULL);

    clk_disable(balongsv7r2->clk);
    clk_put(balongsv7r2->clk);
    balongsv7r2->clk = NULL;

    free_irq(balongsv7r2->balongs.irq, &balongsv7r2->balongs);
    balong_spi_remove_host(&balongsv7r2->balongs);
    iounmap(balongsv7r2->balongs.regs);
    kfree(balongsv7r2);

    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (NULL == mem) {
        pr_err("%s %d platform_get_resource NULL\n", __func__, __LINE__);
        return -1;
    }
    release_mem_region(mem->start, resource_size(mem));
    return 0;
}


#ifdef CONFIG_PM
static int __devexit balong_spi_v7r2_suspend(struct device *dev)
{
/*store reg data*/
    struct platform_device *pdev=to_platform_device(dev);
    struct balong_spi_v7r2 *spi=platform_get_drvdata(pdev);

    if(NULL == spi)
    {
        printk(KERN_ERR "platform_get_drvdata ERR,spi is NULL! \n");
        return -1;
    }

    spi->suspended =1;
    balong_spi_suspend_host(&spi->balongs);
    clk_disable(spi->clk);

    printk(KERN_INFO "balong_spi_v7r2_suspend ok! \n");

    return 0;
}
static int __devexit balong_spi_v7r2_resume(struct device *dev)
{
    struct platform_device *pdev=to_platform_device(dev);
    struct balong_spi_v7r2 *spi=platform_get_drvdata(pdev);

    if(NULL == spi)
    {
        printk(KERN_ERR "platform_get_drvdata ERR,spi is NULL! \n");
        return -1;
    }

    spi->suspended =0;
    clk_enable(spi->clk);
    balong_spi_resume_host(&spi->balongs);

    printk(KERN_INFO "balong_spi_v7r2_resume ok! \n");

    return 0;
}
int spi_suspend_and_resume_test()
{
    dpm_suspend_start(PMSG_SUSPEND);
    dpm_suspend_end(PMSG_SUSPEND);
    dpm_resume_start(PMSG_RESUME);
    dpm_resume_end(PMSG_RESUME);

    return 0;
}
static const struct dev_pm_ops balong_spi_dev_pm_ops ={
    .suspend = balong_spi_v7r2_suspend,
    .resume = balong_spi_v7r2_resume,
};

#define BALONG_DEV_PM_OPS (&balong_spi_dev_pm_ops)
#else
#define BALONG_DEV_PM_OPS NULL
#endif
/*SPI驱动注册*/
static struct platform_driver balong_driver_spi0 = {
    .probe      = balong_spi_v7r2_probe,
    .remove     = __devexit_p(balong_spi_v7r2_remove),
    .driver     = {
        .name   = "balong_spi_v7r2_0",
        .owner  = THIS_MODULE,
#if 0
        .pm        =BALONG_DEV_PM_OPS,
#endif
    },
};

static struct platform_driver balong_driver_spi1 = {
    .probe      = balong_spi_v7r2_probe,
    .remove     = __devexit_p(balong_spi_v7r2_remove),
    .driver     = {
        .name   = DRIVER_NAME"_1",
        .owner  = THIS_MODULE,
        .pm        =BALONG_DEV_PM_OPS,
    },
};

static struct platform_driver *balong_spi_drivers[] __initdata = {
    &balong_driver_spi0,
    &balong_driver_spi1,
};

/*SPI设备注册*/
static struct resource balong_spi0_resource[] = {
    [0] = {
        .start = HI_SPI_MST0_REGBASE_ADDR,
        .end   = HI_SPI_MST0_REGBASE_ADDR + HI_SPI_MST0_REG_SIZE,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = INT_LVL_SPI0,
        .end   = INT_LVL_SPI0,
        .flags = IORESOURCE_IRQ,
    }

};
static struct resource balong_spi1_resource[] = {
    [0] = {
        .start = HI_SPI_MST1_REGBASE_ADDR,
        .end   = HI_SPI_MST1_REGBASE_ADDR + HI_SPI_MST1_REG_SIZE,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = INT_LVL_SPI1,
        .end   = INT_LVL_SPI1,
        .flags = IORESOURCE_IRQ,
    }

};
static u64 balong_device_spi0_dmamask = 0xffffffffUL;

struct platform_device balong_device_spi0 = {
    .name         = "balong_spi_v7r2_0",
    .id       = 0,
    .num_resources    = ARRAY_SIZE(balong_spi0_resource),
    .resource     = balong_spi0_resource,
       .dev              = {
                .dma_mask = &balong_device_spi0_dmamask,
                .coherent_dma_mask = 0xffffffffUL,
        }
};

static u64 balong_device_spi1_dmamask = 0xffffffffUL;

struct platform_device balong_device_spi1 = {
    .name         = "balong_spi_v7r2_1",
    .id       = 1,
    .num_resources    = ARRAY_SIZE(balong_spi1_resource),
    .resource     = balong_spi1_resource,
       .dev              = {
                .dma_mask = &balong_device_spi1_dmamask,
                .coherent_dma_mask = 0xffffffffUL,
        }
};
/*spi控制器私有数据*/
#ifdef CONFIG_PMIC_HI6451
 struct balong_spi_chip hi6451_chip_info = {
    .poll_mode = 1, /* 1 for contoller polling mode */
    .type = SSI_MOTO_SPI,   /* SPI/SSP/Micrwire */
    .enable_dma = 0 ,
    /*.cs_control = null_cs_control,*/
};
#endif
#ifdef CONFIG_SFLASH
 struct balong_spi_chip sflash_chip_info = {
    .poll_mode = 1, /* 1 for contoller polling mode */
    .type = SSI_MOTO_SPI,   /* SPI/SSP/Micrwire */
    .enable_dma = 0 ,
    /*.cs_control = null_cs_control,*/
};
#endif
 struct balong_spi_chip lcd_chip_info = {
    .poll_mode = 1,
    .type = SSI_MOTO_SPI,
    .enable_dma = 1,
 };
/*spi从设备私有数据*/
 struct spi_board_info spidev_board_info[] __initdata = {
#ifdef CONFIG_PMIC_HI6451
    {
     .modalias = "pmic_hi6451_spi",
     .max_speed_hz = 2000000,
     .bus_num = 0,
     .chip_select = 0,
     .mode = SPI_MODE_3,
     .controller_data = &hi6451_chip_info,
    },
#endif
    {
        .modalias = "balong_lcd_spi",
        .max_speed_hz = 8000000,
        .bus_num = 1,
        .chip_select = 0,
        .mode = SPI_MODE_0,
        .controller_data = &lcd_chip_info,
    },
#ifdef CONFIG_SFLASH
    {
     .modalias = "balong-sflash-spi",
     .max_speed_hz = 2000000,
     .bus_num = 0,
     .chip_select = 1,
     .mode = SPI_MODE_3,
     .controller_data = &sflash_chip_info,
    },
#endif
};

static int __init balong_spi_v7r2_init(void)
{
    int ret = 0;

    ret = platform_device_register(&balong_device_spi0);
    if(ret)
        printk(KERN_ERR "spi0:platform_device_register err! \n");
    ret = platform_device_register(&balong_device_spi1);
    if(ret)
        printk(KERN_ERR "spi1:platform_device_register err! \n");
    ret = platform_driver_register(&balong_driver_spi0);
    if(ret)
        printk(KERN_ERR "spi0:platform_driver_register err! \n");
    ret = platform_driver_register(&balong_driver_spi1);
    if(ret)
        printk(KERN_ERR "spi1:platform_driver_register err! \n");

    spi_register_board_info(&spidev_board_info, ARRAY_SIZE(spidev_board_info));

    return ret;
}
arch_initcall(balong_spi_v7r2_init);

static void __exit balong_spi_v7r2_exit(void)
{
    platform_driver_unregister(&balong_driver_spi0);
    platform_driver_unregister(&balong_driver_spi1);
}
module_exit(balong_spi_v7r2_exit);

MODULE_AUTHOR("Feng Tang <feng.tang@intel.com>");
MODULE_DESCRIPTION("Driver for DesignWare SPI controller core");
MODULE_LICENSE("GPL v2");

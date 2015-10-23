#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <mach/irqs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>
#include <linux/poll.h>
#include <asm/irq.h>
#include "asm/delay.h"
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/atomic.h>
#include <linux/kthread.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <mach/pcie.h>
#include "bsp_memmap.h"
#include "pcie_test.h"
#include "pcie_dma_drv.h"

/*pcie application interface*/
static unsigned long pctrl_addr = (unsigned long)HI_IO_ADDRESS(REG_BASE_PCTRL); 
static unsigned long pcie_cfg_addr = 0;

static unsigned long ep_tx_virt_buf = 0;
static u8 *ep_rx_virt_buf = NULL;
static dma_addr_t ep_rx_phys_buf = 0;
static unsigned int *sync_virt_buf = NULL;
static dma_addr_t sync_phys_buf = 0;
static u32 test_buf_size = SZ_2M;
static u32 sync_buf_size = SZ_2M;
static tx_rx_sync* rc_rx_ep_tx_sync;
static tx_rx_sync* ep_task_start_sync;
static tx_rx_sync* rc_tx_ep_rx_sync;
static tx_rx_sync* ep_read_rc_sync;
static dma_buf_sync* ep_dma_buf_sync;
static u32* intx_event_sync;
static u8 *dma_virt_buf = NULL;
static dma_addr_t dma_phys_buf = 0;
static u32 dma_buf_size = SZ_4M;
struct task_struct * tx_to_rc_by_cpu_task_id;
struct task_struct * rx_from_rc_by_cpu_task_id;
struct task_struct * ep_read_rc_by_cpu_task_id;
static int ep_task_run = 1;

static __inline__ void set_iatu(u32 pcie_cfg_base,
                       u32 viewport, 
                       u32 region_ctrl_1, 
                       u32 region_ctrl_2,
                       u32 lbar,
                       u32 ubar,
                       u32 lar,
                       u32 ltar,
                       u32 utar)
{
	    writel(viewport, pcie_cfg_base + IATU_IDX);
	    writel(region_ctrl_1, pcie_cfg_base + IATU_CTRL1);
	    writel(lbar, pcie_cfg_base + IATU_LOWER_ADDR);
	    writel(ubar, pcie_cfg_base + IATU_UPPER_ADDR);
	    writel(lar, pcie_cfg_base + IATU_LIMIT_ADDR);
	    writel(ltar, pcie_cfg_base + IATU_LTAR_ADDR);
	    writel(utar, pcie_cfg_base + IATU_UTAR_ADDR);    
	    writel(region_ctrl_2, pcie_cfg_base + IATU_CTRL2);
}

s32 tx_to_rc_by_cpu_task(void* data)
{
    int i = 0;
    u32 offset = 0;

    printk(KERN_ERR "PCIE EP:%s enter.",__func__);
    
    while(ep_task_run)
    {
        if(rc_rx_ep_tx_sync->work_start != 1)
        {
            msleep(50);
            continue;
        }

        printk(KERN_ERR "PCIE EP:%s work.",__func__);
        offset = rc_rx_ep_tx_sync->addr_offset;

        if(rc_rx_ep_tx_sync->trans_times == 1)
        {
            memset((void*)(ep_tx_virt_buf + offset),rc_rx_ep_tx_sync->trans_pattern,rc_rx_ep_tx_sync->trans_len);
        }
        else
        {
            for(i = 0; i < rc_rx_ep_tx_sync->trans_times; i++)
            {
                memset((void*)ep_tx_virt_buf,i,rc_rx_ep_tx_sync->trans_len);
            }
        }    
                
        if((rc_rx_ep_tx_sync->readback_check)&&(rc_rx_ep_tx_sync->trans_times == 1))
        {
            u8* expect_virt_buf = NULL;
            expect_virt_buf = (u8*)kmalloc(rc_rx_ep_tx_sync->trans_len,GFP_KERNEL);
            if(!expect_virt_buf)
            {
                printk(KERN_ERR "PCIE EP:kmalloc expect_virt_buf fail.");
                rc_rx_ep_tx_sync->readback_check_pass = 2;
            }
            memcpy((void*)expect_virt_buf,(void*)(ep_tx_virt_buf + offset),rc_rx_ep_tx_sync->trans_len);
            for(i = 0; i < rc_rx_ep_tx_sync->trans_len; i++)
            {
                if(expect_virt_buf[i] != (u8)(rc_rx_ep_tx_sync->trans_pattern))
                {
                    rc_rx_ep_tx_sync->readback_check_pass = 0;
                    break;
                }
            }
            if(i == rc_rx_ep_tx_sync->trans_len)
            {
                rc_rx_ep_tx_sync->readback_check_pass = 1;
            }
            kfree((void*)expect_virt_buf);
            expect_virt_buf = NULL;
        }
        
        rc_rx_ep_tx_sync->work_done = 1;
        rc_rx_ep_tx_sync->work_start = 0;
    }
    return 0;
}

s32 rx_from_rc_by_cpu_task(void* data)
{
    u8* p_last_byte = (u8*)((u32)ep_rx_virt_buf + test_buf_size - 1);

    printk(KERN_ERR "PCIE EP:%s enter.",__func__);
    while(ep_task_run)
    {
        if(rc_tx_ep_rx_sync->work_start != 1)
        {
           msleep(5);
           continue;
        }

        printk(KERN_ERR "PCIE EP:%s work.",__func__);
        
        rc_tx_ep_rx_sync->start_time = jiffies;
        
        *p_last_byte = 0;
        
        while(!rc_tx_ep_rx_sync->work_done)
        {
            msleep(5);
        }

        rc_tx_ep_rx_sync->end_time = jiffies;

        if(*p_last_byte != 0xff)
        {
            printk(KERN_ERR "PCIE EP:oh,bad, p_last_byte is not 0xff");
        }
        
        rc_tx_ep_rx_sync->work_start = 0;
    }
    return 0;
}

s32 ep_read_rc_by_cpu_task(void* data)
{
    u32 i = 0;
    u8* readback_virt_buf = NULL;

    printk(KERN_ERR "PCIE EP:%s enter.",__func__);
    while(ep_task_run)
    {
        if(ep_read_rc_sync->work_start != 1)
        {
            msleep(50);
            continue;
        }

        printk(KERN_ERR "PCIE EP:%s work.",__func__);
        
        readback_virt_buf = (u8*)kmalloc(ep_read_rc_sync->trans_len,GFP_KERNEL);
        if(!readback_virt_buf)
        {
            printk(KERN_ERR "PCIE EP:oh,bad ,kmalloc readback_buf fail.");
            ep_read_rc_sync->work_done = 1;
            ep_read_rc_sync->work_start = 0;
            ep_read_rc_sync->readback_check_pass = 2;
            continue;
        }
        
        ep_read_rc_sync->start_time = jiffies;
        for(i = 0; i < ep_read_rc_sync->trans_times; i++)
        {
            memcpy((void*)readback_virt_buf,(void*)ep_tx_virt_buf,ep_read_rc_sync->trans_len);
        }
        ep_read_rc_sync->end_time = jiffies;
        kfree((void*)readback_virt_buf);
        readback_virt_buf = NULL;
        ep_read_rc_sync->work_done = 1;
        while(ep_read_rc_sync->work_start)
        {
            msleep(10);
        }
    }

    return 0;
}
s32 intx_deamon_task(void* data)
{
    u32 val = 0;
    while(ep_task_run)
    {
        msleep(1);
        dbi_enable(pctrl_addr);
        val = readl(pctrl_addr + PCIE0_CTRL12);
        if(*intx_event_sync)
        {
            if(val == 0)
            {
                writel(0x1,pctrl_addr + PCIE0_CTRL12);
#if defined(BSP_CONFIG_V7R2_SFT)
                __delay(2000);
#endif 
            }
                
        }
        else
        {
            if(val == 1)
            {
                writel(0x0,pctrl_addr + PCIE0_CTRL12);
#if defined(BSP_CONFIG_V7R2_SFT)
                __delay(2000);
#endif                 
            }            
        }       
        dbi_disable(pctrl_addr);            
    }
    return 0;
}

int pcie_ep_send_intx(u32 data)
{
    *intx_event_sync = data;
    return 0;
}

u32 ep_send_msi_count = 0;
u32 ep_send_msi_jiffis[101] = {0};
int pcie_ep_send_msi(u32 msi_data)
{
    int val = 0, offset = 0;;

    ep_send_msi_jiffis[ep_send_msi_count%100] = jiffies;
    ep_send_msi_count++;
    dbi_enable(pctrl_addr);
    val = readl(pcie_cfg_addr + 0x50);
    if(0x5 == (val&0xff))
    {   
        writel(0x0,pctrl_addr + PCIE0_CTRL12);
#if defined(BSP_CONFIG_V7R2_SFT)        
        __delay(2000);
#endif
        offset = ((val>>23)&0x1)? PCI_MSI_DATA_64 : PCI_MSI_DATA_32;
        val = readl(pcie_cfg_addr + 0x50 + offset);
        val &= (~0x1f);
        val |= (msi_data&0x1f);
        writel(val,pcie_cfg_addr + 0x50 + offset);
        writel(0x2,pctrl_addr + PCIE0_CTRL12);
#if defined(BSP_CONFIG_V7R2_SFT)        
        __delay(2000);
#endif        
    }
    dbi_disable(pctrl_addr); 

    return 0;
}
int send_msi_test(u32 t,u32 msi_data)
{
   int i = 0;
   for(i = 0; i < t; i++)
   {
       pcie_ep_send_msi(msi_data);
   }

   return 0;
}
int show_msi_send_count(u32 t)
{
    int i = 0;

    printk(KERN_ERR "ep_send_msi_count:%d.\n",ep_send_msi_count);
    for(i = 0; i < t; i++)
    {
        printk(KERN_ERR "send jiffies:0x%x.\n",ep_send_msi_jiffis[i%100]);
    }
    return 0;
}
#if defined(EP_DMA_RISE_LOCAL_INT)
static u32 wr_done_int = 0;
static u32 wr_error_int = 0;
static u32 rd_done_int = 0;
static u32 rd_error_int = 0;
static __inline__ void pcie_dma_common_isr(u32 dma_wr_int_status,
                                    u32 dma_wr_err_status,
                                    u32 dma_rd_int_status,
                                    u32 dma_rd_err_lstatus,
                                    u32 dma_rd_err_hstatus)
{
    u32 done_int = 0;
    u32 abort_int = 0;
    s32 i = 0;
    /*handle int*/
    /*handle wr done int*/
    if(dma_wr_int_status)
    {
        done_int = (dma_wr_int_status>>done_int_pos)&done_int_mask;
        if(done_int)
        {
            for(i = 0; i < MAX_CHN_NUM; i++)
            {
                if((done_int>>i)&0x1)
                {
                    pcie_ep_send_msi(0);
                    wr_done_int++;
                }
            }
        }

        abort_int = (dma_wr_int_status>>abort_int_pos)&abort_int_mask;
        if(abort_int)
        {
            for(i = 0; i < MAX_CHN_NUM; i++)
            {
                if((abort_int>>i)&0x1)
                {
                    pcie_ep_send_msi(1);
                    wr_error_int++;
                }
            }            
        }
    }

    if(dma_rd_int_status)
    {
        done_int = (dma_rd_int_status>>done_int_pos)&done_int_mask;
        if(done_int)
        {
            for(i = 0; i < MAX_CHN_NUM; i++)
            {
                if((done_int>>i)&0x1)
                {
                    pcie_ep_send_msi(2);
                    rd_done_int++;
                }
            }            
        }

        abort_int = (dma_rd_int_status>>abort_int_pos)&abort_int_mask;
        if(abort_int)
        {
            for(i = 0; i < MAX_CHN_NUM; i++)
            {
                if((abort_int>>i)&0x1)
                {
                    pcie_ep_send_msi(3);
                    rd_error_int++;
                }
            }            
        }        
    }    
}

void set_int_count(void)
{
    wr_done_int = 0;
    wr_error_int = 0;
    rd_done_int = 0;
    rd_error_int = 0;
}

void show_int_count(void)
{
    printk(KERN_ERR "wr_done_int:%d.\nwr_error_int:%d.\nrd_done_int:%d.\nrd_error_int:%d.\n",
        wr_done_int,wr_error_int,rd_done_int,rd_error_int);
}
static irqreturn_t pcie_local_dma_isr(s32 irq, void* private)
{
    u32 dma_wr_int_status = 0;
    u32 dma_wr_err_status = 0;
    u32 dma_rd_int_status = 0;
    u32 dma_rd_err_lstatus = 0;
    u32 dma_rd_err_hstatus = 0;

    dbi_enable(pctrl_addr);
    /*read int status*/
    dma_wr_int_status = readl(pcie_cfg_addr + DMA_WR_INT_STATUS);
    dma_wr_err_status = readl(pcie_cfg_addr + DMA_WR_ERR_STATUS);
    dma_rd_int_status = readl(pcie_cfg_addr + DMA_RD_INT_STATUS);
    dma_rd_err_lstatus = readl(pcie_cfg_addr + DMA_RD_ERR_STATUS_LOW);
    dma_rd_err_hstatus = readl(pcie_cfg_addr + DMA_RD_ERR_STATUS_HIGH);  
    /*clear int*/
    writel(dma_wr_int_status,pcie_cfg_addr + DMA_WR_INT_CLEAR);
    writel(dma_rd_int_status,pcie_cfg_addr + DMA_RD_INT_CLEAR);    
    dbi_disable(pctrl_addr);
        
    pcie_dma_common_isr(dma_wr_int_status,dma_wr_err_status,
                         dma_rd_int_status,dma_rd_err_lstatus,
                         dma_rd_err_hstatus);

    return IRQ_HANDLED;
}
#endif

int pcie_ep_test_init(void)
{
    u32 val = 0;
    
    pcie_cfg_addr = (unsigned long)ioremap(REG_BASE_PCIE_CFG,REG_BASE_PCIE_CFG_SIZE);
	if(!pcie_cfg_addr)
	{
		printk(KERN_ERR "PCIE EP:Cannot map physical base for pcie");
		return -EIO;
	}

    ep_rx_virt_buf = (u8*)dma_alloc_coherent(NULL,(test_buf_size + sync_buf_size),&ep_rx_phys_buf,GFP_DMA|__GFP_WAIT);
    if(NULL == ep_rx_virt_buf)
    {
		printk(KERN_ERR "PCIE EP:dma_alloc_coherent ep_rx_virt_buf failed.");
		return -EIO;        
    }
    printk(KERN_ERR "PCIE EP:ep_rx_virt_buf:0x%x,ep_rx_phys_buf:0x%x.",(u32)ep_rx_virt_buf,ep_rx_phys_buf);

    memset((void*)ep_rx_virt_buf,0,(test_buf_size + sync_buf_size));
    
    dbi_enable(pctrl_addr);
    /*wait bar0 is configured by rc*/
    while(!(readl(pcie_cfg_addr + PCI_BASE_ADDRESS_0)&0xfffffff0))
    {
        udelay(1000);
    }
    val = readl(pcie_cfg_addr + PCI_BASE_ADDRESS_0)&0xfffffff0;
    printk(KERN_ERR "PCIE EP:rc config ep bar0 addr:0x%x.\n",val);
    
    set_iatu(pcie_cfg_addr,
           0x80000000,  			/*bit31:1 inbound , bit[3:0]:0 region index 0*/
           0x00000000,   		    /*region type, 4:config, 2:IO, 0:MEM*/ 
           0x80000000,              /*bit31:1 enable, bit30:1 bar match, bit10..8: 0 bar0*/
           val,
           0,
           val + (test_buf_size + sync_buf_size - 1),
           (u32)ep_rx_phys_buf,
           0);
    dbi_disable(pctrl_addr);
    sync_virt_buf = (u32*)((u32)ep_rx_virt_buf + test_buf_size);
    sync_phys_buf = ep_rx_phys_buf + test_buf_size;
    printk(KERN_ERR "PCIE EP:sync_virt_buf:0x%x,sync_phys_buf:0x%x.",(u32)sync_virt_buf,sync_phys_buf);

    rc_tx_ep_rx_sync = (tx_rx_sync*)sync_virt_buf;
    ep_task_start_sync = (tx_rx_sync*)((u32)rc_tx_ep_rx_sync + sizeof(tx_rx_sync));
    ep_dma_buf_sync = (dma_buf_sync*)((u32)ep_task_start_sync + sizeof(tx_rx_sync));
    intx_event_sync = (u32*)((u32)ep_dma_buf_sync + sizeof(dma_buf_sync));
    
    printk(KERN_ERR "PCIE EP:rc_tx_ep_rx_sync:0x%x.",(u32)rc_tx_ep_rx_sync);
    printk(KERN_ERR "PCIE EP:ep_task_start_sync:0x%x.",(u32)ep_task_start_sync);
    printk(KERN_ERR "PCIE EP:ep_dma_buf_sync:0x%x.",(u32)ep_dma_buf_sync);
        
    dma_virt_buf = (u8*)dma_alloc_coherent(NULL,dma_buf_size,&dma_phys_buf,GFP_DMA|__GFP_WAIT);
    if(NULL == dma_virt_buf)
    {
		printk(KERN_ERR "PCIE EP:dma_alloc_coherent dma_virt_buf failed.");
        dma_free_coherent(NULL,(test_buf_size + sync_buf_size),(void*)ep_rx_virt_buf,ep_rx_phys_buf);   
		return -EIO;        
    } 

    /*set inbound window for dma operate*/
    dbi_enable(pctrl_addr);
    val = readl(pcie_cfg_addr + PCI_BASE_ADDRESS_2)&0xfffffff0;
    printk(KERN_ERR "PCIE EP:rc config ep bar2 addr:0x%x.\n",val);    
    set_iatu(pcie_cfg_addr,
           0x80000001,  			/*bit31:1 inbound , bit[3:0]:0 region index 1*/
           0x00000000,   		    /*region type, 4:config, 2:IO, 0:MEM*/ 
           0x80000000,              /*bit31:1 enable, bit30:1 bar match, bit10..8: 0 bar0*/
           val,
           0,
           val + (dma_buf_size - 1),
           (u32)dma_phys_buf,
           0);    
    dbi_disable(pctrl_addr);
    
    printk(KERN_ERR "PCIE EP:dma_virt_buf:0x%x,dma_phys_buf:0x%x.",(u32)dma_virt_buf,dma_phys_buf);
    ep_dma_buf_sync->local_low = (u32)dma_phys_buf;
    ep_dma_buf_sync->local_high = 0;   
    ep_dma_buf_sync->remote_low = (u32)REG_BASE_PCIE_DATA;
    ep_dma_buf_sync->remote_high = 0;    
    ep_dma_buf_sync->size       = dma_buf_size;

    printk(KERN_ERR "PCIE EP:wait rc enable bus master bit");
    dbi_enable(pctrl_addr);
    for(;;)
    {
        val = readl(pcie_cfg_addr + PCI_COMMAND);
        if(val&PCI_COMMAND_MASTER)
        {
            break;
        }
        mdelay(100);
        
    }
    dbi_disable(pctrl_addr);
    printk(KERN_ERR "PCIE EP:bus master bit is enabled");

    ep_tx_virt_buf = (u32)ioremap((u32)REG_BASE_PCIE_DATA,(u32)(test_buf_size + sync_buf_size));
    if(!ep_tx_virt_buf)
    {
        printk(KERN_ERR "PCIE EP:ioremap REG_BASE_PCIE_DATA fail.");
        dma_free_coherent(NULL,(test_buf_size + sync_buf_size),(void*)ep_rx_virt_buf,ep_rx_phys_buf);
        dma_free_coherent(NULL,dma_buf_size,(void*)dma_virt_buf,dma_phys_buf);
        return -EIO;        
    }
    dbi_enable(pctrl_addr);
    set_iatu(pcie_cfg_addr,
           0x2,                 /*outbound region index 2*/
           0x00000000, 
           0x80000000,
           REG_BASE_PCIE_DATA,
           0x0,
           (REG_BASE_PCIE_DATA + test_buf_size + sync_buf_size - 1),
           0x80000000,
           0);    
    dbi_disable(pctrl_addr); 

    rc_rx_ep_tx_sync = (tx_rx_sync*)((u32)ep_tx_virt_buf + test_buf_size);
    ep_read_rc_sync = (tx_rx_sync*)((u32)rc_rx_ep_tx_sync + sizeof(tx_rx_sync));
    printk(KERN_ERR "PCIE EP:rc_rx_ep_tx_sync:0x%x.",(u32)rc_rx_ep_tx_sync);
    printk(KERN_ERR "PCIE EP:ep_read_rc_sync:0x%x.",(u32)ep_read_rc_sync);
    
    printk(KERN_ERR "PCIE EP:wait rc init ok...");
    while(!ep_task_start_sync->work_start)
    {
        mdelay(100);
    }
    printk(KERN_ERR "PCIE EP:rc init ok.\n");

    ep_task_run = 1;
    tx_to_rc_by_cpu_task_id = kthread_run(tx_to_rc_by_cpu_task,0,"tx_to_rc_by_cpu_task");
    mdelay(10);
    rx_from_rc_by_cpu_task_id = kthread_run(rx_from_rc_by_cpu_task,0,"rx_from_rc_by_cpu_task");
    mdelay(10);
    ep_read_rc_by_cpu_task_id = kthread_run(ep_read_rc_by_cpu_task,0,"ep_read_rc_by_cpu_task");
    mdelay(10);
    kthread_run(intx_deamon_task,0,"intx_deamon_task");
    mdelay(10);    

#if defined(EP_DMA_RISE_LOCAL_INT)
    if(request_irq(INT_LVL_PCIE0_EDMA, pcie_local_dma_isr, IRQF_DISABLED, "pcie local dma int", NULL))
    {
        printk(KERN_ERR "pcie edma irq request failed!\n");
        dma_free_coherent(NULL,(test_buf_size + sync_buf_size),(void*)ep_rx_virt_buf,ep_rx_phys_buf);
        dma_free_coherent(NULL,dma_buf_size,(void*)dma_virt_buf,dma_phys_buf);
        iounmap((void*)ep_tx_virt_buf);
        return -1;
	}    
#endif
    ep_task_start_sync->work_done = 1;

    return 0;
}

void pcie_ep_test_exit(void)
{
    ep_task_run = 0;
    mdelay(200);
        
    dma_free_coherent(NULL,(test_buf_size + sync_buf_size),(void*)ep_rx_virt_buf,ep_rx_phys_buf);
    ep_rx_virt_buf = NULL;
    ep_rx_phys_buf = 0;
    sync_virt_buf = NULL;
    sync_phys_buf = 0;
    iounmap((void*)ep_tx_virt_buf);
    dma_free_coherent(NULL,dma_buf_size,(void*)dma_virt_buf,dma_phys_buf);
    dma_virt_buf = NULL;
    dma_phys_buf = 0;
}

module_init(pcie_ep_test_init);
module_exit(pcie_ep_test_exit);


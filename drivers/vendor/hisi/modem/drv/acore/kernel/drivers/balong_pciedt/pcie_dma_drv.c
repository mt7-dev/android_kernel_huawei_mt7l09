#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/mbus.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <asm/mach/pci.h>
#include <asm/delay.h>
#include <linux/spinlock.h>
#include <asm/signal.h>
#include <asm/string.h>
#include "bsp_memmap.h"
#include "soc_interrupts.h"
#include "mach/pcie.h"
#include "pcie_dma_hal.h"
#include "pcie_dma_drv.h"

extern struct pci_dev* balong_rc;
extern struct pci_dev* balong_ep;
u32 dma_pos = PCIE_LOCAL;
static dma_chn_info dma_wr_chn[MAX_CHN_NUM];
static dma_chn_info dma_rd_chn[MAX_CHN_NUM];
static struct semaphore dma_ctx_sema;
static struct semaphore dma_wr_chn_sema[MAX_CHN_NUM];
static struct semaphore dma_rd_chn_sema[MAX_CHN_NUM];

static u32 pctrl_addr = (unsigned long)HI_IO_ADDRESS(REG_BASE_PCTRL);
static u32 pcie_cfg_addr = 0;

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
                    if(dma_wr_chn[i].done_isr)
                    {
                        dma_wr_chn[i].done_isr(dma_wr_chn[i].done_isr_private);
                    }
                    dma_wr_chn[i].done_isr_count++;
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
                    if(dma_wr_chn[i].abort_isr)
                    {
                        dma_wr_chn[i].abort_isr(dma_wr_err_status,0,dma_wr_chn[i].abort_isr_private);
                    }
                    dma_wr_chn[i].abort_isr_count++;
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
                    if(dma_rd_chn[i].done_isr)
                    {
                        dma_rd_chn[i].done_isr(dma_rd_chn[i].done_isr_private);
                    }
                    dma_rd_chn[i].done_isr_count++;
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
                    if(dma_rd_chn[i].abort_isr)
                    {
                        dma_rd_chn[i].abort_isr(dma_rd_err_lstatus,dma_rd_err_hstatus,dma_rd_chn[i].abort_isr_private);
                    }
                    dma_rd_chn[i].abort_isr_count++;
                }
            }
        }
    }
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
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
static irqreturn_t pcie_remote_dma_isr(s32 irq, void* private)
{
    u32 dma_wr_int_status = 0;
    u32 dma_wr_err_status = 0;
    u32 dma_rd_int_status = 0;
    u32 dma_rd_err_lstatus = 0;
    u32 dma_rd_err_hstatus = 0;

    printk(KERN_ERR "irq-%d enter.\n",irq);
    pci_read_config_dword(balong_ep,DMA_WR_INT_STATUS,&dma_wr_int_status);
    pci_read_config_dword(balong_ep,DMA_WR_ERR_STATUS,&dma_wr_err_status);
    pci_read_config_dword(balong_ep,DMA_RD_INT_STATUS,&dma_rd_int_status);
    pci_read_config_dword(balong_ep,DMA_RD_ERR_STATUS_LOW,&dma_rd_err_lstatus);
    pci_read_config_dword(balong_ep,DMA_RD_ERR_STATUS_HIGH,&dma_rd_err_hstatus);
    pci_write_config_dword(balong_ep,DMA_WR_INT_CLEAR,dma_wr_int_status);
    pci_write_config_dword(balong_ep,DMA_RD_INT_CLEAR,dma_rd_int_status);
    printk(KERN_ERR "dma_wr_int_status:0x%x,dma_rd_int_status:0x%x.\n",dma_wr_int_status,dma_rd_int_status);

    pcie_dma_common_isr(dma_wr_int_status,dma_wr_err_status,
                         dma_rd_int_status,dma_rd_err_lstatus,
                         dma_rd_err_hstatus);
    printk(KERN_ERR "irq-%d exit.\n",irq);
    return IRQ_HANDLED;
}
#endif
s32 pcie_dma_common_init(u32 dma_pos)
{
    s32 ret = 0;
    u32 val = 0;
    s32 i = 0;
    dma_chn_ctx ctx;

    ret = pcie_dma_wr_engine_disable(dma_pos);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_wr_engine_disable fail.");
        return ret;
    }

    ret = pcie_dma_rd_engine_disable(dma_pos);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_rd_engine_disable fail.");
        return ret;
    }

    /*mask all wr int*/
    val = (abort_int_mask<<abort_int_pos) | (done_int_mask<<done_int_pos);
    ret = pcie_dma_wr_int_mask_set(dma_pos, val);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_wr_int_mask_set fail.");
        return ret;
    }

    /*clear all wr int*/
    val = (abort_int_mask<<abort_int_pos) | (done_int_mask<<done_int_pos);
    ret = pcie_dma_wr_int_clear(dma_pos, val);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_wr_int_mask_set fail.");
        return ret;
    }

    /*mask all rd int*/
    val = (abort_int_mask<<abort_int_pos) | (done_int_mask<<done_int_pos);
    ret = pcie_dma_rd_int_mask_set(dma_pos, val);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_wr_int_mask_set fail.");
        return ret;
    }

    /*clear all rd int*/
    val = (abort_int_mask<<abort_int_pos) | (done_int_mask<<done_int_pos);
    ret = pcie_dma_rd_int_clear(dma_pos, val);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_wr_int_mask_set fail.");
        return ret;
    }

    if(dma_pos == PCIE_LOCAL)
    {
        if(request_irq(INT_LVL_PCIE0_EDMA, pcie_local_dma_isr, IRQF_DISABLED, "pcie local dma int", NULL))
        {
            printk(KERN_ERR "pcie edma irq request failed!\n");
            return -1;
    	}
    }
    else
    {
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
        if(request_irq(balong_ep->irq, pcie_remote_dma_isr, IRQF_DISABLED, "pcie local dma int", NULL))
        {
            printk(KERN_ERR "pcie edma irq request failed!\n");
            return -1;
    	}
#endif
    }


    /*unmask all wr int*/
    ret = pcie_dma_wr_int_mask_set(dma_pos, 0);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_wr_int_mask_set fail.");
        return ret;
    }

    /*unmask all rd int*/
    ret = pcie_dma_rd_int_mask_set(dma_pos, 0);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_wr_int_mask_set fail.");
        return ret;
    }

    for(i = 0; i < MAX_CHN_NUM; i++)
    {
        memset((void*)&ctx,0,sizeof(dma_chn_ctx));

        ret = pcie_dma_chn_ctx_idx_set(dma_pos, WR, i);
        if(ret)
        {
            printk(KERN_ERR "pcie_dma_chn_ctx_idx_set fail.");
            return ret;
        }

        ret = pcie_dma_chn_ctx_set(dma_pos,&ctx);
        if(ret)
        {
            printk(KERN_ERR "pcie_dma_chn_ctx_set fail.");
            return ret;
        }

        ret = pcie_dma_chn_ctx_idx_set(dma_pos, RD, i);
        if(ret)
        {
            printk(KERN_ERR "pcie_dma_chn_ctx_idx_set fail.");
            return ret;
        }

        ret = pcie_dma_chn_ctx_set(dma_pos,&ctx);
        if(ret)
        {
            printk(KERN_ERR "pcie_dma_chn_ctx_set fail.");
            return ret;
        }
#if defined(CONFIG_PCI_MSI)
        if(dma_pos == PCIE_REMOTE)
        {
            ret = pcie_dma_wr_done_imwr_adr_set(dma_pos,MSI_LOCAL_LOWER_ADDR,MSI_LOCAL_UPPER_ADDR);
            if(ret)
            {
                printk(KERN_ERR "pcie_dma_wr_done_imwr_adr_set fail.");
                return ret;
            }
            ret = pcie_dma_wr_abort_imwr_adr_set(dma_pos,MSI_LOCAL_LOWER_ADDR,MSI_LOCAL_UPPER_ADDR);
            if(ret)
            {
                printk(KERN_ERR "pcie_dma_wr_abort_imwr_adr_set fail.");
                return ret;
            }
            ret = pcie_dma_wr_chn_imwr_data_set(dma_pos,i,0);
            if(ret)
            {
                printk(KERN_ERR "pcie_dma_wr_chn_imwr_data_set fail.");
                return ret;
            }
            ret = pcie_dma_rd_done_imwr_adr_set(dma_pos,MSI_LOCAL_LOWER_ADDR,MSI_LOCAL_UPPER_ADDR);
            if(ret)
            {
                printk(KERN_ERR "pcie_dma_rd_done_imwr_adr_set fail.");
                return ret;
            }
            ret = pcie_dma_rd_abort_imwr_adr_set(dma_pos,MSI_LOCAL_LOWER_ADDR,MSI_LOCAL_UPPER_ADDR);
            if(ret)
            {
                printk(KERN_ERR "pcie_dma_rd_abort_imwr_adr_set fail.");
                return ret;
            }
            ret = pcie_dma_rd_chn_imwr_data_set(dma_pos,i,0);
            if(ret)
            {
                printk(KERN_ERR "pcie_dma_rd_chn_imwr_data_set fail.");
                return ret;
            }
        }
#endif
    }

    ret = pcie_dma_wr_engine_enable(dma_pos);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_wr_engine_enable fail.");
        return ret;
    }

    ret = pcie_dma_rd_engine_enable(dma_pos);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_rd_engine_enable fail.");
        return ret;
    }

    return 0;
}

s32 pcie_dma_init(void)
{
    s32 ret = 0;
    s32 i = 0;
    struct pcie_info* info;

    info = (struct pcie_info*)(((struct pci_sys_data*)(balong_rc->sysdata))->private_data);
    pcie_cfg_addr = info->conf_base_addr;

    sema_init(&dma_ctx_sema, 1);

    for(i = 0; i < MAX_CHN_NUM; i++)
    {
        memset((void*)&dma_wr_chn[i],0,sizeof(dma_chn_info));
        dma_wr_chn[i].id = i;
        dma_wr_chn[i].type = WR;
        sema_init(&dma_wr_chn_sema[i],1);

        memset((void*)&dma_rd_chn[i],0,sizeof(dma_chn_info));
        dma_rd_chn[i].id = i;
        dma_rd_chn[i].type = RD;
        sema_init(&dma_rd_chn_sema[i],1);
    }

    ret = pcie_dma_common_init(PCIE_LOCAL);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_common_init PCIE_LOCAL fail.");
        return ret;
    }

    ret = pcie_dma_common_init(PCIE_REMOTE);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_common_init PCIE_REMOTE fail.");
        return ret;
    }

    return 0;
}

void pcie_dma_deinit(void)
{
    free_irq(INT_LVL_PCIE0_EDMA,NULL);
#if defined(MSI_SUPPORT_1_DEV_1_BIT)
    free_irq(balong_ep->irq,NULL);
#endif
}

s32 pcie_dma_chn_allocate(u32 type, u32* chn_id)
{
    s32 i = 0;
    dma_chn_info* chn_info = NULL;
    struct semaphore* chn_sem;

    if(type == WR)
    {
        chn_info = dma_wr_chn;
        chn_sem = dma_wr_chn_sema;
    }
    else
    {
        chn_info = dma_rd_chn;
        chn_sem = dma_rd_chn_sema;
    }

    for(i = 0; i < MAX_CHN_NUM; i++)
    {
        down(&chn_sem[i]);
        if(chn_info[i].status == not_allocated)
        {
            *chn_id = (u32)i;
            chn_info[i].status = allocated;
            up(&chn_sem[i]);
            return 0;
        }
        up(&chn_sem[i]);
    }

    return -1;
}

s32 pcie_dma_chn_status_get(u32 position,u32 wr_or_rd,u32 chn_id,u32* status)
{
    s32 ret = 0;
    dma_chn_ctx chn_ctx;

    down(&dma_ctx_sema);

    ret = pcie_dma_chn_ctx_idx_set(position,wr_or_rd,chn_id);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_ctx_idx_set fail.");
        up(&dma_ctx_sema);
        return ret;
    }

    ret = pcie_dma_chn_ctx_get(position,&chn_ctx);
    if(ret)
    {
        printk(KERN_ERR "pcie_cfg_reg_read DMA_CHN_CTRL1_REG fail.");
        up(&dma_ctx_sema);
        return ret;
    }

    up(&dma_ctx_sema);
    *status = ((chn_ctx.ctrl_reg1)>>5)&0x3;

    return 0;
}

s32 pcie_dma_chn_free(u32 type, u32 chn_id)
{
    dma_chn_info* chn_info = NULL;
    u32 status = 0;
    s32 ret = 0;
    struct semaphore* chn_sem;

    if(type == WR)
    {
        chn_info = &dma_wr_chn[chn_id];
        chn_sem = &dma_wr_chn_sema[chn_id];
    }
    else
    {
        chn_info = &dma_rd_chn[chn_id];
        chn_sem = &dma_rd_chn_sema[chn_id];
    }

    /*check chip chn status*/
    ret = pcie_dma_chn_status_get(dma_pos,type,chn_id,&status);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_status_get fail.");
        return ret;
    }

    if(status == running)
    {
        printk(KERN_ERR "chn is running, can't free.");
        return -1;
    }

    down(chn_sem);
    memset((void*)chn_info,0,sizeof(dma_chn_info));
    chn_info->id = chn_id;
    chn_info->type = type;
    up(chn_sem);

    return 0;
}
s32 pcie_dma_single_blk_config(u32 type, u32 chn_id, dma_blk_info* single_blk)
{
    s32 ret = 0;
    dma_chn_info* chn_info = NULL;
    u32 status = 0;
    struct semaphore* chn_sem;

    if(type == WR)
    {
        chn_info = &dma_wr_chn[chn_id];
        chn_sem = &dma_wr_chn_sema[chn_id];
    }
    else
    {
        chn_info = &dma_rd_chn[chn_id];
        chn_sem = &dma_rd_chn_sema[chn_id];
    }

    down(chn_sem);
    if(chn_info->status == not_allocated)
    {
        printk(KERN_ERR "chn is not allocated, can't config.");
        up(chn_sem);
        return -1;
    }
    up(chn_sem);

    ret = pcie_dma_chn_status_get(dma_pos, type, chn_id, &status);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_status_get type:%d, id:%d fail.",type,chn_id);
        return ret;
    }

    if(status == running)
    {
        printk(KERN_ERR "chn is running, can't config.");
        return -1;
    }

    /*disable ll error int*/
    ret = pcie_dma_ll_err_disable(dma_pos, type, chn_id);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_ll_err_disable fail.");
        return ret;
    }

    /*set single blk info*/
    chn_info->type = type;
    chn_info->ctx.trans_size = single_blk->trans_size;
    chn_info->ctx.sar_low = single_blk->sar_low;
    chn_info->ctx.sar_high = single_blk->sar_high;
    chn_info->ctx.dar_low = single_blk->dar_low;
    chn_info->ctx.dar_high = single_blk->dar_high;

    /*set ctrl info*/
#if !defined(EP_DMA_RISE_LOCAL_INT)
    chn_info->ctx.ctrl_reg1 = (dma_pos == PCIE_LOCAL) ? 0x04000008 : 0x04000010;
#else
    chn_info->ctx.ctrl_reg1 = 0x04000008;
#endif
    chn_info->ctx.ctrl_reg2 = 0;

    /*set int routine*/
    chn_info->done_isr = single_blk->done_isr;
    chn_info->done_isr_private = single_blk->done_isr_private;
    chn_info->abort_isr = single_blk->abort_isr;
    chn_info->abort_isr_private = single_blk->abort_isr_private;

    down(&dma_ctx_sema);
    ret = pcie_dma_chn_ctx_idx_set(dma_pos, chn_info->type, chn_info->id);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_ctx_idx_set fail.");
        up(&dma_ctx_sema);
        return ret;
    }

    ret = pcie_dma_chn_ctx_set(dma_pos,&(chn_info->ctx));
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_ctx_set fail.");
        up(&dma_ctx_sema);
        return ret;
    }
    up(&dma_ctx_sema);

    return 0;
}

s32 pcie_dma_ll_config(u32 type, u32 chn_id, dma_ll_info* ll_info)
{
    s32 ret = 0;
    dma_chn_info* chn_info = NULL;
    u32 status = 0;
    struct semaphore* chn_sem;

    if(type == WR)
    {
        chn_info = &dma_wr_chn[chn_id];
        chn_sem = &dma_wr_chn_sema[chn_id];
    }
    else
    {
        chn_info = &dma_rd_chn[chn_id];
        chn_sem = &dma_rd_chn_sema[chn_id];
    }

    down(chn_sem);
    if(chn_info->status == not_allocated)
    {
        printk(KERN_ERR "chn is not allocated, can't config.");
        up(chn_sem);
        return -1;
    }
    up(chn_sem);

    ret = pcie_dma_chn_status_get(dma_pos, type, chn_id, &status);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_status_get type:%d, id:%d fail.",type,chn_id);
        return ret;
    }
    if(status == running)
    {
        printk(KERN_ERR "chn is running, can't config.");
        return -1;
    }

    /*enable ll error int*/
    ret = pcie_dma_ll_err_enable(dma_pos, type, chn_id);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_ll_err_disable fail.");
        return ret;
    }

    /*set ctx info*/
    chn_info->type = type;
    chn_info->ctx.trans_size = 0;
    chn_info->ctx.sar_low = 0;
    chn_info->ctx.sar_high = 0;
    chn_info->ctx.dar_low = 0;
    chn_info->ctx.dar_high = 0;
    chn_info->ctx.ll_ptr_low = ll_info->ll_ptr_low;
    chn_info->ctx.ll_ptr_high = ll_info->ll_ptr_high;

    /*set ctrl info*/
#if !defined(EP_DMA_RISE_LOCAL_INT)
    chn_info->ctx.ctrl_reg1 = (dma_pos == PCIE_LOCAL) ? 0x04000308 : 0x04000310;
#else
    chn_info->ctx.ctrl_reg1 = 0x04000308;
#endif
    chn_info->ctx.ctrl_reg2 = 0;

    /*set int routine*/
    chn_info->done_isr = ll_info->done_isr;
    chn_info->done_isr_private = ll_info->done_isr_private;
    chn_info->abort_isr = ll_info->abort_isr;
    chn_info->abort_isr_private = ll_info->abort_isr_private;

    down(&dma_ctx_sema);
    ret = pcie_dma_chn_ctx_idx_set(dma_pos, chn_info->type, chn_info->id);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_ctx_idx_set fail.");
        up(&dma_ctx_sema);
        return ret;
    }

    ret = pcie_dma_chn_ctx_set(dma_pos,&(chn_info->ctx));
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_ctx_set fail.");
        up(&dma_ctx_sema);
        return ret;
    }
    up(&dma_ctx_sema);
    return 0;
}

/*build cycle ll or single ll*/
/*todo: need check??*/
s32 pcie_dma_ll_build(dma_ll* cur_ll,dma_data_ele* data_ele,dma_ll* next_ll)
{
    s32 i = 0;

    cur_ll->data_ele = (dma_data_ele*)cur_ll->virt_low_adr;

    for(i = 0; i < cur_ll->data_ele_num; i++)
    {
        memcpy((void*)&(cur_ll->data_ele[i]),(void*)&data_ele[i],sizeof(dma_ll_ele));
        cur_ll->data_ele[i].ctrl_reg1 = 0x1;
    }

    if(cur_ll->cycled)
    {
        cur_ll->ll_ele = (dma_ll_ele*)&(cur_ll->data_ele[cur_ll->data_ele_num]);
        cur_ll->ll_ele->ctrl_reg1 = 0x6;
        cur_ll->ll_ele->ll_ptr_low = cur_ll->phys_low_adr;
        cur_ll->ll_ele->ll_ptr_high = cur_ll->phys_high_adr;

        /*config watermark and empty int*/
#if !defined(EP_DMA_RISE_LOCAL_INT)
        cur_ll->data_ele[cur_ll->data_ele_num/2 + 1].ctrl_reg1
               = (dma_pos == PCIE_LOCAL) ? 0x9 : 0x11;
        cur_ll->data_ele[cur_ll->data_ele_num - 1].ctrl_reg1
               = (dma_pos == PCIE_LOCAL) ? 0x9 : 0x11;
#else
        cur_ll->data_ele[cur_ll->data_ele_num/2 + 1].ctrl_reg1
               = 0x9;
        cur_ll->data_ele[cur_ll->data_ele_num - 1].ctrl_reg1
               = 0x9;
#endif
    }
    else if(next_ll != NULL)
    {
        cur_ll->ll_ele = (dma_ll_ele*)&(cur_ll->data_ele[cur_ll->data_ele_num]);
        cur_ll->ll_ele->ctrl_reg1 = 0x6;
        cur_ll->ll_ele->ll_ptr_low = next_ll->phys_low_adr;
        cur_ll->ll_ele->ll_ptr_high = next_ll->phys_high_adr;
    }
    else
    {
#if !defined(EP_DMA_RISE_LOCAL_INT)
        cur_ll->data_ele[cur_ll->data_ele_num - 1].ctrl_reg1
               = (dma_pos == PCIE_LOCAL) ? 0x9 : 0x11;
#else
        cur_ll->data_ele[cur_ll->data_ele_num - 1].ctrl_reg1
               = 0x9;
#endif
        cur_ll->ll_ele = (dma_ll_ele*)&(cur_ll->data_ele[cur_ll->data_ele_num]);
        cur_ll->ll_ele->ctrl_reg1 = 0x6;
        cur_ll->ll_ele->ll_ptr_low = cur_ll->phys_low_adr;
        cur_ll->ll_ele->ll_ptr_high = cur_ll->phys_high_adr;
    }

    return 0;
}

s32 pcie_dma_chn_start(u32 type,u32 chn_id)
{
    s32 ret = 0;
    dma_chn_info* chn_info = NULL;
    struct semaphore* chn_sem;

    if(type == WR)
    {
        chn_info = &dma_wr_chn[chn_id];
        chn_sem = &dma_wr_chn_sema[chn_id];
        ret = pcie_dma_wr_chn_start(dma_pos, chn_id);
    }
    else
    {
        chn_info = &dma_rd_chn[chn_id];
        chn_sem = &dma_rd_chn_sema[chn_id];
        ret = pcie_dma_rd_chn_start(dma_pos, chn_id);
    }
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_start type:%d, id:%d fail.",type,chn_id);
        return ret;
    }

    return 0;
}

s32 pcie_dma_chn_stop(u32 type,u32 chn_id)
{
    s32 ret = 0;
    u32 status = 0;
    dma_chn_info* chn_info;
    struct semaphore* chn_sem;

    if(type == WR)
    {
        chn_info = &dma_wr_chn[chn_id];
        chn_sem = &dma_wr_chn_sema[chn_id];
        ret = pcie_dma_wr_chn_stop(dma_pos, chn_id);
    }
    else
    {
        chn_info = &dma_rd_chn[chn_id];
        chn_sem = &dma_rd_chn_sema[chn_id];
        ret = pcie_dma_rd_chn_stop(dma_pos, chn_id);
    }

    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_start type:%d, id:%d fail.",type,chn_id);
        return ret;
    }

    ret = pcie_dma_chn_status_get(dma_pos, type, chn_id, &status);
    if(ret)
    {
        printk(KERN_ERR "pcie_dma_chn_status_get type:%d, id:%d fail.",type,chn_id);
        return ret;
    }

    if(status != stopped)
    {
        printk(KERN_ERR "status != stopped,pcie_dma_chn_stop fail.");
        return -1;
    }

    return 0;
}


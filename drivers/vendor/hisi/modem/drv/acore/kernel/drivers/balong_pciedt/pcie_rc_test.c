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
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/atomic.h>
#include <linux/kthread.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/msi.h>
#include <linux/pci_ids.h>
#include <mach/pcie.h>
#include "pcie_dma_drv.h"
#include "pcie_test.h"

#define BALONG_PCI_VENDOR_ID		PCI_VENDOR_ID_HISILICON
#if defined(CONFIG_ARCH_V7R2)
#define BALONG_PCI_DEVICE_ID		PCI_DEVICE_ID_HI6930
#elif defined(CONFIG_ARCH_P531)
#define BALONG_PCI_DEVICE_ID		PCI_DEVICE_ID_P531
#endif

struct pci_dev* balong_rc = NULL;
struct pci_dev *balong_ep = NULL;

static u8* read_buf = NULL;
static u8* write_buf = NULL;

static u8 *rc_rx_virt_buf = NULL;
static dma_addr_t rc_rx_phys_buf = 0;
static u8 *rc_tx_virt_buf = NULL;
static u32 test_buf_size = SZ_2M;
static u32 sync_buf_size = SZ_2M;

static tx_rx_sync* rc_tx_ep_rx_sync;
static tx_rx_sync* ep_task_start_sync;
static tx_rx_sync* rc_rx_ep_tx_sync;
static tx_rx_sync* ep_read_rc_sync;
static dma_buf_sync* ep_dma_buf_sync;
static u32* intx_event_sync;

static u8* dma_virt_buf = NULL;
static dma_addr_t dma_phys_buf;
static u8* dma_remote_virt_buf = NULL;
static dma_addr_t dma_remote_phys_buf = 0;
static u32 dma_buf_size = SZ_4M;

struct pci_dev* pcie_find_rc(u32 vendor_id,u32 device_id)
{
	struct pci_dev *dev = pci_get_device(vendor_id,
							device_id, NULL);
    
	if((NULL != dev) && ((dev->class >> 8) == PCI_CLASS_BRIDGE_PCI))
    {
		printk(KERN_ERR "PCIE RC:Found BALONG PCIe RC @0x%08x\n",(int)dev);
		return dev;       
	}

	printk(KERN_ERR  "PCIE RC:No more BALONG PCIe RC found\n");
	return NULL;    
}

struct pci_dev* pcie_find_dev(u32 vendor_id,u32 device_id)
{
	struct pci_dev *dev = pci_get_device(vendor_id,
							device_id, NULL);
	while (NULL != dev) {
		if ((dev->class >> 8) == PCI_CLASS_BRIDGE_PCI) {
			printk(KERN_ERR  
					"PCIE RC:skipping BALONG PCIe RC...\n");

			dev = pci_get_device(BALONG_PCI_VENDOR_ID,
							device_id, dev);

			continue;
		}

		printk(KERN_ERR "PCIE RC:Found BALONG PCIe EP @0x%08x\n",
								(int)dev);
		return dev;
	}

	printk(KERN_ERR  "PCIE RC:No more BALONG PCIe EP found\n");
	return NULL;    
}

static s32 pcie_set_iatu(const struct pci_dev* dev,
                       u32 viewport, 
                       u32 region_ctrl_1, 
                       u32 region_ctrl_2,
                       u32 lbar,
                       u32 ubar,
                       u32 lar,
                       u32 ltar,
                       u32 utar)
{
    s32 ret = 0;
    ret |= pci_write_config_dword(dev,PORT_LOGIC_BASE + 0x200,viewport);
    ret |= pci_write_config_dword(dev,PORT_LOGIC_BASE + 0x204,region_ctrl_1);
    ret |= pci_write_config_dword(dev,PORT_LOGIC_BASE + 0x20c,lbar);
    ret |= pci_write_config_dword(dev,PORT_LOGIC_BASE + 0x210,ubar);
    ret |= pci_write_config_dword(dev,PORT_LOGIC_BASE + 0x214,lar);
    ret |= pci_write_config_dword(dev,PORT_LOGIC_BASE + 0x218,ltar);
    ret |= pci_write_config_dword(dev,PORT_LOGIC_BASE + 0x21c,utar);    
    ret |= pci_write_config_dword(dev,PORT_LOGIC_BASE + 0x208,region_ctrl_2);

    return ret;
}

/*
rc_tx_virt_buf          
1)data_buffer   size 2MB
2)sync_buffer   size 2MB

ep allocate ddr buffer size 2MB+2MB

bar0 size 16MB
only used 2MB + 2MB
so just ioremap 2MB+2MB
*/
s32 rc_buf_init(void)
{
    resource_size_t pciaddr;
    u8* start;
    s32 ret = 0;

    /*rx buf*/       
    pciaddr = pci_resource_start(balong_ep,0);
    if(!pciaddr)
    {
        printk(KERN_ERR "PCIE RC:pci_resource_start bar0 fail.");
        return -EIO;
    }
    
    start = (u8*)ioremap(pciaddr,test_buf_size + sync_buf_size);
    if(!start)
    {
        printk(KERN_ERR "PCIE RC:ioremap bar0.");
        return -EIO;
    }

    rc_tx_virt_buf = start;
    rc_tx_ep_rx_sync = (tx_rx_sync*)((u32)rc_tx_virt_buf + test_buf_size);
    ep_task_start_sync = (tx_rx_sync*)((u32)rc_tx_ep_rx_sync + sizeof(tx_rx_sync));
    ep_dma_buf_sync = (dma_buf_sync*)((u32)ep_task_start_sync + sizeof(tx_rx_sync));
    intx_event_sync = (u32*)((u32)ep_dma_buf_sync + sizeof(dma_buf_sync));


    printk(KERN_ERR "PCIE RC:rc_tx_virt_buf:0x%x.",(u32)rc_tx_virt_buf);
    printk(KERN_ERR "PCIE RC:rc_tx_ep_rx_sync:0x%x.",(u32)rc_tx_ep_rx_sync);
    printk(KERN_ERR "PCIE RC:ep_task_start_sync:0x%x.",(u32)ep_task_start_sync);
    printk(KERN_ERR "PCIE RC:ep_dma_buf_sync:0x%x.",(u32)ep_dma_buf_sync);

    /*tx buf*/
    
    /*set rc inbound iatu for recive by cpu*/
    rc_rx_virt_buf = (u8*)dma_alloc_coherent(NULL,(test_buf_size + sync_buf_size),&rc_rx_phys_buf, GFP_DMA|__GFP_WAIT);
    if(rc_rx_virt_buf == NULL)
    {
        printk(KERN_ERR "PCIE RC:dma_alloc_coherent rc_rx_virt_buf fail.");
        return -EIO;
    }
    printk(KERN_ERR "PCIE RC:rc_rx_virt_buf:0x%x,rc_rx_phys_buf:0x%x.",(u32)rc_rx_virt_buf,rc_rx_phys_buf);

    ret = pcie_set_iatu(balong_rc,
             0x80000001,
             0x0,
             0x80000000,
             0x80000000,
             0x0,
             (0x80000000 + test_buf_size + sync_buf_size - 1),
             (u32)rc_rx_phys_buf,
             0x0
             );
    if(ret)
    {
        return -EIO;
    }
    
    rc_rx_ep_tx_sync = (tx_rx_sync*)((u32)rc_rx_virt_buf + test_buf_size);
    ep_read_rc_sync = (tx_rx_sync*)((u32)rc_rx_ep_tx_sync + sizeof(tx_rx_sync));
    
    printk(KERN_ERR "PCIE RC:rc_rx_ep_tx_sync:0x%x.",(u32)rc_rx_ep_tx_sync);
    printk(KERN_ERR "PCIE RC:ep_read_rc_sync:0x%x.\n",(u32)ep_read_rc_sync);    
    return 0;
}
void rc_buf_deinit(void)
{
    iounmap((void*)rc_tx_virt_buf);

    rc_tx_virt_buf = NULL;
    rc_tx_ep_rx_sync = (tx_rx_sync*)NULL;
    ep_task_start_sync = (tx_rx_sync*)NULL;
    ep_dma_buf_sync = (dma_buf_sync*)NULL;
    intx_event_sync = (u32*)NULL;
    
    dma_free_coherent(NULL,(test_buf_size + sync_buf_size),(void*)rc_rx_virt_buf,rc_rx_phys_buf);
    rc_rx_virt_buf = NULL;
    rc_rx_phys_buf = 0;   
    rc_rx_ep_tx_sync = (tx_rx_sync*)NULL;
    ep_read_rc_sync = (tx_rx_sync*)NULL;
}
/*rc mw tlp and mr tlp base test function*/
s32 write_read_ep_by_cpu_base_test(u32 len, u8 pattern, u8 offset)
{
    s32 ret = 0;
    
    write_buf = (u8*)kmalloc(len,GFP_KERNEL);
    read_buf =  (u8*)kmalloc(len,GFP_KERNEL);

    if((write_buf == NULL) || (read_buf == NULL))
    {
        printk(KERN_ERR "PCIE RC:dma_alloc_coherent write and read buf or read buf fail.");
        ret = -EIO;
        goto TEST_END;
    }

    memset((void*)(write_buf + offset),pattern,len);
    memset((void*)(read_buf + offset),0,len);
    memset((void*)(rc_tx_virt_buf+ offset),0,len);

    memcpy((void*)(rc_tx_virt_buf + offset), (void*)(write_buf + offset), len);

    if(len < 256)
    {
        udelay(15);
    }
    else
    {
        msleep(1000);
    }
    
    memcpy((void*)(read_buf + offset),(void*)(rc_tx_virt_buf + offset),len);

    if(memcmp((void*)(write_buf + offset),(void*)(read_buf + offset),len))
    {
        printk(KERN_ERR "PCIE RC:memcmp fail.");
        printk(KERN_ERR "PCIE RC:write_buf:0x%x,read_buf:0x%x.",(u32)(write_buf + offset),(u32)(read_buf + offset));
        ret = -EIO;
        goto TEST_END;
    }

TEST_END:
    kfree((void*)write_buf);
    kfree((void*)read_buf);
    write_buf = read_buf = 0; 
    return ret;
}

s32 rc_rx_from_ep_by_cpu(u32 len, u32 pattern,u32 offset)
{
    u32 i = 0;
    
    memset((void*)rc_rx_ep_tx_sync,0,sizeof(tx_rx_sync));
    
    rc_rx_ep_tx_sync->trans_pattern = pattern; 
    rc_rx_ep_tx_sync->trans_len = len;
    rc_rx_ep_tx_sync->trans_times = 1;
    rc_rx_ep_tx_sync->addr_offset = offset;
    rc_rx_ep_tx_sync->work_start = 1;
    rc_rx_ep_tx_sync->readback_check = 1;

    while(!rc_rx_ep_tx_sync->work_done)
    {
        msleep(2);
    }

    for(i = 0; i < len; i++)
    {
        if(rc_rx_virt_buf[i + offset] != (u8)pattern)
        {
            printk(KERN_ERR "PCIE RC:compare fail,rc rx(buf:0x%x,pos:%d,data:%x).",
                (u32)rc_rx_virt_buf,i,rc_rx_virt_buf[i]);
            return -EIO;
        }
    }

    if(rc_rx_ep_tx_sync->readback_check_pass == 0)
    {
        printk(KERN_ERR "PCIE RC:ep readback check fail.");
        return -EIO;        
    }
    else if(rc_rx_ep_tx_sync->readback_check_pass == 2)
    {
        printk(KERN_ERR "PCIE RC:ep readback buf allocate fail.");
        return -EIO;         
    }
    return 0;
}

static void calc_rate(u32 trans_times,u32 blk_size,u32 start_time,u32 end_time)
{
    u32 cost_ms = (end_time - start_time)*1000/HZ;
    u32 kb_size = (trans_times*blk_size) >> 10;
    u32 mb_size = (trans_times*blk_size) >> 20;
    u32 kb_rate = (cost_ms)?(trans_times*blk_size)/cost_ms:0;
    u32 mb_rate = (cost_ms)?(trans_times*blk_size)/(1000*cost_ms):0;

    printk(KERN_ERR "PCIE RC:total size:%d(KB)/%d(MB),cost times:%d(ms),rate:%d(KBps)/about:%d(MBps).\n",
        kb_size,mb_size,cost_ms,kb_rate,mb_rate);    
}

/*ep mw tlp rate*/
s32 rc_rx_from_ep_by_cpu_rate(void* data)
{
    u32 rx_start =0;
    u32 rx_end = 0;
    u8* p_last_byte = (u8*)((u32)rc_rx_virt_buf + test_buf_size - 1);
          
    memset((void*)rc_rx_ep_tx_sync,0,sizeof(tx_rx_sync));
    rc_rx_ep_tx_sync->trans_len = test_buf_size;
    rc_rx_ep_tx_sync->trans_times = 256;
    rc_rx_ep_tx_sync->work_start = 1; 

    *p_last_byte = 0;

    rx_start = jiffies;
    while(!rc_rx_ep_tx_sync->work_done)
    {
        msleep(50);
    }
    rx_end = jiffies;

    if(*p_last_byte != 0xff)
    {
        printk(KERN_ERR "PCIE RC:oh,bad,p_last_byte not 0xff.\n");
    }

    calc_rate(rc_rx_ep_tx_sync->trans_times,rc_rx_ep_tx_sync->trans_len,rx_start,rx_end);
    
    return 0;
}

/*rc mw tlp rate*/
s32 rc_tx_to_ep_by_cpu_rate(void* data)
{
    s32 i = 0;
    
    memset((void*)rc_tx_ep_rx_sync,0,sizeof(tx_rx_sync));
    rc_tx_ep_rx_sync->trans_len = test_buf_size;
    rc_tx_ep_rx_sync->trans_times = 256;
    rc_tx_ep_rx_sync->work_start = 1; 

    for(i = 0; i < rc_tx_ep_rx_sync->trans_times; i++)
    {
        memset((void*)rc_tx_virt_buf,i,test_buf_size);
    }

    rc_tx_ep_rx_sync->work_done = 1;
    
    while(rc_tx_ep_rx_sync->work_start)
    {
        msleep(50);
    }

    calc_rate(rc_tx_ep_rx_sync->trans_times ,rc_tx_ep_rx_sync->trans_len,
        rc_tx_ep_rx_sync->start_time,rc_tx_ep_rx_sync->end_time);
    return 0;
}


s32 rc_read_ep_by_cpu_rate(void* data)
{
    u32 read_start = 0;
    u32 read_end = 0;
    u8* readback_virt_buf = NULL;
    u32 i = 0;
    u32 test_times = 256;
    u32 test_size = 2*1024*1024;

    readback_virt_buf = (u8*)kmalloc(test_size,GFP_KERNEL);
    if(!readback_virt_buf)
    {
        printk(KERN_ERR
            "PCIE RC:kmalloc readback_virt_buf fail.");
        return -EIO;
    }
    
    read_start = jiffies;
    for(i = 0; i < test_times; i++)
    {
        memcpy((void*)readback_virt_buf,rc_tx_virt_buf,test_size);
    }
    read_end = jiffies;

    calc_rate(test_times,test_size,read_start,read_end);

    kfree((void*)readback_virt_buf);
    readback_virt_buf = NULL;
    return 0;
}

s32 ep_read_rc_by_cpu_rate(void* data)
{
    memset((void*)ep_read_rc_sync,0,sizeof(tx_rx_sync));
    
    ep_read_rc_sync->trans_len = 2*1024*1024;
    ep_read_rc_sync->trans_times = 256;
    ep_read_rc_sync->work_start = 1;

    while(!ep_read_rc_sync->work_done)
    {
        msleep(50);
    }
    ep_read_rc_sync->work_start = 0;

    if(ep_read_rc_sync->readback_check_pass == 2)
    {
        printk(KERN_ERR "PCIE RC:ep alloc readback buf fail.");
    }

    calc_rate(ep_read_rc_sync->trans_times,ep_read_rc_sync->trans_len,
        ep_read_rc_sync->start_time,ep_read_rc_sync->end_time);

    return 0;
}

/*****************************

Pcie Data Transfer Test By CPU

******************************/

/*rc mw tlp and mr tlp test case*/
s32 pcie_test_001_001(void)
{
    return write_read_ep_by_cpu_base_test(1,0x31,0); 
}

s32 pcie_test_001_001_2(void)
{
    return write_read_ep_by_cpu_base_test(3,0x31,0); 
}

s32 pcie_test_001_001_3(void)
{
    return write_read_ep_by_cpu_base_test(7,0x31,0); 
}

s32 pcie_test_001_001_4(void)
{
    return write_read_ep_by_cpu_base_test(8,0x31,0); 
}

s32 pcie_test_001_002(void)
{
    return write_read_ep_by_cpu_base_test(256,0x32,0);
}

s32 pcie_test_001_003(void)
{
    return write_read_ep_by_cpu_base_test(257,0x33,0);
}

s32 pcie_test_001_004(void)
{
    return write_read_ep_by_cpu_base_test(512,0x34,0);
}

s32 pcie_test_001_005(void)
{
    return write_read_ep_by_cpu_base_test(1024,0x35,0);
}

s32 pcie_test_001_006(void)
{
    return write_read_ep_by_cpu_base_test(4096,0x36,0);
}

s32 pcie_test_001_007(void)
{
    return write_read_ep_by_cpu_base_test(test_buf_size,0x37,0);
}

/*ep mw tlp and mr tlp test case*/
s32 pcie_test_001_008(void)
{
    return rc_rx_from_ep_by_cpu(1,0x38,0);
}

s32 pcie_test_001_009(void)
{
    return rc_rx_from_ep_by_cpu(256,0x39,0);
}

s32 pcie_test_001_010(void)
{
    return rc_rx_from_ep_by_cpu(257,0x40,0);
}

s32 pcie_test_001_011(void)
{
    return rc_rx_from_ep_by_cpu(512,0x41,0);
}

s32 pcie_test_001_012(void)
{
    return rc_rx_from_ep_by_cpu(1024,0x42,0);
}

s32 pcie_test_001_013(void)
{
    return rc_rx_from_ep_by_cpu(test_buf_size,0x43,0);
}

s32 pcie_test_001_014(void)
{
    return rc_rx_from_ep_by_cpu(255,0x44,0);
}

s32 pcie_test_001_015(void)
{
    return rc_rx_from_ep_by_cpu(254,0x45,0);
}

s32 pcie_test_001_016(void)
{
    return write_read_ep_by_cpu_base_test(8,0x46,1);
}
s32 pcie_test_001_017(void)
{
    return write_read_ep_by_cpu_base_test(8,0x47,2);
}
s32 pcie_test_001_018(void)
{
    return write_read_ep_by_cpu_base_test(8,0x48,3);
}
s32 pcie_test_001_019(void)
{
    return write_read_ep_by_cpu_base_test(8,0x49,4);
}
s32 pcie_test_001_020(void)
{
    return write_read_ep_by_cpu_base_test(8,0x4a,5);
}
s32 pcie_test_001_021(void)
{
    return write_read_ep_by_cpu_base_test(8,0x4b,6);
}
s32 pcie_test_001_022(void)
{
    return write_read_ep_by_cpu_base_test(8,0x4c,7);
}
s32 pcie_test_001_023(void)
{
    return write_read_ep_by_cpu_base_test(8,0x4d,8);
}
s32 pcie_test_001_024(s32 times)
{
    s32 i = 0;
    for(i = 0; i < times; i++)
    {
        pcie_test_001_016();
        pcie_test_001_017();
        pcie_test_001_018();
        pcie_test_001_019();
        pcie_test_001_020();
        pcie_test_001_021();
        pcie_test_001_022();
        pcie_test_001_023();
    }
    return 0;
}
s32 pcie_test_001_025(void)
{
    return rc_rx_from_ep_by_cpu(8,0x4e,1);
}
s32 pcie_test_001_026(void)
{
    return rc_rx_from_ep_by_cpu(8,0x4f,2);
}
s32 pcie_test_001_027(void)
{
    return rc_rx_from_ep_by_cpu(8,0x50,3);
}
s32 pcie_test_001_028(void)
{
    return rc_rx_from_ep_by_cpu(8,0x51,4);
}
s32 pcie_test_001_029(void)
{
    return rc_rx_from_ep_by_cpu(8,0x52,5);
}
s32 pcie_test_001_030(void)
{
    return rc_rx_from_ep_by_cpu(8,0x53,6);
}
s32 pcie_test_001_031(void)
{
    return rc_rx_from_ep_by_cpu(8,0x54,7);
}
s32 pcie_test_001_032(void)
{
    return rc_rx_from_ep_by_cpu(8,0x55,8);
}
s32 pcie_test_001_033(s32 times)
{
    s32 i = 0;
    for(i = 0; i < times; i++)
    {
        pcie_test_001_025();
        pcie_test_001_026();
        pcie_test_001_027();
        pcie_test_001_028();
        pcie_test_001_029();
        pcie_test_001_030();
        pcie_test_001_031();
        pcie_test_001_032();
    }
    return 0;
}
/*rate test, no data verify*/
s32 rc_tx_rate(void)
{
    kthread_run(rc_tx_to_ep_by_cpu_rate,0,"rc_tx_to_ep_by_cpu_rate");
    return 0;
}

s32 rc_rx_rate(void)
{
    kthread_run(rc_rx_from_ep_by_cpu_rate,0,"rc_rx_from_ep_by_cpu_rate");
    return 0;    
}
s32 rc_readback_rate(void)
{
    kthread_run(rc_read_ep_by_cpu_rate,0,"rc_read_ep_by_cpu_rate");
    return 0;
}

s32 ep_readback_rate(void)
{
    kthread_run(ep_read_rc_by_cpu_rate,0,"ep_read_rc_by_cpu_rate");
    return 0;    
}

s32 rc_ep_mwr_rate(void)
{
    rc_tx_rate();
    rc_rx_rate();
    return 0;
}

s32 rc_ep_mrd_rate(void)
{
    rc_readback_rate();
    ep_readback_rate();

    return 0;
}
/*****************************

Pcie Data Transfer Test By DMA

******************************/
typedef enum _dma_ll_status {
    no_ll = 0,
    no_cycled_ll = 1,
    cycled_ll = 2,
}dma_ll_status;
typedef struct _dma_test_isr_info {
    u32 chn_id;
    u32 type;
    u32 err_lstatus;
    u32 err_hstatus;
    u32 start_time;
    u32 end_time;
    dma_ll_status ll_status;
}dma_test_isr_info;

typedef struct _dma_task_info {
    u32 type;
    u32 blk_size;
    u32 data_ele_num;
    dma_data_ele* data_ele;
}dma_task_info;

extern u32 dma_pos;
struct semaphore dma_chn_sema[2][MAX_CHN_NUM];

dma_test_isr_info isr_info[2];
u32 test_done = 0;
u32 ll_total_data_max_size = (1024 + 512)*1024;

void pcie_dma_select(u32 pos)
{
    dma_pos = pos;
}

s32 pcie_dma_test_init(void)
{
    s32 i,j;
    
    if(dma_buf_size != ep_dma_buf_sync->size)
    {
        printk(KERN_ERR "PCIE RC:local dma buf size(0x%x) != remote dma buf size(0x%x)"
            ,dma_buf_size,ep_dma_buf_sync->size);
        return -EIO;
    }
    
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < MAX_CHN_NUM; j++)
        {
            sema_init(&(dma_chn_sema[i][j]),0);
        }
    }

    /*remote dma buf*/       
    dma_remote_phys_buf = pci_resource_start(balong_ep,2);
    if(!dma_remote_phys_buf)
    {
        printk(KERN_ERR "PCIE RC:pci_resource_start bar2 fail.");
        return -EIO;
    }
    
    dma_remote_virt_buf = (u8*)ioremap(dma_remote_phys_buf,ep_dma_buf_sync->size);
    if(!dma_remote_virt_buf)
    {
        printk(KERN_ERR "PCIE RC:ioremap bar2.");
        return -EIO;
    }    

    dma_virt_buf = (u8*)dma_alloc_coherent(NULL,dma_buf_size,&dma_phys_buf, GFP_DMA|__GFP_WAIT);
    if(dma_virt_buf == NULL)
    {
        printk(KERN_ERR "PCIE RC:dma_alloc_coherent dma_virt_buf fail.");
        return -1;
    }

    printk(KERN_ERR "PCIE RC:remote dma virt buf:0x%x,remote dma phys buf:0x%x.\n",
        (u32)dma_remote_virt_buf,dma_remote_phys_buf);
    printk(KERN_ERR "PCIE RC:local dma virt buf:0x%x,local dma phys buf:0x%x.\n",
        (u32)dma_virt_buf,dma_phys_buf);
    printk(KERN_ERR "PCIE RC:ep local dma phys buf:0x%x,ep remote dma phys buf:0x%x.\n",
        ep_dma_buf_sync->local_low,ep_dma_buf_sync->remote_low);         
   
    return 0;
}

s32 pcie_dma_test_deinit(void)
{
    dma_free_coherent(NULL,dma_buf_size,(void*)dma_virt_buf,dma_phys_buf);
    dma_virt_buf = NULL;   
    iounmap((void*)dma_remote_virt_buf);
    return 0;
}

void dma_done_test_isr(void* private)
{
    dma_test_isr_info* info = (dma_test_isr_info*)private; 
    info->end_time = jiffies;
    
    if(info->ll_status == cycled_ll)
    {
    }
    
    up(&(dma_chn_sema[info->type][info->chn_id]));
}

void dma_abort_test_isr(u32 err_lstatus,u32 err_hstatus,void* private)
{
    dma_test_isr_info* info = (dma_test_isr_info*)private; 

    info->err_lstatus = err_lstatus;
    info->err_hstatus = err_hstatus;

    up(&(dma_chn_sema[info->type][info->chn_id]));
}

s32 pcie_dma_single_blk_test(u32 blk_size,u32 pattern)
{
    s32 ret = 0;
    dma_blk_info wr_blk_info;
    dma_blk_info rd_blk_info;
    dma_test_isr_info isr_info;
    u32 wr_chn_id = 0xff;
    u32 rd_chn_id = 0xff;

    if(blk_size > dma_buf_size/2)
    {
        printk(KERN_ERR "PCIE RC:blk_size(0x%x) is not surppot.\n",blk_size);
        return -1;
    }

    memset((void*)&wr_blk_info,0,sizeof(dma_blk_info));
    memset((void*)&rd_blk_info,0,sizeof(dma_blk_info));
    
    ret = pcie_dma_chn_allocate(WR,&wr_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_allocate WR fail.");
        return ret;
    }

    ret = pcie_dma_chn_allocate(RD,&rd_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_allocate RD fail.");
        pcie_dma_chn_free(WR,wr_chn_id);
        return ret;
    }    

    memset((void*)&isr_info,0,sizeof(dma_test_isr_info));
    
    isr_info.chn_id = wr_chn_id;
    isr_info.type   = WR;
    
    wr_blk_info.trans_size = blk_size;
    if(dma_pos == PCIE_LOCAL)
    {
        wr_blk_info.sar_low = (u32)dma_phys_buf;
        wr_blk_info.sar_high = 0;
        wr_blk_info.dar_low = (u32)dma_remote_phys_buf;
        wr_blk_info.dar_high = 0;
    }
    else
    {
        wr_blk_info.sar_low = ep_dma_buf_sync->local_low;
        wr_blk_info.sar_high = ep_dma_buf_sync->local_high;
        wr_blk_info.dar_low = ep_dma_buf_sync->remote_low;
        wr_blk_info.dar_high = ep_dma_buf_sync->remote_high;        
    }
    wr_blk_info.done_isr = dma_done_test_isr;
    wr_blk_info.done_isr_private = (void*)&isr_info;
    wr_blk_info.abort_isr = dma_abort_test_isr;
    wr_blk_info.abort_isr_private = (void*)&isr_info;
    
    ret = pcie_dma_single_blk_config(WR, wr_chn_id, &wr_blk_info);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_single_blk_config WR-%d fail.",wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }

    /*clear buf*/
    if(dma_pos == PCIE_LOCAL)
    {
        memset((void*)dma_virt_buf,0,blk_size);
    }
    else
    {
        memset((void*)dma_remote_virt_buf,0xee,blk_size);
    }
    
    ret = pcie_dma_chn_start(WR,wr_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_start WR-%d fail.",wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    
    down(&dma_chn_sema[WR][wr_chn_id]);
    
    if((isr_info.err_hstatus)||(isr_info.err_hstatus))
    {
        printk(KERN_ERR "PCIE RC:pcie dma write failed.");
        pcie_dma_chn_stop(WR,wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;        
    }

    ret = pcie_dma_single_blk_config(WR, wr_chn_id, &wr_blk_info);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_single_blk_config WR-%d fail.",wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    
    /*init buf*/
    if(dma_pos == PCIE_LOCAL)
    {
        memset((void*)dma_virt_buf,pattern,blk_size);
    }
    else
    {
        memset((void*)dma_remote_virt_buf,pattern,blk_size);
    }
    
    ret = pcie_dma_chn_start(WR,wr_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_start WR-%d fail.",wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    
    down(&dma_chn_sema[WR][wr_chn_id]);
    
    if((isr_info.err_hstatus)||(isr_info.err_hstatus))
    {
        printk(KERN_ERR "PCIE RC:pcie dma write failed.");
        pcie_dma_chn_stop(WR,wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;        
    }

    memset((void*)&isr_info,0,sizeof(dma_test_isr_info));
    
    isr_info.chn_id = rd_chn_id;
    isr_info.type   = RD;

    rd_blk_info.trans_size = blk_size;
    rd_blk_info.sar_low = wr_blk_info.dar_low;
    rd_blk_info.sar_high = wr_blk_info.dar_high;
    rd_blk_info.dar_low = wr_blk_info.sar_low + dma_buf_size/2;
    rd_blk_info.dar_high = wr_blk_info.sar_high;
    
    rd_blk_info.done_isr = dma_done_test_isr;
    rd_blk_info.done_isr_private = (void*)&isr_info;
    rd_blk_info.abort_isr = dma_abort_test_isr;
    rd_blk_info.abort_isr_private = (void*)&isr_info;
    
    ret = pcie_dma_single_blk_config(RD, rd_chn_id, &rd_blk_info);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_single_blk_config RD-%d fail.",rd_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }

    ret = pcie_dma_chn_start(RD,rd_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_start RD-%d fail.",rd_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    
    down(&dma_chn_sema[RD][rd_chn_id]);
    
    if((isr_info.err_hstatus)||(isr_info.err_hstatus))
    {
        printk(KERN_ERR "PCIE RC:pcie dma read failed.");
        pcie_dma_chn_stop(RD,rd_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;        
    }

    if(dma_pos == PCIE_LOCAL)
    {
        if(memcmp((void*)((u32)dma_virt_buf + dma_buf_size/2),(void*)dma_virt_buf,blk_size))
        {
            printk(KERN_ERR "PCIE RC:memcmp(addr1:0x%x,addr2:0x%x) fail.",(u32)dma_virt_buf,((u32)dma_virt_buf + dma_buf_size/2));
        }
        else
        {
            printk(KERN_ERR "PCIE RC:memcmp(addr1:0x%x,addr2:0x%x) ok.",(u32)dma_virt_buf,((u32)dma_virt_buf + dma_buf_size/2));
        }
    }
    else
    {
        if(memcmp((void*)((u32)dma_remote_virt_buf + dma_buf_size/2),(void*)dma_remote_virt_buf,blk_size))
        {
            printk(KERN_ERR "PCIE RC:memcmp(addr1:0x%x,addr2:0x%x) fail.",(u32)dma_remote_virt_buf,((u32)dma_remote_virt_buf + dma_buf_size/2));
        }
        else
        {
            printk(KERN_ERR "PCIE RC:memcmp(addr1:0x%x,addr2:0x%x) ok.",(u32)dma_remote_virt_buf,((u32)dma_remote_virt_buf + dma_buf_size/2));
        }        
    }
    pcie_dma_chn_free(WR,wr_chn_id);
    pcie_dma_chn_free(RD,rd_chn_id);    
    return 0;
}

s32 pcie_test_002_001(void)
{
    return pcie_dma_single_blk_test(1,0x51);
}

s32 pcie_test_002_001_2(void)
{
    return pcie_dma_single_blk_test(3,0x58);
}

s32 pcie_test_002_001_3(void)
{
    return pcie_dma_single_blk_test(7,0x59);
}

s32 pcie_test_002_001_4(void)
{
    return pcie_dma_single_blk_test(8,0x60);
}


s32 pcie_test_002_002(void)
{
    return pcie_dma_single_blk_test(255,0x52);
}

s32 pcie_test_002_003(void)
{
    return pcie_dma_single_blk_test(256,0x53);
}

s32 pcie_test_002_004(void)
{
    return pcie_dma_single_blk_test(512,0x54);
}

s32 pcie_test_002_005(void)
{
    return pcie_dma_single_blk_test(1024,0x55);
}

s32 pcie_test_002_006(void)
{
    return pcie_dma_single_blk_test(4096,0x56);
}

s32 pcie_test_002_007(void)
{
    return pcie_dma_single_blk_test(2*1024*1024,0x57);
}

/*
attention: 
(1)the max blk size is dma_buf_size/2
*/
s32 pcie_dma_single_blk_rate_task(void* data)
{
    dma_task_info* task_info = (dma_task_info*)data;
    u32 type = task_info->type;
    u32 blk_size = task_info->blk_size;
    
    s32 ret = 0;
    dma_blk_info blk_info;
    u32 chn_id = 0xff;
    s32 ok_times = 0;
    s32 total_delta = 0;

    printk(KERN_ERR "PCIE RC:%s enter.",__func__);
    memset((void*)&blk_info,0,sizeof(dma_blk_info));
    
    ret = pcie_dma_chn_allocate(task_info->type,&chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_allocate fail.");
        return ret;
    }

    isr_info[type].chn_id = chn_id;
    isr_info[type].type   = type;
    
    blk_info.trans_size = blk_size;
    if(dma_pos == PCIE_LOCAL)
    {
        blk_info.sar_low = (type == WR) ? (u32)dma_phys_buf : (dma_remote_phys_buf + dma_buf_size/2);
        blk_info.sar_high = 0;

        blk_info.dar_low = (type == WR) ? (u32)dma_remote_phys_buf : ((u32)dma_phys_buf + dma_buf_size/2);
        blk_info.dar_high = 0;
    }
    else
    {
        blk_info.sar_low = (type == WR) ? ep_dma_buf_sync->local_low : (ep_dma_buf_sync->remote_low + dma_buf_size/2);
        blk_info.sar_high = 0;

        blk_info.dar_low = (type == WR) ? ep_dma_buf_sync->remote_low : (ep_dma_buf_sync->local_low + dma_buf_size/2);
        blk_info.dar_high = 0;        
    }
    
    blk_info.done_isr = dma_done_test_isr;
    blk_info.done_isr_private = (void*)&isr_info[type];
    blk_info.abort_isr = dma_abort_test_isr;
    blk_info.abort_isr_private = (void*)&isr_info[type];
    
    isr_info[type].start_time = jiffies;
    while(!test_done)
    {
       
        ret = pcie_dma_single_blk_config(type, chn_id, &blk_info);
        if(ret)
        {
            printk(KERN_ERR "PCIE RC:pcie_dma_single_blk_config %d-%d,fail.",type,chn_id);
            pcie_dma_chn_free(type,chn_id);
            return ret;
        }
        
        /*isr_info[type].start_time = jiffies;*/
        ret = pcie_dma_chn_start(type,chn_id);
        if(ret)
        {
            printk(KERN_ERR "PCIE RC:pcie_dma_chn_start %d-%d fail.",type,chn_id);
            pcie_dma_chn_free(type,chn_id);
            return ret;
        }
        
        down(&dma_chn_sema[type][chn_id]);
        
        if((isr_info[type].err_lstatus)||(isr_info[type].err_hstatus))
        {
            pcie_dma_chn_stop(type,chn_id);
            continue;    
        }
        else
        {
            ok_times++;
            /*total_delta += isr_info[type].end_time - isr_info[type].start_time;*/            
        }
    }
    isr_info[type].end_time = jiffies;
    total_delta = isr_info[type].end_time - isr_info[type].start_time;
    pcie_dma_chn_free(type,chn_id);

    if(ok_times)
    {
        printk(KERN_ERR "blk_size:0x%x,ok_times:%d.",blk_size,ok_times);
        calc_rate(ok_times,blk_size,0,total_delta);
    }
    
    printk(KERN_ERR "PCIE RC:%s exit.",__func__);

    return 0;    
}

dma_task_info task_info[2];

s32 pcie_dma_single_blk_rate_test(u32 type, u32 blk_size,u32 test_time,char* task_name)
{   
    task_info[type].type = type;
    task_info[type].blk_size = blk_size;

    test_done = 0;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[type],task_name);
    msleep(test_time*1000);
    test_done = 1;
    msleep(500);
    return 0;
}

s32 pcie_dma_blk_rate_test(u32 type, u32 blk_size)
{   
    task_info[type].type = type;
    task_info[type].blk_size = blk_size;

    test_done = 0;
    pcie_dma_single_blk_rate_task((void*)&task_info[type]); 
    return 0;
}

s32 pcie_test_003_001(void)
{
    pcie_dma_single_blk_rate_test(WR,2*1024*1024,10,"wr_task");
    return 0;
}

s32 pcie_test_003_002(void)
{
    pcie_dma_single_blk_rate_test(RD,2*1024*1024,10,"rd_task");
    return 0;    
}

s32 pcie_test_003_003(void)
{
    pcie_dma_single_blk_rate_test(WR,1*1024*1024,10,"wr_task");
    return 0;
}

s32 pcie_test_003_004(void)
{
    pcie_dma_single_blk_rate_test(RD,1*1024*1024,10,"rd_task");
    return 0;    
}
s32 pcie_test_003_005(void)
{
    pcie_dma_single_blk_rate_test(WR,256*1024,10,"wr_task");
    return 0;    
}

s32 pcie_test_003_006(void)
{
    pcie_dma_single_blk_rate_test(RD,256*1024,10,"rd_task");
    return 0;    
}
s32 pcie_test_003_007(void)
{
    pcie_dma_single_blk_rate_test(WR,64*1024,10,"wr_task");
    return 0;    
}

s32 pcie_test_003_008(void)
{
    pcie_dma_single_blk_rate_test(RD,64*1024,10,"rd_task");
    return 0;    
}
s32 pcie_test_003_009(void)
{
    pcie_dma_single_blk_rate_test(WR,8*1024,10,"wr_task");
    return 0;    
}

s32 pcie_test_003_010(void)
{
    pcie_dma_single_blk_rate_test(RD,8*1024,10,"rd_task");
    return 0;    
}

s32 pcie_test_003_011(void)
{
    pcie_dma_single_blk_rate_test(WR,1514,10,"wr_task");
    return 0;    
}

s32 pcie_test_003_012(void)
{
    pcie_dma_single_blk_rate_test(RD,1514,10,"rd_task");
    return 0;    
}
s32 pcie_test_003_013(void)
{
    test_done = 0;
    
    task_info[WR].type = WR;
    task_info[WR].blk_size = 2*1024*1024;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[WR],"wr_task");

    task_info[RD].type = RD;
    task_info[RD].blk_size = 2*1024*1024;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[RD],"rd_task");
    
    msleep(10*1000);
    
    test_done = 1;
    msleep(500);
    return 0;    
}
s32 pcie_test_003_014(void)
{
    test_done = 0;
    
    task_info[WR].type = WR;
    task_info[WR].blk_size = 64*1024;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[WR],"wr_task");

    task_info[RD].type = RD;
    task_info[RD].blk_size = 64*1024;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[RD],"rd_task");
    
    msleep(10*1000);
    
    test_done = 1;
    msleep(500);
    return 0;    
}
s32 pcie_test_003_015(void)
{
    test_done = 0;
    
    task_info[WR].type = WR;
    task_info[WR].blk_size = 8*1024;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[WR],"wr_task");

    task_info[RD].type = RD;
    task_info[RD].blk_size = 8*1024;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[RD],"rd_task");
    
    msleep(10*1000);
    
    test_done = 1;
    msleep(500);
    return 0;    
}
s32 pcie_test_003_016(void)
{
    test_done = 0;
    
    task_info[WR].type = WR;
    task_info[WR].blk_size = 1514;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[WR],"wr_task");

    task_info[RD].type = RD;
    task_info[RD].blk_size = 1514;
    kthread_run(pcie_dma_single_blk_rate_task,(void*)&task_info[RD],"rd_task");
    
    msleep(10*1000);
    
    test_done = 1;
    msleep(500);
    return 0;    
}
/*
ll is one-direction.
the last 12Kbytes of local memory is used as ll info, the first is wr-ll,the second is rd-ll.
so the space support 1024 notes.
the max ll total data size is 1.5Mbytes. 
*/
s32 pcie_dma_ll_test(u32 data_ele_num,dma_data_ele* data_ele,u32 pattern)
{
    dma_ll wr_ll;
    dma_ll rd_ll;
    dma_ll_info wr_ll_info;
    dma_ll_info rd_ll_info;
    s32 i = 0;
    u32 total_size = 0;
    u32 wr_chn_id = 0xff;
    u32 rd_chn_id = 0xff;
    s32 ret = 0;

    if(data_ele_num > 1024)
    {
        printk(KERN_ERR "PCIE RC:data_ele_num can't support above %d.",data_ele_num);
        return -1;
    }

    for(i = 0; i < data_ele_num; i++)
    {
        total_size += data_ele[i].trans_size;
    }

    if(total_size > ll_total_data_max_size)
    {
        printk(KERN_ERR "PCIE RC:total_size can't support above %d.",total_size);
        return -1;        
    }
    
    wr_ll.cycled = 0;
    
    if(dma_pos == PCIE_LOCAL)
    {
        wr_ll.phys_low_adr = ((u32)dma_phys_buf + dma_buf_size) - 12*1024;
        wr_ll.phys_high_adr = 0;
        wr_ll.virt_low_adr = ((u32)dma_virt_buf + dma_buf_size) - 12*1024;
        wr_ll.data_ele_num = data_ele_num;
        wr_ll.data_ele = (dma_data_ele*)wr_ll.virt_low_adr;
        data_ele[0].sar_low    = (u32)dma_phys_buf;
        data_ele[0].sar_high   = 0;
        data_ele[0].dar_low    = (u32)dma_remote_phys_buf;
        data_ele[0].dar_high   = 0;        
    }
    else
    {
        wr_ll.phys_low_adr = (ep_dma_buf_sync->local_low + dma_buf_size) - 12*1024;
        wr_ll.phys_high_adr = 0;
        wr_ll.virt_low_adr = ((u32)dma_remote_virt_buf + dma_buf_size) - 12*1024;        
        wr_ll.data_ele_num = data_ele_num;
        wr_ll.data_ele = (dma_data_ele*)wr_ll.virt_low_adr;
        data_ele[0].sar_low    = ep_dma_buf_sync->local_low;
        data_ele[0].sar_high   = 0;
        data_ele[0].dar_low    = ep_dma_buf_sync->remote_low;
        data_ele[0].dar_high   = 0;  
    }

    
    for(i = 1; i < data_ele_num; i++)
    {
        data_ele[i].sar_low    = data_ele[i-1].sar_low + data_ele[i-1].trans_size;
        data_ele[i].sar_high   = 0;
        data_ele[i].dar_low    = data_ele[i-1].dar_low + data_ele[i-1].trans_size;
        data_ele[i].dar_high   = 0;
    }

    pcie_dma_ll_build(&wr_ll,data_ele,NULL);

    rd_ll.cycled = 0;
    if(dma_pos == PCIE_LOCAL)
    {
        rd_ll.phys_low_adr = ((u32)dma_phys_buf + dma_buf_size) - 6*1024;
        rd_ll.phys_high_adr = 0;
        rd_ll.virt_low_adr = ((u32)dma_virt_buf + dma_buf_size) - 6*1024;
        rd_ll.data_ele_num = data_ele_num;
        rd_ll.data_ele = (dma_data_ele*)wr_ll.virt_low_adr;

        data_ele[0].sar_low    = (u32)dma_remote_phys_buf;
        data_ele[0].sar_high   = 0;
        data_ele[0].dar_low    = (u32)dma_phys_buf + ll_total_data_max_size;
        data_ele[0].dar_high   = 0;
    }
    else
    {
        rd_ll.phys_low_adr = (ep_dma_buf_sync->local_low + dma_buf_size) - 6*1024;
        rd_ll.phys_high_adr = 0;
        rd_ll.virt_low_adr = ((u32)dma_remote_virt_buf + dma_buf_size) - 6*1024;
        rd_ll.data_ele_num = data_ele_num;
        rd_ll.data_ele = (dma_data_ele*)wr_ll.virt_low_adr;

        data_ele[0].sar_low    = ep_dma_buf_sync->remote_low;
        data_ele[0].sar_high   = 0;
        data_ele[0].dar_low    = ep_dma_buf_sync->local_low + ll_total_data_max_size;
        data_ele[0].dar_high   = 0;
        
    }
    
    for(i = 1; i < data_ele_num; i++)
    {
        data_ele[i].sar_low    = data_ele[i-1].sar_low + data_ele[i-1].trans_size;
        data_ele[i].sar_high   = 0;
        data_ele[i].dar_low    = data_ele[i-1].dar_low + data_ele[i-1].trans_size;
        data_ele[i].dar_high   = 0;
    }

    pcie_dma_ll_build(&rd_ll,data_ele,NULL); 

    ret = pcie_dma_chn_allocate(WR,&wr_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_allocate WR fail.");
        return ret;
    }

    ret = pcie_dma_chn_allocate(RD,&rd_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_allocate RD fail.");
        pcie_dma_chn_free(WR,wr_chn_id);
        return ret;
    }

    memset((void*)&isr_info[WR],0,sizeof(dma_test_isr_info));
    isr_info[WR].chn_id = wr_chn_id;
    isr_info[WR].type = WR;
    isr_info[WR].ll_status = no_cycled_ll;
    wr_ll_info.ll_ptr_low = wr_ll.phys_low_adr;
    wr_ll_info.ll_ptr_high = wr_ll.phys_high_adr;
    wr_ll_info.done_isr = dma_done_test_isr;
    wr_ll_info.done_isr_private = &isr_info[WR];
    wr_ll_info.abort_isr = dma_abort_test_isr;
    wr_ll_info.abort_isr_private = &isr_info[WR];

    ret = pcie_dma_ll_config(WR, wr_chn_id, &wr_ll_info);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_ll_config WR-%d fail.",wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    if(dma_pos == PCIE_LOCAL)
    {
        memset((void*)dma_virt_buf,0,total_size);
    }
    else
    {
        memset((void*)dma_remote_virt_buf,0,total_size);
    }
    
    ret = pcie_dma_chn_start(WR,wr_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_start WR-%d fail.",wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    
    down(&dma_chn_sema[WR][wr_chn_id]);
    
    if((isr_info[WR].err_hstatus)||(isr_info[WR].err_hstatus))
    {
        printk(KERN_ERR "PCIE RC:pcie dma write failed.");
        pcie_dma_chn_stop(WR,wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;        
    }

    /*init buf*/
    ret = pcie_dma_ll_config(WR, wr_chn_id, &wr_ll_info);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_ll_config WR-%d fail.",wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    if(dma_pos == PCIE_LOCAL)
    {
        memset((void*)dma_virt_buf,pattern,total_size);
    }
    else
    {
        memset((void*)dma_remote_virt_buf,pattern,total_size);
    }
    ret = pcie_dma_chn_start(WR,wr_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_start WR-%d fail.",wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    
    down(&dma_chn_sema[WR][wr_chn_id]);
    
    if((isr_info[WR].err_hstatus)||(isr_info[WR].err_hstatus))
    {
        printk(KERN_ERR "PCIE RC:pcie dma write failed.");
        pcie_dma_chn_stop(WR,wr_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;        
    } 

    memset((void*)&isr_info[RD],0,sizeof(dma_test_isr_info));
    isr_info[RD].chn_id = rd_chn_id;
    isr_info[RD].type = RD;
    isr_info[RD].ll_status = no_cycled_ll;
    rd_ll_info.ll_ptr_low = rd_ll.phys_low_adr;
    rd_ll_info.ll_ptr_high = rd_ll.phys_high_adr;
    rd_ll_info.done_isr = dma_done_test_isr;
    rd_ll_info.done_isr_private = &isr_info[RD];
    rd_ll_info.abort_isr = dma_abort_test_isr;
    rd_ll_info.abort_isr_private = &isr_info[RD];

    ret = pcie_dma_ll_config(RD, rd_chn_id, &rd_ll_info);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_ll_config RD-%d fail.",rd_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    ret = pcie_dma_chn_start(RD,rd_chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_start RD-%d fail.",rd_chn_id);
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;
    }
    
    down(&dma_chn_sema[RD][wr_chn_id]);
    
    if((isr_info[RD].err_hstatus)||(isr_info[RD].err_hstatus))
    {
        printk(KERN_ERR "PCIE RC:pcie dma read failed.");
        pcie_dma_chn_free(WR,wr_chn_id);
        pcie_dma_chn_free(RD,rd_chn_id);
        return ret;        
    }
    if(dma_pos == PCIE_LOCAL)
    {
        if(memcmp((void*)((u32)dma_virt_buf + ll_total_data_max_size),(void*)dma_virt_buf,total_size))
        {
            printk(KERN_ERR "PCIE RC:memcmp(addr1:0x%x,addr2:0x%x) fail.",(u32)dma_virt_buf,((u32)dma_virt_buf + ll_total_data_max_size));
        }
        else
        {
            printk(KERN_ERR "PCIE RC:memcmp(addr1:0x%x,addr2:0x%x) ok.",(u32)dma_virt_buf,((u32)dma_virt_buf + ll_total_data_max_size));
        }
    }
    else
    {
        if(memcmp((void*)((u32)dma_remote_virt_buf + ll_total_data_max_size),(void*)dma_remote_virt_buf,total_size))
        {
            printk(KERN_ERR "PCIE RC:memcmp(addr1:0x%x,addr2:0x%x) fail.",(u32)dma_remote_virt_buf,((u32)dma_remote_virt_buf + ll_total_data_max_size));
        }
        else
        {
            printk(KERN_ERR "PCIE RC:memcmp(addr1:0x%x,addr2:0x%x) ok.",(u32)dma_remote_virt_buf,((u32)dma_remote_virt_buf + ll_total_data_max_size));
        }        
    }
    pcie_dma_chn_free(WR,wr_chn_id);
    pcie_dma_chn_free(RD,rd_chn_id); 
    
    return 0;    
}

s32 pcie_test_004_001(void)
{
    u32 data_ele_num = 10;
    dma_data_ele data_ele[10];
    u32 pattern = 0x71;

    data_ele[0].trans_size = 256;
    data_ele[1].trans_size = 256;
    data_ele[2].trans_size = 256;
    data_ele[3].trans_size = 256;
    data_ele[4].trans_size = 512;
    data_ele[5].trans_size = 512;
    data_ele[6].trans_size = 512;
    data_ele[7].trans_size = 512;
    data_ele[8].trans_size = 1024;
    data_ele[9].trans_size = 1024;
    
    return pcie_dma_ll_test(data_ele_num,data_ele,pattern);    
}

s32 pcie_dma_ll_rate_task(void* data)
{
    dma_ll ll;
    dma_ll_info ll_info;
    s32 i = 0;
    u32 total_size = 0;
    u32 chn_id = 0xff;
    s32 ret = 0;
    dma_task_info* task_info = (dma_task_info*)data;
    u32 data_ele_num = task_info->data_ele_num;
    dma_data_ele* data_ele = task_info->data_ele;
    u32 type = task_info->type;
    u32 ok_times = 0;
    u32 total_delta = 0;

    printk(KERN_ERR "PCIE RC:%s enter.",__func__);
    if(data_ele_num > 1024)
    {
        printk(KERN_ERR "PCIE RC:data_ele_num can't support above %d.",data_ele_num);
        return -1;
    }

    for(i = 0; i < data_ele_num; i++)
    {
        total_size += data_ele[i].trans_size;
    }

    if(total_size > ll_total_data_max_size)
    {
        printk(KERN_ERR "PCIE RC:total_size can't support above %d.",total_size);
        return -1;        
    }
    
    ll.cycled = 0;
    if(dma_pos == PCIE_LOCAL)
    {
        ll.phys_low_adr = ((u32)dma_phys_buf + dma_buf_size) - (12-6*type)*1024;
        ll.phys_high_adr = 0;
        ll.virt_low_adr = ((u32)dma_virt_buf + dma_buf_size) - (12-6*type)*1024;
        ll.data_ele_num = data_ele_num;
        ll.data_ele = (dma_data_ele*)ll.virt_low_adr;

        data_ele[0].sar_low    = (type == WR) ? (u32)dma_phys_buf : ((u32)dma_remote_phys_buf + dma_buf_size/2);
        data_ele[0].sar_high   = 0;
        data_ele[0].dar_low    = (type == WR) ? (u32)dma_remote_phys_buf : ((u32)dma_phys_buf + dma_buf_size/2);
        data_ele[0].dar_high   = 0;
    }
    else
    {
        ll.phys_low_adr = (ep_dma_buf_sync->local_low + dma_buf_size) - (12-6*type)*1024;
        ll.phys_high_adr = 0;
        ll.virt_low_adr = ((u32)dma_remote_virt_buf + dma_buf_size) - (12-6*type)*1024;
        ll.data_ele_num = data_ele_num;
        ll.data_ele = (dma_data_ele*)ll.virt_low_adr;

        data_ele[0].sar_low    = (type == WR) ? ep_dma_buf_sync->local_low : (ep_dma_buf_sync->remote_low + dma_buf_size/2);
        data_ele[0].sar_high   = 0;
        data_ele[0].dar_low    = (type == WR) ? ep_dma_buf_sync->remote_low: (ep_dma_buf_sync->local_low + dma_buf_size/2);
        data_ele[0].dar_high   = 0;
        
    }
    
    for(i = 1; i < data_ele_num; i++)
    {
        data_ele[i].sar_low    = data_ele[i-1].sar_low + data_ele[i-1].trans_size;
        data_ele[i].sar_high   = 0;
        data_ele[i].dar_low    = data_ele[i-1].dar_low + data_ele[i-1].trans_size;
        data_ele[i].dar_high   = 0;
    }

    pcie_dma_ll_build(&ll,data_ele,NULL);

    ret = pcie_dma_chn_allocate(type,&chn_id);
    if(ret)
    {
        printk(KERN_ERR "PCIE RC:pcie_dma_chn_allocate tpye:%d fail.",type);
        return ret;
    }

    memset((void*)&isr_info[type],0,sizeof(dma_test_isr_info));
    isr_info[type].chn_id = chn_id;
    isr_info[type].type = type;
    isr_info[type].ll_status = no_cycled_ll;
    ll_info.ll_ptr_low = ll.phys_low_adr;
    ll_info.ll_ptr_high = ll.phys_high_adr;
    ll_info.done_isr = dma_done_test_isr;
    ll_info.done_isr_private = &isr_info[type];
    ll_info.abort_isr = dma_abort_test_isr;
    ll_info.abort_isr_private = &isr_info[type];

    isr_info[type].start_time = jiffies;
    while(!test_done)
    {      
        
        ret = pcie_dma_ll_config(type, chn_id, &ll_info);
        if(ret)
        {
            printk(KERN_ERR "PCIE RC:pcie_dma_ll_config type:%d-%d fail.",type,chn_id);
            /*pcie_dma_chn_free(type,chn_id);*/
            /*return ret;*/
            break;
        }
        
        /*isr_info[type].start_time = jiffies;*/
        ret = pcie_dma_chn_start(type,chn_id);
        if(ret)
        {
            printk(KERN_ERR "PCIE RC:pcie_dma_chn_start type:%d-%d fail.",type,chn_id);
            /*pcie_dma_chn_free(type,chn_id);*/
            /*return ret;*/
            break;
        }
        
        down(&dma_chn_sema[type][chn_id]);
        
        if((isr_info[type].err_lstatus)||(isr_info[type].err_hstatus))
        {
            pcie_dma_chn_stop(type,chn_id);
            continue;    
        }
        else
        {
            ok_times++;
            /*total_delta += isr_info[type].end_time - isr_info[type].start_time;*/            
        }
    }
    isr_info[type].end_time = jiffies;
    total_delta = isr_info[type].end_time - isr_info[type].start_time;
    pcie_dma_chn_free(type,chn_id);

    if(ok_times)
    {
        printk(KERN_ERR "ll total_size:0x%x,ok_times:%d.",total_size,ok_times);
        calc_rate(ok_times,total_size,0,total_delta);
    }
    printk(KERN_ERR "PCIE RC:%s exit.",__func__);
    
    return 0;    
}

dma_data_ele data_ele[20];

s32 pcie_ll_debug(u32 type)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 128*1024;
    data_ele[1].trans_size = 128*1024;
    data_ele[2].trans_size = 128*1024;
    data_ele[3].trans_size = 128*1024;
    data_ele[4].trans_size = 128*1024;
    data_ele[5].trans_size = 128*1024;
    data_ele[6].trans_size = 128*1024;
    data_ele[7].trans_size = 128*1024;

    task_info[type].type = type;
    task_info[type].data_ele = data_ele;
    task_info[type].data_ele_num = data_ele_num;  

    test_done = 0;
    pcie_dma_ll_rate_task((void*)&task_info[type]);

    return 0;
}
s32 pcie_test_005_001(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 128*1024;
    data_ele[1].trans_size = 128*1024;
    data_ele[2].trans_size = 128*1024;
    data_ele[3].trans_size = 128*1024;
    data_ele[4].trans_size = 128*1024;
    data_ele[5].trans_size = 128*1024;
    data_ele[6].trans_size = 128*1024;
    data_ele[7].trans_size = 128*1024;

    task_info[WR].type = WR;
    task_info[WR].data_ele = data_ele;
    task_info[WR].data_ele_num = data_ele_num;

    test_done = 0;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[WR],"wr_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;
    
}

s32 pcie_test_005_002(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 128*1024;
    data_ele[1].trans_size = 128*1024;
    data_ele[2].trans_size = 128*1024;
    data_ele[3].trans_size = 128*1024;
    data_ele[4].trans_size = 128*1024;
    data_ele[5].trans_size = 128*1024;
    data_ele[6].trans_size = 128*1024;
    data_ele[7].trans_size = 128*1024;

    task_info[RD].type = RD;
    task_info[RD].data_ele = data_ele;
    task_info[RD].data_ele_num = data_ele_num;

    test_done = 0;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[RD],"rd_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;    
}

s32 pcie_test_005_003(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 8*1024;
    data_ele[1].trans_size = 8*1024;
    data_ele[2].trans_size = 8*1024;
    data_ele[3].trans_size = 8*1024;
    data_ele[4].trans_size = 8*1024;
    data_ele[5].trans_size = 8*1024;
    data_ele[6].trans_size = 8*1024;
    data_ele[7].trans_size = 8*1024;

    task_info[WR].type = WR;
    task_info[WR].data_ele = data_ele;
    task_info[WR].data_ele_num = data_ele_num;

    test_done = 0;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[WR],"wr_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;
    
}

s32 pcie_test_005_004(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 8*1024;
    data_ele[1].trans_size = 8*1024;
    data_ele[2].trans_size = 8*1024;
    data_ele[3].trans_size = 8*1024;
    data_ele[4].trans_size = 8*1024;
    data_ele[5].trans_size = 8*1024;
    data_ele[6].trans_size = 8*1024;
    data_ele[7].trans_size = 8*1024;

    task_info[RD].type = RD;
    task_info[RD].data_ele = data_ele;
    task_info[RD].data_ele_num = data_ele_num;

    test_done = 0;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[RD],"rd_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;    
}
s32 pcie_test_005_005(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 1024;
    data_ele[1].trans_size = 1024;
    data_ele[2].trans_size = 1024;
    data_ele[3].trans_size = 1024;
    data_ele[4].trans_size = 1024;
    data_ele[5].trans_size = 1024;
    data_ele[6].trans_size = 1024;
    data_ele[7].trans_size = 1024;

    task_info[WR].type = WR;
    task_info[WR].data_ele = data_ele;
    task_info[WR].data_ele_num = data_ele_num;

    test_done = 0;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[WR],"wr_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;
    
}

s32 pcie_test_005_006(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 1024;
    data_ele[1].trans_size = 1024;
    data_ele[2].trans_size = 1024;
    data_ele[3].trans_size = 1024;
    data_ele[4].trans_size = 1024;
    data_ele[5].trans_size = 1024;
    data_ele[6].trans_size = 1024;
    data_ele[7].trans_size = 1024;

    task_info[RD].type = RD;
    task_info[RD].data_ele = data_ele;
    task_info[RD].data_ele_num = data_ele_num;

    test_done = 0;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[RD],"rd_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;    
}
s32 pcie_test_005_007(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 1514;
    data_ele[1].trans_size = 1514;
    data_ele[2].trans_size = 1514;
    data_ele[3].trans_size = 1514;
    data_ele[4].trans_size = 1514;
    data_ele[5].trans_size = 1514;
    data_ele[6].trans_size = 1514;
    data_ele[7].trans_size = 1514;

    task_info[WR].type = WR;
    task_info[WR].data_ele = data_ele;
    task_info[WR].data_ele_num = data_ele_num;

    test_done = 0;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[WR],"wr_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;
    
}

s32 pcie_test_005_008(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 1514;
    data_ele[1].trans_size = 1514;
    data_ele[2].trans_size = 1514;
    data_ele[3].trans_size = 1514;
    data_ele[4].trans_size = 1514;
    data_ele[5].trans_size = 1514;
    data_ele[6].trans_size = 1514;
    data_ele[7].trans_size = 1514;

    task_info[RD].type = RD;
    task_info[RD].data_ele = data_ele;
    task_info[RD].data_ele_num = data_ele_num;

    test_done = 0;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[RD],"rd_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;    
}
s32 pcie_test_005_009(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 8*1024;
    data_ele[1].trans_size = 8*1024;
    data_ele[2].trans_size = 8*1024;
    data_ele[3].trans_size = 8*1024;
    data_ele[4].trans_size = 8*1024;
    data_ele[5].trans_size = 8*1024;
    data_ele[6].trans_size = 8*1024;
    data_ele[7].trans_size = 8*1024;

    task_info[WR].data_ele = data_ele;
    task_info[WR].data_ele_num = data_ele_num;
    task_info[RD].data_ele = data_ele;
    task_info[RD].data_ele_num = data_ele_num;
    
    test_done = 0;
    task_info[WR].type = WR;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[WR],"wr_task");    
    task_info[RD].type = RD;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[RD],"rd_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;      
}

s32 pcie_test_005_010(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 1024;
    data_ele[1].trans_size = 1024;
    data_ele[2].trans_size = 1024;
    data_ele[3].trans_size = 1024;
    data_ele[4].trans_size = 1024;
    data_ele[5].trans_size = 1024;
    data_ele[6].trans_size = 1024;
    data_ele[7].trans_size = 1024;

    task_info[WR].data_ele = data_ele;
    task_info[WR].data_ele_num = data_ele_num;
    task_info[RD].data_ele = data_ele;
    task_info[RD].data_ele_num = data_ele_num;
    
    test_done = 0;
    task_info[WR].type = WR;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[WR],"wr_task");    
    task_info[RD].type = RD;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[RD],"rd_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;      
}
s32 pcie_test_005_011(void)
{
    u32 data_ele_num = 8;

    data_ele[0].trans_size = 1514;
    data_ele[1].trans_size = 1514;
    data_ele[2].trans_size = 1514;
    data_ele[3].trans_size = 1514;
    data_ele[4].trans_size = 1514;
    data_ele[5].trans_size = 1514;
    data_ele[6].trans_size = 1514;
    data_ele[7].trans_size = 1514;

    task_info[WR].data_ele = data_ele;
    task_info[WR].data_ele_num = data_ele_num;
    task_info[RD].data_ele = data_ele;
    task_info[RD].data_ele_num = data_ele_num;
    
    test_done = 0;
    task_info[WR].type = WR;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[WR],"wr_task");    
    task_info[RD].type = RD;
    kthread_run(pcie_dma_ll_rate_task,(void*)&task_info[RD],"rd_task");
    msleep(10*1000);
    test_done = 1;
    msleep(500);
    return 0;      
}
#if defined(CONFIG_PCI_MSI) || defined(MSI_SUPPORT_1_DEV_32_BIT)
u32 msi_dev_id[33] = {0};
#endif

static u32 wr_done_int = 0;
static u32 wr_error_int = 0;
static u32 rd_done_int = 0;
static u32 rd_error_int = 0;

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

#if defined(CONFIG_PCI_MSI)
static irqreturn_t pcie_msi_isr(s32 irq, void* private)
{   
    u32 val = 0;
    u32 dev_id = *(u32*)private; 
    u32 ep = dev_id>>16;
    u32 irq_bit = dev_id&0xffff;
    
    if(irq_bit > 31)
    {
        return IRQ_NONE;
    }
    
    pci_read_config_dword(balong_rc, MSIC_IRQ_STATUS(ep),&val);
    if((val>>irq_bit)&0x1)
    {
#if !defined(EP_DMA_RISE_LOCAL_INT)
        printk(KERN_ERR "handle irq:%d-ep:%d,irq_bit:%d.\n",irq,ep,irq_bit);
#else
        pci_write_config_dword(balong_rc, MSIC_IRQ_STATUS(ep),(1<<irq_bit)); 
        switch(irq_bit)
        {
        case 0:
            wr_done_int++;
            up(&(dma_chn_sema[WR][0]));
            break;
        case 1:
            printk(KERN_ERR "write error.\n");
            wr_error_int++;
            up(&(dma_chn_sema[WR][0]));
            break;
        case 2:
            rd_done_int++;
            up(&(dma_chn_sema[RD][0]));
            break;
        case 3:
            printk(KERN_ERR "read error.\n");
            rd_error_int++;
            up(&(dma_chn_sema[RD][0]));
            break;
        default:
            break;
        }
#endif                
    }
    else
    {
        return IRQ_NONE;
    }

    return IRQ_HANDLED;
}
#else
static irqreturn_t pcie_intx_isr(s32 irq, void* private)
{
    u32 intx_status = *(u32*)private;

    if(!intx_status)
    {
        return IRQ_NONE;
    }
    
    *intx_event_sync = 0;
    printk(KERN_ERR "handle irq:%d.\n",irq); 
    
    return IRQ_HANDLED;    
}
#endif

s32 pcie_rc_test_init(void)
{   
    balong_rc = pcie_find_rc(BALONG_PCI_VENDOR_ID,BALONG_PCI_DEVICE_ID);
    if(NULL == balong_rc)
    {
        printk(KERN_ERR "PCIE RC:can't find balong pcie rc.\n");
        return -EIO;
    }
        
    balong_ep = pcie_find_dev(BALONG_PCI_VENDOR_ID,BALONG_PCI_DEVICE_ID);
    if(NULL == balong_ep)
    {
        printk(KERN_ERR "PCIE RC:can't find balong pcie ep.\n");
        return -EIO;
    } 

    if(pci_enable_device(balong_ep) != 0)
    {
        printk(KERN_ERR "PCIE RC: pci_enable_device fail.\n");
        return -EIO;
    }
    
#if defined(CONFIG_PCI_MSI)
    /*set ep outbound iatu-idx 0 for msi*/
    pci_write_config_dword(balong_ep,IATU_IDX,0);
    pci_write_config_dword(balong_ep,IATU_CTRL1,0);
    pci_write_config_dword(balong_ep,IATU_LOWER_ADDR,REG_BASE_PCIE_DATA + 0x1000000);
    pci_write_config_dword(balong_ep,IATU_UPPER_ADDR,0);
    pci_write_config_dword(balong_ep,IATU_LIMIT_ADDR,REG_BASE_PCIE_DATA + 0x1000fff);
    pci_write_config_dword(balong_ep,IATU_LTAR_ADDR,MSI_PCIE_LOWER_ADDR);
    pci_write_config_dword(balong_ep,IATU_UTAR_ADDR,MSI_PCIE_UPPER_ADDR);
    pci_write_config_dword(balong_ep,IATU_CTRL2,0x80000000);
    
    if(pci_enable_msi(balong_ep))
    {
        printk(KERN_ERR "PCIE_RC: pci_enable_msi fail.\n");
        return -EIO;
    }

#if defined(MSI_SUPPORT_1_DEV_32_BIT)
    {
        u32 val = 0;
        for(val = 0; val < 4; val++)
        {      
            msi_dev_id[val] = val;        
            if(request_irq(balong_ep->irq, pcie_msi_isr,IRQF_SHARED, "msi test", &msi_dev_id[val]))
            {
                printk(KERN_ERR "pcie msi irq request failed!\n");
                return -1;
            }
        }
    }
#endif   
#endif
    
    if(rc_buf_init() == -EIO)
    {
        return -EIO;
    }

#if !defined(CONFIG_PCI_MSI)
    if(request_irq(balong_ep->irq, pcie_intx_isr,IRQF_SHARED, 
                  "pcie intx", (void*)intx_event_sync))
    {
        printk(KERN_ERR "pcie intx irq request failed!\n");
        return -1;
    }    
#endif

    pci_set_master(balong_ep);
    
    ep_task_start_sync->work_start = 1;

    printk(KERN_ERR "PCIE_RC: wait ep init ok...");
    while(ep_task_start_sync->work_done != 1)
    {
        mdelay(10);
    }
    printk(KERN_ERR "PCIE_RC: ep init ok.");

    if(pcie_dma_test_init() == -EIO)
    {
        return -EIO;
    }

    if(pcie_dma_init() != 0)
    {
        return -EIO;
    } 
    
    return 0;
}

void pcie_rc_test_exit(void)
{
    pcie_dma_deinit();
    pcie_dma_test_deinit();    
#if defined(CONFIG_PCI_MSI)
#if defined(MSI_SUPPORT_1_DEV_32_BIT)
    {
        s32 i = 0;
        for(i = 0; i < 4; i++)
        {     
            free_irq(balong_ep->irq,&msi_dev_id[i]);
        }
    }
#endif
    pci_disable_msi(balong_ep);
#else
    free_irq(balong_ep->irq,(void*)intx_event_sync);  
#endif
    pci_clear_master(balong_ep);
    rc_buf_deinit();
    pci_disable_device(balong_ep);
}

module_init(pcie_rc_test_init);
module_exit(pcie_rc_test_exit);


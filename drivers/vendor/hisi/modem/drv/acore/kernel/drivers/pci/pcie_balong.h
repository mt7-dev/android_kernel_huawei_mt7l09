#ifndef __PCIE_BALONG_H__
#define __PCIE_BALONG_H__

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/printk.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/regulator/consumer.h>

#include <asm-generic/sizes.h>

#include <hi_pcie_interface.h>

#ifdef __cplusplus
extern "C" {
#endif


/********************************* Definition **********************************/

/******* work mode **********/
/* ep mode */
#define PCIE_WORK_MODE_EP   (0x0)
/* legacy ep mode */
#define PCIE_WORK_MODE_LEP  (0x1)
/* rc mode */
#define PCIE_WORK_MODE_RC   (0x4)

/******* link mode **********/
#define PCIE_LINK_MODE_X1   (0x1)
#define PCIE_LINK_MODE_X2   (0x3)


#define pcie_trace(format, args...) do {printk("[PCIE]:line: %d: "format, __LINE__, ##args);} while(0)

#define PCIE_CFG_BUS(busnr) (busnr << 20)
#define PCIE_CFG_DEV(devfn) (devfn << 15)
#define PCIE_CFG_FUNC(func) (func  << 12)
/* set dword align */
#define PCIE_CFG_REG(reg)   (reg & 0xFFF)


#define PCIE_TLP_DIRECTION_OUTBOUND (0x0)
#define PCIE_TLP_DIRECTION_INBOUND  (0x1)

#define PCIE_TLP_TYPE_MEM_RW        (0x0)
#define PCIE_TLP_TYPE_IO_RW         (0x2)
#define PCIE_TLP_TYPE_CFG0_RW       (0x4)
#define PCIE_TLP_TYPE_CFG1_RW       (0x5)

#define PCIE_IATU_MATCH_MODE_ADDR   (0x0)
#define PCIE_IATU_MATCH_MODE_BAR    (0x1)


struct balong_pcie_iatu_table {
    /* +0x200 iATU Index Register */
    union
    {
        struct
        {
            u32 index       :31;   /* Defines which region is being accessed */
            u32 direction   :1 ;   /* 0: Outbound; 1: Inbound */
        } attr;
        u32 value;
    } index;

    /* +0x204 iATU Region Control 1 Register */
    union
    {
        struct
        {
            u32 type   :32;/* bit[4:0]   ----  4b'0000: Memory read/write
                            *                  4b'0010: IO read/write
                            *                  4b'0100: Config Type 0 read/write
                            *                  4b'0101: Config Type 1 read/write
                            */
            /* u32 reserved:27; */
        } attr;
        u32 value;
    } control1;

    /* +0x208 iATU Region Control 2 Register */
    union
    {
        struct
        {
            u32 message_code:8;
            u32 bar_index   :3;
            u32 reserved    :17;
            u32 cfg_shift   :1 ;    /* CFG Shift Mode */
            u32 invert_mode :1 ;
            u32 match_mode  :1 ;
            u32 enable      :1 ;    /* Region Enable */
        } attr;
        u32 value;
    } control2;

    /* +0x20C iATU Region Lower Base Address Register */
    u32 lower_addr;
    /* +0x210 iATU Region Upper Base Address Register */
    u32 upper_addr;
    /* +0x214 iATU Region Limit Address Register */
    u32 limit_addr;
    /* +0x218 iATU Region Lower Target Address Register */
    u32 lower_target_addr;
    /* +0x21C iATU Region Upper Target Address Register */
    u32 upper_target_addr;
    /* +0x220 iATU Region Control 3 Register */
    u32 control3;
};

typedef void (*pcie_callback)(u32, u32, void*);

struct pcie_irq {
    u32 link_down_int;
    u32 edma_int;
    u32 pm_int;
    u32 radm_int_a;
    u32 radm_int_b;
    u32 radm_int_c;
    u32 radm_int_d;

    u32 msi_irq;
    u32 msi_irq_base;

    u32 dma_channel_state; /* 1 for busy, each bit corresponds to a DMA channel */
    struct semaphore dma_semaphore[BALONG_PCIE0_MAX_DMA_CHANNEL_NUM];
    pcie_callback dma_int_callback[BALONG_PCIE0_MAX_DMA_CHANNEL_NUM];
    void *dma_int_callback_args[BALONG_PCIE0_MAX_DMA_CHANNEL_NUM];
};

struct balong_pcie {
    struct hw_pci hw_pci;
    struct pcie_irq irqs;
    struct resource res_io;
    struct resource res_mem;
    struct balong_pcie_iatu_table *iatu_table;
    u32 iatu_table_entry_num;

    u32 enabled;
    u32 work_mode;
    u32 link_mode;
    u32 phys_rc_cfg_addr;
    u32 phys_rc_cfg_size;
    u32 phys_device_config_addr;
    u32 phys_device_config_size;

    u32 reset_gpio;

    u32 linked;
    u32 root_bus_number;
    u32 virt_rc_cfg_addr;
    u32 virt_device_config_addr;
    char *regulator_id;
    struct regulator *regulator;
    char *clock_core_id;
    struct clk *clock_core;
    char *clock_aux_id;
    struct clk *clock_aux;
    char *clock_phy_id;
    struct clk *clock_phy;

    spinlock_t spinlock;    /* for controller */
    spinlock_t spinlock_req_clk;
};


#define PCIE_DMA_DIRECTION_READ     (0)
#define PCIE_DMA_DIRECTION_WRITE    (1)

struct pcie_dma_transfer_info{
    struct pci_dev* dev;
    u32 channel;
    u32 direction;  /* 0 for read, 1 for write */
    pcie_callback callback; /* NULL for sync transfer */
    void *callback_args;
    u32 sar_low;
    u32 sar_high;
    u32 dar_low;
    u32 dar_high;
    u32 transfer_size;
};

int pcie_dma_transfer(struct pcie_dma_transfer_info *transfer_info);

int balong_pcie_read_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 *value);
int balong_pcie_write_config(struct pci_bus *bus, unsigned int devfn, int where, int size, u32 value);
int balong_pcie_setup(int nr, struct pci_sys_data *sys_data);
int balong_pcie_map_irq(const struct pci_dev *dev, u8 slot, u8 pin);
struct pci_bus* balong_pcie_scan(int nr, struct pci_sys_data *sys_data);


extern u32 balong_pcie_num;
extern struct balong_pcie balong_pcie[];
void dbi_enable(u32 id);
void dbi_disable(u32 id);
void pcie_ltssm_enable(u32 id);
void pcie_ltssm_disable(u32 id);
void pcie_link_up_check(u32 id);
void pcie_wait_for_linkup(u32 id);
void pcie_try_change_speed(u32 id);
void pcie_app_req_clk(u32 id, u32 request);
void pcie_set_iatu(u32 id, struct balong_pcie_iatu_table *iatu_table, u32 iatu_table_entry_num);


void pcie_power_on(u32 id);
void pcie_power_off(u32 id);
void pcie_clk_enable(u32 id);
void pcie_clk_disable(u32 id);
void pcie_reset(u32 id);
void pcie_unreset(u32 id);
void pcie_phy_init(u32 id);
void pcie_hardware_init(u32 id);


#ifdef __cplusplus
}
#endif


#endif  /* #ifndef __PCIE_BALONG_H__ */


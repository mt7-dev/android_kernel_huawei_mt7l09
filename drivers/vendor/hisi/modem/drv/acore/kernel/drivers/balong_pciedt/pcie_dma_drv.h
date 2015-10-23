#ifndef _PCIE_DMA_DRV_H_
#define _PCIE_DMA_DRV_H_

#include "pcie_dma_hal.h"

/*#define MAX_CHN_NUM (8)*/
/*only 1 write chn and 1 read chn*/
#define MAX_CHN_NUM   (1)

#define EP_DMA_RISE_LOCAL_INT

typedef void (*dma_done_isr)(void* private);

typedef void (*dma_abort_isr)(u32 err_status_l,u32 err_status_h,void* private);

typedef enum _dma_chn_status {
    not_allocated = 0, /*drv status*/
    running = 1,       /*chip status*/
    halted = 2,        /*chip status*/
    stopped = 3,       /*chip status*/
    allocated,         /*drv status*/
}dma_chn_status;

typedef struct _dma_chn_info {
    u32 id;
    u32 type;
    u32 status;
    dma_chn_ctx ctx;
    dma_done_isr done_isr;
    void* done_isr_private;
    u32 done_isr_count;
    dma_abort_isr abort_isr;
    void* abort_isr_private;
    u32 abort_isr_count;
}dma_chn_info;

typedef struct _dma_blk_info {
    u32 trans_size;
    u32 sar_low;
    u32 sar_high;
    u32 dar_low;
    u32 dar_high;
    dma_done_isr done_isr;
    void* done_isr_private; 
    dma_abort_isr abort_isr;
    void* abort_isr_private;    
}dma_blk_info;

typedef struct _data_blk_info {
    u32 trans_size;
    u32 sar_low;
    u32 sar_high;
    u32 dar_low;
    u32 dar_high;
    dma_done_isr done_isr;
    void* done_isr_private; 
    dma_abort_isr abort_isr;
    void* abort_isr_private;    
}data_blk_info;

typedef struct _dma_ll_info {
    u32 ll_ptr_low;
    u32 ll_ptr_high;
    dma_done_isr done_isr;
    void* done_isr_private; 
    dma_abort_isr abort_isr;
    void* abort_isr_private;    
}dma_ll_info;

typedef struct _dma_ll {
    u32 cycled;
    u32 virt_low_adr;
    u32 virt_high_adr;
    u32 phys_low_adr;
    u32 phys_high_adr;
    u32 data_ele_num;
    dma_data_ele* data_ele;
    dma_ll_ele* ll_ele;
}dma_ll;

s32 pcie_dma_chn_allocate(u32 type, u32* chn_id);
s32 pcie_dma_chn_status_get(u32 position,u32 wr_or_rd,u32 chn_id,u32* status);
s32 pcie_dma_chn_free(u32 type, u32 chn_id);
s32 pcie_dma_single_blk_config(u32 type, u32 chn_id, dma_blk_info* single_blk);
s32 pcie_dma_ll_config(u32 type, u32 chn_id, dma_ll_info* ll_info);
s32 pcie_dma_ll_build(dma_ll* cur_ll,dma_data_ele* data_ele,dma_ll* next_ll);
s32 pcie_dma_chn_start(u32 type,u32 chn_id);
s32 pcie_dma_chn_stop(u32 type,u32 chn_id);
s32 pcie_dma_init(void);
void pcie_dma_deinit(void);
#endif

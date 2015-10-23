#ifndef _PCIE_DMA_HAL_H_
#define _PCIE_DMA_HAL_H_

typedef struct _dma_chn_ctx{
    u32 ctrl_reg1;
    u32 ctrl_reg2;
    u32 trans_size;
    u32 sar_low;
    u32 sar_high;
    u32 dar_low;
    u32 dar_high;
    u32 ll_ptr_low;
    u32 ll_ptr_high;    
}dma_chn_ctx;

typedef struct _dma_data_ele{
    u32 ctrl_reg1;
    u32 trans_size;
    u32 sar_low;
    u32 sar_high;
    u32 dar_low;
    u32 dar_high;       
}dma_data_ele;

typedef struct _dma_ll_ele{
    u32 ctrl_reg1;
    u32 placeholder0;
    u32 ll_ptr_low;
    u32 ll_ptr_high; 
    u32 placeholder1;
    u32 placeholder3;
}dma_ll_ele;

#define PCIE_LOCAL (0)
#define PCIE_REMOTE (1)

#define WR    (0)
#define RD    (1)

#define CHN_RUNNING (1)
#define CHN_HALTED  (2)
#define CHN_STOPPED (3)

s32 pcie_dma_wr_engine_enable(u32 position);
s32 pcie_dma_wr_engine_disable(u32 position);
s32 pcie_dma_wr_chn_start(u32 position,u32 chnid);
s32 pcie_dma_wr_chn_stop(u32 position,u32 chnid);
s32 pcie_dma_wr_engine_arb_wet_set(u32 position,u32 chnid,s32 wet);
s32 pcie_dma_rd_engine_enable(u32 position);
s32 pcie_dma_rd_engine_disable(u32 position);
s32 pcie_dma_rd_chn_start(u32 position,u32 chnid);
s32 pcie_dma_rd_chn_stop(u32 position,u32 chnid);
s32 pcie_dma_rd_engine_arb_wet_set(u32 position,u32 chnid,s32 wet);
s32 pcie_dma_wr_int_status_get(u32 position,u32* status);
s32 pcie_dma_wr_int_mask_set(u32 position,u32 mask);
s32 pcie_dma_wr_int_clear(u32 position,u32 clear);
s32 pcie_dma_wr_done_imwr_adr_set(u32 position,u32 low_adr, u32 high_adr);
s32 pcie_dma_wr_abort_imwr_adr_set(u32 position,u32 low_adr, u32 high_adr);
s32 pcie_dma_wr_chn_imwr_data_set(u32 position,u32 chnid,u16 data);
s32 pcie_dma_ll_err_enable(u32 position, u32 wr_or_rd, u32 chnid);
s32 pcie_dma_ll_err_disable(u32 position, u32 wr_or_rd, u32 chnid);
s32 pcie_dma_rd_int_status_get(u32 position,u32* status);
s32 pcie_dma_rd_int_mask_set(u32 position,u32 mask);
s32 pcie_dma_rd_int_clear(u32 position,u32 clear);
s32 pcie_dma_rd_done_imwr_adr_set(u32 position,u32 low_adr, u32 high_adr);
s32 pcie_dma_rd_abort_imwr_adr_set(u32 position,u32 low_adr, u32 high_adr);
s32 pcie_dma_rd_chn_imwr_data_set(u32 position,u32 chnid,u16 data);
s32 pcie_dma_chn_ctx_idx_set(u32 position,u32 wr_or_rd,u32 idx);
s32 pcie_dma_chn_ctx_idx_get(u32 position,u32* idx);
s32 pcie_dma_chn_ctx_set(u32 position,dma_chn_ctx* chn_ctx);
s32 pcie_dma_chn_ctx_get(u32 position,dma_chn_ctx* chn_ctx);
#endif

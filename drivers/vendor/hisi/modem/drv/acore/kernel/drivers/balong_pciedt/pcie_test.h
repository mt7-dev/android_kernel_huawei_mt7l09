#ifndef _PCIE_TEST_H_
#define _PCIE_TEST_H_

/*for test sync*/
typedef struct _tx_rx_sync{
    u32   work_start;
    u32   trans_len;
    u32   trans_pattern;
    u32   trans_times;
    u32   addr_offset;
    u32   work_done;
    u32   readback_check;
    u32   readback_check_pass;
    u32   start_time;
    u32   end_time; 
}tx_rx_sync;

typedef struct _dma_buf_sync{
    u32  local_low;
    u32  local_high;
    u32  remote_low;
    u32  remote_high;
    u32  size;
}dma_buf_sync;

#endif


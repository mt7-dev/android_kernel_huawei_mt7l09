/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
*
* Filename:    nandc_v600.h
* Description: nandc_v600的寄存器的相关配置宏
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* 
*
******************************************************************************/
#ifndef _NANDC_v600_H_
#define _NANDC_v600_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define NANDC6_NAME                             "NANDC_V6.00"   
#define NANDC6_BUFSIZE_BASIC                    (8192)
#define NANDC6_BUFSIZE_EXTERN                   (1024)
#define NANDC6_BUFSIZE_TOTAL                    (NANDC6_BUFSIZE_BASIC + NANDC6_BUFSIZE_EXTERN)
#define NANDC6_REG_SIZE                         (0x00D8 + 0x04)
#define NANDC6_IRQ                              (106)

#define NANDC6_REG_BASE_ADDR                    (NANDC_REG_BASE_ADDR)
#define NANDC6_BUFFER_BASE_ADDR                 (NANDC_BUFFER_BASE_ADDR)

#define NANDC6_MAX_CHIPS                        NANDC_MAX_CHIPS




/*nandc_v600的寄存器的相关配置*/
#define nandc6_ecc_none                      (0x0)
#define nandc6_ecc_1bit                      (0x2)
#define nandc6_ecc_4smb                      (0x4)
#define nandc6_ecc_8bit                      (0x6)
#define nandc6_ecc_24p1kbit                  (0x8)
#define nandc6_ecc_40bit                     (0xa)
#define nandc6_ecc_64bit                     (0xc)

#define nandc6_async_nand                    (0x0)
#define nandc6_toggle                        (0x1)
#define nandc6_onfi_v2                       (0x2)
#define nandc6_onfi_v3                       (0x3)

#define nandc6_open_randomizer               (0x0)
#define nandc6_close_randomizer              (0x1)

#define nandc6_cs0_int                       (0x0)
#define nandc6_cs0_no_int                    (0x1)

#define nandc6_rw_no_int 					 (0x0)
#define nandc6_rw_int                        (0x1)

#define nandc6_wr_lock_no_int                (0x0)
#define nandc6_wr_lock_int                   (0x1)

#define nandc6_dma_no_int                    (0x0)
#define nandc6_dma_int                       (0x1)

#define nandc6_dma_err_no_int                (0x0)
#define nandc6_dma_err_int                   (0x1)

#define nandc6_dma_over                      (0x0)
#define nandc6_dma_start                     (0x1)

#define nandc6_dma_rd_enable 				 (0x0)
#define nandc6_dma_wr_enable 				 (0x1)

#define nandc6_burst4_enable 				 (0x1)
#define nandc6_burst8_enable 				 (0x2)
#define nandc6_burst16_enable				 (0x4)

#define nandc6_dmaaddr5						 (0x0)
#define nandc6_dmaaddr4						 (0x1)

#define nandc6_dmacs0                        (0x0)
#define nandc6_dmacs1                        (0x1)
#define nandc6_dmacs2                        (0x2)
#define nandc6_dmacs3                        (0x3)

#define nandc6_wr_all                        (0x0)
#define nandc_wr_onlydata                    (0x1)

#define nandc6_rd_page 						 (0x0)
#define nandc_rd_oob_only                    (0x1)

#define nandc6_rb_ready                      (0x0)
#define nandc6_enhanced_clear                (0x1)

#define nandc6_oob_len_8or16                 (0x0)
#define nandc6_oob_len_32                    (0x1)

#define nandc6_segment_first                 (0x0)
#define nandc6_segment_last                  (0x1)


#define nandc6_ecc_err_none                  (0x0)
#define nandc6_ecc_err_valid                 (0x1)
#define nandc6_ecc_err_invalid               (0x2)


#define nandc6_ecc_data                      (0x05)
#define nandc6_ecc_oob                       (0x0a)
#define nandc6_ecc_all                       (0x0f)
#define nandc6_noecc_all                     (0x00)



#define nandc6_op_read_id                    (0x66)   /*0110 0110   cmd1_en + addr_en + wait_ready_en  + read_data_en*/
#define nandc6_op_read_start                 (0x6e)
#define nandc6_op_read_continue              (0x02)
#define nandc6_op_read_end                   (0x02)
#define nandc6_op_wirte_start                (0x70)
#define nandc6_op_wirte_continue             (0x10)
#define nandc6_op_wirte_end                  (0x1d)
#define nandc6_op_erase                      (0x6d)
#define nandc6_op_reset                      (0x44)   /*0100 0100   cmd1_en + wait_ready_en*/


#define nandc6_dma_read                      (0x38)   /* (0x70)   */
#define nandc6_dma_read_oob_only             (0x838)  /* (0x1070) */
#define nandc6_dma_write                     (0x39)   /* (0x72)   */

#define NANDC6_INT_OP_DONE_EN                   (1)
#define NANDC6_INT_OP_DONE_DIS                  (0)
#define NANDC6_INT_CS_DONE_EN                   (0xF)
#define NANDC6_INT_CS_DONE_DIS                  (0)
#define NANDC6_INT_ERR_INVALID_EN               (1)
#define NANDC6_INT_ERR_INVALID_DIS              (0)
#define NANDC6_INT_ERR_VALID_EN                 (1)
#define NANDC6_INT_ERR_VALID_DIS                (0)
#define NANDC6_INT_AHB_OP_EN                    (1)
#define NANDC6_INT_AHB_OP_DIS                   (0)
#define NANDC6_INT_WR_LOCK_EN                   (1)
#define NANDC6_INT_WR_LOCK_DIS                  (0)
#define NANDC6_INT_DMA_DONE_EN                  (1)
#define NANDC6_INT_DMA_DONE_DIS                 (0)
#define NANDC6_INT_DMA_ERR_EN                   (1)
#define NANDC6_INT_DMA_ERR_DIS                  (0)

#define nandc6_int_enable_all                (0x7ff)
#define nandc6_int_disable_all               (0x000)
#define nandc6_int_clear_all                 (0x7ff)



#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_v600_H_*/




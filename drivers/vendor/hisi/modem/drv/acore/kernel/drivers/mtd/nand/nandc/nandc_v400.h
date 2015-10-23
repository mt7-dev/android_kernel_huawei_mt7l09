
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_V400_H_
#define _NANDC_V400_H_
    
#ifdef __cplusplus
extern "C"
{
#endif


#define NANDC4_NAME                             "NANDC_V4.00"   /* TODO:   rename to "hinandv4" */
#define NANDC4_BUFSIZE_BASIC                    (2048)
#define NANDC4_BUFSIZE_EXTERN                   (320)
#define NANDC4_BUFSIZE_TOTAL                    (NANDC4_BUFSIZE_BASIC + NANDC4_BUFSIZE_EXTERN)
#define NANDC4_REG_SIZE                         (0x00B0)

#define NANDC4_REG_BASE_ADDR                    (NANDC_REG_BASE_ADDR)
#define NANDC4_BUFFER_BASE_ADDR                 (NANDC_BUFFER_BASE_ADDR)

#define NANDC4_MAX_CHIPS                        NANDC_MAX_CHIPS
/***********NANDC4_REG_CONF_OFFSET*****************************************
-------------------------------------------------------------------------
*  bit 11    RW    edo_en       ->  0: normal mode
                                    1: EDO mode
--------------------------------------------------------------------------                                               
*  bit10:8  RW    ecc_type      ->  000: no ecc
                                    001: 1bit ecc
                                    010: 4bit ecc
                                    011: reserved
                                    100: 24bit for 1KB
---------------------------------------------------------------------------                                               
*  bit 7      RW   rb_sel       ->  0: use share ready/busy signal
                                    1: use independent ready/busy signal
---------------------------------------------------------------------------
*  bit 6      RW   cs_ctrl      ->  0:  busy is 0
                                    1:  busy is 1
---------------------------------------------------------------------------
*  bit 5:4   --    reserved             
---------------------------------------------------------------------------
*  bit 3     RW    bus_width    ->  0:8bit
                                    1:16bit
 --------------------------------------------------------------------------                                              
*  bit 2:1   RW   pagesize      ->  00: 512Byte ;
                                    01: 2KByte ; 
                                    10: 4KByte ; 
                                    11: 8KByte ;     
--------------------------------------------------------------------------                                                
*  bit 0     RW    opmode       ->  0:boot mode ;
                                    1: normal mode ;  
*****************************************************************/
#define NANDC4_REG_CONF_OFFSET               (0x00)	

#define NANDC4_REG_PWIDTH                     (0x04)
#define NANDC4_REG_OPIDLE                     (0x08)
/***********NANDC4_REG_CMD*****************************************
---------------------------------------------------------------------------
*  bit 31:24   --    reserved             
---------------------------------------------------------------------------
*  bit 23:16  RW    read_status_cmd      ->  read status command
 --------------------------------------------------------------------------                                              
*  bit 7:0     RW    cmd2         ->   the secend command send to nand device 
--------------------------------------------------------------------------                                                
*  bit 7:0     RW    cmd1         ->   the first command send to nand device 
*****************************************************************/
#define NANDC4_REG_CMD_OFFSET                (0x0C)

#define NANDC4_REG_ADDRL_OFFSET               (0x10)
#define NANDC4_REG_ADDRH_OFFSET               (0x14)	
#define NANDC4_REG_DATA_NUM_OFFSET            (0x18)	

#define NANDC4_REG_OP_OFFSET                  (0x1C)

#define NANDC4_REG_STATUS_OFFSET              (0x20)
#define NANDC4_REG_INTEN_OFFSET               (0x24)	
#define NANDC4_REG_INTS_OFFSET                (0x28)	
#define NANDC4_REG_INTCLR_OFFSET              (0x2C)

#define NANDC4_REG_LOCK                       (0x30)
#define NANDC4_REG_LOCK_SA0                   (0x34)	
#define NANDC4_REG_LOCK_SA1                   (0x38)
#define NANDC4_REG_LOCK_SA2                   (0x3C)

#define NANDC4_REG_LOCK_SA3                   (0x40)	
#define NANDC4_REG_LOCK_EA0                   (0x44)
#define NANDC4_REG_LOCK_EA1                   (0x48)	
#define NANDC4_REG_LOCK_EA2                   (0x4C)

#define NANDC4_REG_LOCK_EA3                   (0x50)	
#define NANDC4_REG_EXPCMD                     (0x54)
#define NANDC4_REG_EXBCMD                     (0x58)
#define NANDC4_REG_ECC_TEST                   (0x5C)

#define NANDC4_REG_ECC_PARA_OFFSET            (0x70)
#define NANDC4_REG_VERSION                    (0x74)
#define NANDC4_REG_SEGMENT_ID                 (0x84)   

#define nandc4_op_read_id                    (0x66)   /*0110 0110   cmd1_en + addr_en + wait_ready_en  + read_data_en*/  
#define nandc4_op_read_start                 (0x6e)
#define nandc4_op_read_continue              (0x02)    
#define nandc4_op_read_end                   (0x02)    
#define nandc4_op_wirte_start                (0x70)
#define nandc4_op_wirte_continue             (0x10)
#define nandc4_op_wirte_end                  (0x1d)
#define nandc4_op_erase                      (0x6d)
#define nandc4_op_reset                      (0x44)   /*0100 0100   cmd1_en + wait_ready_en*/

#define nandc4_int_enable_all                (0x7ff)
#define nandc4_int_disable_all               (0x000)
#define nandc4_int_clear_all                 (0x7ff)

#define nandc4_segment_first                 (0x0)
#define nandc4_segment_last                  (0x1)


#define nandc4_ecc_err_none                  (0x0)
#define nandc4_ecc_err_valid                 (0x1)
#define nandc4_ecc_err_invalid               (0x2)


#define nandc4_ecc_data                      (0x15)
#define nandc4_ecc_oob                       (0x2a)
#define nandc4_ecc_all                       (0x3f)
#define nandc4_noecc_data                    (0x01)
#define nandc4_noecc_oob                     (0x02)
#define nandc4_noecc_all                     (0x03)


#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_V400_H_*/




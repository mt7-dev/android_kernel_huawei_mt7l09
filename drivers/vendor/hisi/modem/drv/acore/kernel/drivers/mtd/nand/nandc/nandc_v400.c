
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <hi_nandc.h>
#include "nandc_inc.h"
#include "nandc_v400.h"

/*****************************************************************************/
/* ecc_none */

struct nand_ecclayout nandc4_oob24_ecc4 = 
{
    STU_SET(.eccbytes)  0 ,
    STU_SET(.eccpos)    {0} ,
    STU_SET(.oobavail)  24 ,
    STU_SET(.oobfree)     
    { 
        {4, 19},
    }
}; 

struct nand_ecclayout nandc4_oob32_ecc4 = 
{
    STU_SET(.eccbytes)  0 ,
    STU_SET(.eccpos )   {0} ,
    STU_SET(.oobavail)  32 ,
    STU_SET(.oobfree)     
    { 
        {4, 19},
    }
}; 

struct nand_ecclayout nandc4_oob48_ecc4 = 
{
    STU_SET(.eccbytes)  0 ,
    STU_SET(.eccpos)    {0} ,
    STU_SET(.oobavail)  48 ,
    STU_SET(.oobfree)     
    { 
        {4, 19},
    }
}; 

/*define for register NANDC4_REG_CONF_OFFSET */
static struct nandc_reg_cont nandc4_cont_conf[] =  
{
    { 
        nandc_bits_op_mode,   
        {
            0, 1
        }, 
        {
            nandc_op_boot, nandc_op_normal
        },
    },
    
    { 
        nandc_bits_page_size,   
        {
             1, 2
        }, 
        {
            nandc_page_hk, nandc_page_2k, nandc_page_4k, nandc_page_8k
        },
    },

    { 
        nandc_bits_bus_width,   
        {
            3, 1
        }, 
        {
            nandc_bus_08, nandc_bus_16
        },
    },

    { 
        nandc_bits_ecc_type,   
        {
            7, 4    /*ecc_type and 'rb_sel together*/
        }, 
        {
            nandc4_ecc_none , nandc4_ecc_1bit, nandc4_ecc_4smb, nandc4_ecc_24p1kbit, nandc4_ecc_24p512bit 
        },
    },

    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_CMD */
static struct nandc_reg_cont nandc4_cont_cmd[] =  
{
    { 
        nandc_bits_cmd,   
        {
            0, 32
        }, 
        {0},
    },
    
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_ADDRL */
static struct nandc_reg_cont nandc4_cont_addrl[] =  
{
    { 
        nandc_bits_addr_low,   
        {
            0, 32
        }, 
        {0},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_ADDRH*/
static struct nandc_reg_cont nandc4_cont_addrh[] =  
{
    { 
        nandc_bits_addr_high,   
        {
            0, 32
        }, 
        {0},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_ADDRH*/
static struct nandc_reg_cont nandc4_cont_datanum[] =  
{
    { 
        nandc_bits_data_num,   
        {
            0, 32
        }, 
        {0},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_OP_OFFSET */
static struct nandc_reg_cont nandc4_cont_op[] =  
{
    { 
        nandc_bits_operation,   
        {
            0, 7
        }, 
        {nandc4_op_read_start, nandc4_op_read_id, nandc4_op_wirte_start, nandc4_op_erase },
    },

    { 
        nandc_bits_chip_select,   
        {
            7, 2
        }, 
        {nandc_cs_00, nandc_cs_01, nandc_cs_02, nandc_cs_03},
    },

    { 
        nandc_bits_addr_cycle,   
        {
            9, 3
        }, 
        {0},
    },
    
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },    
};


/*define for register NANDC4_REG_STATUS*/
static struct nandc_reg_cont nandc4_cont_opstatus[] =  
{
    { 
        nandc_bits_op_status,   
        {
            0, 1
        }, 
        {0},
    },

    { 
        nandc_bits_nf_status,   
        {
            5, 8
        }, 
        {0},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};


/*define for register NANDC4_REG_ADDRH*/
static struct nandc_reg_cont nandc4_cont_inten[] =  
{
    { 
        nandc_bits_int_enable,   
        {
            0, 32
        }, 
        {nandc4_int_enable_all,  nandc4_int_disable_all},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_ADDRH*/
static struct nandc_reg_cont nandc4_cont_intstatus[] =  
{
    { 
        nandc_bits_int_status,   
        {
            0, 1
        }, 
        {0},
    },

    { 
        nandc_bits_ecc_result,   
        {
            5, 2
        }, 
        {nandc4_ecc_err_none, nandc4_ecc_err_valid, nandc4_ecc_err_invalid },
    },

    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_ADDRH*/
static struct nandc_reg_cont nandc4_cont_intclr[] =  
{
    { 
        nandc_bits_int_clear,   
        {
            0, 32
        }, 
        {nandc4_int_enable_all},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_ECC_PARA*/
static struct nandc_reg_cont nandc4_cont_eccselect[] =  
{
    { 
        nandc_bits_ecc_select,   
        {
            0, 32
        }, 
        {nandc4_ecc_all, nandc4_noecc_all ,nandc4_ecc_oob},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC4_REG_SEGMENT_ID*/
static struct nandc_reg_cont nandc4_cont_segmentid[] =  
{
    { 
        nandc_bits_segment,   
        {
            0, 32
        }, 
        {nandc4_segment_first, nandc4_segment_last },
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};


struct nandc_reg_desc nandc4_reg_desc_table_v400[] = 
{
    {
        HI_NFC_CON_OFFSET,
        0x08,
        nandc4_cont_conf
    },
    
    {
        HI_NFC_CMD_OFFSET,
        0,
        nandc4_cont_cmd
    },

    {
        HI_NFC_ADDRL_OFFSET,
        0,
        nandc4_cont_addrl
    },

    {
        HI_NFC_ADDRH_OFFSET,
        0,
        nandc4_cont_addrh
    },

    {
        HI_NFC_DATA_NUM_OFFSET,
        0,
        nandc4_cont_datanum
    },
                
    {
        HI_NFC_OP_OFFSET,
        0,
        nandc4_cont_op
    },


    {
        HI_NFC_STATUS_OFFSET,
        0,
        nandc4_cont_opstatus
    },
    
    {
        HI_NFC_INTEN_OFFSET,
        0,
        nandc4_cont_inten
    },

    {
        HI_NFC_INTS_OFFSET,
        0,
        nandc4_cont_intstatus
    },

    {
        HI_NFC_INTCLR_OFFSET,
        0,
        nandc4_cont_intclr
    },

    {
        HI_NFC_OP_PARA_OFFSET,
        0,
        nandc4_cont_eccselect
    },

    {
        HI_NFC_SEGMENT_ID_OFFSET,
        0,
        nandc4_cont_segmentid
    },
    
    {
        NANDC_NULL,
        NANDC_NULL,
        NANDC_NULL
    },
};

struct nandc_dmap datamap_ecc4_v4  =                 /*the data map(page + oob)of the controller buffer, nandc4_dmap*/
{
    STU_SET(.bad2k) 2048,      
    STU_SET(.pmap2k) 
    {
        {
            STU_SET(.data){0,    2048},
            STU_SET(.oob) {2048, 24  },   
        }
    },


    STU_SET(.bad4k) 4096,
    STU_SET(.pmap4k) 
    {
        {
            {0,    2072},
            {0,    0   },     
        },
        {
            {0,    2024},
            {2024, 48  },     
        }
    },

    STU_SET(.bad8k) 8192,
    STU_SET(.pmap8k) 
    {
        {
            {0, 0},
            {0, 0},     
        },
        {
            {0, 0},
            {0, 0},     
        }
    }

};


struct nandc_dmap datamap_ecc16_v4  =                 /*the data map(page + oob)of the controller buffer, nandc4_dmap*/
{
    STU_SET(.bad2k) 2048,      
    STU_SET(.pmap2k) 
    {
        {
            {0,    2048},
            {2048, 32  },   
        }
    },

    STU_SET(.bad4k) 4096,      
    STU_SET(.pmap4k) 
    {
        {
            {0,    2064},
            {0,    0   },     
        },
        {
            {0,    2032},
            {2032, 32  },     
        }
    },

    STU_SET(.bad8k) 8192,      
    STU_SET(.pmap8k) 
    {
        {
            {0,    2056},
            {0,    0   },     
        },
        
        {
            {0,    2056},
            {0,    0  },     
        },
        
        {
            {0,    2056},
            {0,    0   },     
        },
        
        {
            {0,    2024},
            {2024, 32  },     
        }
    }

};

#define datamap_ecc24_v4 datamap_ecc16_v4


struct nandc_dmap_lookup nandc4_dmap_lookup[] =
{
    {
        &datamap_ecc4_v4,
        nandc4_ecc_4smb
    },

    {
        &datamap_ecc16_v4,
        nandc4_ecc_24p1kbit
    },

    {
        &datamap_ecc24_v4,
        nandc4_ecc_24p512bit
    },
    {NANDC_NULL, 0}
};

u8 nandc4_bitcmd[] =
{
    nandc_bits_cmd ,
    /*
    nandc_bits_cmd2 ,
    nandc_bits_cmd3 ,
    */
    nandc_bits_bus_width,
    nandc_bits_addr_high,
    nandc_bits_addr_low,
    nandc_bits_addr_cycle,
    nandc_bits_chip_select,
    nandc_bits_operation,
    nandc_bits_op_status ,
    nandc_bits_int_status,
    nandc_bits_data_num,
    nandc_bits_ecc_type ,
    nandc_bits_ecc_select,
    nandc_bits_page_size,
    nandc_bits_op_mode ,
    nandc_bits_int_enable,
    nandc_bits_int_clear,
    nandc_bits_segment,
    nandc_bits_ecc_result,
    nandc_bits_nf_status,
    
    nandc_bits_enum_end 
};

u32 nandc4_ctrl_init(struct nandc_bit_cmd  *  bitcmd, struct nandc_host *host)
{
    HI_NOTUSED(host);
    
    bitcmd->op_erase           =   nandc4_op_erase;
    bitcmd->op_reset           =   nandc4_op_reset;
    bitcmd->op_read_id         =   nandc4_op_read_id;
    bitcmd->op_read_start      =   nandc4_op_read_start;
    
    bitcmd->op_read_continue   =   nandc4_op_read_continue;
    bitcmd->op_read_end        =   nandc4_op_read_end;  
    bitcmd->op_write_start     =   nandc4_op_wirte_start;
    bitcmd->op_write_continue  =   nandc4_op_wirte_continue;
    
    bitcmd->op_write_end       =   nandc4_op_wirte_end;
    bitcmd->op_segment_start   =   nandc4_segment_first;
    bitcmd->op_segment_end     =   nandc4_segment_last;
    bitcmd->intmask_all        =   nandc4_int_enable_all;
    
    bitcmd->intdis_all         =   nandc4_int_disable_all;
    bitcmd->intclr_all         =   nandc4_int_clear_all;
    bitcmd->ecc_err_none       =   nandc4_ecc_err_none;
    bitcmd->ecc_err_valid      =   nandc4_ecc_err_valid;
    
    bitcmd->ecc_err_invalid    =   nandc4_ecc_err_invalid;
    bitcmd->ecc_all            =   nandc4_ecc_all;
    bitcmd->ecc_none           =   nandc4_noecc_all;
    bitcmd->ecc_oob            =   nandc4_ecc_oob;
	
#ifndef WIN32
    /*set NANDC4_REG_PWIDTH*/
    *(volatile int*)((int)host->regbase + NANDC4_REG_PWIDTH) = 0x00000555;
#endif

    return NANDC_OK;
}

struct nandc_ecc_info nandc4_eccinfo[] =
{
    {NANDC_SIZE_8K,     704,    nandc4_ecc_24p512bit,   &nandc4_oob32_ecc4  },
    {NANDC_SIZE_8K,     368,    nandc4_ecc_24p1kbit,    &nandc4_oob32_ecc4  },
    {NANDC_SIZE_8K,     218,    nandc4_ecc_4smb,        &nandc4_oob32_ecc4  },

    {NANDC_SIZE_4K,     368,    nandc4_ecc_24p512bit,   &nandc4_oob32_ecc4  },
    {NANDC_SIZE_4K,     256,    nandc4_ecc_4smb,        &nandc4_oob32_ecc4  }, /* ÈýÐÇ 4k page */
    {NANDC_SIZE_4K,     200,    nandc4_ecc_24p1kbit,    &nandc4_oob32_ecc4  },    
    {NANDC_SIZE_4K,     128,    nandc4_ecc_4smb,        &nandc4_oob48_ecc4  },
    {NANDC_SIZE_4K,     64,     nandc4_ecc_4smb,        &nandc4_oob32_ecc4  },

    {NANDC_SIZE_2K,     200,    nandc4_ecc_24p512bit,   &nandc4_oob32_ecc4  },
#if 0 //for cs stick samsung flash    
    {nandc_size_2k,     128,    nandc4_ecc_24p1kbit,    &nandc4_oob32_ecc4  },
#endif    
    {NANDC_SIZE_2K,     64,     nandc4_ecc_4smb,        &nandc4_oob24_ecc4  },
    
    {0,0,0,0},
};

struct nandc_ctrl_desc nandc4_spec = 
{
    &nandc4_reg_desc_table_v400[0],
    NANDC4_NAME,
    NANDC4_REG_BASE_ADDR, 
    NANDC4_REG_SIZE, 
    NANDC4_BUFFER_BASE_ADDR,
    NANDC4_BUFSIZE_TOTAL,
    NANDC4_MAX_CHIPS
};


struct nandc_ctrl_func   nandc4_operations =
{
    nandc4_ctrl_init,
};

struct nandc_init_info nandc4_init_info = 
{
    &nandc4_spec,
    &nandc4_operations,
    &nandc4_eccinfo[0],
    &nandc4_bitcmd[0],
    &nandc4_dmap_lookup[0],
    NANDC_NULL
};



/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#include <hi_base.h>
#include <hi_nandc.h>
#include "nandc_inc.h"

/* 此列表中的flash要使用4bit ecc */
unsigned char nand_id_use_4bitecc_table[][NANDC_READID_SIZE] =
{
    {0xad,0xac,0x90,0x15,0x56,0xad,0xac,0x90},          /* v7r2 udp(单封)/e5 */
    {NANDC_NULL,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};


/*****************************************************************************/
/* ecc_none */

struct nand_ecclayout nandc6_oob32_layout =
{
    STU_SET(.eccbytes)  0 ,
    STU_SET(.eccpos )   {0} ,
    STU_SET(.oobavail)  32 ,
    STU_SET(.oobfree)
    {
        {4, 19},
    }
};

/*define for register nandc6_REG_CONF_OFFSET */
static struct nandc_reg_cont nandc6_cont_conf[] =
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
             1, 3
        },
        {
            nandc_page_2k, nandc_page_4k, nandc_page_8k, nandc_page_16k
        },
    },

    {
        nandc_bits_bus_width,
        {
            4, 1
        },

        {
            nandc_bus_08, nandc_bus_16
        },
    },

    {
        nandc_bits_ecc_type,
        {
            8, 4    /*ecc_type and 'rb_sel together*/
        },
        {
            nandc6_ecc_none , nandc6_ecc_1bit, nandc6_ecc_4smb, nandc6_ecc_8bit, \
				nandc6_ecc_24p1kbit, nandc6_ecc_40bit, nandc6_ecc_64bit
        },
    },

	{
        nandc_bits_randomizer,
        {
            13, 1
        },
        {
            nandc6_open_randomizer, nandc6_close_randomizer
        },
    },

	{
        nandc_bits_async_interface_type,
        {
            14, 2
        },
        {
            nandc6_async_nand, nandc6_toggle, nandc6_onfi_v2, nandc6_onfi_v3
        },
    },

    {
        nandc_bits_enum_end,
        {
            0, 0
        },
        {
            0
        },
    },
};

/*define for register nandc6_REG_CMD */
static struct nandc_reg_cont nandc6_cont_cmd[] =
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

/*define for register nandc6_REG_ADDRL */
static struct nandc_reg_cont nandc6_cont_addrl[] =
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

/*define for register nandc6_REG_ADDRH*/
static struct nandc_reg_cont nandc6_cont_addrh[] =
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

/*define for register nandc6_REG_ADDRH*/
static struct nandc_reg_cont nandc6_cont_datanum[] =
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

/*define for register nandc6_REG_OP_OFFSET */
static struct nandc_reg_cont nandc6_cont_op[] =
{
    {
        nandc_bits_operation,
        {
            0, 7
        },
        {nandc6_op_read_start, nandc6_op_read_id, nandc6_op_wirte_start, nandc6_op_erase },
    },

    {
        nandc_bits_chip_select,
        {
            7, 2
        },
        {
        	nandc_cs_00, nandc_cs_01, nandc_cs_02, nandc_cs_03
		},
    },

    {
        nandc_bits_addr_cycle,
        {
            9, 3
        },
        {0},
    },

	{
        nandc_bits_read_id_en,
        {
            12, 1
        },
        {
			nandc6_not_read_id, nandc6_read_id
		},
    },

	{
        nandc_bits_rw_reg_en,
        {
            13, 1
        },
        {
			nandc6_rw_reg, nandc6_rw_data
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


/*define for register nandc6_REG_STATUS*/
static struct nandc_reg_cont nandc6_cont_opstatus[] =
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
            8, 8
        },
        {0},
    },

	{
        nandc_bits_ecc_class,
        {
            16, 3
        },
        {0},
    },

	{
        nandc_bits_randomizer_en,
        {
            19, 1
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


/*define for register nandc6_REG_ADDRH*/
static struct nandc_reg_cont nandc6_cont_inten[] =
{
    {
        NANDC_BITS_OP_DONE_EN,
        {
            0, 1
        },
        {NANDC6_INT_OP_DONE_DIS, NANDC6_INT_OP_DONE_EN},
    },

    {
        NANDC_BITS_CS_DONE_EN,
        {
            1, 4
        },
        {NANDC6_INT_CS_DONE_DIS,  NANDC6_INT_CS_DONE_EN},
    },

    {
        NANDC_BITS_ERR_VALID_EN,
        {
            5, 1
        },
        {NANDC6_INT_ERR_VALID_DIS,  NANDC6_INT_ERR_VALID_EN},
    },

    {
        NANDC_BITS_ERR_INVALID_EN,
        {
            6, 1
        },
        {NANDC6_INT_ERR_INVALID_DIS,  NANDC6_INT_ERR_INVALID_EN},
    },

    {
        NANDC_BITS_AHD_OP_EN,
        {
            7, 1
        },
        {NANDC6_INT_AHB_OP_DIS,  NANDC6_INT_AHB_OP_EN},
    },

    {
        NANDC_BITS_WR_LOCK_EN,
        {
            8, 1
        },
        {NANDC6_INT_WR_LOCK_DIS,  NANDC6_INT_WR_LOCK_EN},
    },

    {
        NANDC_BITS_DMA_DONE_EN,
        {
            9, 1
        },
        {NANDC6_INT_DMA_DONE_DIS,  NANDC6_INT_DMA_DONE_EN},
    },

    {
        NANDC_BITS_DMA_ERR_EN,
        {
            10, 1
        },
        {NANDC6_INT_DMA_ERR_DIS,  NANDC6_INT_DMA_ERR_EN},
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

/*define for register nandc6_REG_ADDRH*/
static struct nandc_reg_cont nandc6_cont_intstatus[] =
{
    {
        nandc_bits_int_status,
        {
            0, 1
        },
        {0},
    },

    {
		nandc_bits_cs0_done,
		{
			1, 1
		},
		{
			nandc6_cs0_int, nandc6_cs0_no_int
		},
	},

    {
        nandc_bits_ecc_result,
        {
            5, 2
        },
        {
        	nandc6_ecc_err_none, nandc6_ecc_err_valid, nandc6_ecc_err_invalid
		},
    },

    {
		nandc_bits_ahb_op_err,
		{
			7, 1
		},
		{
			nandc6_rw_no_int, nandc6_rw_int
		},
	},

	{
		nandc_bits_wr_lock_err,
		{
			8, 1
		},
		{
			nandc6_wr_lock_no_int, nandc6_wr_lock_int
		},
	},

	{
		nandc_bits_dma_done,
		{
			9, 1
		},
		{
			nandc6_dma_no_int, nandc6_dma_int
		},
	},

	{
		nandc_bits_dma_err,
		{
			10, 1
		},
		{
			nandc6_dma_err_no_int, nandc6_dma_err_int
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

/*define for register nandc6_REG_ADDRH*/
static struct nandc_reg_cont nandc6_cont_intclr[] =
{
    {
        nandc_bits_int_clear,
        {
            0, 32
        },
        {nandc6_int_enable_all},
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

static struct nandc_reg_cont nandc6_cont_dma_ctrl[] =
{
	{
		nandc_bits_dma_start,
		{
			0, 1
		},
		{
			nandc6_dma_over, nandc6_dma_start
		},
	},
	{
		nandc_bits_dma_ctrl,
		{
			1, 12
		},
		{
			nandc6_dma_read, nandc6_dma_read_oob_only, nandc6_dma_write
		},
	},
/*
	{
		nandc_bits_dma_rw_enable,
		{
			1, 1
		},
		{
			nandc6_dma_rd_enable, nandc6_dma_wr_enable
		},
	},
	{
		nandc_bits_burst_enable,
		{
			4, 3
		},
		{
			nandc6_burst4_enable, nandc6_burst8_enable, nandc6_burst16_enable
		},
	},
	{
		nandc_bits_dma_addr_num,
		{
			7, 1
		},
		{
			nandc6_dmaaddr5, nandc6_dmaaddr4
		},
	},
	{
		nandc_bits_dma_cs,
		{
			8, 2
		},
		{
			nandc6_dmacs0, nandc6_dmacs1, nandc6_dmacs2, nandc6_dmacs3
		},
	},
	{
		nandc_bits_dma_wr,
		{
			11, 1
		},
		{
			nandc6_wr_all, nandc_wr_onlydata
		},
	},
	{
		nandc_bits_dma_rd_oob,
		{
			12, 1
		},
		{
			nandc6_rd_page, nandc_rd_oob_only
		},
	},
*/
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

static struct nandc_reg_cont nandc6_cont_baddr_data[] =
{
	{
		nandc_bits_base_addr_data,
		{
			0, 32
		},
		{
			0
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

static struct nandc_reg_cont nandc6_cont_baddr_oob[] =
{
	{
		nandc_bits_base_addr_oob,
		{
			0, 32
		},
		{
			0
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

static struct nandc_reg_cont nandc6_cont_dma_len[] =
{
	{
		nandc_bits_dma_len,
		{
			16, 12
		},
		{
			0
		},
	},
	{
        nandc_bits_enum_end,
        {
            0, 0
        },
        {
            0
        },
    },
};

static struct nandc_reg_cont nandc6_cont_num0buf0[] =
{
	{
		nandc_bits_errnum0buf0,
		{
			0, 32
		},
		{
			0
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

static struct nandc_reg_cont nandc6_cont_rbmode[] =
{
	{
		nandc_bits_rb_status,
		{
			0, 1
		},
		{
			nandc6_rb_ready, nandc6_enhanced_clear
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

static struct nandc_reg_cont nandc6_cont_baddrd1[] =
{
	{
		nandc_bits_base_addr_d1,
		{
			0, 32
		},
		{
			0
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

static struct nandc_reg_cont nandc6_cont_baddrd2[] =
{
	{
		nandc_bits_base_addr_d2,
		{
			0, 32
		},
		{
			0
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

static struct nandc_reg_cont nandc6_cont_baddrd3[] =
{
	{
		nandc_bits_base_addr_d3,
		{
			0, 32
		},
		{
			0
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

static struct nandc_reg_cont nandc6_cont_oobsel[] =
{
	{
		nandc_bits_oob_len_sel,
		{
			0, 1
		},
		{
			nandc6_oob_len_8or16, nandc6_oob_len_32
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

/*define for register nandc6_REG_ECC_PARA*/
static struct nandc_reg_cont nandc6_cont_eccselect[] =
{
    {
        nandc_bits_ecc_select,
        {
            0, 32
        },
        {nandc6_noecc_all, nandc6_ecc_data, nandc6_ecc_oob ,nandc6_ecc_all},
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

/*define for register nandc6_REG_SEGMENT_ID*/
static struct nandc_reg_cont nandc6_cont_segmentid[] =
{
    {
        nandc_bits_segment,
        {
            0, 32
        },
        {nandc6_segment_first, nandc6_segment_last },
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


struct nandc_reg_desc nandc6_reg_desc_table_v600[] =
{
    {
        HI_NFC_CON_OFFSET,
        0x0482,
        nandc6_cont_conf
    },

    {
        HI_NFC_CMD_OFFSET,
        0x00703000,
        nandc6_cont_cmd
    },

    {
        HI_NFC_ADDRL_OFFSET,
        0,
        nandc6_cont_addrl
    },

    {
        HI_NFC_ADDRH_OFFSET,
        0,
        nandc6_cont_addrh
    },

    {
        HI_NFC_DATA_NUM_OFFSET,
        0x0840,
        nandc6_cont_datanum
    },

    {
        HI_NFC_OP_OFFSET,
        0,
        nandc6_cont_op
    },


    {
        HI_NFC_STATUS_OFFSET,
        0,
        nandc6_cont_opstatus
    },

    {
        HI_NFC_INTEN_OFFSET,
        0,
        nandc6_cont_inten
    },

    {
        HI_NFC_INTS_OFFSET,
        0,
        nandc6_cont_intstatus
    },

    {
        HI_NFC_INTCLR_OFFSET,
        0,
        nandc6_cont_intclr
    },

    {
		HI_NFC_DMA_CTRL_OFFSET,
		0x70,
		nandc6_cont_dma_ctrl
	},

	{
		HI_NFC_BADDR_D_OFFSET,
		0,
		nandc6_cont_baddr_data
	},

	{
		HI_NFC_BADDR_OOB_OFFSET,
		0,
		nandc6_cont_baddr_oob
	},

	{
		HI_NFC_DMA_LEN_OFFSET,
		0,
		nandc6_cont_dma_len
	},

    {
        HI_NFC_OP_PARA_OFFSET,
        0x0f,
        nandc6_cont_eccselect
    },

    {
        HI_NFC_SEGMENT_ID_OFFSET,
        0,
        nandc6_cont_segmentid
    },

    {
		HI_NFC_ERR_NUM0_BUF0_OFFSET,
		0,
		nandc6_cont_num0buf0
	},

    {
		HI_NF_RB_MODE_OFFSET,
		0,
		nandc6_cont_rbmode
	},

	{
		HI_NFC_BADDR_D1_OFFSET,
		0,
		nandc6_cont_baddrd1
	},

	{
		HI_NFC_BADDR_D2_OFFSET,
		0,
		nandc6_cont_baddrd2
	},

	{
		HI_NFC_BADDR_D3_OFFSET,
		0,
		nandc6_cont_baddrd3
	},

	{
		HI_NFC_OOB_SEL_OFFSET,
		0,
		nandc6_cont_oobsel
	},

    {
        NANDC_NULL,
        NANDC_NULL,
        NANDC_NULL
    },
};

struct nandc_dmap datamap_ecc4_v6  =                 /*the data map(page + oob)of the controller buffer, nandc6_dmap*/
{
    STU_SET(.bad2k) 2048,
    STU_SET(.pmap2k)
    {
        {
            STU_SET(.data){0,    2048},
            STU_SET(.oob) {2048, 32  },
        }
    },


    STU_SET(.bad4k) 4096,
    STU_SET(.pmap4k)
    {
        {
            {0,    	4096},
            {4096,  32  },
        },
        {
            {0,    	0},
            {0, 	0},
        }
    },

	/* 8k page size seems not supported by 4bit ecc refer to << NANDCV600 用户手册 >> */
    STU_SET(.bad8k) 8192,
    STU_SET(.pmap8k)
    {
        {
            {0, 	8192},
            {8192, 	32  },
        },
        {
            {0, 0},
            {0, 0},
        }
    }

};

struct nandc_dmap datamap_ecc8_v6  =      /*the data map(page + oob)of the controller buffer, nandc6_dmap*/
{
    STU_SET(.bad2k) 2048,
    STU_SET(.pmap2k)
    {
        {
            STU_SET(.data){0,    2048},
            STU_SET(.oob) {2048, 32  },   /* here may be {2048, 8}, which is determined by register NFC_OOB_SEL */
        }
    },


    STU_SET(.bad4k) 4096,
    STU_SET(.pmap4k)
    {
        {
            {0,    	4096},
            {4096,  32  },                /* here may be {4096, 16}, which is determined by register NFC_OOB_SEL */
        },
        {
            {0,    	0},
            {0, 	0},
        }
    },

	/* 8k page size seems not supported by 8bit ecc refer to << NANDCV600 用户手册 >> */
    STU_SET(.bad8k) 8192,
    STU_SET(.pmap8k)
    {
        {
            {0, 	8192},
            {8192, 	32  },
        },
        {
            {0, 0},
            {0, 0},
        }
    }

};

struct nandc_dmap datamap_ecc24_v6  =                 /*the data map(page + oob)of the controller buffer, nandc6_dmap*/
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
            {0,    4096},
            {4096, 32  },
        },
        {
            {0,    0   },
            {0,    0   },
        }
    },

    STU_SET(.bad8k) 8192,
    STU_SET(.pmap8k)
    {
        {
            {0,    8192},
            {8192, 32  },
        },

        {
            {0,    0   },
            {0,    0   },
        }
    }

};

struct nandc_dmap_lookup nandc6_dmap_lookup[] =
{
    {
        &datamap_ecc4_v6,
        nandc6_ecc_4smb
    },

	{
        &datamap_ecc8_v6,
        nandc6_ecc_8bit
    },

    {
        &datamap_ecc24_v6,
        nandc6_ecc_24p1kbit
    },

    {NANDC_NULL, 0}
};

u8 nandc6_bitcmd[] =
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
/*
    nandc_bits_int_enable,
*/
    nandc_bits_int_clear,
    nandc_bits_segment,
    nandc_bits_ecc_result,
    nandc_bits_nf_status,
    nandc_bits_async_interface_type,
    nandc_bits_randomizer,
    nandc_bits_read_id_en,
    nandc_bits_rw_reg_en,
    nandc_bits_ecc_class,
    nandc_bits_randomizer_en,
    nandc_bits_cs0_done,
 	nandc_bits_ahb_op_err,
 	nandc_bits_wr_lock_err,
 	nandc_bits_dma_done,
 	nandc_bits_dma_err,
 	nandc_bits_dma_start,
 /*
 	nandc_bits_dma_rw_enable,
 	nandc_bits_burst_enable,
 	nandc_bits_dma_addr_num,
 	nandc_bits_dma_cs,
 	nandc_bits_dma_wr,
 	nandc_bits_dma_rd_oob,
 */
 	nandc_bits_base_addr_data,
 	nandc_bits_base_addr_oob,
 	nandc_bits_dma_len,
 	nandc_bits_errnum0buf0,
 	nandc_bits_rb_status,
 	nandc_bits_base_addr_d1,
 	nandc_bits_base_addr_d2,
 	nandc_bits_base_addr_d3,
 	nandc_bits_oob_len_sel,
 	nandc_bits_dma_ctrl,

    NANDC_BITS_OP_DONE_EN,
    NANDC_BITS_CS_DONE_EN,
    NANDC_BITS_ERR_VALID_EN,
    NANDC_BITS_ERR_INVALID_EN,
    NANDC_BITS_AHD_OP_EN,
    NANDC_BITS_WR_LOCK_EN,
    NANDC_BITS_DMA_DONE_EN,
    NANDC_BITS_DMA_ERR_EN,

    nandc_bits_enum_end
};

u32 nandc6_ctrl_init(struct nandc_bit_cmd  *bitcmd, struct nandc_host *host)
{
    HI_NOTUSED(host);

    bitcmd->op_erase           =   nandc6_op_erase;
    bitcmd->op_reset           =   nandc6_op_reset;
    bitcmd->op_read_id         =   nandc6_op_read_id;
    bitcmd->op_read_start      =   nandc6_op_read_start;

    bitcmd->op_read_continue   =   nandc6_op_read_continue;
    bitcmd->op_read_end        =   nandc6_op_read_end;
    bitcmd->op_write_start     =   nandc6_op_wirte_start;
    bitcmd->op_write_continue  =   nandc6_op_wirte_continue;

    bitcmd->op_write_end       =   nandc6_op_wirte_end;
    bitcmd->op_segment_start   =   nandc6_segment_first;
    bitcmd->op_segment_end     =   nandc6_segment_last;
    bitcmd->intmask_all        =   nandc6_int_enable_all;

    bitcmd->intdis_all         =   nandc6_int_disable_all;
    bitcmd->inten_all          =   nandc6_int_enable_all;
    bitcmd->intclr_all         =   nandc6_int_clear_all;
    bitcmd->ecc_err_none       =   nandc6_ecc_err_none;
    bitcmd->ecc_err_valid      =   nandc6_ecc_err_valid;

    bitcmd->ecc_err_invalid    =   nandc6_ecc_err_invalid;
    bitcmd->ecc_all            =   nandc6_ecc_all;
    bitcmd->ecc_none           =   nandc6_noecc_all;
    bitcmd->ecc_oob            =   nandc6_ecc_oob;

	bitcmd->dma_read		   =   nandc6_dma_read;
	bitcmd->dma_read_oob_only  =   nandc6_dma_read_oob_only;
	bitcmd->dma_write          =   nandc6_dma_write;

	writel(0x555, (unsigned)(host->regbase + HI_NFC_PWIDTH_OFFSET));  /*lint !e124*/
    host->nand_4bitecc_table = &nand_id_use_4bitecc_table[0][0];

    return NANDC_OK;
}

struct nandc_ecc_info nandc6_eccinfo[] =
{
    {NANDC_SIZE_4K,     144,    nandc6_ecc_8bit,        &nandc6_oob32_layout  },
    {NANDC_SIZE_4K,     88,     nandc6_ecc_4smb,        &nandc6_oob32_layout  },

    {NANDC_SIZE_2K,     88,     nandc6_ecc_8bit,        &nandc6_oob32_layout  },
    {NANDC_SIZE_2K,     60,     nandc6_ecc_4smb,        &nandc6_oob32_layout  },

    {0,0,0,0},
};

struct nandc_ctrl_desc nandc6_spec =
{
    &nandc6_reg_desc_table_v600[0],
    NANDC6_NAME,
    NANDC6_REG_BASE_ADDR,
    NANDC6_REG_SIZE,
    NANDC6_BUFFER_BASE_ADDR,
    NANDC6_BUFSIZE_TOTAL,
    NANDC6_MAX_CHIPS
};


struct nandc_ctrl_func   nandc6_operations =
{
    nandc6_ctrl_init,
};

struct nandc_init_info nandc6_init_info =
{
    &nandc6_spec,
    &nandc6_operations,
    &nandc6_eccinfo[0],
    &nandc6_bitcmd[0],
    &nandc6_dmap_lookup[0],
    NANDC_NULL
};


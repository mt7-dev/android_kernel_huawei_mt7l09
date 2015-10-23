/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: The Hisilicon Nand Flash Controller physical and logical driver
*
*
* Filename:    nandc_ctrl.c
* Description: nandc模块的控制寄存器的操作
*
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*
*    All rights reserved.
*
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*lint -save -e506 */
/*lint --e{537}*/

#include <hi_nandc.h>
#include <ptable_com.h>
#include "nandc_inc.h"

/*nandflash模块的打印级别*/
#define NFCDBGLVL(LVL)       (NANDC_TRACE_CTRL|NANDC_TRACE_##LVL)

static inline u32 nandc_ctrl_program_result(struct nandc_host *host, u32 size_left);
static inline u32 nandc_ctrl_erase_result(struct nandc_host *host);
static inline void nandc_ctrl_recur_snapshot(struct nandc_host *host, struct nandc_reg_list* op_config);
static inline u32 nandc_ctrl_create_snapshot(struct nandc_host *host,
                                                       struct nandc_config_snapshot **snapshot,
                                                       u32 *totalsize,
                                                       u32 *onesize,
                                                       u32 *eccselect);
int g_nand_status;


/**
* 作用:根据pagesize的大小来得到要分几次来传送
*
* 参数:
* @pagesize          --页面的大小
* 描述:根据pagesize要计算分几次传送
*
*/
static inline u32 nandc_ctrl_divs_shift(u32 pagesize)
{
	u32 shifttime = 0;

	while(pagesize > NANDC_BUFSIZE_BASIC)
	{
		pagesize = pagesize >> 1;
		shifttime++;
	}
	return shifttime;
}


/**
* 作用:根据nandc_host->option参数来得到一次传送数据的大小、ecc的类型等参数
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @totalsize          		--总共传送的长度
* @ecctype          		--ECC的类型(1bit,4bit,16bit)
* @eccselect          		--ECC的选择类型(oob有ECC,oob+data有ECC)
* @onesize          		--一次传送的长度
* 描述:根据nandc_host中的参数得到上面的相关配置信息
*
*/
static inline u32 nandc_ctrl_get_param(struct nandc_host *host,
                                                u32 *totalsize,
                                                u32 *ecctype,
                                                u32 *eccselect,
                                                u32 *onesize
                                                )
{
	u32  ofs_in_page , total_size, total_oob, shifttimes,one_size;
	struct nandc_nand       *nand   =   host->nandchip;
	struct nandc_spec       *spec   =   &nand->spec;
	struct nandc_bit_cmd    *ctrlcmd=   &host->bitcmd;
	/*总共的ecc的数目*/
	total_oob   = host->curpart->ecclayout->oobavail;
	ofs_in_page  = ((u32)host->addr_physics[0]) & 0x0000FFFF;

	if(nandc_bus_16 == nand->busset)
	{
	    ofs_in_page <<=  1;
	}

	shifttimes   =  nandc_ctrl_divs_shift(spec->pagesize);
	/*不使能ECC模式操作*/
	if(host->options & NANDC_OPTION_ECC_FORBID)
	{
		/*total_size表示的是一页读的总共的数据长度,而不是总共要读的数据长度，上面会做处理，分一页一页的读数据*/
		total_size = host->length;

		/*参数检查*/
		if((ofs_in_page + total_size) > (spec->pagesize + spec->sparesize))
		{
			NANDC_TRACE(NFCDBGLVL(ERRO), ("mode0 not support yet! length:0x%x, offset in page: 0x%x",total_size, ofs_in_page));
			NANDC_DO_ASSERT(0, HICHAR_NULL, 0);
			goto ERRO;
		}
		/*当不使能ECC时一次最大读取到的数据为pagesize+sparesize*/
		one_size    =   (spec->pagesize + spec->sparesize) >> shifttimes;
		*ecctype    =   0 ;
		*eccselect  =   ctrlcmd->ecc_none;
	}
	else
	{
	    /*使能ECC时得到ECC的类型*/
	    *ecctype    =   nand->ecctype;
	     NANDC_REJECT_NULL(!ofs_in_page);
		/*仅仅是OOB数据的话*/
	    if(host->options & NANDC_OPTION_OOB_ONLY)
	    {
	    	/*得到相关的配置信息*/
	        total_size  =   total_oob;
	        one_size    =   total_size;
	        *eccselect  =   ctrlcmd->ecc_oob;

	    }
		/*OOB和data都要计算ECC*/
	    else
	    {
	    	/*总共的数据长度是OOB和pagesize*/
	        total_size  =   spec->pagesize + total_oob;
	        one_size    =   total_size >> shifttimes;
	        *eccselect  =   ctrlcmd->ecc_all;/* nandc_ecc_all */

	    }

	}

	*totalsize = total_size;
	*onesize = one_size;

	return NANDC_OK;

ERRO:

	return NANDC_ERROR;
}


/**
* 作用:创建寄存器相关数据结构挂载到链表上去
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @regaddr          		--寄存器的地址
* @listreg          		--寄存器链表指针
*
* 描述:在寄存器的链表上添加以regaddr地址的表项
*
*/
static inline  void nandc_ctrl_reg_to_list(struct nandc_host *host,u32 regaddr, struct nandc_reg_list** listreg)
{
	HI_NOTUSED(host);

	/*链表的头为空则创建*/
	if(0 == *listreg)
	{
		*listreg = (struct nandc_reg_list*)himalloc(sizeof(struct nandc_reg_list));
		NANDC_REJECT_NULL(*listreg);
		(*listreg)->address = regaddr;
		(*listreg)->value   = NANDC_REG_READ32(regaddr);
		(*listreg)->p_next  = NANDC_NULL;
		return ;
	}
	else
	{
		struct nandc_reg_list* list = *listreg;
		struct nandc_reg_list* last;

		do
		{   /*如果已经创建过*/
		    if(regaddr == list->address)
		    {
		        list->value = NANDC_REG_READ32(regaddr);
		        return;
		    }

		    last = list;
		    list = list->p_next;
		}while(NANDC_NULL != list);

		/*链表中没有要创建一个数据结构*/
		last->p_next = (struct nandc_reg_list*)himalloc(sizeof(struct nandc_reg_list));
		NANDC_REJECT_NULL(last->p_next);
		last->p_next->address = regaddr;
		last->p_next->value   = NANDC_REG_READ32(regaddr);
		last->p_next->p_next  = NANDC_NULL;
	}
ERRO:
	return;
}


/**
* 作用:把寄存器的值放到snapshot中去并且写硬件寄存器
*
* 参数:
* @bitfunc          		--寄存器的定义
* @value          		    --要写的寄存器的值
* @snapshot          		--snapshot的数据结构
*
*/
#define  nandc_ctrl_write_snapshot(bitfunc,value, snapshot)  \
{                                                                                   \
    if((bitfunc).bitset)                                                            \
    {                                                                               \
        NANDC_SET_REG_BITS(bitfunc, value);                                         \
        nandc_ctrl_reg_to_list(host,(u32)host->regbase + ((bitfunc).reg_offset), &((*snapshot)->listreg));\
    }                                                                               \
}


/**
* 作用:根据参数来创建snapshot,一般的情况下一个snapshot创建一次就行了
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @snapshot          		--snapshot的指针的指针,把创建的snapshot数据结构指针存放在此值中
* @totalsize          		--总共传送的数据长度
* @onesize          		--一次传送的数据长度
* @eccselect          		--ECC的类型
* 描述:根据参数来创建一个snapshot
*
*/
static inline u32 nandc_ctrl_create_snapshot(struct nandc_host *host,
                                                       struct nandc_config_snapshot **snapshot,
                                                       u32 *totalsize,
                                                       u32 *onesize,
                                                       u32 *eccselect)
{
    register struct nandc_bit_cluster*   cluster;
    struct nandc_nand           *nand = host->nandchip;
    struct nandc_spec           *spec = &nand->spec;
    u32  ecctype;

    register struct nandc_bit_cmd*       ctrlcmd;


    if(NANDC_OK != nandc_ctrl_get_param(host, totalsize, &ecctype, eccselect, onesize))
    {
        goto ERRO;
    }

    cluster = host->normal;

    *snapshot = (struct nandc_config_snapshot*)himalloc(sizeof(struct nandc_config_snapshot));
    NANDC_REJECT_NULL(*snapshot);
    memset((void*)*snapshot, 0x00 ,sizeof(struct nandc_config_snapshot));

    if(NAND_CMD_READSTART == host->command)
    {
        nandc_ctrl_write_snapshot(cluster->cmd, NAND_CMD_READ0|(NAND_CMD_READSTART<<8)|(NAND_CMD_NOP<<16),snapshot);
    }
    else
    {
        nandc_ctrl_write_snapshot(cluster->cmd, NAND_CMD_SEQIN|(NAND_CMD_PAGEPROG<<8)|(NAND_CMD_STATUS<<16),snapshot);
    }
    nandc_ctrl_write_snapshot(cluster->bus_width,    nand->busset,       snapshot );
    nandc_ctrl_write_snapshot(cluster->page_size,    nand->pageset,      snapshot );
    nandc_ctrl_write_snapshot(cluster->ecc_type,     ecctype,            snapshot );
    nandc_ctrl_write_snapshot(cluster->ecc_select,   *eccselect,         snapshot );



    ctrlcmd = &host->bitcmd;
    NANDC_WRITE_REG_BITS     (cluster->addr_cycle,   spec->addrcycle              );
    nandc_ctrl_write_snapshot(cluster->addr_cycle,   spec->addrcycle,    snapshot );
    nandc_ctrl_write_snapshot(cluster->chip_select,  host->chipselect,   snapshot );
    nandc_ctrl_write_snapshot(cluster->int_enable,   ctrlcmd->inten_all, snapshot );

	/*如果是DMA的方式下且是在ECC0时才要配置下面的寄存器*/
    if((host->options & NANDC_OPTION_DMA_ENABLE) && ((host->options & NANDC_OPTION_ECC_FORBID)))
    {
        nandc_ctrl_write_snapshot(cluster->dma_len,   spec->sparesize,   snapshot );
    }

    (*snapshot)->totalsize = *totalsize;
    (*snapshot)->onesize   = *onesize;
    (*snapshot)->eccselect = *eccselect;

    return NANDC_OK;
ERRO:
    return NANDC_ERROR;
}


/**
* 作用:把寄存器链表中的数据值写入到硬件寄存器中
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @op_config          		--寄存器链表数据结构

* 描述:写硬件寄存器的值
*
*/
static inline void nandc_ctrl_recur_snapshot(struct nandc_host *host, struct nandc_reg_list* op_config)
{
    HI_NOTUSED(host);

    while(NANDC_NULL != op_config)
    {
        NANDC_REG_WRITE32(op_config->address, op_config->value);
        op_config = op_config->p_next;
    }
}/*lint !e438*/

/**
* 作用:根据nandc_host中的参数来操作硬件寄存器
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @snapshot          		--snapshot的指针
* @totalsize          		--总共传送的数据长度
* @onesize          		--一次传送的数据长度
* @eccselect          		--ECC的类型
* 描述:根据参数来设置相关的硬件寄存器
*
*/
static inline void nandc_ctrl_snapshot(struct nandc_host *host, struct nandc_snapshot *snapshot,
                                                u32 *total_size, u32 *one_size, u32* eccselect)
{
	register struct nandc_config_snapshot      *config = NULL;
	struct nandc_bit_cluster*   cluster = host->normal;
	u32 onesize = 0,  totalsize=0 ; /*pc-lint 644*/
	/*使能ECC的时候是下面的处理流程*/
	if((!(host->options & NANDC_OPTION_ECC_FORBID)))
	{
		/*如果有配置nandc_config_snapshot直接把值写入硬件寄存器*/
		if(NANDC_NULL != (u32)snapshot->config_ecc)
		{
		    config = snapshot->config_ecc;
		    nandc_ctrl_recur_snapshot(host, config->listreg);
		    totalsize  = config->totalsize;
		    onesize    = config->onesize;
		    *eccselect = config->eccselect;
		}
		else
		{
			/*创建nandc_config_ecc的数据结构*/
		    nandc_ctrl_create_snapshot(host, &snapshot->config_ecc, &totalsize, &onesize, eccselect);
		}

#ifdef NANDC_USE_V600
		/* just for 8bit ecc */
		if (NANDC_ECC_8BIT == host->nandchip->ecctype)
		{
            #ifndef NANDC_OOBLENGTH_8BITECCTYPE
		    /* m3boot分区外，set 1，else set 0 */
		    if((host->addr_physics[0] < host->addr_to_sel_oob_len) && (0 == host->addr_physics[1]))
		    {
		        NANDC_SET_REG_BITS(cluster->oob_len_sel, 0);
		    }
			else
            #endif
			{
		        NANDC_SET_REG_BITS(cluster->oob_len_sel, 1);
		    }
		}
#endif

	}
	/*如果不使能ECC*/
	else if(host->options & NANDC_OPTION_ECC_FORBID)
	{
		if(NANDC_NULL != (u32)snapshot->config_raw)
		{
			/*裸写操作*/
		    config = snapshot->config_raw;
		    nandc_ctrl_recur_snapshot(host, config->listreg);
			/*这里表示此次要裸读写的数据大小*/
		    totalsize  = host->length;
		    onesize    = config->onesize;
		    *eccselect = config->eccselect;
		}
		else
		{
		    nandc_ctrl_create_snapshot(host,&snapshot->config_raw, &totalsize, &onesize, eccselect);
		}
		/*ECC不使能的时候都要配置下面的寄存器*/
		onesize    = ((totalsize <= onesize) ? totalsize : onesize);
		/*不使能ECC的时候时就要设置此寄存器,表示裸读的数据*/
		NANDC_SET_REG_BITS(cluster->data_num,   onesize );
	}
	/*填充数据*/
	*total_size = totalsize;
	*one_size   = onesize;

	return ;
}

/**
* 作用:发送命令字
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @cmdval1          		--命令字
* @cmdval2          		--命令字
* @cmdval3          		--命令字

* 描述:发送命令字
*
*/
static inline void nandc_ctrl_set_cmd(struct nandc_host *host, u32 cmdval1, u32 cmdval2, u32 cmdval3)
{
    struct nandc_bit_cluster*  cluster;

    cluster = host->normal;
	/*写硬件寄存器的值*/
    NANDC_SET_REG_BITS (cluster->cmd,     cmdval1|(cmdval2<<8)|(cmdval3<<16) );

}

/**
* 作用:等待状态寄存器的值为value
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @status_bit          		--状态寄存器
* @value          		    --等待具体的值为value
* @timeout_ms          		--超时时间
*
* 描述:等待状态寄存器的值为value
*
*/
static inline u32 nandc_ctrl_register_wait(struct nandc_host *host, struct nandc_bit_reg* status_bit, u32 value, u32 timeout_ms)
{
	u32 count =   NANDC_MS_TO_COUNT(timeout_ms);

	do
	{
		/*查看状态寄存器的值是否与value的值相等*/
		if(value == NANDC_REG_GETBIT32((u32)host->regbase + status_bit->reg_offset,
		                                                    status_bit->bitset->offset ,
		                                                    status_bit->bitset->length))
		{
		    return NANDC_OK;
		}
		count--;
	}
	while(count > 0);

	NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_register_wait,timeout! \n"));/*lint !e778*/
	return NANDC_E_TIMEOUT;

}


/**
* 作用:读nandc的状态寄存器操作是否完成
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
*
* 描述:读状态寄存器
*
*/
static inline u32 nandc_ctrl_read_result(struct nandc_host *host)
{
	struct nandc_bit_cluster*  cluster = NANDC_NULL;
	u32 errorcode = NANDC_ERROR;
	cluster = host->normal;

#ifdef NANDC_DEBUG
	/*nandc_ctrl_dump_reg(host, "nandc read result");*/
#endif
	/*查询读的状态，判断是否完成*/
	if(host->options & NANDC_OPTION_DMA_ENABLE)
	{
		errorcode = nandc_ctrl_register_wait(host, &cluster->dma_start, nandc_status_dma_done, NANDC_RD_RESULT_TIMEOUT);
	}
	else
	{
		errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_RD_RESULT_TIMEOUT);
	}

	if(NANDC_OK != errorcode)
	{
	    NANDC_TRACE(NFCDBGLVL(ERRO), ("wait nandc_ctrl_read_result!ERROR \n"));
	    goto ERRO;
	}

#ifdef NANDC_READ_RESULT_DELAY
	{
	    volatile u32 loopcnt = 500;
	    while(loopcnt--)
	    {};           /*wait 500 before read controler status (0x20 bit1)*/
	}
#endif
	if(host->bitcmd.ecc_err_valid == NANDC_GET_REG_BITS(cluster->ecc_result))
	{
		u32 ecc_warning_data = 0;
		ecc_warning_data = NANDC_GET_REG_BITS(cluster->errnum0buf0);
		NANDC_TRACE(NFCDBGLVL(WARNING), ("just ecc warning errnum0buf0  0x%x!\n",ecc_warning_data));
	}
    if(host->bitcmd.ecc_err_invalid == NANDC_GET_REG_BITS(cluster->ecc_result))
    {
        u32 ecc_error_data = 0;
        ecc_error_data = NANDC_GET_REG_BITS(cluster->errnum0buf0);
        if(host->force_readdata)
        {
            g_nand_status =0;
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_read_result: ecc error invalid errnum0buf0  0x%x continu read data ecc!\n",ecc_error_data));/*lint !e778*/

        }
        else
        {
            g_nand_status = NAND_STATUS_FAIL;
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_read_result: ecc error invalid errnum0buf0  0x%x!\n",ecc_error_data));/*lint !e778*/
            errorcode = NANDC_E_ECC;
            goto ERRO;
        }
    }
    else
    {
        g_nand_status = 0;
    }
	return NANDC_OK;

ERRO:
	NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_read_result:error address[0]:0x%x, address[1]:0x%x!\n",
	                                    host->addr_physics[0],host->addr_physics[1]));
	return errorcode;
}

/**
* 作用:nandc模块的清中断
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @value          		    --清除nandc模块的哪一些中断
*
* 描述:清除nandc模块的中断
*
*/
static inline u32 nandc_ctrl_int_clear(struct nandc_host *host,  u32 value)
{
    struct nandc_bit_cluster*  cluster;
    cluster = (struct nandc_bit_cluster*)host->normal;

    NANDC_SET_REG_BITS (cluster->int_clr,     value);

    return NANDC_OK;
}


/**
* 作用:使能nandc模块的中断
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @value          		    --使能nandc模块的中断
*
* 描述:使能nandc模块的中断
*
*/
static inline u32 nandc_ctrl_int_set(struct nandc_host *host, u32 value)
{
    writel(value, (unsigned)(host->regbase + HI_NFC_INTEN_OFFSET));     /*lint !e124*/
    return NANDC_OK;
}

/**
* 作用:复位nandflash的操作
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:复位nandflash的操作
*
*/
u32 nandc_ctrl_reset(struct nandc_host *host)
{
	register struct nandc_bit_cluster*  cluster;
	register struct nandc_bit_cmd*     ctrlcmd;
	volatile int reset_timeout = 10000;
	u32 errorcode = NANDC_ERROR;

	cluster = (struct nandc_bit_cluster*)host->normal;
	ctrlcmd = (struct nandc_bit_cmd*)&host->bitcmd;

	nandc_ctrl_set_cmd(host, NAND_CMD_RESET, NAND_CMD_NOP, NAND_CMD_NOP );
	nandc_ctrl_int_clear(host,                     ctrlcmd->intmask_all );

	NANDC_SET_REG_BITS (cluster->op_mode,          nandc_op_normal      );
	NANDC_SET_REG_BITS (cluster->ecc_select,       ctrlcmd->ecc_none    );
	/*把此寄存器的其它的bit的值都写为了0*/
	NANDC_WRITE_REG_BITS(cluster->addr_cycle,      NANDC_ADDRCYCLE_0    );
	NANDC_SET_REG_BITS (cluster->chip_select,      host->chipselect     );
	nandc_ctrl_int_set(host, ctrlcmd->intdis_all); /* disable all interrupt */

	NANDC_SET_REG_BITS (cluster->operate,          ctrlcmd->op_reset    );

	g_nand_status = 0;

	/*等待nandflash的操作完成*/
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 4) (int to unsigned int))*/
	errorcode = nandc_ctrl_register_wait(host, &cluster->int_stauts, nandc_status_int_done, (u32)reset_timeout);
	/*end*/
	if(NANDC_OK != errorcode)
	{
	    return NANDC_ERROR;
	}

	errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_RESULT_TIME_OUT);
	if(NANDC_OK != errorcode)
	{
	    return NANDC_ERROR;
	}

	return NANDC_OK;

}


/**
* 作用:读nandflash的ID
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:读nandflash的ID
*
*/
u32 nandc_ctrl_read_id(struct nandc_host *host)
{
    register struct nandc_bit_cluster*  cluster;
    register struct nandc_bit_cmd*     ctrlcmd;

    u32 errorcode;

    cluster = (struct nandc_bit_cluster*)host->normal;
    ctrlcmd = (struct nandc_bit_cmd*)&host->bitcmd;

    nandc_ctrl_int_set(host, ctrlcmd->intdis_all);

    nandc_ctrl_reset(host);

    nandc_ctrl_set_cmd(host, NAND_CMD_READID, NAND_CMD_NOP, NAND_CMD_NOP);

	NANDC_SET_REG_BITS(cluster->addr_high,         0                   );
	NANDC_SET_REG_BITS(cluster->addr_low,          0                   );
	NANDC_SET_REG_BITS(cluster->ecc_type,          0                   );
	NANDC_SET_REG_BITS(cluster->ecc_select,        ctrlcmd->ecc_none   );


    nandc_ctrl_int_clear(host,ctrlcmd->intmask_all);

    NANDC_SET_REG_BITS(cluster->op_mode,           nandc_op_normal     );
    NANDC_SET_REG_BITS(cluster->page_size,         NANDC_PAGE_SIZE_2K_REG);
    NANDC_SET_REG_BITS(cluster->bus_width,         host->probwidth     );

    NANDC_WRITE_REG_BITS(cluster->addr_cycle,      NANDC_ADDRCYCLE_1   );
    NANDC_SET_REG_BITS(cluster->chip_select,       host->chipselect    );
    NANDC_SET_REG_BITS(cluster->operate,           ctrlcmd->op_read_id );

    errorcode = nandc_ctrl_register_wait(host, &cluster->int_stauts, nandc_status_int_done, NANDC_RESULT_TIME_OUT);
    if(NANDC_OK != errorcode)
    {
        goto ERRO;
    }

    errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_RESULT_TIME_OUT);
    if(NANDC_OK != errorcode)
    {
        goto ERRO;
    }
	/*把硬件控制器中的数据读到内存中去*/
    if(NANDC_OK == errorcode)
    {
         nandc_data_transmit_raw(host, NANDC_READID_SIZE, 0, NAND_CMD_READSTART);
    }

    return errorcode;

ERRO:

    return NANDC_ERROR;

}

/**
* 作用:通过DMA的方式来读数据
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:以DMA的方式来读数据操作
*
*/
u32 nandc_ctrl_read_dma(struct nandc_host *host)
{
	register struct nandc_bit_cluster*   cluster;
	register struct nandc_bit_cmd*       ctrlcmd;
	struct nandc_nand           *nand = host->nandchip;
	struct nandc_spec           *spec = &nand->spec;
	u32 sizeleft;
	/*下面的三个为什么要用静态变量*/
	/*这里面的totalsize表示的是一页的数据的大小而不是这一次传送的数据的总大小*/
	static u32 totalsize, onesize, eccselect;
	u32 errorcode;


	cluster = host->normal;
	ctrlcmd = &host->bitcmd;

	nandc_ctrl_int_set(host, ctrlcmd->intdis_all); /* disable all interrupt */
	/*是否已经写了部分的硬件寄存器，如果没有写的话要写一次*/
	if(nandc_read_latch_done != host->latched)
	{
		/*这里面就有写寄存器的值*/
		nandc_ctrl_snapshot(host, &host->read_snapshot, &totalsize, &onesize, &eccselect  );

		if(nandc_read_latch_start == host->latched)
		{
			NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_read_dma, nandc_read_latch_start! \n"));
			host->latched = nandc_read_latch_done;
		}
	}
	else
	{
		NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_read_dma, nandc_read_latch_done, fast skip!\n"));
	}

	NANDC_SET_REG_BITS (cluster->addr_low,       host->addr_physics[0]		);
	NANDC_SET_REG_BITS (cluster->addr_high,      host->addr_physics[1]		);

	NANDC_SET_REG_BITS (cluster->base_addr_data,  (u32)host->databuf	);
	NANDC_SET_REG_BITS (cluster->base_addr_oob,   (u32)host->oobbuf		);


	NANDC_SET_REG_BITS (cluster->dma_ctrl,      ctrlcmd->dma_read	    );
	NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all	);

	if(NANDC_BUFSIZE_BASIC >= spec->pagesize)
	{
		NANDC_SET_REG_BITS (cluster->dma_start,      0x1					);
	}
	else
	{
		NANDC_SET_REG_BITS (cluster->segment,     ctrlcmd->op_segment_start	);
		NANDC_SET_REG_BITS (cluster->dma_start,      0x1					);
	}

	/*轮询方式等待nandflash的操作完成*/
	errorcode = nandc_ctrl_read_result(host);
	if(NANDC_OK != errorcode)
	{
		goto EXIT;
	}

	/*pagesie的大小比nandc的硬件缓冲区的大小还大的话*/
	sizeleft    =   totalsize - onesize;
	while(sizeleft > 0)
	{
		NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all);
		sizeleft -= onesize;
		if(0 == sizeleft)
		{
			NANDC_SET_REG_BITS (cluster->segment,   ctrlcmd->op_segment_end);
			NANDC_SET_REG_BITS (cluster->dma_start,      0x1						);
		}
		else
		{
			NANDC_SET_REG_BITS (cluster->dma_start,      0x1						);
		}

		errorcode = nandc_ctrl_read_result(host);
		if(NANDC_OK != errorcode)
		{
			goto EXIT;
		}
	EXIT:
		return errorcode;

	}
	return NANDC_OK;

}

/**
* 作用:通过总线的方式来读数据
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:以总线的方式来读数据操作
*
*/
u32 nandc_ctrl_read_normal(struct nandc_host *host)
{
	register struct nandc_bit_cluster*   cluster;
	register struct nandc_bit_cmd*       ctrlcmd;
	struct nandc_pagemap        *pagemap;
	struct nandc_nand           *nand = host->nandchip;
	struct nandc_spec           *spec = &nand->spec;
	u32 copy_offset = 0, read_times = 0, errorcode, sizeleft;
	static u32 totalsize, onesize, eccselect;

	cluster = host->normal;
	ctrlcmd = &host->bitcmd;

	if(nandc_read_latch_done != host->latched)
	{
		nandc_ctrl_snapshot(host, &host->read_snapshot, &totalsize, &onesize, &eccselect  );

		if(nandc_read_latch_start == host->latched)
		{
			NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_read_normal, nandc_read_latch_start! \n"));
			host->latched = nandc_read_latch_done;
		}
	}
	else
	{
		NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_read_normal, nandc_read_latch_done, fast skip!\n"));
	}

	NANDC_SET_REG_BITS(cluster->addr_low,       host->addr_physics[0]);
	NANDC_SET_REG_BITS(cluster->addr_high,      host->addr_physics[1]);
	NANDC_SET_REG_BITS(cluster->int_clr,        ctrlcmd->intmask_all );

	if(NANDC_BUFSIZE_BASIC >= spec->pagesize)
	{
		NANDC_SET_REG_BITS(cluster->operate,    ctrlcmd->op_read_start | ctrlcmd->op_read_end);
	}
	else
	{
		NANDC_SET_REG_BITS(cluster->segment,     ctrlcmd->op_segment_start);
		NANDC_SET_REG_BITS(cluster->operate,     ctrlcmd->op_read_start);
	}

	errorcode = nandc_ctrl_read_result(host);
	if(NANDC_OK == errorcode)
	{
		if((ctrlcmd->ecc_none == eccselect )||(ctrlcmd->ecc_oob == eccselect))
		{
			copy_offset = 0;
			nandc_data_transmit_raw(host, onesize, copy_offset, NAND_CMD_READSTART);
		}
		else
		{  /*whole page read with ECC mode*/
			pagemap =   host->nandchip->datamap; /* nandc_host_get_pagemap */
			read_times = 0;
			nandc_data_transmit_page(host, pagemap ,read_times, NAND_CMD_READSTART);
		}
	}
	else
	{
		goto EXIT;
	}

	/* for page size more than 8k bytes nand flash, for example, 16k bytes page size. */
	sizeleft    =   totalsize - onesize;
	while(sizeleft > 0)
	{
		NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all);
		sizeleft -= onesize;
		if(0 == sizeleft)
		{
			NANDC_SET_REG_BITS (cluster->segment,   ctrlcmd->op_segment_end);
			NANDC_SET_REG_BITS (cluster ->operate,  ctrlcmd->op_read_end);
		}
		else
		{
			NANDC_SET_REG_BITS (cluster ->operate,  ctrlcmd->op_read_continue);
		}

		errorcode = nandc_ctrl_read_result(host);
		if(NANDC_OK == errorcode)
		{
			if(ctrlcmd->ecc_none== eccselect )
			{
				copy_offset +=  onesize;
				nandc_data_transmit_raw(host, onesize, copy_offset, NAND_CMD_READSTART);
			}
			else
			{
				read_times  +=  1;
				pagemap =   host->nandchip->datamap;   /* nandc_host_get_pagemap */
				nandc_data_transmit_page(host, pagemap ,read_times, NAND_CMD_READSTART);
			}
		}
		else
		{
			goto EXIT;
		}

	}
	return NANDC_OK;

	EXIT:
	return errorcode;
}


/**
* 作用:查询nandc控制器的编程状态
*
* 参数:
* @host          		    --nandc的主机控制器指针
* @size_left          		--还剩多少个字节的要读
* 描述:查询编程的状态
*
*/
static inline u32 nandc_ctrl_program_result(struct nandc_host *host, u32 size_left)
{
	struct nandc_bit_cluster*  cluster = NANDC_NULL;
	u32 errorcode = NANDC_ERROR;
	u32 nand_status_timeout = NANDC_PRG_RESULT_TIMEOUT;

	cluster = host->normal;

#ifdef NANDC_DEBUG
	/* nandc_ctrl_dump_reg(host, "nandc program result"); */
#endif

	if(host->options & NANDC_OPTION_DMA_ENABLE)
	{
		errorcode = nandc_ctrl_register_wait(host, &cluster->dma_start, nandc_status_dma_done, NANDC_PRG_RESULT_TIMEOUT);
	}
	else
	{
		errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_PRG_RESULT_TIMEOUT);
	}
	if(NANDC_OK != errorcode)
	{
		NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_program_result,wait return ERROR! \n"));/*lint !e778*/
		return errorcode;
	}
	if (0 == size_left)
	{
		while ((NAND_STATUS_FAIL ==  (NAND_STATUS_FAIL & NANDC_GET_REG_BITS(cluster->nf_status))) && (nand_status_timeout))
		{
			nand_status_timeout--;
		}

		g_nand_status = NANDC_GET_REG_BITS(cluster->nf_status);

		if (0 ==  nand_status_timeout)
		{
			NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_program_result,status return FAIL! \n"));/*lint !e778*/
			return NANDC_E_TIMEOUT;
		}
	}
	return NANDC_OK;
}

/**
* 作用:查询nandc控制器的擦除状态
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:查询擦除的状态
*
*/
static inline u32 nandc_ctrl_erase_result(struct nandc_host *host)
{
	struct nandc_bit_cluster*  cluster = NANDC_NULL;
	u32 errorcode = NANDC_ERROR;
	u32 nand_status;

	cluster = host->normal;

#ifdef NANDC_DEBUG
	/* nandc_ctrl_dump_reg(host, "nandc erase result"); */
#endif
	/*等待此次操作完成*/
	errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_ERASE_RESULT_TIMEOUT);
	if(NANDC_OK != errorcode)
	{
		NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_erase_result,wait return error! \n"));/*lint !e778*/
		goto ERRO;
	}

	if(nandc_status_int_done != NANDC_GET_REG_BITS(cluster->int_stauts))
	{
		errorcode = NANDC_E_ERASE;
		NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_erase_result,int return error! \n"));/*lint !e778*/
		goto ERRO;
	}
	/*读nandflash的状态来判断操作是否执行成功*/
	nand_status = NANDC_GET_REG_BITS(cluster->nf_status);
	g_nand_status = nand_status;

	if(NAND_STATUS_FAIL & nand_status)
	{
		NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_erase_result,status return FAIL!:0x%x \n",nand_status));/*lint !e778*/
		errorcode = NANDC_E_ERASE;
		goto ERRO;
	}

	return NANDC_OK;

ERRO:
	return errorcode;
}

/**
* 作用:nandc控制器以DMA方式的编程
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:以DMA的方式编程
*
*/
u32 nandc_ctrl_program_dma(struct nandc_host *host)
{
	register struct nandc_bit_cluster*   cluster;
	register struct nandc_bit_cmd*       ctrlcmd;
	struct nandc_nand           *nand = host->nandchip;
	struct nandc_spec           *spec = &nand->spec;
	u32 sizeleft = spec->pagesize;
	static u32 totalsize, onesize, eccselect;
	u32 errorcode;


	cluster = host->normal;
	ctrlcmd = &host->bitcmd;

	nandc_ctrl_int_set(host, ctrlcmd->intdis_all); /* disable all interrupt */

	if(nandc_write_latch_done != host->latched)
	{
		nandc_ctrl_snapshot(host, &host->write_snapshot, &totalsize, &onesize, &eccselect  );

		if(nandc_write_latch_start == host->latched)
		{
		NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_program_normal, nandc_write_latch_start! \n"));
		host->latched = nandc_write_latch_done;
		}
	}
	else
	{
		NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_program_normal, nandc_write_latch_done, fast skip!\n"));
	}

	NANDC_SET_REG_BITS (cluster->base_addr_data,  (u32)host->databuf	);
	NANDC_SET_REG_BITS (cluster->base_addr_oob,   (u32)host->oobbuf		);

	NANDC_SET_REG_BITS (cluster->addr_low,      host->addr_physics[0]   );          /* nfc addrl */
	NANDC_SET_REG_BITS (cluster->addr_high,     host->addr_physics[1]   );          /* nfc addrh */
	NANDC_SET_REG_BITS (cluster->dma_ctrl,      ctrlcmd->dma_write		);          /* dma ctrl [1:12] */
	NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all	);          /* interupt clear */

	if(NANDC_BUFSIZE_BASIC >= spec->pagesize)
	{
		NANDC_SET_REG_BITS (cluster->dma_start,      0x1						);
	}
	else if(totalsize == onesize)
	{
		NANDC_SET_REG_BITS(cluster->segment,     ctrlcmd->op_segment_start | ctrlcmd->op_segment_end);
		NANDC_SET_REG_BITS (cluster->dma_start,      0x1						);
	}
	else
	{
		NANDC_SET_REG_BITS(cluster->segment,     ctrlcmd->op_segment_start);
		NANDC_SET_REG_BITS (cluster->dma_start,      0x1						);
	}

	errorcode   = nandc_ctrl_program_result(host, sizeleft);
	if(NANDC_OK != errorcode)
	{
		goto EXIT;
	}
	sizeleft    =   totalsize - onesize;
	while(sizeleft > 0)
	{
		NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all);
		sizeleft    -=  onesize;
		if(0 == sizeleft)
		{
			NANDC_SET_REG_BITS (cluster->segment,   ctrlcmd->op_segment_end);
			NANDC_SET_REG_BITS (cluster->dma_start,      0x1						);
		}
		else
		{
			NANDC_SET_REG_BITS (cluster->dma_start,      0x1						);
		}

		errorcode   = nandc_ctrl_program_result(host, sizeleft);
		if(NANDC_OK != errorcode)
		{
			goto EXIT;
		}

	EXIT:
		return errorcode;
	}

	return NANDC_OK;
}


/**
* 作用:nandc控制器以总线方式的编程
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:以总线的方式编程
*
*/
u32 nandc_ctrl_program_normal(struct nandc_host *host)
{
	register struct nandc_bit_cluster*   cluster;
	register struct nandc_bit_cmd*       ctrlcmd;
	struct nandc_pagemap        *pagemap;
	struct nandc_nand           *nand = host->nandchip;
	struct nandc_spec           *spec = &nand->spec;
	u32 copy_offset = 0, write_times = 0, errorcode, sizeleft;
	static u32 totalsize, onesize, eccselect;

	cluster = host->normal;
	ctrlcmd = &host->bitcmd;
	if(nandc_write_latch_done != host->latched)
	{
		nandc_ctrl_snapshot(host, &host->write_snapshot, &totalsize, &onesize, &eccselect  );

		if(nandc_write_latch_start == host->latched)
		{
			NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_program_normal, nandc_write_latch_start! \n"));
			host->latched = nandc_write_latch_done;
		}
	}
	else
	{
		NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_program_normal, nandc_write_latch_done, fast skip!\n"));
	}

	NANDC_SET_REG_BITS (cluster->addr_low,      host->addr_physics[0]   );
	NANDC_SET_REG_BITS (cluster->addr_high,     host->addr_physics[1]   );
	NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all	);

	if((ctrlcmd->ecc_none== eccselect )||(ctrlcmd->ecc_oob== eccselect))
	{
		copy_offset =   0;
		nandc_data_transmit_raw(host, onesize, copy_offset, NAND_CMD_PAGEPROG);
	}
	else
	{   /* whole page write with ECC mode */
		pagemap     =   host->nandchip->datamap;  /* nandc_host_get_pagemap */
		write_times =   0;
		nandc_data_transmit_page(host, pagemap ,write_times, NAND_CMD_PAGEPROG);
	}

	if(NANDC_BUFSIZE_BASIC >= spec->pagesize)
	{
		NANDC_SET_REG_BITS(cluster ->operate,    ctrlcmd->op_write_start | ctrlcmd->op_write_end);
	}
	else if(totalsize == onesize)
	{
		NANDC_SET_REG_BITS(cluster->segment,     ctrlcmd->op_segment_start | ctrlcmd->op_segment_end);
		NANDC_SET_REG_BITS(cluster ->operate,    ctrlcmd->op_write_start | ctrlcmd->op_write_end);
	}
	else
	{
		NANDC_SET_REG_BITS(cluster->segment,     ctrlcmd->op_segment_start);
		NANDC_SET_REG_BITS(cluster->operate,     ctrlcmd->op_write_start);
	}

	sizeleft    =   totalsize - onesize;
	errorcode   = nandc_ctrl_program_result(host, sizeleft);
	if(NANDC_OK != errorcode)
	{
		goto EXIT;
	}

	while(sizeleft > 0)
	{

		if(ctrlcmd->ecc_none== eccselect )
		{
			copy_offset += onesize;
			nandc_data_transmit_raw(host, onesize, copy_offset, NAND_CMD_PAGEPROG);
		}
		else
		{
			write_times +=  1;
			pagemap =   host->nandchip->datamap;  /* nandc_host_get_pagemap */
			nandc_data_transmit_page(host, pagemap ,write_times, NAND_CMD_PAGEPROG);
		}

		NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all);
		sizeleft    -=  onesize;
		if(0 == sizeleft)
		{
			NANDC_SET_REG_BITS (cluster->segment,   ctrlcmd->op_segment_end);
			NANDC_SET_REG_BITS (cluster ->operate,  ctrlcmd->op_write_end);
		}
		else
		{
			NANDC_SET_REG_BITS (cluster ->operate,  ctrlcmd->op_read_continue);
		}

		errorcode = nandc_ctrl_program_result(host,sizeleft);
		if(NANDC_OK != errorcode)
		{
			goto EXIT;
		}
	}

	return NANDC_OK;

EXIT:
	return errorcode;
}


/**
* 作用:nandflash的擦除操作
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:擦除nandflash的块
*
*/
u32 nandc_ctrl_erase_block(struct nandc_host *host)
{
    register struct nandc_bit_cluster    *cluster;
    register struct nandc_bit_cmd        *ctrlcmd;
    struct nandc_nand           *nand = host->nandchip;
    struct nandc_spec           *spec = &nand->spec;
    u32 addrcycle;
    u32 count = 3;
    u32 errocode = NANDC_ERROR;

    ctrlcmd = (struct nandc_bit_cmd*)&host->bitcmd;
    cluster = host->normal;

    addrcycle = spec->addrcycle - 2;

    nandc_ctrl_int_set(host, ctrlcmd->intdis_all); /* disable all interrupt */

    if(0 == host->addr_physics[0])
    {
        NANDC_TRACE(NFCDBGLVL(WARNING), ("nandc_ctrl_earse_block: erase blknum 0.\n"));
    }

    nandc_ctrl_set_cmd  (host,      NAND_CMD_ERASE1, NAND_CMD_ERASE2, NAND_CMD_STATUS);
    nandc_ctrl_int_clear(host,      ctrlcmd->intmask_all                );

    NANDC_SET_REG_BITS(cluster->addr_low,       host->addr_physics[0]   );
    NANDC_SET_REG_BITS(cluster->ecc_select,     ctrlcmd->ecc_none       );
    NANDC_SET_REG_BITS(cluster->ecc_type,       0                       );

    NANDC_WRITE_REG_BITS(cluster->addr_cycle,   addrcycle			    );
    NANDC_SET_REG_BITS(cluster->chip_select,    host->chipselect        );
	/*要重复最多试三次*/
    while((NANDC_OK != errocode)&&(count > 0))
    {
        NANDC_SET_REG_BITS(cluster->operate,        ctrlcmd->op_erase   );
        errocode =  nandc_ctrl_erase_result(host);
        count--;
    }

    if(NANDC_OK != errocode)
    {
       NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_earse_block, failed 3 times!\n"));/*lint !e778*/
       return errocode;
    }

    return NANDC_OK;
}


/**
* 作用:nandflash的命令处理流程
*
* 参数:
* @host          		    --nandc的主机控制器指针
*
* 描述:nandflash的命令处理流程,所有的命令的执行都在此函数中
*
*/
u32 nandc_ctrl_entry(struct nandc_host *host)
{
    u32 errorcode = NANDC_ERROR ;
	/*读出错了之后会重复读的次数*/
    u32 try_left = NANDC_RD_RETRY_CNT;
    volatile u32 delay_loop;

    NANDC_TRACE(NFCDBGLVL(NORMAL), ("(0x%x)nandc_ctrl_entrycs(%d),real(0x%x), low addr:0x%x, high addr:0x%x\n",
                                               host->command,  host->chipselect, (unsigned int)host->addr_real, host->addr_physics[0],host->addr_physics[1]));
TRY_AGAIN1:

    switch (host->command)
    {
        case NAND_CMD_READSTART:
            if(host->options & NANDC_OPTION_DMA_ENABLE)
            {
                errorcode = nandc_ctrl_read_dma(host);
            }
            else
            {
                errorcode = nandc_ctrl_read_normal(host);
            }
        break;

        case NAND_CMD_READID:
            errorcode = nandc_ctrl_read_id(host);
        break;

        case NAND_CMD_PAGEPROG:

            if(host->options & NANDC_OPTION_DMA_ENABLE)
            {
               errorcode =  nandc_ctrl_program_dma(host);
            }
            else
            {
               errorcode =  nandc_ctrl_program_normal(host);
            }
        break;

        case NAND_CMD_ERASE2:

            errorcode = nandc_ctrl_erase_block(host);

        break;

        case NAND_CMD_SEQIN:
        case NAND_CMD_ERASE1:
        case NAND_CMD_READ0:
        break;

        case NAND_CMD_RESET:
			errorcode = nandc_ctrl_reset(host);
        break;

        default :
            errorcode = NANDC_ERROR;
            NANDC_DO_ASSERT(0, "unknown nand command!", host->command);
        break;
    }

    switch (host->command)
    {
        case NAND_CMD_READSTART:
        {
            if((NANDC_OK != errorcode)&&(try_left))
            {
                try_left--;
                delay_loop = 100000;
                while(delay_loop--)
                {};
                goto TRY_AGAIN1;
            }
        }
        break;
        default:
        break;
    }

    return errorcode;


}

/**
* 作用:打印nandc控制器寄存器的值
*
* 参数:
* @headstr          		    --字符串头
*
* 描述:当如果在调试的过程中如果出现了错误可以把此代码加入调试寄存器的值
*
*/
extern struct nandc_host*  nandc_nand_host;
void nandc_ctrl_dump_reg(char* headstr)
{
    struct nandc_host *host = nandc_nand_host;
    void __iomem * dumpaddr = host->regbase;
    u32 regsize =host->regsize;
    u32  i=0;
    u32* buf = NULL;

    buf = (u32 *)himalloc(regsize+0x20);
    if(!buf)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO),("nandc_ctrl_dump_reg Line %d!ERROR \n",(int)__LINE__));
        return;
    }
    memset(buf, 0, regsize+0x20);
    memcpy(buf,dumpaddr,regsize);
    if(headstr)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO),("%s\n",headstr));
    }
    NANDC_TRACE(NFCDBGLVL(ERRO),("nandc_ctrl_dump_reg start:\n"));
    /*可能会多打一些值出来,当regsize不是32的整数倍时*/
    for(i=0; i<regsize/4; i+=4)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO),("dump reg:\n0x%x 0x%x 0x%x 0x%x \n",
        *(buf+0),*(buf+1),*(buf+2),*(buf+3)));
        buf+=4;
    }
    NANDC_TRACE(NFCDBGLVL(ERRO),("nandc_ctrl_dump_reg end:\n"));
    hifree(buf);

}

/*lint -restore */

#ifdef COMPILE_BOOTLOADER
u32 nandc_io_bitwidth_set(u32 bitwidth)
{
    u32 ioshare_val;

    /*设置寄存器*/
    if(NAND_BUSWIDTH_16 == bitwidth)
    {
        ioshare_val = 1;
    }
    else    /* 8bit NAND,配置上下拉 */
    {
        ioshare_val = 0;
        /* 2_24, 2_25, 2_27, 2_28 下拉*/
	    NANDC_REG_WRITE32(INTEGRATOR_SC_BASE + 0x08B0, 0x02000200);
	    NANDC_REG_WRITE32(INTEGRATOR_SC_BASE + 0x08B4, 0x02000000);
	    NANDC_REG_WRITE32(INTEGRATOR_SC_BASE + 0x08B8, 0x00000200);
    }
    NANDC_REG_SETBIT32(INTEGRATOR_SC_BASE + INTEGRATOR_SC_IOS_CTRL98, 2, 1, ioshare_val);
    return NANDC_OK;
}
#endif

#ifdef __cplusplus
}
#endif



/*lint --e{124,752}*/
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/syscalls.h>
#include "osl_bio.h" 
#include "osl_sem.h"
#include "hi_syssc_interface.h"

#include "nandc_mtd.h"
#include "nandc_balong.h"
#include "../../mtdcore.h"

/*lint -save -e767*/ 
#define NFCDBGLVL(LVL)      (NANDC_TRACE_MTD|NANDC_TRACE_##LVL)
/*lint -restore */  

extern irqreturn_t nandc_ctrl_interrupt(int irqno,void *dev_id);
extern u32 ptable_parse_mtd_partitions(struct mtd_partition** mtd_parts, u32 *nr_parts);   
extern struct nand_ecclayout nandc6_oob32_layout;

#define NANDC_STATUS_SUSPEND    1
#define NANDC_STATUS_RESUME     0
int g_nandc_status = NANDC_STATUS_RESUME;
int nand_spec_info_resume(struct mtd_info *mtd, struct nand_spec_shared_mem *spec_shmem)
{
    struct nand_chip *chip = NULL;
    struct nandc_host *host = NULL;

	if(!mtd || !spec_shmem)
	{
		return -EINVAL;
	}

    if(NAND_SPEC_ALREADY_SAVED != spec_shmem->flag)
	{
	    printk("ERROR: invalid flag 0x%x\n", spec_shmem->flag);
		return -EINVAL;
	}
    
    chip = (struct nand_chip *)mtd->priv;
    host = (struct nandc_host *)chip->priv;

	mtd->writesize = spec_shmem->page_size;
	mtd->oobsize = spec_shmem->spare_size;
	mtd->erasesize = spec_shmem->block_size;
	chip->chipsize = (uint64_t)spec_shmem->chip_size;
    host->ecctype_onfi  = spec_shmem->ecc_type;
    printk("ecctype_onfi %x,writesize %x,oobsize %x,erasesize %x,chipsize %x spec_shmem->ecc_type %x\n",
        (u32)host->ecctype_onfi,(u32)mtd->writesize,(u32)mtd->oobsize,(u32)mtd->erasesize,(u32)chip->chipsize,(u32)spec_shmem->ecc_type);
	return 0;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_dump_reg() - view the value of nand controller's registers.
 *
 * PARAMETER:
 * @headstr - [input]it is a character string describe which function calls it.
 * @mtd - [input]through this structure pointer to get nand controler registers's base
 *        address,and it is virtual address.
 *
 * DESCRIPTION:
 * This function is a debug function.you can see nand controller registers's values
 * by calling it.
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
void nandc_dump_reg(char* headstr,struct mtd_info *mtd)
{

	struct nand_chip *chip = mtd->priv;
	struct nandc_host *host = chip->priv;
	void __iomem * dumpaddr = host->regbase;

	char* buf = NULL;
	char* buf2 = NULL;


	buf2 = (char*)kmalloc(64,GFP_KERNEL);
	if(0 == buf2)/*&&(yaffs_dbg != 1))*/
		return;

	memcpy(buf2,dumpaddr,64);
	buf = buf2;

	printk("%s\n",headstr);

	printk("dump reg:\n0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
	          *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
	          *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));
	buf+=32;
	printk("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
              *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
              *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));

	kfree(buf2);

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_cmd_ctrl() - transfer command and parameter to low layer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * @dat - [input]it contains address or command.
 * @ctrl - [input] control bit mask:
 *                 NAND_NCE:select the nand flash chip.
 *                 NAND_CLE:latch the command.
 *                 NAND_CTRL_CHANGE:command change.
 *                 NAND_ALE:latch the address.
 *
 * DESCRIPTION:
 * This function sets operation address to host->addr_physics[],and address is written to
 * nand controller address register by low layer interface (NANDC_SET_REG_BITS).be the
 * same as,the function also sets command to host->command,then command will be written to
 * nand controller command register by low layer interface (NANDC_SET_REG_BITS).
 *
 * CALL FUNC:
 * nand_command_lp() - /drivers/mtd/nand/nand_base.c
 *
 ********************************************************************************/
static void nandc_mtd_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;

    /*set the operation(read/write/erase) address in nand flash chip*/
    if (ctrl & NAND_ALE)
    {
        /*addr_value is 0 or 1,corresponding to host->addr_physics[0] and host->addr_physics[1].
        host->addr_physics[0] and host->addr_physics[1] is 32 bits.*/
        unsigned int addr_value = 0;
        unsigned int addr_offset = 0;

        if (ctrl & NAND_CTRL_CHANGE)
        {
            host->addr_cycle = 0x0;
            host->addr_physics[0] = 0x0;
            host->addr_physics[1] = 0x0;
        }
        addr_offset =  host->addr_cycle << 3;  /* 2 power of 3 equal 8,the result is mutiple of 8 .*/

        if (host->addr_cycle >= NANDC_ADDR_CYCLE_MASK)/*[false alarm]:屏蔽fortify错误 */
        {
            addr_offset = (host->addr_cycle - NANDC_ADDR_CYCLE_MASK) << 3;/*[false alarm]:屏蔽fortify错误 */
            addr_value = 1; /*[false alarm]:屏蔽fortify错误 */
        }

        host->addr_physics[addr_value] |= (((unsigned int)dat & 0xff) << addr_offset);

        host->addr_cycle ++;
    }

    if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE))
    {
        int ret;
        host->command = dat & 0xff;

        if((chip->options & NAND_BAD_MANAGEMENT) && ((dat == NAND_CMD_READSTART) || (dat == NAND_CMD_PAGEPROG)))
        {
            host->options   = NANDC_OPTION_ECC_FORBID;
#ifdef NANDC_USE_V600
            host->options   |= NANDC_OPTION_DMA_ENABLE;
#endif
            host->length    = mtd->writesize + mtd->oobsize;
            chip->options   &= ~NAND_BAD_MANAGEMENT;
        }
        else
        {
#ifdef NANDC_USE_V600
            host->options =  NANDC_OPTION_DMA_ENABLE; /*NANDC_OPTION_DMA_ENABLE;*/
#else
            host->options =  0;
#endif
        }
        host->addr_real = NANDC_ADDR_INVALID;
        
        if((NAND_CMD_ERASE2 != host->command) && (NAND_CMD_ERASE1 != host->command) 
            && (!(NAND_BAD_MANAGEMENT & chip->options)))
        host->addr_physics[0] &= 0xffff0000; /*mask offset in page*/

        if(NANDC_STATUS_SUSPEND == g_nandc_status)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERROR Nand controller is still suspending !!!!\n"));
            return;
        }

        ret = clk_enable(host->clk);
        if(ret)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERROR Nandc clk enable failed, ret = 0x%x !!!!\n", ret));
            return;
        }
        
        nandc_ctrl_entry(host);
        clk_disable(host->clk); 
    }

    if ((dat == NAND_CMD_NONE) && host->addr_cycle)
    {
        if (host->command == NAND_CMD_SEQIN
            || host->command == NAND_CMD_READ0
            || host->command == NAND_CMD_READID)
        {
            host->offset = 0x0;
            host->column = host->addr_physics[0] & 0xffff;
        }
    }
}
/*****************************************************************************/

static int nandc_mtd_dev_ready(struct mtd_info *mtd)
{
    return 0x1;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_select_chip() - determines which nand flash chip to operate.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the main structure host.
 * @chipselect - [input]chip number.
 *
 * DESCRIPTION:
 * This function determines which nand flash chip to operate.(There may be more than
 * one nand flash chip)
 *
 * CALL FUNC:
 * chip->select_chip()-
 *
 ********************************************************************************/
static void nandc_mtd_select_chip(struct mtd_info *mtd, int chipselect)
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;

    if (chipselect < 0)
        return;

    if (chipselect >= NANDC_MAX_CHIPS)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("invalid chipselect: %d\n", chipselect));/*lint !e778*/
    }

	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
    host->chipselect = (u32)chipselect;
	/*end*/
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_read_byte() - read byte from nand controller buffer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 *
 * DESCRIPTION:
 * This function reads byte from nand controller buffer.
 * host->databuf is virtual address for nand controller buffer.
 *
 * CALL FUNC:
 * chip->read_byte() - /drivers/mtd/nand/nand_base.c
 *
 ********************************************************************************/
static uint8_t nandc_mtd_read_byte(struct mtd_info *mtd)
{
    struct nand_chip *chip = mtd->priv;   
    struct nandc_host *host = chip->priv;
    int ret;
    unsigned char value = 0;//pclint 734

    if (host->command == NAND_CMD_STATUS)
    {
        ret = clk_enable(host->clk);
        if(ret)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERROR Nandc clk enable failed, ret = 0x%x !!!!\n", ret));
            return 0xFF;
        }       

        value = readb(chip->IO_ADDR_R);
        clk_disable(host->clk);  
        return value; 
    }

    if (host->IsUseBuffer)
    {
        value = readb(host->databuf + host->column + host->offset);
    }
    else
    {
        ret = clk_enable(host->clk);
        if(ret)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERROR Nandc clk enable failed, ret = 0x%x !!!!\n", ret));
            return 0xFF;
        }       
        value = readb(chip->IO_ADDR_R + host->column + host->offset);
        clk_disable(host->clk);  
    }

    host->offset++;

    return value;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_read_word() -  read word from nand controller buffer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 *
 * DESCRIPTION:
 * This function reads word from nand controller buffer.
 * host->databuf is virtual address for nand controller buffer.
 *
 * CALL FUNC:
 * chip->read_word() - /drivers/mtd/nand/nand_base.c
 *
 ********************************************************************************/
static u16 nandc_mtd_read_word(struct mtd_info *mtd)
{
    struct nand_chip  *chip = mtd->priv;  
    struct nandc_host *host = chip->priv;
    u32 comoffset = host->column << 1;
    int ret;
    u16 value = 0;

    if (host->IsUseBuffer)
    {
        value = readw(host->databuf + comoffset + host->offset);
    }
    else
    {
        ret = clk_enable(host->clk);
        if(ret)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("ERROR Nandc clk enable failed, ret = 0x%x !!!!\n", ret));
            return 0xFF;
        }       
        value = readw(chip->IO_ADDR_R + comoffset + host->offset);
        clk_disable(host->clk);  
    }
    host->offset += 2;

    return value;
}/*lint !e529*/

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_read_buf() - read data from nand controller buffer to ram buffer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * @buf - [input]data destination address in ram.
 * @len - [input]data len.
 *
 * DESCRIPTION:
 * This function read data from nand controller buffer to ram buffer.
 *
 * CALL FUNC:
 * chip->read_buf() - /drivers/mtd/nand/nand_base.c
 *
 ********************************************************************************/
static void nandc_mtd_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;
    u32 comoffset = host->column;

    if( nandc_bus_16 == host->nandchip->busset)
    {
        comoffset = comoffset << 1;
    }

    if (host->IsUseBuffer)
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        memcpy(buf, host->databuf + comoffset + host->offset, (unsigned int)len);
		/*end*/
    }
    else
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        memcpy(buf, (unsigned char *)(chip->IO_ADDR_R)
            + comoffset + host->offset, (unsigned int)len);
		/*end*/
    }
    host->offset += (unsigned int)len;

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_write_buf() - write data from ram buffer to nand controller buffer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * @buf - [input]data source address in ram.
 * @len - [input]data len.
 *
 * DESCRIPTION:
 * This function writes data from ram buffer to nand controller buffer.
 *
 * CALL FUNC:
 * chip->write_buf() - /drivers/mtd/nand/nand_base.c
 *
 ********************************************************************************/
static void nandc_mtd_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
    struct nand_chip  *chip = mtd->priv;
    struct nandc_host *host = chip->priv;
    u32 comoffset = host->column;

    if(nandc_bus_16 == host->nandchip->busset)
    {
        comoffset  <<= 1;
    }
#ifdef NANDC_USE_V400
    if (host->IsUseBuffer)
    {
#endif
	    /*TODO: bad flag must clear, for nand page size large then 2k, 4k, 8k*/
        memset(host->databuf + host->nandchip->spec.pagesize, 0xff, NANDC_BAD_FLAG_BYTE_SIZE); 
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        memcpy(host->databuf + comoffset + host->offset, buf, (unsigned int)len);
		/*end*/
#ifdef NANDC_USE_V400
    }
    else
    {
	    /*bad flag must clear, for 2k page flash*/
   		memset(chip->IO_ADDR_W + NANDC_BUFSIZE_BASIC, 0xff, NANDC_BAD_FLAG_BYTE_SIZE);
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        memcpy((unsigned char *)(chip->IO_ADDR_W)
            + comoffset + host->offset, buf, (unsigned int)len);
		/*end*/
    }
#endif
    host->offset += (unsigned int)len;

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_nand_init() - initialize host -> nandchip
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * @parts - [input]store information for each partition in part[i]
 * @nr_parts - [nr_parts]partition amount
 *
 * DESCRIPTION:
 * This function initlizes host -> nandchip.
 *
 * CALL FUNC:
 * nandc_mtd_probe() -
 *
 ********************************************************************************/
int nandc_mtd_nand_init(struct mtd_info *mtd, struct mtd_partition *parts, u32 nr_parts )
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;
    u32 buswidth = NAND_BUSWIDTH_16;

    host->IsUseBuffer = NANDC_FALSE ;
    host->chip  = chip;
    host->chipselect    = 0;

#ifdef CONFIG_PM  
     /* get memory to story register value when deepsleep */  
     host->sleep_buffer = (u32 *)kmalloc(NANDC_SLEEP_BUFFER_SIZE, GFP_KERNEL);  
     if(!host->sleep_buffer)  
     {  
         return -ENOMEM;  
     }  
#endif


    if(mtd->writesize > NANDC_BUFSIZE_BASIC)
    {
       host->IsUseBuffer = NANDC_TRUE;
    }

#ifdef NANDC_USE_V600
    host->IsUseBuffer = NANDC_TRUE;
#endif

#ifdef NANDC_USE_V400
    if(NANDC_TRUE == host->IsUseBuffer)
    {
        host->databuf = kmalloc((NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE), GFP_KERNEL);
        if (!host->databuf)
            return -ENOMEM;
        host->oobbuf = host->databuf + NAND_MAX_PAGESIZE;
    }
#else    
    host->databuf = dma_alloc_coherent(NANDC_NULL, NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE, &host->dma_addr,GFP_ATOMIC);
    if (!host->databuf)
        return -ENOMEM;
    host->oobbuf = host->databuf + mtd->writesize;
    host->dma_oob_addr = host->dma_addr + mtd->writesize;
    memset(host->databuf, 0xFF, (NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE));
#endif
    
    buswidth = chip->options & NAND_BUSWIDTH_16;
   /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 7) (int to unsigned int))*/
    if( !nandc_host_init_mtd( host,
                            mtd->writesize,
                            mtd->oobsize,
                            (u32)mtd->size,
                            mtd->erasesize,
                            buswidth,
                            (u32)chip->numchips,
                            parts,
                            nr_parts))         /*end*/
    {

        chip->ecc.layout  =  host->curpart->ecclayout;
        return 0;

    }
    else
    {
        return -ENOMEM;
    }

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_host_init() - initialize the nand_chip structure.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the nand_chip structure.
 * @pltdev - [input]platform data structure.
 * @phost - [input/output]
 *
 * DESCRIPTION:
 * This function initializes the nand_chip structure which contains many functions such
 * as chip->read_buf and chip->write_buf that will be used for operation to nand
 * (eg read,write etc.)
 *
 * CALL FUNC:
 * nandc_mtd_probe() -
 *
 ********************************************************************************/
int nandc_mtd_host_init(struct mtd_info *mtd, struct platform_device *pltdev, struct nandc_host **phost)
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = NULL;
    int result = 0, i;

    chip->cmd_ctrl      =   nandc_mtd_cmd_ctrl;
    chip->dev_ready     =   nandc_mtd_dev_ready;
    chip->select_chip   =   nandc_mtd_select_chip;
    chip->read_byte     =   nandc_mtd_read_byte;
    chip->read_word     =   nandc_mtd_read_word;
    chip->write_buf     =   nandc_mtd_write_buf;
    chip->read_buf      =   nandc_mtd_read_buf;

    chip->chip_delay    =   NANDC_CHIP_DELAY;
    chip->options       =   NAND_NO_AUTOINCR | NAND_SKIP_BBTSCAN;

    chip->ecc.layout    =   &nandc6_oob32_layout;
    chip->ecc.mode      =   NAND_ECC_NONE;

    /*if(nandc_nand_create_host(&host))*/
    host = nandc_native_host_create(nandc_init_seed);
    nandc_nand_host = host;
    if(NULL == host)
    {
        dev_err(&pltdev->dev, "failed to nandc_nand_create_host.\n");
        return -ENOMEM;
    }
    (void)nand_balong_port_mtd();

    host->chip = chip ;
    host->mtd  = mtd;

    chip->priv = host;

    /*get virtual address for nand controller registers from physical address*/
    host->regbase = (unsigned *)ioremap(pltdev->resource[0].start,
                    pltdev->resource[0].end - pltdev->resource[0].start + 1);

    if (!host->regbase)
    {
        dev_err(&pltdev->dev, "ioremap failed\n");
        kfree(host);
        return -EIO;
    }


   /* nandc4_ctrl_init or nandc6_ctrl_init
      It saves configuration value for erery operation,when we do
      one operation to nand flash chip later, we write the value
      to corresponding register.*/
    if(host->ctrlfunc->init_cmd)
    {
        host->ctrlfunc->init_cmd(&host->bitcmd,  host);
    }
    host->databuf  = (u8*)host->chip->IO_ADDR_R;
    host->oobbuf  = (u8*)host->chip->IO_ADDR_R + NANDC_BUFSIZE_BASIC;
    host->bufbase = (u8*)host->chip->IO_ADDR_R;

    /*first ,try to find 8 bit width nand chip*/
    chip->options &= ~NAND_BUSWIDTH_16;

    if(nand_scan(mtd, NANDC_MAX_CHIPS))
    {
        /*if failed ,try to find 16 bit width nand chip*/
        chip->options |= NAND_BUSWIDTH_16;

        if (nand_scan(mtd, NANDC_MAX_CHIPS))
        {
            result = -ENXIO;
        }
    }
 
    /* 保存flash id */
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);
	chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);
   	for (i = 0; i < NANDC_READID_SIZE; i++)
		host->flash_id[i] = chip->read_byte(mtd);
    

    *phost = host;

    return result;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_probe() - linux MTD layer initialization function.
 *
 * PARAMETER:
 * @dev - [input]through it to get the platform_device structure.
 *
 * DESCRIPTION:
 * This function mallocs ram space for the main structure corelative with MTD layer.
 *
 * CALL FUNC:
 * driver_register() -
 *
 ********************************************************************************/
static int nandc_mtd_probe(struct platform_device* dev)
{
    struct nandc_host *host = NULL;
    struct nand_chip  *chip = NULL;
    struct mtd_info   *mtd  = NULL;
    struct platform_device *pltdev = dev; /* nandc_mtd_pltdev */
    struct mtd_partition *parts = NULL;
    struct mtd_partition *temt_parts = NULL ;
    unsigned int nr_parts = 0;
    int result = 0;
    unsigned int i;

    int size = sizeof(struct nand_chip) + sizeof(struct mtd_info);

    /* set ebi normal mode to avoid fault when sharing ebi with emi, only for nandc v600 */
    hi_syssc_ebi_mode_normal();
    
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    chip = (struct nand_chip*)kmalloc((size_t)size, GFP_KERNEL);
    /*end*/
    if (!chip)
    {
        dev_err(&pltdev->dev, "nandc_mtd_probe failed to allocate device structure.\n");
        return -ENOMEM;
    }
    memset((void *)chip, 0, (unsigned int)size); /*clean the pclint e516*/
    mtd  = (struct mtd_info *)&chip[1]; /*lint !e740*/
    mtd->priv  = chip;
    mtd->owner = THIS_MODULE;
    mtd->name  = (char*)(pltdev->name);

    /*get virtual address for nand controller buffer from physical address*/
    chip->IO_ADDR_R = chip->IO_ADDR_W = ioremap_nocache(
                                    pltdev->resource[1].start,
                                    pltdev->resource[1].end - pltdev->resource[1].start + 1);

    if (!chip->IO_ADDR_R)
    {
        dev_err(&pltdev->dev, "ioremap failed\n");
        kfree(chip);
        return -EIO;
    }

    if (nandc_mtd_host_init(mtd, pltdev, &host))
    {
        dev_err(&pltdev->dev, "failed to nandc_mtd_host_init.\n");
        result = -ENOMEM;
        goto err;
    }
    dev->dev.platform_data = host;

    mtd->name  = NANDC_NULL;
    /*get partition table from C-core bootloader flash partition table*/
    /* coverity[alloc_arg] */
    if(ptable_parse_mtd_partitions(&parts, &nr_parts))/*lint !e64*/
    {
        dev_err(&pltdev->dev, "failed to ptable_parse_mtd_partitions.\n");
        result = -ENOMEM;
        goto err;
    }
    mtd->name  = (char*)(pltdev->name);

    /* get clock and enable */
    host->clk = clk_get(NULL,"nandc_clk");/*[false alarm]:屏蔽fortify错误 */
    if(IS_ERR(host->clk)){
		dev_err(&pltdev->dev, "cannot get nandc clk!\n");
		goto err;
	}

#ifdef NANDC_USE_V600
#ifdef FEATURE_NANDC_DMA_USE_INTERRUPT

    /* get nandc irq  */
	host->irq_id =  platform_get_irq(dev, 0);
	if (host->irq_id <= 0) 
    {
		dev_err(&dev->dev, "cannot get IRQ\n");
		goto err;
	}

    /* combined interupt id with ISR */
	result = request_irq(host->irq_id, nandc_ctrl_interrupt, IRQF_DISABLED, dev_name(&dev->dev), host);
	if (result != 0) 
    {
		dev_err(&dev->dev, "cannot claim IRQ %d\n", host->irq_id);
		goto err;
	}

    /* sem init */
    osl_sem_init(0, &(host->sem));
    
#endif 
#endif 
    if (nr_parts > 0)
    {
        temt_parts = parts;
        for (i = 0; i < nr_parts; i++)
        {
#if defined(NANDC_DEBUG)
            NANDC_TRACE(NFCDBGLVL(NORMAL), ("partitions[%d] = "
            "{.name = %s, .offset = 0x%.8llx, "".size = 0x%.8llx (%lluKiB) }\n",
            i,
            temt_parts->name,
            temt_parts->offset,
            temt_parts->size,
            (temt_parts->size)/1024));/*lint !e778*/
#endif
            temt_parts++;
        }
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        if (nandc_mtd_nand_init(mtd, parts, (u32)nr_parts))
		/*end*/
        {
            printk("ERROR ! nandc_mtd_nand_init!!!\n");
            result = -ENODEV;
            goto err;
        }

        return add_mtd_partitions(mtd, parts, (int)nr_parts);
    }
    else
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("ERROR !No Partition !!!!\n"));/*lint !e778*/
        result = -ENODEV;
        goto err;
    }
    
err:
    if(host)
    {
        if (host->databuf != host->chip->IO_ADDR_R)
        {
			#ifdef NANDC_USE_V600
            dma_free_coherent(NANDC_NULL,NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE,host->databuf,host->dma_addr);
			#else
			kfree(host->databuf);
			#endif
			
        }
			
        iounmap(host->regbase);
        kfree(host);
    }
    if(chip)
    {
        iounmap(chip->IO_ADDR_W);
    }
    dev->dev.platform_data = NULL;

    /* coverity[leaked_storage] */
    return result;
}/*lint !e550 !e529*/

 /*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_remove() -
 *
 * PARAMETER:
 * @dev - [input]the linux standard data structure.
 *
 * DESCRIPTION:
 *
 * CALL FUNC:
 * () -
 ********************************************************************************/
static int nandc_mtd_remove(struct platform_device *dev)
{
    struct nandc_host *host = dev->dev.platform_data;

    nand_release(host->mtd);
	kfree(host->usrdefine->partition);
    kfree(host->usrdefine);
	kfree(host->buffers);
	if (host->databuf)
	{
		#ifdef NANDC_USE_V600
	    dma_free_coherent(NANDC_NULL,NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE,host->databuf,host->dma_addr);
		#else
		kfree(host->databuf);
		#endif
	}
    iounmap(host->chip->IO_ADDR_W);
    iounmap(host->regbase);
    kfree(host);
    dev->dev.platform_data = NULL;

    return 0;
}
/*****************************************************************************/

static void nandc_mtd_pltdev_release (struct device *dev)
{
    return ;
}
/*****************************************************************************/

/* for power manager */  
#ifdef CONFIG_PM 
static int balong_nandc_prepare(struct device *dev)
{        
    return 0;      
}            

static void balong_nandc_complete(struct device *dev)      
{
    return;      
}            

static int balong_nandc_suspend(struct device *dev)     
{          
    struct nandc_host *host = (struct nandc_host *)dev->platform_data;          
    u32 nandc_reg_base_addr = (u32)(host->regbase);          
    u32 *nandc_sleep_buffer  = host->sleep_buffer;                   
    u32 i;
    s32 ret;

    ret = clk_enable(host->clk);
    if(ret)
    {
        return ret;
    }  
    
    for(i = 0; reg_offset[i] != END_FLAG; i++)
    {
        *nandc_sleep_buffer = readl(nandc_reg_base_addr + reg_offset[i]);
        nandc_sleep_buffer++;
    }
    clk_disable(host->clk); 
    g_nandc_status = NANDC_STATUS_SUSPEND;
    /* ok, return */          
    return 0;      
}            

static int balong_nandc_resume(struct device *dev)      
{          
    struct nandc_host *host = (struct nandc_host *)dev->platform_data;    
    
    /* resume register value */          
    u32 nandc_reg_base_addr = (u32)(host->regbase);          
    u32 *nandc_sleep_buffer  = host->sleep_buffer;         
    u32 i;
    s32 ret;
    
    ret = clk_enable(host->clk);
    if(ret)
    {
        return ret;
    }  

    host->command = NAND_CMD_RESET;
    nandc_ctrl_entry(host);    
    for(i = 0; reg_offset[i] != END_FLAG; i++)
    {
        writel(*nandc_sleep_buffer, nandc_reg_base_addr + reg_offset[i]); 
        nandc_sleep_buffer++;

    }
    clk_disable(host->clk); 
    g_nandc_status = NANDC_STATUS_RESUME;
    /* ok, return */      
    return 0;      
} 

static const struct dev_pm_ops balong_nandc_dev_pm_ops =
{          
    .prepare    =   balong_nandc_prepare,          
    .complete   =   balong_nandc_complete,      
    .suspend_noirq    =   balong_nandc_suspend,      
    .resume_noirq     =   balong_nandc_resume,      
};  

#define BALONG_NANDC_PM_OPS (&balong_nandc_dev_pm_ops)      
#else      
#define BALONG_NANDC_PM_OPS NULL      
#endif            
/*****************************************************************************/ 

static struct platform_driver nandc_mtd_driver =
{
    .driver = {	.name       =   NANDC_NAME,
				.owner  	=	THIS_MODULE,
			    .bus        =   &platform_bus_type,
			    .pm         =   BALONG_NANDC_PM_OPS, 
    		  },
    .probe      =   nandc_mtd_probe,
    .remove     =   nandc_mtd_remove,
};
/*****************************************************************************/

static struct resource nandc_mtd_resources[] =
{
    [0] = {
        .start  = NANDC_REG_BASE_ADDR,
        .end    = NANDC_REG_BASE_ADDR + NANDC_REG_SIZE - 1,
        .flags  = IORESOURCE_MEM,
    },

    [1] = {
        .start  = NANDC_BUFFER_BASE_ADDR,
        .end    = NANDC_BUFFER_BASE_ADDR + NANDC_BUFSIZE_TOTAL - 1,
        .flags  = IORESOURCE_MEM,
    },
#if defined(NANDC_USE_V600)
    [2] = {
        .start  = NANDC_IRQ,
        .end    = NANDC_IRQ,
        .flags  = IORESOURCE_IRQ,
    },
#endif
};
/*****************************************************************************/

static struct platform_device nandc_mtd_pltdev =
{
    .name           =   NANDC_NAME,
    .id             =   -1,

    .dev.platform_data     = NULL,
    .dev.dma_mask          = (u64*)~0,
    .dev.coherent_dma_mask = (u64) ~0,
    .dev.release           = nandc_mtd_pltdev_release,
    /*解决参数类型不对的问题-- linux内核*/
    .num_resources  = ARRAY_SIZE(nandc_mtd_resources),/*lint !e30 !e806*/
    .resource       = nandc_mtd_resources,
};

/*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_module_init() - register dirver and device for nand driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver initialization interface.
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
static int __init nandc_mtd_module_init(void)
{
    int result;

    printk("Hisilicon Nand Flash Controller Driver, Version 1.00\n");

    result = platform_driver_register(&nandc_mtd_driver);
    if (result < 0)
    {
        return result;
    }

    result = platform_device_register(&nandc_mtd_pltdev);
    if (result < 0)
    {
        platform_driver_unregister(&nandc_mtd_driver);
        return result;
    }

    return result;
}

/*******************************************************************************
 * FUNC NAME:
 * nandc_mtd_module_exit() - unregister dirver and device for nand driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver de_initialization interface.
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
static void __exit nandc_mtd_module_exit (void)
{
    platform_driver_unregister(&nandc_mtd_driver);
    platform_device_unregister(&nandc_mtd_pltdev);
}
/*****************************************************************************/
#if defined (FEATURE_FLASH_LESS)
#else
module_init(nandc_mtd_module_init); /*lint !e19 */
#endif
module_exit(nandc_mtd_module_exit); /*lint !e19 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Tech.Co.,Ltd.<monan@huawei.com>");
MODULE_DESCRIPTION("BalongV3R2 Hisilicon Nand Flash  Controller driver");




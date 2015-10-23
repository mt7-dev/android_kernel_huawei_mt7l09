

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e537*/
#include <flash.h>
#include "nv_balong.h"
#include "nv_xml_dec.h"
/*lint -restore +e537*/

#define NV_WRITE_SUCCESS     ((unsigned char)0)


static struct nv_flash_file_header_stru g_boot_nv_file[NV_FILE_BUTT] = {
                                  {NULL,NV_FILE_DLOAD,         0,0,0,0,0,NV_DLOAD_PATH,      NV_DLOAD_SEC_NAME},
                                  {NULL,NV_FILE_BACKUP,        0,0,0,0,0,NV_BACK_PATH,       NV_BACK_SEC_NAME},
                                  {NULL,NV_FILE_XNV_CARD_1,    0,0,0,0,0,NV_XNV_CARD1_PATH,  NV_DLOAD_SEC_NAME},
                                  {NULL,NV_FILE_XNV_CARD_2,    0,0,0,0,0,NV_XNV_CARD2_PATH,  NV_DLOAD_SEC_NAME},
                                  {NULL,NV_FILE_CUST_CARD_1,   0,0,0,0,0,NV_CUST_CARD1_PATH, NV_DLOAD_SEC_NAME},
                                  {NULL,NV_FILE_CUST_CARD_2,   0,0,0,0,0,NV_CUST_CARD2_PATH, NV_DLOAD_SEC_NAME},
                                  //lint --e{784}
                                  {NULL,NV_FILE_SYS_NV,        0,0,0,0,0,NV_SYS_NV_PATH,     NV_SYS_SEC_NAME},
                                  {NULL,NV_FILE_DEFAULT,       0,0,0,0,0,NV_DEFAULT_PATH,    NV_DEF_SEC_NAME},
                                  {NULL,NV_FILE_XNV_MAP_CARD_1,0,0,0,0,0, NV_XNV_CARD1_MAP_PATH,  NV_DLOAD_SEC_NAME},
                                  {NULL,NV_FILE_XNV_MAP_CARD_2,0,0,0,0,0, NV_XNV_CARD2_MAP_PATH,  NV_DLOAD_SEC_NAME},
                                          };
static struct nv_flash_global_ctrl_stru g_boot_file_info;
static u32 g_nvInitFlag = 0;




/*flash ops part*/
/**************************************************************/
void nv_fastboot_debug(u32 line,u32 ret,u32 reserved1,u32 reserved2)
{
	  //lint --e{835}
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    ddr_info->fb_debug.line = line;
    ddr_info->fb_debug.ret = ret;
    ddr_info->fb_debug.reseverd1 = reserved1;
    ddr_info->fb_debug.reseverd2 = reserved2;
}


/**
 * Name			:	bsp_nand_read
 * Arguments	:	@partition_name		- partition name
 *					@partition_offset	- offset from partition to read from
 *					@ptr_ram_addr	- pointer to ram addr to store the data read from flash
 *					@length		- number of bytes to read
 * Return		:	0				- success
 *				:   else			- failure
 * Desciption	:	length must be block aligned
 */
u32 bsp_nand_read(const char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length,u32* skip_len)
{
    s32 ret;
    struct ptentry *p;
    u32 block_off;  /*offset in the emmc block*/
    u32 sec_off;
    u8* temp_ram_addr = (u8*)ptr_ram_addr;
    static u8* block_data = NULL;
    u32 len;
    u32 byte;

	//lint --e{605}
    p = find_ptn(partition_name);
    if(p == NULL)
    {
        cprintf("ERROR: bsp_nand_read can't find partition %s\n", partition_name);
        return 1;
    }

    if(!block_data)
    {
        block_data = (u8*)alloc(EMMC_BLOCK_SIZE);/*lint !e718 !e746*/
        if(!block_data)
            return 2;
    }
    memset(block_data,0,EMMC_BLOCK_SIZE);

    block_off = partition_offset%EMMC_BLOCK_SIZE; /*当前块中的偏移*/
    sec_off   = partition_offset - block_off;  /*当前分区偏移*/


    byte = EMMC_BLOCK_SIZE;
    ret = flash_read(p,sec_off,block_data,&byte);
    if(ret)
        return (u32)ret;
    len = EMMC_BLOCK_SIZE - block_off;/*读取的数据在当前块中的长度*/

    if(len >= length)
    {
        len = length;
        memcpy(temp_ram_addr,block_data+block_off,(s32)len);
    }
    else
    {
        memcpy(temp_ram_addr,block_data+block_off,(s32)len);
        temp_ram_addr += len;   /*buff 偏移*/

        length  -= len;/*剩余数据长度*/
        byte = EMMC_BLOCK_SIZE;

        sec_off += EMMC_BLOCK_SIZE;/*物理偏移后移*/

        while(length)
        {
            /*每次读取需要的数据长度*/
            len = (length >= EMMC_BLOCK_SIZE) ? EMMC_BLOCK_SIZE :length;

            ret = flash_read(p,sec_off,block_data,&byte);
            if(ret){
                cprintf("%s %d, sec_off :0x%x\n",__func__,__LINE__,sec_off);
                return (u32)ret;
            }

            /*拷贝需要的长度数据到缓冲区中*/
            memcpy(temp_ram_addr, block_data,(s32)len);

            /*剩余长度减需要的长度*/
            length -= len;

            /*buffer指针后移*/
            temp_ram_addr += len;

            /*物理偏移地址移动一个block*/
            sec_off += EMMC_BLOCK_SIZE;
        }
#if 0
        byte = (byte/EMMC_BLOCK_SIZE + 1)*EMMC_BLOCK_SIZE;/*长度必须block对齐*/
        sec_off += EMMC_BLOCK_SIZE;

        ret = flash_read(p,sec_off,temp_ram_addr, &byte);
        if(ret)
            return ret;
#endif
    }
    if(skip_len)
        *skip_len = 0;
    return 0;

}

/**
 * Name			:	bsp_nand_isbad
 * Arguments	:	@partition_name		- partition name
 *					@partition_offset	- block offset from partition to check
 *
 * Return		:	0				- good block
 *				:   1			    - bad block
 *              :   -1              - error
 * Description	:	check whether a block is bad
 */
u32 bsp_nand_isbad(const char *partition_name, u32 partition_offset)
{
    /* eMMC doesn't need check bad */
	return 0;
}


/**
 * Name			:	bsp_get_nand_info
 * Arguments	:	@spec		    - to store flash info
 * Return		:	0				- success
 *				:   else			- failure
 * Desciption	:	adapt for nv in k3v3
 */
u32 bsp_get_nand_info(struct nand_spec *spec)
{
	spec->pagesize			= 0x1;
	spec->pagenumperblock	= 0;
	spec->addrnum			= 0;
	spec->ecctype			= 0;
	spec->buswidth			= 0;
    spec->blocksize         = 0x200;          /* 512byte */
    spec->sparesize         = 0;
    spec->chipsize          = 0;

	return 0;
}

/**
 * Name			:	bsp_nand_blk_write_nv
 * Arguments	:	@partition_name		- partition name
 *					@partition_offset   - offset from partition to read from, must be block aligned.
 *					@flag		- flag value. NV_FLAG means write successful last time, else failure
 * Return		:	0				- success
 *				:   else			- failure
 * Description	:	read nv_flag of a block
 */
unsigned int bsp_nand_read_flag_nv(const char *partition_name, unsigned int partition_offset, unsigned char *flag)
{

    if((!flag)||(!partition_name))
        return 1;

    *flag = NV_WRITE_SUCCESS;

    return 0;

}

/*flash ops part*/
/**************************************************************/
u32 nv_boot_get_emmc_file_len\
    (struct nv_ctrl_file_info_stru* ctrl_info,struct nv_file_list_info_stru* file_info,u32 * file_len)
{
    u32 i;
    *file_len = ctrl_info->ctrl_size;

	//lint --e{605}
    for(i = 0;i<ctrl_info->file_num;i++)
    {
        *file_len += file_info->file_size;
        file_info ++;
    }
    if(*file_len > NV_MAX_FILE_SIZE)  /*文件不能超过分区长度*/
    {
        printf("[%s]:file len 0x%x,max file size 0x%x\n",__func__,*file_len,NV_MAX_FILE_SIZE);
        return NV_ERROR;
    }
    return NV_OK;
}

/*
 * count bad block num,bewteen sec head to off in this sec
 */
u32 nv_boot_sec_off_count(const s8* name,u32 vir_off,u32* phy_off)
{
    *phy_off = vir_off;
    return NV_OK;
}


/*
 * 读nand接口
 * name      :   sec name
 * off      :   loggic offset in this file,need
 * len      :   data len write to flash ,len <= mtd->erasesize
 * ptr      :   the data need to write
 */
u32 nv_boot_read_emmc(const s8* name,FSZ off,u32 len,u8* ptr)
{
    u32 ret;
    /*传进来的偏移相对于文件头的逻辑偏移*/
    u32 offset;
    ret = nv_boot_sec_off_count(name,off,&offset);
    if(ret != NAND_OK)
    {
        printf("[nv]:<%s>:file off 0x%x,sec off 0x%x\n",__func__,off,offset);
        return ret;
    }
    return bsp_nand_read((char*)name,offset,ptr,len,NULL);
}
u32 nv_boot_dload_info_init(void)
{
    u32 ret;

    ret = bsp_nand_read((char*)NV_DLOAD_SEC_NAME,0,&g_boot_file_info.nv_dload,sizeof(g_boot_file_info.nv_dload),NULL);
    if(ret)
    {
        nv_fastboot_debug(__LINE__,ret,0,0);
        return ret;
    }
    return NV_OK;

}
/*lint -save -e18*/
u32 nv_boot_sec_file_info_init(const s8* name,struct nv_file_info_stru* sec_info)
{
    u32 ret = NV_ERROR;
    u32 file_len = 0;
    struct nv_file_info_stru info;
    struct nv_ctrl_file_info_stru ctrl_info;
    u8* file_info;

    /*first: read nv ctrl file*/
    ret = bsp_nand_read((char*)name,0,&ctrl_info,sizeof(ctrl_info),NULL);
    if(NAND_OK != ret)
    {
        printf("[%s]:patrition name %s,get file magic fail ret 0x%x,\n",__func__,name,ret);
        return ret;
    }

    /*second :check magic num in file head*/
    if(ctrl_info.magicnum != NV_CTRL_FILE_MAGIC_NUM)
    {
        printf("[%s]:enter this way  1111! %s\n",__func__,name);
        return NV_OK;
    }

    /*third: read all nv ctrl file*/
    file_info = (u8*)nv_malloc(ctrl_info.file_size+EMMC_BLOCK_SIZE);
    if(NULL == file_info)
    {
        printf("[%s]:enter this way  2222!\n",__func__);
        return BSP_ERR_NV_MALLOC_FAIL;
    }
    ret = bsp_nand_read((char*)name,sizeof(struct nv_ctrl_file_info_stru),file_info,ctrl_info.file_size,NULL);
    if(NAND_OK != ret)
    {
        printf("[%s]:enter this way 3333!\n",__func__);
        goto init_end;
    }

    /*fourth: count nv file len base the ctrl file info*/
    ret = nv_boot_get_emmc_file_len(&ctrl_info,(struct nv_file_list_info_stru*)file_info,&file_len);
    if(ret)
    {
        printf("[%s]:enter this way 4444!\n",__func__);
        goto init_end;
    }


    info.len       = file_len;
    info.magic_num = NV_FILE_EXIST;
    info.off       = 0;


    memcpy(sec_info,&info,sizeof(info));
init_end:
    nv_free(file_info);
    return NV_OK;
}
/*lint -restore*/

u32 nv_boot_get_sys_nv_info(const s8* mode,u32* offset,u32* len)
{
    s32 ret = strcmp((const s8*)NV_FILE_READ,mode);
    if(B_READ  == ret)
    {
        if(g_boot_file_info.sys_nv.magic_num != NV_FILE_EXIST)
        {
            return NV_ERROR;
        }
        *offset = g_boot_file_info.sys_nv.off;
        *len    = g_boot_file_info.sys_nv.len;
        return NV_OK;
    }
    if(B_WRITE == ret)
    {
        *offset = g_boot_file_info.sys_nv.off;
        *len    = g_boot_file_info.sys_nv.len;
        return NV_OK;
    }
    return NV_ERROR;

}
u32 nv_boot_get_dload_nv_info(u32* offset,u32* len)
{
    if(g_boot_file_info.nv_dload.nv_bin.magic_num == NV_FILE_EXIST)
    {
        *offset = g_boot_file_info.nv_dload.nv_bin.off;
        *len    = g_boot_file_info.nv_dload.nv_bin.len;
        return NV_OK;
    }
    else
    {
        return NV_ERROR;
    }
}

u32 nv_boot_get_bak_file_info(const s8* mode,u32* offset,u32* len)
{
    s32 ret = strcmp((const s8*)NV_FILE_READ,mode);
    if(B_READ  == ret)
    {
        if(g_boot_file_info.bak_info.magic_num!= NV_FILE_EXIST)
        {
            return NV_ERROR;
        }
        *offset = g_boot_file_info.bak_info.off;
        *len    = g_boot_file_info.bak_info.len;
        return NV_OK;
    }
    if(B_WRITE == ret)
    {
        *offset = g_boot_file_info.bak_info.off;
        *len    = g_boot_file_info.bak_info.len;
        return NV_OK;
    }
    return NV_ERROR;

}

u32 nv_boot_get_xnv_info(u32 card_type,u32* offset,u32* len)
{
    if(g_boot_file_info.nv_dload.xnv_xml[card_type-1].magic_num == NV_FILE_EXIST)
    {
        *offset = g_boot_file_info.nv_dload.xnv_xml[card_type-1].off;
        *len    = g_boot_file_info.nv_dload.xnv_xml[card_type-1].len;
        return NV_OK;
    }
    else
    {
        return NV_ERROR;
    }
}
u32 nv_boot_get_xnv_map_info(u32 card_type,u32* offset,u32* len)
{
    if(g_boot_file_info.nv_dload.xnv_map[card_type-1].magic_num == NV_FILE_EXIST)
    {
        *offset = g_boot_file_info.nv_dload.xnv_map[card_type-1].off;
        *len    = g_boot_file_info.nv_dload.xnv_map[card_type-1].len;
        return NV_OK;
    }
    else
    {
        return NV_ERROR;
    }
}
u32 nv_boot_get_cust_info(u32 card_type,u32* offset,u32* len)
{
    if(g_boot_file_info.nv_dload.cust_xml[card_type-1].magic_num== NV_FILE_EXIST)
    {
        *offset = g_boot_file_info.nv_dload.cust_xml[card_type-1].off;
        *len    = g_boot_file_info.nv_dload.cust_xml[card_type-1].len;
        return NV_OK;
    }
    else
    {
        return NV_ERROR;
    }
}


u32 nv_emmc_init(void)
{
    u32 ret;

    memset((void*)&g_boot_file_info,0,sizeof(struct nv_flash_global_ctrl_stru));
    /*get dload info*/
    ret = nv_boot_dload_info_init();
    if(ret)
    {
        printf("[%s]:get dload info fail need to check this sec!\n",__func__);
    }

    /*get bak info*/
    ret = nv_boot_sec_file_info_init(g_boot_nv_file[NV_FILE_BACKUP].name,&g_boot_file_info.bak_info);
    if(ret)
    {
        printf("[%s]:get bak info fail need to check this sec!\n",__func__);
    }

    /*get sys info*/
    ret = nv_boot_sec_file_info_init(g_boot_nv_file[NV_FILE_SYS_NV].name,&g_boot_file_info.sys_nv);
    if(ret)
    {
        printf("[%s]:get sys nv info fail need to check this sec!\n",__func__);
    }
    return NV_OK;
}

FILE* nv_emmc_open(const s8* path,const s8* mode)
{
    u32 ret = NV_ERROR;
    u32 i = 0;
    struct nv_flash_file_header_stru* fd = NULL;
    u32 offset = 0;
    u32 len = 0;
    for(i=0; i<NV_FILE_BUTT; i++)
    {
        if(0 == strcmp(path,g_boot_nv_file[i].path))
        {
            fd = &g_boot_nv_file[i];
            break;
        }
    }
    if(NULL == fd)
    {
        return NULL;
    }
    switch(fd->flash_type)
    {
        case NV_FILE_DLOAD:
            ret = nv_boot_get_dload_nv_info(&offset,&len);
            break;
        case NV_FILE_BACKUP:
            ret = nv_boot_get_bak_file_info(mode,&offset,&len);
            break;
        case NV_FILE_XNV_CARD_1:
            ret = nv_boot_get_xnv_info(NV_USIMM_CARD_1,&offset,&len);
            break;
        case NV_FILE_CUST_CARD_1:
            ret = nv_boot_get_cust_info(NV_USIMM_CARD_1,&offset,&len);
            break;
        case NV_FILE_XNV_CARD_2:
            ret = nv_boot_get_xnv_info(NV_USIMM_CARD_2,&offset,&len);
            break;
        case NV_FILE_CUST_CARD_2:
            ret = nv_boot_get_cust_info(NV_USIMM_CARD_2,&offset,&len);
            break;
        case NV_FILE_SYS_NV:
            ret = nv_boot_get_sys_nv_info(mode,&offset,&len);
            break;
        case NV_FILE_XNV_MAP_CARD_1:
            ret = nv_boot_get_xnv_map_info(NV_USIMM_CARD_1,&offset,&len);
            break;
        case NV_FILE_XNV_MAP_CARD_2:
            ret = nv_boot_get_xnv_map_info(NV_USIMM_CARD_2,&offset,&len);
            break;
        default:
            ret = BSP_ERR_NV_INVALID_PARAM;
            break;
    }

    if(NV_OK != ret)
    {
        return NULL;
    }

    fd->ops ++;
    fd->seek   = 0;
    fd->length = len;
    fd->off    = offset;
    fd->fp     = fd;

    return fd;
}

s32 nv_emmc_read(u8* ptr, u32 size, u32 count, FILE* fp)
{
    u32 real_size;
    u32 ret;
    struct nv_flash_file_header_stru* fd = (struct nv_flash_file_header_stru*)fp;
    u32 len = size*count;

    if((NULL == fd)||(fd->fp != fd)||(fd->ops == 0))
    {
        return BSP_ERR_NV_INVALID_PARAM;
    }

    real_size = ((fd->seek+len) < fd->length)? len: (fd->length - fd->seek );

    ret = nv_boot_read_emmc((char*)fd->name,(fd->off+fd->seek),real_size,ptr);/*注意文件偏移*/
    if(ret != NAND_OK)
    {
        printf("[%s]:%d ret 0x%x\n",__func__,__LINE__,ret);
        return -1;
    }
    fd->seek += real_size;
    return (s32)real_size;
}
s32 nv_emmc_seek(FILE* fp,s32 offset,s32 whence)
{
    u32 ret;
    struct nv_flash_file_header_stru* fd = (struct nv_flash_file_header_stru*)fp;
    if((NULL == fd)||(fd->fp != fd)||(fd->ops == 0))
    {
        return BSP_ERR_NV_INVALID_PARAM;
    }
    ret = fd->seek;
    switch(whence)
    {
        case SEEK_SET:
            ret = (u32)offset;
            break;
        case SEEK_CUR:
            ret += (u32)offset;
            break;
        case SEEK_END:
            ret = fd->length -(u32)offset;
            break;
        default:
            return BSP_ERR_NV_INVALID_PARAM;
    }
    if(ret > fd->length)
    {
        return -1;
    }

    fd->seek = ret;
    return NV_OK;
}

s32 nv_emmc_close(FILE* fp)
{
    struct nv_flash_file_header_stru* fd = (struct nv_flash_file_header_stru*)fp;

    if((NULL == fd)||(fd->fp != fd)||(fd->ops == 0))
    {
        return BSP_ERR_NV_INVALID_PARAM;
    }

    fd->fp = NULL;
    fd->seek = 0;
    fd->length = 0;
    fd->off = 0;
    fd->ops --;
    if(fd->ops != 0)
    {
        return BSP_ERR_NV_CLOSE_FILE_FAIL;
    }
    return NV_OK;
}

s32 nv_emmc_ftell(FILE* fp)
{
    struct nv_flash_file_header_stru* fd = (struct nv_flash_file_header_stru*)fp;

    if((NULL == fd)||(fd->fp != fd)||(fd->ops == 0))
    {
        return BSP_ERR_NV_INVALID_PARAM;
    }
    return (s32)(fd->seek);
}

s32 nv_emmc_access(s8* path,s32 mode)
{
    u32 ret = NV_ERROR;
    u32 i = 0;
    struct nv_flash_file_header_stru* fd = NULL;


    for(i=0; i<NV_FILE_BUTT; i++)
    {
        if(0 == strcmp(path,g_boot_nv_file[i].path))
        {
            fd = &g_boot_nv_file[i];
            break;
        }
    }
    if(NULL == fd)
    {
        return -1;
    }
    mode = mode;
    switch(fd->flash_type)
    {
        case NV_FILE_DLOAD:
            ret = (g_boot_file_info.nv_dload.nv_bin.magic_num == NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        case NV_FILE_BACKUP:
            ret = (g_boot_file_info.bak_info.magic_num== NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        case NV_FILE_XNV_CARD_1:
            ret = (g_boot_file_info.nv_dload.xnv_xml[0].magic_num == NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        case NV_FILE_XNV_CARD_2:
            ret = (g_boot_file_info.nv_dload.xnv_xml[1].magic_num == NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        case NV_FILE_CUST_CARD_1:
            ret = (g_boot_file_info.nv_dload.cust_xml[0].magic_num == NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        case NV_FILE_CUST_CARD_2:
            ret = (g_boot_file_info.nv_dload.cust_xml[1].magic_num == NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        case NV_FILE_SYS_NV:
            ret = (g_boot_file_info.sys_nv.magic_num == NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        case NV_FILE_XNV_MAP_CARD_1:
            ret = (g_boot_file_info.nv_dload.xnv_map[0].magic_num == NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        case NV_FILE_XNV_MAP_CARD_2:
            ret = (g_boot_file_info.nv_dload.xnv_map[1].magic_num == NV_FILE_EXIST)?NV_FILE_EXIST:0;
            break;
        default:
            return -1;
    }
    if(ret != NV_FILE_EXIST)
    {
        return -1;
    }
    return 0;
}

/*flash part end*/
/**************************************************************/

/*
 * pick up the base info from the major info,then reg in base_info
 */
u32 nv_boot_init_file_info(u8* major_info,u8* base_info)
{
    u32 i;
    u32 ret;
    struct nv_ctrl_file_info_stru * ctrl_file = (struct nv_ctrl_file_info_stru*)major_info;
    struct nv_global_ddr_info_stru* ddr_info  = (struct nv_global_ddr_info_stru*)base_info;
    struct nv_file_list_info_stru * file_info = (struct nv_file_list_info_stru *)((u8*)ctrl_file+NV_GLOBAL_CTRL_INFO_SIZE);

    ddr_info->file_num = ctrl_file->file_num;   /*reg file num*/
    ddr_info->file_len = ctrl_file->ctrl_size;  /*reg ctrl file size,then add file size*/

    for(i = 0;i<ctrl_file->file_num;i++)
    {
        /*check file id*/
        if((i+1) != file_info->file_id)
        {
            ret = BSP_ERR_NV_FILE_ERROR;
            nv_fastboot_debug(__LINE__,ret,i,file_info->file_id);
            return ret;
        }
        ddr_info->file_info[i].file_id = file_info->file_id;
        ddr_info->file_info[i].size    = file_info->file_size;
        ddr_info->file_info[i].offset  = ddr_info->file_len;

        ddr_info->file_len            += file_info->file_size;

        file_info++;
    }
    if(ddr_info->file_len > NV_MAX_FILE_SIZE)
    {
        ret = BSP_ERR_NV_FILE_ERROR;
        nv_fastboot_debug(__LINE__,ret,0,ddr_info->file_len);
        return ret;
    }
    return NV_OK;
}

bool nv_boot_dload_file_check(void )
{
    u32 i;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    struct nv_file_list_info_stru* file_info = (struct nv_file_list_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE);
    struct nv_ref_data_info_stru* ref_info   = (struct nv_ref_data_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE\
        +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num);

    struct nv_ref_data_info_stru* ref_info_next = ref_info+1;

    /*check ref list id sort */
    for(i = 0;i<ctrl_info->ref_count-1;i++)
    {
        if(ref_info->itemid >=ref_info_next->itemid)
        {
            nv_fastboot_debug(__LINE__,i,ref_info->itemid,ref_info_next->itemid);
            return FALSE;
        }
        ref_info ++;
        ref_info_next ++;
    }

    /*check file id sort*/
    for(i = 0;i<ctrl_info->file_num;i++)
    {
        if(file_info->file_id != (i+1))
        {
            nv_fastboot_debug(__LINE__,i,file_info->file_id,0);
            return FALSE;
        }
        file_info ++;
    }
    return TRUE;
}

/*
 * get file len
 * return : file len
 */
u32 nv_boot_get_file_len(FILE* fp)
{
    s32 ret;
    u32 seek;


    ret = nv_emmc_seek(fp,0,SEEK_END);
    if(ret)
    {
        goto out;
    }
    seek = (u32)nv_emmc_ftell(fp);

    ret = nv_emmc_seek(fp,0,SEEK_SET);
    if(ret)
    {
        goto out;
    }

    return seek;

out:
    printf("[%s]:ret 0x%x\n",__func__,ret);
    return NV_ERROR;
}

/*
 * search nv info by nv id
 * &pdata:  data start ddr
 * output: ref_info,file_info
 */
u32 nv_boot_search_byid(u32 itemid,u8* pdata,struct nv_ref_data_info_stru* ref_info,struct nv_file_list_info_stru* file_info)
{
    u32 low;
    u32 high;
    u32 mid;
    u32 offset;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)pdata;

    high = ctrl_info->ref_count;
    low  = 1;
    while(low <= high)
    {
        mid = (low+high)/2;

        offset =NV_GLOBAL_CTRL_INFO_SIZE +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num + (mid -1)*NV_REF_LIST_ITEM_SIZE;
        memcpy((u8*)ref_info,(u8*)ctrl_info+offset,NV_REF_LIST_ITEM_SIZE);
        if(itemid < ref_info->itemid)
        {
            high = mid-1;
        }
        else if(itemid > ref_info->itemid)
        {
            low = mid+1;
        }
        else
        {
            offset = NV_GLOBAL_CTRL_INFO_SIZE + NV_GLOBAL_FILE_ELEMENT_SIZE*(ref_info->file_id -1);
            memcpy((u8*)file_info,(u8*)ctrl_info+offset,NV_GLOBAL_FILE_ELEMENT_SIZE);
            return NV_OK;
        }
    }
    return BSP_ERR_NV_NO_THIS_ID;

}

u32 nv_boot_read_from_mem(u8* pdata,u32 size,u32 file_id,u32 offset)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    if(offset > ddr_info->file_info[file_id-1].size)
    {
        printf("[%s]:offset 0x%x\n",__func__,offset);
        return BSP_ERR_NV_FILE_ERROR;
    }
    memcpy(pdata,(u8*)(NV_GLOBAL_CTRL_INFO_ADDR +ddr_info->file_info[file_id-1].offset + offset),(s32)size);

    return NV_OK;
}

/*
 * copy user buff to global ddr,used to write nv data to ddr
 * &file_id :file id
 * &offset:  offset of global file ddr
 */
u32 nv_boot_write_to_mem(u8* pdata,u32 size,u32 file_id,u32 offset)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    if(offset > ddr_info->file_info[file_id-1].size)
    {
        printf("[%s]:offset 0x%x\n",__FUNCTION__,offset);
        return BSP_ERR_NV_FILE_ERROR;
    }
    memcpy((u8*)(NV_GLOBAL_CTRL_INFO_ADDR+ddr_info->file_info[file_id-1].offset + offset),pdata,(s32)size);

    return NV_OK;
}
/*
 * read file to ddr,include download,backup,workaround,default
 */
u32 nv_boot_read_from_emmc(FILE* fp,u8* ptr,u32* datalen)
{
    u32 ret;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    *datalen = nv_boot_get_file_len(fp);

    if((*datalen > NV_MAX_FILE_SIZE)||(*datalen == 0))
    {
        printf("%s,%d datalen :0x%x\n",__func__,__LINE__,*datalen);
        ret = BSP_ERR_NV_FILE_ERROR;
        goto out;
    }

    ret = (u32)nv_emmc_read((u8*)ptr,1,(*datalen),fp);
    if(ret != (*datalen))
    {
        printf("%s,%d ret :0x%x,datalen :0x%x\n",__func__,__LINE__,ret,*datalen);
        goto out;
    }
    //lint --e{835}
    ret = nv_boot_init_file_info((u8*)NV_GLOBAL_CTRL_INFO_ADDR,(u8*)NV_GLOBAL_INFO_ADDR);
    if(ret)
    {
        printf("%s,%d ret :0x%x\n",__func__,__LINE__,ret);
        goto out;
    }
    if(*datalen != ddr_info->file_len)
    {
        printf("%s,%d datalen :0x%x,ddr file len :0x%x\n",__func__,__LINE__,datalen,ddr_info->file_len);
        nv_fastboot_debug(__LINE__,*datalen,ddr_info->file_len,0);
        ret = BSP_ERR_NV_FILE_ERROR;
        goto out;
    }

    return NV_OK;
out:
    printf("[%s]:ret 0x%x\n",__func__,ret);
    return NV_ERROR;
}
/*lint -e14*/
u32 bsp_nvm_read(u32 itemid, u8 * pdata, u32 datalen)
{
    u32 ret;
    struct nv_file_list_info_stru file_info;
    struct nv_ref_data_info_stru  ref_info;
    struct nv_global_ddr_info_stru *ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    if((NULL == pdata)||(0 == datalen))
    {
        return BSP_ERR_NV_INVALID_PARAM;
    }
    if(!g_nvInitFlag)
    {
        printf("nv no init success,can not read! %d\n",ddr_info->acore_init_state);
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }
    //lint --e{835}
    ret = nv_boot_search_byid(itemid,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info,&file_info);
    if(ret)
    {
        nv_fastboot_debug(__LINE__,ret,0,0);
        goto nv_read_err;
    }

    if(datalen > (u32)ref_info.nv_len)
    {
        ret = BSP_ERR_NV_ITEM_LEN_ERR;
        nv_fastboot_debug(__LINE__,ret,datalen,ref_info.nv_len);
        goto nv_read_err;
    }

    ret = nv_boot_read_from_mem(pdata,datalen,file_info.file_id,ref_info.nv_off);
    if(ret)
    {
        ret = BSP_ERR_NV_READ_DATA_FAIL;
        nv_fastboot_debug(__LINE__,ret,file_info.file_id,ref_info.nv_off);
        goto nv_read_err;
    }

    return NV_OK;

nv_read_err:
   return ret;
}

u32 nv_boot_xml_decode(s8* path,s8* map_path,u32 card_type)
{
    u32 ret = NV_ERROR;
    FILE* fp = NULL;

    cprintf("Decode %s start !\n",path);
    fp = nv_emmc_open(path,(s8*)NV_FILE_READ);
    if(!fp)
    {
        ret = BSP_ERR_NV_NO_FILE;
        return ret;
    }
    ret = xml_decode_main(fp,map_path,card_type);
    nv_emmc_close(fp);
    if(ret)
        return ret;

    return NV_OK;
}


void modem_nv_xml_decode(void)
{
    u32 ret = NV_ERROR;
    struct nv_global_ddr_info_stru *ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    cprintf("modem xnv xml is decoding,waiting for a moment ....\n");

    if(!nv_emmc_access(NV_XNV_CARD1_PATH,0))
    {
        ret = nv_boot_xml_decode(NV_XNV_CARD1_PATH,NV_XNV_CARD1_MAP_PATH,NV_USIMM_CARD_1);
        if(ret)
        {
            return;
        }
    }

    if(!nv_emmc_access(NV_XNV_CARD2_PATH,0))
    {
        ret = nv_boot_xml_decode(NV_XNV_CARD2_PATH,NV_XNV_CARD2_MAP_PATH,NV_USIMM_CARD_2);
        if(ret)
        {
            return;
        }
    }


    /*CUST XML 无对应MAP文件，传入空值即可*/
    if(!nv_emmc_access(NV_CUST_CARD1_PATH,0))
    {
        ret = nv_boot_xml_decode(NV_CUST_CARD1_PATH,NULL,NV_USIMM_CARD_1);
        if(ret)
        {
            return;
        }
    }

    if(!nv_emmc_access(NV_CUST_CARD2_PATH,0))
    {
        ret = nv_boot_xml_decode(NV_CUST_CARD2_PATH,NULL,NV_USIMM_CARD_2);
        if(ret)
        {
            return;
        }
    }
    ddr_info->xml_dec_state = NV_XML_DEC_SUCC_STATE;
    return ;
}
/*lint -e14*/
u32 modem_nv_init(void)
{
    u32 ret;
    u32 datalen = 0;
    FILE* fp = NULL;
    u32 mem_type = 0;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    printf("[%s],modem nv addr :0x%x\n",__func__,NV_GLOBAL_INFO_ADDR);
    memset(ddr_info,0,sizeof(struct nv_global_ddr_info_stru));

    ret = nv_emmc_init();
    if(ret)
    {
        printf("[%s]:%d \n",__func__,__LINE__);
        goto nv_boot_init_fail;
    }

    fp = nv_emmc_open((s8*)NV_DLOAD_PATH,(s8*)NV_FILE_READ);
    if(fp)
    {
	    //lint --e{835}
        ret = nv_boot_read_from_emmc(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
        nv_emmc_close(fp);
        if(ret)
        {
            ret = BSP_ERR_NV_READ_FILE_FAIL;
            goto nv_boot_init_fail;
        }

        if(FALSE == nv_boot_dload_file_check())
        {
            ret = BSP_ERR_NV_FILE_ERROR;
            goto nv_boot_init_fail;
        }

        /*fastboot xml decode ,err case */
        modem_nv_xml_decode();

        /*reg dload file in using*/
        mem_type    = NV_MEM_DLOAD;
    }
    else
    {
        fp = nv_emmc_open((s8*)NV_SYS_NV_PATH,(s8*)NV_FILE_READ);
        if(NULL == fp)
        {
            fp= nv_emmc_open((s8*)NV_BACK_PATH,(s8*)NV_FILE_READ);
            if(NULL == fp)
            {
                nv_fastboot_debug(__LINE__,ret,0,0);
                goto nv_boot_init_fail;
            }
            else
            {
                mem_type = NV_MEM_BACKUP;
            }
        }
        else
        {
            mem_type = NV_MEM_SYSTEM;
        }
        //lint --e{835}
        ret = nv_boot_read_from_emmc(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
        (void)nv_emmc_close(fp);
        fp = NULL;
        if(ret)
        {
            ret = BSP_ERR_NV_READ_FILE_FAIL;
            goto nv_boot_init_fail;
        }
    }

    ddr_info->acore_init_state = NV_BOOT_INIT_OK;
    ddr_info->mcore_init_state = NV_BOOT_INIT_OK;
    ddr_info->ccore_init_state = NV_BOOT_INIT_OK;
    ddr_info->mem_file_type    = mem_type;
    g_nvInitFlag = 1;
    printf("balong nv init ok!\n");
    return NV_OK;
nv_boot_init_fail:
    ddr_info->acore_init_state = NV_BOOT_INIT_FAIL;
    ddr_info->mcore_init_state = NV_BOOT_INIT_FAIL;
    ddr_info->ccore_init_state = NV_BOOT_INIT_FAIL;
    printf("balong nv init err! ret :0x%x\n",ret);
    return NV_ERROR;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

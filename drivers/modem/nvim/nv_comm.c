

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -save -e322 -e7 -e537*/
#include <stdarg.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <drv_rfile.h>
#include <bsp_hardtimer.h>
#include "nv_comm.h"
#include "nv_file.h"
#include "nv_ctrl.h"
#include "nv_xml_dec.h"
#include "bsp_dump.h"
#include "NVIM_ResumeId.h"
/*lint -restore +e322 +e7 +e537*/

/*lint -save -e438 -e530 -e713 -e830 -e529*/
static const u32 nv_mid_droit[NV_MID_PRI_LEVEL_NUM] = {1,1,1,1,1,1};

struct nv_global_ctrl_info_stru  g_nv_ctrl;
struct nv_global_debug_stru      g_nv_debug[NV_FUN_MAX_ID];

u16 g_NvSysList[] = { NV_ID_DRV_IMEI,
                      NV_ID_DRV_NV_FACTORY_INFO_I,
                      NV_ID_DRV_NV_DRV_VERSION_REPLACE_I,
                      NV_ID_DRV_SEC_BOOT_ENABLE_FLAG,
                      NV_ID_DRV_SOCP_LOG_CFG};

/*nv debug func,reseverd1 used to reg branch*/
void nv_debug(u32 type,u32 reseverd1,u32 reserved2,u32 reserved3,u32 reserved4)
{
    if(0 == reseverd1)
    {
        g_nv_debug[type].callnum++;
    }
    g_nv_debug[type].reseved1 = reseverd1;
    g_nv_debug[type].reseved2 = reserved2;
    g_nv_debug[type].reseved3 = reserved3;
    g_nv_debug[type].reseved4 = reserved4;
}


/*系统启动log记录接口，保存到 NV_LOG_PATH 中，大小限定在 NV_LOG_MAX_SIZE*/
void nv_mntn_record(char* fmt,...)
{
    char   buffer[256];
    va_list arglist;
    FILE* fp = NULL;
    int ret = 0;
    int file_len;

    /*lint -save -e530*/
    va_start(arglist, fmt);
    /*lint -restore +e530*/
    vsnprintf(buffer,256, fmt, arglist);/* [false alarm]:format string */
    va_end(arglist);

    printk("%s",buffer);

    if(BSP_access((char*)NV_LOG_PATH,0))
    {
        fp = BSP_fopen((char*)NV_LOG_PATH,"w+");
        if(!fp)
            return;
    }
    else
    {
        fp = BSP_fopen((char*)NV_LOG_PATH,"r+");
        if(!fp)
            return;
        BSP_fseek(fp,0,SEEK_END);
        file_len = BSP_ftell(fp);
        /*lint -save -e737*/
        if((file_len+strlen(buffer))>= NV_LOG_MAX_SIZE)
        {
            BSP_fclose(fp);
            fp = BSP_fopen((char*)NV_LOG_PATH,"w+");
            if(!fp)
                return;
        }
    }
    ret = BSP_fwrite(buffer,1,(unsigned int)strlen(buffer),fp);
    if(ret != strlen(buffer))
    {
        printf("BSP_fwrite   nv   log err!  ret :0x%x buffer len :0x%x\n",ret,strlen(buffer));
    }
    /*lint -restore +e737*/
    BSP_fclose(fp);
}
/*创建flag标志文件,数据写入之前调用*/
void nv_create_flag_file(const s8* path)
{
    FILE* fp = NULL;

    if(!BSP_access(path,0))
        return;
    fp = BSP_fopen((char*)path, (char*)NV_FILE_WRITE);

    if(fp){
        BSP_fclose(fp);
        return;
    }
    else
        return;
}

/*删除flag标志文件，数据写完之后调用*/
void nv_delete_flag_file(const s8* path)
{
    if(BSP_access((char*)path,0))
        return;
    else
        BSP_remove((char*)path);
}

/*判断标志文件是否存在 true :存在， false :不存在*/
bool nv_flag_file_isExist(const s8* path)
{
    return (BSP_access((char*)path,0) == 0)?true:false;
}

/*启动之后如果备份区与系统分区flag文件存在，则需要将数据重新写入对应分区中*/
void nv_file_flag_check(void)
{
    if( !BSP_access((char*)NV_BACK_FLAG_PATH,0)){
        nv_mntn_record("%s %s :last time [back file] write abnomal,rewrite !\n",__DATE__,__TIME__);
        (void)bsp_nvm_backup();
    }

    if( !BSP_access((char*)NV_SYS_FLAG_PATH,0)){
        nv_mntn_record("%s %s :last time [sys file] write abnomal,rewrite !\n",__DATE__,__TIME__);
        (void)bsp_nvm_flushSys(NV_ERROR);
    }

    if( !BSP_access((char*)NV_IMG_FLAG_PATH,0)){
        nv_mntn_record("%s %s :last time [img file] write abnomal,rewrite !\n",__DATE__,__TIME__);
        (void)bsp_nvm_flushEn();
    }
}

/*升级或异常情况下将数据写入各个分区*/
u32 nv_data_writeback(void)
{
    u32 ret;

    ret = bsp_nvm_flushEn();
    if(ret)
    {
        nv_error_printf("write back to [img] fail! ret :0x%x\n",ret);
        return ret;
    }

    ret = bsp_nvm_backup();
    if(ret)
    {
        nv_error_printf("write back to [back] fail! ret :0x%x\n",ret);
        return ret;
    }

    ret = bsp_nvm_flushSys(NV_ERROR);
    if(ret)
    {
        nv_error_printf("write back to [system] fail! ret :0x%x\n",ret);
        return ret;
    }

    return NV_OK;
}

/*紧急将数据从工厂分区恢复*/
u32 nv_load_err_proc(void)
{
    u32 ret = NV_ERROR;
    FILE* fp = NULL;
    u32 datalen = 0;

    if(nv_file_access((s8*)NV_DLOAD_PATH,0))
    {
        nv_mntn_record("%s ,%s has no file\n",__func__,NV_DLOAD_PATH);
        return NV_ERROR;
    }

    nv_mntn_record("%s %s load from %s ...\n",__DATE__,__TIME__,NV_DLOAD_PATH);
    fp = nv_file_open((s8*)NV_DLOAD_PATH,(s8*)NV_FILE_READ);
    if(!fp)
    {
        nv_error_printf("open %s failed \n",NV_DLOAD_PATH);
        return BSP_ERR_NV_NO_FILE;
    }

    ret = nv_read_from_file(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
    nv_file_close(fp);
    if(ret)
    {
        return ret;
    }
    if(false == nv_dload_file_check())
    {
        return NV_ERROR;
    }

    ret = bsp_nvm_xml_decode();
    if(ret)
    {
        return ret;
    }
    /*如果出厂分区存在数据则将数据从出厂分区恢复*/
    if(!nv_file_access((s8*)NV_DEFAULT_PATH,0))
    {
        ret = nv_revertEx((s8*)NV_DEFAULT_PATH);
        if(ret)
        {
            return ret;
        }
    }
    ret = nv_data_writeback();
    if(ret)
    {
        return ret;
    }

    return NV_OK;
}


/*检查工作区数据的有效性*/
bool nv_check_img_validity(void)
{
    u32 ret;

    /*工作区文件不存在*/
    if(nv_file_access((s8*)NV_IMG_PATH,0))
    {
        return false;
    }
    /*工作区有未写入完成的标志 */
    if(true == nv_flag_file_isExist((s8*)NV_IMG_FLAG_PATH))
    {
        nv_mntn_record("%s  last time write abornormal !\n",NV_IMG_PATH);
        return false;
    }

    /*工作区imei号检查*/
    ret = nv_imei_data_comp((s8*)NV_IMG_PATH);
    if(ret)
    {
        nv_mntn_record("%s imei compare with factory data is not same ret :0x%x!\n",NV_IMG_PATH,ret);
        return false;
    }

    return true;
}

/*检查备份区数据的有效性*/
bool nv_check_backup_validity(void)
{
    u32 ret;

    /*备份区文件不存在*/
    if(nv_file_access((s8*)NV_BACK_PATH,0))
    {
        return false;
    }
    /*备份区有未写入完成的标志 */
    if(true == nv_flag_file_isExist((s8*)NV_BACK_FLAG_PATH))
    {
        nv_mntn_record("%s  last time write abornormal !\n",NV_BACK_PATH);
        return false;
    }

    /*备份区imei号检查*/
    ret = nv_imei_data_comp((s8*)NV_BACK_PATH);
    if(ret)
    {
        nv_mntn_record("%s imei compare with factory data is not same ret :0x%x!\n",NV_BACK_PATH,ret);
        return false;
    }

    return true;
}


/*需要支持正常升级的备份恢复项恢复及裸板升级*/
u32 nv_upgrade_revert_proc(void)
{
    u32 ret;

    /*检查工作区数据的有效性*/
    if(true == nv_check_img_validity())
    {
        ret = nv_revertEx((s8*)NV_IMG_PATH);
        if(ret)
        {
            nv_mntn_record("revert from %s fail,goto next err proc ret:0x%x!\n",NV_IMG_PATH,ret);
            goto revert_backup;
        }

        /*从工作区恢复完成之后，备份工作区数据到备份分区*/
        ret = nv_copy_img2backup();
        if(ret)/*拷贝异常直接退出*/
        {
            nv_mntn_record("copy img to backup fail,ret :0x%x\n",ret);
            return ret;
        }
        return NV_OK;
    }

revert_backup:
    /*检查备份区数据的有效性*/
    if(true == nv_check_backup_validity())
    {
        ret = nv_revertEx((s8*)NV_BACK_PATH);
        if(ret)
        {
            nv_mntn_record("revert from %s fail,goto next err proc ret:0x%x!\n",NV_BACK_PATH,ret);
            goto revert_factory;
        }

        return NV_OK;
    }

revert_factory:
    /*出厂分区有数据直接从出厂分区恢复*/
    if(!nv_file_access((s8*)NV_DEFAULT_PATH,0))
    {
        ret = nv_revertEx((s8*)NV_DEFAULT_PATH);
        if(ret)
        {
            nv_mntn_record("revert from %s fail,return err! ret:0x%x\n",NV_DEFAULT_PATH,ret);
            return ret;
        }

    }

    /*烧片版本无数据恢复，直接返回ok*/
    return NV_OK;
}
bool nv_check_update_default_right(void)
{
    /*不存在，则直接返回true*/
    if(nv_file_access((s8*)NV_DEFAULT_PATH,0))
    {
        return true;
    }
    /*存在允许写入的文件*/
    if(!BSP_access((char*)NV_FACTORY_RIGHT_PATH,0))
    {
        return true;
    }

    return false;
}

void nv_delete_update_default_right(void)
{
    if(!BSP_access((char*)NV_FACTORY_RIGHT_PATH,0))
    {
        (void)BSP_remove((char*)NV_FACTORY_RIGHT_PATH);
    }
}

/*
 * pick up the base info from the major info,then reg in base_info
 */
u32 nv_init_file_info(u8* major_info,u8* base_info)
{
    u32 i;
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
            printf("[%s]:file id  %d error ,i: %d\n",__FUNCTION__,file_info->file_id,i);
            return BSP_ERR_NV_FILE_ERROR;
        }
        ddr_info->file_info[i].file_id = file_info->file_id;
        ddr_info->file_info[i].size    = file_info->file_size;
        ddr_info->file_info[i].offset  = ddr_info->file_len;

        ddr_info->file_len            += file_info->file_size;

        file_info++;
    }
    if(ddr_info->file_len > NV_MAX_FILE_SIZE)
    {
        nv_mntn_record("[%s]: file len error 0x%x\n",__FUNCTION__,ddr_info->file_len);
        return BSP_ERR_NV_FILE_ERROR;
    }
    return NV_OK;

}

void nv_modify_print_sw(u32 arg)
{
    g_nv_ctrl.debug_sw = arg;
}
void nv_modify_statis_sw(u32 arg)
{
    g_nv_ctrl.statis_sw = arg;
}

s32 nv_modify_pm_sw(s32 arg)
{
    g_nv_ctrl.pmSw = (bool)arg;
    return 0;
}
bool nv_isSysNv(u16 itemid)
{
    /*lint -save -e958*/
    u32 i;
    /*lint -restore*/
    if(itemid >= NV_ID_SYS_MIN_ID && itemid <= NV_ID_SYS_MAX_ID)
        return true;

    for(i = 0;i<sizeof(g_NvSysList)/sizeof(g_NvSysList[0]);i++)
    {
        if(itemid == g_NvSysList[i])
            return true;
    }
    return false;

}
void nv_AddListNode(u32 itemid)
{
    struct list_head * me = NULL;
    struct nv_write_list_stru * cur = NULL;
    if(g_nv_ctrl.statis_sw == true)
    {
        list_for_each(me, &g_nv_ctrl.stList)
        {
            cur = list_entry(me,struct nv_write_list_stru,stList);/*存在相同的nv id*/
            if(cur->itemid == itemid){
                cur->count ++;
                cur->slice = bsp_get_slice_value() - cur->slice;
                return;
            }
        }

        cur = NULL;
        /* [false alarm]:test using */
        cur = (struct nv_write_list_stru*)nv_malloc(sizeof(struct nv_write_list_stru));/* [false alarm]:test using */
        if(!cur)
            return;
        cur->itemid = itemid;
        cur->count  = 1;
        cur->slice  = bsp_get_slice_value();
        list_add(&cur->stList, &g_nv_ctrl.stList);
    }
    /* coverity[leaked_storage] */
    return;
}

/*
 * get nv read right,check the nv init state or upgrade state to read nv,
 * A core may read nv data after kernel init ,C core read nv data must behine the phase of
 *       acore kernel init or acore init ok
 */
bool nv_read_right(u32 itemid)
{
    struct nv_global_ddr_info_stru* ddr_info= (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    if( NV_BOOT_INIT_OK > ddr_info->acore_init_state)
    {
        return false;
    }
    return true;
}

bool nv_write_right(u32 itemid)
{
    struct nv_global_ddr_info_stru* ddr_info= (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    if(NV_INIT_OK != ddr_info->acore_init_state)
    {
        return false;
    }

    /*TMODE CHECK*/

    return true;
}
/*
 * get file len
 * return : file len
 */
u32 nv_get_file_len(FILE* fp)
{
    s32 ret = -1;
    u32 seek = 0;

    ret = nv_file_seek(fp,0,SEEK_END);
    if(ret)
    {
        goto out;
    }

    seek = (u32)nv_file_ftell(fp);

    ret = nv_file_seek(fp,0,SEEK_SET);
    if(ret)
    {
        goto out;
    }

    return seek;

out:
    printf("\n[%s]\n",__FUNCTION__);
    return NV_ERROR;
}

/*
 * check the dload file validity
 *
 */
bool nv_dload_file_check(void )
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
            printf("[%s] : i %d,itemid 0x%x,itemid_next 0x%x\n",__FUNCTION__,i,ref_info->itemid,ref_info_next->itemid);
            return false;
        }
        ref_info ++;
        ref_info_next ++;
    }

    /*check file id sort*/
    for(i = 0;i<ctrl_info->file_num;i++)
    {
        if(file_info->file_id != (i+1))
        {
            printf("[%s] :i %d,file_id %d",__FUNCTION__,i,file_info->file_id);
            return false;
        }
        file_info ++;
    }


    return true;
}
/*
 * xml decode
 * path     :xml path
 * map_path :xml map file path
 */
u32 nv_xml_decode(s8* path,s8* map_path,u32 card_type)
{
    u32 ret = NV_ERROR;
    FILE* fp = NULL;

    nv_debug(NV_FUN_XML_DECODE,0,0,0,0);
    fp = nv_file_open(path,(s8*)NV_FILE_READ);
    if(NULL == fp)
    {
        ret = BSP_ERR_NV_NO_FILE;
        nv_debug(NV_FUN_XML_DECODE,1,ret,0,0);
        return ret;
    }
    nv_mntn_record("enter to decode  %s \n",path);
    ret = xml_decode_main(fp,map_path,card_type);
    nv_file_close(fp);
    fp = NULL;
    if(ret)
    {
        nv_debug(NV_FUN_XML_DECODE,2,ret,0,0);
        goto xml_decode_err;
    }

    return NV_OK;
xml_decode_err:
    nv_mntn_record("file path :%s card type %d\n",path,card_type);
    nv_help(NV_FUN_XML_DECODE);
    return ret;
}
/*
 * read file to ddr,include download,backup,workaround,default
 */

u32 nv_read_from_file(FILE* fp,u8* ptr,u32* datalen)
{
    u32 ret = NV_ERROR;
    struct nv_global_ddr_info_stru * ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    /* coverity[var_decl] */
    struct nv_ctrl_file_info_stru ctrl_info;

    *datalen = nv_get_file_len(fp);

    if((*datalen > NV_MAX_FILE_SIZE)||(*datalen == 0))
    {
        printf("[%s]:  datalen 0x%x\n",__func__,*datalen);
        ret = BSP_ERR_NV_FILE_ERROR;
        goto out;
    }
    /*add file head magic num check */
    ret = (u32)nv_file_read((u8*)&ctrl_info,1,sizeof(ctrl_info),fp);
    if(ret != sizeof(ctrl_info))
    {
         printf("[%s]:  ret 0x%x\n",__func__,ret);
        goto out;
    }
    /* coverity[uninit_use] */
    if(ctrl_info.magicnum != NV_CTRL_FILE_MAGIC_NUM)
    {
        /*add file head mntn info*/
        printf("[%s]:  ctrl_info.magicnum 0x%x\n",__func__,ctrl_info.magicnum);
        goto out;
    }
    nv_file_seek(fp,0,SEEK_SET);

    ret = (u32)nv_file_read((u8*)ptr,1,(*datalen),fp);
    if(ret != (*datalen))
    {
        printf("[%s]:  ret 0x%x, datalen 0x%x\n",__func__,ret,*datalen);
        goto out;
    }

    ret = nv_init_file_info((u8*)NV_GLOBAL_CTRL_INFO_ADDR,(u8*)NV_GLOBAL_INFO_ADDR);
    if(ret)
    {
        nv_mntn_record("[%s]:  ret 0x%x\n",__func__,ret);
        goto out;
    }
    if(*datalen != ddr_info->file_len)
    {
        printf("[%s]:%d,ddr len:0x%x\n",__func__,__LINE__,ddr_info->file_len);
        ret = BSP_ERR_NV_FILE_ERROR;
        goto out;
    }

    nv_flush_cache((void*)NV_GLOBAL_CTRL_INFO_ADDR,(u32)NV_MAX_FILE_SIZE);
    return NV_OK;
out:
    nv_mntn_record("ret : 0x%x,datalen 0x%x\n",ret,*datalen);
    return NV_ERROR;
}

/*
 * revert manufacture version,single file
 */
u32 nv_revert_default(FILE* fp,u32 len)
{
    u32 ret = NV_ERROR;
    u32 i = 0;
    u8* pdata = NULL;
    struct nv_ref_data_info_stru img_ref_info;
    struct nv_file_list_info_stru img_file_info;
    struct nv_comm_file_head_stru* comm_head = NULL;
    struct nv_comm_file_lookup_stru* comm_lookup = NULL;
    u32 datalen = 0;

    nv_debug(NV_FUN_REVERT_DEFAULT,20,0,0,0);

    pdata = (u8*)nv_malloc(len+1);
    if(NULL == pdata)
    {
        ret = BSP_ERR_NV_MALLOC_FAIL;
        nv_debug(NV_FUN_REVERT_DEFAULT,21,ret,0,0);
        return ret;
    }


    ret = (u32)nv_file_read(pdata,1,len,fp);
    if(ret != len)
    {
        nv_debug(NV_FUN_REVERT_DEFAULT,22,0,0,0);
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        goto err_out ;
    }

    comm_head  = (struct nv_comm_file_head_stru*)pdata;
    comm_lookup = (struct nv_comm_file_lookup_stru*)((u8*)pdata + \
        sizeof(struct nv_comm_file_head_stru));

    for(i=0;i<comm_head->lookup_num;i++)
    {
        ret = nv_search_byid(comm_lookup->itemid, (u8*)NV_GLOBAL_CTRL_INFO_ADDR,&img_ref_info,&img_file_info);
        if(ret)
        {
            nv_mntn_record("item id 0x%x none!\n",comm_lookup->itemid);
            comm_lookup++;
            continue;
        }
        if(comm_lookup->nv_len != img_ref_info.nv_len)
        {
            nv_mntn_record("comm len :0x%x,ref info len :0x%x\n",comm_lookup->nv_len,img_ref_info.nv_len);
            comm_lookup++;
            continue;
        }
        if((comm_lookup->modem_num == NV_USIMM_CARD_2) &&
           (img_ref_info.modem_num == NV_USIMM_CARD_2))
        {
            datalen = comm_lookup->nv_len * 2;
        }
        else
        {
            datalen = comm_lookup->nv_len;
        }

        ret = nv_write_to_mem((u8*)pdata+comm_lookup->nv_off,datalen,img_ref_info.file_id,img_ref_info.nv_off);
        if(ret)
        {
            nv_debug(NV_FUN_REVERT_DEFAULT,24,ret,0,0);
            goto err_out;
        }
        comm_lookup++;
    }
    nv_free(pdata);
    return NV_OK;

err_out:
    nv_free(pdata);
    printf("\n[%s]\n",__FUNCTION__);
    nv_help(NV_FUN_REVERT_DEFAULT);
    return ret;
}
/*
 * revert nv data,suport double sim card
 */
u32 nv_revert_data(s8* path,const u16* revert_data,u32 len)
{
    FILE* fp = NULL;
    u32 ret = NV_ERROR;
    u32 i = 0;
    u32 datalen = 0;        /*read file len*/
    u32 file_offset = 0;
    u8* pdata = NULL;
    u8* nvdata = NULL;           /*single nv data ,max len 2048byte*/
    struct nv_ctrl_file_info_stru    ctrl_head_info = {0};   /*bak file ctrl file head*/
    struct nv_file_list_info_stru    bak_file_info  = {0};
    struct nv_global_ddr_info_stru   bak_ddr_info   = {0};
    struct nv_ref_data_info_stru     bak_ref_info   = {0};

    struct nv_ref_data_info_stru    img_ref_info  = {0};
    struct nv_file_list_info_stru   img_file_info = {0};
    u16* pidlist  = (u16*)revert_data;

    nv_debug(NV_FUN_REVERT_DATA,0,0,0,0);
    fp = nv_file_open(path,(s8*)NV_FILE_READ);
    if(NULL == fp)
    {
        nv_debug(NV_FUN_REVERT_DATA,1,0,0,0);
        return BSP_ERR_NV_NO_FILE;
    }

    datalen = (u32)nv_file_read((u8*)(&ctrl_head_info),1,sizeof(ctrl_head_info),fp);
    if(datalen != sizeof(ctrl_head_info))
    {
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        nv_debug(NV_FUN_REVERT_DATA,2,(u32)fp,datalen,ret);
        goto close_file;
    }
    nv_file_seek(fp,0,SEEK_SET); /*jump to file head*/
    if(ctrl_head_info.magicnum != NV_CTRL_FILE_MAGIC_NUM)
    {
        ret = BSP_ERR_NV_FILE_ERROR;
        nv_debug(NV_FUN_REVERT_DATA,31,0,0,0);
        goto close_file;
    }

    pdata = (u8*)nv_malloc(ctrl_head_info.ctrl_size+1);
    if(NULL == pdata)
    {
        ret = BSP_ERR_NV_MALLOC_FAIL;
        nv_debug(NV_FUN_REVERT_DATA,3,ctrl_head_info.ctrl_size,0,ret);
        goto close_file;
    }

    datalen = (u32)nv_file_read(pdata,1,ctrl_head_info.ctrl_size,fp);
    nv_file_seek(fp,0,SEEK_SET); /*jump to file head*/
    if(datalen != ctrl_head_info.ctrl_size)
    {
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        nv_debug(NV_FUN_REVERT_DATA,4,ctrl_head_info.ctrl_size,datalen,ret);
        goto free_pdata;
    }

    ret = nv_init_file_info((u8*)pdata,(u8*)&bak_ddr_info);
    if(ret)
    {
        ret = BSP_ERR_NV_MEM_INIT_FAIL;
        nv_debug(NV_FUN_REVERT_DATA,5,0,0,ret);
        goto free_pdata;
    }

    nvdata = (u8*)nv_malloc(2*NV_MAX_UNIT_SIZE+1);
    if(nvdata == NULL)
    {
        ret = BSP_ERR_NV_MALLOC_FAIL;
        nv_debug(NV_FUN_REVERT_DATA,6,NV_MAX_UNIT_SIZE,0,ret);
        goto free_pdata;
    }

    for(i = 0;i<len ;i++)
    {
        ret = nv_search_byid((u32)(*pidlist),pdata,&bak_ref_info,&bak_file_info);
        if(ret)
        {
            g_nv_ctrl.revert_search_err++;
            pidlist++;
            continue;
        }
        file_offset = bak_ddr_info.file_info[bak_file_info.file_id-1].offset +bak_ref_info.nv_off;

        /*search nv from global ddr data*/
        ret = nv_search_byid((u32)(*pidlist),(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&img_ref_info,&img_file_info);
        if(ret)
        {
            g_nv_ctrl.revert_search_err++;
            pidlist++;
            nv_debug(NV_FUN_REVERT_DATA,9,ret,(u32)*pidlist,0);
            continue;
        }

        if(img_ref_info.nv_len != bak_ref_info.nv_len)
        {
            nv_mntn_record("nv item len err,itemid :0x%x,new len:0x%x, old len :0x%x\n",\
                *pidlist,img_ref_info.nv_len,bak_ref_info.nv_len);
            g_nv_ctrl.revert_len_err++;
            pidlist++;
            continue;
        }
        if((img_ref_info.modem_num == NV_USIMM_CARD_2)
            &&(bak_ref_info.modem_num == NV_USIMM_CARD_2))
        {
            bak_ref_info.nv_len *= 2; /*将双卡数据同时恢复*/
        }
        /*jump to nv off*/
        nv_file_seek(fp,(s32)file_offset,SEEK_SET);
        datalen = (u32)nv_file_read(nvdata,1,(u32)(bak_ref_info.nv_len),fp);
        if(datalen != bak_ref_info.nv_len)
        {
            ret = BSP_ERR_NV_READ_FILE_FAIL;
            nv_debug(NV_FUN_REVERT_DATA,11,datalen,(u32)*pidlist,bak_ref_info.nv_len);
            goto free_nvdata;
        }
        nv_file_seek(fp,0,SEEK_SET);

        ret = nv_write_to_mem(nvdata,bak_ref_info.nv_len,img_file_info.file_id,img_ref_info.nv_off);
        if(ret)
        {
            ret = BSP_ERR_NV_WRITE_DATA_FAIL;
            nv_debug(NV_FUN_REVERT_DATA,12,(u32)*pidlist,ret,img_file_info.file_id);
            goto free_nvdata;
        }
        pidlist++;
        g_nv_ctrl.revert_count ++;
    }
    nv_free(pdata);
    nv_free(nvdata);
    nv_file_close(fp);

    return NV_OK;

free_nvdata:
    nv_free(nvdata);
free_pdata:
    nv_free(pdata);
close_file:
    nv_file_close(fp);
    fp = NULL;
    nv_mntn_record("\n[%s]\n",__FUNCTION__);
    nv_help(NV_FUN_REVERT_DATA);
    return ret;
}


u32 nv_revertEx(const s8* path)
{
    u32 ret = NV_ERROR;

    nv_mntn_record("start to revert nv from %s!\n",path);
    ret = nv_revert_data((s8*)path,g_ausNvResumeUserIdList,\
        bsp_nvm_getRevertNum(NV_USER_ITEM));
    if(ret)
    {
        nv_error_printf("ret = 0x%x\n",ret);
        return ret;
    }
    ret = nv_revert_data((s8*)path,g_ausNvResumeManufactureIdList,\
        bsp_nvm_getRevertNum(NV_MANUFACTURE_ITEM));
    if(ret)
    {
        nv_error_printf("ret = 0x%x\n",ret);
        return ret;
    }
    ret = nv_revert_data((s8*)path,g_ausNvResumeSecureIdList,\
        bsp_nvm_getRevertNum(NV_SECURE_ITEM));
    if(ret)
    {
        nv_error_printf("ret = 0x%x\n",ret);
        return ret;
    }
    nv_mntn_record("end of revert nv from %s!\n",path);

    return NV_OK;
}

/*
 * search nv info by nv id
 * &pdata:  data start ddr
 * output: ref_info,file_info
 */
u32 nv_search_byid(u32 itemid,u8* pdata,struct nv_ref_data_info_stru* ref_info,struct nv_file_list_info_stru* file_info)
{
    u32 low;
    u32 high;
    u32 mid;
    u32 offset;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)pdata;

    high = ctrl_info->ref_count;
    low  = 1;

    nv_debug(NV_FUN_SEARCH_NV,0,itemid,high,(u32)ctrl_info);

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

/*
 * copy user buff to global ddr,used to write nv data to ddr
 * &file_id :file id
 * &offset  :  offset of global file ddr
 */
u32 nv_write_to_mem(u8* pdata,u32 size,u32 file_id,u32 offset)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    unsigned long nvflag;
    if(offset > ddr_info->file_info[file_id-1].size)
    {
        show_ddr_info();
        printf("[%s]:offset 0x%x\n",__FUNCTION__,offset);
        return BSP_ERR_NV_FILE_ERROR;
    }


    nv_spin_lock(nvflag, IPC_SEM_NV);
    memcpy((u8*)(NV_GLOBAL_CTRL_INFO_ADDR+ddr_info->file_info[file_id-1].offset + offset),pdata,size);
    nv_flush_cache((u8*)(NV_GLOBAL_CTRL_INFO_ADDR+ddr_info->file_info[file_id-1].offset + offset), size);
    nv_spin_unlock(nvflag, IPC_SEM_NV);

    return NV_OK;
}


/*
 * copy global ddr to user buff,used to read nv data from ddr
 * &file_id : file id
 * &offset:  offset of the file
 */
u32 nv_read_from_mem(u8* pdata,u32 size,u32 file_id,u32 offset)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    if(offset > ddr_info->file_info[file_id-1].size)
    {
        show_ddr_info();
        printf("[%s]:offset 0x%x\n",__FUNCTION__,offset);
        return BSP_ERR_NV_FILE_ERROR;
    }

    memcpy(pdata,(u8*)(NV_GLOBAL_CTRL_INFO_ADDR +ddr_info->file_info[file_id-1].offset + offset),size);
    return NV_OK;
}


/*
 *  nv read data from global ddr of icc through the chanel id
 */
u32 nv_icc_read(u32 chanid, u32 len)
{
    u32  ret = NV_ERROR;

    memset(g_nv_ctrl.nv_icc_buf,0,NV_ICC_BUF_LEN);
    ret = (u32)bsp_icc_read(chanid,g_nv_ctrl.nv_icc_buf,len);
    if(len != ret)
    {
        printf("[%s]:ret : 0x%x ,len 0x%x\n",__FUNCTION__,ret,len);
        return BSP_ERR_NV_ICC_CHAN_ERR;
    }
    return NV_OK;
}

/*
 *  acore callback of icc msg.only accept req message
 */
s32 nv_icc_msg_proc(u32 chanid ,u32 len,void* pdata)
{
    if((chanid != (ICC_CHN_NV << 16 | NV_RECV_FUNC_AC)) &&
       (chanid != (ICC_CHN_MCORE_ACORE << 16 | NV_RECV_FUNC_AM))
       )
    {
        printf("[%s] icc channel error :0x%x\n",__func__,chanid);
        return -1;
    }

    g_nv_ctrl.icc_cb_count++;
    osl_sem_up(&g_nv_ctrl.task_sem);

    return 0;

}

/*
 *  nv use this inter to send data through the icc channel
 */
u32 nv_icc_send(u32 chanid,u8* pdata,u32 len)
{
    s32  ret ;
    u32  fun_id = chanid & 0xffff;/*get fun id*/
    u32  core_type ;
    u32  i = 0;

    if(fun_id == NV_RECV_FUNC_AC)
    {
        core_type = ICC_CPU_MODEM;
    }
    else if(fun_id == NV_RECV_FUNC_AM)
    {
        return NV_OK;
    }
    else
    {
        return BSP_ERR_NV_INVALID_PARAM;
    }
    /*lint -save -e578 -e530*/
    nv_debug_trace(pdata, len);
    /*lint -restore +e578 +e530*/
    for(i = 0;i<NV_ICC_SEND_COUNT;i++)
    {
        ret = bsp_icc_send(core_type,chanid,pdata,len);
        if(ICC_INVALID_NO_FIFO_SPACE == ret)/*消息队列满,则50ms之后重新发送*/
        {
            nv_taskdelay(50);
            continue;
        }
        else if(ret != (s32)len)
        {
            printf("[%s]:ret :0x%x,len 0x%x\n",__FUNCTION__,ret,len);
            return BSP_ERR_NV_ICC_CHAN_ERR;
        }
        else
        {
            return NV_OK;
        }
    }
    system_error(DRV_ERRNO_NV_ICC_FIFO_FULL,core_type,chanid,(char*)pdata,len);
    return NV_ERROR;
}

/*
 *  init icc channel used by nv module
 */
u32 nv_icc_chan_init(u32 fun_id)
{
    u32 chanid;
    if(fun_id == NV_RECV_FUNC_AC)
    {
        chanid = ICC_CHN_NV << 16 | fun_id;
    }
    else if(fun_id == NV_RECV_FUNC_AM)
    {
        chanid = ICC_CHN_MCORE_ACORE << 16 | fun_id;
    }
    else
    {
        return BSP_ERR_NV_INVALID_PARAM;
    }

     /*reg icc debug proc*/
    (void)bsp_icc_debug_register(chanid,nv_modify_pm_sw,(s32)true);

    return (u32)bsp_icc_event_register(chanid,nv_icc_msg_proc,NULL,NULL,NULL);
}

/*
 *  write data to file/flash/rfile,base the nv priority,inner packing write to ddr
 *  &pdata:    user buff
 *  &offset:   offset of nv in ddr
 *  &len :     data length
 */
u32 nv_write_to_file(struct nv_ref_data_info_stru* ref_info)
{
    u32 ret = NV_OK;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    /* [false alarm]:off is in using */
    u32 off;
    u32 temp_prio = 0;
    unsigned long nvflag;

    if(NV_HIGH_PRIORITY == ref_info->priority)
    {
        off = ddr_info->file_info[ref_info->file_id-1].offset+ref_info->nv_off;
        ret = bsp_nvm_flushEx(off,ref_info->nv_len,ref_info->itemid);
    }
    else if(NV_LOW_PRIORITY == ref_info->priority)
    {
        return ret;
    }
    else
    {
        nv_spin_lock(nvflag, IPC_SEM_NV);

        ddr_info->priority[ref_info->priority - NV_MID_PRIORITY1] +=\
            nv_mid_droit[ref_info->priority - NV_MID_PRIORITY1];
        temp_prio = ddr_info->priority[ref_info->priority - NV_MID_PRIORITY1];

        nv_flush_cache((void*)NV_GLOBAL_INFO_ADDR, (u32)NV_GLOBAL_INFO_SIZE);
        nv_spin_unlock(nvflag, IPC_SEM_NV);
    }
    if(temp_prio >= g_nv_ctrl.mid_prio)
    {
         /*[false alarm]:Value Never Read*/
        off = ddr_info->file_info[ref_info->file_id-1].offset+ref_info->nv_off;
        ret = bsp_nvm_flushEx(0,ddr_info->file_len,ref_info->itemid);
    }

    if(true == nv_isSysNv(ref_info->itemid))
    {
        ret = bsp_nvm_flushSys(ref_info->itemid);
    }
    return ret;
}

/* nv_get_key_data
 * 从对应分区中根据nv id提取数据
 * path : 文件路径
 * itemid: nv id
 * buffer: 数据缓存,输入/输出
 * len   : buffer len
 */
u32 nv_get_key_data(const s8* path,u32 itemid,void* buffer,u32 len)
{
    FILE* fp = NULL;
    u32 ret = NV_ERROR;
    u32 file_offset = 0;
    u8* ctrl_head = NULL;
    struct nv_ctrl_file_info_stru    ctrl_head_info = {0};   /*bak file ctrl file head*/
    struct nv_global_ddr_info_stru   file_base_info = {0};
    struct nv_file_list_info_stru file_info = {0};
    struct nv_ref_data_info_stru ref_info  = {0};


    fp = nv_file_open(path,(s8*)NV_FILE_READ);
    if(!fp)
        return BSP_ERR_NV_NO_FILE;

    ret = (u32)nv_file_read((u8*)(&ctrl_head_info),1,sizeof(ctrl_head_info),fp);
    if(ret != sizeof(ctrl_head_info))
    {
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        goto out;
    }

    if(ctrl_head_info.magicnum != NV_CTRL_FILE_MAGIC_NUM)
    {
        ret = BSP_ERR_NV_FILE_ERROR;
        goto out;
    }

    ctrl_head = (u8*)nv_malloc(ctrl_head_info.ctrl_size+1);
    if(NULL == ctrl_head)
    {
        ret = BSP_ERR_NV_MALLOC_FAIL;
        goto out;
    }

    nv_file_seek(fp,0,SEEK_SET); /*jump to file head*/
    ret = (u32)nv_file_read(ctrl_head,1,ctrl_head_info.ctrl_size,fp);
    if(ret != ctrl_head_info.ctrl_size)
    {
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        goto out1;
    }

    ret = nv_init_file_info((u8*)ctrl_head,(u8*)&file_base_info);
    if(ret)
        goto out1;

    ret = nv_search_byid(itemid,(u8*)ctrl_head,&ref_info,&file_info);
    if(ret)
        goto out1;
    file_offset = file_base_info.file_info[ref_info.file_id-1].offset +ref_info.nv_off;

    nv_file_seek(fp,(s32)file_offset,SEEK_SET);
    ret = (u32)nv_file_read(buffer,1, len,fp);
    if(ret != len)
    {
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        goto out1;
    }
    nv_free(ctrl_head);
    nv_file_close(fp);
    return NV_OK;

out1:
    nv_free(ctrl_head);
out:
    nv_file_close(fp);
    return ret;
}

/* nv_imei_data_comp
 * 指定分区中的imei与出厂分区中的数据对边
 * path : 指定分区文件路径
 */
u32 nv_imei_data_comp(const s8* path)
{
    u32 ret;
    char fac_imei[16];
    char path_imei[16];

    memset(fac_imei,0,sizeof(fac_imei));
    memset(path_imei,0,sizeof(path_imei));

    /*出厂分区中的imei号获取失败的情况下无需比较*/
    ret = nv_get_key_data((s8*)NV_DEFAULT_PATH,NV_ID_DRV_IMEI,fac_imei,sizeof(fac_imei));
    if(ret)
    {
        nv_printf("get imei from %s fail ,ret :0x%x\n",NV_DEFAULT_PATH,ret);
        return NV_OK;
    }

    /*出厂分区imei号如果全0为无效数据，则不比较*/
    ret = (u32)memcmp(fac_imei,path_imei,sizeof(fac_imei));
    if(!ret)
    {
        nv_printf("factory imei all 0,return direct !\n");
        return NV_OK;
    }

    ret = nv_get_key_data((s8*)path,NV_ID_DRV_IMEI,path_imei,sizeof(path_imei));
    if(BSP_ERR_NV_MALLOC_FAIL == ret)/*分配内存失败无需比较*/
    {
        nv_printf("mem malloc failed ,no compare!\n");
        return NV_OK;
    }
    if(ret)
    {
        nv_printf("get imei from %s fail ,ret :0x%x\n",path,ret);
        return ret;
    }

    ret = (u32)memcmp(fac_imei,path_imei,sizeof(fac_imei));
    if(ret)
    {
        nv_modify_print_sw(1);
        nv_debug_trace(fac_imei, sizeof(fac_imei));
        nv_debug_trace(path_imei, sizeof(path_imei));
        nv_modify_print_sw(0);
        return ret;
    }

    return NV_OK;
}

void show_ddr_info(void)
{
    u32 i;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    struct nv_file_list_info_stru* file_info = (struct nv_file_list_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE);
    struct nv_ref_data_info_stru* ref_info   = (struct nv_ref_data_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE\
        +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num);

    printf("global start ddr        :0x%x\n",NV_GLOBAL_INFO_ADDR);
    printf("global ctrl file ddr    :0x%x\n",NV_GLOBAL_CTRL_INFO_ADDR);
    printf("global file list ddr    :0x%x\n",file_info);
    printf("global ref info  ddr    :0x%x\n",ref_info);
    printf("icc core type           :0x%x\n",g_nv_ctrl.icc_core_type);
    printf("*******************ddr global ctrl************************\n");
    printf("acore init state: 0x%x\n",ddr_info->acore_init_state);
    printf("ccore init state: 0x%x\n",ddr_info->ccore_init_state);
    printf("ddr read case  : 0x%x\n",ddr_info->ddr_read);
    for(i = 0;i<NV_MID_PRI_LEVEL_NUM;i++)
    {
        printf("mid priority %d  : 0x%x\n",i,ddr_info->priority[i]);
    }
    nv_mntn_record("mem file type   : 0x%x\n",ddr_info->mem_file_type);
    nv_mntn_record("total revert count      :%d\n",g_nv_ctrl.revert_count);
    nv_mntn_record("revert search err count :%d\n",g_nv_ctrl.revert_search_err);
    nv_mntn_record("revert len err count    :%d\n",g_nv_ctrl.revert_len_err);
    nv_mntn_record("file total len  : 0x%x\n",ddr_info->file_len);
    nv_mntn_record("comm file num   : 0x%x\n",ddr_info->file_num);


    for(i = 0;i<ddr_info->file_num;i++)
    {
        nv_mntn_record("##############################\n");
        nv_mntn_record("** file id   0x%x\n",ddr_info->file_info[i].file_id);
        nv_mntn_record("** file size 0x%x\n",ddr_info->file_info[i].size);
        nv_mntn_record("** file off  0x%x\n",ddr_info->file_info[i].offset);
    }

    nv_mntn_record("*******************global ctrl file***********************\n");
    nv_mntn_record("ctrl file size    : 0x%x\n",ctrl_info->ctrl_size);
    nv_mntn_record("file num          : 0x%x\n",ctrl_info->file_num);
    nv_mntn_record("file list off     : 0x%x\n",ctrl_info->file_offset);
    nv_mntn_record("file list size    : 0x%x\n",ctrl_info->file_size);
    nv_mntn_record("ctrl file magic   : 0x%x\n",ctrl_info->magicnum);
    nv_mntn_record("modem num         : 0x%x\n",ctrl_info->modem_num);
    nv_mntn_record("nv count          : 0x%x\n",ctrl_info->ref_count);
    nv_mntn_record("nv ref data off   : 0x%x\n",ctrl_info->ref_offset);
    nv_mntn_record("nv ref data size  : 0x%x\n",ctrl_info->ref_size);
    nv_mntn_record("*******************global file list***********************\n");
    for(i = 0;i<ctrl_info->file_num;i++)
    {
        nv_mntn_record("file_info     : 0x%x\n",file_info);
        nv_mntn_record("file id       : 0x%x\n",file_info->file_id);
        nv_mntn_record("file name     : %s\n",file_info->file_name);
        nv_mntn_record("file size     : 0x%x\n",file_info->file_size);
        file_info ++;
        nv_mntn_record("\n");
    }
}


void show_ref_info(u32 arg1,u32 arg2)
{
    u32 i;
    u32 _max;
    u32 _min;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    struct nv_ref_data_info_stru* ref_info   = (struct nv_ref_data_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE\
        +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num);

    _max = arg2 > ctrl_info->ref_count ? ctrl_info->ref_count:arg2;
    _min = arg1 > _max ? 0: arg1;

    _max = (arg2 ==0)?ctrl_info->ref_count: _max;

    ref_info = ref_info+_min;

    for(i = _min;i<_max;i++)
    {
        printf("第%d项 :\n",i);
        printf("nvid   :0x%-8x, file id : 0x%-8x\n",ref_info->itemid,ref_info->file_id);
        printf("nvlen  :0x%-8x, nv_off  : 0x%-8x, nv_pri 0x%-8x\n",ref_info->nv_len,ref_info->nv_off,ref_info->priority);
        printf("dsda   :0x%-8x\n",ref_info->modem_num);
        ref_info++;
    }
}

void nv_show_fastboot_err(void)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    nv_mntn_record("**************************************\n");
    nv_mntn_record("line       :%d\n",  ddr_info->fb_debug.line);
    nv_mntn_record("result     :0x%x\n",ddr_info->fb_debug.ret);
    nv_mntn_record("reseved1   :0x%x\n",ddr_info->fb_debug.reseverd1);
    nv_mntn_record("reseved2   :0x%x\n",ddr_info->fb_debug.reseverd2);
    nv_mntn_record("**************************************\n");
}
void nv_show_listInfo(void)
{
    struct list_head *me = NULL;
    struct nv_write_list_stru * cur = NULL;
    list_for_each(me,&g_nv_ctrl.stList)
    {
        cur = list_entry(me, struct nv_write_list_stru, stList);
        printf("NV ID :0x%4x 写入 %d 次，最近写入间隔:0x%x\n",cur->itemid,cur->count,cur->slice);
    }
}


/* [false alarm]:i is in using */
void nv_help(u32 type)
{
    /*[false alarm]:i is in using*/
    u32 i = type;
    if(type == 63/*'?'*/)
    {
        printf("1   -------  read\n");
        printf("4   -------  auth read\n");
        printf("5   -------  write\n");
        printf("6   -------  auth write\n");
        printf("8   -------  get len\n");
        printf("9   -------  auth get len\n");
        printf("10  -------  flush\n");
        printf("12  -------  backup\n");
        printf("15  -------  import\n");
        printf("16  -------  export\n");
        printf("19  -------  kernel init\n");
        printf("20  -------  remain init\n");
        printf("21  -------  nvm init\n");
        printf("22  -------  xml decode\n");
        printf("24  -------  revert\n");
        printf("25  -------  update default\n");
        return;

    }
    if(type == NV_FUN_MAX_ID)
    {
        for(i = 0;i< NV_FUN_MAX_ID;i++)
        {
            printf("************fun id %d******************\n",i);
            printf("call num             : 0x%x\n",g_nv_debug[i].callnum);
            printf("out branch (reseved1): 0x%x\n",g_nv_debug[i].reseved1);
            printf("reseved2             : 0x%x\n",g_nv_debug[i].reseved2);
            printf("reseved3             : 0x%x\n",g_nv_debug[i].reseved3);
            printf("reseved4             : 0x%x\n",g_nv_debug[i].reseved4);
            printf("***************************************\n");
        }
        return ;
    }

    i = type;
    nv_mntn_record("************fun id %d******************\n",i);
    nv_mntn_record("call num             : 0x%x\n",g_nv_debug[i].callnum);
    nv_mntn_record("out branch (reseved1): 0x%x\n",g_nv_debug[i].reseved1);
    nv_mntn_record("reseved2             : 0x%x\n",g_nv_debug[i].reseved2);
    nv_mntn_record("reseved3             : 0x%x\n",g_nv_debug[i].reseved3);
    nv_mntn_record("reseved4             : 0x%x\n",g_nv_debug[i].reseved4);
    nv_mntn_record("***************************************\n");
}

/*lint -restore*/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif




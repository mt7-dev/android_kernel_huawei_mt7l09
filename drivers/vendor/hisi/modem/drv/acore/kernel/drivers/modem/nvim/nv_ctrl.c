



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e537*/
#include <ptable_com.h>
#include <drv_rfile.h>
#include <bsp_nandc.h>
#include "nv_comm.h"
#include "nv_ctrl.h"
/*lint -restore +e537*/
/*lint -save -e958*/
struct file_ops_table_stru  g_nv_ops = {
#ifdef FEATURE_NV_FLASH_ON
    .fo = nv_flash_open,
    .fc = nv_flash_close,
    .frm= nv_flash_remove,
    .fr = nv_flash_read,
    .fw = nv_flash_write,
    .fs = nv_flash_seek,
    .ff = nv_flash_ftell,
    .fa = nv_flash_access,
#elif  defined(FEATURE_NV_LFILE_ON)
    .fo = nv_lfile_open,
    .fc = nv_lfile_close,
    .frm= nv_lfile_remove,
    .fr = nv_lfile_read,
    .fw = nv_lfile_write,
    .fs = nv_lfile_seek,
    .ff = nv_lfile_ftell,
    .fa = nv_lfile_access,
#elif  defined(FEATURE_NV_RFILE_ON)
    .fo = nv_rfile_open,
    .fc = nv_rfile_close,
    .frm= nv_rfile_remove,
    .fr = nv_rfile_read,
    .fw = nv_rfile_write,
    .fs = nv_rfile_seek,
    .fa = nv_lfile_access,
    .ff = nv_rfile_ftell,
    .fa = nv_rfile_access,
#elif  defined(FEATURE_NV_EMMC_ON)
    .fo = nv_emmc_open,
    .fc = nv_emmc_close,
    .frm= nv_emmc_remove,
    .fr = nv_emmc_read,
    .fw = nv_emmc_write,
    .fs = nv_emmc_seek,
    .ff = nv_emmc_ftell,
    .fa = nv_emmc_access,
#endif
};

/*
 * Function:    nv_file_init
 * Discription: global info init,flash: get mtd device
 */
u32 nv_file_init(void)
{
    u32 ret = NV_ERROR;

#ifdef FEATURE_NV_FLASH_ON
    ret = nv_flash_init();
    if(ret != NV_OK)
    {
        return ret;
    }
#elif  defined(FEATURE_NV_LFILE_ON)
#elif  defined(FEATURE_NV_RFILE_ON)
#elif  defined(FEATURE_NV_EMMC_ON)
    ret = nv_emmc_init();
    if(ret)
    {
        return ret;
    }
#endif

    return NV_OK;

}

/*
 * Function: nv_file_open
 * Discription: open file
 * Parameter:   path  :  file path
 *              mode  :  file ops type etc:"r+","rb+","w+","wb+"
 * Output   :   file pointer
 */
FILE* nv_file_open(const s8* path,const s8* mode)
{
    struct nv_file_p* fp = NULL;

    fp = (struct nv_file_p*)nv_malloc(sizeof(struct nv_file_p));
    if(!fp)
    {
        return NULL;
    }

#ifdef BSP_CONFIG_HI3630
    if(0 == strcmp((char*)path,(char*)NV_IMG_PATH))
#else
    if(0 == strcmp((char*)path,(char*)NV_IMG_PATH))
#endif
    {
        fp->fd = BSP_fopen((char*)path,(char*)mode);
        fp->stor_type = NV_FILE_STOR_FS;
    }
    else
    {
        fp->fd = g_nv_ops.fo(path,mode);
        fp->stor_type = NV_FILE_STOR_NON_FS;
    }

    /* coverity[leaked_storage] */
    if(NULL == fp->fd)
    {
        /* coverity[leaked_storage] */
        return NULL;
    }

    return fp;
}


s32 nv_file_read(u8* ptr,u32 size,u32 count,FILE* fp)
{
    struct nv_file_p* fd = (struct nv_file_p*)fp;

    if(fd->stor_type == NV_FILE_STOR_FS)
    {
        return BSP_fread(ptr,size,count, fd->fd);
    }
    else if(fd->stor_type == NV_FILE_STOR_NON_FS)
    {
        return g_nv_ops.fr(ptr,size,count,fd->fd);
    }
    else
    {
        return -1;
    }

}

s32 nv_file_write(u8* ptr,u32 size,u32 count,FILE* fp)
{
    struct nv_file_p* fd = (struct nv_file_p*)fp;

    if(fd->stor_type == NV_FILE_STOR_FS)
    {
        return BSP_fwrite(ptr,size,count,fd->fd);
    }
    else if(fd->stor_type == NV_FILE_STOR_NON_FS)
    {
        return g_nv_ops.fw(ptr,size,count,fd->fd);
    }
    else
    {
        return -1;
    }

}


s32 nv_file_seek(FILE* fp,s32 offset,s32 whence)
{
    struct nv_file_p* fd = (struct nv_file_p*)fp;

    if(fd->stor_type == NV_FILE_STOR_FS)
    {
        return BSP_fseek(fd->fd,(long)offset,(int)whence);
    }
    else if(fd->stor_type == NV_FILE_STOR_NON_FS)
    {
        return g_nv_ops.fs(fd->fd,offset,whence);
    }
    else
    {
        return -1;
    }

}


s32 nv_file_close(FILE* fp)
{
    struct nv_file_p* fd = (struct nv_file_p*)fp;
    s32 ret;


    if(fd->stor_type == NV_FILE_STOR_FS)
    {
        ret = BSP_fclose(fd->fd);
    }
    else if(fd->stor_type == NV_FILE_STOR_NON_FS)
    {
        ret= g_nv_ops.fc(fd->fd);
    }
    else
    {
        return -1;
    }
    nv_free(fp);
    return ret;
}

s32 nv_file_remove(const s8* path)
{
#ifdef BSP_CONFIG_HI3630
    if(0 == strcmp((char*)path,(char*)NV_IMG_PATH))
#else
    if(0 == strcmp((char*)path,(char*)NV_IMG_PATH))
#endif
    {
        return BSP_remove((char*)path);
    }
    else
    {
        return g_nv_ops.frm(path);
    }
}


s32 nv_file_ftell(FILE* fp)
{
    struct nv_file_p* fd = (struct nv_file_p*)fp;
    if(fd->stor_type == NV_FILE_STOR_FS)
    {
        return BSP_ftell(fd->fd);
    }
    else if(fd->stor_type == NV_FILE_STOR_NON_FS)
    {
        return g_nv_ops.ff(fd->fd);
    }
    else
    {
        return -1;
    }
}




s32 nv_file_access(const s8* path,s32 mode)
{
#ifdef BSP_CONFIG_HI3630
    if(0 == strcmp((char*)path,(char*)NV_IMG_PATH))
#else
    if(0 == strcmp((char*)path,(char*)NV_IMG_PATH))
#endif
    {
        return BSP_access((char*)path,mode);
    }
    else
    {
        return g_nv_ops.fa(path,mode);
    }
}

/*
 * copy file from src_path to dst_path
 * dst_path : 文件拷贝目的目录
 * src_path : 文件拷贝
 * path     : dst_path type :  fasle == file system
 *                             true  == non file system
 */
#define NV_FILE_COPY_UNIT_SIZE      (16*1024)
s32 nv_file_copy(s8* dst_path,s8* src_path,bool path)
{
    u32 ret = NV_ERROR;
    FILE* dst_fp = NULL;
    FILE* src_fp = NULL;
    u32 u_ulen;  /*文件拷贝单位长度*/
    u32 u_tlen;  /*源文件总长度*/
    void* pdata;   /*文件拷贝临时buffer*/


    src_fp = nv_file_open(src_path,NV_FILE_READ);
    dst_fp = nv_file_open(dst_path,NV_FILE_WRITE);
    if(!src_fp || !dst_fp)
    {
        nv_printf("open fail src :%p,dst :%p\n",src_fp,dst_fp);
        return BSP_ERR_NV_NO_FILE;
    }

    u_tlen = nv_get_file_len(src_fp);
    if(u_tlen >= NV_MAX_FILE_SIZE)
    {
        nv_printf("u_tlen :0x%x\n",u_tlen);
        goto out;
    }

    pdata = (void*)nv_malloc(NV_FILE_COPY_UNIT_SIZE);/*拷贝单位长度为16k*/
    if(!pdata)
    {
        nv_printf("malloc failed !\n");
        goto out;
    }

    while(u_tlen)
    {
        u_ulen = u_tlen > NV_FILE_COPY_UNIT_SIZE ? u_tlen :NV_FILE_COPY_UNIT_SIZE;

        ret = (u32)nv_file_read(pdata,1,u_ulen,src_fp);
        if(ret != u_ulen)
        {
            nv_printf("ret :0x%x u_ulen: 0x%x\n",ret,u_ulen);
            goto out1;
        }

        ret = (u32)nv_file_write(pdata,1,u_ulen,dst_fp);
        if(ret != u_ulen)
        {
            nv_printf("ret :0x%x u_ulen: 0x%x\n",ret,u_ulen);
            goto out1;
        }

        u_tlen -= u_ulen;
    }

    (void)nv_file_close(src_fp);
    (void)nv_file_close(dst_fp);
    nv_free(pdata);
    return NV_OK;


out1:
    nv_free(pdata);
out:
    (void)nv_file_close(src_fp);
    (void)nv_file_close(dst_fp);
    return -1;
}

/*copy img to backup*/
u32 nv_copy_img2backup(void)
{
    u32 ret;
    FILE* fp = NULL;
    u32 total_len;
    u32 phy_off = 0;
    u32 unit_len;
    void* pdata = NULL;


    fp = BSP_fopen((char*)NV_IMG_PATH,"rb");
    if(!fp)
    {
        return BSP_ERR_NV_NO_FILE;
    }

    BSP_fseek(fp,0,SEEK_END);
    total_len = (u32)BSP_ftell(fp);
    BSP_fseek(fp,0,SEEK_SET);

    pdata = (void*)nv_malloc(NV_FILE_COPY_UNIT_SIZE);
    if(!pdata)
    {
        BSP_fclose(fp);
        return BSP_ERR_NV_MALLOC_FAIL;
    }

    nv_create_flag_file((s8*)NV_BACK_FLAG_PATH);
    while(total_len)
    {
        unit_len = (total_len >= NV_FILE_COPY_UNIT_SIZE)?NV_FILE_COPY_UNIT_SIZE : total_len;

        ret = (u32)BSP_fread(pdata,1,unit_len,fp);
        if(ret != unit_len)
        {
            nv_free(pdata);
            BSP_fclose(fp);
            return BSP_ERR_NV_READ_FILE_FAIL;
        }

        ret = (u32)bsp_nand_write((char*)NV_BACK_SEC_NAME,phy_off,pdata,unit_len);
        if(ret)
        {
            nv_free(pdata);
            BSP_fclose(fp);
            return BSP_ERR_NV_WRITE_FILE_FAIL;
        }

        phy_off += unit_len;
        total_len -= unit_len;
    }

    nv_free(pdata);
    BSP_fclose(fp);
    nv_delete_flag_file((s8*)NV_BACK_FLAG_PATH);

    return NV_OK;

}


/*修改升级包标志
 *true :有效   false :无效
 */

s32 nv_modify_upgrade_flag(bool flag)
{
    struct nv_dload_packet_head_stru nv_dload;
    s32 ret;
    u32 old_magic;
    u32 new_magic;

    ret = bsp_nand_read((char*)NV_DLOAD_SEC_NAME,0,&nv_dload,sizeof(nv_dload),NULL);
    if(ret)
    {
        return ret;
    }
/*lint -save -e731*/
    if(true == flag)
    {
        old_magic = NV_DLOAD_INVALID_FLAG;
        new_magic = NV_FILE_EXIST;
    }
    else
    {
        new_magic = NV_DLOAD_INVALID_FLAG;
        old_magic = NV_FILE_EXIST;
    }
/*lint -restore*/
    nv_dload.nv_bin.magic_num = (nv_dload.nv_bin.magic_num == old_magic) ? new_magic : nv_dload.nv_bin.magic_num;
    ret = bsp_nand_write((char*)NV_DLOAD_SEC_NAME,0,&nv_dload,sizeof(nv_dload));
    if(ret)
    {
        return ret;
    }

    return 0;
}

/*获取升级包数据有效性
 *true :有效 false: 无效
 */
bool nv_get_upgrade_flag(void)
{
    struct nv_dload_packet_head_stru nv_dload;
    s32 ret;

    ret = bsp_nand_read((char*)NV_DLOAD_SEC_NAME,0,&nv_dload,sizeof(nv_dload),NULL);
    if(ret)
    {
        return false;
    }

    if(nv_dload.nv_bin.magic_num == NV_FILE_EXIST)
    {
        return true;
    }
    return false;
}


char sc_file_path[SC_FILE_NUM][128] =
{
    "/mnvm2:0/SC/Pers/CKFile.bin",
    "/mnvm2:0/SC/Pers/DKFile.bin",
    "/mnvm2:0/SC/Pers/AKFile.bin",
    "/mnvm2:0/SC/Pers/PIFile.bin",
    "/mnvm2:0/SC/Pers/ImeiFile_I0.bin",
    "/mnvm2:0/SC/Pers/ImeiFile_I1.bin",
    "/mnvm2:0/SC/Pers/CKSign.hash",
    "/mnvm2:0/SC/Pers/DKSign.hash",
    "/mnvm2:0/SC/Pers/AKSign.hash",
    "/mnvm2:0/SC/Pers/PISign.hash",
    "/mnvm2:0/SC/Pers/ImeiFile_I0.hash",
    "/mnvm2:0/SC/Pers/ImeiFile_I1.hash"
};

int sc_file_packet(void* fp,struct sc_file_packet_info_struct* sc_file)
{
    void * single_fp = NULL;
    int i = 0;
    int len;
    int ret;
    void * buf;

    for(i = 0;i<SC_FILE_NUM;i++)
    {
        single_fp = BSP_fopen(sc_file_path[i],"rb");
        if(!single_fp)
            return 1;

        BSP_fseek(single_fp,0,SEEK_END);
        len = BSP_ftell(single_fp);
        BSP_fseek(single_fp,0,SEEK_SET);

        buf = (void*)kmalloc((unsigned int)len,GFP_KERNEL);
        if(!buf){
            BSP_fclose(single_fp);
            printk("malloc failed %s\n",__func__);
            return 2;
        }

        ret = BSP_fread(buf,1,(unsigned int)len,single_fp);
        if(ret != len){
            kfree(buf);
            BSP_fclose(single_fp);
            printk("read signle file failed %s\n",__func__);
            return 3;
        }

        ret = BSP_fwrite(buf,1,(unsigned int)len,fp);
        if(ret != len){
            kfree(buf);
            BSP_fclose(single_fp);
            printk("write packet file  failed %s\n",__func__);
            return 4;
        }

        memcpy(sc_file->file_info[i].path,sc_file_path[i],strlen(sc_file_path[i]));

        sc_file->file_info[i].len = len;
        sc_file->file_info[i].off = sc_file->total_len;
        sc_file->file_info[i].magic_num = SC_FILE_EXIST_MAGIC;

        sc_file->total_len += len;

        kfree(buf);
        (void)BSP_fclose(single_fp);
        buf = NULL;
        single_fp = NULL;
    }
    return 0;
}

int sc_file_back2factory(char* path)
{
    int len;
    void* fp = NULL;
    void* buf = NULL;
    int ret;

    fp = BSP_fopen(path,"rb");
    if(!fp)
        return 1;

    BSP_fseek(fp,0,SEEK_END);
    len = BSP_ftell(fp);
    BSP_fseek(fp,0,SEEK_SET);

    buf = (void*)kmalloc((unsigned int)len,GFP_KERNEL);
    if(!buf){
        BSP_fclose(fp);
        return 2;
    }

    ret = BSP_fread(buf,1,(unsigned int)len,fp);
    if(ret != len){
        kfree(buf);
        BSP_fclose(fp);
        return 3;
    }
    BSP_fclose(fp);

    ret = bsp_nand_write((char*)SC_BACKUP_SEC_NAME,SC_BACKUP_SEC_OFFSET,buf,(unsigned int)len);
    if(ret){
        printk("%s :write patrition fail ,ret :%d\n",__func__,ret);
        kfree(buf);
        return 4;
    }

    kfree(buf);
    return 0;
}
/*lint -save -e813*/
int sc_file_backup(void)
{
    int ret;
    struct sc_file_packet_info_struct sc_file;
    void * fp = NULL;

    memset(&sc_file,0,sizeof(sc_file));
    sc_file.total_len = sizeof(sc_file);

    /*创建标志文件*/
    fp = BSP_fopen((char*)SC_FILE_BACKUP_FLAG,"wb+");
    if(!fp){
    }else{
        BSP_fclose(fp);
        fp = NULL;
    }

    fp = BSP_fopen((char*)SC_PACKET_TEMP_FILE,"wb+");
    if(!fp)
        return 1;

    /*写入文件头*/
    ret = BSP_fwrite(&sc_file,1,(unsigned int)sizeof(sc_file),fp);
    if(ret != (int)sizeof(sc_file)){
        printk("write packet head fail ret :%d!\n",ret);
        BSP_fclose(fp);
        BSP_remove((char*)SC_PACKET_TEMP_FILE);
        return -1;
    }

    ret = sc_file_packet(fp,&sc_file);
    if(ret){
        printk("sc_file_packet fail ret :0x%x!\n",ret);
        BSP_fclose(fp);
        return 2;
    }


    sc_file.magic_num = SC_FILE_PACKET_MAGIC;
    /*组装包头写入文件*/
    BSP_fseek(fp,0,SEEK_SET);

    ret = BSP_fwrite(&sc_file,1,(unsigned int)sizeof(sc_file),fp);
    if(ret != (int)sizeof(sc_file)){
        printk("write packet head err ret :0x%x!\n",ret);
        BSP_fclose(fp);
        return 3;
    }

    BSP_fclose(fp);

    ret = sc_file_back2factory((char*)SC_PACKET_TEMP_FILE);
    if(ret){
        printk("sc_file_back2factory fail ret :0x%x!\n",ret);
        return 4;
    }

    /*删除标志*/
    if(BSP_access((char*)SC_FILE_BACKUP_FLAG,0)){
    }else{
    BSP_remove((char*)SC_FILE_BACKUP_FLAG);
    }

    /*删除组装临时文件*/
    BSP_remove((char*)SC_PACKET_TEMP_FILE);

    return 0;
}


bool get_sc_file_back_state(void)
{
    struct sc_file_packet_info_struct sc_file;
    int ret;
    int i;

    /*标志文件存在返回false*/
    if(!BSP_access((char*)SC_FILE_BACKUP_FLAG,0)){
        printk("%s is exist!\n",SC_FILE_BACKUP_FLAG);
        return false;
    }

    ret = bsp_nand_read((char*)SC_BACKUP_SEC_NAME,SC_BACKUP_SEC_OFFSET,&sc_file,sizeof(sc_file),NULL);
    if(ret){
        printk("%s bsp_nand_read err ret :0x%x!\n",__func__,ret);
        return false;
    }

    if(sc_file.magic_num != SC_FILE_PACKET_MAGIC){
        printk(" %s magic num is invalid!\n",__func__);
        return false;
    }

    for(i= 0;i<SC_FILE_NUM;i++)
    {
        if(sc_file.file_info[i].magic_num != SC_FILE_EXIST_MAGIC){
            printk("%d  file is not exist !\n",i);
            return false;
        }
    }

    return true;
}
/*lint -restore*/

int sc_file_restore_from_factory(struct sc_file_info* file_info)
{
    int ret;
    void* fp = NULL;
    void* buf = NULL;
    unsigned int off = SC_BACKUP_SEC_OFFSET +(unsigned int)file_info->off;



    buf = (void*)kmalloc((unsigned int)(file_info->len),GFP_KERNEL);
    if(!buf)
        return 1;

    ret = bsp_nand_read((char*)SC_BACKUP_SEC_NAME,off,buf,(unsigned int)(file_info->len),NULL);
    if(ret){
        printk("%s : off = 0x%x,len = 0x%x\n",__func__,off,file_info->len);
        kfree(buf);
        return 2;
    }

    fp = BSP_fopen(file_info->path,"wb+");
    if(!fp){
        printk("open %s fail !\n",file_info->path);
        kfree(buf);
        return 3;
    }

    ret = BSP_fwrite(buf,1,(unsigned int)(file_info->len),fp);
    kfree(buf);
    BSP_fclose(fp);

    if(ret != file_info->len){
        printk("write %s fail,ret :0x%x,len :0x%x\n",__func__,ret,file_info->len);
        BSP_remove(file_info->path);
        return 4;
    }

    return 0;
}

/*lint -save -e813*/
int sc_file_restore(void)
{
    int i;
    int ret;
    struct sc_file_packet_info_struct sc_file;


    if(false == get_sc_file_back_state()){
        return -1;
    }

    ret = bsp_nand_read((char*)SC_BACKUP_SEC_NAME,SC_BACKUP_SEC_OFFSET,&sc_file,sizeof(sc_file),NULL);
    if(ret){
        printk("%s bsp_nand_read err ret :0x%x!\n",__func__,ret);
        return ret;
    }

    for(i=0;i<SC_FILE_NUM;i++)
    {
        ret = sc_file_restore_from_factory(&sc_file.file_info[i]);
        if(ret){
            return ret;
        }
    }
    return 0;
}
/*lint -restore*/
/*lint -restore*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



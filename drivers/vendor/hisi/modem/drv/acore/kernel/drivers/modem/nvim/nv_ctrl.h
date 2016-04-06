
#ifndef _NV_CTRL_H_
#define _NV_CTRL_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e537*/
#include "nv_comm.h"
#include "nv_file.h"
/*lint -restore +e537*/

typedef enum
{
    NV_FILE_STOR_FS     = 0,
    NV_FILE_STOR_NON_FS,
    NV_FILE_STOR_BUTT
}NV_FILE_STOR_TYPE_ENUM;

struct nv_file_p
{
    NV_FILE_STOR_TYPE_ENUM stor_type;
    void* fd;
};


typedef FILE* (*file_open)(const s8 * path,const s8* mode);
typedef s32  (*file_close)(FILE* fp);
typedef s32  (*file_read)(u8* ptr,u32 size,u32 count,FILE* fp);
typedef s32  (*file_write)(u8* ptr,u32 size,u32 count,FILE* fp);
typedef s32  (*file_remove)(const s8* path);
typedef s32  (*file_seek)(FILE* fp,s32 offset,s32 whence);
typedef s32  (*file_ftell)(FILE* fp);
typedef s32  (*file_access)(const s8* path,s32 mode);
typedef s32  (*file_mkdir)(const s8* path);

struct file_ops_table_stru
{
    file_open   fo;
    file_read   fr;
    file_write  fw;
    file_close  fc;
    file_remove frm;
    file_seek   fs;
    file_ftell  ff;
    file_access fa;
    file_mkdir  fm;
};


u32 nv_file_init(void);
FILE* nv_file_open(const s8 * path,const s8* mode);
s32 nv_file_read(u8 * ptr, u32 size, u32 count, FILE * fp);
s32 nv_file_write(u8 * ptr, u32 size, u32 count, FILE * fp);
s32 nv_file_close(FILE * fp);
s32 nv_file_seek(FILE * fp, s32 offset, s32 whence);
s32 nv_file_remove(const s8 * path);
s32 nv_file_ftell(FILE * fp);
s32 nv_file_access(const s8* path,s32 mode);


s32 nv_file_copy(s8 * dst_path, s8 * src_path, bool path);
u32 nv_copy_img2backup(void);
s32 nv_modify_upgrade_flag(bool flag);
bool nv_get_upgrade_flag(void);

/*sc file*/
#define SC_FILE_NUM    12

#define SC_FILE_EXIST_MAGIC    0x37ab65cd
struct sc_file_info
{
    unsigned int magic_num;   /*0x37ab65cd*/
    int len;
    int off;
    char  path[128];
};

#define SC_FILE_PACKET_MAGIC     0xdeacb589
struct sc_file_packet_info_struct
{
    unsigned int magic_num;   /*0xdeacb589*/
    int total_len;
    struct sc_file_info file_info[12];
};


#define SC_PACKET_TEMP_FILE       "/modem_log/sc_packet.bin"
#define SC_FILE_BACKUP_FLAG       "/modem_log/sc_backup.flag"
#define SC_BACKUP_SEC_NAME        NV_DEF_SEC_NAME
#define SC_BACKUP_SEC_OFFSET      0x280000   /*出厂分区 2.5M偏移处*/

int sc_file_backup(void);
int sc_file_restore(void);
bool get_sc_file_back_state(void);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /*_NV_CTRL_H_*/



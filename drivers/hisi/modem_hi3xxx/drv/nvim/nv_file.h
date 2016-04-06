
#ifndef _NV_FILE_H_
#define _NV_FILE_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e537*/
#include <linux/kernel.h>
#include <osl_types.h>
#include "nv_comm.h"
/*lint -restore +e537*/
#ifndef NAND_OK
#define NAND_OK    0
#endif


enum
{
    NV_FILE_DLOAD = 0,
    NV_FILE_BACKUP ,
    NV_FILE_XNV_CARD_1 ,
    NV_FILE_XNV_CARD_2,
    NV_FILE_CUST_CARD_1,
    NV_FILE_CUST_CARD_2,
    NV_FILE_SYS_NV,
    NV_FILE_DEFAULT,
    NV_FILE_XNV_MAP_CARD_1,
    NV_FILE_XNV_MAP_CARD_2,
    NV_FILE_BUTT
};

#define   NV_FILE_EXIST              NV_FILE_USING   /*nv*/
#define   NV_DLOAD_INVALID_FLAG      0xe601cdba



/*API ENUM*/
enum
{
    NV_FILE_INIT_API = 0,
    NV_FILE_OPEN_API = 1,
    NV_FILE_CLOSE_API= 2,
    NV_FILE_READ_API = 3,
    NV_FILE_WRITE_API= 4,
    NV_FILE_WRITE_API_IMG = 5,
    NV_FILE_WRITE_API_BAK = 6,
    NV_FILE_WRITE_API_DEF = 7,
    NV_FILE_SEEK_API = 8,
    NV_FILE_FTELL_API= 9,
    NV_FILE_REMOVE_API = 10,
    NV_FILE_OPS_MAX_API
};

enum
{
    NV_GOOD_SECTION,
    NV_BAD_SECTION
};


struct nv_file_info_stru
{
    u32 magic_num;      /*标识文件   0x766e(nv): 有*/
    u32 off;            /*file offset in one section*/
    u32 len;            /*file lenght */
};

/*dload file packet head*/
struct nv_dload_packet_head_stru
{

    struct nv_file_info_stru nv_bin;     /*nv.bin文件信息*/
    struct nv_file_info_stru xnv_xml[2]; /*双卡xnv.xml文件信息*/
    struct nv_file_info_stru cust_xml[2];/*双卡cust.xml文件信息*/
    struct nv_file_info_stru xnv_map[2];/*双卡xnv.xml map文件信息*/
};


#define NV_FLASH_NULL                        (0xffffffff)
#define NV_FLASH_VALID_DATA                  (0x0)
#define NV_FLASH_FILL                        (0xff)    /*mtd write one page,need to fill invality data*/

/*8bit nand,blocksize = 128k*/
#define NV_BIN_FILE_BLOCK_NUM         8            /*bin文件长度当前最大896k*/
#define NV_DLOAD_FILE_BLOCK_NUM       16           /*dload 分区长度2M,块数最多16块*/

struct nv_sec_file_block_info
{
    u32 nv_dload[NV_DLOAD_FILE_BLOCK_NUM];
    u32 sys_nv[NV_DLOAD_FILE_BLOCK_NUM];
    u32 nv_bak[NV_BIN_FILE_BLOCK_NUM];
    u32 nv_default[NV_BIN_FILE_BLOCK_NUM];
};


#if defined(FEATURE_NV_FLASH_ON)

struct nv_nand_info_stru
{
    u32 page_size;
    u32 block_size;
    u32 total_size;
};

struct nv_flash_global_ctrl_stru
{
    struct nv_dload_packet_head_stru nv_dload;  /*data info in dload flag*/
    struct nv_file_info_stru  sys_nv;
    struct nv_file_info_stru  bak_sec;
    struct nv_file_info_stru  def_sec;
};


struct nv_flash_file_header_stru
{
    void* fp;          /*file head pointer*/
    u32   flash_type;  /*flash type*/
    u32   off;         /*offset in this flash*/
    u32   seek;        /*flash seek*/
    u32   ops;         /*ops count*/
    u32   length;      /*file length,not section length*/
    u32   image;       /*nv img id :ptable_com.h*/
    s8    path[64];    /*file path*/
    s8    name[16];    /*patrition name*/
    struct mtd_info *mtd;  /*mtd device*/
    osl_sem_id file_sem;
};

#define NV_IMG_SEC_NAME                PTABLE_NVDLOAD_NM
#define NV_BACK_SEC_NAME               PTABLE_NVBACK_NM
#define NV_DLOAD_SEC_NAME              PTABLE_NVDLOAD_NM
#define NV_DEF_SEC_NAME                PTABLE_NVDEFAULT_NM

u32 nv_flash_init(void);
FILE* nv_flash_open(const s8 * path,const s8* mode);
s32   nv_flash_close(FILE * fp);
s32   nv_flash_ftell(FILE * fp);
s32   nv_flash_read(u8 * ptr, u32 size, u32 count, FILE * fp);
s32   nv_flash_write(u8 * ptr, u32 size, u32 count, FILE * fp);
s32   nv_flash_remove(const s8 * path);
s32   nv_flash_seek(FILE * fp, s32 offset, s32 whence);
s32   nv_flash_access(const s8 * path, s32 mode);

#elif defined(FEATURE_NV_LFILE_ON)

FILE* nv_lfile_open(const s8 * path,const s8* mode);
s32   nv_lfile_close(FILE * fp);
s32   nv_lfile_ftell(FILE * fp);
s32   nv_lfile_read(u8 * ptr, u32 size, u32 count, FILE * fp);
s32   nv_lfile_write(u8 * ptr, u32 size, u32 count, FILE * fp);
s32   nv_lfile_remove(const s8 * path);
s32   nv_lfile_seek(FILE * fp, s32 offset, s32 whence);

#elif  defined(FEATURE_NV_RFILE_ON)

FILE* nv_rfile_open(const s8 * path,const s8* mode);
s32   nv_rfile_close(FILE * fp);
s32   nv_rfile_ftell(FILE * fp);
s32   nv_rfile_read(u8 * ptr, u32 size, u32 count, FILE * fp);
s32   nv_rfile_write(u8 * ptr, u32 size, u32 count, FILE * fp);
s32   nv_rfile_remove(const s8 * path);
s32   nv_rfile_seek(FILE * fp, s32 offset, s32 whence);

#elif  defined(FEATURE_NV_EMMC_ON)

#define FSZ                             long unsigned int

#ifdef HI3630_FASTBOOT_MODEM

#define NV_IMG_SEC_NAME                "block2mtd: /dev/block/mmcblk0p24"
#define NV_BACK_SEC_NAME               "block2mtd: /dev/block/mmcblk0p23"
#define NV_DLOAD_SEC_NAME              "block2mtd: /dev/block/mmcblk0p29"
#define NV_DEF_SEC_NAME                "block2mtd: /dev/block/mmcblk0p7"
#define NV_SYS_SEC_NAME                "block2mtd: /dev/block/mmcblk0p25"

#else

#define NV_IMG_SEC_NAME                "block2mtd: /dev/block/mmcblk0p11"
#define NV_BACK_SEC_NAME               "block2mtd: /dev/block/mmcblk0p10"
#define NV_DLOAD_SEC_NAME              "block2mtd: /dev/block/mmcblk0p20"
#define NV_DEF_SEC_NAME                "block2mtd: /dev/block/mmcblk0p6"
#define NV_SYS_SEC_NAME                "block2mtd: /dev/block/mmcblk0p12"

#endif

struct nv_emmc_info_stru
{
    u32 page_size;
    u32 block_size;
    u32 total_size;
};

struct nv_emmc_global_ctrl_stru
{
    struct nv_dload_packet_head_stru nv_dload;  /*data info in dload flag*/
    struct nv_file_info_stru  sys_nv; /*img all section info*/
    struct nv_file_info_stru  bak_sec;
    struct nv_file_info_stru  def_sec;
};


struct nv_emmc_file_header_stru
{
    void* fp;          /*file head pointer*/
    u32   emmc_type;   /*emmc type*/
    u32   off;         /*offset in this flash*/
    u32   seek;        /*flash seek*/
    u32   ops;         /*ops count*/
    u32   length;      /*file length,not section length*/
    s8    path[64];    /*file path*/
    s8    name[64];    /*patrition name*/
    struct mtd_info *mtd;  /*mtd device*/
    osl_sem_id file_sem;
};

u32   nv_emmc_init(void);

FILE* nv_emmc_open(const s8 * path,const s8* mode);
s32   nv_emmc_close(FILE * fp);
s32   nv_emmc_ftell(FILE * fp);
s32   nv_emmc_read(u8 * ptr, u32 size, u32 count, FILE * fp);
s32   nv_emmc_write(u8 * ptr, u32 size, u32 count, FILE * fp);
s32   nv_emmc_remove(const s8 * path);
s32   nv_emmc_seek(FILE * fp, s32 offset, s32 whence);
s32   nv_emmc_access(const s8 * path, s32 mode);

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif /*_NV_FILE_H_*/


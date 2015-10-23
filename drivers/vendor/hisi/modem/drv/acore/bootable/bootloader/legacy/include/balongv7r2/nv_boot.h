

#ifndef __NV_BOOT_H__
#define __NV_BOOT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -save -e537*/
#include <types.h>
#include <ptable_com.h>
#include <bsp_shared_ddr.h>
#include <boot/flash.h>
#include <bsp_nandc.h>
#include <bsp_nvim.h>
#include <nandc_balong.h>
/*lint -restore +e537*/

#define NV_BACK_SEC_NAME               PTABLE_NVBACK_NM
#define NV_DLOAD_SEC_NAME              PTABLE_NVDLOAD_NM
#define NV_DEF_SEC_NAME                PTABLE_NVDEFAULT_NM


#define NV_FLASH_NULL                        (0xffffffff)
#define NV_FLASH_VALID_DATA                  (0x0)
#define NV_FLASH_FILL                        (0xff)    /*mtd write one page,need to fill invality data*/

#ifndef NAND_OK
#define NAND_OK    0
#endif

#define  printf              cprintf

void *alloc(unsigned sz);
void free(void * addr);
#define  nv_malloc(a)        alloc(a)
#define  nv_free(p)          free(p)

/*file type*/
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


enum
{
    NV_GOOD_SECTION,
    NV_BAD_SECTION
};

/*flash file exist flag*/
#define   NV_FILE_EXIST              NV_FILE_USING

struct nv_file_info_stru
{
    u32 magic_num;                  /*file valid flag*//*标识文件   0x766e(nv): 有*/
    u32 off;                        /*file offset in flash*/
    u32 len;                        /*length of the data in this section */
};

struct nv_dload_packet_head_stru
{
    struct nv_file_info_stru nv_bin;
    struct nv_file_info_stru xnv_xml[2];
    struct nv_file_info_stru cust_xml[2];
    struct nv_file_info_stru xnv_map[2];
};

struct nv_flash_global_ctrl_stru
{
    struct nv_dload_packet_head_stru nv_dload;  /*data info in dload flag*/
    struct nv_file_info_stru sys_nv; /*img all section info*/
    struct nv_file_info_stru bak_info;
    struct nand_spec  nand_info;
    u32    shared_addr;
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
    const s8    path[64];    /*file path*/
    const s8    name[16];    /*sec name*/
};

void cprintf(const char *fmt, ...);
void *memcpy(void *_dst, const void *_src, unsigned len);
int strlen(const char *s);
void *memset(void *_p, unsigned v, unsigned count);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif






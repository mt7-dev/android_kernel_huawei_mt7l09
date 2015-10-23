

#ifndef __NV_BALONG_H__
#define __NV_BALONG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "types.h"
#include "bsp_shared_ddr.h"

/*global ddr distribution*/
/*
   ------------------------512k----------------------------
   --------------------------------------------------------
   | glb  | ctrl |file |ref  |file 1|file 2|..... |file n|
   | info | info |list |list |data  |data  |..... |data  |
   | 1k   |      |     |     |      |      |      |      |
   --------------------------------------------------------
*/



#define NV_GLOBAL_START_ADDR         SHM_MEM_NV_ADDR
#define NV_GLOBAL_END_ADDR           SHM_MEM_NV_ADDR + SHM_MEM_NV_SIZE

/*reg global info*/
#define NV_GLOBAL_INFO_ADDR         NV_GLOBAL_START_ADDR
#define NV_GLOBAL_INFO_SIZE         0x400               /*1 K*/

#define NV_GLOBAL_CTRL_INFO_ADDR    (NV_GLOBAL_INFO_ADDR+NV_GLOBAL_INFO_SIZE)
#define NV_GLOBAL_CTRL_INFO_SIZE    (sizeof(struct nv_ctrl_file_info_stru))

#define NV_GLOBAL_FILE_LIST_ADDR    (NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE)
#define NV_GLOBAL_FILE_ELEMENT_SIZE (sizeof(struct nv_file_list_info_stru))

#define NV_REF_LIST_ITEM_SIZE       (sizeof(struct nv_ref_data_info_stru))

#define NV_MAX_FILE_SIZE            (SHM_MEM_NV_SIZE - NV_GLOBAL_INFO_SIZE)
/****************************************************************/

/****************************************************************/

#ifndef NV_OK
#define NV_OK           (0)
#endif

#ifndef NV_ERROR
#define NV_ERROR        (0xffffffff)
#endif

#define NV_IMEI_LEN              15

#ifndef FILE
#define FILE    void
#endif

#ifndef NULL
#define NULL    (void*)0
#endif

/*sim card type*/
typedef enum
{
    NV_USIMM_CARD_1 = 1,
    NV_USIMM_CARD_2 = 2,
}NV_DOUBLE_CARD_TYPE_ENUM;

/*init state*/
enum
{
    NV_BOOT_INIT_FAIL = 0,
    NV_INIT_FAIL,
    NV_KERNEL_INIT_DOING,
    NV_BOOT_INIT_OK,
    NV_INIT_OK
};

/*file ops*/
#ifndef  SEEK_SET
#define  SEEK_SET        0
#endif
#ifndef  SEEK_CUR
#define  SEEK_CUR        1
#endif
#ifndef  SEEK_END
#define  SEEK_END        2
#endif

#define NV_FILE_READ                 "rb"
#define NV_FILE_WRITE                "wb+"

#define B_READ                        strcmp(NV_FILE_READ,NV_FILE_READ)
#define B_WRITE                       strcmp(NV_FILE_READ,NV_FILE_WRITE)


#define NV_FILE_MAX_NUM                        20
#define NV_MAX_UNIT_SIZE                       2048

#define NV_SYS_NV_PATH                          "/system/nvimg.bin"
#define NV_BACK_PATH                            "/system/nvbak.bin"
#define NV_DLOAD_PATH                           "/system/nvdload.bin"
#define NV_XNV_CARD1_PATH                       "/system/xnv_card1.xml"
#define NV_XNV_CARD1_MAP_PATH                   "/system/xnv_card1.map"
#define NV_XNV_CARD2_PATH                       "/system/xnv_card2.xml"
#define NV_XNV_CARD2_MAP_PATH                   "/system/xnv_card2.map"
#define NV_CUST_CARD1_PATH                      "/system/cust_card1.xml"
#define NV_CUST_CARD2_PATH                      "/system/cust_card2.xml"
#define NV_DEFAULT_PATH                         "/system/manufactrue_ver.bin"


/* error code */
#define BSP_ERR_NV_BASE                         0x100f0000
#define BSP_ERR_NV_MEM_INIT_FAIL                (BSP_ERR_NV_BASE + 0x1)
#define BSP_ERR_NV_INVALID_PARAM                (BSP_ERR_NV_BASE + 0x2)
#define BSP_ERR_NV_MALLOC_FAIL                  (BSP_ERR_NV_BASE + 0x3)
#define BSP_ERR_NV_ERASER_FAIL                  (BSP_ERR_NV_BASE + 0x4)
#define BSP_ERR_NV_FREE_BUF_FAIL                (BSP_ERR_NV_BASE + 0x5)
#define BSP_ERR_NV_NO_FILE                      (BSP_ERR_NV_BASE + 0x6)
#define	BSP_ERR_NV_READ_FILE_FAIL               (BSP_ERR_NV_BASE + 0x7)
#define	BSP_ERR_NV_WRITE_FILE_FAIL              (BSP_ERR_NV_BASE + 0x8)
#define	BSP_ERR_NV_FTELL_FILE_FAIL              (BSP_ERR_NV_BASE + 0x9)
#define	BSP_ERR_NV_SEEK_FILE_FAIL               (BSP_ERR_NV_BASE + 0xa)
#define	BSP_ERR_NV_REMOVE_FILE_FAIL             (BSP_ERR_NV_BASE + 0xb)
#define	BSP_ERR_NV_CLOSE_FILE_FAIL              (BSP_ERR_NV_BASE + 0xc)
#define	BSP_ERR_NV_READ_DATA_FAIL               (BSP_ERR_NV_BASE + 0xd)
#define	BSP_ERR_NV_BACKUP_FILE_FAIL             (BSP_ERR_NV_BASE + 0xe)
#define	BSP_ERR_NV_BACKUP_DATA_FAIL             (BSP_ERR_NV_BASE + 0xf)
#define	BSP_ERR_NV_UPDATE_FILE_FAIL             (BSP_ERR_NV_BASE + 0x10)
#define	BSP_ERR_NV_UPDATE_DATA_FAIL             (BSP_ERR_NV_BASE + 0x11)
#define	BSP_ERR_NV_SECTION_NOT_SUPPORT_UPDATE   (BSP_ERR_NV_BASE + 0x12)
#define	BSP_ERR_NV_NO_WRITE_RIGHT               (BSP_ERR_NV_BASE + 0x13)
#define	BSP_ERR_NV_WRITE_DATA_FAIL              (BSP_ERR_NV_BASE + 0x14)
#define	BSP_ERR_NV_FILE_ERROR                   (BSP_ERR_NV_BASE + 0x15)
#define	BSP_ERR_NV_NO_THIS_ID                   (BSP_ERR_NV_BASE + 0x16)
#define	BSP_ERR_NV_ICC_CHAN_ERR                 (BSP_ERR_NV_BASE + 0x17)
#define	BSP_ERR_NV_ITEM_LEN_ERR                 (BSP_ERR_NV_BASE + 0x18)
#define	BSP_ERR_NV_RECOVER_BAK_ERR              (BSP_ERR_NV_BASE + 0x19)
#define	BSP_ERR_NV_AUTH_FAIL                    (BSP_ERR_NV_BASE + 0x20)
#define BSP_ERR_NV_DDR_OPS_ERR                  (BSP_ERR_NV_BASE + 0x21)
#define BSP_ERR_NV_OVERTIME_WAIT                (BSP_ERR_NV_BASE + 0x22)


#define NV_BACK_SEC_NAME               "modemnvm_backup"
#define NV_DLOAD_SEC_NAME              "modemnvm_update"
#define NV_DEF_SEC_NAME                "modemnvm_factory"
#define NV_SYS_SEC_NAME                "modemnvm_system"




#define NV_FLASH_NULL                        (0xffffffff)
#define NV_FLASH_VALID_DATA                  (0x0)
#define NV_FLASH_FILL                        (0xff)

#ifndef EMMC_BLOCK_SIZE
#define EMMC_BLOCK_SIZE       512
#endif

#ifndef NAND_OK
#define NAND_OK    0
#endif

#ifndef FSZ
#define FSZ                             long unsigned int
#endif

#define  printf           cprintf
#define  nv_malloc(a)     alloc(a)
#define  nv_free(p)       (0)


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

#define   NV_MID_PRI_LEVEL_NUM   6

/*priority type*/
enum
{
    NV_HIGH_PRIORITY = 0,
    NV_MID_PRIORITY1,
    NV_MID_PRIORITY2,
    NV_MID_PRIORITY3,
    NV_MID_PRIORITY4,
    NV_MID_PRIORITY5,
    NV_MID_PRIORITY6,
    NV_LOW_PRIORITY = 7,
    NV_BUTT_PRIORITY
};


enum
{
    NV_GOOD_SECTION,
    NV_BAD_SECTION
};

enum
{
    NV_MEM_DATA_INVALID = 0,   /*无效数据*/
    NV_MEM_DATA_NVDLOAD,       /*升级包数据*/
    NV_MEM_DATA_NVBACK,        /*备份区数据*/
    NV_MEM_DATA_NVSYS_IMG,     /*系统分区数据*/
};

/*flash file is using?*/
#define NV_FILE_USING                     0x766e       /*nv*/
/*flash file exist flag*/
#define   NV_FILE_EXIST                   NV_FILE_USING


/*内存数据类型*/
#define  NV_MEM_DLOAD                   0x12345678
#define  NV_MEM_SYSTEM                  0xabcde123
#define  NV_MEM_BACKUP                  0xdce582e1


#define   NV_CTRL_FILE_MAGIC_NUM         0x224e4944
#define   NV_XML_DEC_SUCC_STATE          0x5687dcfe

/* NV Ctrl File Header Struct */
struct nv_ctrl_file_info_stru
{
    u32  magicnum;
    u32  ctrl_size;        /*ctrl file size*/
    u8   version[2];       /*file version*/
    u8   modem_num;        /*modem num*/
    u8   reserved1;        /*reserved*/
    u32  file_offset;      /*Offset of the File list start address*/
    u32  file_num;         /* File num at file list*/
    u32  file_size;        /* File list size*/
    u32  ref_offset;       /* Offset of the NV reference data start address*/
    u32  ref_count;        /* NV ID num*/
    u32  ref_size;         /* NV reference data size*/
    u8   reserve2[12];
    u32  timetag[4];       /*time stamp*/
    u32  product_version[8];/*product version*/
};

/* NV File List Struct */
struct nv_file_list_info_stru
{
    u32  file_id;       /* NV File ID */
    u8   file_name[28]; /* NV File Name */
    u32  file_size;     /* NV File size*/
};

/* NV reference Data Struct */
struct nv_ref_data_info_stru
{
    u16  itemid;     /* NV ID */
    u16  nv_len;     /* NV Length */
    u32  nv_off;     /* Offset of the NV ID start address at the Common NV file */
    u16  file_id;    /* File ID */
    u16  priority;   /* NV ID priority*/
    u8   modem_num;  /* pid num */
    u8   reserved[3];   /* reserve*/
};

struct nv_global_file_handle_stru
{
    u32 file_id;
    u32 offset;                 /*compare with the ctrl file start addr*/
    u32 size;
};

/*common file head*/
struct nv_comm_file_head_stru
{
    u32 magic_num;
    u32 lookup_off;
    u32 lookup_num;
    u32 lookup_len;
    u32 nvdata_len;
    u32 reserved1[3];
    u32 time_tag[4];
    u32 reserved2[4];
};

/*common file lookup stru*/
struct nv_comm_file_lookup_stru
{
    u16 itemid;
    u16 nv_len;
    u32 nv_off;
    u16 priority;
    u8  modem_num;
    u8  reserved;
};

struct nv_fastboot_debug_stru
{
    u32 line;
    u32 ret;
    u32 reseverd1;
    u32 reseverd2;
};

/*nv global ddr info, size < 1k*/
struct nv_global_ddr_info_stru
{
    u32 ddr_read;               /*whether ddr can to write*/
    u32 xml_dec_state;               /*fastboot xml dec state*/
    u32 acore_init_state;         /*acore init state,after kernel init OK ,then start next step */
    u32 ccore_init_state;         /*ccore init state,only after acore kernel init ok or init all ok*/
    u32 mcore_init_state;
    u32 mem_file_type;
    u32 priority[NV_MID_PRI_LEVEL_NUM];         /*reg the priority sum*/
    u32 cur_time;
    u32 file_num;                                  /*file list file num*/
    u32 file_len;                                  /*file toatl len,include ctrl file*/
    u32 mem_file_len;                            /*currently file len in ddr*/
    struct nv_global_file_handle_stru file_info[NV_FILE_MAX_NUM];  /*reg every file size&offset*/
    struct nv_fastboot_debug_stru  fb_debug;
};


/*structure of nand chip specification parameter */
struct nand_spec
{
    u32 blocksize;          /*one block size of nand flash*/
    u32 pagesize;           /*one page size of nand flash*/
    u32 chipsize;           /*the total size of nand flash*/
    u32 sparesize;           /*the total size of nand flash*/

    u32 pagenumperblock;
	u32 ecctype;            /* selected by nandc, not nand itself */
	u32 buswidth;
	u32 addrnum;
};

/*dload file packet head*/
struct nv_file_info_stru
{
    u32 magic_num;      /*标识文件   0x766e(nv): 有*/
    u32 off;
    u32 len;
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

/*fastboot init*/
u32 modem_nv_init(void);
u32 bsp_nvm_read(u32 itemid, u8 * pdata, u32 datalen);
u32 nv_boot_read_from_mem(u8 * pdata, u32 size, u32 file_id, u32 offset);
u32 nv_boot_write_to_mem(u8 * pdata, u32 size, u32 file_id, u32 offset);
u32 nv_search_byid(u32 itemid, u8 * pdata, struct nv_ref_data_info_stru * ref_info, struct nv_file_list_info_stru * file_info);
FILE* nv_emmc_open(const s8 * path, const s8 * mode);
s32 nv_emmc_read(u8 * ptr, u32 size, u32 count, FILE * fp);
s32 nv_emmc_access(s8 * path, s32 mode);
s32 nv_emmc_ftell(FILE * fp);
s32 nv_emmc_seek(FILE * fp, s32 offset, s32 whence);
s32 nv_emmc_close(FILE * fp);





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif



#ifndef __BSP_NVIM_H__
#define __BSP_NVIM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -save -e537*/
#include "bsp_nvim_mem.h"
#include "drv_nvim.h"
#if (defined(__KERNEL__) || defined(__VXWORKS__) || (defined(__CMSIS_RTOS)))
#include "osl_common.h"
#include "bsp_om.h"
#define BSP_ERR_NV_BASE                         BSP_DEF_ERR(BSP_MODU_NV, 0)
#else
#include "types.h"
#define BSP_ERR_NV_BASE                         0x100f0000
#endif
/*lint -restore +e537*/

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
#define NV_ERROR        0xffffffff
#endif

#ifndef FILE
#define FILE    void
#endif

#ifndef NULL
#define NULL    (void*)0
#endif


/*外部组件读取双卡双待数据sim卡数据使用一下宏定义*/
#ifndef MODEM_ID_0
#define MODEM_ID_0          0   /*卡1参数*/
#endif

#ifndef MODEM_ID_1
#define MODEM_ID_1          1   /*卡2参数*/
#endif

#ifndef MODEM_ID_BUTT
#define MODEM_ID_BUTT       2
#endif


/*底软读取双卡双待nv项请使用下面的枚举定义*/
/*sim card type*/
typedef enum
{
    NV_USIMM_CARD_1 = 1,       /*卡1参数*/
    NV_USIMM_CARD_2 = 2,       /*卡2参数*/
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


/*func type*/
enum
{
    NV_API_READ = 0,     /*-- 0 --*/
    NV_FUN_READ_EX = 1,
    NV_API_AUTHREAD = 2,
    NV_API_DIRECTREAD = 3,
    NV_API_WRITE = 4,
    NV_FUN_WRITE_EX = 5,
    NV_API_AUTHWRITE = 6,
    NV_API_DIRECTWRITE = 7,
    NV_API_GETLEN = 8,
    NV_API_AUTHGETLEN = 9,
    NV_API_FLUSH = 10,
    NV_API_REVERT = 11,
    NV_API_BACKUP = 12,
    NV_API_REVERT_MANF = 13,
    NV_API_REVERT_USER = 14,
    NV_API_IMPORT = 15,
    NV_API_EXPORT = 16,
    NV_FUN_MEM_INIT = 17,
    NV_FUN_UPGRADE_PROC = 18,
    NV_FUN_KERNEL_INIT = 19,
    NV_FUN_REMAIN_INIT = 20,
    NV_FUN_NVM_INIT = 21,
    NV_FUN_XML_DECODE = 22,
    NV_FUN_SEARCH_NV = 23,
    NV_FUN_REVERT_DATA = 24,
    NV_FUN_UPDATE_DEFAULT = 25,
    NV_FUN_REVERT_DEFAULT = 26,
    NV_FUN_ERR_RECOVER = 27,
    NV_FUN_KEY_CHECK   = 28,
    NV_FUN_FLUSH_SYS   = 29,
    NV_FUN_MAX_ID,
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


#define NV_FILE_USING                0x766e       /*nv*/

/*内存数据类型*/
#define  NV_MEM_DLOAD                   0x12345678
#define  NV_MEM_SYSTEM                  0xabcde123
#define  NV_MEM_BACKUP                  0xdce582e1

#define NV_CTRL_FILE_MAGIC_NUM       0x224e4944   /*文件头标志位*/
#define NV_XML_DEC_SUCC_STATE        0x5687dcfe   /*fastboot 阶段xml解析是否成功标志*/


#define NV_FILE_READ                 "rb"
#define NV_FILE_RW                   "rb+"
#define NV_FILE_WRITE                "wb+"

#define B_READ                        strcmp(NV_FILE_READ,NV_FILE_READ)
#define B_WRITE                       strcmp(NV_FILE_READ,NV_FILE_WRITE)
#define B_RW                          strcmp(NV_FILE_READ,NV_FILE_RW)


#define NV_FILE_MAX_NUM                        20
#define NV_MAX_UNIT_SIZE                       2048

#define NV_FILE_SYS_NV_PATH                     "/system/sys_nv.bin"
#define NV_BACK_PATH                            "/system/nvbak.bin"
#define NV_DLOAD_PATH                           "/system/nvdload.bin"
#define NV_XNV_CARD1_PATH                       "/system/xnv_card1.xml"
#define NV_XNV_CARD1_MAP_PATH                   "/system/xnv_card1.map"
#define NV_XNV_CARD2_PATH                       "/system/xnv_card2.xml"
#define NV_XNV_CARD2_MAP_PATH                   "/system/xnv_card2.map"
#define NV_CUST_CARD1_PATH                      "/system/cust_card1.xml"
#define NV_CUST_CARD2_PATH                      "/system/cust_card2.xml"
#define NV_DEFAULT_PATH                         "/system/manufactrue_ver.bin"


#ifdef BSP_CONFIG_HI3630
#define NV_IMG_PATH                             "/mnvm2:0/modem_nv/nv.bin"
#define NV_FACTORY_RIGHT_PATH                   "/mnvm2:0/modem_nv/factory.right"
#define NV_IMG_FLAG_PATH                        "/modem_log/modem_nv/img_flag.bin"
#define NV_BACK_FLAG_PATH                       "/modem_log/modem_nv/back_flag.bin"
#define NV_SYS_FLAG_PATH                        "/modem_log/modem_nv/sys_flag.bin"

#define NV_LOG_PATH                             "/modem_log/NvLog.txt"

#define NV_LOG_MAX_SIZE                         0x80000/*512k*/
#else
#define NV_IMG_PATH                             "/mnvm2:0/nv.bin"
#define NV_FACTORY_RIGHT_PATH                   "/mnvm2:0/factory.right"
#define NV_IMG_FLAG_PATH                        "/modem_log/img_flag.bin"
#define NV_BACK_FLAG_PATH                       "/modem_log/back_flag.bin"
#define NV_SYS_FLAG_PATH                        "/modem_log/sys_flag.bin"

#define NV_LOG_PATH                             "/modem_log/NvLog.txt"
#define NV_LOG_MAX_SIZE                         0x4000/*16k*/
#endif



/* error code */

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
#define BSP_ERR_NV_NAND_ALL_BAD                 (BSP_ERR_NV_BASE + 0x23)

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
    NV_MEM_DATA_INVALID = 0,   /*无效数据*/
    NV_MEM_DATA_NVDLOAD,       /*升级包数据*/
    NV_MEM_DATA_NVBACK,        /*备份区数据*/
    NV_MEM_DATA_NVSYS_IMG,     /*系统分区数据*/
};



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
    u32 xml_dec_state;               /*xml dec state*/
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



u32 nv_readEx(u32 modem_id, u32 itemid, u32 offset, u8 * pdata, u32 datalen);
u32 nv_writeEx(u32 modem_id, u32 itemid, u32 offset, u8 * pdata, u32 datalen);

u32 bsp_nvm_dcread(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen);
u32 bsp_nvm_dcwrite(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen);

u32 bsp_nvm_dcreadpart(u32 modem_id, u32 itemid, u32 offset, u8 * pdata, u32 datalen);
u32 bsp_nvm_dcwritepart(u32 modem_id, u32 itemid, u32 offset, u8 * pdata, u32 datalen);

u32 bsp_nvm_dcread_direct(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen);
u32 bsp_nvm_dcwrite_direct(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen);

u32 bsp_nvm_auth_dcread(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen);
u32 bsp_nvm_auth_dcwrite(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen);


#if defined(INSTANCE_1)

#define bsp_nvm_read(itemid,pdata,datalen)             bsp_nvm_dcread(NV_USIMM_CARD_2,itemid,pdata,datalen)
#define bsp_nvm_write(itemid,pdata,datalen)            bsp_nvm_dcwrite(NV_USIMM_CARD_2,itemid,pdata,datalen)

#define bsp_nvm_readpart(itemid,offset,pdata,datalen)  bsp_nvm_dcreadpart(NV_USIMM_CARD_2,itemid,offset,pdata,datalen)
#define bsp_nvm_writepart(itemid,offset,pdata,datalen) bsp_nvm_dcwritepart(NV_USIMM_CARD_2,itemid,offset,pdata,datalen)

#define bsp_nvm_read_direct(itemid,pdata,datalen)      bsp_nvm_dcread_direct(NV_USIMM_CARD_2,itemid,pdata,datalen)
#define bsp_nvm_write_direct(itemid,pdata,datalen)     bsp_nvm_dcwrite_direct(NV_USIMM_CARD_2,itemid,pdata,datalen)

#define bsp_nvm_authread(itemid,pdata,datalen)         bsp_nvm_auth_dcread(NV_USIMM_CARD_2,itemid,pdata,datalen)
#define bsp_nvm_authwrite(itemid,pdata,datalen)        bsp_nvm_auth_dcwrite(NV_USIMM_CARD_2,itemid,pdata,datalen)


#else

#define bsp_nvm_read(itemid,pdata,datalen)             bsp_nvm_dcread(NV_USIMM_CARD_1,itemid,pdata,datalen)
#define bsp_nvm_write(itemid,pdata,datalen)            bsp_nvm_dcwrite(NV_USIMM_CARD_1,itemid,pdata,datalen)

#define bsp_nvm_readpart(itemid,offset,pdata,datalen)  bsp_nvm_dcreadpart(NV_USIMM_CARD_1,itemid,offset,pdata,datalen)
#define bsp_nvm_writepart(itemid,offset,pdata,datalen) bsp_nvm_dcwritepart(NV_USIMM_CARD_1,itemid,offset,pdata,datalen)

#define bsp_nvm_read_direct(itemid,pdata,datalen)      bsp_nvm_dcread_direct(NV_USIMM_CARD_1,itemid,pdata,datalen)
#define bsp_nvm_write_direct(itemid,pdata,datalen)     bsp_nvm_dcwrite_direct(NV_USIMM_CARD_1,itemid,pdata,datalen)

#define bsp_nvm_authread(itemid,pdata,datalen)         bsp_nvm_auth_dcread(NV_USIMM_CARD_1,itemid,pdata,datalen)
#define bsp_nvm_authwrite(itemid,pdata,datalen)        bsp_nvm_auth_dcwrite(NV_USIMM_CARD_1,itemid,pdata,datalen)


#endif

u32 bsp_nvm_backup(void);

/*将所有数据刷到文件系统中*/
u32 bsp_nvm_flush(void);

/*创建式将所有数据刷到文件系统中，慎用*/
u32 bsp_nvm_flushEn(void);

/*将内存中偏移为off，长度为len的数据刷到文件系统中*/
u32 bsp_nvm_flushEx(u32 off,u32 len,u32 itemid);

/*将内存中的数据刷到sys nv中*/
u32 bsp_nvm_flushSys(u32 itemid);

u32 bsp_nvm_get_len(u32 itemid, u32 * length);
u32 bsp_nvm_authgetlen(u32 itemid, u32 * len);

u32 bsp_nvm_revert_default(void);

u32 bsp_nvm_update_default(void);

u32 bsp_nvm_revert(void);

u32 bsp_nvm_revert_user(void);

u32 bsp_nvm_revert_manufacture(void);

u32 bsp_nvm_xml_decode(void);

u32 nvm_read_rand(u32 nvid);
u32 nvm_read_randex(u32 nvid, u32 modem_id);

u32 bsp_nvm_get_nv_num(void);
u32 bsp_nvm_get_nvidlist(NV_LIST_INFO_STRU*  nvlist);

/*ccore/mcore init*/
u32 bsp_nvm_init(void);

s32 bsp_nvm_kernel_init(void);
s32 bsp_nvm_remain_init(void);

/*char device */
s32 nvchar_init(void);

/*fastboot init*/
u32 nv_init(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /*__BSP_NVIM_H__*/






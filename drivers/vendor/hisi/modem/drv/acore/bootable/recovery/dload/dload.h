/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  dload.h
*
*   作    者 :  wuzechun
*
*   描    述 :  dload.c 的头文件
*
*   修改记录 :  2012年12月13日  v1.00  wuzechun  创建
*
*************************************************************************/
#ifndef __DLOAD_H__
#define __DLOAD_H__
/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/
 #include "dload_show.h"
 #include "ptable_com.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include "hdlc.h"

#define DEBUG
 
#ifdef DEBUG

extern pthread_mutex_t Dbug_lock;
#  define  Dbug(...)                                      \
            do {                                           \
                    int save_errno = errno;                \
                    pthread_mutex_lock(&Dbug_lock);               \
                    fprintf(stderr, "%s::%s():",           \
                            __FILE__, __FUNCTION__);       \
                    errno = save_errno;                    \
                    fprintf(stderr, __VA_ARGS__ );         \
                    fflush(stderr);                        \
                    pthread_mutex_unlock(&Dbug_lock);             \
                    errno = save_errno;                    \
            } while (0)
    
#else
#  define  Dbug(...)          ((void)0)
#endif

#define OK (0)
#ifndef ERROR
#define ERROR (-1)
#endif
#define TRUE (1)
#define FALSE (0)
#define NULL  0

#ifndef isupper
#define isupper(c)      (((c) >= 'A') && ((c) <= 'Z'))
#endif

#ifdef BCMDRIVER
#include <osl.h>
#include <bcmutils.h>
#define strtoul(nptr, endptr, base) bcm_strtoul((nptr), (endptr), (base))
#define tolower(c) (isupper((c)) ? ((c) + 'a' - 'A') : (c))
#else

#endif
#define bool unsigned long


#define RECV_BUF_SIZE 0x80000
#define SEND_BUF_SIZE 0x400

#define VER_LEN_MAX                 30              /*MNTN接口固定为30*/
#define CMD_VERIFY_LEN              20              /*该命令长度由工具固定*/
#define CMD_TCPU_VER_LEN            24              /*该命令长度由工具固定*/
#define CMD_BCPU_VER_LEN            64              /*该命令长度由工具固定*/
//#define CMD_VERIFY_FRONT_VAL        "HUAWEI "   /*最后一个字符是空格,zfh test i2c*/
#define CMD_VERIFY_FRONT_VAL "HUAWEI "

#define HDLC_SEND_BUF_MAX_LEN   0x400

#define CMD_VERIFY_LEN              20              /*该命令长度由工具固定*/
#define CMD_TCPU_VER_LEN            24              /*该命令长度由工具固定*/
#define CMD_BCPU_VER_LEN            64              /*该命令长度由工具固定*/
#define STUB_STR_HW_VER "DLOADID_STUB"
#define DEV_ACM_AT_NAME "/dev/acm_at"

/* 升级文件数据以块对齐的buffer为单位，依次写入flash() */
//#define IMG_DLOAD_DIRECT_BLOCK_ALIGN    0x00

/* 升级文件数据是一块完整的buffer，并且接受外部模块传过来的数据被动填充,最后整块写入flash*/
//#define IMG_DLOAD_PASSIVE_WHOLE_BUFF    0x01
#define IMAGE_NAME_SIZE 25

#define HDLC_CMD_NUM 20
#define HDLC_CMD_LEN 8
#define HDLC_CMD_RSP_NUM 25
#define HDLC_PACKET_MAXLEN  ((8*1024) + 512)


#define	CMD_ACK_LEN 5
#define	CMD_NAK_INVALID_FCS_LEN 	7
#define	CMD_NAK_INVALID_DEST_LEN	7
#define	CMD_NAK_INVALID_LEN_LEN	7
#define	CMD_NAK_EARLY_END_LEN		7
#define	CMD_NAK_TOO_LARGE_LEN	8
#define	CMD_NAK_INVALID_CMD_LEN	7
#define	CMD_NAK_FAILED_LEN			7
#define	CMD_NAK_WRONG_IID_LEN	7
#define	CMD_NAK_BAD_VPP_LEN		7
#define	CMD_NAK_VERIFY_FAILED_LEN	7
#define	CMD_NAK_NO_SEC_CODELEN	7
#define	CMD_NAK_BAD_SEC_CODE_LEN	7
#define	CMD_Reserved_LEN			7
#define	CMD_NAK_OP_NOT_PERMITTED_LEN 7
#define	CMD_NAK_INVALID_ADDR_LEN	7
#define	CMD_DLOAD_SWITCH_LEN		5
#define	CMD_CDROM_STATUS_LEN		6

/*--------------------------------------------------------------*
 * 数据结构                                                     *
 *--------------------------------------------------------------*/
 
#if 0
/* 镜像区域 */
enum image_type
{
//zstart
    IMAGE_SFS_BOTTOM            = 0x0,        /*静态文件 开始 */
    IMAGE_CDROMISOVER          = 0x1,
    IMAGE_WEBUIVER             = 0x2,   
    IMAGE_SFS_TOP              = 0x50,      /*静态文件结束*/
    

//zend

    /* 三 、flash分区映像类型 , 这个分区包含所有需要下载升级的映像标志 */
    IMAGE_PART_START           = 0xff,      /*255*//*分区表索引开始*/

    IMAGE_PTABLE                = 0x100,     /* 分区表 镜像标志*/
    IMAGE_M3BOOT               = 0x101,     /* M3 BOOT */
    IMAGE_FASTBOOT             = 0x102,     /* FASTBOOT镜像 区 */
    IMAGE_NVBACKLTE            = 0x108,     /* LTE NV 备份 镜像区*/ 
    IMAGE_NVBACKGU             = 0x104,     /* GU  NV 备份 镜像区*/
    IMAGE_NVDLD                 = 0x105,     /* 下载NV 区 */
    IMAGE_NVIMG                 = 0x106,     /* 工作NV 区 */
    IMAGE_NVFACTORY            = 0x115,      /* 工厂NV 区 */
    IMAGE_RECOVERY		= 0x107,     /* RECOVERY镜像 区  */
    IMAGE_MCORE                 = 0x103,     /* MCORE VWXORKS 区 */
    IMAGE_DSP                   = 0x109,     /* DSP 镜像 区 */
    IMAGE_ACORE                = 0x10A,     /* ACORE LINUX 区 */
    IMAGE_SYSTEM               = 0x10B,     /* SYSTEM.IMG 镜像 区 */
    IMAGE_MISC                  = 0x10C,     /* MISC 镜像 区 */
    IMAGE_CACHE                 = 0x10D,     /* CACHE 镜像 区 */
    IMAGE_OEMINFO             = 0x10E,      /* OEMONFO 镜像 区 */

    IMAGE_DATA                  = 0x10F,     /* DATA.IMG镜像 区 */
	IMAGE_APP                   = 0x110,     /* APP.IMG 镜像 区 */	
    IMAGE_ONLINE               = 0x111,     /* ONLINE 镜像 区 */
    IMAGE_WEBUI               = 0x112,     /* WEBUI 镜像 区 */
    IMAGE_CDROMISO             = 0x113,     /* CDROM ISO 镜像区*/ 
	IMAGE_LOGO	               = 0x114,     /* LOGO 区 */
    IMAGE_RECOVERY_B          = 0x116,     /* RECOVERY镜像 区  */
    IMAGE_PART_TOP             = 0x200      /*分区表索引 结束*/
};
#endif
/*HDLC USB交互命令*/
enum dload_cmd
{
    CMD_ACK                  = 0x02,    /*正常响应, UE->PC*/
    CMD_RESET                = 0x0A,    /*复位命令, PC->UE*/
    CMD_VERREQ               = 0x0C,    /*软件信息请求命令, PC->UE*/
    CMD_VERRSP               = 0x0D,    /*软件信息返回响应, UE->PC*/
    CMD_DLOAD_SWITCH         = 0x3A,    /*下载模式切换虚拟命令, UE<->PC*/
    CMD_DLOAD_INIT           = 0x41,    /*一次下载初始化命令, PC->UE*/
    CMD_DATA_PACKET          = 0x42,    /*下载数据包传输命令, PC->UE*/
    CMD_DLOAD_END            = 0x43,    /*结束一次下载命令, PC->UE*/
    CMD_VERIFY               = 0x45,    /*产品ID查询与返回命令, UE<->PC*/
    CMD_TCPU_VER             = 0x46,    /*单板软件版本查询与返回命令,仅存在于强制加载模式,UE<->PC*/  
    CMD_BCPU_VER             = 0x4B,    /*单板软件版本查询与返回命令,仅存在于bootrom重启模式,UE<->PC*/  
    CMD_CDROM_STATUS         = 0xFB,    /*CDROM状态查询, 仅存在于强制加载模式,UE<->PC*/
    
    /*非法响应, UE->PC*/  
    CMD_NAK_INVALID_FCS      = 0x0301,    /*CRC校验错误*/  
    CMD_NAK_INVALID_DEST     = 0x0302,    /*目标内存空间超出范围*/ 
    CMD_NAK_INVALID_LEN      = 0x0303,    /*接收到的与预置的长度不一致*/ 
    CMD_NAK_EARLY_END        = 0x0304,    /*数据长度过短*/ 
    CMD_NAK_TOO_LARGE        = 0x0305,    /*数据长度过长*/ 
    CMD_NAK_INVALID_CMD      = 0x0306,    /*无效的命令*/ 
    CMD_NAK_FAILED           = 0x0307,    /*操作无法完成*/ 
    CMD_NAK_WRONG_IID        = 0x0308,    /*未使用*/ 
    CMD_NAK_BAD_VPP          = 0x0309,    /*未使用*/ 
    CMD_NAK_VERIFY_FAILED    = 0x030A,    /*鉴权失败,没有权限操作*/ 
    CMD_NAK_NO_SEC_CODE      = 0x030B,    /*没有安全码*/ 
    CMD_NAK_BAD_SEC_CODE     = 0x030C,    /*安全码错误/无效*/
    CMD_Reserved             = 0x030D,    /*Reserved */
    CMD_NAK_OP_NOT_PERMITTED = 0x030E,    /*现特性不允许该操作*/ 
    CMD_NAK_INVALID_ADDR     = 0x030F,    /*此地址的内存无法访问*/ 
    
    DLOAD_CMD_BUF
};

/*用于存储iso_ver和webui_ver*/
struct version_image_type{
	char buf[IMAGE_NAME_SIZE];
	unsigned int len;
};

struct dload_buf_ctrl
{
    char  *data;               /*buffer数据区，大小为1block*/
    unsigned int used;                 /*buffer中有效数据的长度*/  
    unsigned int size;                /*buffer申请空间的长度*/  
    unsigned int type;                /*数据映像类型*/
};

/*DCB表*/
struct dload_ctrl_block
{
    enum dload_status dload_state;             /*当前升级状态*/ 
    enum IMAGE_TYPE image_type;                 /*当前升级部件的映像类型*/
    unsigned int        image_size;                 /*当前升级部件的大小*/
    unsigned int        flash_current_offset;         /*当前包在FLASH中的烧写地址相对分区首地址偏移,仅FLASH映像相关*/
    unsigned int        ram_addr;                   /*当前接收到的包在RMA中的源地址*/ 
    unsigned int        current_loaded_length;      /*当前接收到的包长度*/
    unsigned int        last_loaded_total_length;   /*上次为止已收到的包总长度*/
    unsigned int        last_loaded_packet;         /*上次已收到的包序号*/
    struct dload_buf_ctrl write_buf;
    enum IMAGE_TYPE finish_image_type;	
    int                 reserved[4];              /*保留*/ 
    
};
struct dload_hdlc
{
    unsigned int  u32Mode;                       /* HDLC解析使用的内部参数，HDLC负责维护 */
    unsigned int  u32Length;                     /* HDLC解析时内部维护的中间变量 */
    unsigned int  u32DecapBufSize;               /* HDLC解析内容存放BUFFER的长度，调用者负责指定 */
    unsigned int  u32InfoLen;                    /* HDLC解析成功的数据长度，HDLC负责维护 */
    unsigned char au8DecapBuf[HDLC_PACKET_MAXLEN];                   /* HDLC解析内容存放地址，由调用者负责指定空间 */
};

enum hdlc_cmd_id
{
	CMD_ACK_ID=0,
	CMD_NAK_INVALID_FCS_ID,
	CMD_NAK_INVALID_DEST_ID,
	CMD_NAK_INVALID_LEN_ID,
	CMD_NAK_EARLY_END_ID,
	CMD_NAK_TOO_LARGE_ID,
	CMD_NAK_INVALID_CMD_ID,
	CMD_NAK_FAILED_ID,
	CMD_NAK_WRONG_IID_ID,
	CMD_NAK_BAD_VPP_ID,
	CMD_NAK_VERIFY_FAILED_ID,
	CMD_NAK_NO_SEC_CODE_ID,
	CMD_NAK_BAD_SEC_CODE_ID,
	CMD_Reserved_ID,
	CMD_NAK_OP_NOT_PERMITTED_ID,
	CMD_NAK_INVALID_ADDR_ID,
	CMD_DLOAD_SWITCH_ID,
	CMD_CDROM_STATUS_ID
};
/*--------------------------------------------------------------*
 * 函数原型声明                                                 *
 *--------------------------------------------------------------*/

extern void dload_cleanup(void);
extern int dload_main(void);

#endif /* dload.h */

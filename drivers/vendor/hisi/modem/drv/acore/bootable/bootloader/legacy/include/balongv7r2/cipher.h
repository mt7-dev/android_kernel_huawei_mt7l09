#ifndef _CIPHER_DRV_SHAONLY_H
#define _CIPHER_DRV_SHAONLY_H


/*************************CIPHER START**********************************/
/* 错误码 */
#define CIPHER_SUCCESS                  0
#define CIPHER_IS_CHECK_PARAM		//函数里的宏，是否做函数传入参数检查的开关

/*寄存器地址*/
#define	CIPHER_REGBASE_ADDR		0x900AB000//基地址
#define CIPHER_KEYRAM_OFFSET       0x800
#define CIPHER_CHN_RESET(i)   (0x80 + (0x80*(i)))
#define CIPHER_CHN_ENABLE(i)  (0x84 + (0x80*(i)))
#define CIPHER_CHN_CONFIG(i)  (0x88 + (0x80*(i)))
#define CIPHER_CHNBDQ_BASE(i)  (0x8c + (0x80*(i)))
#define CIPHER_CHNBDQ_SIZE(i)  (0x90 + (0x80*(i)))
#define CIPHER_CHNBDQ_RWPTR(i) (0x94 + (0x80*(i)))
#define CIPHER_CHNBDQ_RDPTR(i) (0x98 + (0x80*(i)))
#define CIPHER_CHNBDQ_WRPTR(i) (0x9c + (0x80*(i)))
#define CIPHER_CHNBDQ_BDNUM(i) (0xa0 + (0x80*(i)))
#define CIPHER_CHNBDQ_BADBDNUM(i) (0xa4 + (0x80*(i)))
#define CIPHER_CHNRDQ_BASE(i)  (0x480 + (0x80*(i)))
#define CIPHER_CHNRDQ_SIZE(i)  (0x484 + (0x80*(i)))
#define CIPHER_CHNRDQ_RWPTR(i) (0x488 + (0x80*(i)))
#define CIPHER_CHNRDQ_RDPTR(i) (0x48C + (0x80*(i)))
#define CIPHER_CHNRDQ_WRPTR(i) (0x490 + (0x80*(i)))

#define CHN_IV_SEL                 0x0
#define CHN_RDQ_CTRL               0x1
#define CHN_RDQCTRL_BIT            0x2
#define CHN_USRFIELDLEN_BIT        0x5

#define CIPHER_KEY_NUM             0x10
#define CIPHER_KEY_LEN             0x20
#define CIPHER_TIMEOUT_TICKS       100
#define CIPHER_WAIT_THREHLD        20

#define CIPHER_BSWAP32(val) \
        (((val) >> 24) | (((val) >> 8) & 0x0000ff00) | \
        (((val) << 8) & 0x00ff0000) | ((val) << 24))

#define KDF_CHN_NUM            0x4
#define KDF_CHN_USRFIELD_LEN   0x0
#define KDF_SHA_KEY_LENGTH     0x20
#define KDF_SHA_S_LENGTH_MAX   0x200

typedef enum tagKDF_SHA_KEY_SOURCE_E
{
    SHA_KEY_SOURCE_DDR,             //sha_key来源于input point指定的地址空间
    SHA_KEY_SOURCE_KEYRAM,          //sha_key来源于内部KeyRam
    SHA_KEY_SOURCE_FOLLOW,          //sha_key来源于上次KDF操作的sha_key
    SHA_KEY_SOURCE_RESULT,          //sha_key来源于上一次KDF操作的结果
    SHA_KEY_SOURCE_MAX
}KDF_SHA_KEY_SOURCE_E;

typedef enum tagKDF_SHA_S_SOURCE_E
{
    SHA_S_SOURCE_DDR,               //sha_s来源于input point指定的地址空间
    SHA_S_SOURCE_KEYRAM,            //sha_s来源于内部KeyRam
    SHA_S_SOURCE_MAX
}KDF_SHA_S_SOURCE_E;

/* CIPHER密钥长度, 注意枚举值与寄存器设置值一致 */
typedef enum tagCIPHER_KEY_LEN_E
{
    CIPHER_KEY_L128 = 128,        /* 密钥长度128 bit */
    CIPHER_KEY_L192 = 192,        /* 密钥长度192 bit */
    CIPHER_KEY_L256 = 256,        /* 密钥长度256 bit */
    CIPHER_KEY_LEN_BUTTOM
}CIPHER_KEY_LEN_E;

/* CIPHER通知类型 */
typedef enum tagCIPHER_NOTIFY_STAT_E
{
    CIPHER_STAT_OK = 0,           /* 成功完成 */
    CIPHER_STAT_CHECK_ERR = 1,    /* 完整性检查错误 */
    CIPHER_STAT_BUTTOM
}CIPHER_NOTIFY_STAT_E;

typedef struct tagS_CONFIG_INFO_S
{
    KDF_SHA_S_SOURCE_E   enShaSSource;      //sha_s来源
    unsigned int              u32ShaSIndex;       //sha_s来源于KeyRam时，其在keyRam中的索引
    unsigned int              u32ShaSLength;     //sha_s的长度
    void             *pSAddr;           //
}S_CONFIG_INFO_S;

typedef enum tagCIPHER_KEY_OUTPUT_E
{
    CIPHER_KEY_NOT_OUTPUT,    //不输出密钥
    CIPHER_KEY_OUTPUT,        //输出密钥
    CIPHER_KEY_OUTPUT_BUTTOM
}CIPHER_KEY_OUTPUT_E;

typedef unsigned int CIPHER_KEY_LEN_E_U32;
typedef unsigned int CIPHER_KEY_OUTPUT_E_U32;

/* 获取Key长度和Key内容的结构体*/
typedef struct
{
    CIPHER_KEY_LEN_E_U32    enKeyLen;//要读取的Key的长度，由上层传入
    void                *pKeyAddr;//Key值存放地址
    CIPHER_KEY_LEN_E_U32    *penOutKeyLen;//该地址用于存放实际返回的Key的长度
}KEY_GET_S;

/* KeyMake时，获取Key的长度和Key值*/
typedef struct
{
    CIPHER_KEY_OUTPUT_E_U32 enKeyOutput; //指示是否输出Key到stKeyGet中
    KEY_GET_S stKeyGet;
}KEY_MAKE_S;

typedef struct tagKEY_CONFIG_INFO_S
{
    KDF_SHA_KEY_SOURCE_E enShaKeySource;    //sha_key来源
    unsigned int              u32ShaKeyIndex;     //sha_key来源于KeyRam时，其在keyRam中的索引
    void             *pKeySourceAddr;   //
}KEY_CONFIG_INFO_S;

/* KDF使用的BD结构*/
typedef struct tagKDF_BD_SCPT_S
{
    unsigned int u32KdfConfig;
    unsigned int u32Address;
}KDF_BD_SCPT_S;

/* KDF使用的RD结构*/
typedef struct tagKDF_RD_SCPT_S
{
    unsigned int u32Address;
    unsigned int u32KdfConfig;
}KDF_RD_SCPT_S;

typedef struct tagKDF_BDCONFIG_INFO_S
{
    KDF_SHA_KEY_SOURCE_E enShaKeySource;    //sha_key来源
    KDF_SHA_S_SOURCE_E   enShaSSource;      //sha_s来源
    unsigned int              u32ShaKeyIndex;     //sha_key来源于KeyRam时，其在keyRam中的索引
    unsigned int              u32ShaSIndex;       //sha_s来源于KeyRam时，其在keyRam中的索引
    unsigned int              u32Length;         //KDF运算作为sha_s的长度
    unsigned int              u32DestIndex;      //sha_result_sn在key_ram中的位置
    void *           pAddress;          //目的(源)地址
}KDF_BDCONFIG_INFO_S;

#define	BDQ_SIZE	(8*2) //sizeof(KDF_BD_SCPT_S)*2
#define	RDQ_SIZE	(8*2) //sizeof(KDF_RD_SCPT_S)*2
#pragma pack(8)
    struct Data_Buff
    {
	    unsigned char BD_BUFF[BDQ_SIZE];
	    unsigned char RD_BUFF[RDQ_SIZE];
    };
#pragma pack()

int bsp_calc_hash(unsigned long * s_addr, long size, unsigned long * key_addr, unsigned long * out_hash, unsigned long * out_hash_len);

/**************************************************************************
  宏定义
**************************************************************************/
/* 注意CIPHER错误码要为负值 */
#define CIPHER_ERROR_BASE               0x10004000

typedef enum tagCIPHER_ERR_CODE_E
{
    CIPHER_ERR_CODE_NULL_PTR = 1,
    CIPHER_ERR_CODE_NO_MEM ,
    CIPHER_ERR_CODE_NOT_INIT ,
    CIPHER_ERR_CODE_FIFO_FULL ,
    CIPHER_ERR_CODE_INVALID_CHN ,
    CIPHER_ERR_CODE_INVALID_OPT ,
    CIPHER_ERR_CODE_ALIGN_ERROR ,
    CIPHER_ERR_CODE_PURGING ,
    CIPHER_ERR_CODE_TIME_OUT,
    CIPHER_ERR_CODE_INVALID_ENUM,
    CIPHER_ERR_CODE_INVALID_RD,
    CIPHER_ERR_CODE_RD_NULL,
    CIPHER_ERR_CODE_INVALID_KEY,
    CIPHER_ERR_CODE_CHECK_ERROR,
    CIPHER_ERR_CODE_BDLEN_ERROR,
    CIPHER_ERR_CODE_INVALID_NUM,
    CIPHER_ERR_CODE_NO_KEY,
    CIPHER_ERR_CODE_KEYLEN_ERROR
}CIPHER_ERR_CODE_E;

#define CIPHER_NULL_PTR      ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NULL_PTR))
#define CIPHER_NO_MEM        ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_MEM))
#define CIPHER_NOT_INIT      ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NOT_INIT))
#define CIPHER_FIFO_FULL     ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_FIFO_FULL))
#define CIPHER_INVALID_CHN   ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_CHN))
#define CIPHER_INVALID_OPT   ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_OPT))
#define CIPHER_ALIGN_ERROR   ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_ALIGN_ERROR))
#define CIPHER_PURGING       ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_PURGING))
#define CIPHER_TIME_OUT      ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_TIME_OUT))
#define CIPHER_INVALID_ENUM  ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_ENUM))
#define CIPHER_INVALID_RD    ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_RD))
#define CIPHER_RDQ_NULL      ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_RD_NULL))
#define CIPHER_INVALID_KEY   ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_KEY))
#define CIPHER_CHECK_ERROR   ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_CHECK_ERROR))
#define CIPHER_BDLEN_ERROR   ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_BDLEN_ERROR))
#define CIPHER_INVALID_NUM   ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_NUM))
#define CIPHER_NO_KEY        ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_KEY))
#define CIPHER_KEYLEN_ERROR  ((int)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_KEYLEN_ERROR))
#define CIPHER_UNKNOWN_ERROR ((int)(CIPHER_ERROR_BASE|0xff))

#endif

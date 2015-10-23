
#ifndef _CIPHER_COMN_H
#define _CIPHER_COMN_H


#include <semLib.h>
#include <logLib.h>
#include <cacheLib.h>
#include <soc_interrupts.h>
#include <osl_types.h>
#include <osl_sem.h>
#include <bsp_om.h>
#include <soc_memmap.h>
#include <hi_cipher.h>


/*=============================宏定义=================================*/
#define CIPHER_REGBASE_ADDR HI_CIPHER_BASE_ADDR  /*cipher寄存器基地址*/

#define CIPHER_IS_CHECK_PARAM/*定义这个宏后会在函数开始执行前做传入参数检查*/
#ifndef WAIT_FOREVER
#define  WAIT_FOREVER   (-1)
#endif
#ifndef CIPHER_SUCCESS
#define CIPHER_SUCCESS   0/* 错误码 */
#endif

#define CIPHER_FALSE    (0)
#define CIPHER_TRUE     (1)
#define CIPHER_OK       (0)
#define CIPHER_ERROR    (-1)
#define CHN_IV_SEL                 0x0
#define CHN_RDQ_CTRL               0x1
#define CHN_RDQCTRL_BIT            0x2
#define CHN_USRFIELD_LEN           0x3
#define CHN_USRFIELDLEN_BIT        0x5
#define CIPHER_CONFIG_AEC_CTR      0x7
#define CIPHER_CONFIG_MAC_LEN      0x0
#define CIPHER_CHN_PRI             0x0      /*除通道4外，通道0优先级最高*/
#define CHN_PRI_BIT                28

/*内存分配相关*/
#define CIPHER_IS_CACHED_MEM 0       /*是否使用CACHE存储空间，都为0则使用cacheDma*/
#define CIPHER_SFIFO_BASE_ALIGN  32  /* SFIFO基地址要求64bit(8byte)对齐(考虑到刷Cache的问题32字节对齐) */
#define CIPHER_BUF_LIST_ALIGN    32  /* Buf List基地址最好64bit(8byte)对齐(考虑到刷Cache的问题32字节对齐)  */
#if CIPHER_IS_CACHED_MEM
    /* 需要多分配出32字节的内存为对齐预留 */
    #define CIPHER_MALLOC(size) memalign(CIPHER_SFIFO_BASE_ALIGN, (size) + CIPHER_SFIFO_BASE_ALIGN)
    #define CIPHER_FREE(ptr) free(ptr)
    #define CIPHER_FLUSH_CACHE(ptr, size) \
        cacheFlush(DATA_CACHE, (void *)(((u32)(ptr)) & (~0x1F)), (((size) + (((u32)(ptr)) & 0x1f) + 31)>>5)<<5)
#else
    /* 不再需要多分配出32字节的内存为对齐预留 */
    #define CIPHER_MALLOC(size) cacheDmaMalloc(size)
    #define CIPHER_FREE(ptr) cacheDmaFree(ptr)
    #define CIPHER_FLUSH_CACHE(ptr, size)
#endif

/*寄存器地址*/
#define CIPHER_CTRL_OFFSET         HI_CIPHER_CTRL_OFFSET         /*0x0*/
#define CIPHER_PRI_OFFSET          HI_CIPHER_PRI_OFFSET          /*0x4*/
#define CIPHER_BDNUM_OFFSET        HI_TOTAL_BD_NUM_OFFSET        /*0x8*/
#define CIPHER_BADBDNUM_OFFSET     HI_INVALID_BD_NUM_OFFSET      /*0xc*/
#define CIPHER_DMA_CFG_OFFSET      HI_DMA_CFG_OFFSET             /*0x14*/
#define CIPHER_INT_PKT_OFFSET      HI_CH_INT_PKT_INTERVAL_OFFSET /*0x400*/
#define CIPHER_INT0_STAT_OFFSET    HI_CIPHER_INT0_STATUS_OFFSET  /*0x408*/
#define CIPHER_INT0_MASK_OFFSET    HI_CIPHER_INT0_MASK_OFFSET    /*0x40c*/
#define CIPHER_INT0_MSSTAT_OFFSET  HI_CIPHER_INT0_MSTATUS_OFFSET /*0x410*/
#define CIPHER_INT1_STAT_OFFSET    HI_CIPHER_INT1_STATUS_OFFSET  /*0x414*/
#define CIPHER_INT1_MASK_OFFSET    HI_CIPHER_INT1_MASK_OFFSET    /*0x418*/
#define CIPHER_INT1_MSSTAT_OFFSET  HI_CIPHER_INT1_MSTATUS_OFFSET /*0x41c*/
#define CIPHER_INT2_STAT_OFFSET    HI_CIPHER_INT2_STATUS_OFFSET  /*0x420*/
#define CIPHER_INT2_MASK_OFFSET    HI_CIPHER_INT2_MASK_OFFSET    /*0x424*/
#define CIPHER_INT2_MSSTAT_OFFSET  HI_CIPHER_INT2_MSTATUS_OFFSET /*0x428*/
#define CIPHER_KEYRAM_OFFSET       HI_KEY_RAM_OFFSET             /*0x800*/
#define CIPHER_CHN_RESET(i)   	   (0x80 + (0x80*(i)))
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

/*翻转字节序*/
#define CIPHER_BSWAP32(val) \
        (((val) >> 24) | (((val) >> 8) & 0x0000ff00) | \
        (((val) << 8) & 0x00ff0000) | ((val) << 24))

/*通道超时间隔*/
#define CIPHER_TIMEOUT_TICKS       100
#define CIPHER_WAIT_THREHLD        20
/*key相关*/
#define CIPHER_KEY_NUM		0x10	/*key ram中key的个数*/
#define CIPHER_KEY_LEN		0x20	/*以字节为单位*/
/* 参数判断相关函数定义*/
#define CIPHER_CHECK_INIT() \
    do{\
        if (sg_module_mgr.status == CIPHER_STAT_NULL)\
        {\
            CIPHER_PRINT_ERR(CIPHER_NOT_INIT); \
            return CIPHER_NOT_INIT;\
        }\
    }while(0)

#define CIPHER_CHECK_CHN_NUM(chn) \
    do{\
        if (((chn) >= CIPHER_MAX_CHN) ||((chn) < CIPHER_MIN_CHN) )\
        {\
            CIPHER_PRINT_ERR(CIPHER_INVALID_CHN); \
            return CIPHER_INVALID_CHN;\
        }\
    }while(0)

#define CIPHER_CHECK_PTR(ptr) \
    do{\
        if (NULL == (ptr))\
        {\
            CIPHER_PRINT_ERR(CIPHER_NULL_PTR); \
            return CIPHER_NULL_PTR;\
        }\
    }while(0)

#define CIPHER_CHECK_BUFLIST_PTR(ptr) \
    do{\
        if (NULL == (ptr))\
        {\
            CIPHER_PRINT_ERR(CIPHER_NULL_PTR); \
            return CIPHER_NULL_PTR;\
        }\
        if (((u32)(ptr) % 4) != 0)\
        {\
            CIPHER_PRINT_ERR(CIPHER_ALIGN_ERROR); \
            return CIPHER_ALIGN_ERROR;\
        }\
    }while(0)

#define CIPHER_CHECK_ENUM(en, last) \
    do{\
        if ((u32)(en) >= (u32)(last))\
        {\
            CIPHER_PRINT_ERR(CIPHER_INVALID_ENUM); \
            return CIPHER_INVALID_ENUM;\
        }\
    }while(0)

#define CIPHER_CHECK_KEYINDEX(keyindex) \
    do{\
        if ((u32)(keyindex) >= CIPHER_KEY_NUM)\
        {\
            CIPHER_PRINT_ERR(CIPHER_INVALID_KEY); \
            return CIPHER_INVALID_KEY;\
        }\
    }while(0)

/* 调试函数实现*/
#define CIPHER_PRINT_ERR(errno) \
do{\
    {\
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_CIPHER, "\t[CIPHER API ERROR]:\n\t\t\t <func>: %s;  <line>: %d;  <Errno>: %s (0x%08x)\n\n", (int)__FUNCTION__, (int)__LINE__, (int)#errno, errno, 0, 0); \
    }\
}while(0)

/*错误处理宏*/
#define CIPHER_ERR_HANDLER(ret, handler) \
do{\
    if (CIPHER_SUCCESS > (ret))\
    {\
        CIPHER_PRINT_ERR(ret);\
        goto handler;\
    }\
}while(0)
#define CIPHER_ERR_HANDLER_BDFULL(ret, handler) \
do{\
    if (CIPHER_SUCCESS > (ret))  \
    {\
        if(ret != CIPHER_FIFO_FULL) \
        {  \
            CIPHER_PRINT_ERR(ret); \
        }  \
        goto handler;\
    }\
}while(0)


/* Mutex 行为封装 */
#define CIPHER_MUTEX_T SEM_ID
#define CIPHER_MTX_CREATE(mtx) \
    do {\
        mtx = semMCreate(SEM_Q_FIFO | SEM_DELETE_SAFE);\
    }while(0)
#define CIPHER_MTX_DELETE(mtx) semDelete(mtx)
#define CIPHER_MTX_ENTER(mtx) semTake (mtx, WAIT_FOREVER)
#define CIPHER_MTX_LEAVE(mtx) semGive (mtx)

/*=============================枚举型定义=============================*/
/* CIPHER初始化状态 */
enum CIPHER_STATUS_E
{
    CIPHER_STAT_NULL = 0x0,         /* 无效状态 */
    CIPHER_STAT_INIT = 0x1          /* 已初始化状态 */
};


#endif

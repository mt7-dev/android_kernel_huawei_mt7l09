

#ifndef __DRV_EDMA_H__
#define __DRV_EDMA_H__

#include "drv_comm.h"
#include "drv_edma_enum.h"

/* v7r1 */
typedef struct tagAXI_DMA_TASK_HANDLE_S
{
	unsigned int u32SrcAddr;
	unsigned int u32DstAddr;
	unsigned int ulLength;
	unsigned int ulChIdx;
}AXI_DMA_TASK_HANDLE_S;
BSP_S32 EDMA_NormTaskLaunch(AXI_DMA_TASK_HANDLE_S * pstDMAHandle);
BSP_S32 EDMA_2VecTaskLaunch(AXI_DMA_TASK_HANDLE_S * pstDMAHandle);
BSP_S32 EDMA_QueryCHNLState(BSP_U32 u32ChIdx);
BSP_S32 EDMA_QueryCHNsLState(BSP_U32 u32ChIdxMap);

/*错误码定义*/
#ifndef OK
#define OK     0
#endif
#ifndef ERROR
#define ERROR (-1)
#endif
/*错误码定义*/

#define DMAC_SUCCESS                       0
#define DMA_FAIL                          (-1)

#define DMA_ERROR_BASE                          -100
#define DMA_CHANNEL_INVALID                     (DMA_ERROR_BASE-1)
#define DMA_TRXFERSIZE_INVALID                  (DMA_ERROR_BASE-2)
#define DMA_SOURCE_ADDRESS_INVALID              (DMA_ERROR_BASE-3)
#define DMA_DESTINATION_ADDRESS_INVALID         (DMA_ERROR_BASE-4)
#define DMA_MEMORY_ADDRESS_INVALID              (DMA_ERROR_BASE-5)
#define DMA_PERIPHERAL_ID_INVALID               (DMA_ERROR_BASE-6)
#define DMA_DIRECTION_ERROR                     (DMA_ERROR_BASE-7)
#define DMA_TRXFER_ERROR                        (DMA_ERROR_BASE-8)
#define DMA_LLIHEAD_ERROR                       (DMA_ERROR_BASE-9)
#define DMA_SWIDTH_ERROR                        (DMA_ERROR_BASE-0xa)
#define DMA_LLI_ADDRESS_INVALID                 (DMA_ERROR_BASE-0xb)
#define DMA_TRANS_CONTROL_INVALID               (DMA_ERROR_BASE-0xc)
#define DMA_MEMORY_ALLOCATE_ERROR               (DMA_ERROR_BASE-0xd)
#define DMA_NOT_FINISHED                        (DMA_ERROR_BASE-0xe)
#define DMA_CONFIG_ERROR                        (DMA_ERROR_BASE-0xf)

/* v9r1 */

typedef void (*channel_isr)(unsigned int channel_arg, unsigned int int_status);

/*定义外设请求号*/
enum edma_req_id
{
    EDMA_SPI0_RX = 0,
    EDMA_SPI0_TX,
    EDMA_SPI1_RX,
    EDMA_SPI1_TX,   /* Acore drv lcd 刷屏           实际分配通道 edma ch16 - 14 */
    EDMA_LTESIO_RX, /* HIFI                         实际分配通道 edma ch16 - 8  */
    EDMA_LTESIO_TX, /* HIFI                         实际分配通道 edma ch16 - 9  */
    RESERVE_0,
    RESERVE_1,
    EDMA_HSUART_RX = 8, /* Acore drv 蓝牙语音场景   实际分配通道 edma ch16 - 12 */
    EDMA_HSUART_TX,     /* Acore drv 蓝牙语音场景   实际分配通道 edma ch16 - 13 */
    EDMA_UART0_RX,
    EDMA_UART0_TX,
    EDMA_UART1_RX,
    EDMA_UART1_TX,
    EDMA_UART2_RX,
    EDMA_UART2_TX,
    EDMA_SCI0_RX = 0x10,    /* Acore drv SIM卡1收   实际分配通道 edma ch16 - 10 */
	EDMA_SCI_RX  = 0x10,
    EDMA_SCI0_TX,           /* Acore drv SIM卡1发   实际分配通道 edma ch16 - 10 */
    EDMA_SCI1_RX,           /* Acore drv SIM卡2收   实际分配通道 edma ch16 - 11 */
    EDMA_SCI1_TX,           /* Acore drv SIM卡2发   实际分配通道 edma ch16 - 11 */

    EDMA_GBBP0_DBG = 0x14,
    EDMA_BBP_DBG   = 0x14, /* gudsp 的请求 bbp搬数  实际分配通道 edma ch16 - 6  */
    EDMA_GBBP1_GRIF = 0x15,
    EDMA_BBP_GRIF   = 0x15,/* gudsp 的请求 bbp采数  实际分配通道 edma ch16 - 6  */

    EDMA_AMON_SOC = 0x16,     /* Acore drv AXIMON   实际分配通道 edma ch16 - 15 */
    EDMA_AMON_CPUFAST = 0x17, /* Acore drv AXIMON   实际分配通道 edma ch16 - 15 */


    /* gudsp 的请求 M2M */
    /* EDMA_GBBP0_DBG      gudsp使用 EDMA_BBP_DBG   实际分配通道 edma ch16 - 6  */
    EDMA_MEMORY_DSP_1 = 0x1A,   /* 用于turbo译码    实际分配通道 edma ch4  - 0  */
    EDMA_MEMORY_DSP_2,          /* 用于viterb译码   实际分配通道 edma ch4  - 1  */
    /*TL与GU复用 ch16 - 2，为保险每次启用前检查当前通道是否空闲***/
    EDMA_MEMORY_DSP_3,          /* 用于CQI表搬移    实际分配通道 edma ch16 - 2  */
    EDMA_MEMORY_DSP_4,          /* 用于上行编码     实际分配通道 edma ch4  - 2  */
    EDMA_MEMORY_DSP_5,          /* 用于APT表搬移    实际分配通道 edma ch4  - 3  */
    EDMA_MEMORY_DSP_6 = 0x1F,   /* GUDSP/AHB加载    实际分配通道 edma ch16 - 7  */

    EDMA_PWC_LDSP_TCM  ,        /* Mcore  drv   实际分配通道 edma ch16 - 0  */
    EDMA_PWC_TDSP_TCM ,         /* Mcore  drv   实际分配通道 edma ch16 - 1  */


    /* TLDSP  的请求定义，没有实际用途 放到MAX前面用于测试0通道 */
    EDMA_LDSP_API_USED_0,       /* LDSP加载专有镜像 实际分配通道 edma ch16 - 0  */

    EDMA_REQ_MAX,               /*如果设备请求不小于此值，则为非法请求*/

    /* TLDSP  的请求定义，没有实际用途 */
    EDMA_LDSP_API_USED_1,       /* LDSP加载专有镜像 实际分配通道 edma ch16 - 1  */
    /*TL与GU复用 ch16 - 2，为保险每次启用前检查当前通道是否空闲***/
    EDMA_LDSP_LCS_SDR,          /* TLDSP LCS        实际分配通道 edma ch16 - 2  */
    EDMA_LDSP_CSU_SDR,          /* LDSP小区搜索     实际分配通道 edma ch16 - 3  */
    EDMA_LDSP_EMU_SDR,          /* LDSP能量测量     实际分配通道 edma ch16 - 4  */
    EDMA_LDSP_NV_LOADING,       /* LDSP动态加载NV   实际分配通道 edma ch16 - 5  */

    EDMA_BUTT

} ;

typedef enum edma_req_id BALONG_DMA_REQ; /*adapt*/
/* adapt for v9r1 */
#define EDMA_MEMORY_DSP EDMA_MEMORY_DSP_1

/*for test*/
#define EDMA_MEMORY  EDMA_LDSP_API_USED_0 /* use ldsp's channel ch16- 0 when testing busstress */


/* 函数void (*channel_isr)(BSP_U32 channel_arg, BSP_U32 int_status)的参数int_status、
    函数int balong_dma_channel_init (BALONG_DMA_REQ req,  channel_isr pFunc,
                     UINT32 channel_arg, UINT32 int_flag)的参数int_flag
    为以下几种中断类型，可组合       */
#define BALONG_DMA_INT_DONE           1          /*DMA传输完成中断*/
#define BALONG_DMA_INT_LLT_DONE       2          /*链式DMA节点传输完成中断*/
#define BALONG_DMA_INT_CONFIG_ERR     4          /*DMA配置错误导致的中断*/
#define BALONG_DMA_INT_TRANSFER_ERR   8          /*DMA传输错误导致的中断*/
#define BALONG_DMA_INT_READ_ERR       16         /*DMA链表读错误导致的中断*/

/* EDMAC传输方向定义*/
#define BALONG_DMA_P2M      1
#define BALONG_DMA_M2P      2
#define BALONG_DMA_M2M      3

/* EDMAC流控制与传输类型*/
typedef enum tagEDMA_TRANS_TYPE
{
    MEM_MEM_DMA = 0x00,    /* 内存到内存，DMA流控*/
    MEM_PRF_DMA = 0x01,        /* 内存与外设，DMA流控*/
    MEM_PRF_PRF = 0x02        /* 内存与外设，外设流控*/
} EDMA_TRANS_TYPE;

/* 通道状态 */

#define   EDMA_CHN_FREE          1   /* 通道空闲 */
#define   EDMA_CHN_BUSY          0   /* 通道忙 */


/* EDMA传输位宽，源、目的地址约束为一致的值 */
#define   EDMA_TRANS_WIDTH_8       0x0   /* 8bit位宽*/
#define   EDMA_TRANS_WIDTH_16      0x1   /* 16bit位宽*/
#define   EDMA_TRANS_WIDTH_32      0x2   /* 32bit位宽*/
#define   EDMA_TRANS_WIDTH_64      0x3   /* 64bit位宽*/

/*  EDMA burst length, 取值范围0~15，表示的burst长度为1~16*/
#define   EDMA_BUR_LEN_1    0x0    /* burst长度，即一次传输的个数为1个*/
#define   EDMA_BUR_LEN_2    0x1    /* burst长度，即一次传输的个数为2个*/
#define   EDMA_BUR_LEN_3    0x2   /* burst长度，即一次传输的个数为3个*/
#define   EDMA_BUR_LEN_4    0x3   /* burst长度，即一次传输的个数为4个*/
#define   EDMA_BUR_LEN_5    0x4   /* burst长度，即一次传输的个数为5个*/
#define   EDMA_BUR_LEN_6    0x5   /* burst长度，即一次传输的个数为6个*/
#define   EDMA_BUR_LEN_7    0x6   /* burst长度，即一次传输的个数为7个*/
#define   EDMA_BUR_LEN_8    0x7   /* burst长度，即一次传输的个数为8个*/
#define   EDMA_BUR_LEN_9    0x8   /* burst长度，即一次传输的个数为9个*/
#define   EDMA_BUR_LEN_10   0x9   /* burst长度，即一次传输的个数为10个*/
#define   EDMA_BUR_LEN_11   0xa   /* burst长度，即一次传输的个数为11个*/
#define   EDMA_BUR_LEN_12   0xb   /* burst长度，即一次传输的个数为12个*/
#define   EDMA_BUR_LEN_13   0xc   /* burst长度，即一次传输的个数为13个*/
#define   EDMA_BUR_LEN_14   0xd   /* burst长度，即一次传输的个数为14个*/
#define   EDMA_BUR_LEN_15   0xe   /* burst长度，即一次传输的个数为15个*/
#define   EDMA_BUR_LEN_16   0xf   /* burst长度，即一次传输的个数为16个*/


/* EDMA 对应的具体位，供EDMA  寄存器配置宏
       EDMAC_BASIC_CONFIG、BALONG_DMA_SET_LLI、BALONG_DMA_SET_CONFIG 使用*/
/*config------Bit 31*/
#define EDMAC_TRANSFER_CONFIG_SOUR_INC      (0X80000000)
/*Bit 30*/
#define EDMAC_TRANSFER_CONFIG_DEST_INC      (0X40000000)
#define EDMAC_TRANSFER_CONFIG_BOTH_INC      (0XC0000000)

/*Bit 27-24*/
#define EDMAC_TRANSFER_CONFIG_SOUR_BURST_LENGTH_MASK     (0xF000000)
#define EDMAC_TRANSFER_CONFIG_SOUR_BURST_LENGTH( _len )  ((unsigned int)((_len)<<24))
/*Bit 23-20*/
#define EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH_MASK     (0xF00000)
#define EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH( _len )  ((unsigned int)((_len)<<20))

/*Bit18-16*/
#define EDMAC_TRANSFER_CONFIG_SOUR_WIDTH_MASK     (0x70000)
#define EDMAC_TRANSFER_CONFIG_SOUR_WIDTH( _len )  ((unsigned int)((_len)<<16))
/*Bit14-12*/
#define EDMAC_TRANSFER_CONFIG_DEST_WIDTH_MASK     (0x7000)
#define EDMAC_TRANSFER_CONFIG_DEST_WIDTH( _len )  ((unsigned int)((_len)<<12))

/*Bit9-4*/
#define EDMAC_TRANSFER_CONFIG_REQUEST( _ulReg )    ( (_ulReg ) << 4)
/*Bit3-2*/
#define EDMAC_TRANSFER_CONFIG_FLOW_DMAC( _len )    ((unsigned int)((_len)<<2))

#define EDMAC_TRANSFER_CONFIG_INT_TC_ENABLE            ( 0x2 )
#define EDMAC_TRANSFER_CONFIG_INT_TC_DISABLE           ( 0x0 )

#define EDMAC_TRANSFER_CONFIG_CHANNEL_ENABLE           ( 0x1 )
#define EDMAC_TRANSFER_CONFIG_CHANNEL_DISABLE          ( 0x0 )
#define EDMAC_NEXT_LLI_ENABLE       0x2           /* Bit 1 */
/*Bit 15*/
#define EDMAC_TRANSFER_CONFIG_EXIT_ADD_MODE_A_SYNC     ( 0UL )
#define EDMAC_TRANSFER_CONFIG_EXIT_ADD_MODE_AB_SYNC    ( 0x00008000 )

/*链式传输时的节点信息*/
typedef struct edma_cb
{
    volatile unsigned int lli;     /*指向下个LLI*/
    volatile unsigned int bindx;
    volatile unsigned int cindx;
    volatile unsigned int cnt1;
    volatile unsigned int cnt0;   /*块传输或者LLI传输的每个节点数据长度 <= 65535字节*/
    volatile unsigned int src_addr; /*物理地址*/
    volatile unsigned int des_addr; /*物理地址*/
    volatile unsigned int config;
} BALONG_DMA_CB __attribute__((aligned(32)));

#define P2M_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_PRF_DMA) | EDMAC_TRANSFER_CONFIG_DEST_INC)
#define M2P_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_PRF_DMA) | EDMAC_TRANSFER_CONFIG_SOUR_INC)
#define M2M_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_MEM_DMA) | EDMAC_TRANSFER_CONFIG_SOUR_INC | EDMAC_TRANSFER_CONFIG_DEST_INC)

#define EDMAC_BASIC_CONFIG(burst_width, burst_len) \
               ( EDMAC_TRANSFER_CONFIG_SOUR_BURST_LENGTH(burst_len) | EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH(burst_len) \
               | EDMAC_TRANSFER_CONFIG_SOUR_WIDTH(burst_width) | EDMAC_TRANSFER_CONFIG_DEST_WIDTH(burst_width) )

/*addr:物理地址*/
#define BALONG_DMA_SET_LLI(addr, last)   ((last)?0:(EDMAC_MAKE_LLI_ADDR(addr) | EDMAC_NEXT_LLI_ENABLE))

#define BALONG_DMA_SET_CONFIG(req, direction, burst_width, burst_len) \
                 ( EDMAC_BASIC_CONFIG(burst_width, burst_len) | EDMAC_TRANSFER_CONFIG_REQUEST(req) \
                 | EDMAC_TRANSFER_CONFIG_INT_TC_ENABLE | EDMAC_TRANSFER_CONFIG_CHANNEL_ENABLE \
                 | ((direction == BALONG_DMA_M2M)?M2M_CONFIG:((direction == BALONG_DMA_P2M)?P2M_CONFIG:M2P_CONFIG)))

/**************************************************************************
  宏定义
**************************************************************************/



/*******************************************************************************
  函数名:      BSP_S32 balong_dma_init(void)
  功能描述:    DMA初始化程序，挂接中断
  输入参数:    无
  输出参数:    无
  返回值:      0
*******************************************************************************/
extern BSP_S32 balong_dma_init(void);

/*******************************************************************************
  函数名:       int balong_dma_current_transfer_address(UINT32 channel_id)
  函数描述:     获得某通道当前传输的内存地址
  输入参数:     channel_id : 通道ID，调用balong_dma_channel_init函数的返回值
  输出参数:     无
  返回值:       成功：通道当前传输的内存地址
                失败：负数
*******************************************************************************/
extern int balong_dma_current_transfer_address(BSP_U32 channel_id);
#define DRV_EDMA_CURR_TRANS_ADDR(channel_id)  balong_dma_current_transfer_address(channel_id)

/*******************************************************************************
  函数名:       int balong_dma_channel_stop(UINT32 channel_id)
  函数描述:     停止指定的DMA通道
  输入参数:     channel_id : 通道ID，调用balong_dma_channel_init函数的返回值
  输出参数:     无
  返回值:       成功：通道当前传输的内存地址
                失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_stop(BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_STOP(channel_id)  balong_dma_channel_stop(channel_id)


/*******************************************************************************
  函数名:      BALONG_DMA_CB *balong_dma_channel_get_lli_addr(UINT32 channel_id)
  函数描述:    获取指定DMA通道的链表控制块的起始地址
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BALONG_DMA_CB *balong_dma_channel_get_lli_addr (BSP_U32 channel_id);
#define DRV_EDMA_CHAN_GET_LLI_ADDR(channel_id)  balong_dma_channel_get_lli_addr(channel_id)


/******************************************************************************
  函数名:      int balong_dma_channel_init (BALONG_DMA_REQ req,
                  channel_isr pFunc, UINT32 channel_arg, UINT32 int_flag)
  函数描述:    根据外设号分配通道，注册通道中断回调函数、初始化传输完成信号量、
               将外设号写入config寄存器
  输入参数:    req : 外设请求号
               pFunc : 上层模块注册的DMA通道中断处理函数，NULL时表明不注册
               channel_arg : pFunc的入参1，
                             pFunc为NULL，不需要设置这个参数
               int_flag : pFunc的入参2, 产生的中断类型，取值范围为
                        BALONG_DMA_INT_DONE、BALONG_DMA_INT_LLT_DONE、
                      ALONG_DMA_INT_CONFIG_ERR、BALONG_DMA_INT_TRANSFER_ERR、
                        BALONG_DMA_INT_READ_ERR之一，或者组合。
                        pFunc为NULL，不需要设置这个参数
  输出参数:    无
  返回值:      成功：通道号
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_init (BALONG_DMA_REQ req, channel_isr pFunc, BSP_U32 channel_arg, BSP_U32 int_flag);
#define DRV_EDMA_CHANNEL_INIT(req, pFunc,channel_arg,int_flag) balong_dma_channel_init(req, pFunc,channel_arg,int_flag)

/*******************************************************************************
  函数名:      int balong_dma_channel_set_config (UINT32 channel_id,
                       UINT32 direction, UINT32 burst_width, UINT32 burst_len)
  函数描述:    非链式DMA传输时，调用本函数配置通道参数
               链式DMA传输时，不需要使用本函数。
  输入参数:    channel_id : 通道ID，调用balong_dma_channel_init函数的返回值
               direction : DMA传输方向, 取值为BALONG_DMA_P2M、BALONG_DMA_M2P、
                           BALONG_DMA_M2M之一
               burst_width：取值为0、1、2、3，表示的burst位宽为8、16、32、64bit
               burst_len：取值范围0~15，表示的burst长度为1~16
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_set_config (BSP_U32 channel_id, BSP_U32 direction,BSP_U32 burst_width, BSP_U32 burst_len);
#define DRV_EDMA_CHANNEL_CONFIG(channel_id, direction,burst_width,burst_len)  balong_dma_channel_set_config(channel_id, direction,burst_width,burst_len)

/*******************************************************************************
  函数名:      int balong_dma_channel_dest_set_config (UINT32 channel_id,
                       UINT32 burst_width, UINT32 burst_len)
  函数描述:    BBP GRIF需要单独配置目的数据位宽和长度，调用本函数，其它情形不使用。
               使用时，该接口在balong_dma_channel_set_config之后调用。
  输入参数:    channel_id：通道ID，调用balong_dma_channel_init函数的返回值
               burst_width：取值为0、1、2、3，表示的burst位宽为8、16、32、64bit
               burst_len：取值范围0~15，表示的burst长度为1~16
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_dest_set_config (BSP_U32 channel_id, BSP_U32 burst_width, BSP_U32 burst_len);
#define DRV_EDMA_CHANNEL_DEST_CONFIG(channel_id, dest_width, dest_len)  balong_dma_channel_dest_set_config(channel_id, dest_width, dest_len)

/*******************************************************************************
  函数名:      int balong_dma_channel_start (UINT32 channel_id, UINT32 src_addr,
                       UINT32 des_addr, UINT32 len)
  函数描述:    启动一次同步DMA传输, DMA传输完成后，才返回
               使用本函数时，不需要注册中断处理函数
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
               src_addr：数据传输源地址，必须是物理地址
               des_addr：数据传输目的地址，必须是物理地址
               len：数据传输长度，单位：字节；一次传输数据的最大长度是65535字节
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_start (BSP_U32 channel_id, BSP_U32 src_addr, BSP_U32 des_addr, BSP_U32 len);
#define DRV_EDMA_CHANNEL_START(channel_id,src_addr,des_addr,len)  balong_dma_channel_start(channel_id,src_addr,des_addr,len)


/*******************************************************************************
  函数名:      int balong_dma_channel_async_start (UINT32 channel_id,
                unsigned int src_addr, unsigned int des_addr, unsigned int len)
  函数描述:    启动一次异步DMA传输。启动DMA传输后，就返回。不等待DMA传输完成。
               使用本函数时，注册中断处理函数，中断处理函数中处理DMA
传输完成事件
               或者，不注册中断处理函数，使用balong_dma_channel_is_idle函数查询
               DMA传输是否完成
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
               src_addr：数据传输源地址，必须是物理地址
               des_addr：数据传输目的地址，必须是物理地址
               len：数据传输长度，单位：字节；一次传输数据的最大长度是65535字节
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_async_start (BSP_U32 channel_id, BSP_U32 src_addr, BSP_U32 des_addr, BSP_U32 len);
#define DRV_EDMA_CHANNEL_ASYNC_START(channel_id,src_addr,des_addr,len) balong_dma_channel_async_start(channel_id,src_addr,des_addr,len)


/*******************************************************************************
  函数名:      int balong_dma_channel_lli_start (UINT32 channel_id)
  函数描述:    启动链式DMA传输。在链式DMA的所有节点传输都全部完成后才返回。
               链式DMA的每个节点的数据最大传输长度为65535字节。
               注意：调用此函数前，必须设置好链表控制块。
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_lli_start (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_lli_START(channel_id)   balong_dma_channel_lli_start(channel_id)

/*******************************************************************************
  函数名:      int balong_dma_channel_lli_start (UINT32 channel_id)
  函数描述:    启动链式DMA传输，然后立即返回，不等待DMA传输完成。
               链式DMA的每个节点的数据最大传输长度为65535字节。
               注意：调用此函数前，必须设置好链表控制块。
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_lli_async_start (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_lli_ASYNC_START(channel_id)   balong_dma_channel_lli_async_start(channel_id)


/******************************************************************************
*
  函数名:       int balong_dma_channel_is_idle (UINT32 channel_id)
  函数描述:     查询DMA通道是否空闲
  输入参数:     channel_id : 通道ID，调用balong_dma_channel_init函数的返回值
  输出参数:     无
  返回值:       0 : 通道忙碌
                1 : 通道空闲
                负数 : 失败
*******************************************************************************/
extern BSP_S32 balong_dma_channel_is_idle (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_IS_IDLE(chanel_id) balong_dma_channel_is_idle(chanel_id)

/*****************************************************************************
 函 数 名  : DRV_EDMA_BBP_SAMPLE_REBOOT
 功能描述  : BBP采数使用重启接口，重启后系统会停留在fastboot阶段，以便导出采集数据，
             SFT平台A核使用，其他直接返回-1
 输入参数  : NA
 输出参数  : 无
 返 回 值  : OK-执行重启，ERROR-不执行重启
 调用函数  : NA
 被调函数  : NA



*****************************************************************************/
static INLINE BSP_S32 DRV_EDMA_BBP_SAMPLE_REBOOT(BSP_VOID)
{
    return -1;
}

#endif



#ifndef __DLOAD_HDLC_H__
#define __DLOAD_HDLC_H__
    
/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/
#define HDLC_FRAME_FLAG  0x7e        /* HDLC帧标志位，固定为0x7e，区分不同的HDLC帧 */
#define HDLC_ESC         0x7d        /* 转义字符，紧接其后的字符需要转义 */
#define HDLC_ESC_MASK    0x20        /* 转义字符掩字 */
        
#define HDLC_INIT_FCS    0xffff      /* 计算FCS的初始FCS值 */
#define HDLC_GOOD_FCS    0xf0b8      /* 计算结果为该FCS值时，说明FCS校验正确 */

#define HDLC_MODE_HUNT   0x1         /* 解封装模式中bit0为1，表明未找到帧标志，没有开始解封装 */
#define HDLC_MODE_ESC    0x2         /* 解封装模式中bit1为1，表明上个字符为转义字符，当前字符为真正数据的掩字 */
        
#define HDLC_FCS_LEN     2           /* HDLC帧中FCS未转义时的长度(单位: Byte) */

#define HDLC_BUF_SIZE    1024

typedef enum _eHdlcState
{
    HDLC_OK = 0,                /* HDLC处理成功 */
    HDLC_PARA_ERR,              /* 参数检查异常 */
    HDLC_ERR_BUF_FULL,
    HDLC_ERR_INVALAID_FRAME,    /* 没有完整的帧 */
    HDLC_ERR_FCS,               /* 发生了校验错误 */
    HDLC_ERR_DISCARD            /* 发生异常，丢弃当前HDLC帧 */
}eHdlcState;

typedef struct tagDLOAD_HDLC_S
{
    UINT32  u32Mode;                       /* HDLC解析使用的内部参数，HDLC负责维护 */
    UINT32  u32Length;                     /* HDLC解析时内部维护的中间变量 */
    UINT32  u32DecapBufSize;               /* HDLC解析内容存放BUFFER的长度，调用者负责指定 */
    UINT32  u32InfoLen;                    /* HDLC解析成功的数据长度，HDLC负责维护 */
    UINT8   au8DecapBuf[HDLC_BUF_SIZE];                   /* HDLC解析内容存放地址，由调用者负责指定空间 */
}DLOAD_HDLC_S;

void HDLC_Init(DLOAD_HDLC_S *pstHdlc);
eHdlcState HDLC_Encap(const UINT8 *pu8Src,UINT16 u16SrcLen,UINT8 *pu8Dst,UINT16 u16DestBufLen,UINT16 *pu16DstLen);
eHdlcState HDLC_Decap(DLOAD_HDLC_S *pstHdlc, UINT8 ucChar);
UINT16 HDLC_Fcs( UINT8 *pucData, UINT32 ulDataLen);


#endif /* hdlc.h */


/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  HDLC.h
*
*   作    者 :  w170956
*
*   描    述 :  HDLC头文件
*
*   修改记录 :  2013年1月13日  v1.00  w170956  创建
*
*************************************************************************/
#ifndef __HDLC_H__
#define __HDLC_H__

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

#define HDLC_PACKET_MAXLEN  ((8*1024) + 512)

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
    unsigned int  u32Mode;                       /* HDLC解析使用的内部参数，HDLC负责维护 */
    unsigned int  u32Length;                     /* HDLC解析时内部维护的中间变量 */
    unsigned int  u32DecapBufSize;               /* HDLC解析内容存放BUFFER的长度，调用者负责指定 */
    unsigned int  u32InfoLen;                    /* HDLC解析成功的数据长度，HDLC负责维护 */
    unsigned char au8DecapBuf[HDLC_PACKET_MAXLEN];                   /* HDLC解析内容存放地址，由调用者负责指定空间 */
}DLOAD_HDLC_S;

void HDLC_Init(DLOAD_HDLC_S **pstHdlc);
eHdlcState HDLC_Encap( const char *pu8Src, unsigned short u16SrcLen, char *pu8Dst, int u16DestBufLen, int *pu16DstLen);
eHdlcState HDLC_Decap(DLOAD_HDLC_S *pstHdlc, unsigned char ucChar);
unsigned short HDLC_Fcs( unsigned char *pucData, unsigned int ulDataLen);


#endif /* hdlc.h */


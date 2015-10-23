
#include <ctype.h>
#include "hdlc.h"
#include "dload.h"

/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 外部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 内部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 外部变量引用                                                 *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 全局变量申明                                                 *
 *--------------------------------------------------------------*/
/* 该全局数组为FCS查找表，用于计算16位FCS。
   rfc1662: the lookup table used to calculate the FCS-16. 
 */
const unsigned short g_au16HdlcFcsTab[256] = 
{
   /* 00 */ 0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
   /* 08 */ 0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
   /* 10 */ 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
   /* 18 */ 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
   /* 20 */ 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
   /* 28 */ 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
   /* 30 */ 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
   /* 38 */ 0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
   /* 40 */ 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
   /* 48 */ 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
   /* 50 */ 0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
   /* 58 */ 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
   /* 60 */ 0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
   /* 68 */ 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
   /* 70 */ 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
   /* 78 */ 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
   /* 80 */ 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
   /* 88 */ 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
   /* 90 */ 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
   /* 98 */ 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
   /* a0 */ 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
   /* a8 */ 0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
   /* b0 */ 0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
   /* b8 */ 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
   /* c0 */ 0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
   /* c8 */ 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
   /* d0 */ 0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
   /* d8 */ 0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
   /* e0 */ 0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
   /* e8 */ 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
   /* f0 */ 0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
   /* f8 */ 0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/*****************************************************************************
* 函 数 名  : HDLC_Encap
*
* 功能描述  : 将输入的原始数据封装成HDLC帧
*
* 输入参数  : const unsigned char *pu8Src   
*             unsigned short u16SrcLen       
*             unsigned char *pu8Dst        
*             unsigned short u16DestBufLen  
*             unsigned short *pu16DstLen    
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : NA
*
*****************************************************************************/
eHdlcState HDLC_Encap
(
    const char *pu8Src,
    unsigned short u16SrcLen,
    char *pu8Dst,
    int u16DestBufLen,
    int *pu16DstLen
)
{
    unsigned short u16Fcs = HDLC_INIT_FCS;
    char *pu8DstPos = pu8Dst;
    char u8FcsChar = 0;

    if ((NULL == pu8Src) 
        ||(NULL == pu8Dst) 
        ||(NULL == pu16DstLen))
    {
        Dbug("[HDLC_Encap]:NULL PTR! line %d\n",__LINE__);
        return HDLC_PARA_ERR;
    }

    if (0 == u16SrcLen)
    {
        *pu16DstLen = 0;
        Dbug("[HDLC_Encap]:Src Data Len is 0 !\n");
        return HDLC_PARA_ERR;
    }
    
    /* 信息域长度不为0的HDLC帧长度至少为5 */
    if (u16DestBufLen <= 4)
    {
        *pu16DstLen = 0;
        Dbug("[HDLC_Encap]:Dst Buf is not Enough(BufLen:%d)!\n" ,u16DestBufLen);
        return HDLC_PARA_ERR;
    }

    /* 填帧头 */
    *pu8DstPos++ = HDLC_FRAME_FLAG;

    /* 遍历输入数据，计算FCS并转义 */
    while (u16SrcLen-- && ((pu8DstPos - pu8Dst) <= (u16DestBufLen - 3)))
    {
        u16Fcs = (u16Fcs >> 8) ^ g_au16HdlcFcsTab[(u16Fcs ^ *pu8Src) & 0xff];

        if ((HDLC_FRAME_FLAG == *pu8Src) || (HDLC_ESC == *pu8Src))
        {
            *pu8DstPos++ = HDLC_ESC;
            *pu8DstPos++ = (*pu8Src++) ^ HDLC_ESC_MASK;
        }
        else
        {
            *pu8DstPos++ = *pu8Src++;
        }
    }

    /* 判断目的BUFFER是否够添加FCS和帧尾 */
    if ((pu8DstPos - pu8Dst) > (u16DestBufLen - 3))
    {
        *pu16DstLen = 0;
        Dbug("[HDLC_Encap]:Dst Buf is not Enough(BufLen:%d)!\n" ,u16DestBufLen);
        return HDLC_ERR_BUF_FULL;
    }

    u16Fcs = ~u16Fcs;

    /* 转义并添加FCS第一个字节 */
    u8FcsChar = u16Fcs & 0xFF;
    if ((HDLC_FRAME_FLAG == u8FcsChar) || (HDLC_ESC == u8FcsChar))
    {
        *pu8DstPos++ = HDLC_ESC;
        *pu8DstPos++ = u8FcsChar ^ HDLC_ESC_MASK;
    }
    else
    {
        *pu8DstPos++ = u8FcsChar;
    }

    /* 判断目的BUFFER是否够添加FCS和帧尾 */
    if ((pu8DstPos - pu8Dst) > (u16DestBufLen - 2))
    {
        *pu16DstLen = 0;
        Dbug("[HDLC_Encap]:Dst Buf is not Enough(BufLen:%d)!\n" ,u16DestBufLen);
        return HDLC_ERR_BUF_FULL;
    }

    /* 转义并添加FCS第二个字节 */
    u8FcsChar = (u16Fcs >> 8) & 0xFF;
    if ((HDLC_FRAME_FLAG == u8FcsChar) || (HDLC_ESC == u8FcsChar))
    {
        *pu8DstPos++ = HDLC_ESC;
        *pu8DstPos++ = u8FcsChar ^ HDLC_ESC_MASK;
    }
    else
    {
        *pu8DstPos++ = u8FcsChar;
    }

    /* 判断目的BUFFER是否够添加帧尾 */
    if ((pu8DstPos - pu8Dst) > (u16DestBufLen - 1))
    {
        *pu16DstLen = 0;
        Dbug("[HDLC_Encap]:Dst Buf is not Enough(BufLen:%d)!\n" ,u16DestBufLen);
        return HDLC_ERR_BUF_FULL;
    }

    *pu8DstPos++   = HDLC_FRAME_FLAG;
    *pu16DstLen     = (unsigned short)(pu8DstPos - pu8Dst);

    return HDLC_OK;
}

/*****************************************************************************
* 函 数 名  : HDLC_Init
*
* 功能描述  : HDLC结构体初始化
*
* 输入参数  : DLOAD_HDLC_S *pstHdlc  
* 输出参数  : DLOAD_HDLC_S *pstHdlc 
*
* 返 回 值  : NA
*
* 其它说明  : 每个应用在第一次使用解封装功能HDLC_Decap前需要调用一次该函数
*
*****************************************************************************/
void HDLC_Init(DLOAD_HDLC_S **pstHdlc)
{
    if(NULL == (*pstHdlc))
    {
        Dbug("HDLC_Init, DLOAD_HDLC_S Pointer is NULL !\n");
        return ;
    }

    (*pstHdlc)->u32DecapBufSize = HDLC_BUF_SIZE;    
    (*pstHdlc)->u32InfoLen  = 0;
    (*pstHdlc)->u32Length   = 0;
    (*pstHdlc)->u32Mode     = HDLC_MODE_HUNT;
}

/*****************************************************************************
* 函 数 名  : HDLC_Fcs
*
* 功能描述  : 计算FCS. 见RFC 1662 Appendix C and CCITT X.25 section 2.27.
*
* 输入参数  : unsigned char *pucData    
*             unsigned int ulDataLen  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
unsigned short HDLC_Fcs( unsigned char *pucData, unsigned int ulDataLen)
{
    unsigned short u16Fcs = HDLC_INIT_FCS;

    while (ulDataLen--)
    {
        u16Fcs = (u16Fcs >> 8) ^ g_au16HdlcFcsTab[(u16Fcs ^ *pucData++) & 0xff];
    }

    return u16Fcs;
}

/*****************************************************************************
* 函 数 名  : HDLC_Decap
*
* 功能描述  : 从输入的HDLC帧字符流中解析出数据内容
*
* 输入参数  : DLOAD_HDLC_S *pstHdlc  
*             unsigned char ucChar          
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
eHdlcState HDLC_Decap(DLOAD_HDLC_S *pstHdlc, unsigned char ucChar)
{
    unsigned short u16Fcs;

    if(!pstHdlc)
    {
        Dbug("Error:HDLC_Decap, Pointer Para is NULL !\n");
        return HDLC_PARA_ERR;
    }

    /* 未找到帧头时，丢弃非HDLC_FRAME_FLAG字符 */
    if ((pstHdlc->u32Mode & HDLC_MODE_HUNT) && (HDLC_FRAME_FLAG != ucChar))
    {
        pstHdlc->u32Length   = 0;        /* 如果用户首次调用HDLC_Decap前进行了初始化HDLC_Init，则此处pstHdlc->u32Length可以不清0 */
	return HDLC_ERR_INVALAID_FRAME;
    }
    switch (ucChar)
    {
        case HDLC_FRAME_FLAG:
            pstHdlc->u32Mode &= ~HDLC_MODE_HUNT;          /* 清标记 */

            if (pstHdlc->u32Length > HDLC_FCS_LEN)
            {                           /* 该分支表示解析到(信息域+FCS)长度大于2 BYTE的帧 */
                u16Fcs = HDLC_Fcs(pstHdlc->au8DecapBuf, pstHdlc->u32Length);
                if (HDLC_GOOD_FCS != u16Fcs)
                {
                    pstHdlc->u32Length       = 0;
		    Dbug("HDLC_FRAME_FLAG/HDLC_ERR_FCS\n");
                    return HDLC_ERR_FCS;
                }

                pstHdlc->u32InfoLen      = pstHdlc->u32Length - HDLC_FCS_LEN;
                pstHdlc->u32Length       = 0;
                return HDLC_OK;
            }
            else
            {                           /* 该分支若pstHdlc->u32Length为0，认为是正常情况：连续的HDLC_FRAME_FLAG */
                                        /* 该分支若pstHdlc->u32Length不为0，认为有HDLC帧，但(信息域+FCS)长度小于3，故丢弃 */
                pstHdlc->u32Length   = 0;
                break;
            }
            
        case HDLC_ESC:
            if (!(pstHdlc->u32Mode & HDLC_MODE_ESC))
            {
                pstHdlc->u32Mode |= HDLC_MODE_ESC;        /* 置标记 */
            }
            else
            {                           /* 异常情况: 连续两个HDLC_ESC */
                pstHdlc->u32Mode &= ~HDLC_MODE_ESC;       /* 清标记 */
                pstHdlc->u32Mode |= HDLC_MODE_HUNT;       /* 置标记 */
                pstHdlc->u32Length = 0;
                return HDLC_ERR_DISCARD;
            }
            break;
            
        default:
            /* 判断目的BUFFER是否已满 */
            if (pstHdlc->u32Length >= pstHdlc->u32DecapBufSize)
            {                           /* 异常情况: 解封装BUFFER不足 */
                pstHdlc->u32Mode |= HDLC_MODE_HUNT;        /* 置标记 */
                pstHdlc->u32Length = 0;
                Dbug("WARNING, HDLC_Decap, Dst Buf is full #1:BufLen:%d !\n\r",(unsigned int)pstHdlc->u32DecapBufSize);
                return HDLC_ERR_BUF_FULL;
            }

            if (pstHdlc->u32Mode & HDLC_MODE_ESC)
            {
                pstHdlc->u32Mode &= ~HDLC_MODE_ESC;          /* 清标记 */
                if (((HDLC_FRAME_FLAG^HDLC_ESC_MASK) == ucChar)
                    || ((HDLC_ESC^HDLC_ESC_MASK) == ucChar))
                {
                    ucChar ^= HDLC_ESC_MASK;
                }
                else
                {                           /* 异常情况: HDLC_ESC后面的字符不正确 */
                    pstHdlc->u32Mode |= HDLC_MODE_HUNT;        /* 置标记 */
                    pstHdlc->u32Length = 0;
                    Dbug("HDLC_Decap, Esc Char Error:0x%x !\n\r",ucChar);
                    return HDLC_ERR_DISCARD;
                }
            }

            pstHdlc->au8DecapBuf[pstHdlc->u32Length] = ucChar;
            pstHdlc->u32Length++;
            break;
    }
    return HDLC_ERR_INVALAID_FRAME;
}



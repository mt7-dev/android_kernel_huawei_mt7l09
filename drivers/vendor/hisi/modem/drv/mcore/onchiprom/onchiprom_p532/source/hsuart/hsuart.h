/*************************************************************************
*   °æÈ¨ËùÓÐ(C) 1987-2020, ÉîÛÚ»ªÎª¼¼ÊõÓÐÏÞ¹«Ë¾.
*
*   ÎÄ ¼þ Ãû :  hsuart.h
*
*   ×÷    Õß :  wuzechun
*
*   Ãè    Êö :  hsuart.c µÄÍ·ÎÄ¼þ
*
*   ÐÞ¸Ä¼ÇÂ¼ :  2011Äê6ÔÂ13ÈÕ  v1.00  wuzechun  ´´½¨
*
*************************************************************************/

#ifndef __HSUART_H__
#define __HSUART_H__

#include "types.h"
#include "OnChipRom.h"

/*--------------------------------------------------------------*
 * ºê¶¨Òå                                                       *
 *--------------------------------------------------------------*/

/* ¸ßËÙ´®¿ÚÄ¬ÈÏ²¨ÌØÂÊ115200bps */
#define HSUART_BAUT_RATE_DEFAULT BAUDRATE_115200

#define HS_UART_RECV_TIMEOUT_MS 10000   /* 10s */

#define HSUART_BASE_ADDR  0x900A8000

/* FCRÏà¹Ø */
/* ÉèÖÃ·¢ËÍFIFOË®Ïß */
#define UART_DEF_TET_NULL     0x00 /* FIFO ÍêÈ«¿Õ*/
#define UART_DEF_TET_2CHAR    0x10 /* FIFO ÖÐÓÐ2 ¸ö×Ö·û*/
#define UART_DEF_TET_ONEFOUR  0x20 /* FIFO ËÄ·ÖÖ®Ò»Âú*/
#define UART_DEF_TET_ONETWO   0x30 /* FIFO ¶þ·ÖÖ®Ò»Âú*/

/* ÉèÖÃ½ÓÊÕFIFOË®Ïß */
#define UART_DEF_RT_1CHAR     0x00 /* FIFO ÖÐÓÐ1 ¸ö×Ö·û*/
#define UART_DEF_RT_2CHAR     0xc0 /* FIFO ²î2 ¸ö×Ö·û¾ÍÂú*/
#define UART_DEF_RT_ONEFOUR   0x40 /* FIFO ËÄ·ÖÖ®Ò»Âú*/
#define UART_DEF_RT_ONETWO    0x80 /* FIFO ¶þ·ÖÖ®Ò»Âú*/

/* DMA Mode */
#define UART_DEF_NO_DMA_MODE  0x00   /* Mode 0 */
#define UART_DEF_DMA_MODE     0x08   /* Mode 1 */

/* ¸´Î»·¢ËÍ½ÓÊÕFIFO */
#define UART_RESET_TX_FIFO    0x04
#define UART_RESET_RCV_FIFO   0x02

/* ÉèÖÃFIFOÊ¹ÄÜ */
#define UART_DEF_FIFOE_ENABLE   0x1 /* FIFO Ê¹ÄÜ*/
#define UART_DEF_FIFOE_DISABLE  0x0 /* FIFO Ê¹ÄÜ*/


/* ÉèÖÃLCR */
/* Í£Ö¹Î»ÉèÖÃ */
#define UART_DEF_LCR_STP2   0x04  /*ÔÚLCR bit[1:0]=0 µÄÌõ¼þÏÂÎª1.5 ¸öÍ£Ö¹Î»£¬·ñÔòÎª2 ¸öÍ£Ö¹Î»*/
#define UART_DEF_LCR_STP1   0x00  /*1¸öÍ£Ö¹Î»*/

#define UART_DEF_LCR_PEN    0x08 /* ÆæÅ¼Ð£ÑéÊ¹ÄÜ*/
#define UART_DEF_LCR_EPS    0x10 /* ÆæÅ¼Ð£ÑéÑ¡Ôñ*/

#define UART_DEF_PARITY_NONE 0x00 /* ÎÞÆæÅ¼Ð£Ñé */

#define UART_DEF_LCR_DIVENABLE  0x80  /* ·ÖÆµ¼Ä´æÆ÷Ê¹ÄÜ */

/* ÉèÖÃÊý¾ÝÁ÷ÖÐµÄÃ¿¸ö×Ö·ûµÄ¸öÊý*/
#define UART_DEF_LCR_CS8    0x03 /* 8bit*/
#define UART_DEF_LCR_CS7    0x02 /* 7bit*/
#define UART_DEF_LCR_CS6    0x01 /* 6bit*/
#define UART_DEF_LCR_CS5    0x00 /* 5bit*/

#define HSUART_MIN_BAUT_RATE  61
#define HSUART_MAX_BAUT_RATE  4000000

/* ê±?ó?µ?ê */
#if PLATFORM==PLATFORM_PORTING
#define HSUART_CLOCK_FREQ   48000000
#else
#define HSUART_CLOCK_FREQ   64000000
#endif


#define HSUUART_DELAY_TIMES 1000


#define HSUART_BUFFER_SIZE   1024

#define HSUART_MAKE_WORD(_h , _l ) ( ((UINT16)(_l) & 0xFF) | (((UINT16)(_h) << 8) & 0xFF00) )

#define HSUART_MAKE_LONG( _c4, _c3, _c2, _c1 )  \
    ( ((UINT32)(_c1) & 0xFF) | (((UINT32)(_c2) << 8) & 0xFF00) | (((UINT32)(_c3) << 16) & 0xFF0000) | (((UINT32)(_c4) << 24) & 0xFF000000) )


/*--------------------------------------------------------------*
 * Êý¾Ý½á¹¹                                                     *
 *--------------------------------------------------------------*/
typedef enum tagHSUART_STATUS
{
    HSUART_STATUS_OK = 0,
    HSUART_STATUS_PARA_ERR,
    HSUART_STATUS_BAUTRATE_ERR,
    HSUART_STATUS_SEND_ERR,
    HSUART_STATUS_BUFFER_FULL_ERR,
    HSUART_RECV_TIMEOUT
}HSUART_STATUS;

typedef enum tagBaudRateE
{
    BAUDRATE_9600   = 9600,
    BAUDRATE_115200 = 115200,
    BAUDRATE_1M     = 1000000,
    BAUDRATE_2M     = 2000000,
    BAUDRATE_3M     = 3000000,
    BAUDRATE_4M     = 4000000
}baudRateE;

typedef struct hsuart_recv_str
{
    UINT32 ulRead;
    UINT32 ulWrite;
    UINT32 ulTotalNum;
    UINT8 ucData[HSUART_BUFFER_SIZE];
}RECV_STR;
/*--------------------------------------------------------------*
 * º¯ÊýÔ­ÐÍÉùÃ÷                                                 *
 *--------------------------------------------------------------*/

HSUART_STATUS hsUartInit(RECV_STR *pstQue);
HSUART_STATUS hsUartRecv(RECV_STR *pstHsUartRecvData);
HSUART_STATUS hsUartSend(UINT8* pucSrcAddr, UINT32 u32TransLength);
int OutQue(RECV_STR *pstQue, UINT8 *pBuf,UINT32 ulLong);

UINT16 crcChk(UINT8 *buf, UINT32 len, UINT16 crc);
INT32 hsUartRecvData(RECV_STR *pstQue, UINT8 *pBuf, UINT32 ulLong);
INT32 hsUartRecvCrc(RECV_STR *pstQue, UINT8* ucRecvBuf, UINT32 ulLength);
BOOL isHsSptBaud(UINT32 u32BaudRate);
HSUART_STATUS hsUartSetBaud(UINT32 u32BaudRate);

#endif /* hsuart.h */


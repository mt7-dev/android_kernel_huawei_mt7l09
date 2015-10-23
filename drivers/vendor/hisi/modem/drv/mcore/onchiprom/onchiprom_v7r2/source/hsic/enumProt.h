/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  enumProt.h
*
*   作    者 :  wuzechun
*
*   描    述 :  自举协议帧格式定义
*
*   修改记录 :  2011年6月29日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __ENUMPROT_H__
#define __ENUMPROT_H__

/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/

/* 自举协议帧结构定义 */
/* FILE FRAME: TYPE(1)+SEQ(1)+CSEQ(1)+FILE(1)+LENGTH(4)+ADDRESS(4)+CRC(2) */
/* DATA FRAME: TYPE(1)+SEQ(1)+CSEQ(1)+DATA(0~1024)+CRC(2) */
/* EOT  FRAME: TYPE(1)+SEQ(1)+CSEQ(1)+CRC(2) */
#define	ENUM_FRAME_HEAD	            0
#define	ENUM_FRAME_SEQ	            1
#define	ENUM_FRAME_CSEQ	            2
#define	ENUM_FRAME_FILE_TYPE		3
#define	ENUM_FRAME_DATA_LOC	        3
#define	ENUM_FRAME_FILE_LEN	        4
#define	ENUM_FRAME_FILE_ADDR		8
#define	ENUM_FRAME_FILE_CRC	        12
/* 自举帧类型定义 */
#define	ENUM_FRAME_TYPE_FILE		0xFE
#define	ENUM_FRAME_TYPE_DATA		0xDA
#define	ENUM_FRAME_TYPE_EOT	        0xED
/* 自举帧响应定义 */
#define	ENUM_RESP_TYPE_ACK	        0xAA
#define	ENUM_RESP_TYPE_NAK	        0x55
#define	ENUM_RESP_TYPE_SNAK	        0xEE
/* 自举帧类型长度定义 */
#define ENUM_FRAME_LEN_FILE            14
#define ENUM_FRAME_LEN_EOT             5

/* 自举文件下载类型定义 */
#define	ENUM_XFR_FTYPE_RAMINIT	            0x01
#define	ENUM_XFR_FTYPE_USB	            0x02

/* 自举协议处理返回值定义 */
#define	ENUM_XFR_PROT_OK	            0
#define	ENUM_XFR_PROT_ERR	            1
#define	ENUM_XFR_PROT_SKIP	            2
#define	ENUM_XFR_PROT_COMPLETE	        3
#define ENUM_XFR_PROT_INPROGRESS	    4


#endif /* __ENUMPROT_H__.h */


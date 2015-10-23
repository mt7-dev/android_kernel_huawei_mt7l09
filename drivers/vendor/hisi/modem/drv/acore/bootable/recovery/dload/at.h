/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  at.h
*
*   作    者 :  wuzechun
*
*   描    述 :  at.c 的头文件
*
*   修改记录 :  2012年12月13日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __AT_H__
#define __AT_H__
/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/
    
#define ATCOMMAND_ERROR             "\r\nERROR\r\n"
#define ATCOMMAND_OK                "\r\nOK\r\n"

#define ATCOMMAND_PROCESS_SUCEED    0
#define ATCOMMAND_PROCESS_FAIL      1
#define ATCOMMAND_NOT_FOUND         2

#define IMEI_LEN                    16
#define BCD_IMEI_LEN                9

#define DLOAD_VERSION               "2.0"                   /* 下载协议版本 */

/*--------------------------------------------------------------*
 * 数据结构                                                     *
 *--------------------------------------------------------------*/
    
/*AT command*/
typedef enum
{
    DLOAD_AT_E0,    /*ate0*/
    DLOAD_AT_E1,    /*ate1*/
    DLOAD_AT_DLOADVER,    /*at^dloadver*/
    DLOAD_AT_DLOADINFO,     /*at^dloadinfo*/
    DLOAD_AT_AUTHORITYVER,        /*at^authorityver*/
    DLOAD_AT_AUTHORITYID,         /*at^authorityid*/
    DLOAD_AT_DATAMODE,  /*at^datamode*/
    DLOAD_AT_HWNATQRY,  /*at^hwnatqry*/
    DLOAD_AT_FLASHINFO,
    DLOAD_AT_CMEE,
    DLOAD_AT_GODLOAD,
    DLOAD_AT_NVBACKUP,//ZFH for test
    DLOAD_AT_BUF     /*无效状态*/
}DLOAD_AT_COMMAND;

/*--------------------------------------------------------------*
 * 函数原型声明                                                 *
 *--------------------------------------------------------------*/

extern int at_cmd_proc( unsigned char *pcAtcmd, int len);

#endif /* at.h */


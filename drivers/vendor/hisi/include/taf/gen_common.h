


#ifndef __GEN_COMMON_H__
#define __GEN_COMMON_H__


#include "gen_msg.h"
#include <string.h>



#define MSP_CNF_PACKET_SIGN              (0x80000000)
#define MSP_SYS_PACKET_SIGN              (0x40000000)
#define MSP_GE_PACKET_SIGN               (0)


#define MSP_ALIGIN_U32_SIZE(x)  ((x&(~3))+4) /* 构造4个字节对齐*/
#define MSP_IS_ALIGIN_U32(x)    ((x)&3)      /* 判断是否为4字节对齐*/
#define MSP_FIELDOFFSET(s,m)             (VOS_UINT32)&(((s *)0)->m) /* 结构成员偏移*/
#define MSP_CHAR_CR   '\r' /* 0x0D*/
#define MSP_CHAR_LF   '\n' /* 0x0A*/
#define AT_TERMINAL_CHAR  MSP_CHAR_CR


/* 给HSO标识是CNF*/
#define MSP_CNF_PACKET_SIGN              (0x80000000)
#define MSP_SYS_PACKET_SIGN              (0x40000000)
#define MSP_GE_PACKET_SIGN               (0)

/* MSP NV 项定义*/
#define MSP_MAX_HWVER_SIZE     (31)
#define MSP_MAX_IMEI_SIZE      (9)

#define MAKEU16(a, b)       ((VOS_UINT16)(((VOS_UINT8)(a)) | ((VOS_UINT16)((VOS_UINT8)(b))) << 8))
#define MAKEU32(a, b)       ((VOS_UINT32)(((VOS_UINT16)(a)) | ((VOS_UINT32)((VOS_UINT16)(b))) << 16))


#define MSP_MAKE_IDENTIFIER(a,b,c,d)     MAKEU32(MAKEU16(a,b),MAKEU16(c,d))
#define HNV_FILE_SIGNATURE               MSP_MAKE_IDENTIFIER('H','N','V','$')


#define VOLATILE_GETVAR(x,val) (x = val)
#define VOLATILE_VAR(x, val)   (x = val)
#define VOLATILE_VARADD(x)     (x++)


typedef struct
{
    /* 数组第一个元素为长度*/
    /* 数组第二个元素的低4位为奇偶校验位，*/
    /* 其余从前一个元素的高4位到后一个元素的低4位采用BCD码方式依次排列IMEI*/
    VOS_UINT8 aucImei[MSP_MAX_IMEI_SIZE];
    VOS_UINT8 aucRsd[3];
}MSP_NV_IMEI_STRU;



#endif /* __MSP_COMMON_H__*/


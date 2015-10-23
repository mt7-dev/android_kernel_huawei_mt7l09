
#include <vxWorks.h>
#include <logLib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "product_config.h"
#include "drv_comm.h"
#include "osl_bio.h"
#include "osl_types.h"
#include "hi_efuse.h"
#include "drv_efuse.h"
#include "efuse_balong.h"
#include "bsp_icc.h"
#include "bsp_om.h"


#ifdef CONFIG_EFUSE
/* start---- adapt for v7r2 */
int efuseWriteHUK(char *pBuf,unsigned int len)
{
    BSP_U32 tmpLen = 0,i = 0;

    BSP_U32 readBuf[EFUSE_HUK_SIZE] = {0};
    
    if((NULL == pBuf)||(0 == len))
    {
        efuse_print_error(" para error.\n");
        return BSP_ERROR;
    }
    for(i=0;i<len;i++)
    {
        if(0 != *(pBuf+i))
        {
            break;
        }
    }
    if(i>=len)
    {
        efuse_print_error("pBuf is null, please contact CW equipment colleague to confirm why send the command 'at^huk=000'.\n");
        return BSP_ERROR;
    }

    tmpLen = EFUSE_GROUP_SIZE * EFUSE_HUK_SIZE ;

    if(tmpLen != len)
    {
        efuse_print_error(" len is not %d.\n",tmpLen);
        return BSP_ERROR;
    }

    tmpLen = len/EFUSE_GROUP_SIZE;

    /*read HUK first,ensure just write once*/
    if(BSP_OK != bsp_efuse_read( readBuf,EFUSE_GRP_HUK,EFUSE_HUK_SIZE))
    {
        efuse_print_error( " read huk failed!\n" );
        return BSP_ERROR;
    }

    /*if efuse area of HUK has been written,return error*/
    for(i=0;i<EFUSE_HUK_SIZE;i++)
    {
        if(0 != readBuf[i])
        {
            efuse_print_error( " HUK has been written!\n" );
            return BSP_ERROR;
        }
    }

    /*调用efuse烧写接口,EFUSE_GRP_HUK为efuse中分配的位置*/
    if( BSP_OK != bsp_efuse_write( (BSP_U32 *)pBuf, EFUSE_GRP_HUK, tmpLen ) )
    {
        efuse_print_error( " efuseWrite error!\n" );
        return BSP_ERROR;
    }

    efuse_print_info( " efuseWriteHUK SUCC!\n" );
    return BSP_OK;
}

int efuseReadHUK(u32 *pBuf, u32 len)
{
    if((NULL == pBuf)||(len < EFUSE_HUK_SIZE))
    {
        efuse_print_error(" para error, pbuf is 0x%x, len is %d.\n", pBuf, len);
        return BSP_ERROR;
    }

	if(BSP_OK != bsp_efuse_read(pBuf, EFUSE_GRP_HUK, EFUSE_HUK_SIZE))
	{
        efuse_print_error( "efuse_read is failed!\n" );
        return BSP_ERROR;	
    }

    return BSP_OK;
}

int CheckHukIsValid()
{
    BSP_U32 i = 0;
    BSP_U32 readBuf[EFUSE_HUK_SIZE] = {0};

    /*read HUK*/
    if(BSP_OK != bsp_efuse_read((BSP_U32 *)readBuf,EFUSE_GRP_HUK,EFUSE_HUK_SIZE))
    {
        efuse_print_error( " read huk failed!\n" );
        return BSP_FALSE;
    }

    for(i=0;i<EFUSE_HUK_SIZE;i++)
    {
        if(0 != readBuf[i])
        {
            efuse_print_info( "\r\n CheckHukIsValid success!\n" );
            return BSP_TRUE;
        }
    }

    return BSP_FALSE;
}
int DRV_GET_DIEID(unsigned char* buf,int length)
{
    u32 *buf_die_id = (u32*)buf;
    if(NULL == buf)
    {
        efuse_print_error("die id buf is error.\n");
        return ERROR;
    }

    if(length < EFUSE_DIEID_LEN)
    {
        efuse_print_error("die id lenth is error.\n");
        return ERROR;
    }

    memset(buf, 0, EFUSE_DIEID_LEN);
    
    if(ERROR == bsp_efuse_read((u32*)buf, EFUSE_GRP_DIEID, EFUSE_DIEID_SIZE))
    {
        efuse_print_error("die id read efuse error.\n");
        return READ_EFUSE_ERROR;
    }

    buf_die_id[EFUSE_DIEID_SIZE-1] &= ((0x1 << EFUSE_DIEID_BIT) - 1);

    return OK;
    
}


#ifdef HI_K3_EFUSE
int DRV_GET_CHIPID(unsigned char* buf,int length)
{
    if(NULL == buf)
    {
        efuse_print_error("die id buf is error.\n");
        return ERROR;
    }

    if(length < EFUSE_CHIPID_LEN)
    {
        efuse_print_error("die id lenth is error.\n");
        return ERROR;
    }

    memset(buf, 0, EFUSE_CHIPID_LEN);
    
    if(ERROR == bsp_efuse_read((u32*)buf, EFUSE_GRP_CHIPID, EFUSE_CHIPID_SIZE))
    {
        efuse_print_error("chip id read efuse error.\n");
        return READ_EFUSE_ERROR;
    }
    
    return OK;
}
#else

int DRV_GET_CHIPID(unsigned char* buf,int length)
{
    return OK;
}

#endif

#else
int efuseWriteHUK(char *pBuf,unsigned int len)
{
    return OK;
}
int efuseReadHUK(u32 *pBuf,u32 len)
{
    return OK;
}
int CheckHukIsValid(void)
{
    return OK;
}

int DRV_GET_DIEID(unsigned char* buf,int length)
{
    return OK;
}

int DRV_GET_CHIPID(unsigned char* buf,int length)
{
    return OK;
}


#endif




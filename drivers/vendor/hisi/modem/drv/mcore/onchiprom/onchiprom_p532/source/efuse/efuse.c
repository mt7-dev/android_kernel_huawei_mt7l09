/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  efuse.c
*
*   作    者 :  wuzechun
*
*   描    述 :  Efuse读取
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#include "sys.h"
#include "OnChipRom.h"
#include "efuse.h"

/*********************************************************************************************
Function:           TestBitValue
Description:        测试寄存器中的某位，是否等于指定的值
Calls:              无
Data Accessed:      无
Data Updated:       无
Input:              addr：  寄存器地址
                    bitMask：需要检测的位置
                    bitValue：指定的值
Output:             无
Return:             成功则返回TRUE， 失败则返回FALSE
Others:             无
*********************************************************************************************/
BOOL TestBitValue(UINT32 addr, UINT32 bitMask, UINT32 bitValue)
{
    UINT32 time;                       /*延时时间*/

    for(time=0; time<TIMEMAXDELAY; time++)
    {
        if(bitValue==(INREG32(addr)&bitMask))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*****************************************************************************
* 函 数 名  : efuseRead
*
* 功能描述  : 按组读取EFUSE中的数据
*
* 输入参数  : group  起始group
*                   num  数组长度(word数,不超过512/4=128)
* 输出参数  : pBuf ：EFUSE中的数据
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
int efuseRead(UINT32 group, UINT32* pBuf, UINT32 num)
{
    UINT32* pSh;
    UINT32 cnt;

    if((group+num > EFUSE_MAX_SIZE/WORD_SIZE)||(NULL == pBuf))
    {
        return ERROR;
    }
    pSh = pBuf;

    /* 设置EFUSE信号为APB信号 */
    SETREG32(EFUSEC_CFG, EFUSEC_APB);

    /* 循环读取Efuse值 */
    for(cnt=group; cnt<num; cnt++)
    {
        /* 设置读取地址 */
        OUTREG32(EFUSE_GROUP, group+cnt);

        /* 使能读 */
        SETREG32(EFUSEC_CFG, EFUSEC_RD_EN);
        /* 等待读使能设置成功，读使能超时返回错误 */
        if(!TestBitValue(EFUSEC_CFG, EFUSEC_RD_EN, EFUSEC_RD_EN))
        {
            return ERROR;
        }
        /* 等待读完成 */
        if(!TestBitValue(EFUSEC_STATUS, EFUSEC_RD_STATUS, EFUSEC_RD_STATUS))
        {
            return ERROR;
        }

        /* 读取数据 */
        *pSh = INREG32(EFUSEC_DATA);
        pSh++;

    }

    return OK;
}

#if PLATFORM==PLATFORM_PORTING
int efuseWrite(UINT32 group, UINT32* pBuf, UINT32 num)
{
    int i;

    SETREG32(EFUSEC_CFG, EFUSEC_APB);
    SETBITVALUE32(EFUSEC_COUNT, EFUSE_COUNT_MASK, EFUSE_COUNT_CFG);
    SETBITVALUE32(EFUSEC_PGM_COUNT, EFUSE_PGM_COUNT_MASK, PGM_COUNT_CFG);
    SETREG32(EFUSEC_CFG, EFUSEC_PRE_PG);
    if (!TestBitValue(EFUSEC_STATUS, EFUSEC_PRE_PG_FIN, EFUSEC_PRE_PG_FIN))
    {
        return ERROR;
    }
    for (i = 0; i < num; i++)
    {
        SETBITVALUE32(EFUSE_GROUP, EFUSE_GP_MASK, group + i);
        OUTREG32(EFUSE_PG_VALUE, *(pBuf + i));
        SETREG32(EFUSEC_CFG, EFUSEC_PG_EN);
        if (!TestBitValue(EFUSEC_STATUS, EFUSEC_PG_STATUS, EFUSEC_PG_STATUS))
        {
            return ERROR;
        }
    }
    CLRREG32(EFUSEC_CFG, EFUSEC_PRE_PG);

    return OK;
}
#endif
/*****************************************************************************
* 函 数 名  : efuseWriteCheck
*
* 功能描述  : 检验EFUSE是否已经烧写
*
* 输入参数  : pBuf   读取到的EFUSE中的数据
*                   num  数组长度(word数,不超过512/4=128)
* 输出参数  :
*
* 返 回 值  : EFUSE_NOT_WRITED EFUSE未曾烧写
*                EFUSE_HAD_WRITED EFUSE已经烧写
*
* 其它说明  :
*
*****************************************************************************/
int efuseWriteCheck(UINT32* pBuf, UINT32 num)
{
    int i;
    /*读取EFUSE中的值*/
    for(i = 0; i < (int)num; i++)
    {
        if(0 != pBuf[i])
        {
            return EFUSE_HAD_WRITED;
        }
    }
    return EFUSE_NOT_WRITED;
}



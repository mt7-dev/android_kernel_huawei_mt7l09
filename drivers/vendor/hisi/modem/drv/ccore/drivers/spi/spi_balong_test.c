
#include <bsp_spi.h>

/*c核中该函数只在test时调用*/
s32 spi_init(u32 spiNo)
{
    u32 reg;
    if ((0 != spiNo) && (1 != spiNo))
    {
        return ERROR;
    }

    /*三线模式*/
    reg = readl(HI_SYSCTRL_BASE_ADDR+0x040C);
    reg &= ~0x100;
    writel(reg,HI_SYSCTRL_BASE_ADDR+0x040C);
    mdelay(10);//10ms

    /* 禁止SPI Slave*/
    writel(0,SPI_SLAVE_EN(spiNo));

    /* 禁止SPI Master*/
    writel(0,SPI_EN(spiNo));

    if(spiNo)
    {   /*spi1*/
        writel((0x08<<SPI_CTRL0_DFS_BITPOS)    \
                | (0<<SPI_CTRL0_SCPH_BITPOS)    \
                | (0<<SPI_CTRL0_SCPOL_BITPOS)   \
                | (0 << SPI_CTRL0_FRF_BITPOS)   \
                | (0x00<<SPI_CFS_BITPOS),SPI_CTRL0(spiNo));
         /* 配置SPI波特率为SSI CLK的1/24，即48/6=8MHz */
        writel(6,SPI_BAUD(spiNo));
    }
    else
    {
        /*spi0*/
        /* 配置ctrl0寄存器，命令字长度为1，数据宽度为8,上升沿触发,低电平有效 */
        writel((0x0f<<SPI_CTRL0_DFS_BITPOS)    \
                | (1<<SPI_CTRL0_SCPH_BITPOS)    \
                | (1<<SPI_CTRL0_SCPOL_BITPOS)   \
                | (0 << SPI_CTRL0_FRF_BITPOS)   \
                | (0x00<<SPI_CFS_BITPOS),SPI_CTRL0(spiNo));

    /* 配置SPI波特率为SSI CLK的1/24，即48/24=2MHz */
    writel(24,SPI_BAUD(spiNo));
    }

    /*禁止所有中断信号*/
    writel(0,SPI_IMR(spiNo));

    /*屏蔽DMA传输*/
    writel(0, SPI_DMAC(spiNo));

    return OK;
}

/*****************************************************************************
* 函 数 名  : spiDevDetect
*
* 功能描述  : 动态探测SPI总线上接入的设备类型，验证SPI总线驱动
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
s32 spi_dev_detect( eSpiDevType *devType )
{
	u16 ucCmdRDID = SPI_DEV_CMD_RDID;
	u32 ulRecv = 0;

	if(NULL == devType)
	{
		spi_print_error("\r\nPARA *devType is NULL!");
		return BSP_SPI_ERROR;
	}

	/* EEPROM对该命令不支持，会收不到数据超时退出 */
	if(BSP_SPI_OK != spi_recv(SPI_NO_DEV, SPI_CS_DEV, (u16*)&ulRecv, 3, &ucCmdRDID, 1))
	{
	    spi_print_error("\r\nread ID err");
	    *devType = E_SPI_DEV_TYPE_EEPROM;

	    return BSP_SPI_OK;
	}

	spi_print_info("\r\nID: %x", ulRecv);

	/* EEPROM不支持该命令，会返回全0或全1 */
	if(0 != ulRecv && 0xFFFFFF != ulRecv)
	{
	    *devType = E_SPI_DEV_TYPE_SFLASH;
	}
	else
	{
	    *devType = E_SPI_DEV_TYPE_EEPROM;
	}
	spi_print_info("\r\ndevType is :  @ %x", *devType);

	return BSP_SPI_OK;

}
/*****************************************************************************
* 函 数 名  : spi_test
*
* 功能描述  : 通过动态探测SPI总线上接入的设备类型，验证SPI总线驱动,验证设备为eeprom或sflash
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
s32 spi_test( void )
{
	eSpiDevType edevType;

	(void)spi_init(SPI_NO_DEV);

	(void)spi_dev_detect(&edevType);

	if(E_SPI_DEV_TYPE_EEPROM == edevType)
	{
	    spi_print_info("\r\nE2PROM_boot!" );
	}
	else
	{
	    spi_print_info("\r\nSF_boot!" );
	}
	return 0;
}



/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  spiDev.h
*
*   作    者 :  wuzechun
*
*   描    述 :  spiDev.h 的头文件
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __SPIDEV_H__
#define __SPIDEV_H__

/* EEPROM和串行Flash通用命令 */
#define SPI_DEV_CMD_WRSR     0x1     /*Write Status Register*/
#define SPI_DEV_CMD_WRITE    0x2     /*Write Data to Memory Array*/
#define SPI_DEV_CMD_READ     0x3     /*Read Data from Memory Array*/
#define SPI_DEV_CMD_WRDI     0x4     /*Reset Write Enable Latch*/
#define SPI_DEV_CMD_RDSR     0x5     /*Read Status Register*/
#define SPI_DEV_CMD_WREN     0x6     /*Set Write Enable Latch*/

/* 串行Flash特有命令 */
#define SPI_DEV_CMD_RDID     0x9F    /* Read Identification */
#define SPI_DEV_CMD_PP      0x02     /* 页编程 */
#define SPI_DEV_CMD_SE      0xD8    /* 块擦除 */
#define SPI_DEV_CMD_BE      0xC7    /* Bulk Erase */

/* 状态寄存器状态位 */
#define SPI_DEV_STATUS_WIP  (1<<0)  /* Write In Progress Bit */
#define SPI_DEV_STATUS_WEL  (1<<1)  /* Write Enable Latch Bit */
#define SPI_DEV_STATUS_BP0  (1<<2)  /* Block Protect Bits */
#define SPI_DEV_STATUS_BP1  (1<<3)  /* Block Protect Bits */
#define SPI_DEV_STATUS_BP2  (1<<4)  /* Block Protect Bits */
#define SPI_DEV_STATUS_SRWD  (1<<7)  /* Status Register Write Protect */

typedef enum eSpiDevType_t
{
    E_SPI_DEV_TYPE_EEPROM = 0,
    E_SPI_DEV_TYPE_SFLASH
}eSpiDevType;

spiReadStatus spiDevDetect( eSpiDevType *devType );
spiReadStatus spiDevReadBl(UINT8 *pReadBuf);

#endif

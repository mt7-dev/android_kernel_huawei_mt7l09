

#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "osl_types.h"
#include "product_config.h"

/*º¯ÊýÉùÃ÷*/
void bsp_spi_init (void);
s32 spi_recv (u32 spiNo, u32 cs, u16* prevData, u32 recvSize,u16* psendData,u32 sendSize );
s32 spi_send (u32 spiNo, u32 cs, u16* pData, u32 ulLen);
#endif

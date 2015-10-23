#include "sys.h"
#include "config.h"
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"


/* NFIO复用配置 */
#define NF_IOS_SYS_CONFIG() \
{\
}

#if PLATFORM==PLATFORM_PORTING
/* SPI0 — Porting用SPI0复用配置,片选为CS1 */
#define SPI_IOS_SYS_CONFIG() \
{\
}
#else
/* SPI1 — ASIC用SPI1复用配置，片选为CS1 */
#define SPI_IOS_SYS_CONFIG() \
{\
}
#endif

/* EMMC管管脚配置 */
#if PLATFORM==PLATFORM_PORTING
#define EMMC_IOS_SYS_CONFIG() \
{\
}
#else
/* ASIC使用MMC0 */
#define EMMC_IOS_SYS_CONFIG() \
{\
}
#endif

/* HsUart管脚配置 */
#define HSUART_IOS_SYS_CONFIG() \
{\
}


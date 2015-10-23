#ifndef __MDDRC_H__
#define __MDDRC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "product_config.h"

#define LPDDR1

#define MDDRC_REGBASE_ADR    0x90020000       
#define MDDRC_ODT_CFG        0x1
#define MDDRC_PHY_CFG0       0x0
#define MDDRC_PHY_CFG1       0x0
#define MDDRC_IO_CFG0        0x7
#define MDDRC_IO_CFG1        0x7
#define MDDRC_INIT_CTRL      0x1
#define MDDRC_ZQ_ENABLE      0x1
#ifndef LPDDR1
#define MDDRC_CK_SEL         0x19
#else
/*2012-3-22 wxs modify begin*/
//使用保守版本，关闭命令线自动开关功能
//#define MDDRC_CK_SEL         0xd
#define MDDRC_CK_SEL         0x1d
/*2012-3-22 wxs modify end*/
#endif

#ifdef BSP_EDA
#define VALUE_DELAY_200US    1
#define VALUE_DELAY_10US     1
#else
#define VALUE_DELAY_200US    20000
#define VALUE_DELAY_10US     5000
#endif


#define MDDRC_IO_MODE         0x4000

#define MDDRC_SMALL_FRESH     0x22300002
#define MDDRC_WAIT_FRESH      0x22300040


#define MDDRC_ZQ_DRIVER       0x6b
#define MDDRC_ZQ_CTRL0        0x40000000
#define MDDRC_ZQ_CTRL1        0x2000014a

#define MDDRC_CTRL1_CONFIG    0x1006
#define MDDRC_CTRL2_CONFIG    0x1003
#define MDDRC_FRESH_DISABLE   0x7771e000
#define MDDRC_DDR_BASEMENT    0x1e00210
#define MDDRC_CMD_CFG         0x3f02

#define MDDRC_BURST_LEN       0x00800032    /* DDR驱动能力改为3/4,避免过冲 */
#define MDDRC_LATENCY_LEN     0x0
#define MDDRC_PWR_MODE        0x21e00110


#define MDDRC_AUTO_REFRESH   0x30005300

#define MDDRC_BASE_ADDR      0x30000000

//#define MDDRC_TIMING0_CFG    0x23440c09
//#define MDDRC_TIMING1_CFG    0x0e321318
//#define MDDRC_TIMING2_CFG    0x22300040
//#define MDDRC_TIMING3_CFG    0x00000052
#define MDDRC_TIMING0_CFG    0x23440c08
#define MDDRC_TIMING1_CFG    0x0e321318
#define MDDRC_TIMING2_CFG    0x22300049
#define MDDRC_TIMING3_CFG    0xffc00022


#define MDDRC_DTR_CTRL        0x0f000103
#define MDDRC_DTR_PRD         0x5d769

#define MDDRC_CLK_ENABLE     0x21e70110

#define MDDRC_PRECHARGE      0x30005700
#define MDDRC_CKSEL          0x19


void mddrc_init(void);

#ifdef __cplusplus
}
#endif

#endif

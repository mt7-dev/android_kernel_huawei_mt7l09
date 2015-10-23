/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : bsp_temperature.h */
/* Version        : 1.0 */
/* Created       : 2013-04-07*/
/* Last Modified : */
/* Description   :  The C union definition file for the module TEMPERATURE*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
#ifndef __BSP_TEMPERATURE_H__
#define __BSP_TEMPERATURE_H__

#include "hi_temperature.h"

#define TEMPERATURE_OK                   0
#define TEMPERATURE_ERROR                -1

/*ÎÂ¶ÈÄ§»ÃÊý*/
#define TEMPERATURE_MAGIC_DATA           0x5A5A5A5A

#define  tem_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_TEMPERATURE, "[tem]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define  tem_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_TEMPERATURE, "[tem]: "fmt, ##__VA_ARGS__))

/* TEMPERATURE SHARED MEM VIRT ADDR */
#define TEMPERATURE_VIRT_ADDR  SHM_MEM_TEMPERATURE_ADDR

#define ERROR_CHAN_MAX 2

typedef enum
{
    NORMAL_TEMPERATURE  = 0,
    HIGH_TEMPERATURE    = 1,
    LOW_TEMPERATURE     = 2,
    BUTT_TEMPERATURE
}TEMPERATURE_TYPE;

typedef enum
{
    TSENS_START       = 0,
    TSENS_TSENS       = TSENS_START,
    TSENS_A9          = 1,
    TSENS_DDR         = 2,
    TSENS_END         = TSENS_DDR,
    
    HKADC_START       = 3,
    HKADC_LCD         = HKADC_START,
    HKADC_BATTERY     = 4,
    HKADC_SIM         = 5, /*5*/
    HKADC_END         = HKADC_SIM,
    
    MAX_REGION
}TEMPERATURE_REGION;

struct tem_msg_stru
{
    unsigned int id;
    unsigned int region;    
    unsigned int status;
};


#endif

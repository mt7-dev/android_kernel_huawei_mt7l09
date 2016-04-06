#ifndef __DRV_TEMP_CFG_H__
#define __DRV_TEMP_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hi_temperature.h>
#include <drv_temperature.h>

#define TSENS_REGION_MAX 3

struct drv_hkadc_map
{
    unsigned int logic_chan;
    unsigned int phy_chan;
    unsigned int nv_id;
    unsigned int nv_len;
    unsigned int modem_id;
    char * name;
};
/* 温度输出区结构 */
typedef struct hkadc_chan_out
{
    unsigned short volt_l;    /* 返回的HKADC电压值 */
    unsigned short volt_r;    /* 返回的HKADC电压值取反 */
    short          temp_l;    /* 返回的温度值 */
    short          temp_r;    /* 返回的温度值取反 */
} DRV_CHANNEL_OUTPUT;

/* 温度电压对应表结构 */
typedef struct hkadc_tem_value
{
    short          temp;    /* 温度点 */
    unsigned short volt;    /* 电压点 */
} TEM_VOLT_TABLE;

/* 输出配置bit结构 */
typedef struct hkadc_out_config
{
   unsigned int out_prop:2; 
   unsigned int out_wake:1;
   unsigned int out_vol_temp:1;
   unsigned int out_reserv1:4;
   unsigned int out_acore:1;
   unsigned int out_ccore:1;
   unsigned int out_reserv2:22;
} OUT_PROP_CONFIG;

/* 温度配置区结构 */
typedef struct hkadc_chan_config
{
    union 
    {
        unsigned int out_config;    /* bit0-bit1: 0:不输出 1:单次输出 2:循环输出 */
                                    /* bit2: 1:唤醒输出 0:非唤醒输出 */
                                    /* bit3: 0:只输出电压 1:电压温度均输出 */
                                    /* bit8: A核输出 */
                                    /* bit9: C核输出 */
        struct hkadc_out_config bit_out_config;
    } outcfg;

    unsigned int have_config;    
    unsigned short out_peroid;      /* 循环输出时的循环周期，单位:秒 */
    unsigned short temp_data_len;   /* 温度转换表长度 */
    struct hkadc_tem_value temp_table[32]; /* 温度转换表，实际长度参见temp_data_len */
} DRV_CHANNEL_CONFIG;

/* 系统放电温度保护配置结构 */
typedef struct sys_temp_cfg
{
    unsigned short enable;        /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    unsigned short hkadc_id;      /* 系统放电温度保护的hkadc通道ID */
    short          high_thres;    /* 系统放电高温保护的电压阀值 ，温度越高，电压越低*/
    unsigned short high_count;    /* 系统放电高温保护次数上限，到了后系统关机 */
    short          low_thres;     /* 系统放电低温保护的电压阀值 ，温度越低，电压越高*/
    unsigned short low_count;     /* 系统放电低温保护次数上限，到了后系统关机 */

    unsigned int   reserved[2];   /*保留*/
} DRV_SYS_TEMP_PRO;

/* TSENEOR温度保护配置结构 */
typedef struct tsens_region_cfg
{
    unsigned short enable;         /* bit0:高温保护使能 bit1:低温保护使能 1 使能 0 关闭*/
    unsigned short high_thres;     /* 系统放电高温保护*/
    unsigned short high_count;     /* 系统放电高温保护次数上限，系统关机 */
    unsigned short reserved;
    /*unsigned short low_thres;*/     /* 系统放电低温保护*/
    /*unsigned short low_count;*/     /* 系统放电低温保护次数上限，系统关机 */
} TSENS_REGION_CFG;

typedef struct tsens_region_config
{
    short temp[256];

}TSENS_REGION_CONFIG;

/* 共享温度区数据结构 */
typedef struct thermal_data_area
{
    unsigned int             magic_start;                 /* 分区起始Magic Code 0x5A5A5A5A*/
    struct hkadc_chan_out       chan_out[HKADC_CHAN_MAX];    /* 各通道输出区域*/
    struct hkadc_chan_config    chan_cfg[HKADC_CHAN_MAX];    /* 各通道配置区域 */
    struct sys_temp_cfg         sys_temp_cfg;                /* 系统放电温保配置 */
    unsigned int               hw_ver_id;                   /* 版本ID */
    unsigned short              phy_tbl[HKADC_CHAN_MAX];     /* HKADC物理通道与逻辑通道对应表 */
    unsigned int              debug_flag;                  /* 调测开关 */
    struct tsens_region_cfg     tens_cfg[TSENS_REGION_MAX];  /* TSENSOR温保配置 */
    struct tsens_region_config  region_cfg;                  /* TSENSOR各区域的配置信息*/
    short                       tsens_out[TSENS_REGION_MAX]; /* TSENSOR各区域的输出信息*/
    short                       reserved;                    /* 保留信息*/
    unsigned int               magic_end;                   /* 分区结束Magic Code 0x5A5A5A5A*/
} DRV_HKADC_DATA_AREA;

#ifdef __cplusplus
}
#endif

#endif


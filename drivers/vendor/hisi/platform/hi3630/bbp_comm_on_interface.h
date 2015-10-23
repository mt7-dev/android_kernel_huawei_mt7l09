

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "soc_baseaddr_interface.h"

#ifndef __BBP_COMM_ON_INTERFACE_H__
#define __BBP_COMM_ON_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/***======================================================================***
                     (1/3) register_define_afc_mix
 ***======================================================================***/
/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      0，表示配置AFC选择PDM输出；
             1，表示配置选择PWM输出
   UNION结构:  BBPCOMM_AFC_PWM_SEL_MIX_UNION */
#define BBPCOMM_AFC_PWM_SEL_MIX_ADDR                  (SOC_BBP_COMM_ON_BASE_ADDR + 0x3000)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      1，表示配置AFC使能；
             0，表示配置AFC不使能
   UNION结构:  BBPCOMM_AFC_EN_MIX_UNION */
#define BBPCOMM_AFC_EN_MIX_ADDR                       (SOC_BBP_COMM_ON_BASE_ADDR + 0x3004)

/* 寄存器说明：
 bit[31:12]  保留
 bit[11:0]   AFC配置值
   UNION结构:  BBPCOMM_AFC_CFG_WIDTH_MIX_UNION */
#define BBPCOMM_AFC_CFG_WIDTH_MIX_ADDR                (SOC_BBP_COMM_ON_BASE_ADDR + 0x3008)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      1，表示选择配置AFC值
             0，表示选择BBP产生的AFC值
   UNION结构:  BBPCOMM_AFC_CFG_SEL_MIX_UNION */
#define BBPCOMM_AFC_CFG_SEL_MIX_ADDR                  (SOC_BBP_COMM_ON_BASE_ADDR + 0x300C)



/***======================================================================***
                     (2/3) register_define_afc_mix_ch1
 ***======================================================================***/
/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      0，表示配置AFC选择PDM输出；
             1，表示配置选择PWM输出
   UNION结构:  BBPCOMM_AFC_PWM_SEL_MIX_CH1_UNION */
#define BBPCOMM_AFC_PWM_SEL_MIX_CH1_ADDR              (SOC_BBP_COMM_ON_BASE_ADDR + 0x3200)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      1，表示配置AFC使能；
             0，表示配置AFC不使能
   UNION结构:  BBPCOMM_AFC_EN_MIX_CH1_UNION */
#define BBPCOMM_AFC_EN_MIX_CH1_ADDR                   (SOC_BBP_COMM_ON_BASE_ADDR + 0x3204)

/* 寄存器说明：
 bit[31:12]  保留
 bit[11:0]   AFC配置值
   UNION结构:  BBPCOMM_AFC_CFG_WIDTH_MIX_CH1_UNION */
#define BBPCOMM_AFC_CFG_WIDTH_MIX_CH1_ADDR            (SOC_BBP_COMM_ON_BASE_ADDR + 0x3208)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      1，表示选择配置AFC值
             0，表示选择BBP产生的AFC值
   UNION结构:  BBPCOMM_AFC_CFG_SEL_MIX_CH1_UNION */
#define BBPCOMM_AFC_CFG_SEL_MIX_CH1_ADDR              (SOC_BBP_COMM_ON_BASE_ADDR + 0x320C)



/***======================================================================***
                     (3/3) register_define_hkadc_ssi
 ***======================================================================***/
/* 寄存器说明：
 bit[31:1]   保留
 bit[0:0]    软件配置
   UNION结构:  BBPCOMM_DSP_START_UNION */
#define BBPCOMM_DSP_START_ADDR                        (SOC_BBP_COMM_ON_BASE_ADDR + 0x3800)

/* 寄存器说明：
 bit[31:20]  保留
 bit[19:15]  保留，不需要配置。
 bit[14:12]  master check读同步位时间：
             000： 1.5clk； 
             001： 2.0clk；
              ,,, 
             111： 5.0 clk.
             默认值配置为2.
 bit[11:8]   SSI-IP需要的设置超时等待时间，单位cycle。 
             time+1。
 bit[7:3]    保留，不需要配置。
 bit[2:0]    保留，不需要配置。
   UNION结构:  BBPCOMM_DSP_CFG_UNION */
#define BBPCOMM_DSP_CFG_ADDR                          (SOC_BBP_COMM_ON_BASE_ADDR + 0x3804)

/* 寄存器说明：
   详      述：软件HKADC请求超时时间长度，单位cycle。
   UNION结构 ：无 */
#define BBPCOMM_DSP_WAIT_TIME_ADDR                    (SOC_BBP_COMM_ON_BASE_ADDR + 0x380C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0:0]    超时清除脉冲信号。
   UNION结构:  BBPCOMM_TIME_OUT_ERR_CLR_UNION */
#define BBPCOMM_TIME_OUT_ERR_CLR_ADDR                 (SOC_BBP_COMM_ON_BASE_ADDR + 0x3810)

/* 寄存器说明：
   详      述：回读结果，低10位有效。
            软件可以直接读取该寄存器值，不需要任何处理。
   UNION结构 ：无 */
#define BBPCOMM_DSP_RD_DATA_ADDR                      (SOC_BBP_COMM_ON_BASE_ADDR + 0x3814)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0:0]    软件HKADC请求超时状态指示电平信号。
             如果超时，软件需要配置超时清除寄存器hkadc_time_out_err_clr。
   UNION结构:  BBPCOMM_TIME_OUT_ERR_UNION */
#define BBPCOMM_TIME_OUT_ERR_ADDR                     (SOC_BBP_COMM_ON_BASE_ADDR + 0x3818)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0:0]    完成
   UNION结构:  BBPCOMM_DSP_START_CLR_UNION */
#define BBPCOMM_DSP_START_CLR_ADDR                    (SOC_BBP_COMM_ON_BASE_ADDR + 0x381C)

/* 寄存器说明：软件读写寄存器01配置值
 bit[31:31]  HKADC
 bit[30:24]  HKADC
 bit[23:16]  HKADC
 bit[15:15]  HKADC
 bit[14:8]   HKADC
 bit[7:0]    HKADC
   UNION结构:  BBPCOMM_WR01_DATA_UNION */
#define BBPCOMM_WR01_DATA_ADDR                        (SOC_BBP_COMM_ON_BASE_ADDR + 0x3820)

/* 寄存器说明：软件读写寄存器34配置值
 bit[31:31]  HKADC
 bit[30:24]  HKADC
 bit[23:16]  HKADC
 bit[15:15]  HKADC
 bit[14:8]   HKADC
 bit[7:0]    HKADC
   UNION结构:  BBPCOMM_WR34_DATA_UNION */
#define BBPCOMM_WR34_DATA_ADDR                        (SOC_BBP_COMM_ON_BASE_ADDR + 0x3824)

/* 寄存器说明：软件读写寄存器时延值
 bit[31:16]  HKADC
 bit[15:0]   HKADC
   UNION结构:  BBPCOMM_HKADC_DELAY01_UNION */
#define BBPCOMM_HKADC_DELAY01_ADDR                    (SOC_BBP_COMM_ON_BASE_ADDR + 0x3828)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0:0]    
   UNION结构:  BBPCOMM_HKADC_DBG_IND_UNION */
#define BBPCOMM_HKADC_DBG_IND_ADDR                    (SOC_BBP_COMM_ON_BASE_ADDR + 0x38D0)

/* 寄存器说明：
   详      述：新增
   UNION结构 ：无 */
#define BBPCOMM_DBG_INFO_ADDR                         (SOC_BBP_COMM_ON_BASE_ADDR + 0x38D4)





/*****************************************************************************
  3 枚举定义
*****************************************************************************/



/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/



/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/



/*****************************************************************************
  7 UNION定义
*****************************************************************************/

/***======================================================================***
                     (1/3) register_define_afc_mix
 ***======================================================================***/
/*****************************************************************************
 结构名    : BBPCOMM_AFC_PWM_SEL_MIX_UNION
 结构说明  : AFC_PWM_SEL_MIX 寄存器结构定义。地址偏移量:0x00，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_pwm_sel_mix_reg;
    struct
    {
        unsigned long  afc_pwm_sel : 1;  /* bit[0]   : 0，表示配置AFC选择PDM输出；
                                                       1，表示配置选择PWM输出 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_AFC_PWM_SEL_MIX_UNION;
#define BBPCOMM_AFC_PWM_SEL_MIX_afc_pwm_sel_START  (0)
#define BBPCOMM_AFC_PWM_SEL_MIX_afc_pwm_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_AFC_EN_MIX_UNION
 结构说明  : AFC_EN_MIX 寄存器结构定义。地址偏移量:0x04，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_en_mix_reg;
    struct
    {
        unsigned long  afc_en   : 1;  /* bit[0]   : 1，表示配置AFC使能；
                                                    0，表示配置AFC不使能 */
        unsigned long  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_AFC_EN_MIX_UNION;
#define BBPCOMM_AFC_EN_MIX_afc_en_START    (0)
#define BBPCOMM_AFC_EN_MIX_afc_en_END      (0)


/*****************************************************************************
 结构名    : BBPCOMM_AFC_CFG_WIDTH_MIX_UNION
 结构说明  : AFC_CFG_WIDTH_MIX 寄存器结构定义。地址偏移量:0x08，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_cfg_width_mix_reg;
    struct
    {
        unsigned long  afc_cfg_width : 12; /* bit[0-11] : AFC配置值 */
        unsigned long  reserved      : 20; /* bit[12-31]: 保留 */
    } reg;
} BBPCOMM_AFC_CFG_WIDTH_MIX_UNION;
#define BBPCOMM_AFC_CFG_WIDTH_MIX_afc_cfg_width_START  (0)
#define BBPCOMM_AFC_CFG_WIDTH_MIX_afc_cfg_width_END    (11)


/*****************************************************************************
 结构名    : BBPCOMM_AFC_CFG_SEL_MIX_UNION
 结构说明  : AFC_CFG_SEL_MIX 寄存器结构定义。地址偏移量:0x0C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_cfg_sel_mix_reg;
    struct
    {
        unsigned long  afc_cfg_sel : 1;  /* bit[0]   : 1，表示选择配置AFC值
                                                       0，表示选择BBP产生的AFC值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_AFC_CFG_SEL_MIX_UNION;
#define BBPCOMM_AFC_CFG_SEL_MIX_afc_cfg_sel_START  (0)
#define BBPCOMM_AFC_CFG_SEL_MIX_afc_cfg_sel_END    (0)


/***======================================================================***
                     (2/3) register_define_afc_mix_ch1
 ***======================================================================***/
/*****************************************************************************
 结构名    : BBPCOMM_AFC_PWM_SEL_MIX_CH1_UNION
 结构说明  : AFC_PWM_SEL_MIX_CH1 寄存器结构定义。地址偏移量:0x00，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_pwm_sel_mix_ch1_reg;
    struct
    {
        unsigned long  afc_pwm_sel_ch1 : 1;  /* bit[0]   : 0，表示配置AFC选择PDM输出；
                                                           1，表示配置选择PWM输出 */
        unsigned long  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_AFC_PWM_SEL_MIX_CH1_UNION;
#define BBPCOMM_AFC_PWM_SEL_MIX_CH1_afc_pwm_sel_ch1_START  (0)
#define BBPCOMM_AFC_PWM_SEL_MIX_CH1_afc_pwm_sel_ch1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_AFC_EN_MIX_CH1_UNION
 结构说明  : AFC_EN_MIX_CH1 寄存器结构定义。地址偏移量:0x04，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_en_mix_ch1_reg;
    struct
    {
        unsigned long  afc_en_ch1 : 1;  /* bit[0]   : 1，表示配置AFC使能；
                                                      0，表示配置AFC不使能 */
        unsigned long  reserved   : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_AFC_EN_MIX_CH1_UNION;
#define BBPCOMM_AFC_EN_MIX_CH1_afc_en_ch1_START  (0)
#define BBPCOMM_AFC_EN_MIX_CH1_afc_en_ch1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_AFC_CFG_WIDTH_MIX_CH1_UNION
 结构说明  : AFC_CFG_WIDTH_MIX_CH1 寄存器结构定义。地址偏移量:0x08，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_cfg_width_mix_ch1_reg;
    struct
    {
        unsigned long  afc_cfg_width_ch1 : 12; /* bit[0-11] : AFC配置值 */
        unsigned long  reserved          : 20; /* bit[12-31]: 保留 */
    } reg;
} BBPCOMM_AFC_CFG_WIDTH_MIX_CH1_UNION;
#define BBPCOMM_AFC_CFG_WIDTH_MIX_CH1_afc_cfg_width_ch1_START  (0)
#define BBPCOMM_AFC_CFG_WIDTH_MIX_CH1_afc_cfg_width_ch1_END    (11)


/*****************************************************************************
 结构名    : BBPCOMM_AFC_CFG_SEL_MIX_CH1_UNION
 结构说明  : AFC_CFG_SEL_MIX_CH1 寄存器结构定义。地址偏移量:0x0C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_cfg_sel_mix_ch1_reg;
    struct
    {
        unsigned long  afc_cfg_sel_ch1 : 1;  /* bit[0]   : 1，表示选择配置AFC值
                                                           0，表示选择BBP产生的AFC值 */
        unsigned long  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_AFC_CFG_SEL_MIX_CH1_UNION;
#define BBPCOMM_AFC_CFG_SEL_MIX_CH1_afc_cfg_sel_ch1_START  (0)
#define BBPCOMM_AFC_CFG_SEL_MIX_CH1_afc_cfg_sel_ch1_END    (0)


/***======================================================================***
                     (3/3) register_define_hkadc_ssi
 ***======================================================================***/
/*****************************************************************************
 结构名    : BBPCOMM_DSP_START_UNION
 结构说明  : DSP_START 寄存器结构定义。地址偏移量:0x00，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      dsp_start_reg;
    struct
    {
        unsigned long  dsp_start : 1;  /* bit[0-0] : 软件配置 */
        unsigned long  reserved  : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_START_UNION;
#define BBPCOMM_DSP_START_dsp_start_START  (0)
#define BBPCOMM_DSP_START_dsp_start_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_CFG_UNION
 结构说明  : DSP_CFG 寄存器结构定义。地址偏移量:0x04，初值:0x000C1F3E，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      dsp_cfg_reg;
    struct
    {
        unsigned long  ass        : 3;  /* bit[0-2]  : 保留，不需要配置。 */
        unsigned long  dss        : 5;  /* bit[3-7]  : 保留，不需要配置。 */
        unsigned long  wait_time  : 4;  /* bit[8-11] : SSI-IP需要的设置超时等待时间，单位cycle。 
                                                       time+1。 */
        unsigned long  mst_ck_cfg : 3;  /* bit[12-14]: master check读同步位时间：
                                                       000： 1.5clk； 
                                                       001： 2.0clk；
                                                        ,,, 
                                                       111： 5.0 clk.
                                                       默认值配置为2. */
        unsigned long  rw_bit     : 5;  /* bit[15-19]: 保留，不需要配置。 */
        unsigned long  reserved   : 12; /* bit[20-31]: 保留 */
    } reg;
} BBPCOMM_DSP_CFG_UNION;
#define BBPCOMM_DSP_CFG_ass_START         (0)
#define BBPCOMM_DSP_CFG_ass_END           (2)
#define BBPCOMM_DSP_CFG_dss_START         (3)
#define BBPCOMM_DSP_CFG_dss_END           (7)
#define BBPCOMM_DSP_CFG_wait_time_START   (8)
#define BBPCOMM_DSP_CFG_wait_time_END     (11)
#define BBPCOMM_DSP_CFG_mst_ck_cfg_START  (12)
#define BBPCOMM_DSP_CFG_mst_ck_cfg_END    (14)
#define BBPCOMM_DSP_CFG_rw_bit_START      (15)
#define BBPCOMM_DSP_CFG_rw_bit_END        (19)


/*****************************************************************************
 结构名    : BBPCOMM_TIME_OUT_ERR_CLR_UNION
 结构说明  : TIME_OUT_ERR_CLR 寄存器结构定义。地址偏移量:0x10，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      time_out_err_clr_reg;
    struct
    {
        unsigned long  time_out_err_clr : 1;  /* bit[0-0] : 超时清除脉冲信号。 */
        unsigned long  reserved         : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_TIME_OUT_ERR_CLR_UNION;
#define BBPCOMM_TIME_OUT_ERR_CLR_time_out_err_clr_START  (0)
#define BBPCOMM_TIME_OUT_ERR_CLR_time_out_err_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_TIME_OUT_ERR_UNION
 结构说明  : TIME_OUT_ERR 寄存器结构定义。地址偏移量:0x18，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      time_out_err_reg;
    struct
    {
        unsigned long  time_out_err : 1;  /* bit[0-0] : 软件HKADC请求超时状态指示电平信号。
                                                        如果超时，软件需要配置超时清除寄存器hkadc_time_out_err_clr。 */
        unsigned long  reserved     : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_TIME_OUT_ERR_UNION;
#define BBPCOMM_TIME_OUT_ERR_time_out_err_START  (0)
#define BBPCOMM_TIME_OUT_ERR_time_out_err_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_START_CLR_UNION
 结构说明  : DSP_START_CLR 寄存器结构定义。地址偏移量:0x1C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      dsp_start_clr_reg;
    struct
    {
        unsigned long  dsp_start_clr : 1;  /* bit[0-0] : 完成 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_START_CLR_UNION;
#define BBPCOMM_DSP_START_CLR_dsp_start_clr_START  (0)
#define BBPCOMM_DSP_START_CLR_dsp_start_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_WR01_DATA_UNION
 结构说明  : WR01_DATA 寄存器结构定义。地址偏移量:0x20，初值:0x00000000，宽度:32
 寄存器说明: 软件读写寄存器01配置值
*****************************************************************************/
typedef union
{
    unsigned long      wr01_data_reg;
    struct
    {
        unsigned long  dsp_wr0_cfg_data : 8;  /* bit[0-7]  : HKADC */
        unsigned long  dsp_wr0_cfg_addr : 7;  /* bit[8-14] : HKADC */
        unsigned long  dsp_wr0_cfg_wr   : 1;  /* bit[15-15]: HKADC */
        unsigned long  dsp_wr1_cfg_data : 8;  /* bit[16-23]: HKADC */
        unsigned long  dsp_wr1_cfg_addr : 7;  /* bit[24-30]: HKADC */
        unsigned long  dsp_wr1_cfg_wr   : 1;  /* bit[31-31]: HKADC */
    } reg;
} BBPCOMM_WR01_DATA_UNION;
#define BBPCOMM_WR01_DATA_dsp_wr0_cfg_data_START  (0)
#define BBPCOMM_WR01_DATA_dsp_wr0_cfg_data_END    (7)
#define BBPCOMM_WR01_DATA_dsp_wr0_cfg_addr_START  (8)
#define BBPCOMM_WR01_DATA_dsp_wr0_cfg_addr_END    (14)
#define BBPCOMM_WR01_DATA_dsp_wr0_cfg_wr_START    (15)
#define BBPCOMM_WR01_DATA_dsp_wr0_cfg_wr_END      (15)
#define BBPCOMM_WR01_DATA_dsp_wr1_cfg_data_START  (16)
#define BBPCOMM_WR01_DATA_dsp_wr1_cfg_data_END    (23)
#define BBPCOMM_WR01_DATA_dsp_wr1_cfg_addr_START  (24)
#define BBPCOMM_WR01_DATA_dsp_wr1_cfg_addr_END    (30)
#define BBPCOMM_WR01_DATA_dsp_wr1_cfg_wr_START    (31)
#define BBPCOMM_WR01_DATA_dsp_wr1_cfg_wr_END      (31)


/*****************************************************************************
 结构名    : BBPCOMM_WR34_DATA_UNION
 结构说明  : WR34_DATA 寄存器结构定义。地址偏移量:0x24，初值:0x00000000，宽度:32
 寄存器说明: 软件读写寄存器34配置值
*****************************************************************************/
typedef union
{
    unsigned long      wr34_data_reg;
    struct
    {
        unsigned long  dsp_wr3_cfg_data : 8;  /* bit[0-7]  : HKADC */
        unsigned long  dsp_wr3_cfg_addr : 7;  /* bit[8-14] : HKADC */
        unsigned long  dsp_wr3_cfg_wr   : 1;  /* bit[15-15]: HKADC */
        unsigned long  dsp_wr4_cfg_data : 8;  /* bit[16-23]: HKADC */
        unsigned long  dsp_wr4_cfg_addr : 7;  /* bit[24-30]: HKADC */
        unsigned long  dsp_wr4_cfg_wr   : 1;  /* bit[31-31]: HKADC */
    } reg;
} BBPCOMM_WR34_DATA_UNION;
#define BBPCOMM_WR34_DATA_dsp_wr3_cfg_data_START  (0)
#define BBPCOMM_WR34_DATA_dsp_wr3_cfg_data_END    (7)
#define BBPCOMM_WR34_DATA_dsp_wr3_cfg_addr_START  (8)
#define BBPCOMM_WR34_DATA_dsp_wr3_cfg_addr_END    (14)
#define BBPCOMM_WR34_DATA_dsp_wr3_cfg_wr_START    (15)
#define BBPCOMM_WR34_DATA_dsp_wr3_cfg_wr_END      (15)
#define BBPCOMM_WR34_DATA_dsp_wr4_cfg_data_START  (16)
#define BBPCOMM_WR34_DATA_dsp_wr4_cfg_data_END    (23)
#define BBPCOMM_WR34_DATA_dsp_wr4_cfg_addr_START  (24)
#define BBPCOMM_WR34_DATA_dsp_wr4_cfg_addr_END    (30)
#define BBPCOMM_WR34_DATA_dsp_wr4_cfg_wr_START    (31)
#define BBPCOMM_WR34_DATA_dsp_wr4_cfg_wr_END      (31)


/*****************************************************************************
 结构名    : BBPCOMM_HKADC_DELAY01_UNION
 结构说明  : HKADC_DELAY01 寄存器结构定义。地址偏移量:0x28，初值:0x00C0003C，宽度:32
 寄存器说明: 软件读写寄存器时延值
*****************************************************************************/
typedef union
{
    unsigned long      hkadc_delay01_reg;
    struct
    {
        unsigned long  dsp_delay1 : 16; /* bit[0-15] : HKADC */
        unsigned long  dsp_delay2 : 16; /* bit[16-31]: HKADC */
    } reg;
} BBPCOMM_HKADC_DELAY01_UNION;
#define BBPCOMM_HKADC_DELAY01_dsp_delay1_START  (0)
#define BBPCOMM_HKADC_DELAY01_dsp_delay1_END    (15)
#define BBPCOMM_HKADC_DELAY01_dsp_delay2_START  (16)
#define BBPCOMM_HKADC_DELAY01_dsp_delay2_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_HKADC_DBG_IND_UNION
 结构说明  : HKADC_DBG_IND 寄存器结构定义。地址偏移量:0xD0，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      hkadc_dbg_ind_reg;
    struct
    {
        unsigned long  ind_invalid : 1;  /* bit[0-0] :  */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_HKADC_DBG_IND_UNION;
#define BBPCOMM_HKADC_DBG_IND_ind_invalid_START  (0)
#define BBPCOMM_HKADC_DBG_IND_ind_invalid_END    (0)




/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of bbp_comm_on_interface.h */

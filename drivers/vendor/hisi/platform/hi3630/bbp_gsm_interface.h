

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __PHY_GBBP_INTERFACE_H__
#define __PHY_GBBP_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/***======================================================================***
                     (1/2) register_define_g_fe_ctrl
 ***======================================================================***/
/* 寄存器说明：
 bit[31:15]  保留
 bit[14:0]   当RFCMOS使用晶体方案时，软件gdsp配置的校准后温度补偿NV项值。
             该NV项值是开机时，晶体温度对应补偿曲线的补偿值。
   UNION结构:  GBBP_G_AFC_THERM_COM_NV_UNION */
#define GBBP_G_AFC_THERM_COM_NV_ADDR                  (GBBP_BASE_ADDR + 0x8600)

/* 寄存器说明：
 bit[31:30]  保留
 bit[29:16]  使用晶体AFC方案时输出给RF的AFC控制字值。
 bit[15:12]  保留
 bit[11:4]   选择晶体方案时，配置给RF的TCXO SSI地址bit。
 bit[3:1]    保留
 bit[0:0]    保留
   UNION结构:  GBBP_G_RF_SSI_AFC_UNION */
#define GBBP_G_RF_SSI_AFC_ADDR                        (GBBP_BASE_ADDR + 0x8604)

/* 寄存器说明：
 bit[31:29]  保留
 bit[28:28]  使用CPU方式将一个RF-SSI配置是否发送至BBP COMM模块的状态信号，测试用。
             0：表示结束，1表示未结束。
 bit[27:25]  保留
 bit[24:24]  GDSP使用CPU方式配置一个RF-SSI的启动信号。
             逻辑自清零。
 bit[23:16]  GDSP使用CPU方式配置一个RF-SSI的寄存器地址。
 bit[15:0]   GDSP使用CPU方式配置一个RF-SSI的寄存器对应的数据。
   UNION结构:  GBBP_G_RF_SSI_WRONE_CFG_UNION */
#define GBBP_G_RF_SSI_WRONE_CFG_ADDR                  (GBBP_BASE_ADDR + 0x8608)

/* 寄存器说明：
 bit[31:17]  保留
 bit[16:16]  读 ssi允许使能信号
 bit[15:13]  保留
 bit[12:12]  写一个rf-ssi允许使能信号
 bit[11:9]   保留
 bit[8:8]    g-afc ssi允许使能信号
 bit[7:5]    保留
 bit[4:4]    g-rf-ram ssi允许使能信号
 bit[3:1]    保留
 bit[0:0]    g-gagc ssi允许使能信号
   UNION结构:  GBBP_G_RF_SSI_REQ_EN_CFG_UNION */
#define GBBP_G_RF_SSI_REQ_EN_CFG_ADDR                 (GBBP_BASE_ADDR + 0x860C)

/* 寄存器说明：
 bit[31:13]  保留
 bit[12:12]  写ssi-ram完成指示信号。
             1：表示正在回读；
             0：表示完成回读操作。
 bit[11:9]   保留
 bit[8:8]    回读完成指示信号。
             1：表示正在回读；
             0：表示完成回读操作。
 bit[7:7]    保留
 bit[6:4]    RF-SSI回读寄存器个数。
             配置值为0~7，对应的回读寄存器个数为1~8。
             （注：配置为0表示回读一个寄存器）
 bit[3:1]    保留
 bit[0:0]    启动RF-SSI寄存器回读。
             逻辑自清零信号。
   UNION结构:  GBBP_G_RF_SSI_RD_CFG_UNION */
#define GBBP_G_RF_SSI_RD_CFG_ADDR                     (GBBP_BASE_ADDR + 0x8610)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:0]   SSI回读配置数据0。
   UNION结构:  GBBP_G_RF_SSI_RD_0DATA_UNION */
#define GBBP_G_RF_SSI_RD_0DATA_ADDR                   (GBBP_BASE_ADDR + 0x8614)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:0]   SSI回读配置数据1。
   UNION结构:  GBBP_G_RF_SSI_RD_1DATA_UNION */
#define GBBP_G_RF_SSI_RD_1DATA_ADDR                   (GBBP_BASE_ADDR + 0x8618)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:0]   SSI回读配置数据2。
   UNION结构:  GBBP_G_RF_SSI_RD_2DATA_UNION */
#define GBBP_G_RF_SSI_RD_2DATA_ADDR                   (GBBP_BASE_ADDR + 0x861C)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:0]   SSI回读配置数据3。
   UNION结构:  GBBP_G_RF_SSI_RD_3DATA_UNION */
#define GBBP_G_RF_SSI_RD_3DATA_ADDR                   (GBBP_BASE_ADDR + 0x8620)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:0]   SSI回读配置数据4。
   UNION结构:  GBBP_G_RF_SSI_RD_4DATA_UNION */
#define GBBP_G_RF_SSI_RD_4DATA_ADDR                   (GBBP_BASE_ADDR + 0x8624)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:0]   SSI回读配置数据5。
   UNION结构:  GBBP_G_RF_SSI_RD_5DATA_UNION */
#define GBBP_G_RF_SSI_RD_5DATA_ADDR                   (GBBP_BASE_ADDR + 0x8628)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:0]   SSI回读配置数据6。
   UNION结构:  GBBP_G_RF_SSI_RD_6DATA_UNION */
#define GBBP_G_RF_SSI_RD_6DATA_ADDR                   (GBBP_BASE_ADDR + 0x862C)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:0]   SSI回读配置数据7。
   UNION结构:  GBBP_G_RF_SSI_RD_7DATA_UNION */
#define GBBP_G_RF_SSI_RD_7DATA_ADDR                   (GBBP_BASE_ADDR + 0x8630)

/* 寄存器说明：
 bit[31:5]   保留
 bit[4]      GTC方式MIPI客户端工作使能开关，1表示通道打开，0表示通道关闭。
 bit[3:1]    保留
 bit[0]      CPU方式MIPI客户端工作使能开关，1表示通道打开，0表示通道关闭。
   UNION结构:  GBBP_G_MIPI_REQ_EN_CFG_UNION */
#define GBBP_G_MIPI_REQ_EN_CFG_ADDR                   (GBBP_BASE_ADDR + 0x8634)

/* 寄存器说明：
 bit[31:17]  保留
 bit[16:16]  软件方式配置MIPI请求时的触发脉冲信号。
 bit[15:14]  保留
 bit[13:8]   软件方式配置MIPI请求时，配置寄存器的个数。配置范围：1～63。
 bit[7:6]    保留
 bit[5:0]    软件方式配置MIPI请求时，配置寄存器组的首地址。
   UNION结构:  GBBP_G_MIPI_CMD_CFG_UNION */
#define GBBP_G_MIPI_CMD_CFG_ADDR                      (GBBP_BASE_ADDR + 0x8638)

/* 寄存器说明：
 bit[31:17]  保留
 bit[16]     软件配置SSI状态机异常情况的复位脉冲信号。
 bit[15:1]   保留
 bit[0]      软件配置MIPI状态机异常情况的复位脉冲信号。
   UNION结构:  GBBP_G_RF_SSI_MIPI_CLR_REG_UNION */
#define GBBP_G_RF_SSI_MIPI_CLR_REG_ADDR               (GBBP_BASE_ADDR + 0x863C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      ABB上下行数据自然码、补码模式选择。
             1：表示自然码，0：表示补码，GSM专用。
   UNION结构:  GBBP_G_ABB_CFG_SEL_UNION */
#define GBBP_G_ABB_CFG_SEL_ADDR                       (GBBP_BASE_ADDR + 0x8644)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0:0]    主模测G的方式选择。
             0：背景搜；
             1：非背景搜。
   UNION结构:  GBBP_G_MASTER_MEA_G_EN_UNION */
#define GBBP_G_MASTER_MEA_G_EN_ADDR                   (GBBP_BASE_ADDR + 0x8658)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:1]    环回模式使能。
 bit[0:0]    环回模式下，上行通道使能开关。
   UNION结构:  GBBP_G_ABB_TRX_LOOP_EN_UNION */
#define GBBP_G_ABB_TRX_LOOP_EN_ADDR                   (GBBP_BASE_ADDR + 0x865C)

/* 寄存器说明：
   详      述：保留
   UNION结构 ：无 */
#define GBBP_G_DBG_REG0_ADDR                          (GBBP_BASE_ADDR + 0x8660)

/* 寄存器说明：
   详      述：保留
   UNION结构 ：无 */
#define GBBP_G_DBG_REG1_ADDR                          (GBBP_BASE_ADDR + 0x8664)

/* 寄存器说明：
 bit[31:27]  保留
 bit[26:16]  AuxDAC打开控制字配置，该控制字在gtc_gapc_en线控信号上升沿发送至AuxDAC。
 bit[15:11]  保留
 bit[10:0]   AuxDAC打开控制字配置，该控制字在gtc_gapc_en线控下降沿发送至AuxDAC。
   UNION结构:  GBBP_G_AUXDAC_EN_CFG_UNION */
#define GBBP_G_AUXDAC_EN_CFG_ADDR                     (GBBP_BASE_ADDR + 0x8690)



/***======================================================================***
                     (2/2) register_define_g_fe_ctrl_mem
 ***======================================================================***/
/* 寄存器说明：g本频或异频H的rfic-ssi RAM
 bit[31:24]  保留
 bit[23:16]  g本频或强配的rfic-ssi RAM。
             数据：
             [23:16]：rfic的寄存器地址；
 bit[15:0]   g本频或强配的rfic-ssi RAM。
             数据
             [15:0]：rfic的寄存器数据。
   UNION结构:  GBBP_G_RFIC_SSI_RAM_UNION */
#define GBBP_G_RFIC_SSI_RAM_ADDR                      (GBBP_BASE_ADDR + 0x8200)
#define GBBP_G_RFIC_SSI_RAM_MEMDEPTH  (192)

/* 寄存器说明：g本频或异频的mipi RAM
   详      述：g本频或异频的mipi RAM。
   UNION结构 ：无 */
#define GBBP_G_MIPI_SSI_RAM_ADDR                      (GBBP_BASE_ADDR + 0x8000)
#define GBBP_G_MIPI_SSI_RAM_MEMDEPTH  (64)





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
                     (1/2) register_define_g_fe_ctrl
 ***======================================================================***/
/*****************************************************************************
 结构名    : GBBP_G_AFC_THERM_COM_NV_UNION
 结构说明  : G_AFC_THERM_COM_NV 寄存器结构定义。地址偏移量:0x0600，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_afc_therm_com_nv_reg;
    struct
    {
        unsigned long  cpu_g_afc_therm_com_nv : 15; /* bit[0-14] : 当RFCMOS使用晶体方案时，软件gdsp配置的校准后温度补偿NV项值。
                                                                   该NV项值是开机时，晶体温度对应补偿曲线的补偿值。 */
        unsigned long  reserved               : 17; /* bit[15-31]: 保留 */
    } reg;
} GBBP_G_AFC_THERM_COM_NV_UNION;
#define GBBP_G_AFC_THERM_COM_NV_cpu_g_afc_therm_com_nv_START  (0)
#define GBBP_G_AFC_THERM_COM_NV_cpu_g_afc_therm_com_nv_END    (14)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_AFC_UNION
 结构说明  : G_RF_SSI_AFC 寄存器结构定义。地址偏移量:0x0604，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_afc_reg;
    struct
    {
        unsigned long  reserved_0          : 1;  /* bit[0-0]  : 保留 */
        unsigned long  reserved_1          : 3;  /* bit[1-3]  : 保留 */
        unsigned long  cpu_rf_afc_reg_addr : 8;  /* bit[4-11] : 选择晶体方案时，配置给RF的TCXO SSI地址bit。 */
        unsigned long  reserved_2          : 4;  /* bit[12-15]: 保留 */
        unsigned long  rf_ssi_afc_data_cpu : 14; /* bit[16-29]: 使用晶体AFC方案时输出给RF的AFC控制字值。 */
        unsigned long  reserved_3          : 2;  /* bit[30-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_AFC_UNION;
#define GBBP_G_RF_SSI_AFC_cpu_rf_afc_reg_addr_START  (4)
#define GBBP_G_RF_SSI_AFC_cpu_rf_afc_reg_addr_END    (11)
#define GBBP_G_RF_SSI_AFC_rf_ssi_afc_data_cpu_START  (16)
#define GBBP_G_RF_SSI_AFC_rf_ssi_afc_data_cpu_END    (29)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_WRONE_CFG_UNION
 结构说明  : G_RF_SSI_WRONE_CFG 寄存器结构定义。地址偏移量:0x0608，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_wrone_cfg_reg;
    struct
    {
        unsigned long  cpu_rf_ssi_wrone_data    : 16; /* bit[0-15] : GDSP使用CPU方式配置一个RF-SSI的寄存器对应的数据。 */
        unsigned long  cpu_rf_ssi_wrone_addr    : 8;  /* bit[16-23]: GDSP使用CPU方式配置一个RF-SSI的寄存器地址。 */
        unsigned long  cpu_rf_ssi_wrone_ind_imi : 1;  /* bit[24-24]: GDSP使用CPU方式配置一个RF-SSI的启动信号。
                                                                     逻辑自清零。 */
        unsigned long  reserved_0               : 3;  /* bit[25-27]: 保留 */
        unsigned long  rf_ssi_wrone_end_cpu     : 1;  /* bit[28-28]: 使用CPU方式将一个RF-SSI配置是否发送至BBP COMM模块的状态信号，测试用。
                                                                     0：表示结束，1表示未结束。 */
        unsigned long  reserved_1               : 3;  /* bit[29-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_WRONE_CFG_UNION;
#define GBBP_G_RF_SSI_WRONE_CFG_cpu_rf_ssi_wrone_data_START     (0)
#define GBBP_G_RF_SSI_WRONE_CFG_cpu_rf_ssi_wrone_data_END       (15)
#define GBBP_G_RF_SSI_WRONE_CFG_cpu_rf_ssi_wrone_addr_START     (16)
#define GBBP_G_RF_SSI_WRONE_CFG_cpu_rf_ssi_wrone_addr_END       (23)
#define GBBP_G_RF_SSI_WRONE_CFG_cpu_rf_ssi_wrone_ind_imi_START  (24)
#define GBBP_G_RF_SSI_WRONE_CFG_cpu_rf_ssi_wrone_ind_imi_END    (24)
#define GBBP_G_RF_SSI_WRONE_CFG_rf_ssi_wrone_end_cpu_START      (28)
#define GBBP_G_RF_SSI_WRONE_CFG_rf_ssi_wrone_end_cpu_END        (28)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_REQ_EN_CFG_UNION
 结构说明  : G_RF_SSI_REQ_EN_CFG 寄存器结构定义。地址偏移量:0x060C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_req_en_cfg_reg;
    struct
    {
        unsigned long  cpu_rf_ssi_gagc_en  : 1;  /* bit[0-0]  : g-gagc ssi允许使能信号 */
        unsigned long  reserved_0          : 3;  /* bit[1-3]  : 保留 */
        unsigned long  cpu_rf_ssi_wrram_en : 1;  /* bit[4-4]  : g-rf-ram ssi允许使能信号 */
        unsigned long  reserved_1          : 3;  /* bit[5-7]  : 保留 */
        unsigned long  cpu_rf_ssi_afc_en   : 1;  /* bit[8-8]  : g-afc ssi允许使能信号 */
        unsigned long  reserved_2          : 3;  /* bit[9-11] : 保留 */
        unsigned long  cpu_rf_ssi_wrone_en : 1;  /* bit[12-12]: 写一个rf-ssi允许使能信号 */
        unsigned long  reserved_3          : 3;  /* bit[13-15]: 保留 */
        unsigned long  cpu_rf_ssi_rd_en    : 1;  /* bit[16-16]: 读 ssi允许使能信号 */
        unsigned long  reserved_4          : 15; /* bit[17-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_REQ_EN_CFG_UNION;
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_gagc_en_START   (0)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_gagc_en_END     (0)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_wrram_en_START  (4)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_wrram_en_END    (4)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_afc_en_START    (8)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_afc_en_END      (8)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_wrone_en_START  (12)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_wrone_en_END    (12)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_rd_en_START     (16)
#define GBBP_G_RF_SSI_REQ_EN_CFG_cpu_rf_ssi_rd_en_END       (16)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_CFG_UNION
 结构说明  : G_RF_SSI_RD_CFG 寄存器结构定义。地址偏移量:0x0610，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_cfg_reg;
    struct
    {
        unsigned long  cpu_rf_ssi_rd_ind_imi : 1;  /* bit[0-0]  : 启动RF-SSI寄存器回读。
                                                                  逻辑自清零信号。 */
        unsigned long  reserved_0            : 3;  /* bit[1-3]  : 保留 */
        unsigned long  cpu_rf_ssi_rd_num     : 3;  /* bit[4-6]  : RF-SSI回读寄存器个数。
                                                                  配置值为0~7，对应的回读寄存器个数为1~8。
                                                                  （注：配置为0表示回读一个寄存器） */
        unsigned long  reserved_1            : 1;  /* bit[7-7]  : 保留 */
        unsigned long  rf_ssi_rd_end_cpu     : 1;  /* bit[8-8]  : 回读完成指示信号。
                                                                  1：表示正在回读；
                                                                  0：表示完成回读操作。 */
        unsigned long  reserved_2            : 3;  /* bit[9-11] : 保留 */
        unsigned long  rf_ssi_wrram_end_cpu  : 1;  /* bit[12-12]: 写ssi-ram完成指示信号。
                                                                  1：表示正在回读；
                                                                  0：表示完成回读操作。 */
        unsigned long  reserved_3            : 19; /* bit[13-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_CFG_UNION;
#define GBBP_G_RF_SSI_RD_CFG_cpu_rf_ssi_rd_ind_imi_START  (0)
#define GBBP_G_RF_SSI_RD_CFG_cpu_rf_ssi_rd_ind_imi_END    (0)
#define GBBP_G_RF_SSI_RD_CFG_cpu_rf_ssi_rd_num_START      (4)
#define GBBP_G_RF_SSI_RD_CFG_cpu_rf_ssi_rd_num_END        (6)
#define GBBP_G_RF_SSI_RD_CFG_rf_ssi_rd_end_cpu_START      (8)
#define GBBP_G_RF_SSI_RD_CFG_rf_ssi_rd_end_cpu_END        (8)
#define GBBP_G_RF_SSI_RD_CFG_rf_ssi_wrram_end_cpu_START   (12)
#define GBBP_G_RF_SSI_RD_CFG_rf_ssi_wrram_end_cpu_END     (12)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_0DATA_UNION
 结构说明  : G_RF_SSI_RD_0DATA 寄存器结构定义。地址偏移量:0x0614，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_0data_reg;
    struct
    {
        unsigned long  cpu_rfic_ssi_rd_0data : 24; /* bit[0-23] : SSI回读配置数据0。 */
        unsigned long  reserved              : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_0DATA_UNION;
#define GBBP_G_RF_SSI_RD_0DATA_cpu_rfic_ssi_rd_0data_START  (0)
#define GBBP_G_RF_SSI_RD_0DATA_cpu_rfic_ssi_rd_0data_END    (23)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_1DATA_UNION
 结构说明  : G_RF_SSI_RD_1DATA 寄存器结构定义。地址偏移量:0x0618，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_1data_reg;
    struct
    {
        unsigned long  cpu_rfic_ssi_rd_1data : 24; /* bit[0-23] : SSI回读配置数据1。 */
        unsigned long  reserved              : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_1DATA_UNION;
#define GBBP_G_RF_SSI_RD_1DATA_cpu_rfic_ssi_rd_1data_START  (0)
#define GBBP_G_RF_SSI_RD_1DATA_cpu_rfic_ssi_rd_1data_END    (23)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_2DATA_UNION
 结构说明  : G_RF_SSI_RD_2DATA 寄存器结构定义。地址偏移量:0x061C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_2data_reg;
    struct
    {
        unsigned long  cpu_rfic_ssi_rd_2data : 24; /* bit[0-23] : SSI回读配置数据2。 */
        unsigned long  reserved              : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_2DATA_UNION;
#define GBBP_G_RF_SSI_RD_2DATA_cpu_rfic_ssi_rd_2data_START  (0)
#define GBBP_G_RF_SSI_RD_2DATA_cpu_rfic_ssi_rd_2data_END    (23)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_3DATA_UNION
 结构说明  : G_RF_SSI_RD_3DATA 寄存器结构定义。地址偏移量:0x0620，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_3data_reg;
    struct
    {
        unsigned long  cpu_rfic_ssi_rd_3data : 24; /* bit[0-23] : SSI回读配置数据3。 */
        unsigned long  reserved              : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_3DATA_UNION;
#define GBBP_G_RF_SSI_RD_3DATA_cpu_rfic_ssi_rd_3data_START  (0)
#define GBBP_G_RF_SSI_RD_3DATA_cpu_rfic_ssi_rd_3data_END    (23)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_4DATA_UNION
 结构说明  : G_RF_SSI_RD_4DATA 寄存器结构定义。地址偏移量:0x0624，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_4data_reg;
    struct
    {
        unsigned long  cpu_rfic_ssi_rd_4data : 24; /* bit[0-23] : SSI回读配置数据4。 */
        unsigned long  reserved              : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_4DATA_UNION;
#define GBBP_G_RF_SSI_RD_4DATA_cpu_rfic_ssi_rd_4data_START  (0)
#define GBBP_G_RF_SSI_RD_4DATA_cpu_rfic_ssi_rd_4data_END    (23)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_5DATA_UNION
 结构说明  : G_RF_SSI_RD_5DATA 寄存器结构定义。地址偏移量:0x0628，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_5data_reg;
    struct
    {
        unsigned long  cpu_rfic_ssi_rd_5data : 24; /* bit[0-23] : SSI回读配置数据5。 */
        unsigned long  reserved              : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_5DATA_UNION;
#define GBBP_G_RF_SSI_RD_5DATA_cpu_rfic_ssi_rd_5data_START  (0)
#define GBBP_G_RF_SSI_RD_5DATA_cpu_rfic_ssi_rd_5data_END    (23)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_6DATA_UNION
 结构说明  : G_RF_SSI_RD_6DATA 寄存器结构定义。地址偏移量:0x062C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_6data_reg;
    struct
    {
        unsigned long  cpu_rfic_ssi_rd_6data : 24; /* bit[0-23] : SSI回读配置数据6。 */
        unsigned long  reserved              : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_6DATA_UNION;
#define GBBP_G_RF_SSI_RD_6DATA_cpu_rfic_ssi_rd_6data_START  (0)
#define GBBP_G_RF_SSI_RD_6DATA_cpu_rfic_ssi_rd_6data_END    (23)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_RD_7DATA_UNION
 结构说明  : G_RF_SSI_RD_7DATA 寄存器结构定义。地址偏移量:0x0630，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_rd_7data_reg;
    struct
    {
        unsigned long  cpu_rfic_ssi_rd_7data : 24; /* bit[0-23] : SSI回读配置数据7。 */
        unsigned long  reserved              : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_RD_7DATA_UNION;
#define GBBP_G_RF_SSI_RD_7DATA_cpu_rfic_ssi_rd_7data_START  (0)
#define GBBP_G_RF_SSI_RD_7DATA_cpu_rfic_ssi_rd_7data_END    (23)


/*****************************************************************************
 结构名    : GBBP_G_MIPI_REQ_EN_CFG_UNION
 结构说明  : G_MIPI_REQ_EN_CFG 寄存器结构定义。地址偏移量:0x0634，初值:0x00000011，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_mipi_req_en_cfg_reg;
    struct
    {
        unsigned long  cpu_mipi_cmd_cfg_en       : 1;  /* bit[0]   : CPU方式MIPI客户端工作使能开关，1表示通道打开，0表示通道关闭。 */
        unsigned long  reserved_0                : 3;  /* bit[1-3] : 保留 */
        unsigned long  cpu_gtc_mipi_start_cfg_en : 1;  /* bit[4]   : GTC方式MIPI客户端工作使能开关，1表示通道打开，0表示通道关闭。 */
        unsigned long  reserved_1                : 27; /* bit[5-31]: 保留 */
    } reg;
} GBBP_G_MIPI_REQ_EN_CFG_UNION;
#define GBBP_G_MIPI_REQ_EN_CFG_cpu_mipi_cmd_cfg_en_START        (0)
#define GBBP_G_MIPI_REQ_EN_CFG_cpu_mipi_cmd_cfg_en_END          (0)
#define GBBP_G_MIPI_REQ_EN_CFG_cpu_gtc_mipi_start_cfg_en_START  (4)
#define GBBP_G_MIPI_REQ_EN_CFG_cpu_gtc_mipi_start_cfg_en_END    (4)


/*****************************************************************************
 结构名    : GBBP_G_MIPI_CMD_CFG_UNION
 结构说明  : G_MIPI_CMD_CFG 寄存器结构定义。地址偏移量:0x0638，初值:0x00000100，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_mipi_cmd_cfg_reg;
    struct
    {
        unsigned long  cpu_mipi_cmd_ini_addr : 6;  /* bit[0-5]  : 软件方式配置MIPI请求时，配置寄存器组的首地址。 */
        unsigned long  reserved_0            : 2;  /* bit[6-7]  : 保留 */
        unsigned long  cpu_mipi_cmd_num      : 6;  /* bit[8-13] : 软件方式配置MIPI请求时，配置寄存器的个数。配置范围：1～63。 */
        unsigned long  reserved_1            : 2;  /* bit[14-15]: 保留 */
        unsigned long  cpu_mipi_cmd_ind_imi  : 1;  /* bit[16-16]: 软件方式配置MIPI请求时的触发脉冲信号。 */
        unsigned long  reserved_2            : 15; /* bit[17-31]: 保留 */
    } reg;
} GBBP_G_MIPI_CMD_CFG_UNION;
#define GBBP_G_MIPI_CMD_CFG_cpu_mipi_cmd_ini_addr_START  (0)
#define GBBP_G_MIPI_CMD_CFG_cpu_mipi_cmd_ini_addr_END    (5)
#define GBBP_G_MIPI_CMD_CFG_cpu_mipi_cmd_num_START       (8)
#define GBBP_G_MIPI_CMD_CFG_cpu_mipi_cmd_num_END         (13)
#define GBBP_G_MIPI_CMD_CFG_cpu_mipi_cmd_ind_imi_START   (16)
#define GBBP_G_MIPI_CMD_CFG_cpu_mipi_cmd_ind_imi_END     (16)


/*****************************************************************************
 结构名    : GBBP_G_RF_SSI_MIPI_CLR_REG_UNION
 结构说明  : G_RF_SSI_MIPI_CLR_REG 寄存器结构定义。地址偏移量:0x063C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_rf_ssi_mipi_clr_reg_reg;
    struct
    {
        unsigned long  cpu_mipi_clr_ind_imi   : 1;  /* bit[0]    : 软件配置MIPI状态机异常情况的复位脉冲信号。 */
        unsigned long  reserved_0             : 15; /* bit[1-15] : 保留 */
        unsigned long  cpu_rf_ssi_clr_ind_imi : 1;  /* bit[16]   : 软件配置SSI状态机异常情况的复位脉冲信号。 */
        unsigned long  reserved_1             : 15; /* bit[17-31]: 保留 */
    } reg;
} GBBP_G_RF_SSI_MIPI_CLR_REG_UNION;
#define GBBP_G_RF_SSI_MIPI_CLR_REG_cpu_mipi_clr_ind_imi_START    (0)
#define GBBP_G_RF_SSI_MIPI_CLR_REG_cpu_mipi_clr_ind_imi_END      (0)
#define GBBP_G_RF_SSI_MIPI_CLR_REG_cpu_rf_ssi_clr_ind_imi_START  (16)
#define GBBP_G_RF_SSI_MIPI_CLR_REG_cpu_rf_ssi_clr_ind_imi_END    (16)


/*****************************************************************************
 结构名    : GBBP_G_ABB_CFG_SEL_UNION
 结构说明  : G_ABB_CFG_SEL 寄存器结构定义。地址偏移量:0x0644，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_abb_cfg_sel_reg;
    struct
    {
        unsigned long  cpu_abb_code_sel_g : 1;  /* bit[0]   : ABB上下行数据自然码、补码模式选择。
                                                              1：表示自然码，0：表示补码，GSM专用。 */
        unsigned long  reserved           : 31; /* bit[1-31]: 保留 */
    } reg;
} GBBP_G_ABB_CFG_SEL_UNION;
#define GBBP_G_ABB_CFG_SEL_cpu_abb_code_sel_g_START  (0)
#define GBBP_G_ABB_CFG_SEL_cpu_abb_code_sel_g_END    (0)


/*****************************************************************************
 结构名    : GBBP_G_MASTER_MEA_G_EN_UNION
 结构说明  : G_MASTER_MEA_G_EN 寄存器结构定义。地址偏移量:0x0658，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_master_mea_g_en_reg;
    struct
    {
        unsigned long  cpu_master_mea_g_en : 1;  /* bit[0-0] : 主模测G的方式选择。
                                                               0：背景搜；
                                                               1：非背景搜。 */
        unsigned long  reserved            : 31; /* bit[1-31]: 保留 */
    } reg;
} GBBP_G_MASTER_MEA_G_EN_UNION;
#define GBBP_G_MASTER_MEA_G_EN_cpu_master_mea_g_en_START  (0)
#define GBBP_G_MASTER_MEA_G_EN_cpu_master_mea_g_en_END    (0)


/*****************************************************************************
 结构名    : GBBP_G_ABB_TRX_LOOP_EN_UNION
 结构说明  : G_ABB_TRX_LOOP_EN 寄存器结构定义。地址偏移量:0x065C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_abb_trx_loop_en_reg;
    struct
    {
        unsigned long  cpu_gabb_trx_loop_tx_en : 1;  /* bit[0-0] : 环回模式下，上行通道使能开关。 */
        unsigned long  cpu_gabb_trx_loop_en    : 1;  /* bit[1-1] : 环回模式使能。 */
        unsigned long  reserved                : 30; /* bit[2-31]: 保留 */
    } reg;
} GBBP_G_ABB_TRX_LOOP_EN_UNION;
#define GBBP_G_ABB_TRX_LOOP_EN_cpu_gabb_trx_loop_tx_en_START  (0)
#define GBBP_G_ABB_TRX_LOOP_EN_cpu_gabb_trx_loop_tx_en_END    (0)
#define GBBP_G_ABB_TRX_LOOP_EN_cpu_gabb_trx_loop_en_START     (1)
#define GBBP_G_ABB_TRX_LOOP_EN_cpu_gabb_trx_loop_en_END       (1)


/*****************************************************************************
 结构名    : GBBP_G_AUXDAC_EN_CFG_UNION
 结构说明  : G_AUXDAC_EN_CFG 寄存器结构定义。地址偏移量:0x0690，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      g_auxdac_en_cfg_reg;
    struct
    {
        unsigned long  cpu_g_auxdac_close_cfg : 11; /* bit[0-10] : AuxDAC打开控制字配置，该控制字在gtc_gapc_en线控下降沿发送至AuxDAC。 */
        unsigned long  reserved_0             : 5;  /* bit[11-15]: 保留 */
        unsigned long  cpu_g_auxdac_open_cfg  : 11; /* bit[16-26]: AuxDAC打开控制字配置，该控制字在gtc_gapc_en线控信号上升沿发送至AuxDAC。 */
        unsigned long  reserved_1             : 5;  /* bit[27-31]: 保留 */
    } reg;
} GBBP_G_AUXDAC_EN_CFG_UNION;
#define GBBP_G_AUXDAC_EN_CFG_cpu_g_auxdac_close_cfg_START  (0)
#define GBBP_G_AUXDAC_EN_CFG_cpu_g_auxdac_close_cfg_END    (10)
#define GBBP_G_AUXDAC_EN_CFG_cpu_g_auxdac_open_cfg_START   (16)
#define GBBP_G_AUXDAC_EN_CFG_cpu_g_auxdac_open_cfg_END     (26)


/***======================================================================***
                     (2/2) register_define_g_fe_ctrl_mem
 ***======================================================================***/
/*****************************************************************************
 结构名    : GBBP_G_RFIC_SSI_RAM_UNION
 结构说明  : G_RFIC_SSI_RAM 寄存器结构定义。地址偏移量:0x200，初值:0x00000000，宽度:32
 寄存器说明: g本频或异频H的rfic-ssi RAM
*****************************************************************************/
typedef union
{
    unsigned long      g_rfic_ssi_ram_reg;
    struct
    {
        unsigned long  g_rfic_ssi_ram_data : 16; /* bit[0-15] : g本频或强配的rfic-ssi RAM。
                                                                数据
                                                                [15:0]：rfic的寄存器数据。 */
        unsigned long  g_rfic_ssi_ram_addr : 8;  /* bit[16-23]: g本频或强配的rfic-ssi RAM。
                                                                数据：
                                                                [23:16]：rfic的寄存器地址； */
        unsigned long  reserved            : 8;  /* bit[24-31]: 保留 */
    } reg;
} GBBP_G_RFIC_SSI_RAM_UNION;
#define GBBP_G_RFIC_SSI_RAM_g_rfic_ssi_ram_data_START  (0)
#define GBBP_G_RFIC_SSI_RAM_g_rfic_ssi_ram_data_END    (15)
#define GBBP_G_RFIC_SSI_RAM_g_rfic_ssi_ram_addr_START  (16)
#define GBBP_G_RFIC_SSI_RAM_g_rfic_ssi_ram_addr_END    (23)




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

#endif /* end of phy_gbbp_interface.h */

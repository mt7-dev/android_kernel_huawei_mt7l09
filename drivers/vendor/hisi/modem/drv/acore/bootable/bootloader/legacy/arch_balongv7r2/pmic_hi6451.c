
#include <boot/boot.h>
#include <spi.h>
#include <hi_pmu.h>
#include <pmu.h>
#include "pmic_hi6451.h"

/*****************************************************************************
* 函 数 名  : hi6451_reg_write
*
* 功能描述  : 对pmu芯片寄存器的写函数
*
* 输入参数  : u8 u8Addr：待写入的寄存器地址
*                    u8 u8Data：待写入的数据
*
* 输出参数  : 无
*
* 返 回 值  : ERROR：写入失败
*             OK：写入成功
*****************************************************************************/
s32 bsp_hi6451_reg_write(u8 u8Addr, u8 u8Data)
{
    u16 frameSend = (u16)((u8Addr << ADDR_BIT_OFFSET) | u8Data);

    /* 通过SPI向设备写入数据 */
    return spi_send(SPI_NO_PMU, SPI_CS_PMU, &frameSend, 1);
}

/*****************************************************************************
* 函 数 名  : hi6451_reg_read
*
* 功能描述  : 对pmu芯片寄存器的读函数
*
* 输入参数  : u8Addr： 待读取的寄存器地址
*               pu8Data：读取的数据存放的地址
*
* 输出参数  : u8 *pu8Data：读取的数据存放的地址
*
* 返 回 值  : ERROR：读取失败
*                    OK：读取成功
*****************************************************************************/
s32 bsp_hi6451_reg_read(u8 u8Addr, u8 *pu8Data)
{
    u16 frameSend = (u16)((u8Addr << ADDR_BIT_OFFSET) | (0x1 << CMD_BIT_OFFSET));
    u16 frameRecv = 0;   /* A11收发帧格式均为16bit，但有效数据为8bit */
    s32 retval = 0;

    /* 通过SPI从设备读取数据 */
    retval = spi_recv(SPI_NO_PMU, SPI_CS_PMU, &frameRecv, 1, &frameSend, 1);
    *pu8Data = (u8)frameRecv;
    return retval;
}
/*****************************************************************************
 函 数 名  : hi6451_reg_write_mask
 功能描述  : 设置hi6451寄存器的某些bit
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : 操作hi6451寄存器时使用
*****************************************************************************/
int bsp_hi6451_reg_write_mask(u8 addr, u8 value, u8 mask)
{
    u8 reg_tmp = 0;

    bsp_hi6451_reg_read(addr, &reg_tmp);

    reg_tmp &= ~mask;
    reg_tmp |= value;

    return bsp_hi6451_reg_write(addr, reg_tmp);
}
/*****************************************************************************
 函 数 名  : pmu_hi6451_init
 功能描述  : pmu hi6451模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  : fastboot中pmu模块初始化时调用
*****************************************************************************/
void pmu_hi6451_init(void)
{
    s32 iRetVal;
    u8 iregVal;

    (void)spi_init(SPI_NO_PMU);

    /*初始化默认配置*/
    /*LDO14 2.6v,ON; LDO13\15,OFF*/
    iRetVal =  bsp_hi6451_reg_write(HI_PMU_SET_LDO14_OFFSET, 0x5);
    iRetVal |= bsp_hi6451_reg_read(HI_PMU_ONOFF4_OFFSET,&iregVal);
    iregVal |= 0x08;
    iRetVal |= bsp_hi6451_reg_write(HI_PMU_ONOFF4_OFFSET, iregVal);
    /*BUCK4 3.3V,ON; BUCK 1,2,3,4 ON*/
    iRetVal |= bsp_hi6451_reg_read(HI_PMU_ONOFF5_OFFSET,&iregVal);
    iregVal |=  0x02;
    iRetVal |= bsp_hi6451_reg_write(HI_PMU_ONOFF5_OFFSET, iregVal);

    if(iRetVal)
        cprintf("pmu_hi6451_init error!\n" );
    else
        cprintf("pmu_hi6451_init ok!\n" );
}
/*****************************************************************************
 函 数 名  : hi6451_power_key_state_get
 功能描述  : 获取power键是否按下
 输入参数  : void
 输出参数  : 无
 返 回 值  : power键是否按下：1:按下；0:未按下)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6451_power_key_state_get(void)
{
    u8 regval = 0;

    bsp_hi6451_reg_read(HI_PMU_STATUS4_OFFSET, &regval);
    return  (regval & PMU_HI6451_POWER_KEY_MASK) ? 1 : 0;
}
/*****************************************************************************
 函 数 名  : bsp_hi6451_usb_state_get
 功能描述  : 获取usb是否插拔状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : usb插入或拔出(1:插入；0:拔出)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6451_usb_state_get(void)
{

    u8 regval = 0;

    bsp_hi6451_reg_read(HI_PMU_STATUS5_OFFSET, &regval);
    return  (regval & PMU_HI6451_USB_STATE_MASK) ? 1 : 0;

}
/*****************************************************************************
 函 数 名  : hi6451_hreset_state_get
 功能描述  : 判断pmu是否为热启动
 输入参数  : void
 输出参数  : 无
 返 回 值     : pmu是热启动或冷启动(1:热启动；0:冷启动)
 调用函数  :
 被调函数  : 开关机模块
*****************************************************************************/
bool hi6451_hreset_state_get(void)
{
    u8 regval = 0;

    bsp_hi6451_reg_read(HI_PMU_NO_PWR_IRQ1_OFFSET, &regval);
    return  (regval & PMU_HI6451_HRESET_STATE_MASK) ? 1 : 0;
}
/*****************************************************************************
 函 数 名  : hi6451_version_get
 功能描述  : 获取pmu的版本号
 输入参数  : void
 输出参数  : 无
 返 回 值  : pmu版本号
 调用函数  :
 被调函数  : HSO集成,MSP调用
*****************************************************************************/
u8  hi6451_version_get(void)
{
    u8 regval = 0;

    bsp_hi6451_reg_read(HI_PMU_VERSION_OFFSET, &regval);
    return  regval;
}
/*****************************************************************************
 函 数 名  : hi6451_get_boot_state
 功能描述  :系统启动时检查pmu寄存器状态，
                确认是否是由pmu引起的重启
 输入参数  : void
 输出参数  : reset.log
 返 回 值  : pmu问题或ok
 调用函数  :
 被调函数  :系统可维可测
*****************************************************************************/
int hi6451_get_boot_state(void)
{

    u8 a_ucRecordReg[3] = {0};
    u32 i = 0;
    int iret = PMU_STATE_OK;

    /*读取非下电reg*/
    for(i = 0; i < 3; i++)
    {
        bsp_hi6451_reg_read((HI_PMU_NO_PWR_IRQ1_OFFSET+i), &a_ucRecordReg[i]);
    }

    /*欠压，过压*/
    if(a_ucRecordReg[0] & (0x01 << HI6451_VINI_UNDER_2P7_OFFSET ))
        iret |= PMU_STATE_UNDER_VOL;
    if(a_ucRecordReg[0] & (0x01 << HI6451_VINI_OVER_6P0_OFFSET ))
        iret |= PMU_STATE_OVER_VOL;
    /*过流*/
    if(a_ucRecordReg[1] & (0x01 << HI6451_CORE_OVER_CUR_OFFSET ) )
        iret |= PMU_STATE_OVER_CUR;
    /*过温*/
    if(a_ucRecordReg[2] & (0x01 << HI6451_OTMP_150_OFFSET ) )
        iret |= PMU_STATE_OVER_TEMP;

    return iret;
}
/*****************************************************************************
 函 数 名  : hi6451_ldo14_switch
 功能描述  : 开启或关闭LDO14
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : fastboot中lcd文件调用
*****************************************************************************/
void hi6451_ldo14_switch(power_switch_e sw)
{
    u8 regVal = 0;

    switch(sw)
    {
    case POWER_OFF:
        (void)bsp_hi6451_reg_read(HI_PMU_ONOFF4_OFFSET,&regVal);
        regVal &= ~ (0x01<<3);
        bsp_hi6451_reg_write(HI_PMU_ONOFF4_OFFSET,regVal);
        break;
    case POWER_ON:
        bsp_hi6451_reg_read(HI_PMU_ONOFF4_OFFSET,&regVal);
        regVal |=  (0x01<<3);
        bsp_hi6451_reg_write(HI_PMU_ONOFF4_OFFSET,regVal);
        break;
    default:
        break;
    }
    pmic_print_info("hi6451_ldo14_switch ok!");
}
/*****************************************************************************
 函 数 名  : hi6451_ldo14_volt_set
 功能描述  : 设置ldo14的电压
 输入参数  : int voltage：设置的电压值(只支持3/6/9/12/15/18/21/24uV)*
 输出参数  : 无
 返 回 值  : 操作成功或失败
 调用函数  :
 被调函数  : fastboot中lcd文件调用
*****************************************************************************/
int hi6451_ldo14_volt_set(int voltage)
{
    unsigned int i = 0;
    int valid = 0;
    int ldo14_volt[] = {1100000,1200000,1800000,2200000,2500000,2600000,2800000,2850000};

    for(i = 0;i <= sizeof(ldo14_volt)/sizeof(int);i++)
    {
        if (voltage == ldo14_volt[i])
        {
            valid = 1;
            break;
        }
    }
    if(valid)
    {
        bsp_hi6451_reg_write_mask(HI_PMU_SET_LDO14_OFFSET, i, 0x7);
        pmic_print_info("hi6451_ldo14_volt_set ok!");
        return BSP_PMU_OK;
    }
    else
    {
        pmic_print_error("ldo14 cann't support %d uV!\n",voltage);
        return BSP_PMU_ERROR;
    }

}

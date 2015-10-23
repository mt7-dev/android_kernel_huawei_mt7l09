
#include "stdio.h"
#include <vxWorks.h>
#include <intLib.h>
#include <sysLib.h>
#include <logLib.h>
#include "drv_comm.h"
#include "osl_types.h"
#include "wm8990_plat.h"
#include "bsp_i2c.h"

/*I2C操作状态 */
#define WM8990_OK        (0)
#define WM8990_ERROR     (-1)

/*I2C地址，0x34=0x1A(CSB=low)  0x36=0x1B(CSB=high) */
#define WM8990_I2C_ADDR  (0x1A)

/*I2C运行正常运行标志位。0表示没有初始化OK */
static unsigned int bI2CisRunning=0;

/*8990寄存器个数 */
#define WM8990_REG_SUM (0x40)

/*8990寄存器map表 */
volatile unsigned short wm8990_reg_map[WM8990_REG_SUM] = {0};

/*8990寄存器被改写标志位 */
unsigned char wm8990_reg_flg[WM8990_REG_SUM] = {0};

/*8990寄存器可读位-可写位，0=READ, 1=WRITE */
unsigned short wm8990_reg_rw_msk[WM8990_REG_SUM][2] = {{0}};

#ifdef WM8990_OPERATE_STAT

/* 8990写操作总记录条数 */
#define TOTAL_OPERATE_ITEM (1000)

/* 8990写操作次数记录 */
unsigned int reg_op_idx = 0;

/*8990写操作记录，高16表示写操作的地址，低16表示写的数据 */
unsigned int wm8990_reg_op[TOTAL_OPERATE_ITEM] = {0};
#endif

/*8990寄存器名称 */
char *wm8990_reg_name[] =
{
    "Reset",   "Power Management (1)", "Power Management (2)",
    "Power Management (3)",    "Audio Interface (1)",    "Audio Interface (2)",
    "Clocking (1)",    "Clocking (2)",    "Audio Interface (3)",
    "Audio Interface (4)",    "DAC CTRL",    "Left DAC Digital Volume",
    "Right DAC Digital Volume",    "Digital Side Tone",    "ADC CTRL",
    "Left ADC Digital Volume",    "Right ADC Digital Volume",    "",
    "GPIO CTRL 1",    "GPIO1",    "GPIO3 & GPIO4",    "GPIO5",
    "GPIOCTRL 2",    "GPIO_POL",    "Left Line Input 1&2 Volume",
    "Left Line Input 3&4 Volume",    "Right Line Input 1&2 Volume",
    "Right Line Input 3&4 Volume",    "Left Output Volume",
    "Right Output Volume",    "Line Outputs Volume",    "Out3/4 Volume",
    "Left OPGA Volume",    "Right OPGA Volume",    "Speaker Volume",
    "ClassD1",    "",    "ClassD3",    "ClassD4",
    "Input Mixer1",    "Input Mixer2",    "Input Mixer3",     "Input Mixer4",
    "Input Mixer5",    "Input Mixer6",    "Output Mixer1",    "Output Mixer2",
    "Output Mixer3",   "Output Mixer4",   "Output Mixer5",    "Output Mixer6",
    "Out3/4 Mixer",    "Line Mixer1",     "Line Mixer2",      "Speaker Mixer",
    "Additional Control",    "AntiPOP1",    "AntiPOP2",    "MICBIAS",    "",
    "PLL1",    "PLL2",    "PLL3"
};

/*8990操作函数名称 */
char *wm8990_op_func_name[]=
{
"probe","open","close",
"pm_dn","pm_up","is_dn","is_up","samp",
"i_sel","i_unsel","o_sel","o_unsel",
"nv_op",
"p_opn","p_clos","r_opn","r_clos","cb_opn","cb_clos","v_opn","v_clos"
};

const unsigned short wm8990_reg_default_attr[WM8990_REG_SUM][3] =
{
    /*reg  , read  , write */
    {0x8990, 0xFFFF, 0xFFFF},     /* R0  - Reset */ \
    {0x0000, 0x1F17, 0x1F17},     /* R1  - Power Management (1) */ \
    {0x6000, 0xEBF3, 0xEBF3},     /* R2  - Power Management (2) */ \
    {0x0000, 0x3DF3, 0x3DF3},     /* R3  - Power Management (3) */ \
    {0x4050, 0x0000, 0xF1F8},     /* R4  - Audio Interface (1) */ \
    {0x4000, 0x0000, 0xFC1F},     /* R5  - Audio Interface (2) */ \
    {0x01C8, 0x0000, 0xDFDE},     /* R6  - Clocking (1) */ \
    {0x0000, 0x0000, 0x7CFC},     /* R7  - Clocking (2) */ \
    {0x0040, 0x0000, 0xEFFF},     /* R8  - Audio Interface (3) */ \
    {0x0040, 0x0000, 0xAFFF},     /* R9  - Audio Interface (4) */ \
    {0x0004, 0x0000, 0x17F7},     /* R10 - DAC CTRL */ \
    {0x00C0, 0x0000, 0x01FF},     /* R11 - Left DAC Digital Volume */ \
    {0x00C0, 0x0000, 0x01FF},     /* R12 - Right DAC Digital Volume */ \
    {0x0000, 0x0000, 0x1FEF},     /* R13 - Digital Side Tone */ \
    {0x0100, 0x0000, 0x0163},     /* R14 - ADC CTRL */ \
    {0x00C0, 0x0000, 0x01FF},     /* R15 - Left ADC Digital Volume */ \
    {0x00C0, 0x0000, 0x01FF},     /* R16 - Right ADC Digital Volume */ \
    {0x0000, 0x0000, 0x0000},     /* R17 */ \
    {0x0000, 0x1FDD, 0x0FDD},     /* R18 - GPIO CTRL 1 */ \
    {0x1000, 0x0000, 0x00FF},     /* R19 - GPIO1 */ \
    {0x1010, 0x0000, 0xFFFF},     /* R20 - GPIO3 & GPIO4 */ \
    {0x1010, 0x0000, 0x00FF},     /* R21 - GPIO5 */ \
    {0x8000, 0x0000, 0xCFDD},     /* R22 - GPIOCTRL 2 */ \
    {0x0800, 0x1FDD, 0x1FDD},     /* R23 - GPIO_POL */ \
    {0x008B, 0x0000, 0x01DF},     /* R24 - Left Line Input 1&2 Volume */ \
    {0x008B, 0x0000, 0x01DF},     /* R25 - Left Line Input 3&4 Volume */ \
    {0x008B, 0x0000, 0x01DF},     /* R26 - Right Line Input 1&2 Volume */ \
    {0x008B, 0x0000, 0x01DF},     /* R27 - Right Line Input 3&4 Volume */ \
    {0x0000, 0x0000, 0x01FF},     /* R28 - Left Output Volume */ \
    {0x0000, 0x0000, 0x01FF},     /* R29 - Right Output Volume */ \
    {0x0066, 0x0000, 0x0077},     /* R30 - Line Outputs Volume */ \
    {0x0022, 0x0000, 0x0033},     /* R31 - Out3/4 Volume */ \
    {0x0079, 0x0000, 0x01FF},     /* R32 - Left OPGA Volume */ \
    {0x0079, 0x0000, 0x01FF},     /* R33 - Right OPGA Volume */ \
    {0x0003, 0x0000, 0x0003},     /* R34 - Speaker Volume */ \
    {0x0003, 0x0000, 0x0100},     /* R35 - ClassD1 */ \
    {0x0000, 0x0000, 0x0000},     /* R36 */ \
    {0x0100, 0x0000, 0x003F},     /* R37 - ClassD3 */ \
    {0x0079, 0x0000, 0x00FF},     /* R38 - ClassD4 */ \
    {0x0000, 0x0000, 0x000F},     /* R39 - Input Mixer1 */ \
    {0x0000, 0x0000, 0x00FF},     /* R40 - Input Mixer2 */ \
    {0x0000, 0x0000, 0x01B7},     /* R41 - Input Mixer3 */ \
    {0x0000, 0x0000, 0x01B7},     /* R42 - Input Mixer4 */ \
    {0x0000, 0x0000, 0x01FF},     /* R43 - Input Mixer5 */ \
    {0x0000, 0x0000, 0x01FF},     /* R44 - Input Mixer6 */ \
    {0x0000, 0x0000, 0x00FD},     /* R45 - Output Mixer1 */ \
    {0x0000, 0x0000, 0x00FD},     /* R46 - Output Mixer2 */ \
    {0x0000, 0x0000, 0x01FF},     /* R47 - Output Mixer3 */ \
    {0x0000, 0x0000, 0x01FF},     /* R48 - Output Mixer4 */ \
    {0x0000, 0x0000, 0x01FF},     /* R49 - Output Mixer5 */ \
    {0x0000, 0x0000, 0x01FF},     /* R50 - Output Mixer6 */ \
    {0x0180, 0x0000, 0x01B3},     /* R51 - Out3/4 Mixer */ \
    {0x0000, 0x0000, 0x0077},     /* R52 - Line Mixer1 */ \
    {0x0000, 0x0000, 0x0077},     /* R53 - Line Mixer2 */ \
    {0x0000, 0x0000, 0x00FF},     /* R54 - Speaker Mixer */ \
    {0x0000, 0x0000, 0x0001},     /* R55 - Additional Control */ \
    {0x0000, 0x0000, 0x003F},     /* R56 - AntiPOP1 */ \
    {0x0000, 0x0000, 0x004F},     /* R57 - AntiPOP2 */ \
    {0x0000, 0x0000, 0x00FD},     /* R58 - MICBIAS */ \
    {0x0000, 0x0000, 0x0000},     /* R59 */ \
    {0x0008, 0x0000, 0x00CF},     /* R60 - PLL1 */ \
    {0x0031, 0x0000, 0x00FF},     /* R61 - PLL2 */ \
    {0x0026, 0x0000, 0x00FF},     /* R62 - PLL3 */ \
};

int bsp_i2c_data_rx(u8 addr)
{
    unsigned short data = 0;
    
    bsp_i2c_word_data_receive(WM8990_I2C_ADDR, addr, &data);
 
    return data;
}

int bsp_i2c_data_tx(u8 u8Addr, u16 u8Value)
{
    return bsp_i2c_word_data_send(WM8990_I2C_ADDR, u8Addr, u8Value);
}

/*******************************************************************************
  Function:     wm8990_init_reg
  Description:  codec寄存器复位后设置成默认值
  Input:
  Output:       none
  Return：      OK:操作成功;ERROsR:操作失败。
*******************************************************************************/
void wm8990_init_reg(void)
{
    int i;

    for (i = 0; i < WM8990_REG_SUM; i++)
    {
        wm8990_reg_map[i] = wm8990_reg_default_attr[i][0];
    }
}

/*******************************************************************************
  Function:     wm8990_init_rw_msk
  Description:  codec寄存器复位后设置成默认值
  Input:
  Output:       none
  Return：      OK:操作成功;ERROR:操作失败。
*******************************************************************************/
void wm8990_init_rw_msk(void)
{
    int i;

    for (i = 0; i < WM8990_REG_SUM; i++)
    {
        wm8990_reg_rw_msk[i][0] = wm8990_reg_default_attr[i][1];
        wm8990_reg_rw_msk[i][1] = wm8990_reg_default_attr[i][2];

        wm8990_reg_flg[i] = 0;
    }
}

/*******************************************************************************
  Function:     wm8990_i2c_init
  Description:  codec i2c初始化
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
int wm8990_i2c_init(void)
{
    bsp_i2c_slave_register(I2C_MASTER0, WM8990_I2C_ADDR);

    wm8990_init_reg();
    wm8990_init_rw_msk();

    bI2CisRunning=1;

    return WM8990_OK;
}

/*****************************************************************
 *  Function:  wm8990_reg_read
 *  Description:  read a wm8990 register via I2C.
 *
 *  Calls:
 *
 *  Called By: user
 *
 *  Table Accessed: NONE
 *
 *  Table Updated: NONE
 *
 *  Input:
 *         usRegAddr: 8990 register addr
 *         usRegData: data
 *
 *  Output:
 *         None
 *
 *  Return:
 *
 ******************************************************************/
int wm8990_reg_read(unsigned char addr, unsigned short *data)
{
    /*volatile int dwloop;*/
    int ret = WM8990_OK;
#if 0
    if (!bI2CisRunning)
    {
        DRV_LOG(BSP_MODU_AUDIO, 0,/*LOG_SUBMOD_ID_BUTT*/ BSP_LOG_LEVEL_ERROR, "--ERROR:wm8990 i2c not init yet!\r\n");
        return WM8990_ERROR;
    }
#endif

    if (wm8990_reg_rw_msk[addr][0])
    {
#if 0
        stMsgs[0].addr  = wm8990_client.addr;
        stMsgs[0].flags = I2C_M_WR;
        stMsgs[0].len = 1;
        stMsgs[0].buf = &addr;

        stMsgs[1].addr  = wm8990_client.addr;
        stMsgs[1].flags = I2C_M_RD;
        stMsgs[1].len = 2;
        stMsgs[1].buf = regdata;

        key = intLock();
        ret = i2cTransfer(wm8990_client.adapter, stMsgs, 2, wm8990_client.flag);
        OSAL_IntUnlock(key);
#endif
        *data = (unsigned short)bsp_i2c_data_rx(addr);

        #if 0 /*AT2D17247 start*/
        for (dwloop = 0x1000; dwloop > 0; dwloop--)
        {
            ;
        }
        #endif /*AT2D17247 end*/
#if 0
        regdata[1] = *data >> 8;
        regdata[0] = (unsigned char)*data;

        /*MSB 和 LSB位置调整 */
        pdata[0] = regdata[1];
        pdata[1] = regdata[0];
#endif
        /*将不可以读出的数据去掉 */
        *data &= wm8990_reg_rw_msk[addr][0];

        /*将map表中对应的数据去掉 */
        wm8990_reg_map[addr] &= ~wm8990_reg_rw_msk[addr][0];

        /*数据合并 */
        *data |= wm8990_reg_map[addr];

        /*更新到map表中 */
        wm8990_reg_map[addr] = *data;
    }
    else
    {
        *data = wm8990_reg_map[addr];
    }

    return ((ret == WM8990_OK) ? WM8990_OK : WM8990_ERROR);
}

/*****************************************************************
 *  Function:  wm8990_reg_write
 *  Description:  write a 9851 register via I2C.
 *
 *  Calls:
 *
 *  Called By: user
 *
 *  Table Accessed: NONE
 *
 *  Table Updated: NONE
 *
 *  Input:
 *         usRegAddr: 9851 register addr
 *         usRegData: data
 *
 *  Output:
 *         None
 *
 *  Return:
 *
 ******************************************************************/
int wm8990_reg_write(unsigned char usRegAddr, unsigned short usRegData)
{
    unsigned char ucData[3];
    /*volatile int dwloop;*/
    int key;
    int ret = WM8990_OK;

    /*判断可写标志位 */
    if (wm8990_reg_rw_msk[usRegAddr][1])
    {
        /*留下可写的那些位的数据 */
        usRegData &= wm8990_reg_rw_msk[usRegAddr][1];

        /*清空将要写的那些数据位 */
        wm8990_reg_map[usRegAddr] &= ~wm8990_reg_rw_msk[usRegAddr][1];

        /*数据合并 */
        usRegData |= wm8990_reg_map[usRegAddr];

        /*将新的寄存器值更新到map表中 */
        wm8990_reg_map[usRegAddr] = usRegData;

        ucData[0] = usRegAddr;
        ucData[1] = (unsigned char)((usRegData >> 8) & 0x00FF);
        ucData[2] = (unsigned char)(usRegData & 0x00FF);

/*        stWr.addr  = wm8990_client.addr; */
/*        stWr.flags = I2C_M_WR; */
/*        stWr.len = 3; */
/*        stWr.buf = ucData; */

        key = intLock();

        bsp_i2c_data_tx(usRegAddr, usRegData);
        intUnlock(key);

/*        key = intLock(); */
/*        ret = i2cTransfer(wm8990_client.adapter, &stWr, 1, 0); */
/*        OSAL_IntUnlock(key); */

        if (ret != WM8990_OK)
        {
            printf("--ERROR:wm8990 i2c write failure addr=%d, value=0x%4X, ret=!\r\n", usRegAddr, usRegData, ret);
        }

        if (0 == usRegAddr)
        {
            wm8990_init_reg();
        }
    }
    else
    {
        printf("--ERROR:wm8990 this register(%d) modify forbid!\r\n", usRegAddr);
        return WM8990_ERROR;
    }

#ifdef WM8990_OPERATE_STAT
    wm8990_reg_flg[usRegAddr] = 1;
    wm8990_reg_op[reg_op_idx] = ((usRegAddr << 16) | (usRegData));
    reg_op_idx = (reg_op_idx + 1) % TOTAL_OPERATE_ITEM;
#endif

    return ((ret == WM8990_OK) ? WM8990_OK : WM8990_ERROR);
}

/*****************************************************************
 *  Function:  wm8990_reg_writeD
 *  Description:  .
 *
 *  Calls:
 *
 *  Called By: user
 *
 *  Table Accessed: NONE
 *
 *  Table Updated: NONE
 *
 *  Input:
 *         bDebug: 1=TRUE,0=FALSE
 *
 *  Output:
 *         None
 *
 *  Return:
 *
 ******************************************************************/
int wm8990_reg_writeD(unsigned char usRegAddr, unsigned short mask, unsigned short usRegData)
{
    int lCamErroRet = 0;
    unsigned short reg;

    lCamErroRet = wm8990_reg_read(usRegAddr, &reg);

    reg &= ~mask;
    usRegData &= mask;

    reg |= usRegData;

    lCamErroRet |= wm8990_reg_write(usRegAddr, reg);

    if (lCamErroRet != 0)
    {
        return WM8990_ERROR;
    }
    else
    {
        return WM8990_OK;
    }
}

/*******************************************************************************
  Function:     wm8990_i2c_deinit
  Description:  codec i2c 去初始化
  Input:
  Output:       none
  Return：      OK: 操作成功;
*******************************************************************************/
int wm8990_i2c_deinit()
{
    bI2CisRunning=0;
	return WM8990_OK;
}



/***********************Test Code***********************************/


#ifdef WM8990_OPERATE_STAT

#define WM8990_OP_FUNCTION  (0xFFFF0000)

/*x:bit15~bit0 = bit15~bit8(argument) + bit7~bit0(func name) */
void wm8990_func_name_record(WM8990_OP_FUNC x)
{
    unsigned int temp;

    temp = ( (unsigned int)x & 0x0000FFFF );
    wm8990_reg_op[reg_op_idx] = (WM8990_OP_FUNCTION|temp);
    reg_op_idx++;
}

int wm8990reg_op_seq(int Hexmode)
{
    unsigned int i;

    printf("\r\n");
    printf("* ----- ------ -------------------- ------- --------- ------------------------------\r\n");
    printf("*  REG   DATA         ACCESS        READ OR  DEVICE\r\n");
    printf("* INDEX  VALUE         TYPE          WRITE   ADDRESS  COMMENT (for information only)\r\n");
    printf("* ----- ------ -------------------- ------- --------- ------------------------------\r\n");
    for(i=10;i>0;i--)
    {
        printf("");
    }
    for(i=0; i<reg_op_idx; i++)
    {
        if(Hexmode)
        {
            printf("  0x%02X \t0x%04X", (wm8990_reg_op[i]>>16), (wm8990_reg_op[i]&0x0000FFFF));
        }
        else
        {
            printf("   R%d \t0x%04X", (wm8990_reg_op[i]>>16), (wm8990_reg_op[i]&0x0000FFFF));
        }

        if((wm8990_reg_op[i]&0xFFFF0000) == WM8990_OP_FUNCTION)
        {
            printf("            Write     0x34       * %s\r\n", wm8990_op_func_name[(((wm8990_reg_op[i]&0x000000FF)-(unsigned int)WM8990_OP_START)-1)]);
        }
        else
        {
            printf(" SMbus_16_bit_data  Write     0x34       * %s\r\n", wm8990_reg_name[(wm8990_reg_op[i]>>16)]);
        }
        /*NU_Sleep(1);*/
    }
    printf("* End of file *\r\n");

    return 0;
}

int wm8990reg_op_seq_clr(void)
{
    reg_op_idx = 0;
    printf("\r\nclear all the register operation sequence\r\n");
    return 0;
}

#endif


/*modified==1,only display register which modified */
int wm8990reg_dis(int modified, int Hexmode)
{
    int i;
    int status;
    unsigned short data;

    printf("\r\n");
    printf("* ----- ------ -------------------- ------- --------- ------------------------------\r\n");
    printf("*  REG   DATA         ACCESS        READ OR  DEVICE\r\n");
    printf("* INDEX  VALUE         TYPE          WRITE   ADDRESS  COMMENT (for information only)\r\n");
    printf("* ----- ------ -------------------- ------- --------- ------------------------------\r\n");
    for(i=0; i<=0x3E; i++)
    {
        /*3 invalid register */
        if((i==0x11)||(i==0x24)||(i==0x3B))
        {
            continue;
        }
        /*only display register modified */
        if((1==modified)&&(wm8990_reg_flg[i]==0))
        {
            continue;
        }

        status = wm8990_reg_read((unsigned char)i,&data);
        if(0 != status)
        {
            return -1;
        }
        if(Hexmode)
        {
            printf("  0x%02X \t0x%04X",i,data);
        }
        else
        {
            printf("   R%d \t0x%04X",i,data);
        }

        printf(" SMbus_16_bit_data  Write     0x34       * %s\r\n", wm8990_reg_name[i]);
        /*NU_Sleep(1);*/
    }
    printf("* End of file *\r\n");

    return 0;
}

void wm8990reg_help(void)
{
    printf("\r\n");
    printf(" --S : wm8990---------------------------------------\r\n");
    printf("\twm8990reg command parameter explain as below:\r\n");
    printf("  00h:\tdisplay all valid register value\r\n");
    printf("  10h:\tdisplay all valid register value(HEX)\r\n");
    printf("  01h:\tdisplay all modified register value\r\n");
    printf("  11h:\tdisplay all modified register value(HEX)\r\n");
#ifdef WM8990_OPERATE_STAT
    printf("  02h:\tdisplay register operation sequence\r\n");
    printf("  12h:\tdisplay register operation sequence(HEX)\r\n");
    printf("  03h:\tclear   register operation sequence\r\n");
#endif
    printf(" --E : wm8990---------------------------------------\r\n");
    printf("\r\n");
}

int wm8990reg(int choice)
{
    int ret=0;

    switch (choice)
    {
        case 0x00:
        {
            ret = wm8990reg_dis(0, 0);
            break;
        }
        case 0x01:
        {
            ret = wm8990reg_dis(1, 0);
            break;
        }
        case 0x10:
        {
            ret = wm8990reg_dis(0, 1);
            break;
        }
        case 0x11:
        {
            ret = wm8990reg_dis(1, 1);
            break;
        }
#ifdef WM8990_OPERATE_STAT
        case 0x02:
        {
            ret = wm8990reg_op_seq(0);
            break;
        }
        case 0x12:
        {
            ret = wm8990reg_op_seq(1);
            break;
        }
        case 3:
        {
            ret = wm8990reg_op_seq_clr();
            break;
        }
#endif
        default:
        {
            printf("\r\ninput wrong parameter!\r\n");
            wm8990reg_help();
            break;
        }
    }

    return ret;
}


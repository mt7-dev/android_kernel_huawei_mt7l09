

#ifndef _WM8990REG_H_
#define _WM8990REG_H_

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

/*register address */
#define WM8990_R_RESET          0x00 /*reset & chip id */
#define WM8990_R_PWR_1          0x01 /*power management 1 */
#define WM8990_R_PWR_2          0x02 /* */
#define WM8990_R_PWR_3          0x03 /* */
#define WM8990_R_INTERFACE_1    0x04 /*audio interface 1 */
#define WM8990_R_INTERFACE_2    0x05 /*audio interface 2 */
#define WM8990_R_CLOCK_1        0x06
#define WM8990_R_CLOCK_2        0x07
#define WM8990_R_INTERFACE_3    0x08
#define WM8990_R_INTERFACE_4    0x09
#define WM8990_R_DAC_CTL        0x0A
#define WM8990_R_DACL_VOL_D     0x0B /*left DAC Digital Volume */
#define WM8990_R_DACR_VOL_D     0x0C /*right DAC Digital Volume */
#define WM8990_R_DIG_ST         0x0D /*Digital sidetone */
#define WM8990_R_ADC_CTL        0x0E 
#define WM8990_R_ADCL_VOL_D     0x0F /*Left ADC Digital Volume */
#define WM8990_R_ADCR_VOL_D     0x10 /*Right ADC Digital Volume */

#define WM8990_R_GPIO_CTL_1     0x12
#define WM8990_R_GPIO_1         0x13
#define WM8990_R_GPIO_34        0x14
#define WM8990_R_GPIO_5         0x15
#define WM8990_R_GPI_78         0x16
#define WM8990_R_GPIO_CTL_2     0x17
#define WM8990_R_LIN12_VOL      0x18
#define WM8990_R_LIN34_VOL      0x19
#define WM8990_R_RIN12_VOL      0x1A
#define WM8990_R_RIN34_VOL      0x1B
#define WM8990_R_LHP_VOL        0x1C
#define WM8990_R_RHP_VOL        0x1D
#define WM8990_R_LO_VOL         0x1E
#define WM8990_R_OUT34_VOL      0x1F
#define WM8990_R_LOPGA_VOL      0x20
#define WM8990_R_ROPGA_VOL      0x21
#define WM8990_R_SPK_VOL        0x22
#define WM8990_R_CLASSD_1       0x23

#define WM8990_R_CLASSD_3       0x25
#define WM8990_R_CLASSD_4       0x26
#define WM8990_R_IN_MIX_1       0x27 /*AINLMUX,AINRMUX */
#define WM8990_R_IN_MIX_2       0x28 /*LIN1-4 --> PGA, RIN1-4 --> PGA */
#define WM8990_R_IN_MIX_3       0x29 /*LIN34 --> INMIXL, LIN12 --> PGA, LOMIX->INMIXL gain */
#define WM8990_R_IN_MIX_4       0x2A /*RIN34 --> INMIXR, RIN12 --> PGA, ROMIX->INMIXR gain */
#define WM8990_R_IN_MIX_5       0x2B /*gain: LIN2->INMIXL, RXVOICE->AINLMUX, LIN4->INMIXL */
#define WM8990_R_IN_MIX_6       0x2C /*gain: RIN2->INMIXR, RXVOICE->AINRMUX, RIN4->INMIXR */
#define WM8990_R_OUT_MIX_1      0x2D /*X-->LOMIX */
#define WM8990_R_OUT_MIX_2      0x2E /*X-->ROMIX */
#define WM8990_R_OUT_MIX_3      0x2F /*vol: LIN3->LOMIX, RIN12->LOMIX, LIN12->LOMIX */
#define WM8990_R_OUT_MIX_4      0x30 /*vol: RIN3->ROMIX, LIN12->ROMIX, RIN12->ROMIX */
#define WM8990_R_OUT_MIX_5      0x31 /*vol: RIN3->LOMIX, AINRMUX->LOMIX, AINLMUX->LOMIX */
#define WM8990_R_OUT_MIX_6      0x32 /*vol: LIN3->ROMIX, AINLMUX->ROMIX, AINRMUX->ROMIX */
#define WM8990_R_OUT34_MIX      0x33 /*LIN4/LOPGA --> OUT3MIX, RIN4/ROPGA --> OUT4MIX */
#define WM8990_R_LIN_MIX_1      0x34 /*X-->LONMIX, X-->LOPMIX */
#define WM8990_R_LIN_MIX_2      0x35 /*X-->RONMIX, X-->ROPMIX */
#define WM8990_R_SPK_MIX        0x36 /*X-->SPKMIX */
#define WM8990_R_ADD_CTL        0x37 /*VREF to Analogue Output Resistance */
#define WM8990_R_ANTI_POP1      0x38 /* */
#define WM8990_R_ANTI_POP2      0x39
#define WM8990_R_MICBIAS        0x3A /*MCDSCTH:set Detect Threshold; MDCTHR:Current Detect Threshold */
#define WM8990_R_PLL1           0x3C /*SDM, PRESCALE, PLLN */
#define WM8990_R_PLL2           0x3D /*PLLK[15:8] */
#define WM8990_R_PLL3           0x3E /*PLLK[7:0] */


#define B_SPK_ENA           0x1000 /*R=01h */
#define B_OUT3_ENA          0x0800 /*R=01h */
#define B_OUT4_ENA          0x0400 /*R=01h */
#define B_LOUT_ENA          0x0200 /*R=01h */
#define B_ROUT_ENA          0x0100 /*R=01h */
#define B_MICBIAS_ENA       0x0010 /*R=01h */
#define B_VMID_MODE         0x0006 /*R=01h */
#define B_VREF_ENA          0x0001 /*R=01h */

#define B_PLL_ENA           0x8000 /*R=02h */
#define B_TSHUT_ENA         0x4000 /*R=02h, Thermal sensor */
#define B_AINL_ENA          0x0200 /*R=02h, analog input */
#define B_AINR_ENA          0x0100 /*R=02h, analog input */
#define B_LIN34_ENA         0x0080 /*R=02h */
#define B_LIN12_ENA         0x0040 /*R=02h */
#define B_RIN34_ENA         0x0020 /*R=02h */
#define B_RIN12_ENA         0x0010 /*R=02h */
#define B_ADCL_ENA          0x0002 /*R=02h */
#define B_ADCR_ENA          0x0001 /*R=02h */

#define B_LON_ENA           0x2000 /*R=03h */
#define B_LOP_ENA           0x1000 /*R=03h */
#define B_RON_ENA           0x0800 /*R=03h */
#define B_ROP_ENA           0x0400 /*R=03h */
#define B_SPKPGA_ENA        0x0100 /*R=03h */
#define B_LOPGA_ENA         0x0080 /*R=03h */
#define B_ROPGA_ENA         0x0040 /*R=03h */
#define B_LOMIX_ENA         0x0020 /*R=03h */
#define B_ROMIX_ENA         0x0010 /*R=03h */
#define B_DACL_ENA          0x0002 /*R=03h */
#define B_DACR_ENA          0x0001 /*R=03h */

#define B_AIFADCL_SRC       0x8000 /*R=04h, */
#define B_AIFADCR_SRC       0x4000 /*R=04h, */
#define B_AIFADC_TDM        0x2000 /*R=04h,不使用 */
#define B_AIFADC_TDM_CHAN   0x1000 /*R=04h,不使用 */
#define B_AIF_BCLK_INV      0x0100 /*R=04h, */
#define B_AIF_LRCLK_INV     0x0080 /*R=04h, */
#define B_AIF_WL            0x0060 /*R=04h,Audio interface word length */
#define B_AIF_FMT           0x0018 /*R=04h,DSP I2S mode */

#define B_DACL_SRC          0x8000 /*R=05h, */
#define B_DACR_SRC          0x4000 /*R=05h, */
#define B_AIFDAC_TDM        0x2000 /*R=05h,不使用 */
#define B_AIFDAC_TDM_CHAN   0x1000 /*R=05h,不使用 */
#define B_DAC_BOOST         0x0C00 /*R=05h, 0dB/6dB/12dB/18dB */
#define B_DAC_COMP          0x0010
#define B_DAC_COMPMODE      0x0008
#define B_ADC_COMP          0x0004
#define B_ADC_COMPMODE      0x0002
#define B_LOOPBACK          0x0001 /*adc data-->dac data,可以用于检查codec的语音模式配置是否正确 */

/*时钟分频寄存器 */
#define B_TOCLK_RATE        0x8000 /*R=06h,volume updata timeout */
#define B_TOCLK_ENA         0x4000 /*R=06h,timeout enable */
#define B_OPCLKDIV          0x1E00 /*R=06h,GPIO Output Clock Divider */
#define B_DCLKDIV           0x01C0 /*R=06h, */
#define B_BCLK_DIV          0x001E /*R=06h, */

#define B_SYSCLK_SRC        0x4000 /*R=07h, */
#define B_CLK_FORCE         0x2000 /*R=07h, */
#define B_MCLK_DIV          0x1800 /*R=07h, */
#define B_MCLK_INV          0x0400 /*R=07h, */
#define B_ADC_CLKDIV        0x00E0 /*R=07h, */
#define B_DAC_CLKDIV        0x001C /*R=07h, */

#define B_AIF_MSTR1         0x8000 /*R=08h, */
#define B_AIF_MSTR2         0x4000 /*R=08h, */
#define B_AIF_SEL           0x2000 /*R=08h, */
#define B_ADCLRC_DIR        0x0800 /*R=08h, */
#define B_ADCLRC_RATE       0x07FF /*R=08h, */

#define B_ALRCGPIO1         0x8000 /*R=09h, */
#define B_AIF_TRIS          0x2000 /*R=09h, */
#define B_DACLRC_DIR        0x0800 /*R=09h, */
#define B_DACLRC_RATE       0x07FF /*R=09h, */

/*DAC control */
#define B_DAC_SDMCLK_RATE   0x1000 /*R=0Ah, */
#define B_AIF_LRCLKRATE     0x0400 /*R=0Ah, */
#define B_DAC_MONO          0x0200 /*R=0Ah, */
#define B_DAC_SB_FILT       0x0100 /*R=0Ah, */
#define B_DAC_MUTERATE      0x0080 /*R=0Ah, DAC soft mute, rate */
#define B_DAC_MUTEMODE      0x0040 /*R=0Ah, DAC soft mute, mode */
#define B_DEEMP             0x0030 /*R=0Ah, DAC De-Emphasis Control */
#define B_DAC_MUTE          0x0004 /*R=0Ah, */
#define B_DACL_DATINV       0x0002 /*R=0Ah, */
#define B_DACR_DATINV       0x0001 /*R=0Ah, */

/*Left DAC Digital Volume */
#define B_DAC_VU            0x0100 /*R=0bh, */
#define B_DACL_VOL          0x00FF /*R=0bh, */
/*Right DAC Digital Volume */
#define B_DACR_VOL          0x00FF /*R=0ch, */

/*Digital Sidetone */
#define B_ADCL_DAC_SVOL     0x1E00 /*R=0dh, */
#define B_ADCR_DAC_SVOL     0x01E0 /*R=0dh, */
#define B_ADC_TO_DACL       0x000C /*R=0dh, */
#define B_ADC_TO_DACR       0x0003 /*R=0dh, */

/*ADC control */
#define B_ADC_HPF_ENA       0x0100 /*R=0eh,High Pass Filter */
#define B_ADC_HPF_CUT       0x0060 /*R=0eh,High Pass Filter Cut-off Freq */
#define B_ADCL_DATINV       0x0002 /*R=0eh, */
#define B_ADCR_DATINV       0x0001 /*R=0eh, */

/*Left ADC Digital Volume */
#define B_ADC_VU            0x0100 /*R=0fh, */
#define B_ADCL_VOL          0x00FF /*R=0fh, */
/*Right ADC Digital Volume */
#define B_ADCR_VOL          0x00FF /*R=10h, */

/*LIN12 PGA Volume */
#define B_IPVU0             0x0100 /*R=18h, */
#define B_LI12MUTE          0x0080 /*R=18h, */
#define B_LI12ZC            0x0040 /*R=18h, LIN12 PGA Zero Cross Detector */
#define B_LIN12VOL          0x001F /*R=18h, */
/*LIN34 PGA Volume */
#define B_IPVU1             0x0100 /*R=19h, */
#define B_LI34MUTE          0x0080 /*R=19h, */
#define B_LI34ZC            0x0040 /*R=19h, */
#define B_LIN34VOL          0x001F /*R=19h, */
/*RIN12 PGA Volume */
#define B_IPVU2             0x0100 /*R=1Ah, */
#define B_RI12MUTE          0x0080 /*R=1Ah, */
#define B_RI12ZC            0x0040 /*R=1Ah, RIN12 PGA Zero Cross Detector */
#define B_RIN12VOL          0x001F /*R=1Ah, */
/*RIN34 PGA Volume */
#define B_IPVU3             0x0100 /*R=1Bh, */
#define B_RI34MUTE          0x0080 /*R=1Bh, */
#define B_RI34ZC            0x0040 /*R=1Bh, */
#define B_RIN34VOL          0x001F /*R=1Bh, */

/*Left hp Output Volume */
#define B_OPVU0             0x0100 /*R=1Ch, */
#define B_LOZC              0x0080 /*R=1Ch, */
#define B_LOUTVOL           0x007F /*R=1Ch, */

/*Right hp Output Volume */
#define B_OPVU1             0x0100 /*R=1Dh, */
#define B_ROZC              0x0080 /*R=1Dh, */
#define B_ROUTVOL           0x007F /*R=1Dh, */

/*Line Output Volume */
#define B_LONMUTE           0x0040 /*R=1Eh, */
#define B_LOPMUTE           0x0020 /*R=1Eh, */
#define B_LOATTN            0x0010 /*R=1Eh, */
#define B_RONMUTE           0x0004 /*R=1Eh, */
#define B_ROPMUTE           0x0002 /*R=1Eh, */
#define B_ROATTN            0x0001 /*R=1Eh, */

/*OUT34 Volume */
#define B_OUT3MUTE          0x0020 /*R=1Fh, */
#define B_OUT3ATTN          0x0010 /*R=1Fh, */
#define B_OUT4MUTE          0x0002 /*R=1Fh, */
#define B_OUT4ATTN          0x0001 /*R=1Fh, */

/*LOPGA Volume */
#define B_OPVU2             0x0100 /*R=20h, */
#define B_LOPGAZC           0x0080 /*R=20h, */
#define B_LOPGAVOL          0x007F /*R=20h, */

/*ROPGA Volume */
#define B_OPVU3             0x0100 /*R=21h, */
#define B_ROPGAZC           0x0080 /*R=21h, */
#define B_ROPGAVOL          0x007F /*R=21h, */

/*SPK Volume */
#define B_SPKATTN           0x0003 /*R=22h, */

#define B_CDMODE            0x0100 /*R=23h, */
#define B_DCGAIN            0x0038 /*R=25h, */
#define B_ACGAIN            0x0007 /*R=25h, */
#define B_SPKZC             0x0080 /*R=26h, */
#define B_SPKVOL            0x007F /*R=26h, */

#define B_AINLMODE          0x000C /*R=27h, */
#define B_AINRMODE          0x0003 /*R=27h, */

/*Input Mixer */
#define B_LMP4              0x0080 /*R=28h, */
#define B_LMN3              0x0040 /*R=28h, */
#define B_LMP2              0x0020 /*R=28h, */
#define B_LMN1              0x0010 /*R=28h, */
#define B_RMP4              0x0008 /*R=28h, */
#define B_RMN3              0x0004 /*R=28h, */
#define B_RMP2              0x0002 /*R=28h, */
#define B_RMN1              0x0001 /*R=28h, */

#define B_L34MNB            0x0100 /*R=29h, */
#define B_L34MNBST          0x0080 /*R=29h, */
#define B_L12MNB            0x0020 /*R=29h, */
#define B_L12MNBST          0x0010 /*R=29h, */
#define B_LDBVOL            0x0007 /*R=29h, */

#define B_R34MNB            0x0100 /*R=2Ah, */
#define B_R34MNBST          0x0080 /*R=2Ah, */
#define B_R12MNB            0x0020 /*R=2Ah, */
#define B_R12MNBST          0x0010 /*R=2Ah, */
#define B_RDBVOL            0x0007 /*R=2Ah, */

/*Output Mixer */
#define B_LRBLO             0x0080 /*R=2Dh, */
#define B_LLBLO             0x0040 /*R=2Dh, */
#define B_LRI3LO            0x0020 /*R=2Dh, */
#define B_LLI3LO            0x0010 /*R=2Dh, */
#define B_LR12LO            0x0008 /*R=2Dh, */
#define B_LL12LO            0x0004 /*R=2Dh, */
#define B_LDLO              0x0001 /*R=2Dh, */
#define B_RDRO              0x0001 /*R=2Eh, */

/*OUT34 Mixer */
#define B_VSEL              0x0180 /*R=33h, */
#define B_LI4O3             0x0020 /*R=33h, */
#define B_LPGAO3            0x0010 /*R=33h, */
#define B_RI4O4             0x0002 /*R=33h, */
#define B_RPGAO4            0x0001 /*R=33h, */

/*LIne Output Mixer */
#define B_LLOPGALON         0x0040 /*R=34h, */
#define B_LROPGALON         0x0020 /*R=34h, */
#define B_LOPLON            0x0010 /*R=34h, */
#define B_LR12LOP           0x0004 /*R=34h, */
#define B_LL12LOP           0x0002 /*R=34h, */
#define B_LLOPGALOP         0x0001 /*R=34h, */

/*SPK Output Mixer */
#define B_LB2SPK            0x0080 /*R=36h, */
#define B_RB2SPK            0x0040 /*R=36h, */
#define B_LI2SPK            0x0020 /*R=36h, */
#define B_RI2SPK            0x0010 /*R=36h, */
#define B_LOPGASPK          0x0008 /*R=36h, */
#define B_ROPGASPK          0x0004 /*R=36h, */
#define B_LDSPK             0x0002 /*R=36h, */
#define B_RDSPK             0x0001 /*R=36h, */
#define B_VROI              0x0001 /*R=37h, */

#define B_DIS_LLINE         0x0020 /*R=38h, anti-pop1 */
#define B_DIS_RLINE         0x0010 /*R=38h, anti-pop1 */
#define B_DIS_OUT3          0x0008 /*R=38h, anti-pop1 */
#define B_DIS_OUT4          0x0004 /*R=38h, anti-pop1 */
#define B_DIS_LOUT          0x0002 /*R=38h, anti-pop1 */
#define B_DIS_ROUT          0x0001 /*R=38h, anti-pop1 */

#define B_SOFTST            0x0040 /*R=39h, VMID soft start */
#define B_BUFIOEN           0x0008 /*R=39h, VGS */
#define B_BUFCOPEN          0x0004 /*R=39h, */
#define B_POBCTRL           0x0002 /*R=39h, */
#define B_VMIDTOG           0x0001 /*R=39h, */

/*Microphone Bias */
#define B_MCDSCTH           0x00C0 /*R=3Ah, */
#define B_MDCTHR            0x0038 /*R=3Ah, */
#define B_MCD               0x0004 /*R=3Ah, */
#define B_MBSEL             0x0001 /*R=3Ah, */

/*PLL */
#define B_SDM               0x0080 /*R=3Ch, */
#define B_PRESCALE          0x0040 /*R=3Ch, */
#define B_PLLN              0x000F /*R=3Ch, */
#define B_PLLK_HIGH         0x00FF /*R=3Dh, */
#define B_PLLK_LOW          0x00FF /*R=3Eh, */

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif



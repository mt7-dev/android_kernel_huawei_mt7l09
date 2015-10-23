
#include "stdio.h"
#include <taskLib.h>
#include "drv_comm.h"
#include "bsp_wm8990.h"
#include "wm8990_plat.h"
#include "wm8990_reg.h"
#include "bsp_audio.h"

/*输入设备*/
static AUDIO_DEV_TYPE m_Indev=NODEV;
/*输出设备*/
static AUDIO_DEV_TYPE m_Outdev=NODEV;
/*工作场景*/
static AUDIO_SCENE m_audioApplication=AUD_SCENE_IDLE;

int delay1 = 0x02; /*p_up */
int delay2 = 0x03; /*p_up */
int delay3 = 0x08; /*indev */
int delay6 = 0x08; /*p_dn */
int delay7 = 0x0A; /*p_dn */
int delay8 = 0x00; /*p_dn */

void timedelay(int dly_ms)
{
    volatile int i=0;
    volatile int j=0;

    for(i=dly_ms;i>0;i--)
    {
        for(j=0x9000;j>0;j--)
        {
            ;
        }
    }
}

void change_delay(int i,int delay)
{
	switch(i)
	{
		case 1:
			delay1 = delay;
			break;
		case 2:
			delay2 = delay;
			break;
		case 3:
			delay3 = delay;
			break;
		case 6:
		    delay6 = delay;
		    break;
		case 7:
		    delay7 = delay;
		    break;
		case 8:
		    delay8 = delay;
		    break;
		default:
			break;
	}
}

/******************************************************************
*  Function:  wm8990_outdev_hp
*
*  Description:
*      Select the wm8990 output device(HEADPHONE)
*
*  Calls:
*
*  Called By:
*      wm8990_outdev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-select, 0-unselect
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_hp(int sel)
{
    int err=OK;

    /*select headphone output */
    if(sel)
    {
  		/*已经将该处的输出设备上电流程移入wm8990_outdev_power函数，
  		该处本应该添加输出PGA unmute过程，但这个过程在本函数
  		退出后在wm8990_outdev_sel 中完成了，因此该函数为空，留待
  		以后需要时再添加相应操作。*/
    }
    else /*unselect headphone output */
    {
    	/*已经将该处的输出设备下电流程移入wm8990_outdev_power函数，
  		该处只添加输出PGA mute过程.*/
        err |= wm8990_vol_down_set(VL_DN_LROUTVOL, 0x0);
        if(err)
        {
            codec_trace(HI_CODEC_ERR, "--unselect out HANDSET! FAIL\r\n");
        }
        else
        {
            codec_trace(HI_CODEC_INFO, "--unselect out HANDSET! SUCCESS\r\n");
        }
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_outdev_hs
*
*  Description:
*      Select the wm8990 output device(HEADSET)
*
*  Calls:
*
*  Called By:
*      wm8990_outdev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-select, 0-unselect
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_hs(int sel)
{
    int err=OK;

    /*select receiver output */
    if(sel)
    {/*OUT4, ROUT */
    	/*已经将该处的输出设备上电流程移入wm8990_outdev_power函数，
  		该处本应该添加输出PGA unmute过程，但这个过程在本函数
  		退出后在wm8990_outdev_sel 中完成了，因此该函数为空，留待
  		以后需要时再添加相应操作。*/
    }
    else /*unselect receiver output */
    {
    	/*已经将该处的输出设备下电流程移入wm8990_outdev_power函数，
  		该处只添加输出PGA mute过程.*/
		err |= wm8990_vol_down_set(VL_DN_ROUTVOL, 0x0);
    	err |= wm8990_vol_down_set(VL_DN_ROPGAVOL, 0x0);
    	err |= wm8990_vol_down_set(VL_DN_OUT3ATTN, 0x0);
    	err |= wm8990_reg_writeD(WM8990_R_OUT34_VOL, B_OUT3MUTE, 0xFFFF);

        if(err)
        {
            codec_trace( HI_CODEC_ERR, "--unselect out HANDSET! FAIL\r\n");
        }
        else
        {
            codec_trace( HI_CODEC_INFO, "--unselect out HANDSET! SUCCESS\r\n");
        }
    }

    return err;
}


/******************************************************************
*  Function:  wm8990_outdev_hf
*
*  Description:
*      Select the wm8990 output device(HANDFREE)
*
*  Calls:
*
*  Called By:
*      wm8990_outdev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-select, 0-unselect
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_hf(int sel)
{
    int err=OK;

    /*select SPK output */
    if(sel)
    {/*SPKN,SPKP */
    	/*已经将该处的输出设备上电流程移入wm8990_outdev_power函数，
  		该处本应该添加输出PGA unmute过程，但这个过程在本函数
  		退出后在wm8990_outdev_sel 中完成了，因此该函数为空，留待
  		以后需要时再添加相应操作。*/
    }
    else /*unselect SPK output */
    {
    	/*已经将该处的输出设备下电流程移入wm8990_outdev_power函数，
  		该处只添加输出PGA mute过程.*/
  		err |= wm8990_vol_down_set(VL_DN_LROPGAVOL, 0x0);
    	err |= wm8990_vol_down_set(VL_DN_SPKATTN, 0x3);
    	err |= wm8990_vol_down_set(VL_DN_SPKVOL, 0x0);

        if(err)
        {
            codec_trace( HI_CODEC_ERR, "--unselect out HANDFREE! FAIL\r\n");
        }
        else
        {
            codec_trace( HI_CODEC_INFO, "--unselect out HANDFREE! SUCCESS\r\n");
        }
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_outdev_hpf
*
*  Description:
*      Select the wm8990 output device(HANDFREE & HEADPHONE)
*
*  Calls:
*
*  Called By:
*      wm8990_outdev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-select, 0-unselect
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_hpf(int sel)
{
    int err=OK;

    /*select headphone output */
    if(sel)
    {/*LOUT, ROUT */
    	/*已经将该处的输出设备上电流程移入wm8990_outdev_power函数，
  		该处本应该添加输出PGA unmute过程，但这个过程在本函数
  		退出后在wm8990_outdev_sel 中完成了，因此该函数为空，留待
  		以后需要时再添加相应操作。*/
    }
    else /*unselect headphone output */
    {
    	/*已经将该处的输出设备下电流程移入wm8990_outdev_power函数，
  		该处只添加输出PGA mute过程.*/
  		err |= wm8990_vol_down_set(VL_DN_LROUTVOL, 0x0);
  		err |= wm8990_vol_down_set(VL_DN_LROPGAVOL, 0x0);
    	err |= wm8990_vol_down_set(VL_DN_SPKATTN, 0x3);
    	err |= wm8990_vol_down_set(VL_DN_SPKVOL, 0x0);

        if(err)
        {
            codec_trace( HI_CODEC_ERR, "--unselect out HEADSET! FAIL\r\n");
        }
        else
        {
            codec_trace( HI_CODEC_INFO, "--unselect out HEADSET! SUCCESS\r\n");
        }
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_outdev_hp_mute
*
*  Description:
*      mute output device(HEADPHONE)
*
*  Calls:
*
*  Called By:
*      wm8990_outdev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-Mute, 0-Unmute
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_hp_mute(int mute)
{
    int err=OK;

    /*select headphone output */
    if(mute)
    {/*LOUT, ROUT */
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_BUFIOEN, 0xFFFF);
        /*Output device disable */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_LOUT_ENA|B_ROUT_ENA, 0);
        codec_trace( HI_CODEC_INFO, "--Mute output device=HEADSET!\r\n");
    }
    else /*unselect headphone output */
    {
        /*Output device enable */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_LOUT_ENA|B_ROUT_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_BUFIOEN, 0);
        codec_trace( HI_CODEC_INFO, "--Unmute output device=HEADSET!\r\n");
    }

    return err;
}


/******************************************************************
*  Function:  wm8990_outdev_hs_mute
*
*  Description:
*      mute output device(HEADSET)
*
*  Calls:
*
*  Called By:
*      wm8990_outdev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-Mute, 0-Unmute
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_hs_mute(int mute)
{
    int err=OK;

    /*select headphone output */
    if(mute)
    {/*OUT3, OUT4 */
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_BUFIOEN, 0xFFFF);
        /*Output device disable */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_OUT3_ENA|B_OUT4_ENA, 0);
        codec_trace( HI_CODEC_INFO, "--Mute output device=HANDSET!\r\n");
    }
    else /*unselect headphone output */
    {
        /*Output device enable */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_OUT3_ENA|B_OUT4_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_BUFIOEN, 0);
        codec_trace( HI_CODEC_INFO, "--Unmute output device=HANDSET!\r\n");
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_outdev_hf_mute
*
*  Description:
*      mute output device(HANDFREE)
*
*  Calls:
*
*  Called By:
*      wm8990_outdev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-Mute, 0-Unmute
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_hf_mute(int mute)
{
    int err=OK;

    /*select headphone output */
    if(mute)
    {/*SPKN,SPKP */
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_BUFIOEN, 0xFFFF);
        /*Output device disable */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_SPK_ENA, 0);
        codec_trace( HI_CODEC_INFO, "--Mute output device=HANDFREE!\r\n");
    }
    else /*unselect headphone output */
    {
        /*Output device enable */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_SPK_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_BUFIOEN, 0);
        codec_trace( HI_CODEC_INFO, "--Unmute output device=HANDFREE!\r\n");
    }

    return err;
}

int wm8990_outdev_hpf_mute(int mute)
{
    int err=OK;

    err |= wm8990_outdev_hp_mute(1);
    err |= wm8990_outdev_hf_mute(1);

    return err;
}


/******************************************************************
*  Function:  wm8990_indev_hp
*
*  Description:
*      Select the wm8990 input device(HEADPHONE)
*
*  Calls:
*
*  Called By:
*      wm8990_indev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-select, 0-unselect
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_indev_hp(int sel)
{
    int err=OK;

    /*select external mic input */
    if(sel)
    {
        err |= wm8990_vol_up_set(VL_UP_ADCR_VOL, 0);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_2, B_RMN1|B_RMP2, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_RIN12_VOL, B_RI12MUTE, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_RIN12_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_AINR_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_4, B_R12MNB, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_1, B_AINRMODE, 0);
        /*ADCR_ENA */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_ADCR_ENA, 0xFFFF);
        timedelay(delay3);
        /*ADCR->Left Channel, ADCR->Right Channel */
        err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIFADCL_SRC, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIFADCR_SRC, 0xFFFF);

        err |= wm8990_vol_up_set(VL_UP_ADCR_VOL, 0xC0);

        codec_trace( HI_CODEC_INFO, "--select in device = HEADSET!\r\n");
    }
    else /*unselect external mic input */
    {/*RIN1,RIN2 */
        err |= wm8990_vol_up_set(VL_UP_ADCR_VOL, 0);
        err |= wm8990_reg_writeD(WM8990_R_RIN12_VOL, B_RI12MUTE, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_2, B_RMN1|B_RMP2, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_RIN12_ENA, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_AINR_ENA, 0);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_4, B_R12MNB, 0);

        /*ADCR_ENA */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_ADCR_ENA, 0);
        codec_trace( HI_CODEC_INFO, "--unselect in HEADSET!\r\n");
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_indev_hs
*
*  Description:
*      Select the wm8990 input device(HEADPHONE)
*
*  Calls:
*
*  Called By:
*      wm8990_indev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-select, 0-unselect
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_indev_hs(int sel)
{
    int err=OK;

    /*select internal input */
    if(sel)
    {/*LIN1,LIN2 */
        err |= wm8990_vol_up_set(VL_UP_ADCL_VOL, 0);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_2, B_LMN1|B_LMP2, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_LIN12_VOL, B_LI12MUTE, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_LIN12_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_AINL_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_3, B_L12MNB, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_1, B_AINLMODE, 0);
        /*ADCL_ENA */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_ADCL_ENA, 0xFFFF);
        timedelay(delay3);
        /*ADCL->Left Channel, ADCL->Right Channel */
        err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIFADCL_SRC, 0);
        err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIFADCR_SRC, 0);

        err |= wm8990_vol_up_set(VL_UP_ADCL_VOL, 0xC0);

        codec_trace( HI_CODEC_INFO, "--select in device = HANDSET!\r\n");
    }
    else /*unselect internal input */
    {/*LIN1,LIN2 */
        err |= wm8990_vol_up_set(VL_UP_ADCL_VOL, 0);

        err |= wm8990_reg_writeD(WM8990_R_LIN12_VOL, B_LI12MUTE, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_2, B_LMN1|B_LMP2, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_LIN12_ENA, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_AINL_ENA, 0);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_3, B_L12MNB, 0);

        /*ADCL_ENA */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_ADCL_ENA, 0);
        codec_trace( HI_CODEC_INFO, "--unselect in HANDSET!\r\n");
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_indev_hf
*
*  Description:
*      Select the wm8990 input device(HEADPHONE)
*
*  Calls:
*
*  Called By:
*      wm8990_indev_sel
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         1-select, 0-unselect
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_indev_hf(int sel)
{
    int err=OK;

    /*select internal input */
    if(sel)
    {/*LIN1,LIN2 */
        err |= wm8990_vol_up_set(VL_UP_ADCL_VOL, 0);

        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_2, B_LMN1|B_LMP2, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_LIN12_VOL, B_LI12MUTE, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_LIN12_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_AINL_ENA, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_3, B_L12MNB, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_1, B_AINLMODE, 0);
        /*ADCL_ENA */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_ADCL_ENA, 0xFFFF);
        timedelay(delay3);
        /*ADCL->Left Channel, ADCL->Right Channel */
        err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIFADCL_SRC, 0);
        err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIFADCR_SRC, 0);

        err |= wm8990_vol_up_set(VL_UP_ADCL_VOL, 0xC0);

        codec_trace( HI_CODEC_INFO, "--select in device = HANDFREE!\r\n");
    }
    else /*unselect internal input */
    {/*LIN1,LIN2 */
        err |= wm8990_vol_up_set(VL_UP_ADCL_VOL, 0);

        err |= wm8990_reg_writeD(WM8990_R_LIN12_VOL, B_LI12MUTE, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_2, B_LMN1|B_LMP2, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_LIN12_ENA, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_AINL_ENA, 0);
        err |= wm8990_reg_writeD(WM8990_R_IN_MIX_3, B_L12MNB, 0);

        /*ADCL_ENA */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_ADCL_ENA, 0);
        codec_trace( HI_CODEC_INFO, "--unselect in HANDFREE!\r\n");
    }

    return err;
}

/******************************************************************
*  Function:  wm8990SampleRateSet
*
*  Description:
*      Set the wm8990's DAC/ADC sample rates
*
*  Calls:
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         ulSampleRate: 48: Fs=48kHz . 32: Fs=32kHz . 24: Fs=24kHz .
*                       16: Fs=16kHz . 8: Fs=8kHz .
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_samplerate_set(HI6411_MODE mode, AUDIO_SAMPLE_RATE ulSampleRate)
{
	int err = OK;
	AUDIO_UINT16 index;
    AUDIO_UINT16 rate_matrix[][9]=
    {
    /*                  3Ch     3Dh     3Eh     06h4-1  07h7-5  07h4-2  08h     09h */
    /*                                          BCLK_DIV        DAC_CLKDIV      DACLRC_RATE */
    /*SAMPLE_RATE,      PLLN,   PLLK_H, PLLK_L          ADC_CLKDIV      ADCLRC_RATE */
    {SAMPLE_RATE_8K,    0x000A, 0x003D, 0x0070, 0x0006, 0x0006, 0x0006, 0x0100, 0x0100},
    {SAMPLE_RATE_11K025,0x0006, 0x00F2, 0x008B, 0x0004, 0x0004, 0x0004, 0x0100, 0x0100},
    {SAMPLE_RATE_12K,   0x0007, 0x008F, 0x00D5, 0x0004, 0x0004, 0x0004, 0x0100, 0x0100},
    {SAMPLE_RATE_16K,   0x000A, 0x003D, 0x0070, 0x0006, 0x0006, 0x0006, 0x0080, 0x0080},
    {SAMPLE_RATE_22K05, 0x0006, 0x00F2, 0x008B, 0x0004, 0x0002, 0x0002, 0x0080, 0x0080},
    {SAMPLE_RATE_24K,   0x0007, 0x008F, 0x00D5, 0x0004, 0x0002, 0x0002, 0x0080, 0x0080},
    {SAMPLE_RATE_32K,   0x0007, 0x008F, 0x00D5, 0x0003, 0x0001, 0x0001, 0x0080, 0x0080},
    {SAMPLE_RATE_44K1,  0x0006, 0x00F2, 0x008B, 0x0004, 0x0000, 0x0000, 0x0040, 0x0040},
    {SAMPLE_RATE_48K,   0x0007, 0x008F, 0x00D5, 0x0004, 0x0000, 0x0000, 0x0040, 0x0040},
    {SAMPLE_RATE_INVALID,0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}
    };


#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_SAMPLERATE);
#endif

    index=0;
    while(rate_matrix[index][0] != (AUDIO_UINT16)SAMPLE_RATE_INVALID)
    {
        if(ulSampleRate == rate_matrix[index][0])
        {
            break;
        }
        index++;
    }
    if(rate_matrix[index][0] == (AUDIO_UINT16)SAMPLE_RATE_INVALID)
    {
        codec_trace( HI_CODEC_ERR, "--wm8990_samplerate_set: invalid samplerate!\r\n");
        return ERROR;
    }

    err |= wm8990_reg_writeD(WM8990_R_PLL1, B_PLLN, rate_matrix[index][1]);
    err |= wm8990_reg_writeD(WM8990_R_PLL2, B_PLLK_HIGH, rate_matrix[index][2]);
    err |= wm8990_reg_writeD(WM8990_R_PLL3, B_PLLK_LOW, rate_matrix[index][3]);

    err |= wm8990_reg_writeD(WM8990_R_CLOCK_1, B_BCLK_DIV, (AUDIO_UINT16)(rate_matrix[index][4]<<1));

    err |= wm8990_reg_writeD(WM8990_R_CLOCK_2, B_ADC_CLKDIV, (AUDIO_UINT16)(rate_matrix[index][5]<<5));
    err |= wm8990_reg_writeD(WM8990_R_CLOCK_2, B_DAC_CLKDIV, (AUDIO_UINT16)(rate_matrix[index][6]<<2));

    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_3, B_ADCLRC_RATE, rate_matrix[index][7]);
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_4, B_DACLRC_RATE, rate_matrix[index][8]);


    return err;
}

/******************************************************************
*  Function:  wm8990_filter_up_set
*
*  Description:
*      Control the wm8990 uplink filter set
*
*  Calls:
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         usVol: 0xffff - 0x0000, gain from 0db to -96db
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_filter_up_set(FILTER filter)
{
    int err=OK;

    err |= wm8990_reg_writeD(WM8990_R_ADC_CTL, B_ADC_HPF_CUT,
                            filter.bits.adc_hpf_cut<<5);
    err |= wm8990_reg_writeD(WM8990_R_ADC_CTL, B_ADC_HPF_ENA,
                            filter.bits.adc_hpf_ena<<8);
    return err;
}

/******************************************************************
*  Function:  wm8990_filter_down_set
*
*  Description:
*      Control the wm8990 uplink filter set
*
*  Calls:
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         usVol: 0xffff - 0x0000, gain from 0db to -96db
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_filter_down_set(FILTER filter)
{
    int err=OK;

    err |= wm8990_reg_writeD(WM8990_R_DAC_CTL, B_DAC_SB_FILT,
                            filter.bits.dac_sb_filt<<8);
    return err;
}


/******************************************************************
*  Function:  wm8990_vol_up_set
*
*  Description:
*      Control the wm8990 uplink gain set
*
*  Calls:
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         usVol: 0x0000 - 0x001f, gain from +30db to -32db
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_vol_up_set(AUDIO_UP_AMPLIFIER amplifier, AUDIO_UINT16 usVolume)
{
    int err=OK;
    AUDIO_UINT16 usoldVolume;
    AUDIO_UINT16 step;
    AUDIO_UINT16 i;


    switch(amplifier)
    {
        case VL_UP_ADCL_VOL:
        {
            err |= wm8990_reg_read(WM8990_R_ADCL_VOL_D, &usoldVolume);
            usoldVolume &= B_ADCL_VOL;

            if(usoldVolume <= usVolume)
            {
                step = 1;
            }
            else
            {
                step = 0xFFFF;
            }
            for(i=usoldVolume; i!=usVolume; i=i+step)
            {
                err |= wm8990_reg_writeD(WM8990_R_ADCL_VOL_D, B_ADCL_VOL, i);
                err |= wm8990_reg_writeD(WM8990_R_ADCL_VOL_D, B_ADC_VU, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_ADCL_VOL_D, B_ADC_VU, 0);
            }
            err |= wm8990_reg_writeD(WM8990_R_ADCL_VOL_D, B_ADCL_VOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_ADCL_VOL_D, B_ADC_VU, 0xFFFF);
            break;
        }
        case VL_UP_ADCR_VOL:
        {
            err |= wm8990_reg_read(WM8990_R_ADCR_VOL_D, &usoldVolume);
            usoldVolume &= B_ADCR_VOL;

            if(usoldVolume <= usVolume)
            {
                step = 1;
            }
            else
            {
                step = 0xFFFF;
            }
            for(i=usoldVolume; i!=usVolume; i=i+step)
            {
                err |= wm8990_reg_writeD(WM8990_R_ADCR_VOL_D, B_ADCR_VOL, i);
                err |= wm8990_reg_writeD(WM8990_R_ADCR_VOL_D, B_ADC_VU, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_ADCR_VOL_D, B_ADC_VU, 0);
            }
            err |= wm8990_reg_writeD(WM8990_R_ADCR_VOL_D, B_ADCR_VOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_ADCR_VOL_D, B_ADC_VU, 0xFFFF);
            break;
        }
        case VL_UP_LIN12VOL:
        {
            err |= wm8990_reg_writeD(WM8990_R_LIN12_VOL, B_LIN12VOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_LIN12_VOL, B_IPVU0, 0xFFFF);
            break;
        }
        case VL_UP_LIN34VOL:
        {
            err |= wm8990_reg_writeD(WM8990_R_LIN34_VOL, B_LIN34VOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_LIN34_VOL, B_IPVU1, 0xFFFF);
            break;
        }
        case VL_UP_RIN12VOL:
        {
            err |= wm8990_reg_writeD(WM8990_R_RIN12_VOL, B_RIN12VOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_RIN12_VOL, B_IPVU2, 0xFFFF);
            break;
        }
        case VL_UP_RIN34VOL:
        {
            err |= wm8990_reg_writeD(WM8990_R_RIN34_VOL, B_RIN34VOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_RIN34_VOL, B_IPVU3, 0xFFFF);
            break;
        }
        case VL_UP_L12MNBST:
        {
            err |= wm8990_reg_writeD(WM8990_R_IN_MIX_3, B_L12MNBST, (AUDIO_UINT16)(usVolume<<4));
            break;
        }
        case VL_UP_L34MNBST:
        {
            err |= wm8990_reg_writeD(WM8990_R_IN_MIX_3, B_L34MNBST, (AUDIO_UINT16)(usVolume<<7));
            break;
        }
        case VL_UP_R12MNBST:
        {
            err |= wm8990_reg_writeD(WM8990_R_IN_MIX_4, B_R12MNBST, (AUDIO_UINT16)(usVolume<<4));
            break;
        }
        case VL_UP_R34MNBST:
        {
            err |= wm8990_reg_writeD(WM8990_R_IN_MIX_4, B_R34MNBST, (AUDIO_UINT16)(usVolume<<7));
            break;
        }
        default:
        {
            codec_trace( HI_CODEC_ERR,
                    "--wm8990_vol_up_set: not exist amplifier!\r\n");
        }
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_vol_up_dig_set
*
*  Description:
*      Control the wm8990 uplink gain(digital gain) set
*
*  Calls:
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         usVol: 0x0000 - 0x001f, gain from +30db to -32db
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_vol_up_dig_set(AUDIO_UINT16 usVol)
{
    int err=OK;

    err |= wm8990_vol_up_set(VL_UP_ADCL_VOL, usVol);
    err |= wm8990_vol_up_set(VL_UP_ADCR_VOL, usVol);

    return err;
}


/******************************************************************
*  Function:  wm8990_vol_down_set
*
*  Description:
*      Control the wm8990 downlink gain set
*
*  Calls:
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         usVol: 0x0000 - 0x001f, gain from +30db to -32db
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_vol_down_set(AUDIO_DOWN_AMPLIFIER amplifier, AUDIO_UINT16 usVolume)
{
    int err=OK;
    AUDIO_UINT16 usoldVolume;
    AUDIO_UINT16 step;
    AUDIO_UINT16 i;

    switch(amplifier)
    {
        case VL_DN_DACL_VOL:
        {
            err |= wm8990_reg_writeD(WM8990_R_DACL_VOL_D, B_DACL_VOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_DACL_VOL_D, B_DAC_VU, 0xFFFF);
            break;
        }
        case VL_DN_DACR_VOL:
        {
            err |= wm8990_reg_writeD(WM8990_R_DACR_VOL_D, B_DACR_VOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_DACR_VOL_D, B_DAC_VU, 0xFFFF);
            break;
        }
        case VL_DN_DAC_BOOST:
        {
            err |= wm8990_reg_writeD(WM8990_R_INTERFACE_2, B_DAC_BOOST, (AUDIO_UINT16)(usVolume<<10));
            break;
        }
        case VL_DN_LOPGAVOL:
        {
            err |= wm8990_reg_read(WM8990_R_LOPGA_VOL, &usoldVolume);
            usoldVolume &= B_LOPGAVOL;

            if(usoldVolume <= usVolume)
            {
                step = 1;
            }
            else
            {
                step = 0xFFFF;
            }
            for(i=usoldVolume; i!=usVolume; i=i+step)
            {
                err |= wm8990_reg_writeD(WM8990_R_LOPGA_VOL, B_LOPGAVOL, i);
                err |= wm8990_reg_writeD(WM8990_R_LOPGA_VOL, B_OPVU2, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_LOPGA_VOL, B_OPVU2, 0);
            }
            err |= wm8990_reg_writeD(WM8990_R_LOPGA_VOL, B_LOPGAVOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_LOPGA_VOL, B_OPVU2, 0xFFFF);
            break;
        }
        case VL_DN_ROPGAVOL:
        {
            err |= wm8990_reg_read(WM8990_R_ROPGA_VOL, &usoldVolume);
            usoldVolume &= B_ROPGAVOL;

            if(usoldVolume <= usVolume)
            {
                step = 1;
            }
            else
            {
                step = 0xFFFF;
            }
            for(i=usoldVolume; i!=usVolume; i=i+step)
            {
                err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_ROPGAVOL, i);
                err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_OPVU3, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_OPVU3, 0);
            }
            err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_ROPGAVOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_OPVU3, 0xFFFF);
            break;
        }
        case VL_DN_LROPGAVOL:
        {
            err |= wm8990_reg_read(WM8990_R_LOPGA_VOL, &usoldVolume);
            usoldVolume &= B_LOPGAVOL;

            if(usoldVolume <= usVolume)
            {
                step = 1;
            }
            else
            {
                step = 0xFFFF;
            }
            for(i=usoldVolume; i!=usVolume; i=i+step)
            {
                err |= wm8990_reg_writeD(WM8990_R_LOPGA_VOL, B_LOPGAVOL, i);
                err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_ROPGAVOL, i);
                err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_OPVU3, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_OPVU3, 0);
            }
            err |= wm8990_reg_writeD(WM8990_R_LOPGA_VOL, B_LOPGAVOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_ROPGAVOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_OPVU3, 0xFFFF);
            err |= wm8990_reg_writeD(WM8990_R_ROPGA_VOL, B_OPVU3, 0);
            break;
        }
        case VL_DN_LOUTVOL:
        {
            err |= wm8990_reg_read(WM8990_R_LHP_VOL, &usoldVolume);
            usoldVolume &= B_LOUTVOL;

            if(usoldVolume <= usVolume)
            {
                step = 1;
            }
            else
            {
                step = 0xFFFF;
            }
            for(i=usoldVolume; i!=usVolume; i=i+step)
            {
                err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_LOUTVOL, i);
                err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_OPVU0, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_OPVU0, 0);
            }
            err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_LOUTVOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_OPVU0, 0xFFFF);
            break;
        }
        case VL_DN_ROUTVOL:
        {
            err |= wm8990_reg_read(WM8990_R_RHP_VOL, &usoldVolume);
            usoldVolume &= B_ROUTVOL;

            if(usoldVolume <= usVolume)
            {
                step = 1;
            }
            else
            {
                step = 0xFFFF;
            }
            for(i=usoldVolume; i!=usVolume; i=i+step)
            {
                err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_ROUTVOL, i);
                err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_OPVU1, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_OPVU1, 0);
            }
            err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_ROUTVOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_OPVU1, 0xFFFF);
            err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_OPVU1, 0);
            break;
        }
        case VL_DN_LROUTVOL:
        {
            err |= wm8990_reg_read(WM8990_R_LHP_VOL, &usoldVolume);
            usoldVolume &= B_LOUTVOL;

            if(usoldVolume <= usVolume)
            {
                step = 1;
            }
            else
            {
                step = 0xFFFF;
            }
            for(i=usoldVolume; i!=usVolume; i=i+step)
            {
                err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_LOUTVOL, i);
                err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_OPVU0, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_OPVU0, 0);
                err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_ROUTVOL, i);
                err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_OPVU1, 0xFFFF);
                err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_OPVU1, 0);
            }
            err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_LOUTVOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_OPVU0, 0xFFFF);
            err |= wm8990_reg_writeD(WM8990_R_LHP_VOL, B_OPVU0, 0);
            err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_ROUTVOL, usVolume);
            err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_OPVU1, 0xFFFF);
            err |= wm8990_reg_writeD(WM8990_R_RHP_VOL, B_OPVU1, 0);
            break;
        }
        case VL_DN_OUT3ATTN:
        {
            err |= wm8990_reg_writeD(WM8990_R_OUT34_VOL, B_OUT3ATTN, (AUDIO_UINT16)(usVolume<<4));
            break;
        }
        case VL_DN_OUT4ATTN:
        {
            err |= wm8990_reg_writeD(WM8990_R_OUT34_VOL, B_OUT4ATTN, usVolume);
            break;
        }
        case VL_DN_SPKATTN:
        {
            err |= wm8990_reg_writeD(WM8990_R_SPK_VOL, B_SPKATTN, usVolume);
            break;
        }
        case VL_DN_SPKVOL:
        {
            err |= wm8990_reg_writeD(WM8990_R_CLASSD_4, B_SPKVOL, usVolume);
            break;
        }
        case VL_DN_DCGAIN:
        {
            err |= wm8990_reg_writeD(WM8990_R_CLASSD_3, B_DCGAIN, (AUDIO_UINT16)(usVolume<<7));
            break;
        }
        case VL_DN_ACGAIN:
        {
            err |= wm8990_reg_writeD(WM8990_R_CLASSD_3, B_ACGAIN, usVolume);
            break;
        }
        default:
        {
            codec_trace( HI_CODEC_ERR,
                    "--wm8990_vol_down_set: not exist amplifier!\r\n");
        }
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_vol_down_dig_set
*
*  Description:
*      Control the wm8990 downlink gain(digital gain) set
*
*  Calls:
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         usVol: 0x0000 - 0x001f, gain from +30db to -32db
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_vol_down_dig_set(AUDIO_UINT16 usVol)
{
    int err=OK;

    err |= wm8990_vol_down_set(VL_DN_DACL_VOL, usVol);
    err |= wm8990_vol_down_set(VL_DN_DACR_VOL, usVol);

    return err;
}

/******************************************************************
*  Function:  wm8990_vol_linein_set
*
*  Description:
*      Control the wm8990 linein channel(FM Rx) gain
*
*  Calls:
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         usVol: 0x0000 - 0x001f, gain from +30db to -32db
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_vol_linein_set(AUDIO_UINT16 usVolume)
{
    int err=OK;

    /*TODO: add code here */
    /*FM，单板上硬件尚未设计 */
    return err;
}

/******************************************************************
*  Function:  wm8990_outdev_sel
*
*  Description:
*      Select the wm8990 output device
*
*  Calls:
*      gpioSetDirection
*      gpioSetValue
*
*  Called By:
*      audio_ioctl, the function of audio driver module
*      HandFreeOut
*      HeadSetINOUT
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         eAudioDev:
*               HANDSET: Out to Earphone
*               HEADSET: Out to Receiver
*               HANDFREE: Out to Speaker
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_sel(AUDIO_DEV_TYPE eAudioDev)
{
    int err=OK;
    volatile AUDIO_UINT32 tick;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record( (WM8990_OP_FUNC)(((UINT32)WM8990_OP_OUT_SEL)|((UINT32)eAudioDev<<8)) );
#endif

    tick = (AUDIO_UINT32)tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_outdev_sel in=%d \r\n", (int)tick);

    switch(eAudioDev)
    {
        case HEADSET:
        {
            err |= wm8990_outdev_hp(1);
            break;
        }
        case HANDSET:
        {
            err |= wm8990_outdev_hs(1);
            break;
        }
        case HANDFREE:
        {
            err |= wm8990_outdev_hf(1);
            break;
        }
        case (AUDIO_DEV_TYPE)((int)HEADSET|(int)HANDFREE):
        {
            err |= wm8990_outdev_hpf(1);
            break;
        }
        case NODEV:
        {
            err |= ERROR;
            break;
        }
        default:
        {
            err = ERROR;
            break;
        }
    }

    if(err)
    {
        return err;
    }

    m_Outdev=eAudioDev;

    codec_trace( HI_CODEC_INFO, "--select out device = %d!\r\n", (int)m_Outdev);

    if(bEnAudioNV)
    {
        /*NV:根据NV项，设置默认增益*/
        if(((int)AUD_SCENE_VOICE)&((int)m_audioApplication))
        {
            if(((int)m_Outdev)&((int)HEADSET))
            {
                err |= wm8990_vol_down_set(VL_DN_LROUTVOL, (AUDIO_UINT16)struVHeadsetNV.down_lr_outvol);
                err |= wm8990_filter_down_set(struVHeadsetNV.filter);
            }

            if(((int)m_Outdev)&((int)HANDSET))
            {
                err |= wm8990_vol_down_set(VL_DN_ROUTVOL, (AUDIO_UINT16)struVHandsetNV.down_r_outvol);
                err |= wm8990_vol_down_set(VL_DN_ROPGAVOL,(AUDIO_UINT16)struVHandsetNV.down_r_opgavol);
                err |= wm8990_vol_down_set(VL_DN_OUT4ATTN,(AUDIO_UINT16)struVHandsetNV.down_out4attn);
                err |= wm8990_filter_down_set(struVHandsetNV.filter);
            }

            if(((int)m_Outdev)&((int)HANDFREE))
            {
                err |= wm8990_vol_down_set(VL_DN_LOPGAVOL,(AUDIO_UINT16)struVHandfreeNV.down_lr_opgavol);
                err |= wm8990_vol_down_set(VL_DN_ROPGAVOL,(AUDIO_UINT16)struVHandfreeNV.down_lr_opgavol);
                err |= wm8990_vol_down_set(VL_DN_SPKVOL,  (AUDIO_UINT16)struVHandfreeNV.down_spkvol);
                err |= wm8990_vol_down_set(VL_DN_SPKATTN, (AUDIO_UINT16)struVHandfreeNV.down_spkattn);
                err |= wm8990_vol_down_set(VL_DN_DCGAIN,  (AUDIO_UINT16)struVHandfreeNV.down_dcgain);
                err |= wm8990_vol_down_set(VL_DN_ACGAIN,  (AUDIO_UINT16)struVHandfreeNV.down_acgain);
                err |= wm8990_filter_down_set(struVHandfreeNV.filter);
            }
        }
        else
        {
            if(((int)m_Outdev)&((int)HEADSET))
            {
                err |= wm8990_vol_down_set(VL_DN_LROUTVOL, (AUDIO_UINT16)struMHeadsetNV.down_lr_outvol);
                err |= wm8990_filter_down_set(struMHeadsetNV.filter);
            }

            if(((int)m_Outdev)&((int)HANDSET))
            {
                err |= wm8990_vol_down_set(VL_DN_ROUTVOL, (AUDIO_UINT16)struMHandsetNV.down_r_outvol);
                err |= wm8990_vol_down_set(VL_DN_ROPGAVOL,(AUDIO_UINT16)struMHandsetNV.down_r_opgavol);
                err |= wm8990_vol_down_set(VL_DN_OUT4ATTN,(AUDIO_UINT16)struMHandsetNV.down_out4attn);
                err |= wm8990_filter_down_set(struMHandsetNV.filter);
            }

            if(((int)m_Outdev)&((int)HANDFREE))
            {
                err |= wm8990_vol_down_set(VL_DN_LOPGAVOL,(AUDIO_UINT16)struMHandfreeNV.down_lr_opgavol);
                err |= wm8990_vol_down_set(VL_DN_ROPGAVOL,(AUDIO_UINT16)struMHandfreeNV.down_lr_opgavol);
                err |= wm8990_vol_down_set(VL_DN_SPKVOL,  (AUDIO_UINT16)struMHandfreeNV.down_spkvol);
                err |= wm8990_vol_down_set(VL_DN_SPKATTN, (AUDIO_UINT16)struMHandfreeNV.down_spkattn);
                err |= wm8990_vol_down_set(VL_DN_DCGAIN,  (AUDIO_UINT16)struMHandfreeNV.down_dcgain);
                err |= wm8990_vol_down_set(VL_DN_ACGAIN,  (AUDIO_UINT16)struMHandfreeNV.down_acgain);
                err |= wm8990_filter_down_set(struMHandfreeNV.filter);
            }
        }
    }

    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_outdev_sel out=%d \r\n", (int)tick);

    return err;
}

/******************************************************************
*  Function:  wm8990_outdev_unsel
*
*  Description:
*     Unselect the wm8990 output device
*
*  Calls:
*      gpioSetDirection
*      gpioSetValue
*
*  Called By:
*      audio_ioctl, the function of audio driver module
*      HandFreeOut
*      HeadSetINOUT
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         eAudioDev:
*               HANDSET: Out to Earphone
*               HEADSET: Out to Receiver
*               HANDFREE: Out to Speaker
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_outdev_unsel(AUDIO_DEV_TYPE eAudioDev)
{
    int err=OK;
    volatile AUDIO_UINT32 tick;


#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record( (WM8990_OP_FUNC)(((int)WM8990_OP_OUT_UNSEL)|((int)eAudioDev<<8)) );
#endif

    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_outdev_unsel in=%d \r\n", (int)tick);

    switch(eAudioDev)
    {
        case HEADSET:
        {
            err |= wm8990_outdev_hp(0);
            break;
        }
        case HANDSET:
        {
            err |= wm8990_outdev_hs(0);
            break;
        }
        case HANDFREE:
        {
            err |= wm8990_outdev_hf(0);
            break;
        }
        case(AUDIO_DEV_TYPE)((int)HEADSET|(int)HANDFREE):
        {
        	err |= wm8990_outdev_hpf(0);
            break;
        }
        case NODEV:
        {
            break;
        }
        default:
        {
            err = ERROR;
            break;
        }
    }

    if(!err)
    {
        m_Outdev=NODEV;
    }

    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_outdev_unsel out=%d \r\n", (int)tick);

    return err;
}

/******************************************************************
*  Function:  wm8990_indev_sel
*
*  Description:
*      Select the wm8990 input device
*
*  Calls:
*
*  Called By:
*      audio_ioctl, the function of audio driver module
*      HandSetIn
*      HandSetOut
*      HeadSetINOUT
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         eAudioDev:
*                 HEADSET: Mic on Earphone .
*                 HANDSET: Mic on board.
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_indev_sel(AUDIO_DEV_TYPE eAudioDev)
{
    int err=OK;
    volatile AUDIO_UINT32 tick;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record( (WM8990_OP_FUNC)(((int)WM8990_OP_IN_SEL)|((int)eAudioDev<<8)) );
#endif

    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_indev_sel in=%d \r\n", (int)tick);

    switch(eAudioDev)
    {
        case HEADSET:
        {
            err = wm8990_indev_hp(1);
            break;
        }
        case HANDSET:
        {
            err = wm8990_indev_hs(1);
            break;
        }
        case HANDFREE:
        {
            err = wm8990_indev_hf(1);
            break;
        }
        case (AUDIO_DEV_TYPE)((int)HANDFREE|(int)HEADSET):
        {
            err = ERROR;
            break;
        }
        case NODEV:
        {
            err = ERROR;
            break;
        }
        default:
        {
            err = ERROR;
            break;
        }
    }

    if(err)
    {
        codec_trace( HI_CODEC_ERR,
                "--wm8990_indev_sel: invalid input device!\r\n");
        return err;
    }

    m_Indev = eAudioDev;
    codec_trace( HI_CODEC_INFO, "--select in device = %d!\r\n", (int)m_Indev);

    if(bEnAudioNV)
    {
        /*NV:根据NV项，设置默认增益*/
        if(((int)AUD_SCENE_VOICE)&((int)m_audioApplication))
        {
            if(HEADSET == m_Indev)
            {
                err |= wm8990_vol_up_set(VL_UP_RIN12VOL, (AUDIO_UINT16)struVHeadsetNV.up_rin12vol);
                err |= wm8990_vol_up_set(VL_UP_R12MNBST, (AUDIO_UINT16)struVHeadsetNV.up_r12mnbst);
                err |= wm8990_filter_up_set(struVHeadsetNV.filter);
            }

            if(HANDSET == m_Indev)
            {
                err |= wm8990_vol_up_set(VL_UP_LIN12VOL, (AUDIO_UINT16)struVHandsetNV.up_lin12vol);
                err |= wm8990_vol_up_set(VL_UP_L12MNBST, (AUDIO_UINT16)struVHandsetNV.up_l12mnbst);
                err |= wm8990_filter_up_set(struVHandsetNV.filter);
            }

            if(HANDFREE == m_Indev)
            {
                err |= wm8990_vol_up_set(VL_UP_LIN12VOL, (AUDIO_UINT16)struVHandfreeNV.up_lin12vol);
                err |= wm8990_vol_up_set(VL_UP_L12MNBST, (AUDIO_UINT16)struVHandfreeNV.up_l12mnbst);
                err |= wm8990_filter_up_set(struVHandfreeNV.filter);
            }
        }
        else
        {
            if(HEADSET == m_Indev)
            {
                err |= wm8990_vol_up_set(VL_UP_RIN12VOL, (AUDIO_UINT16)struMHeadsetNV.up_rin12vol);
                err |= wm8990_vol_up_set(VL_UP_R12MNBST, (AUDIO_UINT16)struMHeadsetNV.up_r12mnbst);
                err |= wm8990_filter_up_set(struMHeadsetNV.filter);
            }

            if(HANDSET == m_Indev)
            {
                err |= wm8990_vol_up_set(VL_UP_LIN12VOL, (AUDIO_UINT16)struMHandsetNV.up_lin12vol);
                err |= wm8990_vol_up_set(VL_UP_L12MNBST, (AUDIO_UINT16)struMHandsetNV.up_l12mnbst);
                err |= wm8990_filter_up_set(struMHandsetNV.filter);
            }

            if(HANDFREE == m_Indev)
            {
                err |= wm8990_vol_up_set(VL_UP_LIN12VOL, (AUDIO_UINT16)struMHandfreeNV.up_lin12vol);
                err |= wm8990_vol_up_set(VL_UP_L12MNBST, (AUDIO_UINT16)struMHandfreeNV.up_l12mnbst);
                err |= wm8990_filter_up_set(struMHandfreeNV.filter);
            }
        }
    }

    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_indev_sel out=%d \r\n", (int)tick);

    return err;
}

/******************************************************************
*  Function:  wm8990_indev_unsel
*
*  Description:
*      Select the wm8990 input device
*
*  Calls:
*
*  Called By:
*      audio_ioctl, the function of audio driver module
*      HandSetIn
*      HandSetOut
*      HeadSetINOUT
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         eAudioDev:
*                 HEADSET: Mic on Earphone .
*                 HANDSET: Mic on board.
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_indev_unsel(AUDIO_DEV_TYPE eAudioDev)
{
    int err=OK;
    volatile AUDIO_UINT32 tick;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record( (WM8990_OP_FUNC)(((int)WM8990_OP_IN_UNSEL)|((int)eAudioDev<<8)) );
#endif

    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_indev_unsel in=%d \r\n", (int)tick);

    switch(eAudioDev)
    {
        case HEADSET:
        {
            err = wm8990_indev_hp(0);
            break;
        }
        case HANDSET:
        {
            err = wm8990_indev_hs(0);
            break;
        }
        case HANDFREE:
        {
            err = wm8990_indev_hf(0);
            break;
        }
        case (AUDIO_DEV_TYPE)((int)HANDFREE|(int)HEADSET):
        {
            err = ERROR;
            break;
        }
        case NODEV:
        {
            err = ERROR;
            break;
        }
        default:
        {
            err = ERROR;
            break;
        }
    }

    if(err)
    {
        return err;
    }

    m_Indev = eAudioDev;


    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_indev_unsel out=%d \r\n", (int)tick);

    return err;
}

/******************************************************************
*  Function:  wm8990_is_exist
*
*  Description:
*      check wm8990 is exist or not
*
*  Calls:
*      wm8990_reg_read
*      wm8990_reg_write
*
*  Called By:
*      wm8990_Poweron
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE:
*
*  Output:
*         NONE
*
*  Return:
*         OK: 9851 is exist
*         ERROR: not exist
*
******************************************************************/
int wm8990_is_exist(void)
{
    int err=OK;
    AUDIO_UINT16 Regdata;

    /*reset chip */
    err |= wm8990_reg_write(WM8990_R_RESET, 0x0001);


    /*read chip id */
    err |= wm8990_reg_read(WM8990_R_RESET, &Regdata);

    if(0x8990 != Regdata)
    {
        codec_trace( HI_CODEC_ERR, "--Maybe WM8990 is not exist, or communicate failure!\r\n");
        err = ERROR;
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_init_pll
*
*  Description:
*      init wm8990 PLL register
*
*  Calls:
*      wm8990_reg_read
*      wm8990_reg_writeD
*
*  Called By:
*      wm8990_open
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE:
*
*  Output:
*         NONE
*
*  Return:
*         OK:
*         ERROR:
*
******************************************************************/
int wm8990_init_pll(void)
{
    int err=OK;

    /*PRESCALE=1 --> MCLK/2 ; SDM=1, Fractional mode */
    err |= wm8990_reg_writeD(WM8990_R_PLL1, B_PRESCALE|B_SDM, 0xFFFF);
    /*SYSCLK_SRC=1 --> PLL Out */
    err |= wm8990_reg_writeD(WM8990_R_CLOCK_2, B_SYSCLK_SRC, 0xFFFF);
    /*MCLK_DIV=2 */
    err |= wm8990_reg_writeD(WM8990_R_CLOCK_2, B_MCLK_DIV, (2<<11));
    /*ALRCGPIO1=1 */
    /*err |= wm8990_reg_writeD(WM8990_R_INTERFACE_4, B_ALRCGPIO1, 0xFFFF); */
    /*ADCLRC_DIR=1 */
    /*err |= wm8990_reg_writeD(WM8990_R_INTERFACE_3, B_ADCLRC_DIR, 0xFFFF); */

    return err;
}


/******************************************************************
*  Function:  wm8990_init_micbias
*
*  Description:
*      init wm8990 micbias
*
*  Calls:
*      wm8990_reg_read
*      wm8990_reg_writeD
*
*  Called By:
*      wm8990_open
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE:
*
*  Output:
*         NONE
*
*  Return:
*         OK:
*         ERROR:
*
******************************************************************/
int wm8990_init_micbias(void)
{
    int err=OK;

    /*Micbias Detect */
    /*120uA, 350uA, */
    err |= wm8990_reg_writeD(WM8990_R_MICBIAS, B_MCDSCTH, 1<<6);
    err |= wm8990_reg_writeD(WM8990_R_MICBIAS, B_MDCTHR, 1<<3);
    /*Detect enable */

    /*Micbias voltage 0.65*AVDD */
    err |= wm8990_reg_writeD(WM8990_R_MICBIAS, B_MBSEL, 0xFFFF);
    /*MICBIAS enable */
    err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_MICBIAS_ENA, 0xFFFF);

    return err;
}

/*TODO: add code here */
/*音量初始化代码(wm8990_init_vol)在最后发布的代码中 */
/*不是必须的，它将以NV操作的形式出现。 */
int wm8990_init_vol(void)
{
    int err=OK;

    /*Headphone output vol */
    err |= wm8990_vol_down_set(VL_DN_LROUTVOL, 0x0);

    err |= wm8990_vol_down_set(VL_DN_LROPGAVOL, 0x0);
    err |= wm8990_vol_down_set(VL_DN_OUT3ATTN, 0x0);
    err |= wm8990_vol_down_set(VL_DN_SPKATTN, 0x0);
    err |= wm8990_vol_down_set(VL_DN_SPKVOL, 0x0);

    err |= wm8990_vol_up_set(VL_UP_LIN12VOL, 0x0B);
    err |= wm8990_vol_up_set(VL_UP_RIN12VOL, 0x0B);

    return err;
}


/******************************************************************
*  Function:  wm8990_pcm_down
*
*  Description:
*      down link pcm mode
*
*  Calls:
*      wm8990_reg_read
*      wm8990_reg_writeD
*
*  Called By:
*      wm8990_open
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE:
*
*  Output:
*         NONE
*
*  Return:
*         OK:
*         ERROR:
*
******************************************************************/
int wm8990_pcm_down(void)
{
    int err=OK;


#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_PCM_DN);
#endif

    /* Left Chnl->DACL, Left Chnl->DACR */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_2, B_DACL_SRC, 0);
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_2, B_DACR_SRC, 0);

    /*DACR_DATINV */
    /*err |= wm8990_reg_writeD(WM8990_R_DAC_CTL, B_DACR_DATINV, 0xFFFF); */

    /*BCLK inv, LRCLK no-inv */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_BCLK_INV, 0xFFFF);
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_LRCLK_INV, 0);

    /*DSP mode */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_FMT, 3<<3);

    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_4, B_ALRCGPIO1, 0xFFFF);

    return err;
}

/******************************************************************
*  Function:  wm8990_pcm_down
*
*  Description:
*      up link pcm mode
*
*  Calls:
*      wm8990_reg_read
*      wm8990_reg_writeD
*
*  Called By:
*      wm8990_open
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE:
*
*  Output:
*         NONE
*
*  Return:
*         OK:
*         ERROR:
*
******************************************************************/
int wm8990_pcm_up(void)
{
    int err=OK;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_PCM_UP);
#endif

    /*BCLK inv, LRCLK no-inv */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_BCLK_INV, 0xFFFF);
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_LRCLK_INV, 0);

    /*DSP mode */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_FMT, 3<<3);

    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_4, B_ALRCGPIO1, 0xFFFF);

    return err;
}


/******************************************************************
*  Function:  wm8990_i2s_down
*
*  Description:
*      down link i2s mode
*
*  Calls:
*      wm8990_reg_read
*      wm8990_reg_writeD
*
*  Called By:
*      wm8990_open
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE:
*
*  Output:
*         NONE
*
*  Return:
*         OK:
*         ERROR:
*
******************************************************************/
int wm8990_i2s_down(void)
{
    int err=OK;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_I2S_DN);
#endif

    /*Left Chnl->DACL, Right Chnl->DACR */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_2, B_DACL_SRC, 0);
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_2, B_DACR_SRC, 0xFFFF);

    /*DACR_DATINV */
    /*err |= wm8990_reg_writeD(WM8990_R_DAC_CTL, B_DACR_DATINV, 0); */

    /*BCLK inv, LRCLK no-inv */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_BCLK_INV, 0);
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_LRCLK_INV, 0);

    /*I2S mode */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_FMT, 2<<3);

    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_4, B_ALRCGPIO1, 0xFFFF);

    return err;
}

/******************************************************************
*  Function:  wm8990_i2s_up
*
*  Description:
*      up link i2s mode
*
*  Calls:
*      wm8990_reg_read
*      wm8990_reg_writeD
*
*  Called By:
*      wm8990_open
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE:
*
*  Output:
*         NONE
*
*  Return:
*         OK:
*         ERROR:
*
******************************************************************/
int wm8990_i2s_up(void)
{
    int err=OK;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_I2S_UP);
#endif

    /*BCLK inv, LRCLK no-inv */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_BCLK_INV, 0);
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_LRCLK_INV, 0);

    /*I2S mode */
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_FMT, 2<<3);
    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_4, B_ALRCGPIO1, 0xFFFF);

    err |= wm8990_reg_writeD(WM8990_R_INTERFACE_4, B_ALRCGPIO1, 0xFFFF);

    return err;
}

int wm8990_play_open(void)
{
    int err=OK;
    int application;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_PLAY_OPEN);
#endif


    application = (int)m_audioApplication;
    application |= AUD_SCENE_PLAY;
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_play_close(void)
{
    int err=OK;
    int application;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_PLAY_CLOSE);
#endif

    application = (int)m_audioApplication;
    application &= ~(AUD_SCENE_PLAY);
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_record_open(void)
{
    int err=OK;
    int application;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_RECORD_OPEN);
#endif

    application = (int)m_audioApplication;
    application |= AUD_SCENE_RECORD;
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_record_close(void)
{
    int err=OK;
    int application;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_RECORD_CLOSE);
#endif

    application = (int)m_audioApplication;
    application &= ~(AUD_SCENE_RECORD);
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_playrecord_open(void)
{
    int err=OK;
    int application;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_CB_OPEN);
#endif

    application = (int)m_audioApplication;
    application |= AUD_SCENE_CALLBACK;
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_playrecord_close(void)
{
    int err=OK;
    int application;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_CB_CLOSE);
#endif

    application = (int)m_audioApplication;
    application &= ~(AUD_SCENE_CALLBACK);
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_voice_open(void)
{
    int err=OK;
    int application;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_VOICE_OPEN);
#endif

    application = (int)m_audioApplication;
    application |= AUD_SCENE_VOICE;
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_voice_close(void)
{
    int err=OK;
    int application;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_VOICE_CLOSE);
#endif

    application = (int)m_audioApplication;
    application &= ~(AUD_SCENE_VOICE);
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

/*FM接收 */
int wm8990_fm_rx_open(void)
{
    int err=OK;
    int application;

    application = (int)m_audioApplication;
    application |= AUD_SCENE_FM_RX;
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_fm_rx_close(void)
{
    int err=OK;
    int application;

    application = (int)m_audioApplication;
    application &= ~(AUD_SCENE_FM_RX);
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

/*FM发射 */
int wm8990_fm_tx_open(void)
{
    int err=OK;
    int application;

    application = (int)m_audioApplication;
    application |= AUD_SCENE_FM_TX;
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_fm_tx_close(void)
{
    int err=OK;
    int application;

    application = (int)m_audioApplication;
    application &= ~(AUD_SCENE_FM_TX);
    m_audioApplication = (AUDIO_SCENE)application;

    return err;
}

int wm8990_lowpower(int choice)
{
    int err=OK;

    if(choice)
    {/*进入低功耗 */
        /*将Tshut关闭 */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_TSHUT_ENA, 0);

        /*TODO: add code here */
        /*MCLK关闭 */
    }
    else
    {/*退出低功耗 */
        /*TODO: add code here */
        /*MCLK打开 */

        /*将Tshut打开 */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_TSHUT_ENA, B_TSHUT_ENA);
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_probe
*
*  Description:
*      poweron codec
*
*  Calls:
*      wm8990_reg_read
*      wm8990_reg_writeD
*
*  Called By:
*      wm8990_open
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE:
*
*  Output:
*         NONE
*
*  Return:
*         OK:
*         ERROR:
*
******************************************************************/
int wm8990_probe(void)
{
    int err=OK;

    /*i2c初始化 */
    err = wm8990_i2c_init();

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_PROBE);
#endif

    /*判断CODEC是否就绪 */
    err |= wm8990_is_exist();
    if(err)
    {
        codec_trace( HI_CODEC_ERR, "--wm8990 codec is not exist.\r\n");
        return ERROR;
    }

    /*进入静态低功耗 */
    err |= wm8990_lowpower(1);

    return err;
}


int wm8990_outdev_power(int sel)
{
    int err=OK;


    /*select headphone output */
    if(sel)
    {/*LOUT, ROUT */
        err |= wm8990_init_pll();
        /*PLL */
        err |= wm8990_reg_writeD(WM8990_R_PWR_2, B_PLL_ENA, 0xFFFF);
        /*Master */
        err |= wm8990_reg_writeD(WM8990_R_INTERFACE_3, B_AIF_MSTR1, 0xFFFF);

        /*class AB */
        err |= wm8990_reg_writeD(WM8990_R_CLASSD_1,0x100,0x100);

        /*DAC soft mute mode */
        /*err |= wm8990_reg_writeD(WM8990_R_DAC_CTL, B_DAC_MUTEMODE, 0xFFFF); */
        err |= wm8990_reg_writeD(WM8990_R_DAC_CTL, B_DAC_MUTE, 0);
        /*Time out clock */
        err |= wm8990_reg_writeD(WM8990_R_CLOCK_1, B_TOCLK_ENA, 0xFFFF);
        /*Audio Interface 16bit */
        err |= wm8990_reg_writeD(WM8990_R_INTERFACE_1, B_AIF_WL, 0);
        /*Class D Clock */
        err |= wm8990_reg_writeD(WM8990_R_CLOCK_1, B_DCLKDIV, 0xFFFF);
        /*MIC Bias */
        err |= wm8990_init_micbias();

        /*Antipop2, */
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_SOFTST|B_BUFCOPEN|B_POBCTRL|B_VMIDTOG, 0xFFFF);

        /*disable vmid */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_VMID_MODE, 0);

        /*discharge */
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP1, B_DIS_LOUT|B_DIS_ROUT|B_DIS_OUT4, 0xFFFF);

        timedelay(delay1);

    	err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_VMIDTOG, 0xFFFF);

        /*soft_start enable */
        timedelay(delay2);

        /*TODO: add code here */
        /*音量初始化代码在最后发布的代码中不是必须的，它将以NV */
        /*操作的形式出现。 */
        err |= wm8990_init_vol();

        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_VMIDTOG, 0);
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP1, B_DIS_LOUT|B_DIS_ROUT|B_DIS_OUT4, 0);

		/*Output device enable */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_LOUT_ENA|B_ROUT_ENA|B_OUT4_ENA|B_SPK_ENA, 0xFFFF);

		err |= wm8990_reg_writeD(WM8990_R_PWR_3, B_DACL_ENA|B_DACR_ENA, 0xFFFF);
        /*LOMIX_ROMIX */
        err |= wm8990_reg_writeD(WM8990_R_PWR_3, B_LOMIX_ENA|B_ROMIX_ENA, 0xFFFF);

		err |= wm8990_reg_writeD(WM8990_R_PWR_3, B_SPKPGA_ENA, 0xFFFF);


        /*LOPGA_ROPGA */
        err |= wm8990_reg_writeD(WM8990_R_PWR_3, B_LOPGA_ENA|B_ROPGA_ENA, 0xFFFF);

        /*LOPGA->OUT3MIX */
        err |= wm8990_reg_writeD(WM8990_R_OUT34_MIX, B_RPGAO4, 0xFFFF);

        /*LOPGA->SPKMIX, ROPGA->SPKMIX */
        err |= wm8990_reg_writeD(WM8990_R_SPK_MIX, B_LOPGASPK|B_ROPGASPK, 0xFFFF);

        /*DACL->LOMIX, DACR->ROMIX */
        err |= wm8990_reg_writeD(WM8990_R_OUT_MIX_1, B_LDLO, 0xFFFF);
        err |= wm8990_reg_writeD(WM8990_R_OUT_MIX_2, B_RDRO, 0xFFFF);

        /*VMID_VREF enable */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_VMID_MODE , 0x0002);

        timedelay(delay3);

        err |= wm8990_reg_writeD(WM8990_R_PWR_1,  B_VREF_ENA, 0x0001);

        /*sel default bias, soft_start disable */
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_SOFTST|B_BUFCOPEN|B_POBCTRL, 0);
        if(err)
        {
            codec_trace( HI_CODEC_ERR, "--wm8990_outdev_pwrup = FAIL\r\n");
        }
        else
        {
            codec_trace( HI_CODEC_INFO, "--wm8990_outdev_pwrup = SUCCESS\r\n");
        }
    }
    else /*unselect headphone output */
    {/*LOUT, ROUT */
        /*mute spk */
        err |= wm8990_reg_writeD(WM8990_R_SPK_VOL, B_SPKATTN, 3);

        /*mute lrout */
        err |= wm8990_vol_down_set(VL_DN_LROUTVOL, 0);

        /*Antipop2, */
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_SOFTST|B_BUFCOPEN|B_POBCTRL|B_VMIDTOG, 0xFFFF);

        /*disable vmid */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_VMID_MODE, 0);

        timedelay(delay6);

        /*discharge */
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP1, B_DIS_LOUT|B_DIS_ROUT|B_DIS_OUT4, 0xFFFF);

        timedelay(delay7);

        /*mute dac */
        err |= wm8990_reg_writeD(WM8990_R_DAC_CTL, B_DAC_MUTEMODE|B_DAC_MUTE, 0xFFFF);

        /*disable lout/rout/out3/out4 */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1, B_LOUT_ENA|B_ROUT_ENA|B_OUT4_ENA|B_SPK_ENA, 0);

        /*DACL_DACR */
        err |= wm8990_reg_writeD(WM8990_R_PWR_3, B_DACL_ENA|B_DACR_ENA, 0);
        /*LOMIX_ROMIX */
        err |= wm8990_reg_writeD(WM8990_R_PWR_3, B_LOMIX_ENA|B_ROMIX_ENA, 0);
        err |= wm8990_reg_writeD(WM8990_R_PWR_3, B_SPKPGA_ENA, 0x0000);
         /*LOPGA_ROPGA */
        err |= wm8990_reg_writeD(WM8990_R_PWR_3, B_LOPGA_ENA|B_ROPGA_ENA, 0x0000);
        /*DACL->LOMIX, DACR->ROMIX */
        err |= wm8990_reg_writeD(WM8990_R_OUT_MIX_1, B_LDLO, 0);
        err |= wm8990_reg_writeD(WM8990_R_OUT_MIX_2, B_RDRO, 0);
        /*LOPGA->OUT3MIX */
        err |= wm8990_reg_writeD(WM8990_R_OUT34_MIX, B_RPGAO4, 0x0000);
         /*LOPGA->SPKMIX, ROPGA->SPKMIX */
        err |= wm8990_reg_writeD(WM8990_R_SPK_MIX, B_LOPGASPK|B_ROPGASPK, 0x0000);

        /*disable vref */
        err |= wm8990_reg_writeD(WM8990_R_PWR_1,  B_VREF_ENA, 0x0000);

        timedelay(delay8);

        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_POBCTRL, 0);
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_SOFTST|B_BUFCOPEN, 0);
        err |= wm8990_reg_writeD(WM8990_R_ANTI_POP2, B_VMIDTOG, 0x1);
        if(err)
        {
            codec_trace( HI_CODEC_ERR, "--wm8990_outdev_pwrdn = FAIL\r\n");
        }
        else
        {
            codec_trace( HI_CODEC_INFO, "--wm8990_outdev_pwrdn = SUCCESS\r\n");
        }
    }

    return err;
}

/******************************************************************
*  Function:  wm8990_open
*
*  Description:
*      Open the audio codec
*
*  Calls:
*      checkEarphone
*      wm8990InDevSelect
*      wm8990OutDevSelect
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_open(void)
{
    int err=OK;
    volatile AUDIO_UINT32 tick;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_OPEN);
#endif
    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_open in=%d \r\n", (int)tick);

    /*退出静态低功耗 */
    err |= wm8990_lowpower(0);

    /*判断CODEC是否就绪 */
    err |= wm8990_is_exist();
    if(err)
    {
        codec_trace( HI_CODEC_ERR, "--wm8990 codec is not exist.\r\n");
        return ERROR;
    }

    /*codec模拟部分上电 */
    err |= wm8990_outdev_power(1);
   
    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_open out=%d \r\n", (int)tick);

    return err;
}

/******************************************************************
*  Function:  wm8990_lose
*
*  Description:
*      Close the audio codec
*
*  Calls:
*      wm8990_reg_write
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         NONE
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_close(void)
{
    int err=OK;
    volatile AUDIO_UINT32 tick;

#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record(WM8990_OP_CLOSE);
#endif


    tick = tickGet();
    codec_trace(HI_CODEC_INFO, "Tick: wm8990_close in=%d \r\n", (int)tick);

    /*codec模拟部分下电 */
    err |= wm8990_outdev_power(0);

    /*reset chip */
    err |= wm8990_reg_write(WM8990_R_RESET, 0x0001);

    /*进入静态低功耗 */
    err |= wm8990_lowpower(1);

    /*TODO: add code here */
    /*关闭LDO10，现在这个后续会更改 */

    tick = tickGet();
    codec_trace( HI_CODEC_INFO, "Tick: wm8990_close out=%d \r\n", (int)tick);

    return err;
}

int wm8990_nv_write_up(AUDIO_NV_EFFECTIVE *Dbuff, AUDIO_DEV_TYPE dev)
{
    int err=OK;
    AUDIO_UINT16 uiReg;


    /*上行音量设置 */
    if(HEADSET == dev)
    {
        uiReg = (AUDIO_UINT16)(Dbuff->upvol.up_hp.rin12vol);
        err |= wm8990_vol_up_set(VL_UP_RIN12VOL, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->upvol.up_hp.r12mnbst);
        err |= wm8990_vol_up_set(VL_UP_R12MNBST, uiReg);
    }
    else if(HANDSET == dev)
    {
        uiReg = (AUDIO_UINT16)(Dbuff->upvol.up_hs.lin12vol);
        err |= wm8990_vol_up_set(VL_UP_LIN12VOL, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->upvol.up_hs.l12mnbst);
        err |= wm8990_vol_up_set(VL_UP_L12MNBST, uiReg);
    }
    else if(HANDFREE == dev)
    {
        uiReg = (AUDIO_UINT16)(Dbuff->upvol.up_hf.lin12vol);
        err |= wm8990_vol_up_set(VL_UP_LIN12VOL, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->upvol.up_hf.l12mnbst);
        err |= wm8990_vol_up_set(VL_UP_L12MNBST, uiReg);
    }
    else
    {
        codec_trace( HI_CODEC_ERR, "--wm8990_nv_write: updevice not appropriate\r\n");
        return ERROR;
    }

    /*上行滤波器值设置 */
    err |= wm8990_filter_up_set(Dbuff->filter);

    return err;
}

int wm8990_nv_write_down(AUDIO_NV_EFFECTIVE *Dbuff, AUDIO_DEV_TYPE dev)
{
    int err=OK;
    AUDIO_UINT16 uiReg;


    /*下行音量设置值 */
    if(HEADSET == dev)
    {
        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hp.lr_outvol);
        err |= wm8990_vol_down_set(VL_DN_LOUTVOL, uiReg);
        err |= wm8990_vol_down_set(VL_DN_ROUTVOL, uiReg);
    }
    else if(HANDSET == dev)
    {
        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hs.routvol);
        err |= wm8990_vol_down_set(VL_DN_ROUTVOL, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hs.ropgavol);
        err |= wm8990_vol_down_set(VL_DN_ROPGAVOL, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hs.out4attn);
        err |= wm8990_vol_down_set(VL_DN_OUT4ATTN, uiReg);
    }
    else if(HANDFREE == dev)
    {
        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hf.lr_opgavol);
        err |= wm8990_vol_down_set(VL_DN_LOPGAVOL, uiReg);
        err |= wm8990_vol_down_set(VL_DN_ROPGAVOL, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hf.spkattn);
        err |= wm8990_vol_down_set(VL_DN_SPKATTN, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hf.spkvol);
        err |= wm8990_vol_down_set(VL_DN_SPKVOL, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hf.dcgain);
        err |= wm8990_vol_down_set(VL_DN_DCGAIN, uiReg);

        uiReg = (AUDIO_UINT16)(Dbuff->downvol.dn_hf.acgain);
        err |= wm8990_vol_down_set(VL_DN_ACGAIN, uiReg);
    }
    else
    {
        codec_trace( HI_CODEC_ERR, "--wm8990_nv_write: downdevice not appropriate\r\n");
        return ERROR;
    }

    /*下行滤波器值设置 */
    err |= wm8990_filter_down_set(Dbuff->filter);

    return err;
}
/******************************************************************
*  Function:  wm8990NVWrite
*
*  Description:
*      NV设置立即生效
*
*  Calls:
*
*
*  Called By:
*      audioIoctl, the function of audio driver module
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         device:UINT32
*         up:    UINT32  |  line_in:  UINT32
*         down:  UINT32  |  line_out: UINT32
*         st:    UINT32  |  NULL:     UINT32
*         filter:UINT32  |  NULL:     UINT32
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_nv_write(AUDIO_NV_EFFECTIVE *Dbuff)
{
    int err=OK;


    if(m_audioApplication != Dbuff->scene)
    {
        codec_trace( HI_CODEC_ERR, "--wm8990_nv_write: scene not matching! \r\n");
        return ERROR;
    }

    if( (AUD_SCENE_VOICE == Dbuff->scene)||
        (AUD_SCENE_CALLBACK == Dbuff->scene) )
    {
        err |= wm8990_nv_write_up(Dbuff, (AUDIO_DEV_TYPE)Dbuff->indev);
        err |= wm8990_nv_write_down(Dbuff, (AUDIO_DEV_TYPE)Dbuff->outdev);
    }
    else if(AUD_SCENE_PLAY == Dbuff->scene)
    {
        err |= wm8990_nv_write_down(Dbuff, (AUDIO_DEV_TYPE)Dbuff->outdev);
    }
    else if(AUD_SCENE_RECORD == Dbuff->scene)
    {
        err |= wm8990_nv_write_up(Dbuff, (AUDIO_DEV_TYPE)Dbuff->indev);
    }
    else
    {
        Dbuff->scene = 0;
        Dbuff->indev = 0;
        Dbuff->outdev = 0;
        codec_trace(HI_CODEC_ERR, "--wm8990_nv_read: scene=0x%04x \r\n", (int)m_audioApplication);
        return err;
    }

    return err;
}

int wm8990_nv_read_up(AUDIO_NV_EFFECTIVE *Dbuff, AUDIO_DEV_TYPE dev)
{
    int err=OK;
    AUDIO_UINT16 uiReg;


    /*上行音量读取 */
    if(HEADSET == dev)
    {
        err |= wm8990_reg_read(WM8990_R_RIN12_VOL, &uiReg);
        uiReg &= B_RIN12VOL;
        Dbuff->upvol.up_hp.rin12vol = uiReg;

        err |= wm8990_reg_read(WM8990_R_IN_MIX_4, &uiReg);
        uiReg &= B_R12MNBST;
        uiReg >>= 4;
        Dbuff->upvol.up_hp.r12mnbst = uiReg;
    }
    else if(HANDSET == dev)
    {
        err |= wm8990_reg_read(WM8990_R_LIN12_VOL, &uiReg);
        uiReg &= B_LIN12VOL;
        Dbuff->upvol.up_hs.lin12vol = uiReg;

        err |= wm8990_reg_read(WM8990_R_IN_MIX_3, &uiReg);
        uiReg &= B_L12MNBST;
        uiReg >>= 4;
        Dbuff->upvol.up_hs.l12mnbst = uiReg;
    }
    else if(HANDFREE == dev)
    {
        err |= wm8990_reg_read(WM8990_R_LIN12_VOL, &uiReg);
        uiReg &= B_LIN12VOL;
        Dbuff->upvol.up_hf.lin12vol = uiReg;

        err |= wm8990_reg_read(WM8990_R_IN_MIX_3, &uiReg);
        uiReg &= B_L12MNBST;
        uiReg >>= 4;
        Dbuff->upvol.up_hf.l12mnbst = uiReg;
    }
    else
    {
        codec_trace( HI_CODEC_ERR, "--wm8990_nv_read: updevice not appropriate\r\n");
        return ERROR;
    }

    /*上行滤波器值 */
    err |= wm8990_reg_read(WM8990_R_ADC_CTL, &uiReg);
    uiReg &= B_ADC_HPF_ENA;
    uiReg >>= 8;
    Dbuff->filter.bits.adc_hpf_ena = uiReg;

    err |= wm8990_reg_read(WM8990_R_ADC_CTL, &uiReg);
    uiReg &= B_ADC_HPF_CUT;
    uiReg >>= 5;
    Dbuff->filter.bits.adc_hpf_cut = uiReg;


    return err;
}

int wm8990_nv_read_down(AUDIO_NV_EFFECTIVE *Dbuff, AUDIO_DEV_TYPE dev)
{
    int err=OK;
    AUDIO_UINT16 uiReg;


    /*下行音量设置值 */
    if(HEADSET == dev)
    {
        err |= wm8990_reg_read(WM8990_R_LHP_VOL, &uiReg);
        uiReg &= B_LOUTVOL;
        Dbuff->downvol.dn_hp.lr_outvol = uiReg;
    }
    else if(HANDSET == dev)
    {
        err |= wm8990_reg_read(WM8990_R_RHP_VOL, &uiReg);
        uiReg &= B_ROUTVOL;
        Dbuff->downvol.dn_hs.routvol  = uiReg;

        err |= wm8990_reg_read(WM8990_R_ROPGA_VOL, &uiReg);
        uiReg &= B_ROPGAVOL;
        Dbuff->downvol.dn_hs.ropgavol = uiReg;

        err |= wm8990_reg_read(WM8990_R_OUT34_VOL, &uiReg);
        uiReg &= B_OUT4ATTN;
        Dbuff->downvol.dn_hs.out4attn = uiReg;
    }
    else if(HANDFREE == dev)
    {
        err |= wm8990_reg_read(WM8990_R_LOPGA_VOL, &uiReg);
        uiReg &= B_LOPGAVOL;
        Dbuff->downvol.dn_hf.lr_opgavol=uiReg;

        err |= wm8990_reg_read(WM8990_R_SPK_VOL, &uiReg);
        uiReg &= B_SPKATTN;
        Dbuff->downvol.dn_hf.spkattn=uiReg;

        err |= wm8990_reg_read(WM8990_R_CLASSD_4, &uiReg);
        uiReg &= B_SPKVOL;
        Dbuff->downvol.dn_hf.spkvol=uiReg;

        err |= wm8990_reg_read(WM8990_R_CLASSD_3, &uiReg);
        uiReg &= B_DCGAIN;
        uiReg >>= 3;
        Dbuff->downvol.dn_hf.dcgain=uiReg;

        err |= wm8990_reg_read(WM8990_R_CLASSD_3, &uiReg);
        uiReg &= B_ACGAIN;
        Dbuff->downvol.dn_hf.acgain=uiReg;
    }
    else
    {
        codec_trace( HI_CODEC_ERR, "--wm8990_nv_read: updevice not appropriate\r\n");
        return ERROR;
    }

    /*下行滤波器值 */
    err |= wm8990_reg_read(WM8990_R_DAC_CTL, &uiReg);
    uiReg &= B_DAC_SB_FILT;
    uiReg >>= 8;
    Dbuff->filter.bits.dac_sb_filt = uiReg;


    return err;
}

/******************************************************************
*  Function:  wm8990NVRead
*
*  Description:
*      NV项对应的寄存器值立即读取
*
*  Called By:
*      audioIoctl, the function of audio driver module
*  Input:
*
*  Output:
*         NONE
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
int wm8990_nv_read(AUDIO_NV_EFFECTIVE *Dbuff)
{
    int err=OK;

    /*场景 */
    Dbuff->scene = (AUDIO_UINT32)m_audioApplication;

    /*输入设备 */
    if( (HEADSET == m_Indev)||
        (HANDSET == m_Indev)||
        (HANDFREE == m_Indev) )
    {
        Dbuff->indev = (AUDIO_UINT32)m_Indev;
    }
    else
    {
        Dbuff->indev = 0;
    }

    /*输出设备 */
    if( (HEADSET == m_Outdev)||
        (HANDSET == m_Outdev)||
        (HANDFREE == m_Outdev) )
    {
        Dbuff->outdev = (AUDIO_UINT32)m_Outdev;
    }
    else
    {
        Dbuff->outdev = 0;
    }

    if( (AUD_SCENE_VOICE == m_audioApplication)||
        (AUD_SCENE_CALLBACK == m_audioApplication) )
    {
        err |= wm8990_nv_read_up(Dbuff, (AUDIO_DEV_TYPE)Dbuff->indev);
        err |= wm8990_nv_read_down(Dbuff, (AUDIO_DEV_TYPE)Dbuff->outdev);
    }
    else if(AUD_SCENE_PLAY == m_audioApplication)
    {
        Dbuff->indev = 0;
        err |= wm8990_nv_read_down(Dbuff, (AUDIO_DEV_TYPE)Dbuff->outdev);
    }
    else if(AUD_SCENE_RECORD == m_audioApplication)
    {
        Dbuff->outdev = 0;
        err |= wm8990_nv_read_up(Dbuff, (AUDIO_DEV_TYPE)Dbuff->indev);
    }
    else
    {
        Dbuff->scene = 0;
        Dbuff->indev = 0;
        Dbuff->outdev = 0;
        codec_trace( HI_CODEC_WARN, "--wm8990_nv_read: scene=0x%04x \r\n", (int)m_audioApplication);
        /*return err; */
    }


    return err;
}


/*****************************************************************
 *  Function Name:
 *		wm8990_nv_op
 *
 *  Description:
 *		wm8990 CODEC NV立即生效
 *
 ******************************************************************/
int wm8990_nv_op(AUDIO_UINT32 Data)
{
    int err=OK;
    AUDIO_NV_EFFECTIVE *Dbuff=(AUDIO_NV_EFFECTIVE *)Data;


#ifdef WM8990_OPERATE_STAT
    wm8990_func_name_record( (WM8990_OP_FUNC)(((unsigned int)WM8990_OP_NV_RW)|(((unsigned int)Dbuff->nvcmd)<<8)) );
#endif

    if(1 == Dbuff->nvcmd)
    { /*NV读 */
        err = wm8990_nv_read(Dbuff);
    }
    else if(2 == Dbuff->nvcmd)
    { /*NV 写 */
        err = wm8990_nv_write(Dbuff);
    }
    else
    {
        err = ERROR;
    }

    return err;
}

/*****************************************************************
 *  Global Variable:
 *		wm8990_operation
 *
 *  Description:
 *		wm8990 CODEC设备的操作方法
 *
 ******************************************************************/
struct codec_operation wm8990_operation =
{
    wm8990_open,
    wm8990_close,
    wm8990_samplerate_set,
    wm8990_indev_sel,
    wm8990_indev_unsel,
    wm8990_outdev_sel,
    wm8990_outdev_unsel,
    wm8990_pcm_down,
    wm8990_pcm_up,
    wm8990_i2s_down,
    wm8990_i2s_up,
    wm8990_vol_up_dig_set,
    wm8990_vol_down_dig_set,
    wm8990_play_open,
    wm8990_play_close,
    wm8990_record_open,
    wm8990_record_close,
    wm8990_playrecord_open,
    wm8990_playrecord_close,
    wm8990_voice_open,
    wm8990_voice_close,
    wm8990_fm_rx_open,
    wm8990_fm_rx_close,
    wm8990_fm_tx_open,
    wm8990_fm_tx_close,
};




int wm8990_vol_path_down(AUDIO_DEV_TYPE eAudioDev)
{
    int err=OK;

    switch (eAudioDev)
    {
        case HEADSET:
        {
            printf("Left: DAC_BOOST -> DACL_VOL -> LOUTVOL\r\n");
            printf("Rigt: DAC_BOOST -> DACR_VOL -> ROUTVOL\r\n");
            printf("----------------------------------------------\r\n");
            printf("DAC_BOOST[1:0]:\twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DAC_BOOST));
            printf("DACL_VOL[7:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DACL_VOL));
            printf("DACR_VOL[7:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DACR_VOL));
            printf("LOUTVOL[6:0]:  \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_LOUTVOL));
            printf("ROUTVOL[6:0]:  \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_ROUTVOL));
            printf("----------------------------------------------\r\n");
            break;
        }
        case HANDSET:
        {
            printf("Left:           ->          ->          -> ROUTVOL\r\n");
            printf("Rigt: DAC_BOOST -> DACR_VOL -> ROPGAVOL -> OUT4ATTN\r\n");
            printf("----------------------------------------------\r\n");
            printf("DAC_BOOST[1:0]:\twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DAC_BOOST));
            printf("DACL_VOL[7:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DACL_VOL));
            printf("DACR_VOL[7:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DACR_VOL));
            printf("LOPGAVOL[6:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_LOPGAVOL));
            printf("ROPGAVOL[6:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_ROPGAVOL));
            printf("OUT3ATTN[0:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_OUT3ATTN));
            printf("OUT4ATTN[0:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_OUT4ATTN));
            printf("----------------------------------------------\r\n");
            break;
        }
        case HANDFREE:
        {
            printf("Left: DAC_BOOST -> DACL_VOL -> LOPGAVOL -> SPKATTN -> SPKVOL -> DAGAIN -> ACGAIN\r\n");
            printf("Rigt: DAC_BOOST -> DACR_VOL -> ROPGAVOL -> SPKATTN -> SPKVOL -> DAGAIN -> ACGAIN\r\n");
            printf("----------------------------------------------\r\n");
            printf("DAC_BOOST[1:0]:\twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DAC_BOOST));
            printf("DACL_VOL[7:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DACL_VOL));
            printf("DACR_VOL[7:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DACR_VOL));
            printf("LOPGAVOL[6:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_LOPGAVOL));
            printf("ROPGAVOL[6:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_ROPGAVOL));
            printf("SPKATTN[1:0]:\twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_SPKATTN));
            printf("SPKVOL[6:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_SPKVOL));
            printf("DCGAIN[2:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_DCGAIN));
            printf("ACGAIN[2:0]: \twm8990_vol_down_set %d  usVol\r\n", (int)(VL_DN_ACGAIN));
            printf("----------------------------------------------\r\n");
            break;
        }
        default:
        {
            printf("wrong parameter!\r\n");
            printf("the correct parameter is 1/2/3.\r\n");
            break;
        }
    }

    return err;
}

int wm8990_vol_path_up(AUDIO_DEV_TYPE eAudioDev)
{
    int err=OK;

    switch (eAudioDev)
    {
        case HEADSET:
        {
            printf("RIN12VOL -> R12MNBST -> ADCR_VOL\r\n");
            printf("----------------------------------------------\r\n");
            printf("RIN12VOL[4:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_RIN12VOL));
            printf("R12MNBST[0:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_R12MNBST));
            printf("ADCR_VOL[7:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_ADCR_VOL));
            printf("----------------------------------------------\r\n");
            break;
        }
        case HANDSET:
        {
            printf("LIN12VOL -> L12MNBST -> ADCL_VOL\r\n");
            printf("----------------------------------------------\r\n");
            printf("LIN12VOL[4:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_LIN12VOL));
            printf("L12MNBST[0:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_L12MNBST));
            printf("ADCL_VOL[7:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_ADCL_VOL));
            printf("----------------------------------------------\r\n");
            break;
        }
        case HANDFREE:
        {
            printf("LIN12VOL -> L12MNBST -> ADCL_VOL\r\n");
            printf("----------------------------------------------\r\n");
            printf("LIN12VOL[4:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_LIN12VOL));
            printf("L12MNBST[0:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_L12MNBST));
            printf("ADCL_VOL[7:0]: wm8990_vol_down_set %d  usVol\r\n", (int)(VL_UP_ADCL_VOL));
            printf("----------------------------------------------\r\n");
            break;
        }
		case NODEV:
		default:
			printf("NODEV \r\n");
			break;
			
    }

    return err;
}

void wm8990_help(void)
{
    printf("\r\n");
    printf(" --S : wm8990---------------------------------------\r\n");
    printf("  1. wm8990reg(int choice)\r\n");
    printf("  2. wm8990_vol_path_down(AUDIO_DEV_TYPE eAudioDev)\r\n");
    printf("  3. wm8990_vol_path_up(AUDIO_DEV_TYPE eAudioDev)\r\n");
    printf(" --E : wm8990---------------------------------------\r\n");
    printf("\r\n");
}

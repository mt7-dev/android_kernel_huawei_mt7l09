
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <logLib.h>
#include <semLib.h>
#include <bsp_dsp.h>
#include "bsp_wm8990.h"
#include "bsp_audio.h"

/*工作模式*/
HI6411_MODE g_hi6411Mode;

/*设备已经创建*/
static UINT32 bAudioCreated=0;
/*设备已经打开*/
static UINT32 bAudioOpened=0;

/*define audio_nv_read choice flag*/
UINT32 ulAudioNvReadFlag  = 0;

/*****************************************************************
*  Function:  audio_create
*  Description:  Create a audio device
*  Called By: BSP
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         devName: device name
*         arg: O_RDONLY (0), O_WRONLY (1), or O_RDWR (2)
*
*  Output:
*         None
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
STATUS audio_create(const char * devname, UINT32 arg)
{
    STATUS err=OK;

    if(!bAudioCreated)
    {
        err = wm8990_probe();
        if(err)
        {
            bAudioCreated=0;
            return err;
        }

        /*NV初始化 */
        err |= audio_nv_init();
        if(err)
        {
            codec_trace( HI_CODEC_ERR, "--audio NV intialize failure!\r\n");
            return ERROR;
        }

        bAudioCreated=1;
    }
    else
    {
        err = ERROR;
        codec_trace(HI_CODEC_INFO, "audio device is already created!\r\n");
    }

    return err;

}

/*****************************************************************
*  Function:  audio_open
*  Description:  Open a audio device
*
*  Calls:
*       errnoSet
*       semBCreate
*
*  Called By: multimedia software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         paudioDevHdr: pointer to audio device descriptor
*         name: name of the file to open
*         flags: access mode (O_RDONLY,O_WRONLY,O_RDWR)
*         mode: mode of file to create (UNIX chmod style)
*
*  Output:
*         None
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/

int audio_open(const char *devname, UINT32 flags, UINT32 mode)
{
    STATUS err=OK;

    if(!bAudioCreated)
    {
        /*还没有创建设备*/
        codec_trace(HI_CODEC_INFO, "audio device not create yet!\r\n");
        err = ERROR;
    }
    else
    {
        if(!bAudioOpened)
        {
            err |= wm8990_open();

            if(!err)
            {
                bAudioOpened = 1;
            }
            else
            {
                err = ERROR;
                codec_trace(HI_CODEC_ERR, "audio device open failure!\r\n");
            }
        }
        else
        {
            codec_trace(HI_CODEC_INFO, "audio device is already opened!\r\n");
            err = ERROR;
        }
    }

    return err;

}


/*****************************************************************
*  Function:  audio_read
*  Description:  Read from a audio file
*
*  Calls:
*       errnoSet
*       printf
*       sioDataRead
*
*  Called By: multimedia software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         audioDevId: file descriptor of file to read
*         pBuf: buffer to receive data
*         nBytes: number of bytes to read in to buffer
*
*  Output:
*         nBytes: the number of bytes read
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/

int audio_read(int devid, char * const pbuf, UINT32 nbytes)
{
    STATUS err=OK;


    if(NULL == pbuf)
    {
        return ERROR;
    }

    /*判断Audio是否已经打开*/
    if(!bAudioOpened)
    {
        codec_trace(HI_CODEC_ERR, "Audio device not open yet!\r\n");
        return ERROR;
    }

    if(ERROR == err)
    {
        codec_trace(HI_CODEC_ERR, "Data read error!\r\n");
    }
    else
    {
        err = (int)((AUDIO_PCM_DATA *)pbuf)->ulBufALen;
    }

    return err;

}

/*****************************************************************
*  Function:  audio_write
*  Description:  Write to a audio file
*
*  Calls:
*         errnoSet
*         printf
*         sioDataSend
*
*  Called By: multimedia software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         audioDevId: file descriptor of file to write
*         pBuf: buffer to be written
*         nBytes: reserved
*
*  Output:
*         None
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/

int audio_write(int devid, char * const pbuf, UINT32 nbytes)
{
    STATUS err=OK;

    if(NULL == pbuf)
    {
        return ERROR;
    }

    /* 判断Audio是否已经打开 */
    if(!bAudioOpened)
    {
        codec_trace(HI_CODEC_ERR, "Audio device not open yet!\r\n");
        return ERROR;
    }

    /* err = sioDataSend((AUDIO_PCM_DATA *)pbuf); */

    if(ERROR == err)
    {
        codec_trace(HI_CODEC_ERR, "audio write data failure! \r\n");
    }
    else
    {
        err = (int)((AUDIO_PCM_DATA *)pbuf)->ulBufALen;
    }

    return err;
}

/*******************************************************************************
  Function:     audio_play_open
  Description:  将codec配置成播放模式
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
STATUS audio_play_open(void)
{
    STATUS status=OK;

    /*初始化hi6411和sio*/
    if(I2S_MODE == g_hi6411Mode)
    {
        status |= wm8990_i2s_down();
    }
    else
    {
        status |= wm8990_pcm_down();
    }

    status |= wm8990_play_open();

    return status;
}

/*******************************************************************************
  Function:     audio_play_close
  Description:  停止codec播放
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
STATUS audio_play_close(void)
{
    STATUS status=OK;

	status |= wm8990_play_close();

	if(OK != status)
	{
	    codec_trace(HI_CODEC_ERR, "IOCTL_AUDIO_PLAY_CLOSE failure! \r\n");
	};

    /* 播放完毕再复位*/

    return status;
}

/*******************************************************************************
  Function:     audio_record_open
  Description:  开始codec录音
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
STATUS audio_record_open(void)
{
    STATUS status=OK;

    /*初始化hi6411和sio*/
    if(I2S_MODE == g_hi6411Mode)
    {
        status |= wm8990_i2s_up();
    }
    else
    {
        status |= wm8990_pcm_up();
    }

    status |= wm8990_record_open();

    return status;
}

/*******************************************************************************
  Function:     audio_record_close
  Description:  停止codec录音
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
STATUS audio_record_close(void)
{
    STATUS status=OK;

    status |= wm8990_record_close();


    if(OK !=status)
    {
        codec_trace(HI_CODEC_ERR, "IOCTL_AUDIO_RECORD_CLOSE failure! \r\n");
    }

    return status;
}

/*******************************************************************************
  Function:     audio_playrecord_open
  Description:  开始ARM侧语音环回
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
STATUS audio_playrecord_open(void)
{
    STATUS status=OK;

    /*初始化hi6411和sio*/
    if(I2S_MODE == g_hi6411Mode)
    {
        status |= wm8990_i2s_down();
        status |= wm8990_i2s_up();
    }
    else
    {
        status |= wm8990_pcm_down();
        status |= wm8990_pcm_up();
    }

    status |= wm8990_playrecord_open();

    return status;
}

/*******************************************************************************
  Function:     audio_playrecord_close
  Description:  停止ARM侧语音环回
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
STATUS audio_playrecord_close(void)
{
    STATUS status=OK;

    status |= wm8990_playrecord_close();

    return status;
}

/*******************************************************************************
  Function:     audio_voice_open
  Description:  开始Audio语音
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
STATUS audio_voice_open(void)
{
    STATUS status=OK;

    if(PCM_MODE ==g_hi6411Mode)
    {
        status |= wm8990_pcm_down();
        status |= wm8990_pcm_up();
    }
    else if(I2S_MODE ==g_hi6411Mode)
    {
        status |= wm8990_i2s_down();
        status |= wm8990_i2s_up();
    }

    status |= wm8990_voice_open();

    return status;
}

/*S:A32D14379,20081212*/
/*******************************************************************************
  Function:     audio_voice_close
  Description:  开始Audio语音
  Input:
  Output:       none
  Return：      OK:操作成功;
*******************************************************************************/
STATUS audio_voice_close(void)
{
    STATUS status=OK;

    status = wm8990_voice_close();

    return status;
}
/*E:A32D14379,20081212*/

/*****************************************************************
*  Function:  audio_ioctl
*  Description:
*          Do device specific control function
*
*  Calls:   wm8990SampleRateSet
*           sioReset
*           sioOutputOpen
*           wm8990Open
*           sioInputOpen
*           sioI2SInit
*           sioPCMInit
*           wm8990UpVolSet
*           wm8990DownVolSet
*           wm8990SideVolSet
*           wm8990InDevSelect
*           wm8990OutDevSelect
*
*  Called By: multimedia software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         audioDevId: descriptor to control
*         cmd: command
*         arg: some argument
*
*  Output:
*         None
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
STATUS audio_ioctl(int devid, UINT32 cmd, UINT32 arg)
{
    STATUS status = OK;

    /* 判断Audio是否已经打开 */
    if(!bAudioOpened)
    {
        codec_trace(HI_CODEC_ERR, "Audio device not open yet!\r\n");
        return ERROR;
    }

    codec_trace(HI_CODEC_DEBUG, "audio_ioctl %d! \r\n", cmd);

    switch(cmd)
    {
        case IOCTL_AUDIO_STATUS_GET:
        {
            status = OK;
            break;
        }
    	case IOCTL_AUDIO_SAMPLE_RATE_SET:
    	{
            status = wm8990_samplerate_set(g_hi6411Mode, (AUDIO_SAMPLE_RATE)arg);
            if( ERROR == status )
            {
                codec_trace(HI_CODEC_ERR, "IOCTL_AUDIO_SAMPLE_RATE_SET failure! \r\n");
            }
    	    break;
    	}
    	case IOCTL_AUDIO_PLAY_OPEN:
    	{

            status = audio_play_open();

    	    break;
    	}
    	case IOCTL_AUDIO_PLAY_CLOSE:
    	{

            status = audio_play_close();

    	    break;
    	}
    	case IOCTL_AUDIO_RECORD_OPEN:
    	{
            status = audio_record_open();

    	    break;
    	}
    	case IOCTL_AUDIO_RECORD_CLOSE:
    	{

            status = audio_record_close();
    	    break;
    	}
    	case IOCTL_AUDIO_I2S_MODE_SET:
    	{
    	    /*设置工作模式*/
    	    g_hi6411Mode = I2S_MODE;

            /*wm8990Open();*/
    	    break;

    	}
    	case IOCTL_AUDIO_PCM_MODE_SET:
    	{
    	    /*设置工作模式*/
    	    g_hi6411Mode = PCM_MODE;
    	    break;
    	}
    	case IOCTL_AUDIO_RX_VOL_SET:
    	{
            status = wm8990_vol_up_set(VL_UP_RIN12VOL, (UINT16)arg);
            status = wm8990_vol_up_set(VL_UP_R12MNBST, (UINT16)1);
    	    break;
    	}
    	case IOCTL_AUDIO_TX_VOL_SET:
    	{

            status = wm8990_vol_down_set(VL_DN_ROUTVOL,(UINT16)arg);
            status = wm8990_vol_down_set(VL_DN_ROPGAVOL,(UINT16)arg);
    	    break;
    	}
    	case IOCTL_AUDIO_SIDE_VOL_SET:
    	{

            /* status = wm8990_vol_st_dig_set((UINT16)arg);*/

    	    break;
    	}
    	case IOCTL_AUDIO_IN_DEV_SELECT:
    	{

            status = wm8990_indev_sel((AUDIO_DEV_TYPE)arg);

    	    break;

    	}
    	case IOCTL_AUDIO_OUT_DEV_SELECT:
    	{

            status =  wm8990_outdev_sel((AUDIO_DEV_TYPE)arg);
    	    break;
    	}
    	case IOCTL_AUDIO_IN_DEV_UNSELECT:
    	{

            status =  wm8990_indev_unsel((AUDIO_DEV_TYPE)arg);

    	    break;
    	}
    	case IOCTL_AUDIO_OUT_DEV_UNSELECT:
    	{

            status =  wm8990_outdev_unsel((AUDIO_DEV_TYPE)arg);

    	    break;
    	}
    	/*执行同步采集和播放*/
    	case IOCTL_AUDIO_READ_WRITE_SET:
    	{
            /* status = sioDataReadWrite((AUDIO_READ_WRITE_DATA *)arg); */

    	    break;
    	}
    	/*启动同步采集和播放*/
    	case IOCTL_AUDIO_VP_BEGIN:
    	{
    	    break;
    	}
    	/*结束同步采集和播放*/
    	case IOCTL_AUDIO_VP_END:
    	{
    	    break;
    	}
        /*新增打开关闭采集和播放的接口*/
        case IOCTL_AUDIO_PLAY_RECORD_OPEN:
    	{
            status = audio_playrecord_open();
    	    break;

    	}
    	case IOCTL_AUDIO_PLAY_RECORD_CLOSE:
    	{
            status = audio_playrecord_close();
    	    break;
    	}
        case IOCTL_AUDIO_VOICE_OPEN:
        {
            status = audio_voice_open();
            break;
        }
        case IOCTL_AUDIO_VOICE_CLOSE:
        {
            status |= audio_voice_close();
            break;
        }
        case IOCTL_AUDIO_BT_VOICE_OPEN:
        {
            /* status |= sioMuxSel(SIO_Z2BT); */
            break;
        }
        case IOCTL_AUDIO_BT_VOICE_CLOSE:
        {
            break;
        }
        case IOCTL_AUDIO_FM_RX_OPEN:
        {
            status |= wm8990_fm_rx_open();
            break;
        }
        case IOCTL_AUDIO_FM_RX_CLOSE:
        {
            status |= wm8990_fm_rx_close();
            break;
        }
        case IOCTL_AUDIO_FM_TX_OPEN:
        {
            status |= wm8990_fm_tx_open();
            break;
        }
        case IOCTL_AUDIO_FM_TX_CLOSE:
        {
            status |= wm8990_fm_tx_close();
            break;
        }
        case IOCTL_AUDIO_NV_OP:
        {
            status |= wm8990_nv_op(arg);
            break;
        }
        case IOCTL_AUDIO_SLEEP:
        {
            status = ERROR;
            break;
        }
        case IOCTL_AUDIO_DFS:
        {
            status = ERROR;
            break;
        }
    	default:
    	{
    	    status = ERROR;
			codec_trace(HI_CODEC_ERR, "parameter input is incorrect! \r\n");
    	}
    }

    if(OK != status)
    {
		codec_trace(HI_CODEC_ERR, "audio_ioctl: cmd=0x%x, arg=0x%x, status=0x%x\r\n", (long)cmd, (long)arg, status);
    }

    return (status);
}

/*****************************************************************
*  Function:  audio_close
*  Description:  Close a audio file
*
*  Calls:    errnoSet
*           semTake
*           sioInputClose
*           sioOutputClose
*           wm8990Close
*
*  Called By: multimedia software
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         audioDevId: file descriptor of file to close
*
*  Output:
*         None
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/

STATUS audio_close(int devid)
{
    STATUS err = OK;

    if(bAudioOpened)
    {
        err = wm8990_close();
        bAudioOpened = 0;
    }
    else
    {
        codec_trace(HI_CODEC_INFO, "Audio device not open yet!\r\n");
    }

    return err;
}

/*****************************************************************
*  Function:  audio_delete
*  Description:  Delete a audio file
*
*  Calls:    iosDevFind
*           iosDevDelete
*           free
*
*  Called By: BSP
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         devName: file descriptor of file to delete
*
*  Output:
*         None
*
*  Return:
*         OK: the function successful returned
*         ERROR: the function encounter error while running
*
******************************************************************/
STATUS audio_delete(const char *devname)
{
    STATUS err=OK;

    if(bAudioCreated)
    {
        bAudioCreated = 0;
    }
    else
    {
        printf("audio device not create yet!\r\n");
    }

	return err;
}

STATUS audioVoiceOpen(AUDIO_DEV_TYPE Indev, AUDIO_DEV_TYPE Outdev)
{
	STATUS err = OK;
	int fd = 0;

	err = audio_ioctl(fd,IOCTL_AUDIO_PCM_MODE_SET,0);
	if(OK != err)
	{
		audio_close(fd);
		fd = 0;
		printf("file:%s,line:%d\r\n",__FILE__,__LINE__);
		return ERROR;
	}

	/* Open the audio */
	err = audio_ioctl(fd,IOCTL_AUDIO_VOICE_OPEN,0);
	if(OK != err)
	{
		audio_close(fd);
		fd = 0;
		printf("file:%s,line:%d\r\n",__FILE__,__LINE__);
		return ERROR;
	};

	/* Select the in device */
	err = audio_ioctl(fd,IOCTL_AUDIO_IN_DEV_SELECT, Indev);
	if(OK != err)
	{
		audio_close(fd);
		fd = 0;
		printf("file:%s,line:%d\r\n",__FILE__,__LINE__);
		return ERROR;
	};

	/* Select the in device */
	err = audio_ioctl(fd,IOCTL_AUDIO_OUT_DEV_SELECT, Outdev);
	if(OK != err)
	{
		audio_close(fd);
		fd = 0;
		printf("file:%s,line:%d\r\n",__FILE__,__LINE__);
		return ERROR;
	};


	err = audio_ioctl(fd,IOCTL_AUDIO_SAMPLE_RATE_SET, 0);
	if(OK != err)
	{
		audio_close(fd);
		fd = 0;
		printf("file:%s,line:%d\r\n",__FILE__,__LINE__);
		return ERROR;
	};

    printf("\r\nvoice open success!\r\n");
	return OK;
}

STATUS audioVoiceClose(void)
{
	STATUS err = OK;
	int fd = 0;

	/* Close the audio */
	err = audio_ioctl(fd,IOCTL_AUDIO_VOICE_CLOSE,0);
	if(OK != err)
	{
		audio_close(fd);
		fd = 0;
		printf("file:%s,line:%d\r\n",__FILE__,__LINE__);
		return ERROR;
	};

	return err;
}
int codec_open(const char *devname, int flags, int mode)
{
    printf("codec_open devname %s flags 0x%x mode 0x%x\n",devname, flags, mode);

    return audio_open(devname, (UINT32)flags, (UINT32)mode);
}

int codec_close(int devid)
{
    return audio_close(devid);
}

int codec_ioctl(int devid, int cmd, int arg)
{
    printf("codec_ioctl devid 0x%x cmd 0x%x arg 0x%x\n",devid, cmd, arg);

    if (0x12345678 == ulAudioNvReadFlag)
    {
        if (OK != audio_nv_read())
        {
            logMsg("ERROR: audio_nv_read is fail!\n",1,2,3,4,5,6);
            return ERROR;
        }
    }
    switch(cmd)
    {
        case IOCTL_AUDIO_SAMPLE_RATE_SET:
        case IOCTL_AUDIO_PCM_MODE_SET:
        case IOCTL_AUDIO_IN_DEV_UNSELECT:

        case IOCTL_AUDIO_OUT_DEV_UNSELECT:
        case IOCTL_AUDIO_VOICE_OPEN:
        case IOCTL_AUDIO_VOICE_CLOSE:
            return audio_ioctl(devid, (UINT32)cmd, (UINT32)arg);

        case IOCTL_AUDIO_IN_DEV_SELECT:
            return audio_ioctl(devid, (UINT32)cmd, (UINT32)arg);
            /*return audio_ioctl(devid, IOCTL_AUDIO_RX_VOL_SET, RX_VOL_SET_ARG);*/
        case IOCTL_AUDIO_OUT_DEV_SELECT:
            return audio_ioctl(devid, (UINT32)cmd, (UINT32)arg);
            /*return audio_ioctl(devid, IOCTL_AUDIO_TX_VOL_SET, TX_VOL_SET_ARG);*/
        default:
            return ERROR;
    }
}

void audio_init(void)
{
    int ret;

    if (!bsp_dsp_is_hifi_exist())
        return;

    ret = audio_create("/dev/codec0", 0);
    if (BSP_OK != ret)
    {
        logMsg("\r audio_create  fail, result = 0x%x\n",ret,0,0,0,0,0);
    }
}


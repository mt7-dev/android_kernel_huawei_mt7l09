/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_AUDIO_H__
#define __DRV_AUDIO_H__

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

#include "drv_comm.h"

/*************************AUDIO START*********************************/

/*AUDIO IOCTL接口命令字*/
#define AUDIO_IOCTL_BASE					          (0x30000000)
#define IOCTL_AUDIO_STATUS_GET                     (AUDIO_IOCTL_BASE+0)
#define IOCTL_AUDIO_SAMPLE_RATE_SET                (AUDIO_IOCTL_BASE+1)
#define IOCTL_AUDIO_PLAY_OPEN                      (AUDIO_IOCTL_BASE+2)
#define IOCTL_AUDIO_PLAY_CLOSE                     (AUDIO_IOCTL_BASE+3)
#define IOCTL_AUDIO_RECORD_OPEN                    (AUDIO_IOCTL_BASE+4)
#define IOCTL_AUDIO_RECORD_CLOSE                   (AUDIO_IOCTL_BASE+5)
#define IOCTL_AUDIO_I2S_MODE_SET                   (AUDIO_IOCTL_BASE+6)
#define IOCTL_AUDIO_PCM_MODE_SET                   (AUDIO_IOCTL_BASE+7)
#define IOCTL_AUDIO_RX_VOL_SET                     (AUDIO_IOCTL_BASE+8)
#define	IOCTL_AUDIO_TX_VOL_SET                     (AUDIO_IOCTL_BASE+9)
#define	IOCTL_AUDIO_SIDE_VOL_SET                   (AUDIO_IOCTL_BASE+10)
#define	IOCTL_AUDIO_IN_DEV_SELECT                  (AUDIO_IOCTL_BASE+11)
#define	IOCTL_AUDIO_OUT_DEV_SELECT                 (AUDIO_IOCTL_BASE+12)
#define	IOCTL_AUDIO_OUT_DEV_UNSELECT               (AUDIO_IOCTL_BASE+13)
#define	IOCTL_AUDIO_IN_DEV_UNSELECT               (AUDIO_IOCTL_BASE+14)
/************************A32D07003 Modification begins by G57837******************************************/
/*执行同步采集和播放*/
#define	IOCTL_AUDIO_READ_WRITE_SET                 (AUDIO_IOCTL_BASE+15)
/*启动同步采集和播放*/
#define	IOCTL_AUDIO_VP_BEGIN                       (AUDIO_IOCTL_BASE+16)
/*结束同步采集和播放*/
#define	IOCTL_AUDIO_VP_END                         (AUDIO_IOCTL_BASE+17)
/************************A32D07003 Modification ends by G57837********************************************/
/************************A32D07003 Modification begins by G57837******************************************/
/*打开采集和播放*/
#define	IOCTL_AUDIO_PLAY_RECORD_OPEN               (AUDIO_IOCTL_BASE+20)
/*关闭采集和播放*/
#define	IOCTL_AUDIO_PLAY_RECORD_CLOSE              (AUDIO_IOCTL_BASE+21)

#define	IOCTL_AUDIO_VOICE_OPEN                     (AUDIO_IOCTL_BASE+23)
#define	IOCTL_AUDIO_VOICE_CLOSE                    (AUDIO_IOCTL_BASE+24)

#define	IOCTL_AUDIO_BT_VOICE_OPEN                  (AUDIO_IOCTL_BASE+25)
#define	IOCTL_AUDIO_BT_VOICE_CLOSE                 (AUDIO_IOCTL_BASE+26)
/* FM 接收 */
#define	IOCTL_AUDIO_FM_RX_OPEN                     (AUDIO_IOCTL_BASE+27)
#define	IOCTL_AUDIO_FM_RX_CLOSE                    (AUDIO_IOCTL_BASE+28)
/* FM 发射 */
#define	IOCTL_AUDIO_FM_TX_OPEN                     (AUDIO_IOCTL_BASE+29)
#define	IOCTL_AUDIO_FM_TX_CLOSE                    (AUDIO_IOCTL_BASE+30)

#define IOCTL_AUDIO_SLEEP                          (AUDIO_IOCTL_BASE+31)
#define IOCTL_AUDIO_DFS                            (AUDIO_IOCTL_BASE+32)

/*
#define	IOCTL_AUDIO_NV_WRITE                       (AUDIO_IOCTL_BASE+33)
#define	IOCTL_AUDIO_NV_READ                        (AUDIO_IOCTL_BASE+34)
*/
#define	IOCTL_AUDIO_NV_OP                          (AUDIO_IOCTL_BASE+67)

typedef enum
{
    NODEV=0x00,
    HEADSET=0x01,
    HANDSET=0x02,
    HANDFREE=0x04
}AUDIO_DEV_TYPE;

typedef enum
{
    SAMPLE_RATE_8K=0,
    SAMPLE_RATE_11K025=1,
    SAMPLE_RATE_12K=2,
    SAMPLE_RATE_16K=3,
    SAMPLE_RATE_22K05=4,
    SAMPLE_RATE_24K=5,
    SAMPLE_RATE_32K=6,
    SAMPLE_RATE_44K1=7,
    SAMPLE_RATE_48K=8,
    SAMPLE_RATE_INVALID=0xFFFF
}AUDIO_SAMPLE_RATE;


/********************************************************************************************************
 函 数 名  : BSP_AUDIO_CodecOpen
 功能描述  : 打开一个audio设备，并对codec进行初始化。
 输入参数  : devname：该参数固定为"/dev/codec0"。
                           flags：该参数表示读写方式，固定为0。
                           mode：该参数表示新建文件方式，固定为0。。
  输出参数  : 无。
 返 回 值  : 非负值:  设备句柄；
             -1：操作失败。
********************************************************************************************************/
int codec_open(const char *devname, int flags, int mode);
#define DRV_CODEC_OPEN(devname, flags, mode)    codec_open(devname, flags, mode)

/********************************************************************************************************
 函 数 名  : BSP_AUDIO_Codec_ioctl
 功能描述  : codec设备命令字控制。
 输入参数  : devid：设备句柄。
                           cmd：命令字。
                           arg：命令参数。
  输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
int codec_ioctl(int devid, int cmd, int arg);
#define DRV_CODEC_IOCTL(devid, cmd, arg)   codec_ioctl(devid, cmd, arg)

/*************************AUDIO END***********************************/
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif


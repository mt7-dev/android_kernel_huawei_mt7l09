#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <logLib.h>
#include <semLib.h>
#include "bsp_wm8990.h"
#include "bsp_audio.h"

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

int codec_open(const char *devname, int flags, int mode)
{
    return 0;
}

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
int codec_ioctl(int devid, int cmd, int arg)
{
    return OK;
}


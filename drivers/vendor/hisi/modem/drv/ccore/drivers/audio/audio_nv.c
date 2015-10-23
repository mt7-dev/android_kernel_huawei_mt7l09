

#include <vxWorks.h>
#include <logLib.h>
#include "stdio.h"
#include "string.h"
#include "osl_types.h"
#include "bsp_audio.h"

/*是否使能AUDIO NV功能，1表示使能，0表示不使能
 */
unsigned int bEnAudioNV = 1;

/*AUDIO NV项
 */
AUDIO_VER_NV_STRU            struVerNV;
AUDIO_VOICE_HANDSET_NV_STRU  struVHandsetNV;
AUDIO_VOICE_HANDFREE_NV_STRU struVHandfreeNV;
AUDIO_VOICE_HEADSET_NV_STRU  struVHeadsetNV;
AUDIO_MEDIA_HANDSET_NV_STRU  struMHandsetNV;
AUDIO_MEDIA_HANDFREE_NV_STRU struMHandfreeNV;
AUDIO_MEDIA_HEADSET_NV_STRU  struMHeadsetNV;
AUDIO_LINE_NV_STRU           struLineNV;

/*底软Audio在没有OAM层提供NV的情况下使用这一组默认NV项。
 */
unsigned int NVInitTable[][10]=
{
    {AUDIO_VOICE_HEADSET_NV_ID, 0x0079,0x000B,0x0000,                            0x000E},
    {AUDIO_VOICE_HANDSET_NV_ID, 0x0079,0x0079,0x0000,0x000B,0x0000,              0x000E},
    {AUDIO_VOICE_HANDFREE_NV_ID,0x0079,0x0000,0x0079,0x0000,0x0000,0x000B,0x0000,0x000E},
    {AUDIO_MEDIA_HEADSET_NV_ID, 0x0079,0x000B,0x0000,                            0x0002},
    {AUDIO_MEDIA_HANDSET_NV_ID, 0x0079,0x0079,0x0000,0x000B,0x0000,              0x0002},
    {AUDIO_MEDIA_HANDFREE_NV_ID,0x0079,0x0000,0x0079,0x0000,0x0000,0x000B,0x0000,0x0002},
    {AUDIO_LINE_NV_ID,          0x0011,0x0000},
    {AUDIO_VER_NV_ID,           0x8990,0x0004}
};

/*****************************************************************
*  Function:  max9851NVRead
*  Description:  write a NV Item
*
*  Calls:
*       NV_Write
*
*  Called By:
*
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         index:NV Item Index
*         arg1-arg4:NV Item argument
*
*  Output:
*         None
*
*  Return:
*         AUDIO_OK: the function successful returned
*         AUDIO_ERROR: the function encounter error while running
*
******************************************************************/
STATUS audio_nv_default_value(unsigned int *nvdata)
{
    STATUS err=AUDIO_OK;

    switch(*nvdata)
    {
        case AUDIO_VOICE_HANDSET_NV_ID:
        {
            memcpy(&struVHandsetNV, (nvdata+1), sizeof(struVHandsetNV));
            break;
        }
        case AUDIO_VOICE_HANDFREE_NV_ID:
        {
            memcpy(&struVHandfreeNV, (nvdata+1), sizeof(struVHandfreeNV));
            break;
        }
        case AUDIO_VOICE_HEADSET_NV_ID:
        {
            memcpy(&struVHeadsetNV, (nvdata+1), sizeof(struVHeadsetNV));
            break;
        }
        case AUDIO_MEDIA_HANDSET_NV_ID:
        {
            memcpy(&struMHandsetNV, (nvdata+1), sizeof(struMHandsetNV));
            break;
        }
        case AUDIO_MEDIA_HANDFREE_NV_ID:
        {
            memcpy(&struMHandfreeNV, (nvdata+1), sizeof(struMHandfreeNV));
            break;
        }
        case AUDIO_MEDIA_HEADSET_NV_ID:
        {
            memcpy(&struMHeadsetNV, (nvdata+1), sizeof(struMHeadsetNV));
            break;
        }
        case AUDIO_LINE_NV_ID:
        {
            memcpy(&struLineNV, (nvdata+1), sizeof(struLineNV));
            break;
        }
        case AUDIO_VER_NV_ID:
        {
            memcpy(&struVerNV, (nvdata+1), sizeof(struVerNV));
            break;
        }
        default:
        {
            err=AUDIO_ERROR;
            break;
        }
    }

    return err;
}


/*****************************************************************
*  Function:  audioNVInitial
*  Description:
*
*  Calls:
*       NV_Write
*
*  Called By:
*
*
*  Table Accessed: NONE
*
*  Table Updated: NONE
*
*  Input:
*         index:NV Item Index
*         arg1-arg4:NV Item argument
*
*  Output:
*         None
*
*  Return:
*         AUDIO_OK: the function successful returned
*         AUDIO_ERROR: the function encounter error while running
*
******************************************************************/
STATUS audio_nv_init(void)
{
    /*STATUS err=AUDIO_OK;
 */
    /*unsigned int i;
 */
    if (AUDIO_OK != audio_nv_read())
    {
        logMsg("audio nv read fail!!\n",1,2,3,4,5,6);
        return AUDIO_ERROR;
    }

    return AUDIO_OK;
/*AT2D18396 end*/
#if 0
#ifdef INCLUDE_PS
    err = (STATUS)NV_Read(AUDIO_VER_NV_ID,
                          &struVerNV, sizeof(struVerNV));
    err = (STATUS)NV_Read(AUDIO_VOICE_HANDSET_NV_ID,
                          &struVHandsetNV, sizeof(struVHandsetNV));
    err = (STATUS)NV_Read(AUDIO_VOICE_HANDFREE_NV_ID,
                          &struVHandfreeNV, sizeof(struVHandfreeNV));
    err = (STATUS)NV_Read(AUDIO_VOICE_HEADSET_NV_ID,
                          &struVHeadsetNV, sizeof(struVHeadsetNV));
    err = (STATUS)NV_Read(AUDIO_MEDIA_HANDSET_NV_ID,
                          &struMHandsetNV, sizeof(struMHandsetNV));
    err = (STATUS)NV_Read(AUDIO_MEDIA_HANDFREE_NV_ID,
                          &struMHandfreeNV, sizeof(struMHandfreeNV));
    err = (STATUS)NV_Read(AUDIO_MEDIA_HEADSET_NV_ID,
                          &struMHeadsetNV, sizeof(struMHeadsetNV));
    err = (STATUS)NV_Read(AUDIO_LINE_NV_ID,
                          &struLineNV, sizeof(struLineNV));
#else
    for(i=(sizeof(NVInitTable)/sizeof(NVInitTable[0])); i>0; i--)
    {
        err |= audio_nv_default_value(&NVInitTable[i-1][0]);
    }
#if 0
    err |= audio_nv_default_value(&NVInitTable[1][0]);
    err |= audio_nv_default_value(&NVInitTable[2][0]);
    err |= audio_nv_default_value(&NVInitTable[3][0]);
    err |= audio_nv_default_value(&NVInitTable[4][0]);
    err |= audio_nv_default_value(&NVInitTable[5][0]);
    err |= audio_nv_default_value(&NVInitTable[6][0]);
    err |= audio_nv_default_value(&NVInitTable[7][0]);
#endif
#endif


    return err;
#endif
}

void audio_nv_enbale(unsigned int arg)
{
    if(0==arg)
    {
        bEnAudioNV = 0;
    }
    else
    {
        bEnAudioNV = 1;
    }
}

void disp_audio_nv(void)
{

    printf("Version: id(0x%x)\r\n",AUDIO_VER_NV_ID);
    printf("0x%X, 0x%X \r\n",
                              struVerNV.uCodecVersion,
                              struVerNV.tCBDelayTime);

    printf("VoiceHeadset: id(0x%x)\r\n",AUDIO_VOICE_HEADSET_NV_ID);
    printf("0x%X, 0x%X, 0x%X, 0x%X \r\n",
                              struVHeadsetNV.down_lr_outvol,
                              struVHeadsetNV.up_rin12vol,
                              struVHeadsetNV.up_r12mnbst,
                              struVHeadsetNV.filter.dword);

    printf("VoiceHandset: id(0x%x)\r\n",AUDIO_VOICE_HANDSET_NV_ID);
    printf("0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X \r\n",
                              struVHandsetNV.down_r_outvol,
                              struVHandsetNV.down_r_opgavol,
                              struVHandsetNV.down_out4attn,
                              struVHandsetNV.up_lin12vol,
                              struVHandsetNV.up_l12mnbst,
                              struVHandsetNV.filter.dword);

    printf("VoiceHandfree: id(0x%x)\r\n",AUDIO_VOICE_HANDFREE_NV_ID);
    printf("0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X \r\n",
                              struVHandfreeNV.down_lr_opgavol,
                              struVHandfreeNV.down_spkattn,
                              struVHandfreeNV.down_spkvol,
                              struVHandfreeNV.down_dcgain,
                              struVHandfreeNV.down_acgain,
                              struVHandfreeNV.up_lin12vol,
                              struVHandfreeNV.up_l12mnbst,
                              struVHandfreeNV.filter.dword);

    printf("MediaHeadset: id(0x%x)\r\n",AUDIO_MEDIA_HEADSET_NV_ID);
    printf("0x%X, 0x%X, 0x%X, 0x%X \r\n",
                              struMHeadsetNV.down_lr_outvol,
                              struMHeadsetNV.up_rin12vol,
                              struMHeadsetNV.up_r12mnbst,
                              struMHeadsetNV.filter.dword);

    printf("MediaHandset: id(0x%x)\r\n",AUDIO_MEDIA_HANDSET_NV_ID);
    printf("0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X \r\n",
                              struMHandsetNV.down_r_outvol,
                              struMHandsetNV.down_r_opgavol,
                              struMHandsetNV.down_out4attn,
                              struMHandsetNV.up_lin12vol,
                              struMHandsetNV.up_l12mnbst,
                              struMHandsetNV.filter.dword);

    printf("MediaHandfree: id(0x%x)\r\n",AUDIO_MEDIA_HANDFREE_NV_ID);
    printf("0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X \r\n",
                              struMHandfreeNV.down_lr_opgavol,
                              struMHandfreeNV.down_spkattn,
                              struMHandfreeNV.down_spkvol,
                              struMHandfreeNV.down_dcgain,
                              struMHandfreeNV.down_acgain,
                              struMHandfreeNV.up_lin12vol,
                              struMHandfreeNV.up_l12mnbst,
                              struMHandfreeNV.filter.dword);


    printf("Line: id(0x%x)\r\n",AUDIO_LINE_NV_ID);
    printf("0x%X, 0x%X \r\n",
                              struLineNV.linein_gain,
                              struLineNV.lineout_gain);

}

int audio_nv_read(void)
{
#if 0
    if (NV_OK != (int)DR_NV_Read(AUDIO_VOICE_HEADSET_NV_ID,&struVHeadsetNV, sizeof(struVHeadsetNV)))
    {
        return OSAL_ERROR;
    }
    if ( NV_OK !=(int)DR_NV_Read(AUDIO_VOICE_HANDSET_NV_ID,&struVHandsetNV, sizeof(struVHandsetNV)))
    {
        return OSAL_ERROR;
    }
    if (NV_OK != (int)DR_NV_Read(AUDIO_VOICE_HANDFREE_NV_ID,&struVHandfreeNV, sizeof(struVHandfreeNV)))
    {
        return OSAL_ERROR;
    }
#endif
    struVHeadsetNV.down_lr_outvol = 0x68;
    struVHeadsetNV.filter.dword   = 0x18;
    struVHeadsetNV.up_r12mnbst    = 0x1;
    struVHeadsetNV.up_rin12vol    = 0xe;

    struVHandsetNV.down_out4attn  = 0x79;
    struVHandsetNV.down_r_opgavol = 0x79;
    struVHandsetNV.down_r_outvol  = 0x0;
    struVHandsetNV.filter.dword   = 0xb;
    struVHandsetNV.up_l12mnbst    = 0x0;
    struVHandsetNV.up_lin12vol    = 0xe;

    struVHandfreeNV.down_acgain   = 0x79;
    struVHandfreeNV.down_dcgain   = 0x0;
    struVHandfreeNV.down_lr_opgavol = 0x79;
    struVHandfreeNV.down_spkattn  = 0x0;
    struVHandfreeNV.down_spkvol   = 0x0;
    struVHandfreeNV.filter.dword  = 0xb;
    struVHandfreeNV.up_l12mnbst   = 0x0;
    struVHandfreeNV.up_lin12vol   = 0xe;

    return OK;

}

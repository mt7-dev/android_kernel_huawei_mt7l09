
#ifndef __MLIB_REGISTER_PROFILE2_INC__
#define __MLIB_REGISTER_PROFILE2_INC__

#include "mlib_macro.inc"

/* 外部模块头文件包含 */
#include "module_default.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  场景: MLIB_PATH_CS_VOICE_CALL
*****************************************************************************/
MLIB_REGISTER_PATH_MODULE_BEGIN(MLIB_PROFILE2,MLIB_PATH_CS_VOICE_CALL_MICIN)
    /* 模块注册说明: 模块ID|模块初始化|模块参数设置|模块参数获取|模块数据处理|模块清空 */
    MLIB_REGISTER_MODULE(
                MLIB_MODULE_DEFAULT,
                NULL,
                NULL,
                NULL,
                MLIB_DEFAULT_Proc,
                NULL)
MLIB_REGISTER_PATH_MODULE_END()

MLIB_REGISTER_PATH_MODULE_BEGIN(MLIB_PROFILE2,MLIB_PATH_CS_VOICE_CALL_SPKOUT)
    /* 模块注册说明: 模块ID|模块初始化|模块参数设置|模块参数获取|模块数据处理|模块清空 */
    MLIB_REGISTER_MODULE(
                MLIB_MODULE_DEFAULT,
                NULL,
                NULL,
                NULL,
                MLIB_DEFAULT_Proc,
                NULL)
MLIB_REGISTER_PATH_MODULE_END()

/*****************************************************************************
  场景: MLIB_PATH_VOIP_CALL
*****************************************************************************/
MLIB_REGISTER_PATH_MODULE_BEGIN(MLIB_PROFILE2,MLIB_PATH_VOIP_CALL_MICIN)
    /* 模块注册说明: 模块ID|模块初始化|模块参数设置|模块参数获取|模块数据处理|模块清空 */
    MLIB_REGISTER_MODULE(
                MLIB_MODULE_DEFAULT,
                NULL,
                NULL,
                NULL,
                MLIB_DEFAULT_Proc,
                NULL)
MLIB_REGISTER_PATH_MODULE_END()

MLIB_REGISTER_PATH_MODULE_BEGIN(MLIB_PROFILE2,MLIB_PATH_VOIP_CALL_SPKOUT)
    /* 模块注册说明: 模块ID|模块初始化|模块参数设置|模块参数获取|模块数据处理|模块清空 */
    MLIB_REGISTER_MODULE(
                MLIB_MODULE_DEFAULT,
                NULL,
                NULL,
                NULL,
                MLIB_DEFAULT_Proc,
                NULL)
MLIB_REGISTER_PATH_MODULE_END()

/*****************************************************************************
  场景: MLIB_PATH_AUDIO_PLAY
        MLIB_PATH_AUDIO_RECORD
*****************************************************************************/
MLIB_REGISTER_PATH_MODULE_BEGIN(MLIB_PROFILE2,MLIB_PATH_AUDIO_PLAY)
    /* 模块注册说明: 模块ID|模块初始化|模块参数设置|模块参数获取|模块数据处理|模块清空 */
    MLIB_REGISTER_MODULE(
                MLIB_MODULE_DEFAULT,
                NULL,
                NULL,
                NULL,
                MLIB_DEFAULT_Proc,
                NULL)
MLIB_REGISTER_PATH_MODULE_END()

MLIB_REGISTER_PATH_MODULE_BEGIN(MLIB_PROFILE2,MLIB_PATH_AUDIO_RECORD)
    /* 模块注册说明: 模块ID|模块初始化|模块参数设置|模块参数获取|模块数据处理|模块清空 */
    MLIB_REGISTER_MODULE(
                MLIB_MODULE_DEFAULT,
                NULL,
                NULL,
                NULL,
                MLIB_DEFAULT_Proc,
                NULL)
MLIB_REGISTER_PATH_MODULE_END()

/*****************************************************************************
  场景: MLIB_PATH_SIRI
*****************************************************************************/
MLIB_REGISTER_PATH_MODULE_BEGIN(MLIB_PROFILE2,MLIB_PATH_SIRI_SPKOUT)
    /* 模块注册说明: 模块ID|模块初始化|模块参数设置|模块参数获取|模块数据处理|模块清空 */
    MLIB_REGISTER_MODULE(
                MLIB_MODULE_DEFAULT,
                NULL,
                NULL,
                NULL,
                MLIB_DEFAULT_Proc,
                NULL)
MLIB_REGISTER_PATH_MODULE_END()

MLIB_REGISTER_PATH_MODULE_BEGIN(MLIB_PROFILE2,MLIB_PATH_SIRI_MICIN)
    /* 模块注册说明: 模块ID|模块初始化|模块参数设置|模块参数获取|模块数据处理|模块清空 */
    MLIB_REGISTER_MODULE(
                MLIB_MODULE_DEFAULT,
                NULL,
                NULL,
                NULL,
                MLIB_DEFAULT_Proc,
                NULL)
MLIB_REGISTER_PATH_MODULE_END()


/*****************************************************************************
  场景注册
*****************************************************************************/
MLIB_REGISTER_PATH_BEGIN(MLIB_PROFILE2)
    MLIB_REGISTER_PATH(
                MLIB_PATH_CS_VOICE_CALL_MICIN,
                MLIB_GetPathModules(MLIB_PROFILE2,MLIB_PATH_CS_VOICE_CALL_MICIN),
                MLIB_GetPathModuleNum(MLIB_PROFILE2,MLIB_PATH_CS_VOICE_CALL_MICIN))
    MLIB_REGISTER_PATH(
                MLIB_PATH_CS_VOICE_CALL_SPKOUT,
                MLIB_GetPathModules(MLIB_PROFILE2,MLIB_PATH_CS_VOICE_CALL_SPKOUT),
                MLIB_GetPathModuleNum(MLIB_PROFILE2,MLIB_PATH_CS_VOICE_CALL_SPKOUT))
    MLIB_REGISTER_PATH(
                MLIB_PATH_VOIP_CALL_MICIN,
                MLIB_GetPathModules(MLIB_PROFILE2,MLIB_PATH_VOIP_CALL_MICIN),
                MLIB_GetPathModuleNum(MLIB_PROFILE2,MLIB_PATH_VOIP_CALL_MICIN))
    MLIB_REGISTER_PATH(
                MLIB_PATH_VOIP_CALL_SPKOUT,
                MLIB_GetPathModules(MLIB_PROFILE2,MLIB_PATH_VOIP_CALL_SPKOUT),
                MLIB_GetPathModuleNum(MLIB_PROFILE2,MLIB_PATH_VOIP_CALL_SPKOUT))
    MLIB_REGISTER_PATH(
                MLIB_PATH_AUDIO_PLAY,
                MLIB_GetPathModules(MLIB_PROFILE2,MLIB_PATH_AUDIO_PLAY),
                MLIB_GetPathModuleNum(MLIB_PROFILE2,MLIB_PATH_AUDIO_PLAY))
    MLIB_REGISTER_PATH(
                MLIB_PATH_AUDIO_RECORD,
                MLIB_GetPathModules(MLIB_PROFILE2,MLIB_PATH_AUDIO_RECORD),
                MLIB_GetPathModuleNum(MLIB_PROFILE2,MLIB_PATH_AUDIO_RECORD))
    MLIB_REGISTER_PATH(
                MLIB_PATH_SIRI_SPKOUT,
                MLIB_GetPathModules(MLIB_PROFILE2,MLIB_PATH_SIRI_SPKOUT),
                MLIB_GetPathModuleNum(MLIB_PROFILE2,MLIB_PATH_SIRI_SPKOUT))
    MLIB_REGISTER_PATH(
                MLIB_PATH_SIRI_MICIN,
                MLIB_GetPathModules(MLIB_PROFILE2,MLIB_PATH_SIRI_MICIN),
                MLIB_GetPathModuleNum(MLIB_PROFILE2,MLIB_PATH_SIRI_MICIN))
MLIB_REGISTER_PATH_END()


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of mlib_module.h */

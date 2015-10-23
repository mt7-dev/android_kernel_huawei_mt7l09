/* Copyright (c) 2011-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
* These coded instructions, statements, and computer programs are the
* copyrighted works and confidential proprietary information of
* Tensilica Inc. and its licensors, and are licensed to the recipient
* under the terms of a separate license agreement.  They may be
* adapted and modified by bona fide purchasers under the terms of the
* separate license agreement for internal use, but no adapted or
* modified version may be disclosed or distributed to third parties
* in any manner, medium, or form, in whole or in part, without the
* prior written consent of Tensilica Inc.
*/

#ifndef _XA_SRC_PP_API_H_
#define _XA_SRC_PP_API_H_

#include "xa_type_def.h"

/* Sample Rate Convrter configuration parameters */
enum xa_config_param_src_pp
{
    XA_SRC_PP_CONFIG_PARAM_INPUT_SAMPLE_RATE = 0,
    XA_SRC_PP_CONFIG_PARAM_OUTPUT_SAMPLE_RATE,
    XA_SRC_PP_CONFIG_PARAM_INPUT_CHUNK_SIZE,
    XA_SRC_PP_CONFIG_PARAM_OUTPUT_CHUNK_SIZE,
    XA_SRC_PP_CONFIG_PARAM_INPUT_CHANNELS,
    XA_SRC_PP_CONFIG_PARAM_SET_INPUT_BUF_PTR,
    XA_SRC_PP_CONFIG_PARAM_SET_OUTPUT_BUF_PTR,
    XA_SRC_PP_CONFIG_PARAM_GET_NUM_STAGES
};

#define XA_SRC_PP 3

/*****************************************************************************/
/* Class 1: Configuration Errors                                             */
/*****************************************************************************/
/* Fatal Errors */
enum xa_error_fatal_config_src_pp
{
  XA_SRC_PP_CONFIG_FATAL_INVALID_INPUT_RATE             = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_SRC_PP, 0),
  XA_SRC_PP_CONFIG_FATAL_INVALID_OUTPUT_RATE            = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_SRC_PP, 1),
  XA_SRC_PP_CONFIG_FATAL_INVALID_INPUT_CHUNK_SIZE       = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_SRC_PP, 2),
  XA_SRC_PP_CONFIG_FATAL_INVALID_INPUT_CHANNELS         = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_SRC_PP, 3),
  XA_SRC_PP_CONFIG_FATAL_INVALID_NUM_STAGES             = XA_ERROR_CODE(xa_severity_fatal, xa_class_config, XA_SRC_PP, 4)
};

/*****************************************************************************/
/* Class 2: Execution Class Errors                                           */
/*****************************************************************************/
/* Fatal Errors */
enum xa_error_fatal_execute_src_pp
{
  XA_SRC_PP_EXECUTE_FATAL_ERR_POST_CONFIG_INIT          = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_SRC_PP, 0),
  XA_SRC_PP_EXECUTE_FATAL_ERR_INIT                      = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_SRC_PP, 1),
  XA_SRC_PP_EXECUTE_FATAL_ERR_EXECUTE                   = XA_ERROR_CODE(xa_severity_fatal, xa_class_execute, XA_SRC_PP, 2),
};

#if defined(__cplusplus)
extern "C" {
#endif  /* __cplusplus */
xa_codec_func_t xa_src_pp;
#if defined(__cplusplus)
}
#endif  /* __cplusplus */

#endif  /*_XA_SRC_PP_API_H_*/

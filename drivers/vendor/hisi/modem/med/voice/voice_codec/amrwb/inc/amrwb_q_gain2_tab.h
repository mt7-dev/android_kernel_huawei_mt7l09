/*------------------------------------------------------*
 * Tables for function q_gain2()                        *
 *                                                      *
 *  g_pitch(Q14),  g_code(Q11)                          *
 *                                                      *
 * pitch gain are ordered in table to reduce complexity *
 * during quantization of gains.                        *
 *------------------------------------------------------*/
#include "codec_op_etsi.h"

#define nb_qua_gain6b  64     /* Number of quantization level */
#define nb_qua_gain7b  128    /* Number of quantization level */


extern Word16 t_qua_gain6b[64*2];
extern Word16 t_qua_gain7b[128*2];

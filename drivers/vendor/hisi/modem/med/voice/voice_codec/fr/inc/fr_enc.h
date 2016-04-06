/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/*
  ************************** WARNING ******************************
   Several pseudo-functions GSM_... provide faster operation than
   their equivalent true function, but may generate non-compatible
   results. It is known that the implementation of GSM_ADD_2 is
   not working correctly on 16-bit machines/compilers (besides
   an error-free compilation!). In the safer case, compilation
   should be carried out using SLOWER_BUT_SAFER defined.
   Another test is using add_test.c, explained elsewhere.
   -- <simao@cpqd.ansp.br> [08.Apr.94] ---
  *****************************************************************
*/

#ifndef __FR_ENC_H__
#define __FR_ENC_H__

#include "fr_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* FR编码参数中各部分参数的长度 */
#define FR_LARC_LEN                     (8)
#define FR_NC_LEN                       (4)
#define FR_BC_LEN                       (4)
#define FR_MC_LEN                       (4)
#define FR_XMAXC_LEN                    (4)
#define FR_XMC_LEN                      (52)

/*
 *  More prototypes from implementations..
 */
extern void FR_EncInit(
                FR_ENCODE_STATE_STRU                       *pstMedFrEncObj,
                FR_DTX_ENUM_UINT16                          enDtxMode);

extern void Gsm_Coder(
        FR_ENCODE_STATE_STRU    * S,
        Word16    * s,    /* [0..159] samples     IN  */
        Word16    * LARc, /* [0..7] LAR coefficients  OUT */
        Word16    * Nc,   /* [0..3] LTP lag       OUT     */
        Word16    * bc,   /* [0..3] coded LTP gain    OUT     */
        Word16    * Mc,   /* [0..3] RPE grid selection    OUT     */
        Word16    * xmaxc,/* [0..3] Coded maximum amplitude OUT   */
        Word16    * xMc,  /* [13*4] normalized RPE samples OUT    */
        FR_VAD_STATUS_ENUM_UINT16  *penVadFlag,    /* VAD检测标志  OUT */
        Word16    *pshwSpFlag      /* SP判定标志   OUT */);

extern void gsm_encode(FR_ENCODE_STATE_STRU * s, Word16 * source, Word16 * c);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* __FR_ENC_H__ */

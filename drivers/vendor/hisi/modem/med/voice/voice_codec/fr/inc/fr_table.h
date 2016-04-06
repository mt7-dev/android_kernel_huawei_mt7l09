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

#ifndef __FR_TABLE_H__
#define __FR_TABLE_H__

#include "fr_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 *  Tables from fr_table.c
 */
#ifndef GSM_TABLE_C

extern Word16 gsm_A[8], gsm_B[8], gsm_MIC[8], gsm_MAC[8];
extern Word16 gsm_INVA[8];
extern Word16 gsm_DLB[4], gsm_QLB[4];
extern Word16 gsm_H[11];
extern Word16 gsm_NRFAC[8];
extern Word16 gsm_FAC[8];

#endif  /* GSM_TABLE_C */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif


#endif  /* __FR_TABLE_H__ */

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/*$Header: /home/kbs/jutta/src/gsm/gsm-1.0/inc/RCS/private.h,v 1.2 1993/01/29 18:25:27 jutta Exp $*/

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

#ifndef __FR_SHORT_TERM_H__
#define __FR_SHORT_TERM_H__

#include "fr_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 *  More prototypes from implementations..
 */
extern void Decoding_of_the_coded_Log_Area_Ratios(
    Word16    * LARc,     /* coded log area ratio [0..7]  IN  */
    Word16    * LARpp    /* out: decoded ..          */
);

extern void Gsm_Short_Term_Analysis_Filter(
    FR_RPELTP_STATE_STRU * S,
    Word16    * LARc,     /* coded log area ratio [0..7]  IN  */
    Word16    * s     /* signal [0..159]      IN/OUT  */
);

extern void Gsm_Short_Term_Synthesis_Filter(
    FR_RPELTP_STATE_STRU * S,
    Word16  * LARpp_j,
    Word16  * LARpp_j_1,
    Word16    * wt,       /* received d [0..159]         IN  */
    Word16    * s     /* signal   s [0..159]        OUT  */
);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* __FR_SHORT_TERM_H__ */

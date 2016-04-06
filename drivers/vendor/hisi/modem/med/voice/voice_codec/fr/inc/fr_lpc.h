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

#ifndef __FR_LPC_H__
#define __FR_LPC_H__

#include "fr_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 *  More prototypes from implementations..
 */
extern void Gsm_LPC_Analysis(
        Word8    fast,
        Word16 * s,    /* 0..159 signals  IN/OUT  */
        Word16 * LARc,
        Word32       * aswVadLacf,
        Word16       * pshwVadScalauto,
        Word16       * ashwDtxLaruq
);   /* 0..7   LARc's   OUT */

extern void Quantization_and_coding(
        Word16 * LAR     /* [0..7]   IN/OUT  */);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif  /* __FR_LPC_H__ */

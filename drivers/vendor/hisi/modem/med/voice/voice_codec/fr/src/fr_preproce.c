/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header: /home/kbs/jutta/src/gsm/gsm-1.0/src/RCS/preprocess.c,v 1.1 1992/10/28 00:15:50 jutta Exp $ */

#ifdef _MED_C89_
#include <stdio.h>
#endif

#include "fr_etsi_op.h"
#include "fr_codec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*  4.2.0 .. 4.2.3  PREPROCESSING SECTION
 *
 *      After A-law to linear conversion (or directly from the
 *      Ato D converter) the following scaling is assumed for
 *  input to the RPE-LTP algorithm:
 *
 *      in:  0.1.....................12
 *       S.v.v.v.v.v.v.v.v.v.v.v.v.*.*.*
 *
 *  Where S is the sign bit, v a valid bit, and * a "don't care" bit.
 *  The original signal is called sop[..]
 *
 *      out:   0.1................... 12
 *       S.S.v.v.v.v.v.v.v.v.v.v.v.v.0.0
 */


void Gsm_Preprocess(
    FR_RPELTP_STATE_STRU * S,
    Word16         * s,
    Word16         * so)     /* [0..159]     IN/OUT  */
{

    Word16       z1 = S->z1;
    Word32 L_z2 = S->L_z2;
    Word16       mp = S->mp;

    Word16        s1;
    Word32      L_s2;

    Word32      L_temp;

    Word16        msp, lsp;
    Word16        SO;

     int        k = 160;

    while (k--) {

    /*  4.2.1   Downscaling of the input signal
     */
        SO = SASR( *s, 3 ) << 2;
        s++;



    /*  4.2.2   Offset compensation
     *
     *  This part implements a high-pass filter and requires extended
     *  arithmetic precision for the recursive part of this filter.
     *  The input of this procedure is the array so[0...159] and the
     *  output the array sof[ 0...159 ].
     */
        /*   Compute the non-recursive part
         */

        s1 = SO - z1;           /* s1 = gsm_sub( *so, z1 ); */
        z1 = SO;


        /*   Compute the recursive part
         */
        L_s2 = s1;
        L_s2 <<= 15;

        /*   Execution of a 31 bv 16 bits multiplication
         */

        msp = SASR( L_z2, 15 );
        lsp = L_z2-((Word32)msp<<15); /* gsm_L_sub(L_z2,(msp<<15)); */

        L_s2  += GSM_MULT_R( lsp, 32735 );
        L_temp = (Word32)msp * 32735; /* GSM_L_MULT(msp,32735) >> 1;*/
        L_z2   = GSM_L_ADD( L_temp, L_s2 );

        /*    Compute sof[k] with rounding
         */
        L_temp = GSM_L_ADD( L_z2, 16384 );

    /*   4.2.3  Preemphasis
     */

        msp   = GSM_MULT_R( mp, -28180 );
        mp    = SASR( L_temp, 15 );

        *so++ = GSM_ADD( mp, msp );
    }

    S->z1   = z1;
    S->L_z2 = L_z2;
    S->mp   = mp;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif


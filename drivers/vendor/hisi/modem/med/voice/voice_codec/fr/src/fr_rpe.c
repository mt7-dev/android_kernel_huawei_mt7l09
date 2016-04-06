/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header: /home/kbs/jutta/src/gsm/gsm-1.0/src/RCS/rpe.c,v 1.2 1994/01/25 22:21:15 jutta Exp $ */

#ifdef _MED_C89_
#include <stdio.h>
#endif

#include "fr_etsi_op.h"
#include "fr_codec.h"
#include "fr_table.h"
#include "codec_op_vec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*  4.2.13 .. 4.2.17  RPE ENCODING SECTION
 */

/* 4.2.13 */

static void Weighting_filter(
     Word16   * e,        /* signal [-5..0.39.44] IN  */
    Word16        * x     /* signal [0..39]   OUT */
)
/*
 *  The coefficients of the weighting filter are stored in a table
 *  (see table 4.4).  The following scaling is used:
 *
 *  H[0..10] = integer( real_H[ 0..10] * 8192 );
 */
{
    /* Word16         wt[ 50 ]; */

     Word32   L_result;
     int        k /* , i */ ;

    /*  Initialization of a temporary working array wt[0...49]
     */

    /* for (k =  0; k <=  4; k++) wt[k] = 0;
     * for (k =  5; k <= 44; k++) wt[k] = *e++;
     * for (k = 45; k <= 49; k++) wt[k] = 0;
     *
     *  (e[-5..-1] and e[40..44] are allocated by the caller,
     *  are initially zero and are not written anywhere.)
     */
    e -= 5;

    /*  Compute the signal x[0..39]
     */
    for (k = 0; k <= 39; k++) {

        L_result = 8192 >> 1;

        /* for (i = 0; i <= 10; i++) {
         *  L_temp   = GSM_L_MULT( wt[k+i], gsm_H[i] );
         *  L_result = GSM_L_ADD( L_result, L_temp );
         * }
         */

#ifdef STEP
#undef  STEP
#endif
#define STEP( i, H )    (e[ k + i ] * (Word32)H)

        /*  Every one of these multiplications is done twice --
         *  but I don't see an elegant way to optimize this.
         *  Do you?
         */

#ifdef  STUPID_COMPILER
        L_result += STEP(   0,  -134 ) ;
        L_result += STEP(   1,  -374 )  ;
                   /* + STEP(   2,  0    )  */
        L_result += STEP(   3,  2054 ) ;
        L_result += STEP(   4,  5741 ) ;
        L_result += STEP(   5,  8192 ) ;
        L_result += STEP(   6,  5741 ) ;
        L_result += STEP(   7,  2054 ) ;
               /* + STEP(   8,  0    )  */
        L_result += STEP(   9,  -374 ) ;
        L_result += STEP(   10,     -134 ) ;
#else
        L_result +=
          STEP( 0,  -134 )
        + STEP( 1,  -374 )
         /* + STEP( 2,  0    )  */
        + STEP( 3,  2054 )
        + STEP( 4,  5741 )
        + STEP( 5,  8192 )
        + STEP( 6,  5741 )
        + STEP( 7,  2054 )
         /* + STEP( 8,  0    )  */
        + STEP( 9,  -374 )
        + STEP(10,  -134 )
        ;
#endif

        /* L_result = GSM_L_ADD( L_result, L_result ); (* scaling(x2) *)
         * L_result = GSM_L_ADD( L_result, L_result ); (* scaling(x4) *)
         *
         * x[k] = SASR( L_result, 16 );
         */

        /* 2 adds vs. >>16 => 14, minus one shift to compensate for
         * those we lost when replacing L_MULT by '*'.
         */

        L_result = SASR( L_result, 13 );
        x[k] =  (  L_result < MIN_WORD ? MIN_WORD
            : (L_result > MAX_WORD ? MAX_WORD : L_result ));
    }
}

/* 4.2.14 */

static void RPE_grid_selection(
    Word16        * x,        /* [0..39]      IN  */
    Word16        * xM,       /* [0..12]      OUT */
    Word16        * Mc_out    /*          OUT */
)
/*
 *  The signal x[0..39] is used to select the RPE grid which is
 *  represented by Mc.
 */
{
    /*  Word16    temp1;  */
     int        /* m, */  i;
    /*   ulongword  utmp; */
     Word32   L_result, L_temp;
    Word32        EM; /* xxx should be L_EM? */
    Word16            Mc;

    Word32        L_common_0_3;

    EM = 0;
    Mc = 0;

    /* for (m = 0; m <= 3; m++) {
     *  L_result = 0;
     *
     *
     *  for (i = 0; i <= 12; i++) {
     *
     *      temp1    = SASR( x[m + 3*i], 2 );
     *
     *
     *      L_temp   = GSM_L_MULT( temp1, temp1 );
     *      L_result = GSM_L_ADD( L_temp, L_result );
     *  }
     *
     *  if (L_result > EM) {
     *      Mc = m;
     *      EM = L_result;
     *  }
     * }
     */

#undef  STEP
#define STEP( m, i )        L_temp = SASR( (Word32)x[m + 3 * i], 2 ); \
                L_result += L_temp * L_temp;

    /* common part of 0 and 3 */

    L_result = 0;
    STEP( 0, 1 ); STEP( 0, 2 ); STEP( 0, 3 ); STEP( 0, 4 );
    STEP( 0, 5 ); STEP( 0, 6 ); STEP( 0, 7 ); STEP( 0, 8 );
    STEP( 0, 9 ); STEP( 0, 10); STEP( 0, 11); STEP( 0, 12);
    L_common_0_3 = L_result;

    /* i = 0 */

    STEP( 0, 0 );
    L_result <<= 1; /* implicit in L_MULT */
    EM = L_result;

    /* i = 1 */

    L_result = 0;
    STEP( 1, 0 );
    STEP( 1, 1 ); STEP( 1, 2 ); STEP( 1, 3 ); STEP( 1, 4 );
    STEP( 1, 5 ); STEP( 1, 6 ); STEP( 1, 7 ); STEP( 1, 8 );
    STEP( 1, 9 ); STEP( 1, 10); STEP( 1, 11); STEP( 1, 12);
    L_result <<= 1;
    if (L_result > EM) {
        Mc = 1;
        EM = L_result;
    }

    /* i = 2 */

    L_result = 0;
    STEP( 2, 0 );
    STEP( 2, 1 ); STEP( 2, 2 ); STEP( 2, 3 ); STEP( 2, 4 );
    STEP( 2, 5 ); STEP( 2, 6 ); STEP( 2, 7 ); STEP( 2, 8 );
    STEP( 2, 9 ); STEP( 2, 10); STEP( 2, 11); STEP( 2, 12);
    L_result <<= 1;
    if (L_result > EM) {
        Mc = 2;
        EM = L_result;
    }

    /* i = 3 */

    L_result = L_common_0_3;
    STEP( 3, 12 );
    L_result <<= 1;
    if (L_result > EM) {
        Mc = 3;
        EM = L_result;
    }

    /**/

    /*  Down-sampling by a factor 3 to get the selected xM[0..12]
     *  RPE sequence.
     */
    for (i = 0; i <= 12; i ++) xM[i] = x[Mc + 3*i];
    *Mc_out = Mc;
}

/* 4.12.15 */

static void APCM_quantization_xmaxc_to_exp_mant(
    Word16        xmaxc,      /* IN   */
    Word16        * exp_out,  /* OUT  */
    Word16        * mant_out )    /* OUT  */
{
    Word16    exp, mant;

    /* Compute exponent and mantissa of the decoded version of xmaxc
     */

    exp = 0;
    if (xmaxc > 15) exp = SASR(xmaxc, 3) - 1;
    mant = xmaxc - (exp << 3);

    if (mant == 0) {
        exp  = -4;
        mant = 7;
    }
    else {
        while (mant <= 7) {
            mant = mant << 1 | 1;
            exp--;
        }
        mant -= 8;
    }


    *exp_out  = exp;
    *mant_out = mant;
}

static void APCM_quantization(
    Word16        * xM,       /* [0..12]      IN  */

    Word16        * xMc,      /* [0..12]      OUT */
    Word16        * mant_out, /*          OUT */
    Word16        * exp_out,  /*          OUT */
    Word16        * xmaxc_out, /*          OUT */
    Word16      * pshwDtxXmaxuq)
{
    int i, itest;

    /*   Word32   ltmp;   / * for GSM_ADD */
    Word16    xmax, xmaxc, temp, temp1, temp2;
    Word16    exp, mant;


    /*  Find the maximum absolute value xmax of xM[0..12].
     */

    xmax = CODEC_OpVecMaxAbs(&xM[0], 13, 0);

    /* 提取DTX输入参数 */
    *pshwDtxXmaxuq = xmax;

    /*  Qantizing and coding of xmax to get xmaxc.*/
    exp   = 0;
    temp  = SASR( xmax, 9 );
    itest = 0;

    for (i = 0; i <= 5; i++) {

        itest |= (temp <= 0);
        temp = SASR( temp, 1 );

        if (itest == 0) exp++;      /* exp = add (exp, 1) */
    }

    temp = exp + 5;

    xmaxc = gsm_add( SASR(xmax, temp), exp << 3 );

    /*   Quantizing and coding of the xM[0..12] RPE sequence
     *   to get the xMc[0..12]
     */

    APCM_quantization_xmaxc_to_exp_mant( xmaxc, &exp, &mant );

    /*  This computation uses the fact that the decoded version of xmaxc
     *  can be calculated by using the exponent and the mantissa part of
     *  xmaxc (logarithmic table).
     *  So, this method avoids any division and uses only a scaling
     *  of the RPE samples by a function of the exponent.  A direct
     *  multiplication by the inverse of the mantissa (NRFAC[0..7]
     *  found in table 4.5) gives the 3 bit coded version xMc[0..12]
     *  of the RPE samples.
     */


    /* Direct computation of xMc[0..12] using table 4.5
     */


    temp1 = 6 - exp;        /* normalization by the exponent */
    temp2 = gsm_NRFAC[ mant ];      /* inverse mantissa          */

    for (i = 0; i <= 12; i++) {


        temp = xM[i] << temp1;
        temp = GSM_MULT( temp, temp2 );
        temp = SASR(temp, 12);
        xMc[i] = temp + 4;      /* see note below */
    }

    /*  NOTE: This equation is used to make all the xMc[i] positive.
     */

    *mant_out  = mant;
    *exp_out   = exp;
    *xmaxc_out = xmaxc;
}

/* 4.2.16 */

static void APCM_inverse_quantization(
     Word16   * xMc,  /* [0..12]          IN  */
    Word16        mant,
    Word16        exp,
     Word16   * xMp)  /* [0..12]          OUT     */
/*
 *  This part is for decoding the RPE sequence of coded xMc[0..12]
 *  samples to obtain the xMp[0..12] array.  Table 4.6 is used to get
 *  the mantissa of xmaxc (FAC[0..7]).
 */
{
    int i;
    Word16    temp, temp1, temp2, temp3;

    temp1 = gsm_FAC[ mant ];    /* see 4.2-15 for mant */
    temp2 = gsm_sub( 6, exp );  /* see 4.2-15 for exp  */
    temp3 = gsm_asl( 1, gsm_sub( temp2, 1 ));

    for (i = 13; i--;) {


        /* temp = gsm_sub( *xMc++ << 1, 7 ); */
        temp = (*xMc++ << 1) - 7;           /* restore sign   */

        temp <<= 12;                /* 16 bit signed  */
        temp = GSM_MULT_R( temp1, temp );
        temp = GSM_ADD( temp, temp3 );
        *xMp++ = gsm_asr( temp, temp2 );
    }
}

/* 4.2.17 */

static void RPE_grid_positioning(
    Word16        Mc,     /* grid position    IN  */
     Word16   * xMp,      /* [0..12]      IN  */
     Word16   * ep        /* [0..39]      OUT */
)
/*
 *  This procedure computes the reconstructed long term residual signal
 *  ep[0..39] for the LTP analysis filter.  The inputs are the Mc
 *  which is the grid position selection and the xMp[0..12] decoded
 *  RPE samples which are upsampled by a factor of 3 by inserting zero
 *  values.
 */
{
#if defined(VMS) || defined(__TURBOC__)
    int k;
#endif
    int i = 13;


#if defined(VMS) || defined(__TURBOC__)
    for (k = 0; k <= 39; k++) ep[k] = 0;
    for (i = 0; i <= 12; i++) {
        ep[ Mc + (3*i) ] = xMp[i];
    }
#else
        switch (Mc) {
                case 3: *ep++ = 0;
                case 2:  do {
                                *ep++ = 0;
                case 1:         *ep++ = 0;
                case 0:         *ep++ = *xMp++;
                         } while (--i);
        }
        while (++Mc < 4) *ep++ = 0;
#endif
}

/* 4.2.18 */

/*  This procedure adds the reconstructed long term residual signal
 *  ep[0..39] to the estimated signal dpp[0..39] from the long term
 *  analysis filter to compute the reconstructed short term residual
 *  signal dp[-40..-1]; also the reconstructed short term residual
 *  array dp[-120..-41] is updated.
 */

void Gsm_RPE_Encoding(
    Word16    * e,        /* -5..-1][0..39][40..44    IN/OUT  */
    Word16    * xmaxc,    /*              OUT */
    Word16    * Mc,       /*              OUT */
    Word16    * xMc,      /* [0..12]          OUT */
    Word16  * pshwDtxXmaxuq)
{
    Word16    x[40];
    Word16    xM[13], xMp[13];
    Word16    mant, exp;

    Weighting_filter(e, x);
    RPE_grid_selection(x, xM, Mc);

    APCM_quantization(  xM, xMc, &mant, &exp, xmaxc, pshwDtxXmaxuq);
    APCM_inverse_quantization(  xMc,  mant,  exp, xMp);

    RPE_grid_positioning( *Mc, xMp, e );

}

void Gsm_RPE_Decoding(
    Word16        xmaxcr,
    Word16        Mcr,
    Word16        * xMcr,  /* [0..12], 3 bits         IN  */
    Word16        * erp    /* [0..39]         OUT     */
)
{
    Word16    exp, mant;
    Word16    xMp[ 13 ];

    APCM_quantization_xmaxc_to_exp_mant( xmaxcr, &exp, &mant );
    APCM_inverse_quantization( xMcr, mant, exp, xMp );
    RPE_grid_positioning( Mcr, xMp, erp );

}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif


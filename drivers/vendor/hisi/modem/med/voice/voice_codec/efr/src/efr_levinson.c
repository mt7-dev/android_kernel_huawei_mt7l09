/*************************************************************************
 *
 *   FUNCTION:  Levinson()
 *
 *   PURPOSE:  Levinson-Durbin algorithm in double precision. To compute the
 *             LP filter parameters from the speech autocorrelations.
 *
 *   DESCRIPTION:
 *       R[i]    autocorrelations.
 *       A[i]    filter coefficients.
 *       K       reflection coefficients.
 *       Alpha   prediction gain.
 *
 *       Initialisation:
 *               A[0] = 1
 *               K    = -R[1]/R[0]
 *               A[1] = K
 *               Alpha = R[0] * (1-K**2]
 *
 *       Do for  i = 2 to EFR_M
 *
 *            S =  SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i]
 *
 *            K = -S / Alpha
 *
 *            An[j] = A[j] + K*A[i-j]   for j=1 to i-1
 *                                      where   An[i] = new A[i]
 *            An[i]=K
 *
 *            Alpha=Alpha * (1-K**2)
 *
 *       END
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_cnst.h"

/* Lpc order == 10 */


/* Last A(z) for case of unstable filter */

Word16 efr_old_A[EFR_M + 1];

void EFR_Levinson (
    Word16 Rh[],    /* (i)     : Rh[m+1] Vector of autocorrelations (msb) */
    Word16 Rl[],    /* (i)     : Rl[m+1] Vector of autocorrelations (lsb) */
    Word16 A[],     /* (o)     : A[m]    LPC coefficients  (m = 10)       */
    Word16 rc[]     /* (o)     : rc[4]   First 4 reflection coefficients  */

)
{
    Word16 i, j;
    Word16 hi, lo;
    Word16 Kh, Kl;                /* reflexion coefficient; hi and lo      */
    Word16 alp_h, alp_l, alp_exp; /* Prediction gain; hi lo and exponent   */
    Word16 Ah[EFR_M + 1], Al[EFR_M + 1];  /* LPC coef. in double prec.             */
    Word16 Anh[EFR_M + 1], Anl[EFR_M + 1];/* LPC coef.for next iteration in double
                                     prec. */
    Word32 t0, t1, t2;            /* temporary variable                    */

    /* K = A[1] = -R[1] / R[0] */

    t1 = L_Comp (Rh[1], Rl[1]);
    t2 = L_abs (t1);                    /* abs R[1]         */
    t0 = Div_32 (t2, Rh[0], Rl[0]);     /* R[1]/R[0]        */

    if (t1 > 0)
        t0 = L_negate (t0);             /* -R[1]/R[0]       */
    L_Extract (t0, &Kh, &Kl);           /* K in DPF         */

    rc[0] = round (t0);

    t0 = L_shr (t0, 4);                 /* A[1] in          */
    L_Extract (t0, &Ah[1], &Al[1]);     /* A[1] in DPF      */

    /*  Alpha = R[0] * (1-K**2) */

    t0 = Mpy_32 (Kh, Kl, Kh, Kl);       /* K*K             */
    t0 = L_abs (t0);                    /* Some case <0 !! */
    t0 = L_sub ((Word32) 0x7fffffffL, t0); /* 1 - K*K        */
    L_Extract (t0, &hi, &lo);           /* DPF format      */
    t0 = Mpy_32 (Rh[0], Rl[0], hi, lo); /* Alpha in        */

    /* Normalize Alpha */

    alp_exp = norm_l (t0);
    t0 = L_shl (t0, alp_exp);
    L_Extract (t0, &alp_h, &alp_l);     /* DPF format    */

    /*--------------------------------------*
     * ITERATIONS  I=2 to EFR_M                 *
     *--------------------------------------*/

    for (i = 2; i <= EFR_M; i++)
    {
        /* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */

        t0 = 0;
        for (j = 1; j < i; j++)
        {
            t0 = L_add (t0, Mpy_32 (Rh[j], Rl[j], Ah[i - j], Al[i - j]));
        }
        t0 = L_shl (t0, 4);

        t1 = L_Comp (Rh[i], Rl[i]);
        t0 = L_add (t0, t1);            /* add R[i]        */

        /* K = -t0 / Alpha */

        t1 = L_abs (t0);
        t2 = Div_32 (t1, alp_h, alp_l); /* abs(t0)/Alpha              */

        if (t0 > 0)
            t2 = L_negate (t2);         /* K =-t0/Alpha                */
        t2 = L_shl (t2, alp_exp);       /* denormalize; compare to Alpha */
        L_Extract (t2, &Kh, &Kl);       /* K in DPF                      */


        if (sub (i, 5) < 0)
        {
            rc[i - 1] = round (t2);
        }
        /* Test for unstable filter. If unstable keep old A(z) */


        if (sub (abs_s (Kh), 32750) > 0)
        {
            CODEC_OpVecCpy(A, efr_old_A, EFR_M + 1);

            CODEC_OpVecSet(rc, 4, 0);
            return;
        }
        /*------------------------------------------*
         *  Compute new LPC coeff. -> An[i]         *
         *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
         *  An[i]= K                                *
         *------------------------------------------*/

        for (j = 1; j < i; j++)
        {
            t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
            t0 = L_mac (t0, Ah[j], 32767);
            t0 = L_mac (t0, Al[j], 1);
            L_Extract (t0, &Anh[j], &Anl[j]);
        }
        t2 = L_shr (t2, 4);
        L_Extract (t2, &Anh[i], &Anl[i]);

        /*  Alpha = Alpha * (1-K**2) */

        t0 = Mpy_32 (Kh, Kl, Kh, Kl);           /* K*K             */
        t0 = L_abs (t0);                        /* Some case <0 !! */
        t0 = L_sub ((Word32) 0x7fffffffL, t0);  /* 1 - K*K        */
        L_Extract (t0, &hi, &lo);               /* DPF format      */
        t0 = Mpy_32 (alp_h, alp_l, hi, lo);

        /* Normalize Alpha */

        j = norm_l (t0);
        t0 = L_shl (t0, j);
        L_Extract (t0, &alp_h, &alp_l);         /* DPF format    */
        alp_exp = add (alp_exp, j);             /* Add normalization to
                                                   alp_exp */

        /* A[j] = An[j] */

        CODEC_OpVecCpy(Ah + 1, Anh + 1, i);
        CODEC_OpVecCpy(Al + 1, Anl + 1, i);
    }

    A[0] = 4096;
    for (i = 1; i <= EFR_M; i++)
    {
        t0 = L_Comp (Ah[i], Al[i]);
        efr_old_A[i] = A[i] = round (L_shl (t0, 1));
    }

    return;
}

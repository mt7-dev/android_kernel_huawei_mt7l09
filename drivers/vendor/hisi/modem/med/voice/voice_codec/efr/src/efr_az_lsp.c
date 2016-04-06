/***********************************************************************
 *
 *  FUNCTION:  Az_lsp
 *
 *  PURPOSE:   Compute the LSPs from  the LP coefficients  (order=10)
 *
 *  DESCRIPTION:
 *    - The sum and difference filters are computed and divided by
 *      1+z^{-1}   and   1-z^{-1}, respectively.
 *
 *         f1[i] = a[i] + a[11-i] - f1[i-1] ;   i=1,...,5
 *         f2[i] = a[i] - a[11-i] + f2[i-1] ;   i=1,...,5
 *
 *    - The roots of F1(z) and F2(z) are found using Chebyshev polynomial
 *      evaluation. The polynomials are evaluated at 60 points regularly
 *      spaced in the frequency domain. The sign change interval is
 *      subdivided 4 times to better track the root.
 *      The LSPs are found in the cosine domain [1,-1].
 *
 *    - If less than 10 roots are found, the LSPs from the past frame are
 *      used.
 *
 ***********************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_cnst.h"

#include "efr_grid_tab.c"

/* EFR_M = LPC order, NC = M/2 */

#define NC   EFR_M/2

/* local function */

static Word16 EFR_Chebps (Word16 x, Word16 f[], Word16 n);

void EFR_Az_lsp (
    Word16 a[],         /* (i)     : predictor coefficients                 */
    Word16 lsp[],       /* (o)     : line spectral pairs                    */
    Word16 old_lsp[]    /* (i)     : old lsp[] (in case not found 10 roots) */
)
{
    Word16 i, j, nf, ip;
    Word16 xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
    Word16 x, y, sign, exp;
    Word16 *coef;
    Word16 f1[EFR_M / 2 + 1], f2[EFR_M / 2 + 1];
    Word32 t0;

    /*-------------------------------------------------------------*
     *  find the sum and diff. pol. F1(z) and F2(z)                *
     *    F1(z) <--- F1(z)/(1+z**-1) & F2(z) <--- F2(z)/(1-z**-1)  *
     *                                                             *
     * f1[0] = 1.0;                                                *
     * f2[0] = 1.0;                                                *
     *                                                             *
     * for (i = 0; i< NC; i++)                                     *
     * {                                                           *
     *   f1[i+1] = a[i+1] + a[EFR_M-i] - f1[i] ;                       *
     *   f2[i+1] = a[i+1] - a[EFR_M-i] + f2[i] ;                       *
     * }                                                           *
     *-------------------------------------------------------------*/

    f1[0] = 1024;                  /* f1[0] = 1.0 */
    f2[0] = 1024;                  /* f2[0] = 1.0 */

    for (i = 0; i < NC; i++)
    {
        t0 = L_mult (a[i + 1], 8192);   /* x = (a[i+1] + a[M-i]) >> 2  */
        t0 = L_mac (t0, a[EFR_M - i], 8192);
        x = extract_h (t0);
        /* f1[i+1] = a[i+1] + a[EFR_M-i] - f1[i] */
        f1[i + 1] = sub (x, f1[i]);

        t0 = L_mult (a[i + 1], 8192);   /* x = (a[i+1] - a[EFR_M-i]) >> 2 */
        t0 = L_msu (t0, a[EFR_M - i], 8192);
        x = extract_h (t0);
        /* f2[i+1] = a[i+1] - a[EFR_M-i] + f2[i] */
        f2[i + 1] = add (x, f2[i]);
    }

    /*-------------------------------------------------------------*
     * find the LSPs using the Chebychev pol. evaluation           *
     *-------------------------------------------------------------*/

    nf = 0;                        /* number of found frequencies */
    ip = 0;                        /* indicator for f1 or f2      */

    coef = f1;

    xlow = grid[0];
    ylow = EFR_Chebps (xlow, coef, NC);

    j = 0;

    /* while ( (nf < EFR_M) && (j < grid_points) ) */
    while ((sub (nf, EFR_M) < 0) && (sub (j, grid_points) < 0))
    {
        j++;
        xhigh = xlow;
        yhigh = ylow;
        xlow = grid[j];
        ylow = EFR_Chebps (xlow, coef, NC);



        if (L_mult (ylow, yhigh) <= (Word32) 0L)
        {

            /* divide 4 times the interval */

            for (i = 0; i < 4; i++)
            {
                /* xmid = (xlow + xhigh)/2 */
                xmid = add (shr (xlow, 1), shr (xhigh, 1));
                ymid = EFR_Chebps (xmid, coef, NC);



                if (L_mult (ylow, ymid) <= (Word32) 0L)
                {
                    yhigh = ymid;
                    xhigh = xmid;
                }
                else
                {
                    ylow = ymid;
                    xlow = xmid;
                }
            }

            /*-------------------------------------------------------------*
             * Linear interpolation                                        *
             *    xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);            *
             *-------------------------------------------------------------*/

            x = sub (xhigh, xlow);
            y = sub (yhigh, ylow);


            if (y == 0)
            {
                xint = xlow;
            }
            else
            {
                sign = y;
                y = abs_s (y);
                exp = norm_s (y);
                y = shl (y, exp);
                y = div_s ((Word16) 16383, y);
                t0 = L_mult (x, y);
                t0 = L_shr (t0, sub (20, exp));
                y = extract_l (t0);     /* y= (xhigh-xlow)/(yhigh-ylow) */


                if (sign < 0)
                    y = negate (y);

                t0 = L_mult (ylow, y);
                t0 = L_shr (t0, 11);
                xint = sub (xlow, extract_l (t0)); /* xint = xlow - ylow*y */
            }

            lsp[nf] = xint;
            xlow = xint;
            nf++;


            if (ip == 0)
            {
                ip = 1;
                coef = f2;
            }
            else
            {
                ip = 0;
                coef = f1;
            }
            ylow = EFR_Chebps (xlow, coef, NC);


        }

    }

    /* Check if EFR_M roots found */


    if (sub (nf, EFR_M) < 0)
    {
        CODEC_OpVecCpy(lsp, old_lsp, EFR_M);
    }
    return;
}

/************************************************************************
 *
 *  FUNCTION:  Chebps
 *
 *  PURPOSE:   Evaluates the Chebyshev polynomial series
 *
 *  DESCRIPTION:
 *  - The polynomial order is   n = m/2 = 5
 *  - The polynomial F(z) (F1(z) or F2(z)) is given by
 *     F(w) = 2 exp(-j5w) C(x)
 *    where
 *      C(x) = T_n(x) + f(1)T_n-1(x) + ... +f(n-1)T_1(x) + f(n)/2
 *    and T_m(x) = cos(mw) is the mth order Chebyshev polynomial ( x=cos(w) )
 *  - The function returns the value of C(x) for the input x.
 *
 ***********************************************************************/

static Word16 EFR_Chebps (Word16 x, Word16 f[], Word16 n)
{
    Word16 i, cheb;
    Word16 b0_h, b0_l, b1_h, b1_l, b2_h, b2_l;
    Word32 t0;

    b2_h = 256;                    /* b2 = 1.0 */
    b2_l = 0;

    t0 = L_mult (x, 512);          /* 2*x                 */
    t0 = L_mac (t0, f[1], 8192);   /* + f[1]              */
    L_Extract (t0, &b1_h, &b1_l);  /* b1 = 2*x + f[1]     */

    for (i = 2; i < n; i++)
    {
        t0 = Mpy_32_16 (b1_h, b1_l, x);         /* t0 = 2.0*x*b1        */
        t0 = L_shl (t0, 1);
        t0 = L_mac (t0, b2_h, (Word16) 0x8000); /* t0 = 2.0*x*b1 - b2   */
        t0 = L_msu (t0, b2_l, 1);
        t0 = L_mac (t0, f[i], 8192);            /* t0 = 2.0*x*b1 - b2 + f[i] */

        L_Extract (t0, &b0_h, &b0_l);           /* b0 = 2.0*x*b1 - b2 + f[i]*/

        b2_l = b1_l;                 /* b2 = b1; */
        b2_h = b1_h;
        b1_l = b0_l;                 /* b1 = b0; */
        b1_h = b0_h;
    }

    t0 = Mpy_32_16 (b1_h, b1_l, x);             /* t0 = x*b1; */
    t0 = L_mac (t0, b2_h, (Word16) 0x8000);     /* t0 = x*b1 - b2   */
    t0 = L_msu (t0, b2_l, 1);
    t0 = L_mac (t0, f[i], 4096);                /* t0 = x*b1 - b2 + f[i]/2 */

    t0 = L_shl (t0, 6);

    cheb = extract_h (t0);

    return (cheb);
}

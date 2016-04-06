/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : az_lsp.c
*      Purpose          : Compute the LSPs from the LP coefficients
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "az_lsp.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "grid_tab.c"
#define NC   M/2                  /* M = LPC order, NC = M/2 */

/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Chebps
*  Purpose     : Evaluates the Chebyshev polynomial series
*  Description : - The polynomial order is   n = m/2 = 5
*                - The polynomial F(z) (F1(z) or F2(z)) is given by
*                   F(w) = 2 exp(-j5w) C(x)
*                  where
*                   C(x) = T_n(x) + f(1)T_n-1(x) + ... +f(n-1)T_1(x) + f(n)/2
*                  and T_m(x) = cos(mw) is the mth order Chebyshev
*                  polynomial ( x=cos(w) )
*  Returns     : C(x) for the input x.
*
**************************************************************************
*/
static Word16 Chebps (Word16 x,
                      Word16 f[], /* (n) */
                      Word16 n)
{
    Word32 i;
    Word16 cheb, b0_h, b0_l, b1_h, b1_l, b2_h, b2_l;
    Word32 t0;

    b2_h = 256;                     /* b2 = 1.0 */
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

        b2_l = b1_l;                  /* b2 = b1; */
        b2_h = b1_h;
        b1_l = b0_l;                  /* b1 = b0; */
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

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Az_lsp
*  Purpose     : Compute the LSPs from  the LP coefficients
*
**************************************************************************
*/
void Az_lsp (
    Word16 a[],         /* (i)  : predictor coefficients (MP1)               */
    Word16 lsp[],       /* (o)  : line spectral pairs (M)                    */
    Word16 old_lsp[]    /* (i)  : old lsp[] (in case not found 10 roots) (M) */
)
{
    Word16 i, j, nf, ip;
    Word16 xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
    Word16 x, y, sign, exp;
    Word16 *coef;
    Word16 f1[M / 2 + 1], f2[M / 2 + 1];
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
     *   f1[i+1] = a[i+1] + a[M-i] - f1[i] ;                       *
     *   f2[i+1] = a[i+1] - a[M-i] + f2[i] ;                       *
     * }                                                           *
     *-------------------------------------------------------------*/

    f1[0] = 1024;                   /* f1[0] = 1.0 */
    f2[0] = 1024;                   /* f2[0] = 1.0 */

    for (i = 0; i < NC; i++)
    {
        t0 = L_mult (a[i + 1], 8192);   /* x = (a[i+1] + a[M-i]) >> 2  */
        t0 = L_mac (t0, a[M - i], 8192);
        x = extract_h (t0);
        /* f1[i+1] = a[i+1] + a[M-i] - f1[i] */
        f1[i + 1] = sub (x, f1[i]);

        t0 = L_mult (a[i + 1], 8192);   /* x = (a[i+1] - a[M-i]) >> 2 */
        t0 = L_msu (t0, a[M - i], 8192);
        x = extract_h (t0);
        /* f2[i+1] = a[i+1] - a[M-i] + f2[i] */
        f2[i + 1] = add (x, f2[i]);
    }

    /*-------------------------------------------------------------*
     * find the LSPs using the Chebychev pol. evaluation           *
     *-------------------------------------------------------------*/

    nf = 0;                         /* number of found frequencies */
    ip = 0;                         /* indicator for f1 or f2      */

    coef = f1;

    xlow = grid[0];
    ylow = Chebps (xlow, coef, NC);

    j = 0;

    /* while ( (nf < M) && (j < grid_points) ) */
    while (( nf < (Word16)M ) && ( j < (Word16)grid_points))
    {
        j++;
        xhigh = xlow;
        yhigh = ylow;
        xlow = grid[j];
        ylow = Chebps (xlow, coef, NC);

        if (L_mult (ylow, yhigh) <= (Word32) 0L)
        {

            /* divide 4 times the interval */

            for (i = 0; i < 4; i++)
            {
                /* xmid = (xlow + xhigh)/2 */
                xmid = add (shr (xlow, 1), shr (xhigh, 1));
                ymid = Chebps (xmid, coef, NC);

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
            ylow = Chebps (xlow, coef, NC);

        }

    }

    /* Check if M roots found */
    /*if (sub (nf, M) < 0)*/
    if ( nf < (Word16)M )
    {
        CODEC_OpVecCpy(&lsp[0], &old_lsp[0], M);
    }
    return;
}


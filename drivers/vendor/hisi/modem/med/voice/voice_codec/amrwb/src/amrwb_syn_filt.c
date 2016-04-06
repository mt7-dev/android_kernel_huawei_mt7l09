/*-------------------------------------------------------------------*
 *                         SYN_FILT.C                                *
 *-------------------------------------------------------------------*
 * Do the synthesis filtering 1/A(z).                                *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_math_op.h"
#include "amrwb_cnst.h"

#ifdef _MED_C89_

void AMRWB_Syn_filt(
     Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients           */
     Word16 m,                             /* (i)     : order of LP filter                       */
     Word16 x[],                           /* (i)     : input signal                             */
     Word16 y[],                           /* (o)     : output signal                            */
     Word16 lg,                            /* (i)     : size of filtering                        */
     Word16 mem[],                         /* (i/o)   : memory associated with this filtering.   */
     Word16 update                         /* (i)     : 0=no update, 1=update of memory.         */
)
{
    Word16 y_buf[L_SUBFR16k + M16k], a0, s;
    Word32 i, L_tmp;
    Word16 *yy;

    Word32 a32[10];
    Word16 *a16 = (Word16 *)a32;

    if (0 == (((Word32)a) & 0x03))
    {
        CODEC_OpVecCpy(a16, &a[1], m);
    }
    else
    {
        a16 = &a[1];
    }

    yy = &y_buf[0];

    /* copy initial filter states into synthesis buffer */
    /*for (i = 0; i < m; i++)
    {
        *yy++ = mem[i];
    }*/
    CODEC_OpVecCpy(yy, mem, m);
    yy += m;

    s = sub(norm_s(a[0]), 2);
    a0 = shr(a[0], 1);                     /* input / 2 */

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        L_tmp = L_mult(x[i], a0);

        /*for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, a[j], yy[i - j]);*/
        L_tmp = CODEC_OpVvMsuD(a16, &yy[i-1], m, L_tmp);

        L_tmp = L_shl(L_tmp, (3 + s)/*add(3, s)*/);

        yy[i] = round(L_tmp);
    }

    CODEC_OpVecCpy(y, yy, lg);

    /* Update memory if required */
    if (update)
        /*for (i = 0; i < m; i++)
        {
            mem[i] = yy[lg - m + i];
        }*/
        CODEC_OpVecCpy(mem, &yy[lg - m], m);

    return;
}


void Syn_filt_32(
     Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients */
     Word16 m,                             /* (i)     : order of LP filter             */
     Word16 exc[],                         /* (i) Qnew: excitation (exc[i] >> Qnew)    */
     Word16 Qnew,                          /* (i)     : exc scaling = 0(min) to 8(max) */
     Word16 sig_hi[],                      /* (o) /16 : synthesis high                 */
     Word16 sig_lo[],                      /* (o) /16 : synthesis low                  */
     Word16 lg                             /* (i)     : size of filtering              */
)
{
    Word16 a0, s;
    Word32 i, L_tmp;

    s = sub(norm_s(a[0]), 2);

    a0 = shr(a[0], add(4, Qnew));          /* input / 16 and >>Qnew */

    /* Do the filtering. */

    for (i = 0; i < lg; i++)
    {
        L_tmp = 0;
        /*for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, sig_lo[i - j], a[j]);*/
        L_tmp = CODEC_OpVvMsuD(&a[1], &sig_lo[i-1], m, L_tmp);

        L_tmp = L_shr(L_tmp, 12);      /* -4 : sig_lo[i] << 4 */

        L_tmp = L_mac(L_tmp, exc[i], a0);

        /*for (j = 1; j <= m; j++)
            L_tmp = L_msu(L_tmp, sig_hi[i - j], a[j]);*/
        L_tmp = CODEC_OpVvMsuD(&a[1], &sig_hi[i-1], m, L_tmp);

        /* sig_hi = bit16 to bit31 of synthesis */
        L_tmp = L_shl(L_tmp, (3 + s)/*add(3, s)*/);           /* ai in Q12 */
        sig_hi[i] = extract_h(L_tmp);

        /* sig_lo = bit4 to bit15 of synthesis */
        L_tmp = L_shr(L_tmp, 4);           /* 4 : sig_lo[i] >> 4 */
        sig_lo[i] = extract_l(L_msu(L_tmp, sig_hi[i], 2048));
    }

    return;
}
#endif

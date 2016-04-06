/*-------------------------------------------------------------------*
 *                         SYN_FILT.C                                *
 *-------------------------------------------------------------------*
 * Do the synthesis filtering 1/A(z).                                *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_math_op.h"
#include "amrwb_cnst.h"

#ifndef _MED_C89_

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
    Word16 i, j, a0, s;
    Word32 y_buf[((L_SUBFR16k + M16k)/2)+1];
    Word32 k=0;
    Word16 *yy;

    ae_p24x2s aep_mem1, aep_mem2, aep_mem3, aep_mem4, aep_yy;
    ae_p24x2s aep_a, aep_a0, aep_x, aep_a1;
    ae_q56s   aeq_tmp, aeq_L_tmp0, aeq_L_tmp1;

    yy = (Word16*)&y_buf[0];
    /* intend to do so */
    yy++;

    /* copy initial filter states into synthesis buffer */
    /* for (i = 0; i < m; i++)
    {
        *yy++ = mem[i];
    } */

    aep_mem1 = *((ae_p16s *)&mem[0]);
    aep_mem2 = *((ae_p16s *)&mem[1]);

    for (k; k < (m>>1); k++)
    {
        *((ae_p16s *)yy++) = aep_mem1;
        *((ae_p16s *)yy++) = aep_mem2;

        aep_mem1 = *((ae_p16s *)&mem[2*k + 2]);
        aep_mem2 = *((ae_p16s *)&mem[2*k + 3]);
    }

    aep_a = *((ae_p16s *)a);
    /* s = norm_s(a[0]) + 1; */
    aeq_tmp = AE_CVTQ48P24S_L(aep_a);
    s = AE_NSAQ56S(aeq_tmp) - 7;

    /* a0 = shr(a[0], 1); */                    /* input / 2 */
    aep_a0 = AE_SRAIP24(aep_a, 1);

    /* load a[1] only one time */
    aep_a1 = *((ae_p16s *)&a[1]);

    /* preload yy */
    aep_yy = *((ae_p16s *)&yy[-1]);

    /* Do the filtering. */
    for (i = 0; i < (lg>>1); i++)
    {
        /* L_tmp0 = L_mult(x[i],     a0); */
        /* L_tmp1 = L_mult(x[i + 1], a0); */
        aep_x = *((ae_p16s *)&x[2*i]);
        aeq_L_tmp0 = AE_MULFS32P16S_LL(aep_x, aep_a0);
        aep_x = *((ae_p16s *)&x[2*i + 1]);
        aeq_L_tmp1 = AE_MULFS32P16S_LL(aep_x, aep_a0);

        /* L_tmp0 = L_msu(L_tmp0, a[1], yy[i - 1]);*/
        AE_MULSFS32P16S_LL(aeq_L_tmp0, aep_a1, aep_yy);

        for (j = 1; j < (m>>1); j++)
        {
            aep_a = *((ae_p16x2s *)&a[2*j]);

            /* L_tmp1 = L_msu(L_tmp1, a[j], yy[i + 1 - j]); */
            AE_MULSFS32P16S_HH(aeq_L_tmp1, aep_a, aep_yy);

            aep_yy = *((ae_p16x2s *)&yy[2*i - (2*j + 1)]);

            /* L_tmp0 = L_msu(L_tmp0, a[j], yy[i - j]); */
            /* L_tmp0 = L_msu(L_tmp0, a[j+1], yy[i - (j+1)]); */
            AE_MULSSFP24S_HL_LH(aeq_L_tmp0, aep_a, aep_yy);

            /* L_tmp1 = L_msu(L_tmp1, a[j + 1], yy[i - j]); */
            AE_MULSFS32P16S_LL(aeq_L_tmp1, aep_a, aep_yy);
        }

        aep_a = *((ae_p16s *)&a[m]);
        AE_MULSFS32P16S_LH(aeq_L_tmp1, aep_a, aep_yy);
        aep_yy = *((ae_p16s *)&yy[2*i - m]);
        AE_MULSFS32P16S_LL(aeq_L_tmp0, aep_a, aep_yy);

        /* L_tmp0 = L_shl(L_tmp0, s); */
        aeq_L_tmp0 = AE_SLLASQ56S(aeq_L_tmp0, s);
        aeq_L_tmp0 = AE_SATQ48S(aeq_L_tmp0);

        /* y[i]   = yy[i]   = round(L_tmp0); */
        aep_yy = AE_ROUNDSP16Q48ASYM(aeq_L_tmp0);
        *((ae_p16s *)&y[2*i])  = aep_yy;
        *((ae_p16s *)&yy[2*i]) = aep_yy;
        aep_yy = AE_TRUNCP16(aep_yy);

        /* L_tmp1 = L_msu(L_tmp1, a[1], yy[i]); */
        AE_MULSFS32P16S_LL(aeq_L_tmp1, aep_a1, aep_yy);

        /* L_tmp1 = L_shl(L_tmp1, s); */
        aeq_L_tmp1 = AE_SLLASQ56S(aeq_L_tmp1, s);
        aeq_L_tmp1 = AE_SATQ48S(aeq_L_tmp1);

        /* y[i+1]   = yy[i+1]   = round(L_tmp1); */
        aep_yy = AE_ROUNDSP16Q48ASYM(aeq_L_tmp1);
        *((ae_p16s *)&y[2*i + 1])  = aep_yy;
        *((ae_p16s *)&yy[2*i + 1]) = aep_yy;
        aep_yy = AE_TRUNCP16(aep_yy);
    }

    /* Update memory if required */
    if (update)
    {
        for (i = 0; i < m; i++)
        {
            mem[i] = yy[lg - m + i];
        }
    }

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

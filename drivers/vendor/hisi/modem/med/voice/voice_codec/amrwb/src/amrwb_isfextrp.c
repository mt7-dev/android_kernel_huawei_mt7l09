/*-----------------------------------------------------------------------*
 *                         ISFEXTRP.C                                    *
 *-----------------------------------------------------------------------*
 *  Conversion of 16th-order 12.8kHz ISF vector                          *
 *  into 20th-order 16kHz ISF vector                                     *
 *-----------------------------------------------------------------------*/

#include "amrwb_cnst.h"
#include "amrwb_acelp.h"

#define INV_LENGTH 2731                    /* 1/12 */

void Isf_Extrapolation(Word16 HfIsf[])
{
    Word32 IsfDiff32[AMRWB_M/2 - 1];
    Word16 *IsfDiff = (Word16 *)IsfDiff32;
    Word32 IsfCorr[3];
    Word32 i, L_tmp;
    Word16 coeff, mean, tmp, tmp2, tmp3;
    Word16 exp, exp2, hi, lo;
    Word16 MaxCorr;

    HfIsf[M16k - 1] = HfIsf[AMRWB_M - 1];

    /* Difference vector */
    /*for (i = 1; i < (M - 1); i++)
    {
        IsfDiff[i - 1] = sub(HfIsf[i], HfIsf[i - 1]);
    }*/
    CODEC_OpVvSub(&HfIsf[1], HfIsf, AMRWB_M - 2, IsfDiff);

    L_tmp = 0;

    /* Mean of difference vector */
    L_tmp = CODEC_OpVcMac(&IsfDiff[2], AMRWB_M-4, INV_LENGTH, L_tmp);

    mean = round(L_tmp);

    IsfCorr[0] = 0;
    IsfCorr[1] = 0;
    IsfCorr[2] = 0;

    /*tmp = 0;
    for (i = 0; i < (M - 2); i++)
    {
        if (sub(IsfDiff[i], tmp) > 0)
        {
            tmp = IsfDiff[i];
        }
    }*/
    tmp = CODEC_OpVecMax(IsfDiff, (AMRWB_M - 2), 0);
    if (tmp < 0)
    {
        tmp = 0;
    }

    exp = norm_s(tmp);
    /*for (i = 0; i < (M - 2); i++)
    {
        IsfDiff[i] = shl(IsfDiff[i], exp);
    }*/
    CODEC_OpVecShl(IsfDiff, (AMRWB_M - 2), exp, IsfDiff);

    mean = shl(mean, exp);
    for (i = 7; i < (AMRWB_M - 2); i++)
    {
        tmp2 = sub(IsfDiff[i], mean);
        tmp3 = sub(IsfDiff[i - 2], mean);
        L_tmp = L_mult(tmp2, tmp3);
        L_Extract(L_tmp, &hi, &lo);
        L_tmp = Mpy_32(hi, lo, hi, lo);
        IsfCorr[0] = L_add(IsfCorr[0], L_tmp);
    }
    for (i = 7; i < (AMRWB_M - 2); i++)
    {
        tmp2 = sub(IsfDiff[i], mean);
        tmp3 = sub(IsfDiff[i - 3], mean);
        L_tmp = L_mult(tmp2, tmp3);
        L_Extract(L_tmp, &hi, &lo);
        L_tmp = Mpy_32(hi, lo, hi, lo);
        IsfCorr[1] = L_add(IsfCorr[1], L_tmp);
    }
    for (i = 7; i < (AMRWB_M - 2); i++)
    {
        tmp2 = sub(IsfDiff[i], mean);
        tmp3 = sub(IsfDiff[i - 4], mean);
        L_tmp = L_mult(tmp2, tmp3);
        L_Extract(L_tmp, &hi, &lo);
        L_tmp = Mpy_32(hi, lo, hi, lo);
        IsfCorr[2] = L_add(IsfCorr[2], L_tmp);
    }
    if (L_sub(IsfCorr[0], IsfCorr[1]) > 0)
    {
        MaxCorr = 0;
    } else
    {
        MaxCorr = 1;
    }

    if (L_sub(IsfCorr[2], IsfCorr[MaxCorr]) > 0)
        MaxCorr = 2;

    MaxCorr = add(MaxCorr, 1);             /* Maximum correlation of difference vector */

    for (i = AMRWB_M - 1; i < (M16k - 1); i++)
    {
        tmp = sub(HfIsf[i - 1 - MaxCorr], HfIsf[i - 2 - MaxCorr]);
        HfIsf[i] = add(HfIsf[i - 1], tmp);
    }

    /* tmp=7965+(HfIsf[2]-HfIsf[3]-HfIsf[4])/6; */
    tmp = add(HfIsf[4], HfIsf[3]);
    tmp = sub(HfIsf[2], tmp);
    tmp = mult(tmp, 5461);
    tmp = add(tmp, 20390);

    if (sub(tmp, 19456) > 0)
    {                                      /* Maximum value of ISF should be at most 7600 Hz */
        tmp = 19456;
    }
    tmp = sub(tmp, HfIsf[AMRWB_M - 2]);
    tmp2 = sub(HfIsf[M16k - 2], HfIsf[AMRWB_M - 2]);

    exp2 = norm_s(tmp2);
    exp = norm_s(tmp);
    exp = sub(exp, 1);
    tmp = shl(tmp, exp);
    tmp2 = shl(tmp2, exp2);
    coeff = div_s(tmp, tmp2);              /* Coefficient for stretching the ISF vector */
    exp = sub(exp2, exp);

    for (i = AMRWB_M - 1; i < (M16k - 1); i++)
    {
        tmp = mult(sub(HfIsf[i], HfIsf[i - 1]), coeff);
        IsfDiff[i - (AMRWB_M - 1)] = shl(tmp, exp);
    }

    for (i = AMRWB_M; i < (M16k - 1); i++)
    {
        /* The difference between ISF(n) and ISF(n-2) should be at least 500 Hz */
        tmp = sub(add(IsfDiff[i - (AMRWB_M - 1)], IsfDiff[i - AMRWB_M]), 1280);
        if (tmp < 0)
        {
            if (sub(IsfDiff[i - (AMRWB_M - 1)], IsfDiff[i - AMRWB_M]) > 0)
            {
                IsfDiff[i - AMRWB_M] = sub(1280, IsfDiff[i - (AMRWB_M - 1)]);
            } else
            {
                IsfDiff[i - (AMRWB_M - 1)] = sub(1280, IsfDiff[i - AMRWB_M]);
            }
        }
    }

    for (i = AMRWB_M - 1; i < (M16k - 1); i++)
    {
        HfIsf[i] = add(HfIsf[i - 1], IsfDiff[i - (AMRWB_M - 1)]);
    }

    /*for (i = 0; i < (M16k - 1); i++)
    {
        HfIsf[i] = mult(HfIsf[i], 26214); // Scale the ISF vector correctly for 16000 kHz
    }*/
    CODEC_OpVcMult(HfIsf, (M16k - 1), 26214, HfIsf);

    Isf_isp(HfIsf, HfIsf, M16k);

    return;
}

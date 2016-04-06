/*-----------------------------------------------------------------------*
 *                         HP_WSP.C										 *
 *-----------------------------------------------------------------------*
 *                                                                       *
 * 3nd order high pass filter with cut off frequency at 180 Hz           *
 *                                                                       *
 * Algorithm:                                                            *
 *                                                                       *
 *  y[i] = b[0]*x[i] + b[1]*x[i-1] + b[2]*x[i-2] + b[3]*x[i-3]           *
 *                   + a[1]*y[i-1] + a[2]*y[i-2] + a[3]*y[i-3];          *
 *                                                                       *
 * float a_coef[HP_ORDER]= {                                             *
 *    -2.64436711600664f,                                                *
 *    2.35087386625360f,                                                 *
 *   -0.70001156927424f};                                                *
 *                                                                       *
 * float b_coef[HP_ORDER+1]= {                                           *
 *     -0.83787057505665f,                                               *
 *    2.50975570071058f,                                                 *
 *   -2.50975570071058f,                                                 *
 *    0.83787057505665f};                                                *
 *                                                                       *
 *-----------------------------------------------------------------------*/

#include "amrwb_acelp.h"

#ifndef _MED_C89_
/* filter coefficients in Q12 */

static Word16 a[4] = {8192, 21663, -19258, 5734};
static Word16 b[4] = {-3432, +10280, -10280, +3432};


/* Initialization of static values */

void Init_Hp_wsp(Word16 mem[])
{
    CODEC_OpVecSet(mem, 9, 0);

    return;
}

void scale_mem_Hp_wsp(Word16 mem[], Word16 exp)
{
    Word32 i, L_tmp;

    for (i = 0; i < 6; i += 2)
    {
        L_tmp = L_Comp(mem[i], mem[i + 1]);/* y_hi, y_lo */
        L_tmp = L_shl(L_tmp, exp);
        L_Extract(L_tmp, &mem[i], &mem[i + 1]);
    }

    for (i = 6; i < 9; i++)
    {
        L_tmp = L_deposit_h(mem[i]);       /* x[i] */
        L_tmp = L_shl(L_tmp, exp);
        mem[i] = round(L_tmp);
    }

    return;
}

void Hp_wsp(
     Word16 wsp[],                         /* i   : wsp[]  signal       */
     Word16 hp_wsp[],                      /* o   : hypass wsp[]        */
     Word16 lg,                            /* i   : lenght of signal    */
     Word16 mem[]                          /* i/o : filter memory [9]   */
)
{
    Word16 *p;
    Word32 i;
    ae_p24x2s aep_x0, aep_x1, aep_x2, aep_x3;
    ae_p24x2s aep_y3_hi, aep_y3_lo, aep_y2_hi, aep_y2_lo, aep_y1_hi, aep_y1_lo, aep_tmp;
    ae_q56s   aeq_L_tmp, aeq_tmp;

    p = mem;

    aep_y3_hi = *((ae_p16s *)p++);
    aep_y3_lo = *((ae_p16s *)p++);
    aep_y2_hi = *((ae_p16s *)p++);
    aep_y2_lo = *((ae_p16s *)p++);
    aep_y1_hi = *((ae_p16s *)p++);
    aep_y1_lo = *((ae_p16s *)p++);

    aep_x0 = *((ae_p16s *)p++);
    aep_x1 = *((ae_p16s *)p++);
    aep_x2 = *((ae_p16s *)p++);

    for (i = 0; i < lg; i++)
    {
        aep_x3 = AE_MOVP48(aep_x2);
        aep_x2 = AE_MOVP48(aep_x1);
        aep_x1 = AE_MOVP48(aep_x0);

        aep_x0 = *((ae_p16s *)&wsp[i]);

        aeq_L_tmp = AE_CVTQ48A32S(16384L);

        p = &a[1];
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_y1_lo, aep_tmp);
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_y2_lo, aep_tmp);
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_y3_lo, aep_tmp);

        aeq_L_tmp = AE_SRAIQ56(aeq_L_tmp, 15);

        p = &a[1];
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_y1_hi, aep_tmp);
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_y2_hi, aep_tmp);
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_y3_hi, aep_tmp);

        p = b;
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_x0, aep_tmp);
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_x1, aep_tmp);
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_x2, aep_tmp);
        aep_tmp = *((ae_p16s *)p++);
        AE_MULAFS32P16S_LL(aeq_L_tmp, aep_x3, aep_tmp);

        aeq_L_tmp = AE_SLLISQ56S(aeq_L_tmp, 2);
        aeq_L_tmp = AE_SATQ48S(aeq_L_tmp);

        aep_y3_hi = AE_MOVP48(aep_y2_hi);
        aep_y3_lo = AE_MOVP48(aep_y2_lo);
        aep_y2_hi = AE_MOVP48(aep_y1_hi);
        aep_y2_lo = AE_MOVP48(aep_y1_lo);

        aep_y1_hi = AE_TRUNCP24Q48(aeq_L_tmp);
        aep_y1_hi = AE_TRUNCP16(aep_y1_hi);
        aeq_tmp = AE_SRAIQ56(aeq_L_tmp, 1);
        aep_tmp = AE_CVTP24A16(16384);
        AE_MULSFS32P16S_LL(aeq_tmp, aep_y1_hi, aep_tmp);
        aeq_tmp = AE_SLLIQ56(aeq_tmp, 16);
        aep_y1_lo = AE_TRUNCP24Q48(aeq_tmp);

        aeq_L_tmp = AE_SLLISQ56S(aeq_L_tmp, 1);
        aeq_L_tmp = AE_SATQ48S(aeq_L_tmp);

        aep_tmp = AE_ROUNDSP16Q48ASYM(aeq_L_tmp);
        *((ae_p16s *)&hp_wsp[i]) = aep_tmp;
    }

    p = mem;

    *((ae_p16s *)p++) = aep_y3_hi;
    *((ae_p16s *)p++) = aep_y3_lo;
    *((ae_p16s *)p++) = aep_y2_hi;
    *((ae_p16s *)p++) = aep_y2_lo;
    *((ae_p16s *)p++) = aep_y1_hi;
    *((ae_p16s *)p++) = aep_y1_lo;
    *((ae_p16s *)p++) = aep_x0;
    *((ae_p16s *)p++) = aep_x1;
    *((ae_p16s *)p++) = aep_x2;

    return;
}

#endif

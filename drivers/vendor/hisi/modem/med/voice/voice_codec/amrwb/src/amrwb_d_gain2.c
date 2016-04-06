/*-------------------------------------------------------------------*
 *                         D_GAIN2.C                                 *
 *-------------------------------------------------------------------*
 * Decode the pitch and codebook gains                               *
 *-------------------------------------------------------------------*/

#include "amrwb_math_op.h"
#include "amrwb_log2.h"
#include "amrwb_cnst.h"
#include "amrwb_acelp.h"
#include "amrwb_q_gain2_tab.h"

#define MEAN_ENER    30
#define PRED_ORDER   4

#define L_LTPHIST 5

const Word16 pdown_unusable[7] = {32767, 31130, 29491, 24576, 7537, 1638, 328};
const Word16 cdown_unusable[7] = {32767, 16384, 8192, 8192, 8192, 4915, 3277};

const Word16 pdown_usable[7] = {32767, 32113, 31457, 24576, 7537, 1638, 328};
const Word16 cdown_usable[7] = {32767, 32113, 32113, 32113, 32113, 32113, 22938};


/* MA prediction coeff ={0.5, 0.4, 0.3, 0.2} in Q13 */
static Word16 pred[PRED_ORDER] = {4096, 3277, 2458, 1638};

void Init_D_gain2(
     Word16 * mem                          /* output  :static memory (4 words)      */
)
{
    /* 4nd order quantizer energy predictor (init to -14.0 in Q10) */

    CODEC_OpVecSet(mem, 4, -14336);
    CODEC_OpVecSet(&mem[4], 18, 0);

    mem[22] = 21845;                       /* seed */

    return;
}


void D_gain2(
     Word16 index,                         /* (i)     : index of quantization.      */
     Word16 nbits,                         /* (i)     : number of bits (6 or 7)     */
     Word16 code[],                        /* (i) Q9  : Innovative vector.          */
     Word16 L_subfr,                       /* (i)     : Subframe lenght.            */
     Word16 * gain_pit,                    /* (o) Q14 : Pitch gain.                 */
     Word32 * gain_cod,                    /* (o) Q16 : Code gain.                  */
     Word16 bfi,                           /* (i)     : bad frame indicator         */
     Word16 prev_bfi,                      /* (i)     : Previous BF indicator       */
     Word16 state,                         /* (i)     : State of BFH                */
     Word16 unusable_frame,                /* (i)     : UF indicator                */
     Word16 vad_hist,                      /* (i)     : number of non-speech frames */
     Word16 * mem                          /* (i/o)   : static memory (4 words)     */
)
{
    Word16 *p, *past_gain_pit, *past_gain_code, *past_qua_en, *gbuf, *pbuf, *prev_gc;
    Word16 *pbuf2, *seed;
    Word16 tmp, exp, frac, gcode0, exp_gcode0, qua_ener, gcode_inov;
    Word16 g_code;
    Word32 i, L_tmp;

    past_qua_en = mem;
    past_gain_pit = mem + 4;
    past_gain_code = mem + 5;
    prev_gc = mem + 6;
    pbuf = mem + 7;
    gbuf = mem + 12;
    pbuf2 = mem + 17;
    seed = mem + 22;

    /*-----------------------------------------------------------------*
     *  Find energy of code and compute:                               *
     *                                                                 *
     *    L_tmp = 1.0 / sqrt(energy of code/ L_subfr)                  *
     *-----------------------------------------------------------------*/

    L_tmp = Dot_product12(code, code, L_subfr, &exp);
    exp = sub(exp, 18 + 6);                /* exp: -18 (code in Q9), -6 (/L_subfr) */

    Isqrt_n(&L_tmp, &exp);

    gcode_inov = extract_h(L_shl(L_tmp, sub(exp, 3)));  /* g_code_inov in Q12 */

    /*-------------------------------*
     * Case of erasure.              *
     *-------------------------------*/
    if (bfi != 0)
    {
        tmp = median5(&pbuf[2]);
        *past_gain_pit = tmp;
        if (sub(*past_gain_pit, 15565) > 0)
        {
            *past_gain_pit = 15565;        /* 0.95 in Q14 */

        }
        if (unusable_frame != 0)
        {
            *gain_pit = mult(pdown_unusable[state], *past_gain_pit);
        } else
        {
            *gain_pit = mult(pdown_usable[state], *past_gain_pit);
        }
        tmp = median5(&gbuf[2]);
        if (sub(vad_hist, 2) > 0)
        {
            *past_gain_code = tmp;
        } else
        {
            if (unusable_frame != 0)
            {
                *past_gain_code = mult(cdown_unusable[state], tmp);
            } else
            {
                *past_gain_code = mult(cdown_usable[state], tmp);
            }
        }

        /* update table of past quantized energies */

        L_tmp = L_mult(past_qua_en[0], 8192);   /* x 0.25 */
        L_tmp = L_mac(L_tmp, past_qua_en[1], 8192);     /* x 0.25 */
        L_tmp = L_mac(L_tmp, past_qua_en[2], 8192);     /* x 0.25 */
        L_tmp = L_mac(L_tmp, past_qua_en[3], 8192);     /* x 0.25 */
        qua_ener = extract_h(L_tmp);

        qua_ener = sub(qua_ener, 3072);    /* -3 in Q10 */
        if (sub(qua_ener, -14336) < 0)
            qua_ener = -14336;               /* -14 in Q10 */

        past_qua_en[3] = past_qua_en[2];
        past_qua_en[2] = past_qua_en[1];
        past_qua_en[1] = past_qua_en[0];
        past_qua_en[0] = qua_ener;

        for (i = 1; i < 5; i++)
        {
            gbuf[i - 1] = gbuf[i];
        }

        gbuf[4] = *past_gain_code;

        for (i = 1; i < 5; i++)
        {
            pbuf[i - 1] = pbuf[i];
        }

        pbuf[4] = *past_gain_pit;

        /* adjust gain according to energy of code */
        /* past_gain_code(Q3) * gcode_inov(Q12) => Q16 */
        *gain_cod = L_mult(*past_gain_code, gcode_inov);

        return;
    }
    /*-----------------------------------------------------------------*
     * Compute gcode0.                                                 *
     *  = Sum(i=0,1) pred[i]*past_qua_en[i] + mean_ener - ener_code    *
     *-----------------------------------------------------------------*/

    L_tmp = L_deposit_h(MEAN_ENER);        /* MEAN_ENER in Q16 */
    L_tmp = L_shl(L_tmp, 8);               /* From Q16 to Q24 */
    L_tmp = L_mac(L_tmp, pred[0], past_qua_en[0]);      /* Q13*Q10 -> Q24 */
    L_tmp = L_mac(L_tmp, pred[1], past_qua_en[1]);      /* Q13*Q10 -> Q24 */
    L_tmp = L_mac(L_tmp, pred[2], past_qua_en[2]);      /* Q13*Q10 -> Q24 */
    L_tmp = L_mac(L_tmp, pred[3], past_qua_en[3]);      /* Q13*Q10 -> Q24 */

    gcode0 = extract_h(L_tmp);             /* From Q24 to Q8  */


    L_tmp = L_mult(gcode0, 5443);
    L_tmp = L_shr(L_tmp, 8);               /* From Q24 to Q16             */
    L_Extract(L_tmp, &exp_gcode0, &frac);  /* Extract exponant of gcode0  */

    gcode0 = extract_l(AMRWB_Pow2(14, frac));    /* Put 14 as exponant so that  */
    /* output of Pow2() will be:   */
    /* 16384 < Pow2() <= 32767     */
    exp_gcode0 = sub(exp_gcode0, 14);

    /* Read the quantized gains */
    if (sub(nbits, 6) == 0)
    {
        p = &t_qua_gain6b[add(index, index)];
    } else
    {
        p = &t_qua_gain7b[add(index, index)];
    }
    *gain_pit = *p++;                        /* selected pitch gain in Q14 */
    g_code = *p++;                           /* selected code gain in Q11  */

    L_tmp = L_mult(g_code, gcode0);        /* Q11*Q0 -> Q12 */
    L_tmp = L_shl(L_tmp, add(exp_gcode0, 4));   /* Q12 -> Q16 */

    *gain_cod = L_tmp;                       /* gain of code in Q16 */
    if ((sub(prev_bfi, 1) == 0))
    {
        L_tmp = L_mult(*prev_gc, 5120);    /* prev_gc(Q3) * 1.25(Q12) = Q16 */
        /* if((*gain_cod > ((*prev_gc) * 1.25)) && (*gain_cod > 100.0)) */
        if ((L_sub(*gain_cod, L_tmp) > 0) && (L_sub(*gain_cod, 6553600) > 0))
        {
            *gain_cod = L_tmp;
        }
    }
    /* keep past gain code in Q3 for frame erasure (can saturate) */
    *past_gain_code = round(L_shl(*gain_cod, 3));
    *past_gain_pit = *gain_pit;

    *prev_gc = *past_gain_code;
    for (i = 1; i < 5; i++)
    {
        gbuf[i - 1] = gbuf[i];
    }

    gbuf[4] = *past_gain_code;

    for (i = 1; i < 5; i++)
    {
        pbuf[i - 1] = pbuf[i];
    }

    pbuf[4] = *past_gain_pit;

    for (i = 1; i < 5; i++)
    {
        pbuf2[i - 1] = pbuf2[i];
    }

    pbuf2[4] = *past_gain_pit;

    /* adjust gain according to energy of code */
    L_Extract(*gain_cod, &exp, &frac);
    L_tmp = Mpy_32_16(exp, frac, gcode_inov);
    *gain_cod = L_shl(L_tmp, 3);             /* gcode_inov in Q12 */

    /*---------------------------------------------------*
     * qua_ener = 20*log10(g_code)                       *
     *          = 6.0206*log2(g_code)                    *
     *          = 6.0206*(log2(g_codeQ11) - 11)          *
     *---------------------------------------------------*/

    L_tmp = L_deposit_l(g_code);
    AMRWB_Log2(L_tmp, &exp, &frac);
    exp = sub(exp, 11);
    L_tmp = Mpy_32_16(exp, frac, 24660);   /* x 6.0206 in Q12 */

    qua_ener = extract_l(L_shr(L_tmp, 3)); /* result in Q10 */

    /* update table of past quantized energies */

    past_qua_en[3] = past_qua_en[2];
    past_qua_en[2] = past_qua_en[1];
    past_qua_en[1] = past_qua_en[0];
    past_qua_en[0] = qua_ener;

    return;
}

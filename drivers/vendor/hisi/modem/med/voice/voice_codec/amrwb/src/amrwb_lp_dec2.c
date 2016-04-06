/*-------------------------------------------------------------------*
 *                         LP_DEC2.C								 *
 *-------------------------------------------------------------------*
 * Decimate a vector by 2 with 2nd order fir filter.                 *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_cnst.h"

#define L_FIR  5
#define L_MEM  (L_FIR-2)

/* static float h_fir[L_FIR] = {0.13, 0.23, 0.28, 0.23, 0.13}; */
/* fixed-point: sum of coef = 32767 to avoid overflow on DC */
static Word16 h_fir[L_FIR] = {4260, 7536, 9175, 7536, 4260};


void LP_Decim2(
     Word16 x[],                           /* in/out: signal to process         */
     Word16 l,                             /* input : size of filtering         */
     Word16 mem[]                          /* in/out: memory (size=3)           */
)
{
    Word16 *p_x, x_buf[AMRWB_L_FRAME + L_MEM];
    Word32 i, j, L_tmp;

    /* copy initial filter states into buffer */

    p_x = x_buf;
    /*for (i = 0; i < L_MEM; i++)
    {
        *p_x++ = mem[i];
    }*/
    CODEC_OpVecCpy(p_x, mem, L_MEM);
    p_x += L_MEM;

    /*for (i = 0; i < l; i++)
    {
        *p_x++ = x[i];
    }*/
    CODEC_OpVecCpy(p_x, x, l);
    p_x += l;

    /*for (i = 0; i < L_MEM; i++)
    {
        mem[i] = x[l - L_MEM + i];
    }*/
    CODEC_OpVecCpy(mem, &x[l - L_MEM], L_MEM);

    for (i = 0, j = 0; i < l; i += 2, j++)
    {
        p_x = &x_buf[i];

        L_tmp = 0L;

        /*for (k = 0; k < L_FIR; k++)
            L_tmp = L_mac(L_tmp, *p_x++, h_fir[k]);*/
        L_tmp = CODEC_OpVvMac(p_x, h_fir, L_FIR, L_tmp);

        x[j] = round(L_tmp);
    }

    return;
}

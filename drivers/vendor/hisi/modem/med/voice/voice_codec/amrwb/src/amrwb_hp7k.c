/*-------------------------------------------------------------------*
 *                         HP6K.C                                    *
 *-------------------------------------------------------------------*
 * 15th order high pass 7kHz FIR filter.                             *
 *                                                                   *
 *-------------------------------------------------------------------*/

#include "amrwb_cnst.h"
#include "amrwb_acelp.h"


#define L_FIR 31

static Word16 fir_7k[L_FIR] =
{
    -21, 47, -89, 146, -203,
    229, -177, 0, 335, -839,
    1485, -2211, 2931, -3542, 3953,
    28682, 3953, -3542, 2931, -2211,
    1485, -839, 335, 0, -177,
    229, -203, 146, -89, 47,
    -21
};

void Init_Filt_7k(Word16 mem[])            /* mem[30] */
{
    CODEC_OpVecSet(mem, L_FIR - 1, 0);

    return;
}


void Filt_7k(
     Word16 signal[],                      /* input:  signal                  */
     Word16 lg,                            /* input:  length of input         */
     Word16 mem[]                          /* in/out: memory (size=30)        */
)
{
    Word16 x[L_SUBFR16k + (L_FIR - 1)];
    Word32 i, L_tmp;

    CODEC_OpVecCpy(x, mem, L_FIR - 1);

    /*for (i = 0; i < lg; i++)
    {
        x[i + L_FIR - 1] = signal[i];
    }*/
    CODEC_OpVecCpy(&x[L_FIR - 1], signal, lg);

    for (i = 0; i < lg; i++)
    {
        L_tmp = 0;

        /*for (j = 0; j < L_FIR; j++)
            L_tmp = L_mac(L_tmp, x[i + j], fir_7k[j]);*/
        L_tmp = CODEC_OpVvMac(&x[i], fir_7k, L_FIR, L_tmp);

        signal[i] = round(L_tmp);
    }

    CODEC_OpVecCpy(mem, x + lg, L_FIR - 1);

    return;
}

/*-------------------------------------------------------------------*
 *                         SCALE.C                                   *
 *-------------------------------------------------------------------*
 * Scale signal to get maximum of dynamic.                           *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_op_hifi.h"

#ifdef _MED_C89_

void Scale_sig(
     Word16 x[],                           /* (i/o) : signal to scale               */
     Word16 lg,                            /* (i)   : size of x[]                   */
     Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
)
{
    Word32 i;
    Word16 tmp = lg;

    if (tmp%2 != 0)
    {
        tmp -= 1;
        x[tmp] = L_deposit_shl_round(x[tmp], exp);
    }

    for (i = 0; i < tmp; i+=2)
    {
        /*L_tmp = L_deposit_h(x[i]);
        L_tmp = L_shl(L_tmp, exp);
        x[i] = round(L_tmp);*/
        x[i] = L_deposit_shl_round(x[i], exp);
        x[i+1] = L_deposit_shl_round(x[i+1], exp);
    }

    return;
}
#endif

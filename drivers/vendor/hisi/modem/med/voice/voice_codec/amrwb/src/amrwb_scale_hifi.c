/*-------------------------------------------------------------------*
 *                         SCALE.C                                   *
 *-------------------------------------------------------------------*
 * Scale signal to get maximum of dynamic.                           *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_op_hifi.h"

#ifndef _MED_C89_
void Scale_sig(
     Word16 x[],                           /* (i/o) : signal to scale               */
     Word16 lg,                            /* (i)   : size of x[]                   */
     Word16 exp                            /* (i)   : exponent: x = round(x << exp) */
)
{
    Word16 i;
    ae_p24x2s aep_x, aep_x1;

    if(exp >= 0)
    {
        WUR_AE_SAR(exp);

        for (i = 0; i < (lg>>1); i++)
        {
            aep_x = *((ae_p16s *)&x[2*i]);
            aep_x1 = *((ae_p16s *)&x[2*i+1]);

            aep_x = AE_SLLSSP24S(aep_x);
            aep_x = AE_ROUNDSP16ASYM(aep_x);
            aep_x1 = AE_SLLSSP24S(aep_x1);
            aep_x1 = AE_ROUNDSP16ASYM(aep_x1);

            *((ae_p16s *)&x[2*i]) = aep_x;
            *((ae_p16s *)&x[2*i+1]) = aep_x1;
        }

        if((lg&0x1) != 0)
        {
            aep_x = *((ae_p16s *)&x[lg-1]);

            aep_x = AE_SLLSSP24S(aep_x);
            aep_x = AE_ROUNDSP16ASYM(aep_x);

            *((ae_p16s *)&x[lg-1]) = aep_x;
        }
    }
    else
    {
        WUR_AE_SAR(-exp);

        for (i = 0; i < (lg>>1); i++)
        {
            aep_x = *((ae_p16s *)&x[2*i]);
            aep_x1 = *((ae_p16s *)&x[2*i+1]);

            aep_x = AE_SRASP24(aep_x);
            aep_x = AE_ROUNDSP16ASYM(aep_x);
            aep_x1 = AE_SRASP24(aep_x1);
            aep_x1 = AE_ROUNDSP16ASYM(aep_x1);

            *((ae_p16s *)&x[2*i]) = aep_x;
            *((ae_p16s *)&x[2*i+1]) = aep_x1;
        }

        if ((lg&0x1) != 0)
        {
            aep_x = *((ae_p16s *)&x[lg-1]);

            aep_x = AE_SRASP24(aep_x);
            aep_x = AE_ROUNDSP16ASYM(aep_x);

            *((ae_p16s *)&x[lg-1]) = aep_x;
        }
    }

    return;
}

#endif

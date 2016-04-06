/*-----------------------------------------------------------------------*
 *                         HP400.C                                       *
 *-----------------------------------------------------------------------*
 * Interpolation of the LP parameters in 4 subframes.                    *
 *-----------------------------------------------------------------------*/

#include "amrwb_cnst.h"
#include "amrwb_acelp.h"

#define MP1 (AMRWB_M+1)


void Int_isp(
     Word16 isp_old[],                     /* input : isps from past frame              */
     Word16 isp_new[],                     /* input : isps from present frame           */
     Word16 frac[],                        /* input : fraction for 3 first subfr (Q15)  */
     Word16 Az[]                           /* output: LP coefficients in 4 subframes    */
)
{
    Word16 fac_old, fac_new;
    Word16 isp[AMRWB_M];
    Word32 i, k, L_tmp;

    for (k = 0; k < 3; k++)
    {
        fac_new = frac[k];
        fac_old = add(sub(32767, fac_new), 1);  /* 1.0 - fac_new */

        for (i = 0; i < AMRWB_M; i++)
        {
            L_tmp = L_mult(isp_old[i], fac_old);
            L_tmp = L_mac(L_tmp, isp_new[i], fac_new);
            isp[i] = round(L_tmp);
        }
        Isp_Az(isp, Az, AMRWB_M, 0);
        Az += MP1;
    }

    /* 4th subframe: isp_new (frac=1.0) */

    Isp_Az(isp_new, Az, AMRWB_M, 0);

    return;
}

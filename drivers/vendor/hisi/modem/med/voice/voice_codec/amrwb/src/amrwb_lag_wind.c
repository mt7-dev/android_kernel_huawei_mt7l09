/*---------------------------------------------------------*
 *                         LAG_WIND.C					   *
 *---------------------------------------------------------*
 * Lag_window on autocorrelations.                         *
 *                                                         *
 * r[i] *= lag_wind[i]                                     *
 *                                                         *
 *  r[i] and lag_wind[i] are in special double precision.  *
 *  See "oper_32b.c" for the format                        *
 *---------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_cnst.h"

/*#define M 16*/

Word16 lag_h[AMRWB_M] = {
      32750,
      32707,
      32637,
      32538,
      32411,
      32257,
      32075,
      31867,
      31633,
      31374,
      31089,
      30780,
      30449,
      30094,
      29718,
      29321};

Word16 lag_l[AMRWB_M] = {
      16896,
      30464,
       2496,
       4480,
      12160,
       3520,
      24320,
      24192,
      20736,
        576,
      18240,
      31488,
        128,
      16704,
      11520,
      14784};

void AMRWB_Lag_window(
     Word16 r_h[],                         /* (i/o)   : Autocorrelations  (msb)          */
     Word16 r_l[]                          /* (i/o)   : Autocorrelations  (lsb)          */
)
{
    Word32 i, x;

    for (i = 1; i <= AMRWB_M; i++)
    {
        x = Mpy_32(r_h[i], r_l[i], lag_h[i - 1], lag_l[i - 1]);
        L_Extract(x, &r_h[i], &r_l[i]);
    }
    return;
}

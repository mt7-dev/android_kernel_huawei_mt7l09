/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : gains.tab
*      Purpose          : Scalar quantization tables of the pitch gain and
*                       : the codebook gain.
*      $Id $
*
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "cnst.h"

#define NB_QUA_PITCH 16

static const Word16 qua_gain_pitch[NB_QUA_PITCH] =
{
    0, 3277, 6556, 8192, 9830, 11469, 12288, 13107,
    13926, 14746, 15565, 16384, 17203, 18022, 18842, 19661
};

#define NB_QUA_CODE 32

static const Word16 qua_gain_code[NB_QUA_CODE*3] =
{
/* gain factor (g_fac) and quantized energy error (qua_ener_MR122, qua_ener)
 * are stored:
 *
 * qua_ener_MR122 = log2(g_fac)      (not the rounded floating point value, but
 *                                    the value the original EFR algorithm
 *                                    calculates from g_fac [using Log2])
 * qua_ener       = 20*log10(g_fac); (rounded floating point value)
 *
 *
 * g_fac (Q11), qua_ener_MR122 (Q10), qua_ener (Q10)
 */
          159,                -3776,          -22731,
          206,                -3394,          -20428,
          268,                -3005,          -18088,
          349,                -2615,          -15739,
          419,                -2345,          -14113,
          482,                -2138,          -12867,
          554,                -1932,          -11629,
          637,                -1726,          -10387,
          733,                -1518,           -9139,
          842,                -1314,           -7906,
          969,                -1106,           -6656,
         1114,                 -900,           -5416,
         1281,                 -694,           -4173,
         1473,                 -487,           -2931,
         1694,                 -281,           -1688,
         1948,                  -75,            -445,
         2241,                  133,             801,
         2577,                  339,            2044,
         2963,                  545,            3285,
         3408,                  752,            4530,
         3919,                  958,            5772,
         4507,                 1165,            7016,
         5183,                 1371,            8259,
         5960,                 1577,            9501,
         6855,                 1784,           10745,
         7883,                 1991,           11988,
         9065,                 2197,           13231,
        10425,                 2404,           14474,
        12510,                 2673,           16096,
        16263,                 3060,           18429,
        21142,                 3448,           20763,
        27485,                 3836,           23097
};

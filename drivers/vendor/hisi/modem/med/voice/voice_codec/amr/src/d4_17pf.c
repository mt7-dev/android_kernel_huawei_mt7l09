/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : d4_17pf.c
*      Purpose          : Algebraic codebook decoder
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "d4_17pf.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define NB_PULSE 4

#include "gray_tab.c"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  decod_ACELP()
 *
 *  PURPOSE:   Algebraic codebook decoder
 *
 *************************************************************************/

void decode_4i40_17bits(
    Word16 sign,   /* i : signs of 4 pulses.                       */
    Word16 index,  /* i : Positions of the 4 pulses.               */
    Word16 cod[]   /* o : algebraic (fixed) codebook excitation    */
)
{
    Word16 i, j;
    Word16 pos[NB_PULSE];

    /* Decode the positions */

    i = index & 7;
    i = dgray[i];

    pos[0] = add(i, shl(i, 2));   /* pos0 =i*5 */

    index = shr(index, 3);
    i = index & 7;
    i = dgray[i];

    i = add(i, shl(i, 2));        /* pos1 =i*5+1 */
    pos[1] = add(i, 1);

    index = shr(index, 3);
    i = index & 7;
    i = dgray[i];

    i = add(i, shl(i, 2));        /* pos2 =i*5+1 */
    pos[2] = add(i, 2);

    index = shr(index, 3);
    j = index & 1;
    index = shr(index, 1);
    i = index & 7;
    i = dgray[i];

    i = add(i, shl(i, 2));        /* pos3 =i*5+3+j */
    i = add(i, 3);
    pos[3] = add(i, j);

    /* decode the signs  and build the codeword */

    CODEC_OpVecSet(&cod[0], L_SUBFR, 0);

    for (j = 0; j < NB_PULSE; j++) {
        i = sign & 1;
        sign = shr(sign, 1);

        if (i != 0) {
            cod[pos[j]] = 8191;
        } else {
            cod[pos[j]] = -8192;
        }
    }

    return;
}

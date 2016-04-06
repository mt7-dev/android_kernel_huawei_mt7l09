/*-------------------------------------------------------------------*
 *                         D2T64FX.C							     *
 *-------------------------------------------------------------------*
 * 20, 36, 44, 52, 64, 72, 88 bits algebraic codebook decoder.       *
 * 4 tracks x 16 positions per track = 64 samples.                   *
 *                                                                   *
 * 20 bits --> 4 pulses in a frame of 64 samples.                    *
 * 36 bits --> 8 pulses in a frame of 64 samples.                    *
 * 44 bits --> 10 pulses in a frame of 64 samples.                   *
 * 52 bits --> 12 pulses in a frame of 64 samples.                   *
 * 64 bits --> 16 pulses in a frame of 64 samples.                   *
 * 72 bits --> 18 pulses in a frame of 64 samples.                   *
 * 88 bits --> 24 pulses in a frame of 64 samples.                   *
 *                                                                   *
 * All pulses can have two (2) possible amplitudes: +1 or -1.        *
 * Each pulse can have sixteen (16) possible positions.              *
 *                                                                   *
 * See c36_64fx.c for more details of the algebraic code.            *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_cnst.h"

#include "amrwb_q_pulse.h"

#define L_CODE    64                       /* codevector length  */
#define NB_TRACK  4                        /* number of track    */
#define NB_POS    16                       /* number of position */

/* local function */

static void add_pulses(Word16 pos[], Word16 nb_pulse, Word16 track, Word16 code[]);
void DEC_ACELP_4t64_fx(
     Word16 index[],                       /* (i) : index (20): 5+5+5+5 = 20 bits.                 */
                                           /* (i) : index (36): 9+9+9+9 = 36 bits.                 */
                                           /* (i) : index (44): 13+9+13+9 = 44 bits.               */
                                           /* (i) : index (52): 13+13+13+13 = 52 bits.             */
                                           /* (i) : index (64): 2+2+2+2+14+14+14+14 = 64 bits.     */
                                           /* (i) : index (72): 10+2+10+2+10+14+10+14 = 72 bits.   */
                                           /* (i) : index (88): 11+11+11+11+11+11+11+11 = 88 bits. */
     Word16 nbbits,                        /* (i) : 20, 36, 44, 52, 64, 72 or 88 bits              */
     Word16 code[]                         /* (o) Q9: algebraic (fixed) codebook excitation        */
)
{
    Word16 pos[6];
    Word32 k, L_index;

    /*for (i = 0; i < L_CODE; i++)
    {
        code[i] = 0;
    }*/
    CODEC_OpVecSet(code, L_CODE, 0);

    /* decode the positions and signs of pulses and build the codeword */
    switch(nbbits)
    {
        case 20:
        {
            for (k = 0; k < NB_TRACK; k++)
            {
                L_index = index[k];
                dec_1p_N1(L_index, 4, 0, pos);
                add_pulses(pos, 1, k, code);
            }

            break;
        }

        case 36:
        {
            for (k = 0; k < NB_TRACK; k++)
            {
                L_index = index[k];
                dec_2p_2N1(L_index, 4, 0, pos);
                add_pulses(pos, 2, k, code);
            }

            break;
        }

        case 44:
        {
            for (k = 0; k < NB_TRACK - 2; k++)
            {
                L_index = index[k];
                dec_3p_3N1(L_index, 4, 0, pos);
                add_pulses(pos, 3, k, code);
            }

            for (k = 2; k < NB_TRACK; k++)
            {
                L_index = index[k];
                dec_2p_2N1(L_index, 4, 0, pos);
                add_pulses(pos, 2, k, code);
            }

            break;
        }

        case 52:
        {
            for (k = 0; k < NB_TRACK; k++)
            {
                L_index = index[k];
                dec_3p_3N1(L_index, 4, 0, pos);
                add_pulses(pos, 3, k, code);
            }

            break;
        }

        case 64:
        {
            for (k = 0; k < NB_TRACK; k++)
            {
                L_index = L_add(L_shl(index[k], 14), index[k + NB_TRACK]);
                dec_4p_4N(L_index, 4, 0, pos);
                add_pulses(pos, 4, k, code);
            }

            break;
        }

        case 72:
        {
            for (k = 0; k < NB_TRACK - 2; k++)
            {
                L_index = L_add(L_shl(index[k], 10), index[k + NB_TRACK]);
                dec_5p_5N(L_index, 4, 0, pos);
                add_pulses(pos, 5, k, code);
            }

            for (k = 2; k < NB_TRACK; k++)
            {
                L_index = L_add(L_shl(index[k], 14), index[k + NB_TRACK]);
                dec_4p_4N(L_index, 4, 0, pos);
                add_pulses(pos, 4, k, code);
            }

            break;
        }

        case 88:
        {
            for (k = 0; k < NB_TRACK; k++)
            {
                L_index = L_add(L_shl(index[k], 11), index[k + NB_TRACK]);
                dec_6p_6N_2(L_index, 4, 0, pos);
                add_pulses(pos, 6, k, code);
            }

            break;
        }

        default:
            break;

    }

    return;
}

static void add_pulses(Word16 pos[], Word16 nb_pulse, Word16 track, Word16 code[])
{
    Word32 i, k;

    for (k = 0; k < nb_pulse; k++)
    {
        /* i = ((pos[k] & (NB_POS-1))*NB_TRACK) + track; */
        i = (Word16) (add(shl((Word16) (pos[k] & (NB_POS - 1)), 2), track));
        if ((pos[k] & NB_POS) == 0)
            code[i] = add(code[i], 512);
        else
            code[i] = sub(code[i], 512);
    }

    return;
}

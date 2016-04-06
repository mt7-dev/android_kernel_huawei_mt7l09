/*-------------------------------------------------------------------*
 *                         RANDOM.C                                  *
 *-------------------------------------------------------------------*
 * Signed 16 bits random generator.                                  *
 *-------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"


Word16 Random(Word16 * seed)
{
    /* static Word16 seed = 21845; */

    *seed = extract_l(L_add(L_shr(L_mult(*seed, 31821), 1), 13849L));

    return (*seed);
}

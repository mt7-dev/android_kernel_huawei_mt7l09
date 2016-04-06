/*---------------------------------------------------------------------*
 * routine preemphasis()                                               *
 * ~~~~~~~~~~~~~~~~~~~~~                                               *
 * Preemphasis: filtering through 1 - g z^-1                           *
 *---------------------------------------------------------------------*/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"

Word16 efr_mem_pre;

void EFR_preemphasis (
    Word16 *signal, /* (i/o)   : input signal overwritten by the output */
    Word16 g,       /* (i)     : preemphasis coefficient                */
    Word16 L        /* (i)     : size of filtering                      */
)
{
    Word16 *p1, *p2, temp, i;

    p1 = signal + L - 1;
    p2 = p1 - 1;
    temp = *p1;

    for (i = 0; i <= L - 2; i++)
    {
        *p1 = sub (*p1, mult (g, *p2--));
        p1--;
    }

    *p1 = sub (*p1, mult (g, efr_mem_pre));

    efr_mem_pre = temp;

    return;
}

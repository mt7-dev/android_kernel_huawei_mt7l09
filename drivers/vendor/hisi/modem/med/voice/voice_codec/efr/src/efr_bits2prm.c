/*************************************************************************
 *
 *  FUNCTION:  Bits2prm_12k2
 *
 *  PURPOSE: Retrieves the vector of encoder parameters from the received
 *           serial bits in a frame.
 *
 *  DESCRIPTION: The encoder parameters are:
 *
 *     BFI      bad frame indicator      1 bit
 *
 *     LPC:
 *              1st codebook             7 bit
 *              2nd codebook             8 bit
 *              3rd codebook             8+1 bit
 *              4th codebook             8 bit
 *              5th codebook             6 bit
 *
 *     1st and 3rd subframes:
 *           pitch period                9 bit
 *           pitch gain                  4 bit
 *           codebook index              35 bit
 *           codebook gain               5 bit
 *
 *     2nd and 4th subframes:
 *           pitch period                6 bit
 *           pitch gain                  4 bit
 *           codebook index              35 bit
 *           codebook gain               5 bit
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"

/* Local function */

Word16 EFR_Bin2int (        /* Reconstructed parameter                      */
    Word16 no_of_bits,  /* input : number of bits associated with value */
    Word16 *bitstream   /* output: address where bits are written       */
);

#define BIT_0     0
#define BIT_1     1
#define PRM_NO    57

void EFR_Bits2prm_12k2 (
    Word16 bits[],      /* input : serial bits (244 + bfi)                */
    Word16 prm[]        /* output: analysis parameters  (57+1 parameters) */
)
{
    Word16 i;

    static const Word16 bitno[PRM_NO] =
    {
        7, 8, 9, 8, 6,                          /* LSP VQ          */
        9, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 5,  /* first subframe  */
        6, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 5,  /* second subframe */
        9, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 5,  /* third subframe  */
        6, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 5}; /* fourth subframe */

    *prm++ = *bits++;                           /* read BFI */

    for (i = 0; i < PRM_NO; i++)
    {
        prm[i] = EFR_Bin2int (bitno[i], bits);
        bits += bitno[i];
    }
    return;
}

/*************************************************************************
 *
 *  FUNCTION:  Bin2int
 *
 *  PURPOSE: Read "no_of_bits" bits from the array bitstream[] and convert
 *           to integer.
 *
 *************************************************************************/

Word16 EFR_Bin2int (        /* Reconstructed parameter                      */
    Word16 no_of_bits,  /* input : number of bits associated with value */
    Word16 *bitstream   /* output: address where bits are written       */
)
{
    Word16 value, i, bit;

    value = 0;
    for (i = 0; i < no_of_bits; i++)
    {
        value = shl (value, 1);
        bit = *bitstream++;

        if (sub (bit, BIT_1) == 0)
            value = add (value, 1);
    }
    return (value);
}

/********************************************************************************
*
*      File             : amrwb_log2.h
*      Purpose          : Computes log2(L_x)
*
********************************************************************************
*/
#ifndef log2_h
#define log2_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
void AMRWB_Log2 (
    Word32 L_x,        /* (i) : input value                                 */
    Word16 *exponent,  /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
    Word16 *fraction   /* (o) : Fractional part of Log2. (range: 0<=val<1)*/
);

void AMRWB_Log2_norm (
    Word32 L_x,         /* (i) : input value (normalized)                    */
    Word16 exp,         /* (i) : norm_l (L_x)                                */
    Word16 *exponent,   /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
    Word16 *fraction    /* (o) : Fractional part of Log2. (range: 0<=val<1)  */
);

#endif

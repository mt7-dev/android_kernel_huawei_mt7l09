/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : int_lsf.c
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "int_lsf.h"

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
/*
*--------------------------------------*
* Constants (defined in cnst.h)        *
*--------------------------------------*
*  M         : LSF order               *
*--------------------------------------*
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*
**************************************************************************
*
*  Function    : Int_lsf
*  Purpose     : Interpolates the LSFs for selected subframe
*  Description : The 20 ms speech frame is divided into 4 subframes.
*                The LSFs are interpolated at the 1st, 2nd and 3rd
*                subframe and only forwarded at the 4th subframe.
*
*                      |------|------|------|------|
*                         sf1    sf2    sf3    sf4
*                   F0                          F1
*
*                 sf1:   3/4 F0 + 1/4 F1         sf3:   1/4 F0 + 3/4 F1
*                 sf2:   1/2 F0 + 1/2 F1         sf4:       F1
*  Returns     : void
*
**************************************************************************
*/
void Int_lsf(
    Word16 lsf_old[], /* i : LSF vector at the 4th SF of past frame          */
    Word16 lsf_new[], /* i : LSF vector at the 4th SF of present frame       */
    Word16 i_subfr,   /* i : Pointer to current sf (equal to 0,40,80 or 120) */
    Word16 lsf_out[]  /* o : interpolated LSF parameters for current sf      */
)
{
    if ( i_subfr == 0 )
    {
       CODEC_OpVvFormWithQuar(&lsf_new[0],
                            &lsf_old[0],
                            M,
                            &lsf_out[0]);
    }
    else if ( 40 == i_subfr )
    {
       CODEC_OpVvFormWithDimi(&lsf_old[0],
                            &lsf_new[0],
                            M,
                            &lsf_out[0]);
    }
    else if ( 80 == i_subfr )
    {
       CODEC_OpVvFormWithQuar(&lsf_old[0],
                            &lsf_new[0],
                            M,
                            &lsf_out[0]);
    }
    else if ( 120 == i_subfr )
    {
       CODEC_OpVecCpy(&lsf_out[0], &lsf_new[0], M);
    }

    return;
}

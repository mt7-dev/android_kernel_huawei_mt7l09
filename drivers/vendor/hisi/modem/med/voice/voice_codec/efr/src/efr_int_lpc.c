/*************************************************************************
 *
 *  FUNCTION:  Int_lpc()
 *
 *  PURPOSE:  Interpolates the LSPs and converts to LPC parameters to get
 *            a different LP filter in each subframe.
 *
 *  DESCRIPTION:
 *     The 20 ms speech frame is divided into 4 subframes.
 *     The LSPs are quantized and transmitted at the 2nd and 4th subframes
 *     (twice per frame) and interpolated at the 1st and 3rd subframe.
 *
 *          |------|------|------|------|
 *             sf1    sf2    sf3    sf4
 *       F0            Fm            F1
 *
 *     sf1:   1/2 Fm + 1/2 F0         sf3:   1/2 F1 + 1/2 Fm
 *     sf2:       Fm                  sf4:       F1
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"
#include "efr_cnst.h"


void EFR_Int_lpc (
    Word16 lsp_old[],   /* input : LSP vector at the 4th subframe
                           of past frame    */
    Word16 lsp_mid[],   /* input : LSP vector at the 2nd subframe
                           of present frame */
    Word16 lsp_new[],   /* input : LSP vector at the 4th subframe of
                           present frame */
    Word16 Az[]         /* output: interpolated LP parameters in
                           all subframes */
)
{
    Word16 lsp[EFR_M];

    /*  lsp[i] = lsp_mid[i] * 0.5 + lsp_old[i] * 0.5 */

    CODEC_OpVvFormWithDimi(lsp_mid, lsp_old, EFR_M, lsp);

    EFR_Lsp_Az (lsp, Az);           /* Subframe 1 */
    Az += EFR_MP1;

    EFR_Lsp_Az (lsp_mid, Az);       /* Subframe 2 */
    Az += EFR_MP1;

    CODEC_OpVvFormWithDimi(lsp_mid, lsp_new, EFR_M, lsp);

    EFR_Lsp_Az (lsp, Az);           /* Subframe 3 */
    Az += EFR_MP1;

    EFR_Lsp_Az (lsp_new, Az);       /* Subframe 4 */

    return;
}

/*----------------------------------------------------------------------*
 * Function Int_lpc2()                                                  *
 * ~~~~~~~~~~~~~~~~~~                                                   *
 * Interpolation of the LPC parameters.                                 *
 * Same as the previous function but we do not recompute Az() for       *
 * subframe 2 and 4 because it is already available.                    *
 *----------------------------------------------------------------------*/

void EFR_Int_lpc2 (
             Word16 lsp_old[],  /* input : LSP vector at the 4th subframe
                                 of past frame    */
             Word16 lsp_mid[],  /* input : LSP vector at the 2nd subframe
                                 of present frame */
             Word16 lsp_new[],  /* input : LSP vector at the 4th subframe of
                                 present frame */
             Word16 Az[]        /* output: interpolated LP parameters
                                 in subframes 1 and 3 */
)
{
    Word16 lsp[EFR_M];

    /*  lsp[i] = lsp_mid[i] * 0.5 + lsp_old[i] * 0.5 */

    CODEC_OpVvFormWithDimi(lsp_mid, lsp_old, EFR_M, lsp);

    EFR_Lsp_Az (lsp, Az);           /* Subframe 1 */
    Az += EFR_MP1 * 2;

    CODEC_OpVvFormWithDimi(lsp_mid, lsp_new, EFR_M, lsp);
    EFR_Lsp_Az (lsp, Az);           /* Subframe 3 */

    return;
}

/*************************************************************************
 *
 *  FUNCTION:  Pre_Process()
 *
 *  PURPOSE: Preprocessing of input speech.
 *
 *  DESCRIPTION:
 *     - 2nd order high pass filtering with cut off frequency at 80 Hz.
 *     - Divide input by two.
 *
 *************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"

/*------------------------------------------------------------------------*
 *                                                                        *
 * Algorithm:                                                             *
 *                                                                        *
 *  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b[2]*x[i-2]/2                    *
 *                     + a[1]*y[i-1]   + a[2]*y[i-2];                     *
 *                                                                        *
 *                                                                        *
 *  Input is divided by two in the filtering process.                     *
 *------------------------------------------------------------------------*/

/* filter coefficients (fc = 80 Hz, coeff. b[] is divided by 2) */

static const Word16 efr_b[3] = {1899, -3798, 1899};
static const Word16 efr_a[3] = {4096, 7807, -3733};

/* Static values to be preserved between calls */
/* y[] values are kept in double precision     */

static Word16 efr_y2_hi, efr_y2_lo, efr_y1_hi, efr_y1_lo, efr_x0, efr_x1;

/* Initialization of static values */

void EFR_Init_Pre_Process (void)
{
    efr_y2_hi = 0;
    efr_y2_lo = 0;
    efr_y1_hi = 0;
    efr_y1_lo = 0;
    efr_x0 = 0;
    efr_x1 = 0;
}

void EFR_Pre_Process (
    Word16 signal[], /* input/output signal */
    Word16 lg)       /* lenght of signal    */
{
    Word16 i, x2;
    Word32 L_tmp;

    for (i = 0; i < lg; i++)
    {
        x2 = efr_x1;
        efr_x1 = efr_x0;
        efr_x0 = signal[i];

        /*  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b140[2]*x[i-2]/2  */
        /*                     + a[1]*y[i-1] + a[2] * y[i-2];      */

        L_tmp = Mpy_32_16 (efr_y1_hi, efr_y1_lo, efr_a[1]);
        L_tmp = L_add (L_tmp, Mpy_32_16 (efr_y2_hi, efr_y2_lo, efr_a[2]));
        L_tmp = L_mac (L_tmp, efr_x0, efr_b[0]);
        L_tmp = L_mac (L_tmp, efr_x1, efr_b[1]);
        L_tmp = L_mac (L_tmp, x2, efr_b[2]);
        L_tmp = L_shl (L_tmp, 3);
        signal[i] = round (L_tmp);

        efr_y2_hi = efr_y1_hi;
        efr_y2_lo = efr_y1_lo;
        L_Extract (L_tmp, &efr_y1_hi, &efr_y1_lo);
    }
    return;
}

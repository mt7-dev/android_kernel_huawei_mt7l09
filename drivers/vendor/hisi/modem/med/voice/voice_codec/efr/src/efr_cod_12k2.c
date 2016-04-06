/***************************************************************************
 *
 *  FILE NAME:    cod_12k2.c
 *
 *  FUNCTIONS DEFINED IN THIS FILE:
 *                   Coder_12k2  and  Init_Coder_12k2
 *
 *
 *  Init_Coder_12k2(void):
 *      Initialization of variables for the coder section.
 *
 *  Coder_12k2(Word16 ana[], Word16 synth[]):
 *      Speech encoder routine operating on a frame basis.
 *

***************************************************************************/

#include "codec_op_vec.h"
#include "codec_op_etsi.h"
#include "efr_sig_proc.h"
#include "efr_codec.h"
#include "efr_cnst.h"

#include "efr_window2_tab.c"

#include "efr_vad.h"
#include "efr_dtx.h"

/*-----------------------------------------------------------*
 *    Coder constant parameters (defined in "cnst.h")        *
 *-----------------------------------------------------------*
 *   EFR_L_WINDOW    : LPC analysis window size                  *
 *   EFR_L_FRAME     : Frame size                                *
 *   EFR_L_FRAME_BY2 : Half the frame size                       *
 *   EFR_L_SUBFR     : Sub-frame size                            *
 *   EFR_M           : LPC order                                 *
 *   EFR_MP1         : LPC order+1                               *
 *   EFR_L_TOTAL     : Total size of speech buffer               *
 *   EFR_PIT_MIN     : Minimum pitch lag                         *
 *   EFR_PIT_MAX     : Maximum pitch lag                         *
 *   EFR_L_INTERPOL  : Length of filter for interpolation        *
 *-----------------------------------------------------------*/

/*--------------------------------------------------------*
 *         Static memory allocation.                      *
 *--------------------------------------------------------*/

 /* Speech vector */

static Word16 efr_old_speech[EFR_L_TOTAL];
static Word16 *efr_speech, *efr_p_window, *efr_p_window_mid;
Word16 *efr_new_speech;             /* Global variable */

 /* Weight speech vector */

static Word16 efr_old_wsp[EFR_L_FRAME + EFR_PIT_MAX];
static Word16 *efr_wsp;

 /* Excitation vector */

static Word16 efr_old_exc[EFR_L_FRAME + EFR_PIT_MAX + EFR_L_INTERPOL];
static Word16 *efr_exc;

 /* Zero vector */

static Word16 efr_ai_zero[EFR_L_SUBFR + EFR_MP1];
static Word16 *efr_zero;

 /* Impulse response vector */

static Word16 *efr_h1;
static Word16 efr_hvec[EFR_L_SUBFR * 2];

 /* Spectral expansion factors */

static const Word16 efr_F_gamma1[EFR_M] =
{
    29491, 26542, 23888, 21499, 19349,
    17414, 15672, 14105, 12694, 11425
};
static const Word16 efr_F_gamma2[EFR_M] =
{
    19661, 11797, 7078, 4247, 2548,
    1529, 917, 550, 330, 198
};

 /* Lsp (Line spectral pairs) */

static Word16 efr_lsp_old[EFR_M];
static Word16 efr_lsp_old_q[EFR_M];

 /* Filter's memory */

static Word16 efr_mem_syn[EFR_M], efr_mem_w0[EFR_M], efr_mem_w[EFR_M];
static Word16 efr_mem_err[EFR_M + EFR_L_SUBFR], *efr_error;

/***************************************************************************
 *  FUNCTION:   Init_Coder_12k2
 *
 *  PURPOSE:   Initialization of variables for the coder section.
 *
 *  DESCRIPTION:
 *       - initilize pointers to speech buffer
 *       - initialize static  pointers
 *       - set static vectors to efr_zero
 *
 ***************************************************************************/

void EFR_Init_Coder_12k2 (void)
{

/*--------------------------------------------------------------------------*
 *          Initialize pointers to speech vector.                           *
 *--------------------------------------------------------------------------*/

    efr_new_speech = efr_old_speech + EFR_L_TOTAL - EFR_L_FRAME;/* New speech     */
    efr_speech = efr_new_speech;                        /* Present frame  */
    efr_p_window = efr_old_speech + EFR_L_TOTAL - EFR_L_WINDOW; /* For LPC window */
    efr_p_window_mid = efr_p_window;                    /* For LPC window */

    /* Initialize static pointers */

    efr_wsp = efr_old_wsp + EFR_PIT_MAX;
    efr_exc = efr_old_exc + EFR_PIT_MAX + EFR_L_INTERPOL;
    efr_zero = efr_ai_zero + EFR_MP1;
    efr_error = efr_mem_err + EFR_M;
    efr_h1 = &efr_hvec[EFR_L_SUBFR];

    /* Static vectors to efr_zero */

    CODEC_OpVecSet(efr_old_speech, EFR_L_TOTAL, 0);
    CODEC_OpVecSet(efr_old_exc, EFR_PIT_MAX + EFR_L_INTERPOL, 0);
    CODEC_OpVecSet(efr_old_wsp, EFR_PIT_MAX, 0);
    CODEC_OpVecSet(efr_mem_syn, EFR_M, 0);
    CODEC_OpVecSet(efr_mem_w, EFR_M, 0);
    CODEC_OpVecSet(efr_mem_w0, EFR_M, 0);
    CODEC_OpVecSet(efr_mem_err, EFR_M, 0);
    CODEC_OpVecSet(efr_zero, EFR_L_SUBFR, 0);
    CODEC_OpVecSet(efr_hvec, EFR_L_SUBFR, 0);
    /* Initialize efr_lsp_old [] */

    efr_lsp_old[0] = 30000;
    efr_lsp_old[1] = 26000;
    efr_lsp_old[2] = 21000;
    efr_lsp_old[3] = 15000;
    efr_lsp_old[4] = 8000;
    efr_lsp_old[5] = 0;
    efr_lsp_old[6] = -8000;
    efr_lsp_old[7] = -15000;
    efr_lsp_old[8] = -21000;
    efr_lsp_old[9] = -26000;

    /* Initialize efr_lsp_old_q[] */

    CODEC_OpVecCpy(efr_lsp_old_q, efr_lsp_old, EFR_M);

    return;
}

/***************************************************************************
 *   FUNCTION:   Coder_12k2
 *
 *   PURPOSE:  Principle encoder routine.
 *
 *   DESCRIPTION: This function is called every 20 ms speech frame,
 *       operating on the newly read 160 speech samples. It performs the
 *       principle encoding functions to produce the set of encoded parameters
 *       which include the LSP, adaptive codebook, and fixed codebook
 *       quantization indices (addresses and gains).
 *
 *   INPUTS:
 *       No input arguments are passed to this function. However, before
 *       calling this function, 160 new speech data samples should be copied to
 *       the vector efr_new_speech[]. This is a global pointer which is declared in
 *       this file (it points to the end of speech buffer minus 160).
 *
 *   OUTPUTS:
 *
 *       ana[]:     vector of analysis parameters.
 *       synth[]:   Local synthesis speech (for debugging purposes)
 *
 ***************************************************************************/

void EFR_Coder_12k2 (
    Word16 ana[],    /* output  : Analysis parameters */
    Word16 synth[]   /* output  : Local synthesis     */
)
{
    /* LPC coefficients */

    Word16 r_l[EFR_MP1], r_h[EFR_MP1];      /* Autocorrelations lo and hi           */
    Word16 A_t[(EFR_MP1) * 4];          /* A(z) unquantized for the 4 subframes */
    Word16 Aq_t[(EFR_MP1) * 4];         /* A(z)   quantized for the 4 subframes */
    Word16 Ap1[EFR_MP1];                /* A(z) with spectral expansion         */
    Word16 Ap2[EFR_MP1];                /* A(z) with spectral expansion         */
    Word16 *A, *Aq;                 /* Pointer on A_t and Aq_t              */
    Word16 lsp_new[EFR_M], lsp_new_q[EFR_M];/* LSPs at 4th subframe                 */
    Word16 lsp_mid[EFR_M], lsp_mid_q[EFR_M];/* LSPs at 2nd subframe                 */

    /* Other vectors */

    Word16 xn[EFR_L_SUBFR];            /* Target vector for pitch search        */
    Word16 xn2[EFR_L_SUBFR];           /* Target vector for codebook search     */
    Word16 res2[EFR_L_SUBFR];          /* Long term prediction residual         */
    Word16 code[EFR_L_SUBFR];          /* Fixed codebook excitation             */
    Word16 y1[EFR_L_SUBFR];            /* Filtered adaptive excitation          */
    Word16 y2[EFR_L_SUBFR];            /* Filtered fixed codebook excitation    */

    /* Scalars */

    Word16 i, j, k, i_subfr;
    Word16 T_op, T0, T0_min, T0_max, T0_frac;
    Word16 gain_pit, gain_code, pit_flag, pit_sharp;
    Word16 temp;
    Word32 L_temp;

    Word16 scal_acf, VAD_flag, lags[2], rc[4];

    extern Word16 efr_ptch;
    extern Word16 efr_txdtx_ctrl, efr_CN_excitation_gain;
    extern Word32 efr_L_pn_seed_tx;
    extern Word16 efr_dtx_mode;

    /*----------------------------------------------------------------------*
     *  - Perform LPC analysis: (twice per frame)                           *
     *       * autocorrelation + lag windowing                              *
     *       * Levinson-Durbin algorithm to find a[]                        *
     *       * convert a[] to lsp[]                                         *
     *       * quantize and code the LSPs                                   *
     *       * find the interpolated LSPs and convert to a[] for all        *
     *         subframes (both quantized and unquantized)                   *
     *----------------------------------------------------------------------*/

    /* LP analysis centered at 2nd subframe */


    scal_acf = EFR_Autocorr (efr_p_window_mid, EFR_M, r_h, r_l, window_160_80);
                                /* Autocorrelations */

    EFR_Lag_window (EFR_M, r_h, r_l);   /* Lag windowing    */

    EFR_Levinson (r_h, r_l, &A_t[EFR_MP1], rc); /* Levinson-Durbin  */

    EFR_Az_lsp (&A_t[EFR_MP1], lsp_mid, efr_lsp_old); /* From A(z) to lsp */

    /* LP analysis centered at 4th subframe */

    /* Autocorrelations */
    scal_acf = EFR_Autocorr (efr_p_window, EFR_M, r_h, r_l, window_232_8);

    EFR_Lag_window (EFR_M, r_h, r_l);   /* Lag windowing    */

    EFR_Levinson (r_h, r_l, &A_t[EFR_MP1 * 3], rc); /* Levinson-Durbin  */

    EFR_Az_lsp (&A_t[EFR_MP1 * 3], lsp_new, lsp_mid); /* From A(z) to lsp */

    if (efr_dtx_mode == 1)
    {
        /* DTX enabled, make voice activity decision */
        VAD_flag = EFR_Vad_computation (r_h, r_l, scal_acf, rc, efr_ptch);


        EFR_tx_dtx (VAD_flag, &efr_txdtx_ctrl); /* TX DTX handler */
    }
    else
    {
        /* DTX disabled, active speech in every frame */
        VAD_flag = 1;
        efr_txdtx_ctrl = EFR_TX_VAD_FLAG | EFR_TX_SP_FLAG;
    }

    /* LSP quantization (lsp_mid[] and lsp_new[] jointly quantized) */

    EFR_Q_plsf_5 (lsp_mid, lsp_new, lsp_mid_q, lsp_new_q, ana, efr_txdtx_ctrl);

    ana += 5;

    /*--------------------------------------------------------------------*
     * Find interpolated LPC parameters in all subframes (both quantized  *
     * and unquantized).                                                  *
     * The interpolated parameters are in array A_t[] of size (EFR_M+1)*4     *
     * and the quantized interpolated parameters are in array Aq_t[]      *
     *--------------------------------------------------------------------*/

    EFR_Int_lpc2 (efr_lsp_old, lsp_mid, lsp_new, A_t);

    if ((efr_txdtx_ctrl & EFR_TX_SP_FLAG) != 0)
    {
        EFR_Int_lpc (efr_lsp_old_q, lsp_mid_q, lsp_new_q, Aq_t);

        /* update the LSPs for the next frame */
        CODEC_OpVecCpy(efr_lsp_old, lsp_new, EFR_M);
        CODEC_OpVecCpy(efr_lsp_old_q, lsp_new_q, EFR_M);
    }
    else
    {
        /* Use unquantized LPC parameters in case of no speech activity */

        CODEC_OpVecCpy(Aq_t, A_t, 4*EFR_MP1);
        /* update the LSPs for the next frame */
        CODEC_OpVecCpy(efr_lsp_old, lsp_new, EFR_M);
        CODEC_OpVecCpy(efr_lsp_old_q, lsp_new, EFR_M);
    }

    /*----------------------------------------------------------------------*
     * - Find the weighted input speech efr_wsp[] for the whole speech frame    *
     * - Find the open-loop pitch delay for first 2 subframes               *
     * - Set the range for searching closed-loop pitch in 1st subframe      *
     * - Find the open-loop pitch delay for last 2 subframes                *
     *----------------------------------------------------------------------*/

    A = A_t;
    for (i = 0; i < EFR_L_FRAME; i += EFR_L_SUBFR)
    {
        EFR_Weight_Ai (A, efr_F_gamma1, Ap1);

        EFR_Weight_Ai (A, efr_F_gamma2, Ap2);

        EFR_Residu (Ap1, &efr_speech[i], &efr_wsp[i], EFR_L_SUBFR);

        EFR_Syn_filt (Ap2, &efr_wsp[i], &efr_wsp[i], EFR_L_SUBFR, efr_mem_w, 1);

        A += EFR_MP1;
    }

    /* Find open loop pitch lag for first two subframes */

    T_op = EFR_Pitch_ol (efr_wsp, EFR_PIT_MIN, EFR_PIT_MAX, EFR_L_FRAME_BY2);

    lags[0] = T_op;


    if ((efr_txdtx_ctrl & EFR_TX_SP_FLAG) != 0)
    {
        /* Range for closed loop pitch search in 1st subframe */

        T0_min = sub (T_op, 3);

        if (sub (T0_min, EFR_PIT_MIN) < 0)
        {
            T0_min = EFR_PIT_MIN;
        }
        T0_max = add (T0_min, 6);

        if (sub (T0_max, EFR_PIT_MAX) > 0)
        {
            T0_max = EFR_PIT_MAX;
            T0_min = sub (T0_max, 6);
        }
    }
    /* Find open loop pitch lag for last two subframes */

    T_op = EFR_Pitch_ol (&efr_wsp[EFR_L_FRAME_BY2], EFR_PIT_MIN, EFR_PIT_MAX, EFR_L_FRAME_BY2);

    if (efr_dtx_mode == 1)
    {
        lags[1] = T_op;
        EFR_Periodicity_update (lags, &efr_ptch);
    }
    /*----------------------------------------------------------------------*
     *          Loop for every subframe in the analysis frame               *
     *----------------------------------------------------------------------*
     *  To find the pitch and innovation parameters. The subframe size is   *
     *  EFR_L_SUBFR and the loop is repeated EFR_L_FRAME/EFR_L_SUBFR times.             *
     *     - find the weighted LPC coefficients                             *
     *     - find the LPC residual signal res[]                             *
     *     - compute the target signal for pitch search                     *
     *     - compute impulse response of weighted synthesis filter (efr_h1[])   *
     *     - find the closed-loop pitch parameters                          *
     *     - encode the pitch delay                                         *
     *     - update the impulse response efr_h1[] by including pitch            *
     *     - find target vector for codebook search                         *
     *     - codebook search                                                *
     *     - encode codebook address                                        *
     *     - VQ of pitch and codebook gains                                 *
     *     - find synthesis speech                                          *
     *     - update states of weighting filter                              *
     *----------------------------------------------------------------------*/

    /* pointer to interpolated LPC parameters          */
    A = A_t;
    /* pointer to interpolated quantized LPC parameters */
    Aq = Aq_t;

    for (i_subfr = 0; i_subfr < EFR_L_FRAME; i_subfr += EFR_L_SUBFR)
    {


        if ((efr_txdtx_ctrl & EFR_TX_SP_FLAG) != 0)
        {

            /*---------------------------------------------------------------*
             * Find the weighted LPC coefficients for the weighting filter.  *
             *---------------------------------------------------------------*/

            EFR_Weight_Ai (A, efr_F_gamma1, Ap1);

            EFR_Weight_Ai (A, efr_F_gamma2, Ap2);

            /*---------------------------------------------------------------*
             * Compute impulse response, efr_h1[], of weighted synthesis filter  *
             *---------------------------------------------------------------*/

            CODEC_OpVecCpy(efr_ai_zero, Ap1, EFR_M + 1);
            EFR_Syn_filt (Aq, efr_ai_zero, efr_h1, EFR_L_SUBFR, efr_zero, 0);

            EFR_Syn_filt (Ap2, efr_h1, efr_h1, EFR_L_SUBFR, efr_zero, 0);

        }
        /*---------------------------------------------------------------*
         *          Find the target vector for pitch search:             *
         *---------------------------------------------------------------*/

        EFR_Residu (Aq, &efr_speech[i_subfr], res2, EFR_L_SUBFR);   /* LPC residual */

        if ((efr_txdtx_ctrl & EFR_TX_SP_FLAG) == 0)
        {
            /* Compute comfort noise excitation gain based on
            LP residual energy */

            efr_CN_excitation_gain = EFR_compute_CN_excitation_gain (res2);

        }
        else
        {
            CODEC_OpVecCpy (&efr_exc[i_subfr], res2, EFR_L_SUBFR);

            EFR_Syn_filt (Aq, &efr_exc[i_subfr], efr_error, EFR_L_SUBFR, efr_mem_err, 0);

            EFR_Residu (Ap1, efr_error, xn, EFR_L_SUBFR);

            EFR_Syn_filt (Ap2, xn, xn, EFR_L_SUBFR, efr_mem_w0, 0); /* target signal xn[]*/

            /*--------------------------------------------------------------*
             *                 Closed-loop fractional pitch search          *
             *--------------------------------------------------------------*/

            /* flag for first and 3th subframe */
            pit_flag = i_subfr;

            /* set t0_min and t0_max for 3th subf.*/
            if (sub (i_subfr, EFR_L_FRAME_BY2) == 0)
            {
                T0_min = sub (T_op, 3);


                if (sub (T0_min, EFR_PIT_MIN) < 0)
                {
                    T0_min = EFR_PIT_MIN;
                }
                T0_max = add (T0_min, 6);

                if (sub (T0_max, EFR_PIT_MAX) > 0)
                {
                    T0_max = EFR_PIT_MAX;
                    T0_min = sub (T0_max, 6);
                }
                pit_flag = 0;
            }
            T0 = EFR_Pitch_fr6 (&efr_exc[i_subfr], xn, efr_h1, EFR_L_SUBFR, T0_min, T0_max,
                            pit_flag, &T0_frac);

            *ana = EFR_Enc_lag6 (T0, &T0_frac, &T0_min, &T0_max, EFR_PIT_MIN,
                             EFR_PIT_MAX, pit_flag);

        }
        ana++;
        /* Incrementation of ana is done here to work also
        when no speech activity is present */



        if ((efr_txdtx_ctrl & EFR_TX_SP_FLAG) != 0)
        {

            /*---------------------------------------------------------------*
             * - find unity gain pitch excitation (adaptive codebook entry)  *
             *   with fractional interpolation.                              *
             * - find filtered pitch efr_exc. y1[]=efr_exc[] convolved with efr_h1[]     *
             * - compute pitch gain and limit between 0 and 1.2              *
             * - update target vector for codebook search                    *
             * - find LTP residual.                                          *
             *---------------------------------------------------------------*/

            EFR_Pred_lt_6 (&efr_exc[i_subfr], T0, T0_frac, EFR_L_SUBFR);

            EFR_Convolve (&efr_exc[i_subfr], efr_h1, y1, EFR_L_SUBFR);

            gain_pit = EFR_G_pitch (xn, y1, EFR_L_SUBFR);

            *ana = EFR_q_gain_pitch (&gain_pit);

        }
        else
        {
            gain_pit = 0;
        }

        ana++;                  /* Incrementation of ana is done here to work
                                   also when no speech activity is present */



        if ((efr_txdtx_ctrl & EFR_TX_SP_FLAG) != 0)
        {
            /* xn2[i]   = xn[i] - y1[i] * gain_pit  */
            /* res2[i] -= efr_exc[i+i_subfr] * gain_pit */

            for (i = 0; i < EFR_L_SUBFR; i++)
            {
                L_temp = L_mult (y1[i], gain_pit);
                L_temp = L_shl (L_temp, 3);
                xn2[i] = sub (xn[i], extract_h (L_temp));

                L_temp = L_mult (efr_exc[i + i_subfr], gain_pit);
                L_temp = L_shl (L_temp, 3);
                res2[i] = sub (res2[i], extract_h (L_temp));
            }

            /*-------------------------------------------------------------*
             * - include pitch contribution into impulse resp. efr_h1[]        *
             *-------------------------------------------------------------*/

            /* pit_sharp = gain_pit;                   */
            /* if (pit_sharp > 1.0) pit_sharp = 1.0;   */

            pit_sharp = shl (gain_pit, 3);

            CODEC_OpVcMultQ15Add(&efr_h1[T0], &efr_h1[0], EFR_L_SUBFR - T0, pit_sharp, &efr_h1[T0]);

            /*--------------------------------------------------------------*
             * - Innovative codebook search (find index and gain)           *
             *--------------------------------------------------------------*/

            EFR_Code_10i40_35bits (xn2, res2, efr_h1, code, y2, ana);

        }
        else
        {
            EFR_build_CN_code (code, &efr_L_pn_seed_tx);
        }
        ana += 10;


        if ((efr_txdtx_ctrl & EFR_TX_SP_FLAG) != 0)
        {

            /*-------------------------------------------------------*
             * - Add the pitch contribution to code[].               *
             *-------------------------------------------------------*/

            CODEC_OpVcMultQ15Add(code + T0, code, EFR_L_SUBFR - T0, pit_sharp, code + T0);
            /*------------------------------------------------------*
             * - Quantization of fixed codebook gain.               *
             *------------------------------------------------------*/

            gain_code = EFR_G_code (xn2, y2);

        }
        *ana++ = EFR_q_gain_code (code, EFR_L_SUBFR, &gain_code, efr_txdtx_ctrl, i_subfr);

        /*------------------------------------------------------*
         * - Find the total excitation                          *
         * - find synthesis speech corresponding to efr_exc[]       *
         * - update filter memories for finding the target      *
         *   vector in the next subframe                        *
         *   (update efr_mem_err[] and efr_mem_w0[])                    *
         *------------------------------------------------------*/

        for (i = 0; i < EFR_L_SUBFR; i++)
        {
            /* efr_exc[i] = gain_pit*efr_exc[i] + gain_code*code[i]; */

            L_temp = L_mult (efr_exc[i + i_subfr], gain_pit);
            L_temp = L_mac (L_temp, code[i], gain_code);
            L_temp = L_shl (L_temp, 3);
            efr_exc[i + i_subfr] = round (L_temp);
        }

        EFR_Syn_filt (Aq, &efr_exc[i_subfr], &synth[i_subfr], EFR_L_SUBFR, efr_mem_syn, 1);

        if ((efr_txdtx_ctrl & EFR_TX_SP_FLAG) != 0)
        {

            for (i = EFR_L_SUBFR - EFR_M, j = 0; i < EFR_L_SUBFR; i++, j++)
            {
                efr_mem_err[j] = sub (efr_speech[i_subfr + i], synth[i_subfr + i]);

                temp = extract_h (L_shl (L_mult (y1[i], gain_pit), 3));
                k = extract_h (L_shl (L_mult (y2[i], gain_code), 5));
                efr_mem_w0[j] = sub (xn[i], add (temp, k));
            }
        }
        else
        {
            CODEC_OpVecSet(efr_mem_err, EFR_M, 0);
            CODEC_OpVecSet(efr_mem_w0, EFR_M, 0);
        }

        /* interpolated LPC parameters for next subframe */
        A += EFR_MP1;
        Aq += EFR_MP1;
    }

    /*--------------------------------------------------*
     * Update signal for next frame.                    *
     * -> shift to the left by EFR_L_FRAME:                 *
     *     speech[], efr_wsp[] and  efr_exc[]                   *
     *--------------------------------------------------*/

    CODEC_OpVecCpy(&efr_old_speech[0], &efr_old_speech[EFR_L_FRAME], EFR_L_TOTAL - EFR_L_FRAME);

    CODEC_OpVecCpy(&efr_old_wsp[0], &efr_old_wsp[EFR_L_FRAME], EFR_PIT_MAX);

    CODEC_OpVecCpy(&efr_old_exc[0], &efr_old_exc[EFR_L_FRAME], EFR_PIT_MAX + EFR_L_INTERPOL);
    return;
}

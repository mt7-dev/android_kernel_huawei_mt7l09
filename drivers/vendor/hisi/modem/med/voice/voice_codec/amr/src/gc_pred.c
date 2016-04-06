/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
*****************************************************************************
*
*      File             : gc_pred.c
*      Purpose          : codebook gain MA prediction
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "gc_pred.h"

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amr_comm.h"
#include "cnst.h"
#include "log2.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/* 内存中结构体的个数上限 */
#define AMR_MAX_GC_PREDSTATE_NUM 3

/* gc_predState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
gc_predState        g_stAmrGcPredState[AMR_MAX_GC_PREDSTATE_NUM];

#define NPRED 4  /* number of prediction taps */

/* MA prediction coefficients (Q13) */
static const Word16 pred[NPRED] = {5571, 4751, 2785, 1556};

/* average innovation energy.                               */
/* MEAN_ENER  = 36.0/constant, constant = 20*Log10(2)       */
#define MEAN_ENER_MR122  783741L  /* 36/(20*log10(2)) (Q17) */

/* MA prediction coefficients (Q6)  */
static const Word16 pred_MR122[NPRED] = {44, 37, 22, 12};

/* minimum quantized energy: -14 dB */
#define MIN_ENERGY       -14336       /* 14                 Q10 */
#define MIN_ENERGY_MR122  -2381       /* 14 / (20*log10(2)) Q10 */
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   qua_gain_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/

int gc_pred_enc_init (gc_predState **state1,gc_predState **state2)
{
  gc_predState* s1;
  gc_predState* s2;

  if ((state1 == (gc_predState **) AMR_NULL)||(state2 == (gc_predState **) AMR_NULL))
  {
      return -1;
  }

  *state1 = AMR_NULL;
  *state2 = AMR_NULL;

  /* allocate memory */
  s1 = &g_stAmrGcPredState[0];
  s2 = &g_stAmrGcPredState[1];

  gc_pred_reset(s1);
  gc_pred_reset(s2);

  *state1 = s1;
  *state2 = s2;

  return 0;
}

/*************************************************************************
*
*  Function:   qua_gain_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/

int gc_pred_dec_init (gc_predState **state)
{
  gc_predState* s;

  if (state == (gc_predState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrGcPredState[2];

  gc_pred_reset(s);
  *state = s;

  return 0;
}

/*************************************************************************
*
*  Function:   gc_pred_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int gc_pred_reset (gc_predState *state)
{
   Word16 i;

   if (state == (gc_predState *) AMR_NULL){
       return -1;
   }

   for(i = 0; i < NPRED; i++)
   {
      state->past_qua_en[i] = MIN_ENERGY;
      state->past_qua_en_MR122[i] = MIN_ENERGY_MR122;
   }
  return 0;
}

/*************************************************************************
*
*  Function:   gc_pred_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void gc_pred_exit (gc_predState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  *state = AMR_NULL;

  return;
}

/*************************************************************************
 *
 * FUNCTION:  gc_pred_copy()
 *
 * PURPOSE: Copy MA predictor state variable
 *
 *************************************************************************/
void
gc_pred_copy(
    gc_predState *st_src,  /* i : State struct                           */
    gc_predState *st_dest  /* o : State struct                           */
)
{
    CODEC_OpVecCpy(st_dest->past_qua_en,
                 st_src->past_qua_en,
                 NPRED);

    CODEC_OpVecCpy(st_dest->past_qua_en_MR122,
                 st_src->past_qua_en_MR122,
                 NPRED);
}

/*************************************************************************
 *
 * FUNCTION:  gc_pred()
 *
 * PURPOSE: MA prediction of the innovation energy
 *          (in dB/(20*log10(2))) with mean  removed).
 *
 *************************************************************************/
void
gc_pred(
    gc_predState *st,   /* i/o: State struct                           */
    enum Mode mode,     /* i  : AMR mode                               */
    Word16 *code,       /* i  : innovative codebook vector (L_SUBFR)   */
                        /*      MR122: Q12, other modes: Q13           */
    Word16 *exp_gcode0, /* o  : exponent of predicted gain factor, Q0  */
    Word16 *frac_gcode0,/* o  : fraction of predicted gain factor  Q15 */
    Word16 *exp_en,     /* o  : exponent of innovation energy,     Q0  */
                        /*      (only calculated for MR795)            */
    Word16 *frac_en     /* o  : fraction of innovation energy,     Q15 */
                        /*      (only calculated for MR795)            */
)
{
    Word32 ener_code;
    Word16 exp, frac;

    /*-------------------------------------------------------------------*
     *  energy of code:                                                  *
     *  ~~~~~~~~~~~~~~~                                                  *
     *  ener_code = sum(code[i]^2)                                       *
     *-------------------------------------------------------------------*/
    ener_code = CODEC_OpVvMac( code, code, L_SUBFR, 0);

    if ( MR122 == mode)
    {
        Word32 ener;

        /* ener_code = ener_code / lcode; lcode = 40; 1/40 = 26214 Q20       */
        ener_code = L_mult (round (ener_code), 26214);   /* Q9  * Q20 -> Q30 */

        /*-------------------------------------------------------------------*
         *  energy of code:                                                  *
         *  ~~~~~~~~~~~~~~~                                                  *
         *  ener_code(Q17) = 10 * Log10(energy) / constant                   *
         *                 = 1/2 * Log2(energy)                              *
         *                                           constant = 20*Log10(2)  *
         *-------------------------------------------------------------------*/
        /* ener_code = 1/2 * Log2(ener_code); Note: Log2=log2+30 */
        Log2(ener_code, &exp, &frac);
        ener_code = L_Comp (sub (exp, 30), frac);     /* Q16 for log()    */
                                                    /* ->Q17 for 1/2 log()*/

        /*-------------------------------------------------------------------*
         *  predicted energy:                                                *
         *  ~~~~~~~~~~~~~~~~~                                                *
         *  ener(Q24) = (Emean + sum{pred[i]*past_en[i]})/constant           *
         *            = MEAN_ENER + sum(pred[i]*past_qua_en[i])              *
         *                                           constant = 20*Log10(2)  *
         *-------------------------------------------------------------------*/
        ener = CODEC_OpVvMac(&st->past_qua_en_MR122[0],
                           &pred_MR122[0],
                           NPRED,
                           MEAN_ENER_MR122);

        /*-------------------------------------------------------------------*
         *  predicted codebook gain                                          *
         *  ~~~~~~~~~~~~~~~~~~~~~~~                                          *
         *  gc0     = Pow10( (ener*constant - ener_code*constant) / 20 )     *
         *          = Pow2(ener-ener_code)                                   *
         *          = Pow2(int(d)+frac(d))                                   *
         *                                                                   *
         *  (store exp and frac for pow2())                                  *
         *-------------------------------------------------------------------*/

        ener = L_shr (L_sub (ener, ener_code), 1);                /* Q16 */
        L_Extract(ener, exp_gcode0, frac_gcode0);
    }
    else /* all modes except 12.2 */
    {
        Word32 L_tmp;
        Word16 exp_code, gcode0;

        /*-----------------------------------------------------------------*
         *  Compute: means_ener - 10log10(ener_code/ L_sufr)               *
         *-----------------------------------------------------------------*/

        exp_code = norm_l (ener_code);
        ener_code = L_shl (ener_code, exp_code);

        /* Log2 = log2 + 27 */
        Log2_norm (ener_code, exp_code, &exp, &frac);

        /* fact = 10/log2(10) = 3.01 = 24660 Q13 */
        L_tmp = Mpy_32_16(exp, frac, -24660); /* Q0.Q15 * Q13 -> Q14 */


        if ( MR102 == mode)
        {
            /* mean = 33 dB */
            L_tmp = L_mac(L_tmp, 16678, 64);     /* Q14 */
        }
        else if ( MR795 == mode)
        {
            /* ener_code  = <xn xn> * 2^27*2^exp_code
               frac_en    = ener_code / 2^16
                          = <xn xn> * 2^11*2^exp_code
               <xn xn>    = <xn xn>*2^11*2^exp * 2^exp_en
                         := frac_en            * 2^exp_en

               ==> exp_en = -11-exp_code;
             */
            *frac_en = extract_h (ener_code);
            *exp_en = sub (-11, exp_code);

            /* mean = 36 dB */
            L_tmp = L_mac(L_tmp, 17062, 64);     /* Q14 */
        }
        else if ( MR74 == mode)
        {
            /* mean = 30 dB */
            L_tmp = L_mac(L_tmp, 32588, 32);     /* Q14 */
        }
        else if ( MR67 == mode)
        {
            /* mean = 28.75 dB */
            L_tmp = L_mac(L_tmp, 32268, 32);     /* Q14 */
        }
        else /* MR59, MR515, MR475 */
        {
            /* mean = 33 dB */
            L_tmp = L_mac(L_tmp, 16678, 64);     /* Q14 */
        }

        /*-----------------------------------------------------------------*
         * Compute gcode0.                                                 *
         *  = Sum(i=0,3) pred[i]*past_qua_en[i] - ener_code + mean_ener    *
         *-----------------------------------------------------------------*/

        L_tmp = L_shl(L_tmp, 10);                /* Q24 */

        L_tmp = CODEC_OpVvMac(&pred[0],            /* Q13 * Q10 -> Q24 */
                            &st->past_qua_en[0],
                            4,
                            L_tmp);

        gcode0 = extract_h(L_tmp);               /* Q8  */

        /*-----------------------------------------------------------------*
         * gcode0 = pow(10.0, gcode0/20)                                   *
         *        = pow(2, 3.3219*gcode0/20)                               *
         *        = pow(2, 0.166*gcode0)                                   *
         *-----------------------------------------------------------------*/


        if ( MR74 == mode) /* For IS641 bitexactness */
            L_tmp = L_mult(gcode0, 5439);  /* Q8 * Q15 -> Q24 */
        else
            L_tmp = L_mult(gcode0, 5443);  /* Q8 * Q15 -> Q24 */

        L_tmp = L_shr(L_tmp, 8);                   /*          -> Q16 */
        L_Extract(L_tmp, exp_gcode0, frac_gcode0); /*       -> Q0.Q15 */
    }
}

/*************************************************************************
 *
 * FUNCTION:  gc_pred_update()
 *
 * PURPOSE: update MA predictor with last quantized energy
 *
 *************************************************************************/
void gc_pred_update(
    gc_predState *st,      /* i/o: State struct                     */
    Word16 qua_ener_MR122, /* i  : quantized energy for update, Q10 */
                           /*      (log2(qua_err))                  */
    Word16 qua_ener        /* i  : quantized energy for update, Q10 */
                           /*      (20*log10(qua_err))              */
)
{
    Word16 i;

    for (i = 3; i > 0; i--)
    {
        st->past_qua_en[i] = st->past_qua_en[i - 1];
        st->past_qua_en_MR122[i] = st->past_qua_en_MR122[i - 1];
    }

    st->past_qua_en_MR122[0] = qua_ener_MR122;  /*    log2 (qua_err), Q10 */

    st->past_qua_en[0] = qua_ener;              /* 20*log10(qua_err), Q10 */

}

/*************************************************************************
 *
 * FUNCTION:  gc_pred_average_limited()
 *
 * PURPOSE: get average of MA predictor state values (with a lower limit)
 *          [used in error concealment]
 *
 *************************************************************************/
void gc_pred_average_limited(
    gc_predState *st,       /* i: State struct                    */
    Word16 *ener_avg_MR122, /* o: everaged quantized energy,  Q10 */
                            /*    (log2(qua_err))                 */
    Word16 *ener_avg        /* o: averaged quantized energy,  Q10 */
                            /*    (20*log10(qua_err))             */
)
{
    Word16 av_pred_en;

    /* do average in MR122 mode (log2() domain) */
    av_pred_en = CODEC_OpVecSum(&st->past_qua_en_MR122[0], NPRED);

    /* av_pred_en = 0.25*av_pred_en */
    av_pred_en = mult (av_pred_en, 8192);

    /* if (av_pred_en < -14/(20Log10(2))) av_pred_en = .. */

    if ( av_pred_en < (Word16)MIN_ENERGY_MR122 )
    {
        av_pred_en = MIN_ENERGY_MR122;
    }
    *ener_avg_MR122 = av_pred_en;

    /* do average for other modes (20*log10() domain) */
    av_pred_en = CODEC_OpVecSum(&st->past_qua_en[0], NPRED);

    /* av_pred_en = 0.25*av_pred_en */
    av_pred_en = mult (av_pred_en, 8192);

    /* if (av_pred_en < -14) av_pred_en = .. */

    if ( av_pred_en < (Word16)MIN_ENERGY )
    {
        av_pred_en = MIN_ENERGY;
    }
    *ener_avg = av_pred_en;
}

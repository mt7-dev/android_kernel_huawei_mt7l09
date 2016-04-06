/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
*****************************************************************************
*
*      File             : agc.c
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "agc.h"
#include "amr_comm.h"

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "cnst.h"
#include "inv_sqrt.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/* agcState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
agcState g_stAmrAgcState;

/*
*****************************************************************************
*                         LOCAL PROGRAM CODE
*****************************************************************************
*/

static Word32 energy_old( /* o : return energy of signal     */
    Word16 in[],          /* i : input signal (length l_trm) */
    Word16 l_trm          /* i : signal length               */
)
{
    Word32 s;
    Word16 i, temp;

    temp = shr (in[0], 2);
    s = L_mult (temp, temp);

    for (i = 1; i < l_trm; i++)
    {
        temp = shr (in[i], 2);
        s = L_mac (s, temp, temp);
    }

    return s;
}

static Word32 energy_new( /* o : return energy of signal     */
    Word16 in[],          /* i : input signal (length l_trm) */
    Word16 l_trm          /* i : signal length               */
)
{
    Word32 s;
    Flag ov_save;

    ov_save = CODEC_OpGetOverflow();

    s = CODEC_OpVvMac(in,in,l_trm,0);

    /* check for overflow */

    if ( MAX_32 == s )
    {
        if(0 == ov_save)/* restore overflow flag */
        {
            CODEC_OpSetOverflow(0);
        }
        else
        {
            CODEC_OpSetOverflow(1);
        }

        s = energy_old (in, l_trm);  /* function result */
    }
    else
    {
       s = L_shr(s, 4);
    }

    return s;
}
/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*
**************************************************************************
*
*  Function    : agc_init
*  Purpose     : Allocates memory for agc state and initializes
*                state memory
*
**************************************************************************
*/
int agc_init (agcState **state)
{
  agcState* s;

  if (state == (agcState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrAgcState;

  agc_reset(s);
  *state = s;

  return 0;
}

/*
**************************************************************************
*
*  Function    : agc_reset
*  Purpose     : Reset of agc (i.e. set state memory to 1.0)
*
**************************************************************************
*/
int agc_reset (agcState *state)
{
  if (state == (agcState *) AMR_NULL){
      return -1;
  }

  state->past_gain = 4096;   /* initial value of past_gain = 1.0  */

  return 0;
}

/*
**************************************************************************
*
*  Function    : agc_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void agc_exit (agcState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  *state = AMR_NULL;

  return;
}

/*
**************************************************************************
*
*  Function    : agc
*  Purpose     : Scales the postfilter output on a subframe basis
*
**************************************************************************
*/
int agc (
    agcState *st,      /* i/o : agc state                        */
    Word16 *sig_in,    /* i   : postfilter input signal  (l_trm) */
    Word16 *sig_out,   /* i/o : postfilter output signal (l_trm) */
    Word16 agc_fac,    /* i   : AGC factor                       */
    Word16 l_trm       /* i   : subframe size                    */
)
{
    Word16 i, exp;
    Word16 gain_in, gain_out, g0, gain;
    Word32 s;

    /* calculate gain_out with exponent */
    s = energy_new(sig_out, l_trm);  /* function result */

    if (s == 0)
    {
        st->past_gain = 0;
        return 0;
    }
    exp = sub (norm_l (s), 1);
    gain_out = round (L_shl (s, exp));

    /* calculate gain_in with exponent */
    s = energy_new(sig_in, l_trm);    /* function result */

    if (s == 0)
    {
        g0 = 0;
    }
    else
    {
        i = norm_l (s);
        gain_in = round (L_shl (s, i));
        exp = sub (exp, i);

        /*---------------------------------------------------*
         *  g0 = (1-agc_fac) * sqrt(gain_in/gain_out);       *
         *---------------------------------------------------*/

        s = L_deposit_l (div_s (gain_out, gain_in));
        s = L_shl (s, 7);       /* s = gain_out / gain_in */
        s = L_shr (s, exp);     /* add exponent */

        s = Inv_sqrt (s);  /* function result */
        i = round (L_shl (s, 9));

        /* g0 = i * (1-agc_fac) */
        g0 = mult (i, sub (32767, agc_fac));
    }

    /* compute gain[n] = agc_fac * gain[n-1]
                        + (1-agc_fac) * sqrt(gain_in/gain_out) */
    /* sig_out[n] = gain[n] * sig_out[n]                        */

    gain = st->past_gain;

    for (i = 0; i < l_trm; i++)
    {
        gain = mult (gain, agc_fac);
        gain = add (gain, g0);
        sig_out[i] = extract_h (L_shl (L_mult (sig_out[i], gain), 3));

    }

    st->past_gain = gain;

    return 0;
}

/*
**************************************************************************
*
*  Function    : agc2
*  Purpose     : Scales the excitation on a subframe basis
*
**************************************************************************
*/
void agc2 (
 Word16 *sig_in,        /* i   : postfilter input signal  */
 Word16 *sig_out,       /* i/o : postfilter output signal */
 Word16 l_trm           /* i   : subframe size            */
)
{
    Word16 i, exp;
    Word16 gain_in, gain_out, g0;
    Word32 s;

    /* calculate gain_out with exponent */
    s = energy_new(sig_out, l_trm);    /* function result */

    if (s == 0)
    {
        return;
    }
    exp = sub (norm_l (s), 1);
    gain_out = round (L_shl (s, exp));

    /* calculate gain_in with exponent */
    s = energy_new(sig_in, l_trm);    /* function result */

    if (s == 0)
    {
        g0 = 0;
    }
    else
    {
        i = norm_l (s);
        gain_in = round (L_shl (s, i));
        exp = sub (exp, i);

        /*---------------------------------------------------*
         *  g0 = sqrt(gain_in/gain_out);                     *
         *---------------------------------------------------*/

        s = L_deposit_l (div_s (gain_out, gain_in));
        s = L_shl (s, 7);       /* s = gain_out / gain_in */
        s = L_shr (s, exp);     /* add exponent */

        s = Inv_sqrt (s);  /* function result */
        g0 = round (L_shl (s, 9));
    }

    /* sig_out(n) = gain(n) sig_out(n) */

    for (i = 0; i < l_trm; i++)
    {
        sig_out[i] = extract_h (L_shl (L_mult (sig_out[i], g0), 3));

    }

    return;
}

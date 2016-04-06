/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0
*                                REL-4 Version 4.1.0
*
********************************************************************************
*
*      File             : preemph.c
*      Purpose          : Preemphasis filtering
*      Description      : Filtering through 1 - g z^-1
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "preemph.h"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "codec_op_etsi.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/* preemphasisState 状态全局变量,在本模块的init函数中作为预先分配的内存替换malloc */
preemphasisState g_stAmrPreEmphasisState;

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   Post_Filter_init
*  Purpose:    Allocates memory for filter structure and initializes
*              state memory
*
**************************************************************************
*/
int preemphasis_init (preemphasisState **state)
{
  preemphasisState* s;

  if (state == (preemphasisState **) AMR_NULL){
      return -1;
  }
  *state = AMR_NULL;

  /* allocate memory */
  s = &g_stAmrPreEmphasisState;

  preemphasis_reset(s);
  *state = s;

  return 0;
}

/*************************************************************************
*
*  Function:   preemphasis_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int preemphasis_reset (preemphasisState *state)
{
  if (state == (preemphasisState *) AMR_NULL){
      return -1;
  }

  state->mem_pre = 0;

  return 0;
}

/*************************************************************************
*
*  Function:   preemphasis_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void preemphasis_exit (preemphasisState **state)
{
  if (state == AMR_NULL || *state == AMR_NULL)
      return;

  *state = AMR_NULL;

  return;
}

/*
**************************************************************************
*  Function:  preemphasis
*  Purpose:   Filtering through 1 - g z^-1
*
**************************************************************************
*/
int preemphasis (
    preemphasisState *st, /* (i/o)  : preemphasis filter state    */
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

    *p1 = sub (*p1, mult (g, st->mem_pre));

    st->mem_pre = temp;

    return 0;
}

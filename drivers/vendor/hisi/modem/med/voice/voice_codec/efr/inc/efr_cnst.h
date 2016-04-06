/*--------------------------------------------------------------------------*
*       Codec constant parameters (coder, decoder, and postfilter)          *
*---------------------------------------------------------------------------*/
/*lint --e(760)*/
#define  EFR_L_TOTAL      320       /* Total size of speech buffer.             */
#define  EFR_L_WINDOW     240       /* Window size in LP analysis               */
#define  EFR_L_FRAME      160       /* Frame size                               */
#define  EFR_L_FRAME_BY2  80        /* Frame size divided by 2                  */
#define  EFR_L_SUBFR      40        /* Subframe size                            */
#define  EFR_M            10        /* Order of LP filter                       */
#define  EFR_MP1          (EFR_M+1)     /* Order of LP filter + 1                   */
#define  EFR_AZ_SIZE      ((4*EFR_M)+4)   /* Size of array of LP filters in 4 subfr.s */
#define  EFR_PIT_MIN      18        /* Minimum pitch lag                        */
#define  EFR_PIT_MAX      143       /* Maximum pitch lag                        */
#define  EFR_L_INTERPOL   (10+1)    /* Length of filter for interpolation       */

#define  EFR_PRM_SIZE     57        /* Size of vector of analysis parameters    */
#define  EFR_SERIAL_SIZE  (244+1)   /* bits per frame + bfi                     */

#define  EFR_MU       26214         /* Factor for tilt compensation filter 0.8  */
#define  EFR_AGC_FAC  29491         /* Factor for automatic gain control 0.9    */

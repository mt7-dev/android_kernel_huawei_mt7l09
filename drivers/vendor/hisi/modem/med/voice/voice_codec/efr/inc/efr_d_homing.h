/**************************************************************************
 *
 *   File Name:  d_homing.h
 *
 *   Purpose:   Contains the prototypes for all the functions of
 *              decoder homing.
 *
 **************************************************************************/


#define D_HOMING

/* Function Prototypes */

Word16 EFR_decoder_homing_frame_test (Word16 parm[], Word16 nbr_of_params);

void EFR_decoder_reset (void);

void EFR_reset_dec (void);

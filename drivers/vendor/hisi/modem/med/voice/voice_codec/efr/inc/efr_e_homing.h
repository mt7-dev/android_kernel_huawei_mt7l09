/**************************************************************************
 *
 *   File Name:  e_homing.h
 *
 *   Purpose:   Contains the prototypes for all the functions of
 *              encoder homing.
 *
 **************************************************************************/

#define EFR_EHF_MASK 0x0008 /* Encoder Homing Frame pattern */

/* Function Prototypes */

Word16 EFR_encoder_homing_frame_test (Word16 input_frame[]);

void EFR_encoder_reset (void);

void EFR_reset_enc (void);

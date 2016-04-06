/*--------------------------------------------------------------------------*
 *                         amrwb_bits.h                                           *
 *--------------------------------------------------------------------------*
 *       Number of bits for different modes			                        *
 *--------------------------------------------------------------------------*/

#include "codec_op_etsi.h"
#include "codec_op_vec.h"
#include "amrwb_cnst.h"
#include "amrwb_dtx.h"

#define NBBITS_7k     132                  /* 6.60k  */
#define NBBITS_9k     177                  /* 8.85k  */
#define NBBITS_12k    253                  /* 12.65k */
#define NBBITS_14k    285                  /* 14.25k */
#define NBBITS_16k    317                  /* 15.85k */
#define NBBITS_18k    365                  /* 18.25k */
#define NBBITS_20k    397                  /* 19.85k */
#define NBBITS_23k    461                  /* 23.05k */
#define NBBITS_24k    477                  /* 23.85k */

#define NBBITS_SID    35
#define NB_BITS_MAX   NBBITS_24k

#ifdef _AMRWB_ST_
#define BIT_0     ((Word16)-127)
#define BIT_1     ((Word16)127)
#else
#define BIT_0     ((Word16)0)
#define BIT_1     ((Word16)1)
#endif

extern const Word16 nb_of_bits[NUM_OF_MODES];

typedef struct
{
    Word16 sid_update_counter;
    Word16 sid_handover_debt;
    Word16 prev_ft;
} TX_State;


typedef struct
{
    Word16 prev_ft;
    Word16 prev_mode;
} RX_State;

extern TX_State g_stAmrwbTxState;
extern RX_State g_stAmrwbRxState;

void Reset_tx_state(void);
void Reset_rx_state(void);
void wb_sid_sync(TX_State *st, Word16 mode, enum AMRWB_TXFrameType *penTxFrmType);

void Parm_serial(
     Word16 value,                         /* input : parameter value */
     Word16 no_of_bits,                    /* input : number of bits  */
     Word16 ** prms
);
Word16 Serial_parm(                        /* Return the parameter    */
     Word16 no_of_bits,                    /* input : number of bits  */
     Word16 ** prms
);

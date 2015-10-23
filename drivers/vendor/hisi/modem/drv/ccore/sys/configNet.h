/* configNet.h - network configuration header */

/* Copyright 1999-2000 Wind River Systems, Inc. */
/* Copyright 1999-2000 ARM Limited */

/*
modification history
--------------------
01e,16jul02,m_h  C++ protection
01d,20nov00,jpd  reworked endDevTbl definitions to be dynamically filled in.
01c,07feb00,jpd  updated copyright message.
01b,13jan00,pr   added DEC entry.
01a,10nov99,ajb  Copied from pid7t version 01c.
*/

#ifndef INCconfigNeth
#define INCconfigNeth

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "end.h"



#ifdef INCLUDE_END
/*
 * The number of entries here must match INTEGRATOR_MAX_END_DEVS (excluding
 * the terminator entry)
 */

#define END_LOAD_STR "40:40" 

END_TBL_ENTRY endDevTbl [] =
{
    /* Entries that can be filled in dynamically when cards are found */
    { 0, NULL, END_LOAD_STR, 1, NULL, FALSE},
    /* End of table marker */

    { 0, END_TBL_END, NULL, 0, NULL, FALSE}
};

#endif /* INCLUDE_END */

#ifdef __cplusplus
}
#endif

#endif /* INCconfigNeth */


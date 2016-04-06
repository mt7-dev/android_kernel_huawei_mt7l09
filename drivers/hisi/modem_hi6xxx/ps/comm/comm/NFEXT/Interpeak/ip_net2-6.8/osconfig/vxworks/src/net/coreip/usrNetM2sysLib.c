/* usrNetM2SysLib.c - Initialization routine for the MIB2_SYSTEM library */

/* Copyright 1992 - 2004 Wind River Systems, Inc. */



/*
DESCRIPTION
This configlette contains the initialization routine for the
INCLUDE_MIB2_SYSTEM component. 

NOMANUAL
*/

#include <m2Lib.h>

/* externs */

#ifdef VIRTUAL_STACK
VS_REG_ID m2SysRegistrationNum;
extern STATUS m2SysDestructor (VSNUM vsnum);
#endif /* VIRTUAL_STACK */

LOCAL STATUS usrNetM2SysInit (void);

LOCAL STATUS usrNetM2SysStart (void)
    {
#ifdef VIRTUAL_STACK
    /*
     * Register with vsmanager to allow the MIB2 System group
     * to start in every virtual stack at the appropriate time.
     */

    vsComponentRegister (&m2SysRegistrationNum, "MIB2_SYS",
                         VS_SC_ENABLED, VS_SC_NON_OPTIONAL,
                         usrNetM2SysInit, 0, NULL, m2SysDestructor);
#else
    /*
     * When only one network stack exists, just
     * start the MIB2 System group immediately.
     */

    usrNetM2SysInit ();

#endif /* VIRTUAL_STACK */

    return (OK);
    }

LOCAL STATUS usrNetM2SysInit (void)
    {
    static M2_OBJECTID sysObjId = { MIB2SYS_OID_LEN, MIB2SYS_OID };
    return (m2SysInit (MIB2SYS_DESCR, MIB2SYS_CONTACT, MIB2SYS_LOCATION,
                       &sysObjId)); 
    }

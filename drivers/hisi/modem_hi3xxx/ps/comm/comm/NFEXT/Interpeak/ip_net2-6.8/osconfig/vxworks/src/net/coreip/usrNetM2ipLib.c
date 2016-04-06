/* usrNetM2IpLib.c - Initialization routine for the MIB2_IP library */

/* Copyright 1992 - 2004 Wind River Systems, Inc. */



/*
DESCRIPTION
This configlette contains the initialization routine for the
INCLUDE_MIB2_IP component.

NOMANUAL
*/

#include <m2Lib.h>

/* externs */

#ifdef VIRTUAL_STACK
VS_REG_ID m2IpRegistrationNum;
#endif /* VIRTUAL_STACK */

LOCAL STATUS usrNetM2IpInit (void);

LOCAL STATUS usrNetM2IpStart (void)
    {
#ifdef VIRTUAL_STACK
    /*
     * Register with vsmanager to allow the MIB2 IP Layer
     * to start in every virtual stack at the appropriate time.
     */

    return (vsComponentRegister (&m2IpRegistrationNum, "MIB2_IP",
                                 VS_SC_ENABLED, VS_SC_NON_OPTIONAL,
                                 usrNetM2IpInit, 0, NULL, NULL));
#else
    /*
     * When only one network stack exists, just
     * start the MIB2 IP Layer immediately.
     */

    return (usrNetM2IpInit ());

#endif /* VIRTUAL_STACK */
    }

LOCAL STATUS usrNetM2IpInit (void)
    {
    return m2IpInit (MIB2IP_MAX_ROUTE_TBL_SIZE);
    }

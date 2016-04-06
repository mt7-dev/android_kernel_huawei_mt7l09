/* usrNetGtfInit.c - GTF initialization */

/* Copyright 2002 - 2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */



/* globals */

/* externs */

extern GTF_TIMER_MANAGER * gtfTimerManagerCB;	/* global control block for
                                                   timer manager */
extern int hz;                                  /* tick rate */
extern int clkTick;                             /* clock tick rate */
extern struct timeval boottime;                 /* boottime value */

#ifdef INCLUDE_NET_DAEMON
extern void gtfDaemonInfoInit (GTF_TIMER_MANAGER * tm, JOB_QUEUE_ID jobQId,
			       int gtfHz);
#else
int gtfTaskPriority  = 50;
int gtfTaskStackSize = 10000;
int gtfTaskOptions   = VX_SUPERVISOR_MODE;

extern int gtfBookkeeping (GTF_TIMER_MANAGER *);

/****************************************************************************
*
* gtfTask - entry point for GTF task
* 
* This routine is the GTF support task. It calls gtfBookkeeping(). It is 
* spawned only when network support is not included in the image. When
* network support is included, netTask is used for GTF scheduling.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/

void gtfTask (void)
    {
    FOREVER
	{
	semTake (gtfTimerManagerCB->count, WAIT_FOREVER);
	gtfBookkeeping (gtfTimerManagerCB);
	}
    }


#endif /* INCLUDE_NET_DAEMON */

/****************************************************************************
*
* usrNetGtfInit - timer facility initialization routine
* 
* This routine initializes the timer facility
*
*/

STATUS usrNetGtfInit (void)
    {
#ifndef INCLUDE_NET_DAEMON
    int status;
#endif

    /* calculate the boot time value */
    bzero((char *)&boottime, sizeof(struct timeval));
    boottime.tv_sec = tickGet()/sysClkRateGet();

    hz = sysClkRateGet();
    clkTick = 1000000/sysClkRateGet();

    /*
     * If networking support is included, netTask will handle GTF 
     * operations. We check that netTask has initialized.
     *
     * If networking support is not included, then a new task, gtfTask,
     * is spawned which handles the GTF operations.
     */

#ifndef INCLUDE_NET_DAEMON
    status = taskSpawn ("tGtfTask", gtfTaskPriority, gtfTaskOptions,
			gtfTaskStackSize, (FUNCPTR) gtfTask, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0);
    if (status == ERROR)
	{
	if (_func_printErr)
	    (*_func_printErr) ("usrNetGtfInit: failed to spawn GTF task\n");
        return (ERROR);
	}
#endif

    /*
     * GTF processing is different depending on whether netTask or gtfTask
     * is handling operations.
     */

    gtfTimerManagerCB = gtfTimerManagerCreate (GTF_NBRTIMERUNIT_CFG,
                                               GTF_BINPOWTIMERSLOT_CFG,
                                               GTF_BINPOWGRANSCALE_CFG,
                                               GTF_SIZETIMERFREEQUEUE_CFG,
                                               GTF_HZ_CFG,
#ifdef INCLUDE_NET_DAEMON
                                               (GTF_TICK_FUNC) netGtfTickFunc,
                                               netGtfTickInformation
#else
                                               NULL, NULL
#endif
					      );

    if (gtfTimerManagerCB == NULL)
        {
        if (_func_printErr)
            (*_func_printErr) ("usrNetGtfInit: failed to create GTF_TIMER_MANAGER\n");
        return (ERROR);
        }

#ifdef INCLUDE_NET_DAEMON
    gtfDaemonInfoInit (gtfTimerManagerCB, netJobQueueId, GTF_HZ_CFG);
#endif

    return (OK);
    }

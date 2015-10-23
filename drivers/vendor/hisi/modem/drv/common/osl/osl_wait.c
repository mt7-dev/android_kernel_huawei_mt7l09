/*lint -save -e537*/
#include <vxWorks.h>
#include <vsbConfig.h>
#include <classLib.h>
#include <errno.h>
#include <taskLib.h>
#include <intLib.h>
#include <errnoLib.h>
#include <eventLib.h> 
#include <qFifoLib.h>
#include <qFifoGLib.h>
#include <qPriNode.h>
#include <intLib.h>
#include <smObjLib.h>
#include <private/eventLibP.h>
#include <private/sigLibP.h>
#include <private/objLibP.h>
#include <private/semLibP.h>
#include <private/windLibP.h>
#include <private/eventP.h>
#include <private/kernelLibP.h>
#include <private/vxPortableP.h>
#include <private/intLibP.h>

#include <private/funcBindP.h>
#include <private/taskLibP.h>
#include <private/workQLibP.h>
#include <private/readyQLibP.h>
#include <private/taskUtilLibP.h>
#include <private/intLibP.h>		/* intCnt */
#include <ffsLib.h>
#include <cpuset.h>
#include <vxCpuLib.h>
#include <private/tickLibP.h>

#ifdef	_WRS_CONFIG_SMP
#include <private/cpcLibP.h>
#include <private/vxCpuLibP.h>
#endif /* _WRS_CONFIG_SMP */

#include <osl_wait.h>
/*lint -restore +e537*/
/*lint -save -e129 */
/*lint --e{26,746,730,752, 958} */
STATUS semQueueInit
    (
    SEMAPHORE * pSemaphore,             /* pointer to semaphore to init */
    int         options,                /* semaphore options */
    int         initialCount            /* initial count */
    );
SEM_ID semQueueInitialize
    (
    char * pSemMem,		/* pointer to allocated storage */
    int    options,		/* semaphore options */
    int    initialCount		/* initial count */
    );
void semQueuePendQGet
    (
    Q_HEAD *pQHead,      /* pend queue to get first task off */
    Q_NODE *pQNode
    );
/*lint -save -e101 -e132*/

STATUS semQueuePendQPut
    (
    FAST Q_HEAD *pQHead,        /* pend queue to put taskIdCurrent on */
    FAST int    timeout,         /* timeout in ticks */
    struct osl_wait_data* entry
    );
/*lint -restore +e101 +e132*/

void semQueueGiveDefer
    (
    SEM_ID semId        /* semaphore ID to give */
    );


/*******************************************************************************
*
* semCInit - initialize a declared counting semaphore
*
* The initialization of a static counting semaphore, or a counting semaphore
* embedded in some larger object need not deal with allocation.
* This routine may be called to initialize such a semaphore.  The semaphore
* is initialized to the specified initial count.
*
* Counting semaphore options include the queuing style for blocked tasks.
* Tasks may be queued on the basis of their priority or first-in-first-out.
* These options are SEM_Q_PRIORITY and SEM_Q_FIFO respectively.
* SEM_Q_INTERRUPTIBLE is the option for waking up a blocked RTP task by sending
* a signal.
*
* RETURNS: OK, or ERROR
*
* ERRNO:
* \is
* \i S_semLib_INVALID_INITIAL_COUNT
* The specified initial count is negative
* \i S_semLib_INVALID_OPTION
* Options not applicable to counting semaphores were specified.
* \ie
*
* SEE ALSO: semCCreate
*
* \NOMANUAL
*/
/*lint -save -e40*//*lint -save -e63*/

STATUS semQueueInit
    (
    SEMAPHORE * pSemaphore,             /* pointer to semaphore to init */
    int         options,                /* semaphore options */
    int         initialCount            /* initial count */
    )
    {
    if ((options & ~(SEM_CNT_OPTIONS_MASK)) != 0)
	{
	/* coverity[NULL_RETURNS] */
	errno = S_semLib_INVALID_OPTION;
	return (ERROR);
	}

    if (initialCount < 0)
	{
	/* coverity[NULL_RETURNS] */
	errno = S_semLib_INVALID_INITIAL_COUNT;
	return (ERROR);
	}

    if (semQInit (pSemaphore, options) != OK)		/* initialize queue */
	return (ERROR);

    pSemaphore->semCount = (unsigned int)initialCount;		/* initialize count */
    pSemaphore->recurse  = 0;				/* no recursive takes */
    pSemaphore->options  = options;			/* stow away options *//*lint !e734*/
    pSemaphore->semType  = SEM_TYPE_COUNTING;		/* type is counting */

    /* initialize the events structure */

    pSemaphore->events.taskId     = (int)NULL;
    pSemaphore->events.registered = 0x0;
    pSemaphore->events.options    = 0x0;

#ifdef _WRS_CONFIG_OBJECT_LIB
    /* initialize the semaphore object core information */

    objCoreInit (&pSemaphore->objCore, semClassId);
#else
    /*
     * Make the pObjClass point to a unique class structure. The class
     * structure itself is uninitialized without objLib being present.
     * By having a unique value, it is ensured that all semaphores have
     * the same unique pObjClass value.
     */

    pSemaphore->objCore.pObjClass = semClassId;
#endif	/* _WRS_CONFIG_OBJECT_LIB */

    return (OK);
    }	

/*******************************************************************************
*
* semCInitialize - initialize a pre-allocated counting semaphore.
*
* This routine initializes a counting semaphore that has been pre-allocated
* (i.e. by the VX_COUNTING_SEMAPHORE macro).  The semaphore is initialized and
*  an ID is returned for further operations on this semaphore.
*
* The <options> and <initialCount> parameters have the same meaning as those
* for semCCreate(). Please see the documentation for semCCreate() for more
* details.
*
* The following example illustrates use of the VX_COUNTING_SEMAPHORE macro and
* this function together to instantiate a counting semaphore statically
* (without using any dynamic memory allocation):
* \cs
*      #include <vxWorks.h>
*      #include <semLib.h>
*
*      VX_COUNTING_SEMAPHORE(mySemC); /@ declare the semaphore @/
*      SEM_ID mySemCId;               /@ semaphore ID for further operations @/
*
*      STATUS initializeFunction (void)
*          {
*          if ((mySemCId = semCInitialize (mysemC, options, 0)) == NULL)
*               return (ERROR);       /@ initialization failed @/
*          else
*               return (OK);
*          }
* \ce
*
* SMP CONSIDERATIONS
* This API is spinlock and intCpuLock restricted.
*
* RETURNS: The semaphore ID, or NULL on error.
*
* ERRNO: N/A
*/

SEM_ID semQueueInitialize
    (
    char * pSemMem,		/* pointer to allocated storage */
    int    options,		/* semaphore options */
    int    initialCount		/* initial count */
    )
    {
    if ((pSemMem == NULL) ||
	(semQueueInit((SEMAPHORE *) pSemMem, options, initialCount) != OK))
	return ((SEM_ID) NULL);
    else
	return ((SEM_ID) pSemMem);
    }


#define QUEUE_PENDQ_NODE_PTR(pEntry)   (&pEntry->qNode)

#define QUEUE_NODE_PTR_TO_TCB(pNode)  \
          (struct osl_wait_data *)((int) (pNode) - OFFSET (struct osl_wait_data,qNode))
/*lint -save -e522*//*lint -save -e26*//*lint -save -e64*//*lint -save -e119 -e413*/

void semQueuePendQGet
    (
    Q_HEAD *pQHead,      /* pend queue to get first task off */
    Q_NODE *pQNode
    )
    {
    FAST WIND_TCB *pTcb;
	struct osl_wait_data * entry_temp = QUEUE_NODE_PTR_TO_TCB(pQNode);
	pTcb = entry_temp->task;

	Q_REMOVE(pQHead, pQNode);

#ifdef _WRS_CONFIG_SV_INSTRUMENTATION
    /* system viewer - level 2 event logging */
    EVT_TASK_1 (EVENT_WINDPENDQGET, (int) pTcb);	/* log event */
#endif /* _WRS_CONFIG_SV_INSTRUMENTATION */

    pTcb->status &= ~WIND_PEND;			/* clear pended flag */

    if (pTcb->status & WIND_DELAY)		/* task was timing out */
	{
	pTcb->status &= ~WIND_DELAY;
	Q_REMOVE (&tickQHead, &pTcb->tickNode);	/* remove from queue */
	}

    if (pTcb->status == WIND_READY)		/* task is now ready */
	READY_Q_PUT (pTcb);
    }


LOCAL __inline__ BOOL semQueueEventCheck
    (
    Q_NODE *pQNode,
    int  defalut
    )
    {
		struct osl_wait_data *pEntry =  QUEUE_NODE_PTR_TO_TCB (pQNode);

		if (pEntry->func)
			return ! pEntry->func(pEntry->data);
		else
			return (defalut);
    }

/*******************************************************************************
*
* semCGive - give a semaphore
*
* Gives the semaphore.  If a higher priority task has already taken
* the semaphore (so that it is now pended waiting for it), that task
* will now become ready to run, and preempt the task that does the semGive().
* If the semaphore is already full (it has been given but not taken) this
* call is essentially a no-op.
*
* SMP CONSIDERATIONS
* This API is spinlock and intCpuLock restricted.
*
* RETURNS: OK, or ERROR
*
* \NOMANUAL
*/
/*lint -save -e516 */
/*lint --e{533 } */
STATUS osl_waitqueue_wakeup
    (
    wait_queue_head_t semId        /* semaphore ID to give */
    )
    {
#ifndef _WRS_CONFIG_SMP
    int level;
#endif /* !_WRS_CONFIG_SMP */
	Q_NODE * pQNode = NULL;

#ifdef	_WRS_CONFIG_SMP
    /* lock has been taken in the case of ISR before here */

    if (!_WRS_INT_CONTEXT ())
#endif	/* _WRS_CONFIG_SMP */
	OBJ_LOCK (semClassId, level);

    if (OBJ_VERIFY (semId, semClassId) != OK)
	{
	OBJ_UNLOCK (semClassId, level);
	return (ERROR);
	}

	pQNode = Q_EACH(&semId->qHead, semQueueEventCheck, TRUE);

    if (pQNode == NULL)
	{
	    OBJ_UNLOCK (semClassId, level);/*lint !e2 */
	    return (OK);
	}
    else
	{
	KERNEL_LOCK_OBJ_UNLOCK (semClassId, level);/*lint !e2 */
/*lint -save -e722*/
#ifdef _WRS_CONFIG_SV_INSTRUMENTATION
	/* system viewer - level 2 event logging */
	EVT_TASK_1 (EVENT_OBJ_SEMGIVE, semId);/*lint !e681 */
#endif /* _WRS_CONFIG_SV_INSTRUMENTATION */
/*lint -restore +e722*/
	semQueuePendQGet(&semId->qHead, pQNode);		/* unblock a task */

	KERNEL_UNLOCK ();

	return (OK);
	}
    }

/*  ref from windLib.c */
/*lint --e{527, 578, 529, 533 } */
LOCAL void windCalibrateTickQ (void)
{
    Q_PRI_NODE *node;
    UINT64 delta = vxAbsTicks;
	/*lint -save -e530*/

    for (node = (Q_PRI_NODE *)DLL_FIRST ((void*)(&tickQHead));
         node != NULL;
         node = (Q_PRI_NODE *)DLL_NEXT(&node->node))
        {/*lint -save -e78*/	
        WIND_TCB *tcb = (WIND_TCB *) ((int)node - OFFSET (WIND_TCB, tickNode));
	/*lint -restore +e78*/	
	/*lint -restore +e530*/

        /* perform tick queue finish time recalibration for tasks only */

	if  (tcb->objCore.pObjClass == taskClassId)
	    {
	    /*
	     * NOTE:  use of TASK_CPUTIME_XXX is temporary.  A new field will
	     * be added to the TCB explicitly for this purpose in the future.
	     */
	    /*lint -save -e409*/
	    TASK_CPUTIME_INFO_SET (tcb, 0,
				   (TASK_CPUTIME_INFO_GET (tcb, 0) - delta));
            }
        }
    }
/*lint -save -e101*/

STATUS semQueuePendQPut
    (
    FAST Q_HEAD *pQHead,        /* pend queue to put taskIdCurrent on */
    FAST int    timeout,         /* timeout in ticks */
    struct osl_wait_data* entry
    )
    {
    FAST WIND_TCB *pTcbCurrent = _WRS_KERNEL_GLOBAL_ACCESS (taskIdCurrent);
	entry->task = pTcbCurrent;
	/*lint -restore +e101*/	
#ifdef _WRS_CONFIG_RTP
    /* For RTP task, check for pending signals if pQHead is interruptible */
	/*lint -save -e18*/

    if ((pQHead->qPriv2 & QUEUE_INTERRUPTIBLE) == QUEUE_INTERRUPTIBLE)
        {
        if (_func_rtpTaskSigCheck && _func_rtpTaskSigCheck(pTcbCurrent) == OK)
            {
            errno = EINTR;
            return (ERROR);
            }
        }
	/*lint -restore +e18*/
#endif /* _WRS_CONFIG_RTP */

#ifdef _WRS_CONFIG_SV_INSTRUMENTATION
    /* system viewer - level 2 event logging */
    EVT_TASK_0 (EVENT_WINDPENDQPUT);	/* log event */
#endif /* _WRS_CONFIG_SV_INSTRUMENTATION */

    READY_Q_REMOVE (pTcbCurrent);			/* out of ready q */

    pTcbCurrent->status |= WIND_PEND;			/* update status */

    pTcbCurrent->pPendQ = pQHead;			/* pQHead pended on */

    Q_PUT (pQHead, QUEUE_PENDQ_NODE_PTR (entry), pTcbCurrent->priority);

    if (timeout != WAIT_FOREVER)			/* timeout specified? */
	{/*lint --e{564} */
		if ((UINT64)(vxAbsTicks + timeout) < vxAbsTicks)  /* rollover? *//*lint !e737*/
	    {
	    	windCalibrateTickQ ();                        /* reset delays *//*lint !e718*/
#ifdef _WRS_CONFIG_SMP
	    	vxAbsTicksZero ();
#else
	    	vxAbsTicks = 0;
#endif /* _WRS_CONFIG_SMP */
	    }

	/*
	 * Set the finish time of the delay for the task.  This is used to
	 * facilitate recalculation of timeouts on RESTART.
	 */

	TASK_CPUTIME_INFO_SET (pTcbCurrent, 0, (vxAbsTicks + timeout));/*lint !e737*/

	Q_PUT (&tickQHead, &pTcbCurrent->tickNode, timeout);
	pTcbCurrent->status |= WIND_DELAY;
	}
	/*lint -restore +e409*/
    return (OK);
    }/*lint !e550*/

/*******************************************************************************
*
* semCTake - take a semaphore
*
* Takes the semaphore.  If the semaphore is empty, i.e., it has not been given
* since the last semTake() or semInit(), this task will become pended until
* the semaphore becomes available by some other task doing a semGive()
* of it.  If the semaphore is already available, this call will empty
* the semaphore, so that no other task can take it until this task gives
* it back, and this task will continue running.
*
* SMP CONSIDERATIONS
* This API is spinlock and intCpuLock restricted.
*
* WARNING
* This routine may not be used from interrupt level.
*
* RETURNS: OK, or ERROR
*
* \NOMANUAL
*/

STATUS osl_waitqueue_wait
    (
    wait_queue_head_t semId,       /* semaphore ID to take */
    int    timeout,      /* timeout in ticks */
    struct osl_wait_data* entry
    )
{
    int level;
    int status;
#ifdef _WRS_CONFIG_SMP
    int cpuid;

     level = KERNEL_INT_CPU_LOCK();
     cpuid = _WRS_CPU_INDEX_GET ();

     if (_WRS_KERNEL_CPU_GLOBAL_GET (cpuid, intCnt) != 0)
 	{
 	_WRS_KERNEL_CPU_GLOBAL_SET (cpuid, errno, S_intLib_NOT_ISR_CALLABLE);
 	KERNEL_INT_CPU_UNLOCK(level);
 	return (ERROR);
 	}
     KERNEL_INT_CPU_UNLOCK(level);
#else
    if (_WRS_INT_CONTEXT())
	{
	/* coverity[NULL_RETURNS] */
	errno = S_intLib_NOT_ISR_CALLABLE;
	return (ERROR);
	}
#endif /* _WRS_CONFIG_SMP */

again:

    OBJ_LOCK (semClassId, level);
    if (OBJ_VERIFY (semId, semClassId) != OK)
	{
	OBJ_UNLOCK (semClassId, level);
	return (ERROR);
	}

	if (entry->func)
	{
			if ( entry->func(entry->data) )
			{
				OBJ_UNLOCK (semClassId, level);
				return (OK);
			}
	}
	
    if (timeout == NO_WAIT)			/* NO_WAIT = no block */
	{
	OBJ_UNLOCK (semClassId, level);
	errnoSet (S_objLib_OBJ_UNAVAILABLE);
	return (ERROR);
	}

    KERNEL_LOCK_OBJ_UNLOCK (semClassId, level);/*lint !e2 */
/*lint -save -e722*/
#ifdef _WRS_CONFIG_SV_INSTRUMENTATION
    /* system viewer - level 2 event logging */
    EVT_TASK_1 (EVENT_OBJ_SEMTAKE, semId);/*lint !e681 */
#endif /* _WRS_CONFIG_SV_INSTRUMENTATION */
/*lint -restore +e722*/
    if (semQueuePendQPut (&semId->qHead, timeout, entry) != OK)
	{
	KERNEL_UNLOCK ();
	return (ERROR);
	}
	/*lint -save -e144*/

    if ((status = KERNEL_UNLOCK ()) == RESTART)
	{
        /*
         * If SIG_TIMEOUT_RECALC returns NO_WAIT, we want to return an errno of
         * S_objlib_OBJ_TIMEOUT.  If we don't return now, it will go try again,
         * check to see if timeout equals NO_WAIT and set an errno of
         * S_objlib_OBJ_UNAVAILABLE.
         */
	/*lint -restore +e144*/
	if ((timeout = SIG_TIMEOUT_RECALC (timeout)) == NO_WAIT)
            {
            errnoSet (S_objLib_OBJ_TIMEOUT);
            return (ERROR);
            }

	goto again;
	}

    return (status);
    }

/*lint -restore +e516*/
/*lint -restore +e129*/

/*******************************************************************************
*
* semCGiveDefer - give a semaphore as deferred work
*
* Gives the semaphore.  If a higher priority task has already taken
* the semaphore (so that it is now pended waiting for it), that task
* will now become ready to run, and preempt the task that does the semGive().
* If the semaphore is already full (it has been given but not taken) this
* call is essentially a no-op.
*
* RETURNS: N/A
*
* \NOMANUAL
*/
/*lint --e{78, 527,752 } */
void semQueueGiveDefer
    (
    SEM_ID semId        /* semaphore ID to give */
    )
    {
#ifdef	_WRS_CONFIG_SMP
    /* For SMP, we don't know if semId is still valid at this point */

    if (OBJ_VERIFY (semId, semClassId) != OK)
	return;
#endif	/* _WRS_CONFIG_SMP */

    if (Q_FIRST (&semId->qHead) == NULL)		/* anyone blocked? */
	{
	/*lint -save -e52*/
	semId->semCount++;				/* give semaphore */
	/*lint -restore +e52*/
	/* sem is free, send events if registered */

	if (semId->events.taskId != (int)NULL)
	    {
	    /*
	     * We will be here only if semId->events.taskId is non-null (i.e.
	     * the event library has been configured). Hence we can eliminate
	     * the null-check for _func_eventRsrcSend.
	     */
		/*lint -save -e119 -e533 */
	    if ((*_func_eventRsrcSend) (semId->events.taskId,
					semId->events.registered) != OK)
		{
		semId->events.taskId = (int)NULL;
		return;
		}
		/*lint -restore*/

	    if ((semId->events.options & EVENTS_SEND_ONCE) == EVENTS_SEND_ONCE)
		semId->events.taskId = (int)NULL;
	    }
	}
    else
	{
/*lint -save -e516*/

#ifdef _WRS_CONFIG_SV_INSTRUMENTATION
	/* system viewer - level 2 event logging */
	EVT_TASK_1 (EVENT_OBJ_SEMGIVE, semId);
#endif /* _WRS_CONFIG_SV_INSTRUMENTATION */
	/*lint -restore +e516*/

	windPendQGet (&semId->qHead);			/* unblock a task */
	}
    }




wait_queue_head_t osl_waitqueue_create
    (
    int         options                /* semaphore option modes */
    )
    {
    SEM_ID semId;/*lint !e578 */

    if ((semId = (SEM_ID) objAlloc (semClassId)) == NULL)
	return (NULL);

    /* initialize allocated semaphore */

    if (semQueueInit (semId, options, 0) != OK)
	{
	objFree (semClassId, (char *) semId);
	return (NULL);
	}

    /* mark object memory to be freed during automatic resource reclamation */
	/*lint -save -e115*/

    OBJ_SET_DEALLOC_MEM (&semId->objCore);
	/*lint -restore +e115*/

#ifdef _WRS_CONFIG_SV_INSTRUMENTATION
    /* system viewer - level 1 event logging */
    EVT_OBJ_3 (semId, semClassId, EVENT_SEMCCREATE,
               semId, options, initialCount);
#endif /* _WRS_CONFIG_SV_INSTRUMENTATION */

    return (wait_queue_head_t)(semId);
    }
/*lint -restore +e40*//*lint -restore +e63*//*lint -restore +e746*/
/*lint -restore +e522*//*lint -restore +e26*//*lint -restore +e64*//*lint -restore +e119*//*lint -restore +e413*/



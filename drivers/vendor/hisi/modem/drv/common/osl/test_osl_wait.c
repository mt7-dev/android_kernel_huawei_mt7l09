#include <osl_thread.h>
#include <osl_sem.h>
#include <osl_wait.h>

#define MAX_TASK_COUNT 10
static int state = 0;
typedef int task_struct_t;
static task_struct_t  tasks[MAX_TASK_COUNT];


static wait_queue_head_t wq;

static int check_state(void* data)
{
	printf("check_state: %d?=%d\n", state, (int)data);
	return state == (int)data;
}

static void task_main(void* data)
{
	/* coverity[noescape] */
	printf("[%d]wait_event: %d=%d\n", taskIdCurrent, state, (int)data);

	wait_event_timeout_func(wq, MAX_SCHEDULE_TIMEOUT, check_state, data);	

	/* coverity[noescape] */
	printf("[%d]wait_event ok: %d=%d\n", taskIdCurrent, state, (int)data);
}

int test_osl_wait(void)
{
	int i;

	init_waitqueue_head(&wq);
	
	for (i=0; i<MAX_TASK_COUNT; i++)
	{
		tasks[i] = kthread_run(task_main, i, "test_osl_wait");
	}

	taskDelay(10);
	
	for (i=MAX_TASK_COUNT-1; i>=0; i--)
	{
		state = i;
		printf("wake_up: %d\n", i);
		wake_up(&wq);
		taskDelay(10);
	}
	taskDelay(10);
	printf("try delete\n");
	sema_delete(wq);
	return 0;
}






#ifndef __BALONG_ICC__TEST_H__
#define __BALONG_ICC__TEST_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include "osl_types.h"
#include "icc_balong.h"

#define ICC_TEST_MCORE_CHN_SIZE          (20)
#define ICC_TEST_MCORE_START_CHN         (16)
#define ICC_TEST_CHANNEL_START           (32)
#define ICC_TAKE_SEM_TIMEOUT_JIFFIES     (3000)
#define ICC_TAKE_SEM_TIMEOUT_MS          (5000)

#if defined(__KERNEL__)
#define ICC_TEST_TASK_PRI                (25)
#define ICC_TEST_CHANNEL_SIZE            (64)
#define ICC_TEST_USE_CHN_SIZE            (4)
#define ICC_TEST_FIFO_SIZE               (1024)

#elif defined(__VXWORKS__)
#define ICC_TEST_TASK_PRI                (25)
#define ICC_TEST_CHANNEL_SIZE            (64)
#define ICC_TEST_USE_CHN_SIZE            (4)
#define ICC_TEST_FIFO_SIZE               (1024)

#elif defined(__CMSIS_RTOS)
#define ICC_TEST_TASK_PRI                (osPriorityHigh)
#define ICC_TEST_USE_CHN_SIZE            (4)
#define ICC_TEST_FIFO_SIZE               (40)

#endif

typedef s32 (*task_entry)(void *obj);

#define ICC_RECV_OK                      (0x10305070)
#define ICC_RECV_ERR                     (-1)
#define ICC_TEST_PASS                    (0)
#define ICC_TEST_FAIL                    (-1)
#define ICC_RECV_OK_TIMES                (21)
#define ICC_SEND_SYNC_RET                (0x789abcde)
#define ICC_TEST_MULTI_SEND_CHN_SIZE     (ICC_TEST_CHANNEL_SIZE/4)
#define ICC_VALIDE_CHN_NUM               (ICC_CHN_CSHELL)
#define ICC_TEST_CASE_INITILIZED         (0x1)
#define ICC_TEST_DEFAULT_ASYN_SUBCHN_NUM (32)
#define ICC_TEST_DEFAULT_SYNC_SUBCHN_NUM (33)
#define ICC_TEST_SEND_CNT                (100)
#define ICC_TEST_MUL_SUB_CHN_SEND_FLAG        (1)
#define ICC_TEST_MUL_SUB_CHN_SEND_SYNC_FLAG   (2)
#define ICC_TEST_MUL_CHN_SEND_FLAG            (3)
#define ICC_TEST_MULTI_CHN_SEND_SYNC_FLAG     (4)

#define ICC_CONF_MSG_TYPE1               (0x51)
#define ICC_CONF_MSG_TYPE2               (0x22)
#define SUPPORT_ICC_READ_IN_OTHER_TASK   (3)
#define ICC_NEW_START_DATA               (0)
#define ICC_NEW_DATA_LEN                 (32)

struct icc_test_send{
	u32 id;
};

struct icc_test_sub_channel
{
	u8 *wr_buf;
	u8 *rd_buf;
	osl_sem_id confirm_sem;
	struct task_struct* task_id;
	s32 success_cnt;
};

struct icc_test_channel
{
	u32 func_size_bak;
	u32 fifo_size_bak;
	struct icc_channel_vector *vec_bak;
#if defined(__KERNEL__) || defined(__VXWORKS__)
	struct icc_test_sub_channel sub_channels[ICC_TEST_CHANNEL_SIZE];
#elif defined(__CMSIS_RTOS) /* rtx资源有限 */
	struct icc_test_sub_channel sub_channels[ICC_TEST_MCORE_CHN_SIZE];
#endif
};

struct icc_test_case
{
	u32 state;
	u32 mcore_test_state;
	struct task_struct* task_id;
	u8 *rd_buf;
	u8 *wr_buf;
	osl_sem_id multi_send_confirm_sem;
#if defined(__KERNEL__) || defined(__VXWORKS__)
	struct icc_test_channel channels[ICC_CHN_ID_MAX];
#elif defined(__CMSIS_RTOS)
	struct icc_test_channel channels[2];
#endif
	osl_sem_id recv_task_sem;
	struct task_struct*        recv_task_id;
	u32        recv_task_flag;
};

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif    /*  __BALONG_ICC__TEST_H__ */

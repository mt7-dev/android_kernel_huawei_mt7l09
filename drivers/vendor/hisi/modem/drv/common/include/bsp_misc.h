
#ifndef __BSP_MISC_H__
#define __BSP_MISC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -save -e537*/
#include "osl_common.h"
#include "osl_sem.h"
#include "osl_malloc.h"
#include "osl_thread.h"
#include "bsp_icc.h"
#include "bsp_nvim.h"
#include "bsp_rfile.h"
#if defined(__KERNEL__)
#include "bsp_nandc.h"
#endif

/*lint -restore +e537*/

/****************************************************************/

/****************************************************************/

#ifndef SC_OK
#define SC_OK           (0)
#endif

#ifndef SC_ERROR
#define SC_ERROR        (-1)
#endif

/* sc error code */
#define BSP_ERR_SC_BASE                         BSP_DEF_ERR(BSP_MODU_SC, 0)

#define BSP_ERR_SC_INIT_FAIL                    (BSP_ERR_SC_BASE + 0x1)
#define BSP_ERR_SC_INVALID_PARAM                (BSP_ERR_SC_BASE + 0x2)
#define BSP_ERR_SC_MALLOC_FAIL                  (BSP_ERR_SC_BASE + 0x3)
#define BSP_ERR_SC_ICC_READ                     (BSP_ERR_SC_BASE + 0x4)
#define BSP_ERR_SC_ICC_SEND                     (BSP_ERR_SC_BASE + 0x5)
#define BSP_ERR_SC_NO_FILE                      (BSP_ERR_SC_BASE + 0x6)
#define	BSP_ERR_SC_READ_FILE_FAIL               (BSP_ERR_SC_BASE + 0x7)
#define	BSP_ERR_SC_WRITE_FILE_FAIL              (BSP_ERR_SC_BASE + 0x8)
#define	BSP_ERR_SC_TASK_INIT_FAIL               (BSP_ERR_SC_BASE + 0x9)
#define	BSP_ERR_SC_SEM_INIT_FAIL                (BSP_ERR_SC_BASE + 0xA)
#define	BSP_ERR_SC_SEM_TIMEOUT                  (BSP_ERR_SC_BASE + 0xB)
#define	BSP_ERR_SC_CNF_ABNORMAL                 (BSP_ERR_SC_BASE + 0xC)
#define	BSP_ERR_READ_MTD_FAIL                   (BSP_ERR_SC_BASE + 0xD)
#define	BSP_ERR_READ_LGTH_FAIL                  (BSP_ERR_SC_BASE + 0xE)



#define SC_MTD_PTABLE_OFFSET                    (32*1024)
#define SC_LEN_ICC_MAX                          (0x1000)
#define SC_LEN_MAX                              (0x1000 - sizeof(sc_icc_stru))
#define SC_ICC_CHAN_ID                          (ICC_CHN_NV << 16 | NV_RECV_FUNC_SC)
#define SC_PACKET_TRANS_FILE                    "/modem_log/sc_packet.bin"



typedef enum
{
    MISC_SC_OPT_READ            = 0,
    MISC_SC_OPT_WRITE           = 1,
    MISC_SC_OPT_BUTT
}MISC_SC_OPT_ENUM;

typedef struct s_sc_icc_stru
{
    MISC_SC_OPT_ENUM            sc_opt_type;            /* msg type */
    u32                         sc_total_len;
    s32                         sc_cnf_ret;
    s32                         rsv;
}sc_icc_stru;

typedef struct s_sc_status_stru
{
    osl_sem_id                  sc_api_sem; 
    osl_sem_id                  sc_tsk_sem;
    u32                         sc_opt_type;
    u32                         sc_taskid;
    u32                         sc_ram_len;
    s32                         sc_opt_ret;
    u8                          *sc_ram_addr ;
}sc_status_stru;

#define sc_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SC,"[sc]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define sc_warn_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_WARNING,BSP_MODU_SC,"[sc]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define sc_info_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_INFO,BSP_MODU_SC,"[sc]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define sc_debug_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_DEBUG,BSP_MODU_SC,"[sc]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define sc_error_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_SC,"[sc]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)

s32 bsp_sc_init(void);
s32 bsp_sc_kernel_init(void);
s32 bsp_sc_backup(u8* pRamAddr, u32 len);
s32 bsp_sc_restore(u8* pRamAddr,u32 len);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /*__BSP_MISC_H__*/






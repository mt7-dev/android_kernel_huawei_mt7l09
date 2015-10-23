


#ifndef _NV_COMM_H_
#define _NV_COMM_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e537*/
#include <memLib.h>
#include <taskLib.h>
#include <osl_common.h>
#include <osl_spinlock.h>
#include <osl_sem.h>
#include <osl_list.h>
#include <osl_bio.h>
#include <cacheLib.h>
#include <bsp_nvim.h>
#include <bsp_shared_ddr.h>
#include <bsp_ipc.h>
#include <bsp_icc.h>
#include <bsp_om.h>
#include "drv_nv_def.h"
#include "drv_nv_id.h"
#include "msp_nv_id.h"
/*lint -restore +e537*/
#ifdef __KERNEL__

#define nv_malloc(a)     kmalloc(a,GFP_KERNEL)
#define nv_free(p)       kfree(p)
#ifndef printf
#define printf(fmt,...)     bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_NV,fmt,##__VA_ARGS__)
#endif

#define nv_taskdelay(n)      msleep(n)

#elif defined(__VXWORKS__) /*vxworks*/

#define nv_malloc(a)     malloc(a)
#define nv_free(p)       free(p)
#define nv_taskdelay(n)  taskDelay(n)

#undef printf
#define printf(fmt,...)     bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_NV,fmt,##__VA_ARGS__)


#endif
#define nv_flush_cache(p,n)   cache_sync()

extern struct nv_global_ctrl_info_stru g_nv_ctrl;
#define nv_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_NV,"[nv]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define nv_warn_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_WARNING,BSP_MODU_NV,"[nv]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define nv_info_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_INFO,BSP_MODU_NV,"[nv]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define nv_debug_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_DEBUG,BSP_MODU_NV,"[nv]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)
#define nv_error_printf(fmt, ...) \
    bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_NV,"[nv]: <%s> line = %d  "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)

#define NV_ID_SYS_MAX_ID          0xd1ff
#define NV_ID_SYS_MIN_ID          0xd100

#define nv_debug_trace(pdata,len)\
    if(g_nv_ctrl.debug_sw == true)\
    {\
        u32 i;\
        printf("<%s> len :0x%x\n",__FUNCTION__,len);\
        for(i=0;i<len;i++)\
        {\
            printf("%02x ",*((u8*)pdata+i));\
        }\
        printf("\n");\
    };


#define nv_spin_lock(nvflag,lock) \
do{\
    spin_lock_irqsave(&g_nv_ctrl.spinlock, nvflag);\
    bsp_ipc_spin_lock(lock);\
}while(0)

#define nv_spin_unlock(nvflag,lock) \
do{\
    bsp_ipc_spin_unlock(lock);\
    spin_unlock_irqrestore(&g_nv_ctrl.spinlock, nvflag);\
}while(0)





/*icc msg type*/
enum
{
    NV_ICC_REQ      = 63,
    NV_ICC_REQ_SYS  = 64,
    NV_ICC_CNF      = 127
};


struct nv_icc_stru
{
    u32 msg_type;          /*msg type*/
    u32 data_off;          /*data off */
    u32 data_len;          /*data len*/
    u32 ret;
    u32 itemid;
    u32 slice;
};

/*nv debug info*/
struct nv_global_debug_stru
{
    u32 callnum;
    u32 reseved1;
    u32 reseved2;
    u32 reseved3;
    u32 reseved4;
};


#define NV_MAX_WAIT_TICK             500


#define NV_ICC_BUF_LEN               64
#define NV_ICC_SEND_COUNT            5
/*global info type define*/
struct nv_global_ctrl_info_stru
{
    u8 nv_icc_buf[NV_ICC_BUF_LEN];
#ifdef __VXWORKS__
    osl_sem_id rw_sem;      /*lock the write*/
    osl_sem_id cc_sem;      /*lock the core comm*/
#endif
    u32 mid_prio;    /*nv priority*/
    u32 debug_sw;
    spinlock_t spinlock;
    u32 shared_addr;
    struct list_head stList;
    bool statis_sw;
};
struct nv_write_list_stru
{
    u32 itemid;
    u32 count;
    u32 slice;
    struct list_head stList;
};

u32 nv_write_to_mem(u8 * pdata, u32 size, u32 file_id, u32 offset);
u32 nv_read_from_mem(u8 * pdata, u32 size,u32 file_id ,u32 offset);
u32 nv_icc_chan_init(void);
s32 nv_icc_msg_proc(u32 chanid,u32 len, void * pdata);
u32 nv_icc_read(u32 chanid, u32 len);
u32 nv_icc_send(u8 * pdata, u32 len);
bool nv_read_right(u32 itemid);
bool nv_write_right(u32 itemid);
u32 nv_write_to_file(struct nv_ref_data_info_stru* ref_info);
u32 nv_search_byid(u32 itemid,u8 * pdata, struct nv_ref_data_info_stru * ref_info, struct nv_file_list_info_stru * file_info);
void nv_debug(u32 type, u32 reseverd1, u32 reserved2, u32 reserved3, u32 reserved4);
void nv_help(u32 type);
void show_ddr_info(void);
void show_ref_info(u32 min, u32 max);
void nv_print_file(void);
void show_all_nv_data(void);
void nv_modify_print_sw(u32 arg);
void nv_modify_statis_sw(u32 arg);
bool nv_isSysNv(u16 itemid);
void nv_AddListNode(u32 itemid);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



#endif /*_NV_COMM_H_*/



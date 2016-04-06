
#ifndef _SC_BALONG_H_
#define _SC_BALONG_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e537*/
#include <bsp_misc.h>
/*lint -restore +e537*/


s32 sc_opt_comm(MISC_SC_OPT_ENUM sc_enum, u8* pRamAddr, u32 len);
s32 sc_icc_msg_proc(u32 chanid, u32 len, void* pdata);
s32 bsp_sc_backup(u8* pRamAddr, u32 len);
s32 bsp_sc_restore(u8* pRamAddr, u32 len);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



#endif /*_SC_BALONG_H_*/



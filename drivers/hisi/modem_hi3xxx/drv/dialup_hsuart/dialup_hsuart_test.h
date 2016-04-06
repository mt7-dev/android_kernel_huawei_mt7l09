
#ifndef    _HSUART_UDI_TEST_H_
#define    _HSUART_UDI_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif
/**************************************************************************
  头文件包含                            
**************************************************************************/
#include "hi_uart.h"
#include <osl_sem.h>
#include "drv_dpm.h"
#include "bsp_om.h"

#define M2M_RECV_SIZE 1024

typedef struct m2m_recv_debug_str
{
	u32 WritePtr;
	u32 ReadPtr;
	u32 buffer[M2M_RECV_SIZE];
}m2m_recv_str;





int M2m_hsuart_InQue(m2m_recv_str *pstQue, UINT8 ucData);

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */
#endif



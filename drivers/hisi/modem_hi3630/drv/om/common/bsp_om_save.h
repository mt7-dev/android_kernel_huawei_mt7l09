

#ifndef	__BSP_OM_SAVE_H__
#define __BSP_OM_SAVE_H__

#include "product_config.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/**************************************************************************
  宏定义
**************************************************************************/
#define OM_ROOT_PATH            "/modem_log/log/"
/*#define OM_RESET_LOG            "reset.log"*/
#define OM_DUMP_HEAD            "dump_"
#define OM_RESET_LOG            "/modem_log/log/reset.log"
#define OM_ERROR_LOG            "/modem_log/log/error.log"
#define OM_DIAG_LOG             "/modem_log/log/diag.bin"
#define OM_OAM_LOG              "/modem_log/log/oam.bin"
#define OM_NCM_LOG              "/modem_log/log/ncm.bin"
#define OM_ZSP_DUMP             "/modem_log/log/zsp_dump.bin"
#define OM_HIFI_DUMP            "/modem_log/log/hifi_dump.bin"

#define OM_RESET_LOG_MAX        2048
#define OM_ERROR_LOG_MAX        4096
#define OM_PRINT_LOG_MAX        (4096*24)

#define OM_DUMP_FILE_MAX_NUM        3
#define OM_DUMP_FILE_NAME_LENGTH    50
/**************************************************************************
  STRUCT定义
**************************************************************************/

/**************************************************************************
  UNION定义
**************************************************************************/

/**************************************************************************
  OTHERS定义
**************************************************************************/


/**************************************************************************
  函数声明
**************************************************************************/
int  bsp_om_save_loop_file(char * dirName, char *fileHeader, void * address, u32 length);
int  bsp_om_save_file(char *filename, void * address, u32 length, u32 mode);
int  bsp_om_append_file(char *filename, void * address, u32 length, u32 max_size);
bool bsp_om_fs_check(void);
void bsp_om_record_resetlog(void);
void bsp_om_save_resetlog(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_OM_SAVE_H__ */






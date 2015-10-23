



/* New file of OEMINFO_APIs */
#ifndef __OEMINFO_H__
#define __OEMINFO_H__
#ifdef __cplusplus
extern "C"
{
#endif

#define HUAWEI_FEATURE_EMMC_OEMINFO

#define OEMINFO_DEBUG   1

#define LOG_TAG "OEMINFO"
#define LOG_NDEBUG 0
#include "cutils/log.h"


// printf
#ifndef OEMINFO_DEBUG
#define OEMINFO_MSG(a,b)
#define OEMINFO_MSG_3(a,b,c)
#define OEMINFO_MSG_4(a,b,c,d)
#define OEMINFO_MSG_STR(a,b)
#define OEMINFO_MSG_BIN(a,b)
#else
	void oeminfo_log_print(int log_type, char * log_string,int param_a,int param_b,int param_c);

#define OEMINFO_MSG(a,b) oeminfo_log_print(0,a,b,0,0)
#define OEMINFO_MSG_3(a,b,c) oeminfo_log_print(0,a,b,c,0)
#define OEMINFO_MSG_4(a,b,c,d) oeminfo_log_print(0,a,b,c,d)
#define OEMINFO_MSG_STR(a,b) oeminfo_log_print(1,a,b,0,0)
#define OEMINFO_MSG_BIN(a,b) oeminfo_log_print(2,a,b,0,0)
#endif

#ifndef _INT8_BOOLEAN
	typedef  unsigned char      boolean;     /* Boolean value type. */
#define _INT8_BOOLEAN
#endif

#define TRUE   1   /* Boolean true value. */
#define FALSE  0   /* Boolean false value. */

#ifndef _UINT32_DEFINED
	typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
	typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
	typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
	typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
	typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
	typedef  signed char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif


#define OEMINFO_FAIL            0x0
#define OEMINFO_OK              0x1
#define OEMINFO_FILL_CHAR       0xFF

typedef enum {
		OEMINFO_DATA_MIN          = 1,
		OEMINFO_LOGO_TYPE         = 4,      /* Logo data for power-up                     */
		OEMINFO_NVBAK_TYPE        = 5,      /* NV backup data                            */
		OEMINFO_CHARGE_TYPE     = 6,
		OEMINFO_ISO_VER_TYPE       = 15,
		OEMINFO_WEBUI_TYPE       = 16,
		OEMINFO_DATA_MAX,
		OEMINFO_DATA_BAND = 0xFF
	} oeminfo_info_type_enum_type;


	/**************************** FUNCTIONS' DECLARATION ***************************/

	/*===========================================================================

	FUNCTION oeminfo_init_device

	DESCRIPTION
	  init oeminfo device.

	DEPENDENCIES
	  sholl call this function before read oeminfo.

	CALLED BY
	  oeminfo_erase

	RETURN VALUE
	  if initialize sucessfully return TRUE
	  else return FALSE

	SIDE EFFECTS

	===========================================================================*/

	boolean oeminfo_init_device(void);
	/*===========================================================================

	FUNCTION oeminfo_erase

	DESCRIPTION
	  erase block.

	DEPENDENCIES

	CALLED BY
	  oeminfo_program
	  oeminfo_program_from_file

	RETURN VALUE

	SIDE EFFECTS

	===========================================================================*/
	boolean oeminfo_erase(oeminfo_info_type_enum_type oeminfo_info_id);

	/*===========================================================================

	FUNCTION oeminfo_init

	DESCRIPTION
	  Initialization to prepare the header page information for each oeminfo block and memorize the haeder
	  page in oeminfo_header structure

	DEPENDENCIES

	CALLED BY
	  oeminfo_program_from_file

	RETURN VALUE
	  if initialize the oeminfo_hdr_page_ptr and prepare first NBB succeed
	    return TRUR
	  else
	    return FALSE

	SIDE EFFECTS

	===========================================================================*/
	boolean oeminfo_init
	(
	    oeminfo_info_type_enum_type oeminfo_info_id,
	    uint32 oeminfo_len_in_byte
	);
	/*===========================================================================

	FUNCTION oeminfo_program

	DESCRIPTION
	  write data to oeminfo.
	  oeminfo_info_id: data type id.
	  src: data buffer.
	  offset: write index.

	DEPENDENCIES

	CALLED BY
	  oeminfo_program_from_file

	RETURN VALUE
	  if write successfully return TRUE
	  else return FALSE

	SIDE EFFECTS

	===========================================================================*/
	boolean oeminfo_program
	(
	    oeminfo_info_type_enum_type oeminfo_info_id,
	    unsigned char *src,
	    uint32 offset
	);


	/*===========================================================================

	FUNCTION oeminfo_finalize

	DESCRIPTION
	  write the last page.

	DEPENDENCIES

	CALLED BY
	  oeminfo_program_from_file

	RETURN VALUE
	  if write successfully return TRUE
	  else return FALSE

	SIDE EFFECTS

	===========================================================================*/
	boolean oeminfo_finalize(oeminfo_info_type_enum_type oeminfo_info_id);

	/*===========================================================================

	FUNCTION oeminfo_read

	DESCRIPTION
	  read data from oeminfo.
	  oeminfo_info_id: data type.
	  dst: data buffer.
	  size: data size.

	DEPENDENCIES

	CALLED BY

	RETURN VALUE
	 if read successfully return TRUE;
	 else return FALSE;

	SIDE EFFECTS

	===========================================================================*/
	boolean oeminfo_read
	(
	    oeminfo_info_type_enum_type oeminfo_info_id,
	    unsigned char *dst,
	    uint32 size
	);

	/*===========================================================================

	FUNCTION oeminfo_get_info

	DESCRIPTION
	  get the size and block count of the type.
	  oeminfo_info_id: data type.
	  total_blck: return block count.
	  total_byte: return size by byte.

	DEPENDENCIES

	CALLED BY
	  oeminfo_read
	  oeminfo_read_to_file

	RETURN VALUE

	SIDE EFFECTS

	===========================================================================*/
	boolean oeminfo_get_info
	(
	    oeminfo_info_type_enum_type oeminfo_info_id,
	    uint32 *total_blck,
	    uint32 *total_byte
	);


#if defined (HUAWEI_MODEM)
	/*===========================================================================

	FUNCTION oeminfo_read_to_file

	DESCRIPTION
	  read oeminfo data to a file.

	DEPENDENCIES

	CALLED BY
	  cnv_process_command
	  rfnv_auto_restore

	RETURN VALUE
	  TRUE: success.
	  FALSE:fail.

	SIDE EFFECTS

	===========================================================================*/
	boolean oeminfo_read_to_file
	(
	    oeminfo_info_type_enum_type oeminfo_info_id,
	    const char *dir_name,
	    const char *file_name
	);

	/*===========================================================================

	FUNCTION oeminfo_program_from_file

	DESCRIPTION
	  read data from a file and write to oeminfo.

	DEPENDENCIES

	CALLED BY
	  cnv_process_command

	RETURN VALUE
	  TRUE: success.
	  FALSE:fail.

	SIDE EFFECTS

	===========================================================================*/
	boolean oeminfo_program_from_file
	(
	    oeminfo_info_type_enum_type oeminfo_info_id,
	    const char *path
	);
#endif /* HUAWEI_MODEM */

#ifdef __cplusplus
}
#endif

#endif /* __OEMINFO_H__ */


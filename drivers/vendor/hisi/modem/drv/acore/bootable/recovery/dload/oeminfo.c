/*============================================================================

                 OEMINFO APIs

   DESCRIPTION
     This file contains the APIs needed for modules who want to use OEMINFO.

   Copyright (c) 2006-2007 by HUAWEI Technologies Co., Ltd.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: ... ... /drivers/flash/oeminfo.c
 $DateTime: 2006/11/28
 $Author: wangyali

   when     who      what, where, why
 --------  -----  ----------------------------------------------------------
 06/11/30   wyl      Fixed Lint errors.
 06/11/29   hqf      Recomment the code in English.
 06/11/28   wyl      Initial release.
 08/04/14   hanfeng  ported from platform 6245
 08/09/13   duangan  ported for 1210 baseline.
 10/04/09   duangan  ported for 7x30. emmc_oeminfo.
============================================================================*/





/* <BU5D08126 duangan 2010-4-25 begin */
/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "oeminfo.h"

// TODO: for compile.
#define KICK_WATCHDOG()

/* oeminfo partition name */
#define OEMINFO_PARTITION_NAME  "/dev/block/platform/omap/omap_hsmmc.1/by-name/oeminfo"

/* oeminfo log file name */
#define OEMINFO_LOG_FILE  "/data/oeminfo_ap.log"

/*oeminfo type erase block value */ 
#define OEMINFO_BLOCK_ERASE_VALUE  0xFF

/* oeminfo magic number */
#define OEM_INFO_MAGIC1               0x5F4D454F
#define OEM_INFO_MAGIC2               0x4F464E49

#define OEMINFO_VERSION               0x02
#define OEMINFO_PAGE_SIZE             2048   /* Size of page in byte*/
#define OEMINFO_FIRST_LOGICAL_BLOCK   1

/* Num of pages in each block */
#define OEMINFO_PAGES_IN_BLCK         64

/* Size of block in byte*/
#define OEMINFO_BLCK_SIZE_IN_BYTE     ((OEMINFO_PAGES_IN_BLCK -1) * OEMINFO_PAGE_SIZE)

/* size of oeminfo partition*/
#define OEMINFO_PARTITION_LEN 0x40000

/* OEMINFO header page(the first page of each type) */
struct oeminfo_hdr_page;
typedef struct oeminfo_hdr_page *oeminfo_hdr_page_t;
struct oeminfo_hdr_page {
	uint32 magic_number1;
	uint32 magic_number2;
	uint32 version;
	uint32 info_type;         /* of oeminfo_info_type_enum_type                */
	uint32 total_blck;        /* how many blocks to save this type.            */
	uint32 curr_logical_blck; /* block index(from 1)                           */
	uint32 total_byte;        /* how many byte to save this type.              */
};

/* OEMINFO control structure. */
struct oeminfo_ctl;

typedef struct oeminfo_ctl *oeminfo_ctl_t;

struct oeminfo_ctl {
	int32 fd;
	/* Buffer to store data to write in.  Generally only used on blocks
	 * which span successive invocations.
	 */
	unsigned char partial_page_buffer[OEMINFO_PAGE_SIZE];
	unsigned int  partial_page_index;

	/* State info to keep track of current page and block available */
	uint32  current_page;
	uint32  current_physical_block;
	uint32  avail_pages_in_block;
};

static boolean oeminfo_write_current_page(oeminfo_ctl_t pctl, unsigned char * src);
static boolean oeminfo_prepare_this_block (oeminfo_ctl_t pctl,uint32 curr_logical_blck_index);
static boolean oeminfo_prepare_next_block (oeminfo_ctl_t pctl ,uint32 next_logical_blck_index);
/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

/* Global structure defined for OEMINFO apis, encapsulating neccessary parameters for read/write operations */
static struct oeminfo_ctl oeminfo_image;

/* Buffers in memory used for data read and write operations */
static uint8 oeminfo_data_buf[OEMINFO_PAGE_SIZE];
static uint8 oeminfo_hdr_page_buff[OEMINFO_PAGE_SIZE];
static uint8 oeminfo_hdr_page_buff_for_read[OEMINFO_PAGE_SIZE];

/* Flag symbolizing whether the oeminfo related structures and parameters have been initialized */
static boolean oeminfo_initialized = FALSE;

/*============================================================================
                          EXPORTED FUNCTIONS
============================================================================*/

#ifdef OEMINFO_DEBUG
#include <fcntl.h>

#define OEMINFO_LOG_LEN    2048

// log printf to file
// log_type
//     = 0 , number;  at this time, param_a/param_b/param_c are int32 number.
//     = 1 , string ; at this time, param_a is a string ptr;
//     = 2 , binary array print. at this time, param_a is a count of int32 number.
//             log_string is a binary array ptr.
void oeminfo_log_print(int log_type, char * log_string,int param_a,int param_b,int param_c)
{
	char all_log_string[OEMINFO_LOG_LEN+4];
	char sub_string[12];
	FILE *fp = NULL;
	int ii;
	int * my_int_array = NULL;

	fp = fopen(OEMINFO_LOG_FILE, "a");

	if (!fp) {
		return;
	}
	memset(all_log_string,0,sizeof(all_log_string));
	memset(sub_string,0,sizeof(sub_string));

	if(0 == log_type) {
		snprintf(all_log_string, OEMINFO_LOG_LEN, log_string , param_a , param_b , param_c);
	} else if(1 == log_type) {
		if(param_a) {
			snprintf(all_log_string, OEMINFO_LOG_LEN, log_string , (char *)param_a);
		}
	} else {
		my_int_array = (int32 *)log_string;

		for(ii = 0; ii<param_a; ii++) {
			snprintf(sub_string, sizeof(sub_string), "%08X " , my_int_array[ii]);

			strncat(all_log_string,sub_string,sizeof(sub_string));
		}
	}

	strcat(all_log_string,"\n");

	fwrite(all_log_string,1,strlen(all_log_string),fp);

	fclose(fp);
}
/* <BU5D10052 duangan 2010-5-19 begin */
// delete 4 lines.
/* BU5D10052 duangan 2010-5-19 end> */
#endif

/*===========================================================================

DESCRIPTION
  This function return oeminfo partition length.

DEPENDENCIES

RETURN VALUE
  return oeminfo partition length.

SIDE EFFECTS

===========================================================================*/


static uint32 oeminfo_get_length_by_block()
{
	return OEMINFO_PARTITION_LEN / (OEMINFO_PAGE_SIZE * OEMINFO_PAGES_IN_BLCK);
}


/*===========================================================================

DESCRIPTION
  This function does erase one block of a type.

DEPENDENCIES

RETURN VALUE
  If success, return OEMINFO_OK.
  if failed, return OEMINFO_FAIL

SIDE EFFECTS

===========================================================================*/
static uint32 oeminfo_erase_current_block(uint32 block_id)
{
	uint32 hdr_page_id;
	int32 rc;
	int ii;

	OEMINFO_MSG("oeminfo_erase_current_block() enter.",0);

	if(!oeminfo_init_device()) {
		OEMINFO_MSG("oeminfo_init_device fail!",0);
		return OEMINFO_FAIL;
	}

	memset(oeminfo_data_buf,OEMINFO_BLOCK_ERASE_VALUE,sizeof(oeminfo_data_buf));

	lseek(oeminfo_image.fd, (block_id * OEMINFO_PAGE_SIZE * OEMINFO_PAGES_IN_BLCK), SEEK_SET);

	for(ii = 0; ii < OEMINFO_PAGES_IN_BLCK ; ii++) {
		rc = write(oeminfo_image.fd, oeminfo_data_buf, OEMINFO_PAGE_SIZE);

		if (rc != OEMINFO_PAGE_SIZE) {
			OEMINFO_MSG_3("erase type fail! index is %d, erase count is %d.",
			              (block_id * OEMINFO_PAGE_SIZE + ii), rc);
			return OEMINFO_FAIL;
		}
	}

	OEMINFO_MSG("oeminfo_erase_current_block() ok.",0);

	return OEMINFO_OK;
}

/*===========================================================================

DESCRIPTION
  This function check a block is erase.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static uint32 oeminfo_check_block_erase(uint32 block_id)
{
	int32 rc;
	oeminfo_hdr_page_t oeminfo_hdr;

	OEMINFO_MSG("oeminfo_check_block_erase() enter. block id is %d.",block_id);

	if(!oeminfo_init_device()) {
		OEMINFO_MSG("oeminfo_init_device fail!",0);
		return OEMINFO_FAIL;
	}

	lseek(oeminfo_image.fd, (block_id * OEMINFO_PAGE_SIZE * OEMINFO_PAGES_IN_BLCK), SEEK_SET);

	rc = read(oeminfo_image.fd, oeminfo_data_buf, OEMINFO_PAGE_SIZE);

	if (rc != OEMINFO_PAGE_SIZE) {
		OEMINFO_MSG_3("oeminfo read fail. index is %d, read count is %d.",
		              (block_id * OEMINFO_PAGE_SIZE),rc);
		return OEMINFO_FAIL;
	}

	oeminfo_hdr = (struct oeminfo_hdr_page *)oeminfo_data_buf;

	if ((oeminfo_hdr->magic_number1 == OEM_INFO_MAGIC1)
	    && (oeminfo_hdr->magic_number2 == OEM_INFO_MAGIC2)) {
		OEMINFO_MSG("oeminfo_check_block_erase(), block %d is not a blank block.",block_id);
		return OEMINFO_FAIL;
	}

	OEMINFO_MSG("oeminfo_check_block_erase() ok.",0);
	return OEMINFO_OK;
}

/*****************************************************************************
 Prototype    : oeminfo_close
 Description  : close oeminfo
 Input        : None
 Output       : None
 Return Value : static
 Calls        :
 Called By    :
 Others       :

 History      :
               1. 2010/4/16      duangan            Created function

*****************************************************************************/
static void oeminfo_close()
{
	close(oeminfo_image.fd);

	memset(&oeminfo_image,0,sizeof(oeminfo_image));

	/* set the flag to false, so the next use need reinit oeminfo*/
	oeminfo_initialized = FALSE;
}

/*===========================================================================

FUNCTION oeminfo_init_device

DESCRIPTION
  Initialize the oeminfo partition related global structure oeminfo_image

DEPENDENCIES
  The function must be called before all the oeminfo operations could be performed.

CALLED BY
  oeminfo_erase

RETURN VALUE
  if initialize sucessfully return TRUE
  else return FALSE

SIDE EFFECTS

===========================================================================*/

boolean oeminfo_init_device(void)
{
	if (oeminfo_initialized) {
		return TRUE;
	}

	OEMINFO_MSG("oeminfo_init_device() enter.",0);

	memset(&oeminfo_image,0,sizeof(oeminfo_image));

	// oeminfo_image.fd = open(OEMINFO_PARTITION_NAME, O_RDWR);
	oeminfo_image.fd = open(OEMINFO_PARTITION_NAME, O_RDWR|O_SYNC);

	if (oeminfo_image.fd >= 0) {
		oeminfo_initialized = TRUE;
		OEMINFO_MSG("oeminfo device init OK. fs handle is %d.",oeminfo_image.fd);
		return TRUE;
	} else {
		OEMINFO_MSG("oeminfo device init fail.",0);
		return FALSE;
	}
}
/*===========================================================================

FUNCTION oeminfo_erase

DESCRIPTION
   Wipe out the blocks of specified information based on oeminfo_info_id on the OEMINFO partition

DEPENDENCIES

CALLED BY
  oeminfo_program
  oeminfo_program_from_file

RETURN VALUE
  if erase successfully return TRUE
  else return FALSE

SIDE EFFECTS

===========================================================================*/
boolean oeminfo_erase(oeminfo_info_type_enum_type oeminfo_info_id)
{
	int32 rc;
	oeminfo_hdr_page_t oeminfo_hdr;
	uint32 ii;
	uint32 hdr_page_id;
	int32 block_num;

	OEMINFO_MSG("oeminfo_erase() enter.",0);

	if(!oeminfo_init_device()) {
		OEMINFO_MSG("oeminfo_init_device fail.",0);
		return FALSE;
	}

	block_num=oeminfo_get_length_by_block();
	for(ii = 0; ii <block_num;ii++) {
		memset(oeminfo_hdr_page_buff_for_read,0,sizeof(oeminfo_hdr_page_buff_for_read));
		lseek(oeminfo_image.fd, (ii * OEMINFO_PAGE_SIZE * OEMINFO_PAGES_IN_BLCK), SEEK_SET);
		rc = read(oeminfo_image.fd,oeminfo_hdr_page_buff_for_read,OEMINFO_PAGE_SIZE);

		if(rc != OEMINFO_PAGE_SIZE) {
			OEMINFO_MSG_3("oeminfo read fail. index is %d, read count is %d.",ii,rc);
			return FALSE;
		} else {

		}

		oeminfo_hdr = (struct oeminfo_hdr_page *)oeminfo_hdr_page_buff_for_read;

		if ((oeminfo_hdr->magic_number1 == OEM_INFO_MAGIC1)
		    && (oeminfo_hdr->magic_number2 == OEM_INFO_MAGIC2)
		    && (oeminfo_hdr->info_type == oeminfo_info_id)) {
			// TODO: erase block
			if(oeminfo_erase_current_block(ii) == OEMINFO_FAIL) {
				return FALSE;
			}
		}
	}

	OEMINFO_MSG("oeminfo_erase() ok.",0);
	return TRUE;
}

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
)
{
	oeminfo_hdr_page_t my_hdr_page_ptr = NULL;

	OEMINFO_MSG("oeminfo_init() enter.",0);

	/* Erase() MUST be executed before all the following operations to avoid wasting blocks */
	if(!oeminfo_erase(oeminfo_info_id)) {
		OEMINFO_MSG("Erase oeminfo failed.",0);
		return FALSE;
	}

	memset(oeminfo_hdr_page_buff, OEMINFO_BLOCK_ERASE_VALUE, OEMINFO_PAGE_SIZE);

	my_hdr_page_ptr = (struct oeminfo_hdr_page*)oeminfo_hdr_page_buff;
	my_hdr_page_ptr->magic_number1  = OEM_INFO_MAGIC1;
	my_hdr_page_ptr->magic_number2  = OEM_INFO_MAGIC2;
	my_hdr_page_ptr->version        = OEMINFO_VERSION;
	my_hdr_page_ptr->info_type      = oeminfo_info_id;
	my_hdr_page_ptr->total_byte     = oeminfo_len_in_byte;

	/* Compute total blocks with the oeminfo_len_in_byte, as the para is recored in each block */
	/* Note that the size of input data cannot be the exact times of one block size */
	my_hdr_page_ptr->total_blck     =
	    (oeminfo_len_in_byte + OEMINFO_BLCK_SIZE_IN_BYTE - 1) / OEMINFO_BLCK_SIZE_IN_BYTE;

	/* On initialization, the partial_page_index and current_physical_block should be set as zero because
	  * no data has been written ever since */
	oeminfo_image.partial_page_index = 0;
	oeminfo_image.current_physical_block = 0;

	OEMINFO_MSG("oeminfo_init(), oeminfo_hdr_page_buff is:",0);
	OEMINFO_MSG_BIN(oeminfo_hdr_page_buff,8);

	/* Prepare the first physical block for the oeminfo; the block can also be viewed as the first logical block
	  * for this type of information
	  */
	if(!oeminfo_prepare_this_block((oeminfo_ctl_t)&oeminfo_image, OEMINFO_FIRST_LOGICAL_BLOCK)) {
		OEMINFO_MSG("oeminfo_prepare_this_block fail.",0);
		return FALSE;
	}

	OEMINFO_MSG("oeminfo_init() ok.",0);

	return TRUE;
}

/*===========================================================================

FUNCTION oeminfo_program

DESCRIPTION
  This is a generic interface to program information from memory. On occurence of error, the salvage attempt
  must be adopted to erase former programmed blocks. Only in this way can the waste of blocks be avoided.

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
)
{
	/* Local buffer used to hold the data to be programmed */
	unsigned char * wbuf = NULL;

	/*  Pointer accosiated with source address */
	unsigned char * data = src;

	/* The size of data to be programmed stored in memory */
	uint32 size = offset;

	/* Availible space in partial_page_buffer of oeminfo_image structure which is at the size of one page */
	uint32 partial_bytes_avail;

	oeminfo_ctl_t pctl = (struct oeminfo_ctl *)&oeminfo_image;

	OEMINFO_MSG("oeminfo_program() enter. data size is %d. ",offset);
	OEMINFO_MSG_STR("oeminfo_program() data is: %s",(int)src);

	/* So long as the data hasnot be totally programmed, the programm continues */
	while (size > 0) {
		partial_bytes_avail = OEMINFO_PAGE_SIZE - pctl->partial_page_index;

		/* If available space is large enough to hold the rest of data, then copy the data into the buffer
		  * For another, since the data in buffer hasnot reached the limit of one page, we don't start the
		  * programming. Note that for nand, the atom operation is based on page
		  */

		if (partial_bytes_avail > size) {
			memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data, size);

			pctl->partial_page_index += size;

			/* There is no data left, and we needn't to write any page */
			break;

		}

		/* If the space of available buffer is equal to exactly the size of data, then copy
		  * the date into the buffer to have one page of data, which can be programmed now
		  */
		else if ((partial_bytes_avail == size) && (pctl->partial_page_index != 0)) {
			/* fill partial page completely, ready for writing, none left over */
			memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data, size);

			wbuf = pctl->partial_page_buffer;
			/* update count of waitting data */
			data += partial_bytes_avail;
			/* update size of waitting data */
			size -= partial_bytes_avail;
			/* update ptr to write */
			pctl->partial_page_index = 0;
		}
		/* if the waitting date is less then a page, so write it. */
		else if ((partial_bytes_avail < size) && (pctl->partial_page_index != 0)) {
			/* fill partial page completely, ready for writing, some left over */
			memcpy(&pctl->partial_page_buffer[pctl->partial_page_index], data,
			       partial_bytes_avail);
			wbuf = pctl->partial_page_buffer;
			data += partial_bytes_avail;
			size -= partial_bytes_avail;
			pctl->partial_page_index = 0;
		}
		/* if the waitting date is more then a page, so write one page size. */
		else { /* partial_page_index == 0 && size >= page_size */
			/* write a full page from where it is */
			wbuf = data;
			data += OEMINFO_PAGE_SIZE;
			size -= OEMINFO_PAGE_SIZE;
		}
		KICK_WATCHDOG();

		/* Write a full page.  If we do not break out of the while early,
		 * and we reach here, there is a full page to be written.
		 */
		if (FALSE == oeminfo_write_current_page (pctl, wbuf)) {
			// if write fail, then erase total type.
			if(!oeminfo_erase(oeminfo_info_id)) {
				OEMINFO_MSG("oeminfo erase fail!,oeminfo_info_id:%d",oeminfo_info_id);
			}
			return FALSE;
		}
	}

	/* check the data ptr. */
	if (pctl->partial_page_index > OEMINFO_PAGE_SIZE) {
		return FALSE;
	}

	OEMINFO_MSG("oeminfo_program() ok.",0);

	return TRUE;
}

/*===========================================================================

FUNCTION oeminfo_write_current_page

DESCRIPTION
  write a page.

DEPENDENCIES

CALLED BY
  oeminfo_program

RETURN VALUE
  if write a page to OEMINFO successfully return TRUE
  else return FALSE

SIDE EFFECTS

===========================================================================*/
static boolean oeminfo_write_current_page (oeminfo_ctl_t pctl, unsigned char *src)
{
	unsigned char *vsrc1, *vsrc2;
	uint32 i;
	int32 rc;
	oeminfo_hdr_page_t my_hdr_page_ptr = NULL;

	OEMINFO_MSG("oeminfo_write_current_page() enter. blank page count is %d.",
	            pctl->avail_pages_in_block);

	// TODO: get block id.
	// need another block
	if (0 == pctl->avail_pages_in_block) {
		OEMINFO_MSG("need another block.",0);

		my_hdr_page_ptr = (struct oeminfo_hdr_page*)oeminfo_hdr_page_buff;
		if (FALSE == oeminfo_prepare_next_block(pctl,(my_hdr_page_ptr->curr_logical_blck + 1))) {
			OEMINFO_MSG("oeminfo_prepare_next_block fail.",0);
			return FALSE;
		}
	}

	// TODO: write page
	OEMINFO_MSG("oeminfo_write_current_page(), write buffer is:",0);
	OEMINFO_MSG_BIN(src,512/4);

	rc = write(oeminfo_image.fd, src, OEMINFO_PAGE_SIZE);
	if (rc != OEMINFO_PAGE_SIZE ) {
		OEMINFO_MSG("oeminfo write fail. write count is %d.",rc);
		return FALSE;
	} else {
		OEMINFO_MSG("oeminfo write ok. write count is %d.",rc);
	}
	KICK_WATCHDOG();

	/* zero out readback buffer */
	memset(oeminfo_data_buf, 0, OEMINFO_PAGE_SIZE);

	// TODO: write page
	/* read back data and verify it against source */
	lseek(oeminfo_image.fd, -OEMINFO_PAGE_SIZE, SEEK_CUR);
	rc = read(oeminfo_image.fd, oeminfo_data_buf, OEMINFO_PAGE_SIZE);
	if (rc != OEMINFO_PAGE_SIZE ) {
		OEMINFO_MSG("oeminfo read fail. read count is %d.",rc);
		return FALSE;
	}

	vsrc1 = src;
	vsrc2 = oeminfo_data_buf;

	if(0 != memcmp(vsrc1,vsrc2,OEMINFO_PAGE_SIZE)) {
		OEMINFO_MSG("omeinfo_page_write verify fail.",0);
		return FALSE;
	}

	KICK_WATCHDOG();

	pctl->current_page++;
	pctl->avail_pages_in_block--;

	OEMINFO_MSG("oeminfo_write_current_page() ok.",0);

	return TRUE;
}

/*===========================================================================

FUNCTION oeminfo_prepare_this_block

DESCRIPTION
  从可用的块中为信息写入分配第一个可用的非OEMINFO BLOCK

DEPENDENCIES

CALLED BY
  oeminfo_init
  oeminfo_prepare_next_block

RETURN VALUE
  if malloc a NBB successfully return TRUE;
  else return FALSE;

SIDE EFFECTS

===========================================================================*/
static boolean oeminfo_prepare_this_block (oeminfo_ctl_t pctl,uint32 curr_logical_blck_index)
{
	uint16 blck_index;
	int32 rc;
	uint32 current_page_id = 0;
	oeminfo_hdr_page_t my_hdr_page_ptr = NULL;
	int32 block_num=0;

	OEMINFO_MSG("oeminfo_prepare_this_block() enter.",0);

	// find a empty block from current block.

	block_num=oeminfo_get_length_by_block();
	for (blck_index = pctl->current_physical_block;
	     blck_index <block_num ;
	     blck_index++) {
		if (oeminfo_check_block_erase(blck_index)) {
			OEMINFO_MSG("find a blank block, id is %d.",blck_index);
			break;
		}
		KICK_WATCHDOG();
	}

	// if catch the tail of partition, return fail.
	if (blck_index == oeminfo_get_length_by_block()) {
		OEMINFO_MSG("oeminfo is full. id is %d.",blck_index);
		return FALSE;
	}

	my_hdr_page_ptr = (struct oeminfo_hdr_page*)oeminfo_hdr_page_buff;
	my_hdr_page_ptr->curr_logical_blck = curr_logical_blck_index;

	// TODO: write page
	lseek(oeminfo_image.fd, (blck_index * OEMINFO_PAGE_SIZE * OEMINFO_PAGES_IN_BLCK), SEEK_SET);

	OEMINFO_MSG("oeminfo_prepare_this_block(), oeminfo_hdr_page_buff is: ",0);

	/* <BU5D10052 duangan 2010-5-20 begin */
	OEMINFO_MSG_BIN(oeminfo_hdr_page_buff,8);
	/* BU5D10052 duangan 2010-5-20 end> */

	rc = write(oeminfo_image.fd, oeminfo_hdr_page_buff, OEMINFO_PAGE_SIZE);
	if (rc != OEMINFO_PAGE_SIZE) {
		OEMINFO_MSG("oeminfo write header fail. write count is %d.",rc);
		return OEMINFO_FAIL;
	}

	pctl->current_physical_block = blck_index;
	pctl->current_page = pctl->current_physical_block * OEMINFO_PAGES_IN_BLCK;

	/* Now that the header page has been allocated successfully, the avaible page should
	 * exclude the header page for each physical oeminfo block
	 */
	pctl->avail_pages_in_block = OEMINFO_PAGES_IN_BLCK - 1;
	pctl->current_page++;

	OEMINFO_MSG("oeminfo_prepare_this_block() ok.",0);

	return TRUE;
}

/*===========================================================================

FUNCTION oeminfo_prepare_next_block

DESCRIPTION


DEPENDENCIES

CALLED BY
  oeminfo_write_current_page

RETURN VALUE
  if malloc a NBB successfully return TRUE;
  else return FALSE;

SIDE EFFECTS

===========================================================================*/
static boolean oeminfo_prepare_next_block (oeminfo_ctl_t pctl ,uint32 next_logical_blck_index)
{
	/* Point to the next block */
	pctl->current_physical_block++;

	/* Call prepare_this_block() to actually do the next good block
	 * find and the erase when found.
	 */
	return oeminfo_prepare_this_block(pctl, next_logical_blck_index);
}

/*===========================================================================

FUNCTION oeminfo_finalize

DESCRIPTION
  at the tail of type, finalize the write.

DEPENDENCIES

CALLED BY
  oeminfo_program_from_file

RETURN VALUE
  if write successfully return FALSE
  else return TRUE

SIDE EFFECTS

===========================================================================*/
boolean oeminfo_finalize(oeminfo_info_type_enum_type oeminfo_info_id)
{
	uint16 num_fill_bytes;

	OEMINFO_MSG("oeminfo_finalize() enter. left data size is %d.",
	            oeminfo_image.partial_page_index);

	if (oeminfo_image.partial_page_index > 0) {
		num_fill_bytes = OEMINFO_PAGE_SIZE - oeminfo_image.partial_page_index;
		while (num_fill_bytes > 0) {
			oeminfo_image.partial_page_buffer[oeminfo_image.partial_page_index++] =
			    (unsigned char)OEMINFO_FILL_CHAR;

			num_fill_bytes--;
		}
		KICK_WATCHDOG();

		if(!oeminfo_write_current_page((struct oeminfo_ctl *)&oeminfo_image,oeminfo_image.partial_page_buffer)) {
			// if fail, then erase the type.
			if(!oeminfo_erase(oeminfo_info_id)) {
				OEMINFO_MSG("oeminfo_erase fail!,oeminfo_info_id:%d",oeminfo_info_id);
			}
			OEMINFO_MSG("Write page failure in finalization, all former written blocks have been erased!",0);
			return FALSE;
		}
	}

	oeminfo_close();

	OEMINFO_MSG("oeminfo_finalize() ok.",0);
	return TRUE;
}

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
)
{
	uint32 total_blck;
	uint32 total_byte;
	uint32 remained_bytes;
	uint32 total_page;
	uint32 oeminfo_curr_phy_blck;
	uint32 logical_blck_index;
	uint32 phy_blck_index;
	uint32 page_index;
	uint32 oeminfo_page_count = 0;
	int32 rc;

	boolean is_curr_blck_found = FALSE;
	oeminfo_hdr_page_t oeminfo_header_ptr;

	unsigned char *dst_tmp = dst;

	OEMINFO_MSG("oeminfo_read() enter.",0);

	/* get type`s byte and block. */
	if(!oeminfo_get_info(oeminfo_info_id, &total_blck, &total_byte)) {
		OEMINFO_MSG("oeminfo_get_info fail!",0);
		return FALSE;
	}

	// if size mismatch, return fail.
	if(size < total_byte) {
		OEMINFO_MSG("oeminfo size error!",0);
		return FALSE;
	}

	remained_bytes = total_byte % OEMINFO_PAGE_SIZE;

	if (0 == remained_bytes) {
		remained_bytes = OEMINFO_PAGE_SIZE;
	}

	// get page count
	total_page = (total_byte + OEMINFO_PAGE_SIZE  - 1) / OEMINFO_PAGE_SIZE;

	oeminfo_curr_phy_blck = 0;

	// loop all logic block.
	for (logical_blck_index = OEMINFO_FIRST_LOGICAL_BLOCK;
	     logical_blck_index <= total_blck;
	     logical_blck_index++) {
		is_curr_blck_found = FALSE;

		// loop all phy block.
		for (phy_blck_index = oeminfo_curr_phy_blck;
		     phy_blck_index < oeminfo_get_length_by_block();
		     phy_blck_index++) {
			// TODO: read page header.
			memset(oeminfo_hdr_page_buff_for_read,0,OEMINFO_PAGE_SIZE);

			lseek(oeminfo_image.fd, (phy_blck_index * OEMINFO_PAGE_SIZE * OEMINFO_PAGES_IN_BLCK), SEEK_SET);
			rc = read(oeminfo_image.fd, oeminfo_hdr_page_buff_for_read, OEMINFO_PAGE_SIZE);

			if (rc != OEMINFO_PAGE_SIZE) {
				OEMINFO_MSG_3("oeminfo read fail. index is %d, read count is %d.",
				              (phy_blck_index * OEMINFO_PAGE_SIZE),rc);
			}

			oeminfo_header_ptr = (oeminfo_hdr_page_t)oeminfo_hdr_page_buff_for_read;

			// verify the header.
			if ((oeminfo_header_ptr->magic_number1 == OEM_INFO_MAGIC1)
			    && (oeminfo_header_ptr->magic_number2 == OEM_INFO_MAGIC2)
			    && (oeminfo_header_ptr->info_type == oeminfo_info_id)
			    && (oeminfo_header_ptr->curr_logical_blck == logical_blck_index)) {
				oeminfo_curr_phy_blck = phy_blck_index;
				is_curr_blck_found = TRUE;

				break;
			}
		}

		if (!is_curr_blck_found) {
			// if donot find, return fail.
			return FALSE;
		}

		for (page_index =  1 ; page_index < OEMINFO_PAGES_IN_BLCK ; page_index++) {
			oeminfo_page_count++;

			if (oeminfo_page_count > total_page) {
				break;
			} else if (oeminfo_page_count < total_page) {
				// TODO: read page
				rc = read(oeminfo_image.fd, oeminfo_data_buf, OEMINFO_PAGE_SIZE);
				if (rc != OEMINFO_PAGE_SIZE) {
					OEMINFO_MSG("oeminfo read fail. read count is %d.",rc);
					continue;
				}

				/* <BU5D10052 duangan 2010-5-20 begin */
				OEMINFO_MSG("oeminfo_read(), read buffer is:",0);
				OEMINFO_MSG_BIN(oeminfo_data_buf,512/4);
				/* BU5D10052 duangan 2010-5-20 end> */

				memcpy(dst_tmp, oeminfo_data_buf, OEMINFO_PAGE_SIZE);
				dst_tmp += OEMINFO_PAGE_SIZE;
			}
			// the last page.
			else {
				// TODO: read page
				rc = read(oeminfo_image.fd, oeminfo_data_buf, OEMINFO_PAGE_SIZE);
				if (rc != OEMINFO_PAGE_SIZE) {
					OEMINFO_MSG("oeminfo read fail. read count is %d.",rc);
					continue;
				}

				OEMINFO_MSG("oeminfo_read(), read buffer is:",0);
				OEMINFO_MSG_BIN(oeminfo_data_buf,512/4);

				memcpy(dst_tmp, oeminfo_data_buf, remained_bytes);
				dst_tmp += remained_bytes;
			}
			OEMINFO_MSG("oeminfo read ok. readed pages is %d.",oeminfo_page_count);
		}

		oeminfo_curr_phy_blck++;
	}

	oeminfo_close();

	OEMINFO_MSG("oeminfo_read() ok.",0);
	return TRUE;
}

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
)
{
	uint32 blck_index;
	uint32 page_index;
	int32 rc;
	oeminfo_hdr_page_t oeminfo_header_ptr;

	OEMINFO_MSG("oeminfo_get_info() enter.",0);

	for (blck_index = 0; blck_index < oeminfo_get_length_by_block(); blck_index++) {
		// TODO: read page
		memset(oeminfo_hdr_page_buff_for_read,0,OEMINFO_PAGE_SIZE);

		lseek(oeminfo_image.fd, (blck_index * OEMINFO_PAGE_SIZE * OEMINFO_PAGES_IN_BLCK), SEEK_SET);
		rc = read(oeminfo_image.fd, oeminfo_hdr_page_buff_for_read, OEMINFO_PAGE_SIZE);
		if (rc != OEMINFO_PAGE_SIZE) {
			OEMINFO_MSG_3("oeminfo read fail. index is %d, read count is %d.",
			              (blck_index * OEMINFO_PAGE_SIZE * OEMINFO_PAGES_IN_BLCK),rc);
			continue;
		}
		oeminfo_header_ptr = (oeminfo_hdr_page_t)oeminfo_hdr_page_buff_for_read;

		if ((oeminfo_header_ptr->magic_number1 == OEM_INFO_MAGIC1)
		    && (oeminfo_header_ptr->magic_number2 == OEM_INFO_MAGIC2)
		    && (oeminfo_header_ptr->info_type == oeminfo_info_id)
		    && (oeminfo_header_ptr->curr_logical_blck == OEMINFO_FIRST_LOGICAL_BLOCK)) {
			*total_blck = oeminfo_header_ptr->total_blck;
			*total_byte = oeminfo_header_ptr->total_byte;

			OEMINFO_MSG("oeminfo_get_info() ok. page data is: ",0);
			/* <BU5D10052 duangan 2010-5-20 begin */
			OEMINFO_MSG_BIN(oeminfo_hdr_page_buff_for_read,8);
			/* BU5D10052 duangan 2010-5-20 end> */
			return TRUE;
		} else {

		}
	}

	// if don`t find, return fail.
	OEMINFO_MSG("oeminfo_get_info() fail.",0);
	return FALSE;
}

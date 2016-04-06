
#include "product_config.h"

#include <linux/module.h>
#include <linux/io.h>

#include "ptable_com.h"
#include <bsp_reset.h>
#include <bsp_sec.h>
#include <bsp_dsp.h>
#include <hi_dsp.h>
#include "bsp_nandc.h"
#include "reset_balong.h"
#include "load_image.h"


#ifdef CONFIG_TZDRIVER
#include <teek_client_api.h>
#include <teek_client_id.h>
#endif

#ifdef CONFIG_LOAD_SEC_IMAGE	
#define SECBOOT_BUFLEN  (0x100000)
static  u8 SECBOOT_BUFFER[SECBOOT_BUFLEN]; /*1M Bytes*/
static  u8 g_vrl_P[SECBOOT_VRL_SIZE];
static  u8 g_vrl_M[SECBOOT_VRL_SIZE];
static  DEFINE_MUTEX(trans_lock);
struct  modem_head trans_data;
typedef enum SVC_SECBOOT_CMD_ID SECBOOT_CMD_ID;
#endif

#if (defined CONFIG_TZDRIVER) && (defined CONFIG_LOAD_SEC_IMAGE)
/******************************************************************************
Function:	    TEEK_init
Description:	TEEK初始化
Input:		    session	
                context
                
Output:		    none

Return:		    0: OK  其他: ERROR码
******************************************************************************/
static s32 TEEK_init(TEEC_Session *session, TEEC_Context *context)
{
    TEEC_Result result;
    TEEC_UUID svc_uuid = TEE_SERVICE_SECBOOT;
    TEEC_Operation operation = {0};
    u8 package_name[] = "sec_boot";
    u32 root_id = 0;

	mutex_lock(&trans_lock);

	result = TEEK_InitializeContext(
			               NULL,
			               context);

    if(result != TEEC_SUCCESS) {
        sec_print_err("TEEK_InitializeContext failed!\n");
        result = SEC_ERROR;
        goto error1;
    }
    operation.started = 1;
    operation.cancel_flag = 0;
    operation.paramTypes = TEEC_PARAM_TYPES(
            TEEC_NONE,
            TEEC_NONE,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT);
    operation.params[2].tmpref.buffer = (void *)(&root_id);
    operation.params[2].tmpref.size = sizeof(root_id);
    operation.params[3].tmpref.buffer = (void *)(package_name);
    operation.params[3].tmpref.size = strlen(package_name) + 1;

    result = TEEK_OpenSession(
            context,
            session,
            &svc_uuid,
            TEEC_LOGIN_IDENTIFY,
            NULL,
            &operation,
            NULL);

	if (result != TEEC_SUCCESS) 
	{
		sec_print_err("TEEK_OpenSession failed!\n");
		result = SEC_ERROR;
		TEEK_FinalizeContext(context);
	}  
	
error1:
	mutex_unlock(&trans_lock);

	return result;
}

/******************************************************************************
Function:	    bsp_trans_to_os
Description:	从指定偏移开始传送指定大小的镜像
Input:		    
            cmd_id      - 传送给安全OS的cmd_id
            *buf		- 输入参数，存放要传送到安全os中的值
		    size		- 输入参数，要写入的镜像的bytes大小
		    offset		- 镜像需要写入的偏移地址

Output:		    none
Return:		    0: OK  其他: ERROR码
******************************************************************************/
static s32 bsp_trans_to_os(TEEC_Session *session,
		  SECBOOT_CMD_ID cmd_id,
		  void * buf,
		  const unsigned int offset,
		  const unsigned int size)
{
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;

	mutex_lock(&trans_lock);
	
	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes = TEEC_PARAM_TYPES(
						TEEC_VALUE_INPUT,
						TEEC_MEMREF_TEMP_INPUT,
						TEEC_VALUE_INPUT,
						TEEC_NONE);
	operation.params[0].value.a = trans_data.image_addr;
	operation.params[0].value.b = offset;
	operation.params[1].tmpref.buffer = (void *)g_vrl_P;
	operation.params[1].tmpref.size = SECBOOT_VRL_SIZE;
	operation.params[2].value.a = (u32)virt_to_phys(buf);
    operation.params[2].value.b = size;

	result = TEEK_InvokeCommand(
				session,
				cmd_id,
				&operation,
				&origin);
	if (result != TEEC_SUCCESS) {
		sec_print_err("invoke failed!\n");
		result = SEC_ERROR;
	}

	mutex_unlock(&trans_lock);

	return result;
}


/******************************************************************************
Function:	    load_done
Description:	全部镜像发送完毕开始校验
Input:		    
            cmd_id      - 传送给安全OS的cmd_id

Output:		    none
Return:		    0: OK  其他: ERROR码
******************************************************************************/
static s32 load_done(TEEC_Session *session, SECBOOT_CMD_ID cmd_id)		  		  
{
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;

	mutex_lock(&trans_lock);
	
	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes = TEEC_PARAM_TYPES(
						TEEC_NONE,
						TEEC_NONE,
						TEEC_NONE,
						TEEC_NONE);

	result = TEEK_InvokeCommand(
				session,
				cmd_id,
				&operation,
				&origin);
	if (result != TEEC_SUCCESS) {
		sec_print_err("invoke failed!\n");
		result = SEC_ERROR;
	}

	mutex_unlock(&trans_lock);

	return result;
}

/******************************************************************************
Function:	    load_data_to_secos
Description:	从指定偏移开始传送指定大小的镜像
Input:		
			part_name   - 要发送镜像的名称
			offset      - 偏移地址 
		    sizeToRead	- 输入参数，要写入的镜像的bytes大小

Output:		    none
Return:		    SEC_OK: OK  SEC_ERROR: ERROR码
******************************************************************************/
static s32 load_data_to_secos(TEEC_Session *session, char* part_name, u32 offset, u32 sizeToRead, u32 * skip_len)
{
	u32 read_bytes;
	u32 end_bytes;
	u32 copy_bytes;
	u32 timers;
	u32 i;
	s32 ret = SEC_ERROR;

	/*split the size to be read to each 64K bytes.*/
	timers = sizeToRead / SECBOOT_BUFLEN;	
	if (sizeToRead % SECBOOT_BUFLEN)		
	timers = timers + 1;

	end_bytes = sizeToRead;
	
	/*call flash_read each time to read to memDst.*/
	for (i = 0; i < timers; i++) 
	{		
		if (end_bytes >= SECBOOT_BUFLEN)
			read_bytes = SECBOOT_BUFLEN;
		else
			read_bytes = end_bytes;
		
		if (bsp_nand_read(part_name, offset + i * SECBOOT_BUFLEN, (void *)SECBOOT_BUFFER, read_bytes, skip_len)) {
			sec_print_err("%s: err: flash_read\n", __func__);
			return SEC_ERROR;
		}

		if (end_bytes >= SECBOOT_BUFLEN) {
			if (SECBOOT_BUFLEN != read_bytes) {
				sec_print_err("%s: err: SECBOOT_BUFLEN:%d != read_bytes:%d\r\n", __func__, SECBOOT_BUFLEN, read_bytes);
				return SEC_ERROR;
			}
			copy_bytes = SECBOOT_BUFLEN;
		} else {
			if (end_bytes > read_bytes) {
				sec_print_err("%s: err: end_bytes:%d!= read_bytes:%d\r\n", __func__, end_bytes, read_bytes);
				return SEC_ERROR;
			}
			copy_bytes = end_bytes;
		}		

		ret = bsp_trans_to_os(session, SECBOOT_CMD_ID_COPY_DATA, (void *)(SECBOOT_BUFFER),
			                                            (i * SECBOOT_BUFLEN), copy_bytes);	
		if (SEC_ERROR == ret)
		{
			sec_print_err("modem image trans to os is failed, error code 0x%x\r\n", ret);
			return SEC_ERROR;
		}

		end_bytes -= copy_bytes;
	}
	
	if (0 != end_bytes) {
		sec_print_err("%s: end_bytes = 0x%x\n", __func__, end_bytes);
		return SEC_ERROR;
	}
	
	return SEC_OK;
/*lint --e{818}*/
}



static s32 load_modem_head(void)
{	
	u32 skip_len = 0;
	u32 g_ccore_entry = 0;
	u32 image_load_addr = 0;
	/* coverity[var_decl] */
	struct image_head head;  
	s32 ret = SEC_ERROR;	 

    ret = bsp_nand_read(PTN_MODEM_NAME, (FSZ)0, &head, sizeof(struct image_head) , &skip_len);
    if (NAND_OK != ret)
    {
        sec_print_err("fail to read modem image head, error code 0x%x\r\n", ret);
        return SEC_ERROR;
    }

	/* coverity[uninit_use_in_call] */
	if (memcmp(head.image_name, CCORE_IMAGE_NAME, sizeof(CCORE_IMAGE_NAME)))
    {
        sec_print_err("vxworks image error!!.\r\n");
        return SEC_ERROR;
    }

    /* coverity[uninit_use] */
	g_ccore_entry = head.load_addr;
	
    /* coverity[uninit_use] */
    trans_data.image_length = (u32)head.image_length + 2*IDIO_LEN + OEM_CA_LEN + sizeof(struct image_head);

	/* coverity[uninit_use] */
    if (head.is_compressed)
    {
        image_load_addr = g_ccore_entry - (MCORE_TEXT_START_ADDR - DDR_MCORE_ADDR)
            + DDR_MCORE_SIZE - trans_data.image_length;
    }
    else
    {
        image_load_addr = g_ccore_entry;
    }

	if(trans_data.image_length % 64)
	{
		trans_data.image_length = trans_data.image_length + (64 - trans_data.image_length % 64);
	}
	
	trans_data.image_addr = image_load_addr - sizeof(struct image_head);

    return SEC_OK;
}

/************************************************************************************************
*
*	Function:		load_VRL_to_secos
*	Description:	read vrl of specified image from emmc
*	Calls:		
*			bsp_trans_to_os
*			bsp_nand_read
*
*	Data Accessed:	No
*	Data Updated:
*
*	Parameters:
*	
*	Return:
*			@u32
*					SEC_OK:      success
*					SEC_ERROR:      failure
*
*************************************************************************************************/
static s32 load_VRL_to_secos(TEEC_Session *session)
{
	unsigned read_bytes;
	s32 ret;

	read_bytes = SECBOOT_VRL_SIZE;
	
	ret = bsp_nand_read(PTN_VRL_P_NAME, SECBOOT_MODEM_VRL_OFFSET * SECBOOT_VRL_SIZE, (void *)g_vrl_P, read_bytes, NULL);
	if (NAND_OK != ret) 
	{
		sec_print_err("%s: can't make ptn for vrl!\n", __func__);
		return SEC_ERROR;
	}

	ret = bsp_nand_read(PTN_VRL_M_NAME, SECBOOT_MODEM_VRL_OFFSET * SECBOOT_VRL_SIZE, (void *)g_vrl_M, read_bytes, NULL);
	if (NAND_OK != ret) 
	{
		sec_print_err("%s: can't make ptn for vrl_backup!\n", __func__);
		return SEC_ERROR;
	}	

	/* compare the data read from vrl and vrl_back, if not equ, we restore primary vrl to backup vrl */
	if (memcmp((void *)g_vrl_P, (void *)g_vrl_M, SECBOOT_VRL_SIZE)) {
		sec_print_err("%s: memcmp data error!\n", __func__);
		return SEC_ERROR;		
	}

	/* trans the vrl to secure os. */
	ret = bsp_trans_to_os(session,SECBOOT_CMD_ID_COPY_VRL,(void *)g_vrl_P,0,SECBOOT_VRL_SIZE);
	if (SEC_ERROR == ret)
	{
		sec_print_err("modem image trans to os is failed, error code 0x%x\r\n", ret);
		return SEC_ERROR;
	}
	
	return SEC_OK;
}


/************************************************************************************************
*
*	Function:		load_modem_to_secos
*	Description:	read modem of specified image from emmc to sec os
*	Calls:		
*			load_data_to_secos
*
*	Data Accessed:	session
*	Data Updated:
*
*	Parameters:
*	
*	Return:
*			@u32
*					SEC_OK:      success
*					SEC_ERROR:      failure
*
*************************************************************************************************/
static s32 load_modem_to_secos(TEEC_Session *session)
{
	u32 skip_len = 0;
	s32 ret = SEC_ERROR;
	
	/*load modem data to sec os*/
	ret = load_data_to_secos(session,PTN_MODEM_NAME,0,trans_data.image_length,&skip_len);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_data_to_secos fail!\n");
		return ret;
	}
	
	return SEC_OK;
}


/************************************************************************************************
*
*	Function:		load_modem_dsp_to_secos
*	Description:	read modem_dsp of specified image from emmc to sec os
*	Calls:		
*			load_data_to_secos
*			bsp_nand_read
*
*	Data Accessed:	session
*	Data Updated:
*
*	Parameters:
*	
*	Return:
*			@u32
*					SEC_OK:      success
*					SEC_ERROR:      failure
*
*************************************************************************************************/	
static s32 load_modem_dsp_to_secos(TEEC_Session *session)
{	
	u32 skip_len = 0;
	u32 offset = 0;
	/* coverity[var_decl] */
	struct image_head head;  
	s32 ret = SEC_ERROR;	 

	/* clean ok flag */
    writel(0, (void*)SHM_MEM_DSP_FLAG_ADDR);
	
    ret = bsp_nand_read(PTN_MODEM_DSP_NAME, (FSZ)0, &head, sizeof(struct image_head), &skip_len);
    if (NAND_OK != ret)
    {
        sec_print_err("fail to read modem dsp image head, error code 0x%x\r\n", ret);
        return SEC_ERROR;
    }

	/* coverity[uninit_use_in_call] */
	if (memcmp(head.image_name, DSP_IMAGE_NAME, sizeof(DSP_IMAGE_NAME)))
    {
        sec_print_err("DSP image error!!.\r\n");
        return SEC_ERROR;
    }

    trans_data.image_length = DDR_TLPHY_IMAGE_SIZE;
	trans_data.image_addr = DDR_TLPHY_IMAGE_ADDR;
	
 	offset += LPHY_BBE16_MUTI_IMAGE_OFFSET + sizeof(struct image_head) + skip_len;
	ret = load_data_to_secos(session, PTN_MODEM_DSP_NAME, offset, trans_data.image_length, &skip_len);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load dsp fail!\n");
		return ret;
	}

	trans_data.image_length = MODEM_TDS_SIZE;
	trans_data.image_addr = DDR_LPHY_SDR_ADDR + MODEM_DSP_SIZE + MODEM_TDS_SIZE;
	
 	offset += LPHY_BBE16_MUTI_IMAGE_SIZE + skip_len;
	ret = load_data_to_secos(session, PTN_MODEM_DSP_NAME, offset, trans_data.image_length, &skip_len);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load TDS fail!\n");
		return ret;
	}

	/* set the ok flag of dsp image */
    writel(DSP_IMAGE_STATE_OK, (void*)SHM_MEM_DSP_FLAG_ADDR);

    return SEC_OK;
}


s32 load_modem_image(void)
{
	TEEC_Session session;
	TEEC_Context context;
	s32 ret = SEC_ERROR;
	
	mutex_init(&trans_lock);
	ret = TEEK_init(&session, &context);
	if(SEC_ERROR == ret)
	{
		sec_print_err("TEEK_InitializeContext failed!\n");
		return ret;
	}
	
	ret = load_modem_head();
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_modem_head fail!\n");
		goto error;
	}

	ret = load_VRL_to_secos(&session);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_VRL_and_check fail!\n");
		goto error;
	}

	ret = load_modem_to_secos(&session);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_modem_to_os fail!\n");
		goto error;
	}

	ret = load_modem_dsp_to_secos(&session);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_modem_to_os fail!\n");
		goto error;
	}
	
	/*end of trans all data, start verify*/
	ret = load_done(&session, SECBOOT_CMD_ID_VERIFY_DATA);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_done fail!\n");
		goto error;
	}

error:
	TEEK_CloseSession(&session);
	TEEK_FinalizeContext(&context);
	
	return ret;
}

#else 
s32 load_modem_image(void)
{
	s32 ret = SEC_ERROR;

	reset_print_debug("(%d) vxworks & dsp image is loading...\n", ++g_reset_debug.main_stage);

	ret = bsp_load_modem();
	if(ret)
	{
		sec_print_err("vxworks fail\n");
		return SEC_ERROR;
	}

	ret = bsp_load_modem_dsp();
	if(ret)
	{
		sec_print_err("dsp fail\n");
		return SEC_ERROR;
	}

	return SEC_OK;
}
#endif


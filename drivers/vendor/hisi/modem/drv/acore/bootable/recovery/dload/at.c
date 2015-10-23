
#include <stdlib.h>
#include <linux/fs.h>
#include <fcntl.h>
#include "usb_acm.h"
#include "at.h"
#include "dload.h"
#include "dload_comm.h"
#include "../oeminfo.h"

#define VERSION_LEN 32
#define DLOAD_VERSION               (char *)"2.0"
#define DLOAD_AUTH_VERSION          (char *)"1000"

#define false 0
#define true 1

extern struct dload_ctrl_block *dcb;

struct dload_info_type{
	char software_ver[VERSION_LEN];	
	char product_name[VERSION_LEN];
	char iso_ver[VERSION_LEN];
};


/*****************************************************************************
* 函 数 名  : get_product_name
*
* 功能描述  :获取单板名称
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/

static int get_product_name(char * product_name,int len)
{
	int fd,ret;
	char *sysfs_name="/sys/devices/version/product_name";

	fd=open(sysfs_name,O_RDONLY);
	if(fd<0){
		Dbug("Error:open sysfs fail\n");
		printf("get product name Error:open sysfs fail\n");
		return fd;
	}

	ret=read(fd,product_name,len);
	if(ret>0)
		return OK;
	else
		return ERROR;
}

/*****************************************************************************
* 函 数 名  : get_software_version
*
* 功能描述  :获取软件版本号
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/

static int get_software_version(char * software_version,int len)
{
	/*TODO: sysfs*/
	int fd,ret;
	char sysfs_name[50]="/sys/devices/version/software_ver";

	fd=open(sysfs_name,O_RDONLY);
	if(fd<0){
		Dbug("Error:open sysfs fail\n");
		return fd;
	}

	ret=read(fd,software_version,len);
	if(ret>0)
		return OK;
	else
		return ERROR;
}

/*****************************************************************************
* 函 数 名  : get_iso_version
*
* 功能描述  : 从OEM分区获取iso_version
*
* 输入参数  : 无
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/

static int get_iso_version(char * iso_ver)
{
	unsigned int total_block = 0;
	unsigned int total_size = 0;
	
	/* TODO:oeminfo分区还没有内容
	if(false== oeminfo_init_device())
	{
	        Dbug("open oeminfo partition failed\n");
	        return ERROR;
	 }
	    
	if(false== oeminfo_get_info(OEMINFO_ISO_VER_TYPE,  &total_block, &total_size))
	{
	        Dbug("oeminfo ver info get failed!\n");
	        return ERROR;
	}

	if(false== oeminfo_read(OEMINFO_ISO_VER_TYPE, iso_ver, total_size))
	{
	        Dbug("oeminfo ver info read failed!\n");
	        return ERROR;
	}
	*/
	char *temp="00.000.00.000.00";
	memcpy(iso_ver,temp,16);
	return OK;

}

/*****************************************************************************
* 函 数 名  : get_current_dload_info
*
* 功能描述  : 获取dload_info(升级版本号)
*
* 输入参数  : 无
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
static int  get_current_dload_info(struct dload_info_type* dload_info)
{
	char iso_version[VERSION_LEN];
	char product_name[VERSION_LEN];
	char software_version[VERSION_LEN];
	int ret;

	ret=get_iso_version(iso_version);
	if(ret<0){
		Dbug("Error:get iso version fail!\n");
		return ERROR;
	}
	//memcpy(dload_info->iso_ver,iso_version,VERSION_LEN);
	strcat(dload_info->iso_ver,iso_version);
	printf("iso_ver:%s\n",iso_version);
	
	ret=get_product_name(product_name,VERSION_LEN);
	if(ret<0){
		Dbug("Error:get product name fail!\n");
		return ERROR;
	}
	memcpy(dload_info->product_name,product_name,VERSION_LEN);
	printf("product name:%s\n",product_name);

	ret=get_software_version(software_version,VERSION_LEN);
	if(ret<0){
		Dbug("Error:get product name fail!\n");
		return ERROR;
	}
	memcpy(dload_info->software_ver,software_version,sizeof(software_version));
	printf("software_verr:%s\n",software_version);
	    
	return OK;

}

int strnicmp(register char   *s,
           register char   *t,
                    size_t  len)
{
  char a, b;
  
  while (*s && *t && len) {
    len--;
    a = *s; b = *t;
    if (isupper(a))
      a = tolower(a);
    if (isupper(b))
      b = tolower(b);
    if (a != b)
      return -1;
    s++;
    t++;
  }
  
  /* if we're here because len is zero, don't do the final compare,
   * which verifies that both strings are null terminated, cause
   * chances are, they're not.
   */
  if (len == 0)
    return 0;
  
  if (*s != *t) {
    return -1;
  }
  else {
    return 0;
  }
}

int at_cmd_handler(DLOAD_AT_COMMAND atCmdEnum)
{
    char authorityVer[10] = {0x0};
    char atCmdBuf[150] = {0x0};
    struct dload_info_type dload_info={0};

    char ucAuthorityId[IMEI_LEN + 3] = {0x0};

    memset(atCmdBuf, 0x0, sizeof(atCmdBuf));
    memset(authorityVer, 0x0, sizeof(authorityVer));
    
    switch(atCmdEnum)
    {
        case DLOAD_AT_E0:
        {
            acm_write(ATCOMMAND_OK, strlen(ATCOMMAND_OK));  
        }
        break;

        case DLOAD_AT_E1:
        {
            acm_write(ATCOMMAND_OK, strlen(ATCOMMAND_OK));             
        }
        break;
                
        case DLOAD_AT_DATAMODE:
        {
            acm_write(ATCOMMAND_OK, (unsigned int)strlen(ATCOMMAND_OK));
        }
        break;

        case DLOAD_AT_GODLOAD:
        {
	    if(NULL==dcb)
	    {
			Dbug("Error:dcb is NULL!\n");
			return ERROR;
	    }
            dcb->dload_state = DLOAD_AT_GODLOAD;
            acm_write(ATCOMMAND_OK, (unsigned int)strlen(ATCOMMAND_OK));            
        }
        break;
            
        case DLOAD_AT_AUTHORITYVER:
        {    
		/*TODO:   */
		sprintf((char*)atCmdBuf,"\r\n%s\r\n\r\nOK\r\n",DLOAD_AUTH_VERSION);
		acm_write((char*)atCmdBuf,strlen((const char*)atCmdBuf));
        }
        break;
   
        case DLOAD_AT_AUTHORITYID:
        {             
            if(ERROR == get_authorityId(ucAuthorityId, sizeof(ucAuthorityId)))
            {
                acm_write(ATCOMMAND_ERROR, strlen(ATCOMMAND_ERROR));
                return ATCOMMAND_PROCESS_FAIL;
            }               

            strcpy((char*)atCmdBuf, "\r\n");                       
            strcat((char*)atCmdBuf, (char*)ucAuthorityId);
            strcat((char*)atCmdBuf, "\r\n\r\nOK\r\n");
            
            acm_write(atCmdBuf, strlen((const char *)atCmdBuf));            
        }
        break;
                
        case DLOAD_AT_DLOADVER: 
        {     
	    Dbug("\r\nat command buffer:%s\r\n\r\nOK\r\n",atCmdBuf);
	    Dbug("\r\ndload protocal version:%s\r\n",DLOAD_VERSION);
		
            /* TODO: DLOADVERSION */
	    strcat((char*)atCmdBuf,"\r\n2.0\r\n\r\nOK\r\n");
            acm_write(atCmdBuf, strlen((const char *)atCmdBuf));                
        }
        break;
   
        case DLOAD_AT_DLOADINFO:
        {   
            /* TODO: 单板软件版本号 */
	    /*
	    strcat(atCmdBuf,"\r\n\r\nswver:21.090.00.00.000\r\n\
	    \r\niso:00.000.00.000.00\r\n\
	    \r\nproduct name:MPWUDP\r\n\
	    \r\ndload type:0\r\n\
	    \r\nOK\r\n");
	    */
	    if(get_current_dload_info(&dload_info))
	    {
			Dbug("Error:get current dload info fail!\n");
			return ATCOMMAND_PROCESS_FAIL;
	    }
	    printf("get dload info OK\r\n\
            software_ver is:%s\n\
            iso_version is:%s\n\
            product name is:%s\n", dload_info.software_ver,dload_info.iso_ver,dload_info.product_name);
	    sprintf(atCmdBuf,"\r\n\r\nswver:%s\r\n\
	    \r\niso:%s\r\n\
	    \r\nproduct name:%s\r\n\
	    \r\ndload type:0\r\n\
	    \r\nOK\r\n",dload_info.software_ver,dload_info.iso_ver,dload_info.product_name);
            Dbug("get dload info OK\r\n\
            software_ver is:%s\n\
            iso_version is:%s\n\
            product name is:%s\n", dload_info.software_ver,dload_info.iso_ver,dload_info.product_name);
            acm_write(atCmdBuf, strlen((const char *)atCmdBuf));   
        }
        break;
        
        case DLOAD_AT_CMEE:
        {
	//返回错误码
            acm_write(ATCOMMAND_OK, strlen(ATCOMMAND_OK));             
        }
        break;
		
	case DLOAD_AT_NVBACKUP:
	{
		strcat(atCmdBuf,"\r\n0\r\n\r\nOK\r\n");
		acm_write(atCmdBuf, strlen((const char *)atCmdBuf));   
	}
	break;
	
        default:
        {
            acm_write(ATCOMMAND_ERROR, strlen(ATCOMMAND_ERROR));
            return ATCOMMAND_PROCESS_FAIL;
        }
    }

     return ATCOMMAND_PROCESS_SUCEED; 
}


/*****************************************************************************
* 函 数 名  : at_cmd_proc
*
* 功能描述  : AT命令处理
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int at_cmd_proc( unsigned char *pcAtcmd, int len )
{
    struct atcmd{
            char *atCmdStr;
            DLOAD_AT_COMMAND atCmdEnum;
        } atcmdArry[] = {
                             {"ATE0\r", DLOAD_AT_E0},
                             {"ATE1\r", DLOAD_AT_E1},   
                             {"AT^DLOADVER?\r", DLOAD_AT_DLOADVER},
                             {"AT^DLOADINFO?\r", DLOAD_AT_DLOADINFO},
                             {"AT^AUTHORITYVER?\r", DLOAD_AT_AUTHORITYVER},
                             {"AT^AUTHORITYID?\r", DLOAD_AT_AUTHORITYID},
                             {"AT^DATAMODE\r",  DLOAD_AT_DATAMODE},
                             {"AT^HWNATQRY?\r", DLOAD_AT_HWNATQRY},
                             {"AT^FLASHINFO?\r", DLOAD_AT_FLASHINFO},
                             
                             {"AT+CMEE=0\r", DLOAD_AT_CMEE},
                             {"AT^GODLOAD\r", DLOAD_AT_GODLOAD},
			     {"AT^NVBACKUP\r",DLOAD_AT_NVBACKUP}
                             
                        };

    unsigned int  i = 0x0;

    if(NULL == pcAtcmd)
    {
        acm_write(ATCOMMAND_ERROR, strlen(ATCOMMAND_ERROR));
        return ERROR;
    }

    for(i = 0; i < sizeof(atcmdArry)/sizeof(atcmdArry[0]); i++)
    {
        if (len != strlen(atcmdArry[i].atCmdStr))
        {
            continue;
        }

        if(0 == strnicmp(atcmdArry[i].atCmdStr, pcAtcmd,len))
        {
            break;
        }
    }

    if(i == sizeof(atcmdArry)/sizeof(atcmdArry[0]))
    {
        return ATCOMMAND_NOT_FOUND;
    }

    return at_cmd_handler(atcmdArry[i].atCmdEnum);
}


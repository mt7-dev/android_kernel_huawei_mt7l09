/******************************************************************************
  File Name       : boot.c
  Description     : create boot tags 
  History         :
******************************************************************************/

#include <boot/boot.h>
#include <boot/boardid.h>
#include <balongv7r2/types.h>
#include <balongv7r2/preboot.h>

#define FACTORY_SWTYPE_TAG            "androidboot.swtype=factory"
#define NORMAL_SWTYPE_TAG             "androidboot.swtype=normal"

#define NORMAL_REST_TAG				"reset_type=normal"
#define ABNORMAL_REST_TAG			    "reset_type=abnormal"

#define POWER_MODE_RECOVERY		    "power_mode=recovery"
#define POWER_MODE_NORMAL			    "power_mode=normal"
#define POWER_MODE_UNKOWN			    "power_mode=unknow"

void num_to_dec(unsigned n, char *out)
{
    static char todec[11] = "0123456789";
    char tmp[20];
    int i = 0;
    int j = 0;
	/* coverity[var_compare_op] */
    if(NULL == out) 
    {
        cprintf("num_to_dec:out is NULL\n");
        return;
    }

    while( i < 20 ) 
    {
        tmp[i] = todec[n%10];
        if( !(n = n/10) ) 
            break;
        i++;
    }
    while( i >= 0 )
    {
    	/* coverity[var_deref_op] */
        out[j++] = tmp[i--];
    }
    out[j] = 0;
}

/*****************************************************************************
 函 数 名  : creat_pd_charge_tag
 功能描述  : 创建关机充电启动参数
 输入参数  : 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  :
*****************************************************************************/    
int creat_pd_charge_tag(char* pd_flag)
{
	unsigned int len = 0;
	unsigned int flag = 0;
	char buf[20];

	flag = get_pw_charge_flag();

	num_to_dec(flag, buf);

	len = strlen(pd_flag);
	memcpy(pd_flag + len, "pd_charge=", 11);

	len = strlen(pd_flag);
	memcpy(pd_flag + len, buf, strlen(buf) + 1);

	len = strlen(pd_flag);
	memcpy(pd_flag + len, " ", 2);

	return 0;
}

/*****************************************************************************
 函 数 名  : creat_boardid_tag
 功能描述  : 创建单板配置(硬件ID/PMU类型等)启动参数
 输入参数  : 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  :
*****************************************************************************/    
int creat_boardid_tag( const char* boardid_in_cmd, char* boardid_out_cmd )
{
	int length = 0;
	
	if(NULL != boardid_in_cmd) {
		
		length= strlen( boardid_out_cmd );
		memcpy(boardid_out_cmd + length, boardid_in_cmd, strlen(boardid_in_cmd));
		length = strlen( boardid_out_cmd );

		/* copy one more byte '\0' as end of whole cmd string */
		memcpy( boardid_out_cmd + length, " ", 2 ); 
	}

	return 0;
}

/*****************************************************************************
 函 数 名  : create_versionmode_tag
 功能描述  : 创建版本类型启动参数
 输入参数  : 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  :
*****************************************************************************/    
int create_versionmode_tag( char* versionmode)
{
	int length = 0;
	length= strlen( versionmode );
    
    if(FACTORY_SW_TYPE == get_soft_version_flag()){
	    memcpy( versionmode + length, FACTORY_SWTYPE_TAG, strlen(FACTORY_SWTYPE_TAG) + 1 );}
    else{
	    memcpy( versionmode + length, NORMAL_SWTYPE_TAG, strlen(NORMAL_SWTYPE_TAG) + 1 );
    }
	length = strlen( versionmode );
	
	/* copy one more byte '\0' as end of whole cmd string */
	memcpy( versionmode + length, " ", 2 ); 	

	return 0;
}

/*****************************************************************************
 函 数 名  : create_resettype_tag
 功能描述  : 创建复位类型启动参数
 输入参数  : 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  :
*****************************************************************************/    
int  create_resettype_tag( char* resettpye)
{
	int length = 0;
	length= strlen( resettpye );

	if (ABNORMAL_RST_TYPE == get_abnormal_reset_flag())
		memcpy( resettpye + length, ABNORMAL_REST_TAG, strlen(ABNORMAL_REST_TAG) + 1 );
	else
		memcpy( resettpye + length, NORMAL_REST_TAG, strlen(NORMAL_REST_TAG) + 1 );

	length = strlen( resettpye );

	/* copy one more byte '\0' as end of whole cmd string */
	memcpy( resettpye + length, " ", 2 );

	return 0;
}

/*****************************************************************************
 函 数 名  : create_pwmode_tag
 功能描述  : 创建启动模式启动参数
 输入参数  : 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  :
*****************************************************************************/    
int  create_pwmode_tag( char* pwmode)
{
	int length = 0;
	length= strlen( pwmode );
    int rt;
    
    rt = get_pw_mode_flag();
    
	if (BOOT_ON_NORMAL_MODE == rt)
    {
		memcpy( pwmode + length, POWER_MODE_NORMAL, strlen(POWER_MODE_NORMAL) + 1 );
    }
    else if(BOOT_ON_RECOVERY_MODE == rt)
    {
		memcpy( pwmode + length, POWER_MODE_RECOVERY, strlen(POWER_MODE_RECOVERY) + 1 );
    }
    else
    {
		memcpy( pwmode + length, POWER_MODE_UNKOWN, strlen(POWER_MODE_UNKOWN) + 1 );
    }
    
	length = strlen( pwmode );

	/* copy one more byte '\0' as end of whole cmd string */
	memcpy( pwmode + length, " ", 2 );

	return 0;
}

/*****************************************************************************
 函 数 名  : creat_cmdline
 功能描述  : 创建启动参数
 输入参数  : 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  :
*****************************************************************************/    
int creat_cmdline(char* hdrcmdline, char* cmdline )
{
	char *boardid_string = config_boardid_to_string();
    UNUSED(hdrcmdline);
	if(NULL!=boardid_string) {
		creat_boardid_tag( boardid_string, cmdline );
	}	

	creat_pd_charge_tag(cmdline);

	create_versionmode_tag(cmdline);

	create_resettype_tag(cmdline);

    create_pwmode_tag(cmdline);
    
	return 0;
}


#ifndef _IOS_LIST_H_
#define _IOS_LIST_H_

#define IOS_OK      0
#define IOS_ERROR   -1

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __FASTBOOT__
int ios_list_init(void);
#endif

#ifndef __CMSIS_RTOS
int add_ios_list(unsigned int addr);
void ios_addr_show(void);
#endif

int ios_config_resume(void);
void ios_list_echo(void);

#ifdef __cplusplus
}
#endif

#endif

/*end of this file*/

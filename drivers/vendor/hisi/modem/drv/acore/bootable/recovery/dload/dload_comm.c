
#include <linux/types.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <mtd/mtd-abi.h>
#include "dload_comm.h"
#include "mtdutils/mtdutils.h"
#include "cutils/android_reboot.h"

/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/
#define OOB_SIZE (16)
#define PTABLE_SIZE AXI_PTABLE_SIZE_MAX
#define RECOVERY_ANAME "recovery-a"
#define RECOVERY_BNAME "recovery-b"
#define PTABLE_NAME	"ptable"
#define PTABLE_HEAD_SIZE                sizeof(struct ST_PART_TBL)
//#define PTABLE_END_STR          "T" /*"pTableTail"*/


/*--------------------------------------------------------------*
 * 全局变量申明                                                 *
 *--------------------------------------------------------------*/
//enum IMAGE_TYPE g_ucFinishImageType         = IMAGE_SFS_TOP;
//unsigned int g_ulDloadMidwayRestartFlag=0;
//unsigned int g_ulFlashDloadAlignSize;    /*需要做到这几个大小的对齐:1.USB数据包大小4k, 2.nandpage大小2k，3.yaffspage大小2k+16 */
unsigned int mtd_partitions_scanned =0;

/*--------------------------------------------------------------*
 * 外部变量引用                                                 *
 *--------------------------------------------------------------*/
extern  struct dload_ctrl_block *dcb;

void dload_reboot(void)
{
	android_reboot(ANDROID_RB_RESTART, 0, 0);
}
/*****************************************************************************
* 函 数 名    : write_ptable_to_flash
*
* 功能描述  : 向flash中写入ptable   
*
* 输入参数  : 
			data:需要写入的数据buf
			size:需要写入的数据长度
* 输出参数  : 
*
* 返 回 值    : 
			0:  	写入成功
			-1:	写入失败
* 其它说明  : 
*
*****************************************************************************/

int write_ptable_to_flash(char *data,ssize_t size)
{
	MtdWriteContext *write;

	if (!mtd_partitions_scanned) {
                mtd_scan_partitions();
                mtd_partitions_scanned = 1;
          }

	/*查找到flash中的ptable分区*/
	const MtdPartition* part=mtd_find_partition_by_name(PTABLE_NAME);
	if (part == NULL) {
                Dbug("mtd partition \"%s\" not found for writing\n", PTABLE_NAME);
	        return ERROR;
         }

	/*以写的方式打开ptable分区*/
	write=mtd_write_partition(part);
	if(write==NULL){
		Dbug("Error:can't open ptable partition!\n");
		return ERROR;
	}
	
	/*向flash中写入修改后的ptable*/
	if(mtd_write_data(write,data,size)!=size){
		Dbug("Error:can't write ptable partition!\n");
		mtd_write_close(write);
		return ERROR;	
	}

	if(mtd_erase_blocks(write, -1) < 0) {
                Dbug("error finishing mtd write of ptable!\n");
                mtd_write_close(write);
		return ERROR;
	}
	/*关闭ptable分区*/
	if(mtd_write_close(write)){
		Dbug("Error:can't finish ptable partition!\n");
		return ERROR;
	}
	return OK;
}

/*****************************************************************************
* 函 数 名    : dload_get_proc_ptable
*
* 功能描述  : 通过proc_fs获取内存中分取表的信息
*
* 输入参数  : 
			data:需要读出的数据buf
			size:需要读出的数据长度
* 输出参数  : 
*
* 返 回 值    : 
			0:  	获取成功
			-1:	获取失败
* 其它说明  : 此函数获取的分区表不包含头部信息，需要get_ptable_from_ram函数解析后才可以使用
*
*****************************************************************************/

static int dload_get_proc_ptable(char *ptable_product,int len)
{
	int ret;
	int fd;
	char ptable_procfile_name[50]="/proc/ptable/ptable_bin";

	if(len<0)
	{
		Dbug("Error:buf len is invalid!\n");
		return -1;
	}
	/*以读的方式打开ptable proc文件*/
	fd=open(ptable_procfile_name,O_RDONLY);
	if(fd<0)
	{
		Dbug("Error:can not open ptable proc file!ERROR NO is %d\n",ret);
		return -1;
	}
	/*将分区表中的内容读出来，放置buf中*/
	ret=read(fd,ptable_product,len-1);
	
	close(fd);
	if(ret<0)
	{
		Dbug("Error:can not open ptable proc file!ERROR NO is %d\n",ret);
		free(ptable_product);
		return -1;
	}
	ptable_product[ret]='\0';
	
	return ret;

}

/*****************************************************************************
* 函 数 名    : get_ptable_from_ram
*
* 功能描述  : 获取内存中的分区表
*
* 输入参数  : 
			ptable:需要读出的数据buf
			
* 输出参数  : 
*
* 返 回 值    : 
			  0:  	获取成功
			-1:	获取失败
* 其它说明  : 
*
*****************************************************************************/
int get_ptable_from_ram(struct ST_PART_TBL* ptable)
{
	struct ST_PART_TBL* ptable_temp;
	char *ptable_proc=NULL;
	char *ptable_proc_temp=ptable_proc;
	int ret;					/*用于记录从内存中读出的ptable信息的长度*/

	/*申请2k空间,用于从内存存储从内存中读出来的ptable*/
	ptable_proc=(char *)malloc(PTABLE_SIZE);	
	if(NULL==ptable_proc)
	{
		Dbug("malloc for proc ptable fail!");
		goto error;
	}

	/*读取内存中的分区表信息*/
	ret=dload_get_proc_ptable(ptable_proc, PTABLE_SIZE);
	if(ret<0)
	{
		Dbug("Error:get ptable proc file fail!\n");
		goto error;
	}

	/*解析从内存中读出的pable信息*/
	int	i=0;
	ptable_temp=ptable;
	ptable_proc=ptable_proc+PTABLE_HEAD_SIZE;
	while(ret>0&&NULL!=ptable_temp)
	{
		int matches=0;
		ptable_temp->name[0]='\0';


		matches=sscanf(ptable_proc,"%x %x %x %x %x %x %x %x \"%15[^\"]",
								&ptable_temp->image,
								&ptable_temp->offset,
								&ptable_temp->loadsize,
								&ptable_temp->capacity,
								&ptable_temp->loadaddr,
								&ptable_temp->entry,
								&ptable_temp->property,
								&ptable_temp->count,
								ptable_temp->name);
		
		/*如果成功获取一条分区表数据，指针向后移动*/
		if(matches==9){
			ptable_temp++;
		}
		while(ret>0&&*ptable_proc!='\n'){
			ptable_proc++;
			ret--;
		}
		if(ret>0)
		{
			ptable_proc++;
			ret--;
		}
	}
	ptable_temp->name[0]='\0';
	strcat(ptable_temp->name,PTABLE_END_STR);
	free(ptable_proc_temp);
	return OK;
error:
	free(ptable_proc_temp);
	return ERROR;
}

/*****************************************************************************
* 函 数 名    : get_ptable_from_flash
*
* 功能描述  : 获取flash中的分区表
*
* 输入参数  : 
			ptable:需要读出的数据buf
			
* 输出参数  : 
*
* 返 回 值    : 
			  0:  	获取成功
			-1:	获取失败
* 其它说明  : 此函数获取的分区表包含头部信息，为char*格式
*
*****************************************************************************/
int get_ptable_from_flash(char* ptable)
{
	MtdReadContext *read;
	ssize_t size=PTABLE_SIZE;
	char* data;

	data=(char *)malloc(size);
	if (!mtd_partitions_scanned) {
                mtd_scan_partitions();
                mtd_partitions_scanned = 1;
          }

	/*查找到flash中的ptable分区*/
	const MtdPartition* part=mtd_find_partition_by_name(PTABLE_NAME);
	if (part == NULL) {
                Dbug("mtd partition \"%s\" not found for writing\n", PTABLE_NAME);
	        goto error;
         }
	/*以读的方式打开分区*/
	read=mtd_read_partition(part);
	if(read==NULL){
		Dbug("Error:can't open ptable\n");

		goto error;
	}
	/*读取分区中的内容*/
	ssize_t ret=mtd_read_data(read,data,size);
	if(ret!=size){
		Dbug("Error:mtd can't read ptable!\n");
		goto error;
	}
	/*读操作完毕，关闭*/
	mtd_read_close(read);
	if(ret!=size){
		Dbug("Error:mtd can't read ptable!\n");
		goto error;
	}
	/*修改分区表中对应的recovery分区的count值*/
	memcpy(ptable,data,PTABLE_SIZE);

	free(data);
	return OK;
	
error:
	free(data);
	return ERROR;

}

/*****************************************************************************
* 函 数 名    : get_ptable_changed
*
* 功能描述  : 判断分区表是否变化
*
* 输入参数  : 
			ptable:需要读出的数据buf
			
* 输出参数  : TODO
*
* 返 回 值    :  TODO
			  0:  	无变化
			-1:	发生变化
* 其它说明  :  TODO
*
*****************************************************************************/

int get_ptable_changed(void)
{
	struct ST_PART_TBL* old_ptable;		/*旧分区表(内存中的分区表)*/
	struct ST_PART_TBL* new_ptable;	/*新分区表(flash中的分区表)*/
	char *new_ptable_temp=NULL;
	char *old_ptable_temp =NULL;
	char old_ptable_name[PTABLE_NAME_SIZE];
	char old_ptable_property[PTABLE_HEAD_PROPERTY_SIZE];
	char old_ptable_ver[PTABLE_VER_ID_SIZE*2];
	int matches=0;
	int changed=0;
	int ret;

	/*申请2k空间,用于从内存存储从内存中读出来的ptable*/
	old_ptable_temp=(char *)malloc(PTABLE_SIZE);
	if(NULL==old_ptable_temp)
	{
		Dbug("malloc for proc ptable fail!");
		return ERROR;
	}

	/*申请2k空间,用于从存储flash中的ptable*/
	new_ptable_temp=(char *)malloc(PTABLE_SIZE);
	if(NULL==new_ptable_temp)
	{
		Dbug("malloc for flash ptable fail!");
		free(old_ptable_temp);
		return ERROR;
	}


	/*读取内存中的分区表信息*/
	ret=dload_get_proc_ptable(old_ptable_temp, PTABLE_SIZE);
	if(ret<0)
	{
		Dbug("Error:get ptable proc file fail!\n");
		changed=-1;
		goto exit;
	}
	
	/*读取flash中的分区表*/
	ret=get_ptable_from_flash(new_ptable_temp);
		if(ret<0)
	{
		Dbug("Error:get ptable from flash fail!\n");
		changed=-1;
		goto exit;
	}

	/*解析从内存中读出来的分区表的头部信息*/
	/*proc fs中的数据形式
	seq_printf(sfile,"%s %s %s\n",
				(char*)ptable_product+PTABLE_HEAD_PROPERTY_OFFSET,
				(char*)ptable_product + PTABLE_BOOTROM_VER_OFFSET,
				(char*)ptable_product + PTABLE_NAME_OFFSET);

	*/
	old_ptable_name[0]='\0';//memset
	old_ptable_property[0]='\0';
	old_ptable_ver[0]='\0';
	
	matches=sscanf(old_ptable_temp,"%s %s %s ",
						old_ptable_name,
						old_ptable_property,
						old_ptable_ver);
	if(3!=matches)
	{
		Dbug("Error:can't get old ptable head infomation");
		changed=-1;
		goto exit;
	}
	printf("old_ptable_name=%s old_ptable_property=%s old_ptable_ver=%s ",
			old_ptable_name,
			old_ptable_property,
			old_ptable_ver);
	
	/*比较新、旧两个分区表的表头信息是否有差异*/	
	    if(((!strncmp((const void *)(new_ptable_temp + PTABLE_NAME_OFFSET),old_ptable_name,PTABLE_NAME_SIZE)) \
            || (!strncmp((const void *)(new_ptable_temp + PTABLE_HEAD_PROPERTY_OFFSET),old_ptable_property,PTABLE_HEAD_PROPERTY_SIZE))) \
            || (!strncmp((const void *)(new_ptable_temp + PTABLE_BOOTROM_VER_OFFSET),old_ptable_ver,PTABLE_VER_ID_SIZE)))
	    {
		changed=1;
		Dbug("Ptable head has changed!\n");
		goto exit;
	    }
	
	/*比较新、旧两个分区表，除count和size两列，其他是否有差别*/
	new_ptable=(struct ST_PART_TBL*)(new_ptable_temp+PTABLE_HEAD_SIZE);
	old_ptable=(struct ST_PART_TBL*)(new_ptable_temp+PTABLE_HEAD_SIZE);

	while((0 != strcmp((const void *)PTABLE_END_STR, (const void *)old_ptable->name)) &&
          		(0 != strcmp((const void *)PTABLE_END_STR, (const void *)new_ptable->name)))
	{
		if(
	                (old_ptable->offset != new_ptable->offset)      ||
	                (old_ptable->capacity != new_ptable->capacity)  ||
	                (old_ptable->loadaddr != new_ptable->loadaddr)  ||
	                (old_ptable->entry != new_ptable->entry)  ||
	                (old_ptable->image != new_ptable->image)  ||
	                (0 != strcmp( (const void *)old_ptable->name, (const void *)new_ptable->name))
	           )
	        {
		            /*发现不同，设置标志位*/
		            changed=1;
			    Dbug("ptable context changed\n");
		            goto exit;
	        }
        
	        old_ptable++;
	        new_ptable++;
    	}

		/*新老表是否个数有差异*/
	    if((0 != strcmp(PTABLE_END_STR, new_ptable->name))||(0 != strcmp(PTABLE_END_STR, old_ptable->name)))
	    {
			/*发现个数差异，设置标志位*/
			changed=1;
			Dbug("ptable number changed \n");
			goto exit;
	    }

exit:	
	free(old_ptable_temp);
	free(new_ptable_temp);
	return changed;
}

/*****************************************************************************
* 函 数 名    : dload_get_nand_page_size
*
* 功能描述  : 获取到nand flash的页大小
*
* 输入参数  : 
			fd:操作mtd设备时对应的文件描述符
			
* 输出参数  : 无
*
* 返 回 值    : 
			  ret:  	nand的页大小
			  -1:		获取失败
* 其它说明  : 
*
*****************************************************************************/

unsigned int get_nand_page_size(int fd)
{
	struct mtd_info_user mtd_info;
	int ret;
	memset(&mtd_info,0,sizeof(mtd_info));

	/*获取mtd_info*/
	ret=ioctl(fd,MEMGETINFO,&mtd_info);
	
	if ( ret < 0) {
		Dbug("system image:write data error no is%d \n",ret);
		return ERROR;
	}
	/*读出nand page 的大小*/
	ret=mtd_info.writesize;
	Dbug("nand page size is 0x%x\n",ret);
	
	return ret;
}

/*****************************************************************************
* 函 数 名    : get_image_property
*
* 功能描述  : 获取image的属性(升级镜像之前，需要判断该镜像是文件系统镜像 ，还是普通镜像)
*
* 输入参数  : 
			image_idx:	需要烧写镜像的ID	
			
* 输出参数  : 无
*
* 返 回 值    : 
			  property:  	镜像的属性值
				   -1:	操作失败
* 其它说明  : 
*
*****************************************************************************/
int get_image_property(unsigned int image_idx)
{
	int ret;
	int property=-1;
	struct ST_PART_TBL *ptable_temp;
	struct ST_PART_TBL *ptable=(struct ST_PART_TBL *)malloc(PTABLE_SIZE);
	if(!ptable)
	{
		Dbug("Error:can't malloc for ptable_product!\n");
		return ERROR;
	}

	/*获取ram中的分区表，不包含头部信息*/
	ptable_temp=ptable;
	ret=get_ptable_from_ram(ptable);
	if(ret<0)
	{
		Dbug("Error:get ptable from ram failed\n");
		goto error;
	}

	/*获取到ptable中ID为image_idx镜像的count值*/
	while(NULL!=ptable&&strcmp(ptable->name,PTABLE_END_STR))
	{
		if(ptable->image==image_idx){
				property=ptable->property;
		}
		else{
			;
		}
		ptable++;
	}

	free(ptable_temp);
	return property;
error:

	free(ptable_temp);
	return ERROR;
}
/*****************************************************************************
* 函 数 名    : update_image_count
*
* 功能描述  : 升级镜像时，修改分区表中的count 的值 
*
* 输入参数  : 无			
			
* 输出参数  : 无
*
* 返 回 值    : 
			  0:  	操作成功
			-1:	操作失败
* 其它说明  : 
*
*****************************************************************************/

int update_image_count(enum IMAGE_TYPE image_idx)
{
	char *name;
	char*ptable_flash;	/*用来存放从flash中读出的ptable,不包含头部信息*/
	struct ST_PART_TBL *ptable_flash_temp;
	int ret;

	if(IMAGE_RECOVERY_A==image_idx)
		return update_recovery_count();

	/*申请内存*/
	ptable_flash=(char *)malloc(PTABLE_SIZE);
	if(NULL==ptable_flash)
	{
		Dbug("Error:malloc failed!\n");
		return ERROR;
	}

	/*获取flash中的分区表，包含头部信息*/
	ret=get_ptable_from_flash(ptable_flash);
	if(ret<0)
	{
		Dbug("Error:get ptable from flash failed\n");
		goto error;
	}
	
	/*去掉分区表的头部信息(本功能不关注头部信息)*/
	ptable_flash_temp=(struct ST_PART_TBL *)(ptable_flash+PTABLE_HEAD_SIZE);

	/*根据imageID找到对应的镜像，同时更新镜像的count值*/
	while(NULL!=ptable_flash_temp&&strcmp(ptable_flash_temp->name,PTABLE_END_STR))
	{
		if(ptable_flash_temp->image==image_idx){
				ptable_flash_temp->count=ptable_flash_temp->count+1;
		}
		else{
			;
		}
		ptable_flash_temp++;
	}
	
	/*将更新后的buf写入flash*/
	ret=write_ptable_to_flash(ptable_flash,PTABLE_SIZE);
	if(ret<0){
		Dbug("Error:update ptable failed!\n");
		goto error;
	}
	
	free(ptable_flash);
	return OK;

error:
	free(ptable_flash);
	return ERROR;
	
}
/*****************************************************************************
* 函 数 名    : update_ptable_count
*
* 功能描述  : 升级ptable镜像时，更新分区表中的count 的值 
*
* 输入参数  : 无			
			
* 输出参数  : 无
*
* 返 回 值    : 
			  0:  	操作成功
			-1:	操作失败
* 其它说明  : 
*
*****************************************************************************/
int update_ptable_count(void)
{
	struct ST_PART_TBL *ptable_flash;	/*用来存放从flash中读出的ptable,不包含头部信息*/
	struct ST_PART_TBL *ptable_ram;	/*用来存放从内存中读出的ptable*/
	struct ST_PART_TBL *ptable_ram_temp;	/*为了释放ptable_ram申请的空间*/
	char *ptable_flash_all;		/*用来存放从flash中读出的ptable,包含头部信息*/
	int ret=-1;

	/*申请内存*/
	ptable_flash_all=(char *)malloc(PTABLE_SIZE);
	if(NULL==ptable_flash_all)
	{
		Dbug("Error:malloc failed!\n");
		return ERROR;
	}
	ptable_ram=(struct ST_PART_TBL *)malloc(PTABLE_SIZE);
	if(NULL==ptable_ram)
	{
		Dbug("Error:malloc failed!\n");
		free(ptable_flash_all);
		return ERROR;
	}

	/*获取flash中的分区表，包含头部信息*/
	ret=get_ptable_from_flash(ptable_flash_all);
	if(ret<0)
	{
		Dbug("Error:get ptable from flash failed\n");
		goto error;
	}
	
	/*去掉分区表的头部信息(本功能不关注头部信息)*/
	ptable_flash=(struct ST_PART_TBL *)(ptable_flash_all+PTABLE_HEAD_SIZE);

	/*获取ram中的分区表，不包含头部信息*/
	ptable_ram_temp=ptable_ram;
	ret=get_ptable_from_ram(ptable_ram);
	if(ret<0)
	{
		Dbug("Error:get ptable from ram failed\n");
		goto error;
	}

	while(strcmp(ptable_ram->name,PTABLE_END_STR)&&strcmp(ptable_flash->name,PTABLE_END_STR))
	{
		if(ptable_ram->image==ptable_flash->image)
			ptable_flash->count=ptable_ram->count;
		
		else{
			Dbug("Error:ram ptable is different with flash ptable!\n");
			goto error;
		}
		if(ptable_flash->image==IMAGE_PTABLE)
			ptable_flash->count=ptable_flash->count+1;
		ptable_ram++;
		ptable_flash++;
	}

	/*将更新后的buf写入flash*/
	ret=write_ptable_to_flash(ptable_flash_all,PTABLE_SIZE);
	if(ret<0){
		Dbug("Error:update ptable failed!\n");
		goto error;
	}
	
	/*释放申请的空间*/
	free(ptable_ram_temp);
	free(ptable_flash_all);
	return OK;

error:
	free(ptable_ram_temp);
	free(ptable_flash_all);
	return ERROR;
}

/*****************************************************************************
* 函 数 名    : update_recovery_count
*
* 功能描述  : 升级recovery镜像之后，修改分区表中的recovery 分区的count 的值 
*
* 输入参数  : 无			
			
* 输出参数  : 无
*
* 返 回 值    : 
			  0:  	操作成功
			-1:	操作失败
* 其它说明  : 
*
*****************************************************************************/
int update_recovery_count(void)
{
	struct ST_PART_TBL *ptable,*ptable_temp;
	char* data;
	ssize_t size=PTABLE_SIZE;
	data=(char *)malloc(PTABLE_SIZE);
	int count_i=0;		/*用于ptable_temp 的计数*/
	int count_a=-1;		/*用于recovery-A的count值*/
	int count_b=-1;		/*用于recovery-B的count值*/
	int a=0;			/*用于recovery-A指针位置的标置*/
	int b=0;			/*用于recovery-B指针位置的标置*/
	int ret;
	MtdReadContext *read;
	MtdWriteContext *write;

	if (!mtd_partitions_scanned) {
                mtd_scan_partitions();
                mtd_partitions_scanned = 1;
          }

	/*获取flash中的分区表，包含头部信息*/
	ret=get_ptable_from_flash(data);
	if(ret<0)
	{
		Dbug("Error:get ptable from flash failed\n");
		goto error;
	}
	ptable_temp=(struct ST_PART_TBL *)(data+PTABLE_HEAD_SIZE);
	ptable=ptable_temp;

	/*修改分区表中对应的recovery分区的count值*/
	while(0 != strcmp(PTABLE_END_STR, ptable_temp->name))
	{
		if(!strcmp(ptable_temp->name,RECOVERY_ANAME)){
			count_a=ptable_temp->count;
			a=count_i;
		}
		if(!strcmp(ptable_temp->name,RECOVERY_BNAME)){
			count_b=ptable_temp->count;
			b=count_i;
		}
		count_i++;
		ptable_temp++;
	}

	/* 判断是否获取到count的值 */
	if(count_a<0||count_b<0)
	{
		Dbug("Error:modify recovery count fail!\n");
		goto error;
	}
	
	/*recovery 双备份:
	  *初始的recovery分区的count值均为0，此时向A升级镜像，其count+1;
	  *非第一次升级，则找count值小的升级，升级成功后其count+2			*/
	if(count_a<count_b)
	{
		ptable[a].count=ptable[a].count+2;
	}
	else if(count_a>count_b)
	{
 		ptable[b].count=ptable[b].count+2;

	}
	else
	{
		ptable[a].count=ptable[a].count+1;

	}

	/*将更新后的buf写入flash*/
	ret=write_ptable_to_flash(data,PTABLE_SIZE);
	if(ret<0)
	{
		Dbug("Error:write ptable to flash failed!\n");
		goto error;
	}

	free(data);
	return OK;
error:
	
	free(data);
	return ERROR;

}
/*****************************************************************************
* 函 数 名    : ptable_find_older_recovery
*
* 功能描述  : 升级recovery镜像时，根所分区表中的recovery 分区的count 的值判断升级哪个分区
*
* 输入参数  : 无			
			
* 输出参数  : 无
*
* 返 回 值    : 
			  name:  	可升级的recovery分区的名字
			  NULL:	获取失败
* 其它说明  : 
*
*****************************************************************************/
char* ptable_find_older_recovery(void)
{
	int recovery_acount=-1;
	int recovery_bcount=-1;
	int i,ret;
	char *name;
	struct ST_PART_TBL *ptable=(struct ST_PART_TBL *)malloc(PTABLE_SIZE);
	struct ST_PART_TBL *ptable_temp;
	
	if(!ptable)
	{
		Dbug("Error:can't malloc for ptable_product!\n");
		return NULL;
	}

	/*获取ram中的分区表，不包含头部信息*/
	ptable_temp=ptable;
	ret=get_ptable_from_ram(ptable);
	if(ret<0)
	{
		Dbug("Error:get ptable from ram failed\n");
		goto error;
	}

	/*获取到recovery镜像中的count值*/
	while(NULL!=ptable&&strcmp(ptable->name,PTABLE_END_STR))
	{
		if(!strcmp(ptable->name,RECOVERY_ANAME)){
				recovery_acount=ptable->count;
		}
		else if(!strcmp(ptable->name,RECOVERY_BNAME)){
				recovery_bcount=ptable->count;

		}
		else{
			;
		}
		ptable++;
	}
	/*判断recovery count的合法性*/	
	if(recovery_acount<0||recovery_bcount<0)
	{
		Dbug("Error:find recovery count fail!recoverya_count: %x,recoveryb_count:%x\n",recovery_acount,recovery_bcount);
		goto error;
	}

	/*recovery镜像升级时，向recovery分区中count值较小的那个分区升级(分区的count小，代表为旧分区)*/
	name=recovery_acount>recovery_bcount?RECOVERY_BNAME:RECOVERY_ANAME;
	free(ptable_temp);
	return name;

error:
	free(ptable_temp);
	return NULL;
}

/*****************************************************************************
* 函 数 名    : ptable_find_name_by_type
*
* 功能描述  : 升级时，根所分区表中的imagetype查找到分区的名字，以查找mtd分区
*
* 输入参数  : 
			typeid:需要升级的镜像的imageID 			
			
* 输出参数  : 无
*
* 返 回 值    : 
			  name:  	升级的分区的名字
			  NULL:	获取失败
* 其它说明  : 
*
*****************************************************************************/

char* ptable_find_name_by_type(unsigned int type_id)
{
	int i,ret;
	int fd;
	char  *ptable_product=(char *)malloc(PTABLE_SIZE);
	char *ptable_product_temp=ptable_product;
	char  ptable_name[50];
	char *recovery_name;
	
	if(!ptable_product)
	{
		Dbug("Error:can't malloc for ptable_product!\n");
		free(ptable_product);
		return NULL;
	}
	
	if(type_id==IMAGE_RECOVERY_A)
	{
		recovery_name=NULL;
		ptable_name[0]='\0';
		recovery_name=ptable_find_older_recovery();
		if(NULL==recovery_name){
			Dbug("find older recovery failed!\n");

		}
		return recovery_name;
	}
	ret=	dload_get_proc_ptable(ptable_product, PTABLE_SIZE);
	if(ret<0)
	{
		Dbug("Error:can't get ptable proc file!\n");
		return NULL;
	}
	
	i=0;
	ptable_product=ptable_product+PTABLE_HEAD_SIZE;
	while(ret>0)
	{
		int imagetype;
		char *name;
		int matches;
		
		ptable_name[0]='\0';
		imagetype=-1;


		matches=sscanf(ptable_product,"%x %*x %*x %*x %*x %*x %*x %*x \"%49[^\"]",&imagetype,ptable_name);

		if(matches==2){
			name=strdup(ptable_name);
			
			if(type_id==imagetype){
				free(ptable_product_temp);
				return ptable_name;
			}
		}
		while(ret>0&&*ptable_product!='\n'){
			ptable_product++;
			ret--;
		}
		if(ret>0)
		{
			ptable_product++;
			ret--;
		}
	}
	
	free(ptable_product_temp);
	return NULL;
}


/*****************************************************************************
* 函 数 名    : ptable_dload_write_fsimg
*
* 功能描述  : 文件系统镜像烧写
*
* 输入参数  : 
			data_buf  :	需要烧写的镜像的地址
			image_idx:	镜像类型
			this_offset:	本次烧写的内容在buf地址中的偏移
			len           :	要烧写的数据长度
			
* 输出参数  : 无
*
* 返 回 值    : 
			   0:  	写入成功
			  -1:		写入失败
* 其它说明  : 
*
*****************************************************************************/
unsigned int ptable_dload_write_fsimg(unsigned int data_buf, unsigned int image_idx, unsigned int this_offset , unsigned int len)
{
	char* partition_name=NULL;
	char mtd_dev_name[32];
	int fd;
	int i,ret;
	unsigned int page_size=0;
	struct mtd_write_req write_req;
	memset(&write_req,0,sizeof(write_req));
	write_req.mode=MTD_OPS_AUTO_OOB;
	write_req.ooblen=OOB_SIZE;
	
	off_t pos=this_offset;
	
	if (!mtd_partitions_scanned) {
                mtd_scan_partitions();
                mtd_partitions_scanned = 1;
          }

	/*获取分区表名称*/
	partition_name=ptable_find_name_by_type(image_idx);
	if(NULL==partition_name){
		Dbug("Error:can't find partition name!\n");
		return ERROR;
	}

	 /*根据分区名称获取分区*/
	const MtdPartition* mtd=mtd_find_partition_by_name(partition_name);
	if (mtd == NULL) {
                Dbug("mtd partition \"%s\" not found for writing\n", partition_name);
                return ERROR;
         }


	/*打开对应的mtd设备*/
	Dbug("mtd->device_index:%d\n",mtd->device_index);
	sprintf(mtd_dev_name,"/dev/mtd/mtd%d",mtd->device_index);
	fd=open(mtd_dev_name,O_RDWR);
	if(fd<0){
		return ERROR;
	}

	/*获取到nand flash的页大小*/
	page_size=get_nand_page_size(fd);
	if(page_size<0){
		Dbug("Error:nand page size is wrong!\n");
		printf("Error:nand page size is wrong!\n");
		return ERROR;
	}

	/*以每次写一页的方式向flash中写入文件系统的镜像*/
	i=0;
	while(0<(this_offset+len-pos-i*OOB_SIZE)){
		write_req.usr_oob=NULL;
		write_req.usr_data=(char *)data_buf+i*(page_size+OOB_SIZE);
		write_req.len=page_size;
		write_req.start=pos;
		
		/*写入数据*/
		ret=ioctl(fd, MEMWRITE, &write_req);
		if ( ret < 0) {
			Dbug("system image:write data error no is%d \n",ret);
			return ERROR;
		}

		/*写入OOB*/
		write_req.usr_oob=(char *)data_buf+i*(page_size+OOB_SIZE)+page_size;
		write_req.usr_data=NULL;
		write_req.len=OOB_SIZE;
		write_req.start=pos;
		ret=ioctl(fd, MEMWRITE, &write_req) ;
		if (ret< 0) {
			Dbug("system image:write oob error no is %d \n",ret);
			return ERROR;
		}
		pos+=page_size;
		i++;
	}

	return OK;
	
	
}


/*****************************************************************************
* 函 数 名    : ptable_dload_write
*
* 功能描述  : 非文件系统镜像烧写
*
* 输入参数  : 
			data_buf  :	需要烧写的镜像的地址
			image_idx:	镜像类型
			this_offset:	本次烧写的内容在buf地址中的偏移
			len           :	要烧写的数据长度
			
* 输出参数  : 无
*
* 返 回 值    : 
			   0:  	写入成功
			  -1:		写入失败
* 其它说明  : 
*
*****************************************************************************/

unsigned int ptable_dload_write(unsigned int data_buf, unsigned int image_idx, unsigned int this_offset , unsigned int len)
{
	char* name=NULL;
	char older_recovery;
	
	if (!mtd_partitions_scanned) {
                mtd_scan_partitions();
                mtd_partitions_scanned = 1;
          }

	/*根据镜像的id查找名称*/
	name=ptable_find_name_by_type(image_idx);

	/*查找对应分区*/
	const MtdPartition* mtd=mtd_find_partition_by_name(name);
	if (mtd == NULL) {
                Dbug("mtd partition \"%s\" not found for writing\n", name);
                goto error;
         }

	/*以write方式打开分区*/
	 MtdWriteContext* ctx = mtd_write_partition(mtd);
	 if (ctx == NULL) {
             Dbug("failed to init mtd partition \"%s\" for writing\n", name);
                goto error;
          }
	if (lseek(ctx->fd, this_offset, 0) != this_offset ) {
                goto error;
     	}
	/*向分区中写入数据*/
	size_t written = mtd_write_data(ctx, (char*)data_buf, len);
	if (written != len) {
		Dbug("only wrote %d of %d bytes to MTD %s\n",written, len, name);
                mtd_write_close(ctx);
                goto error;
	}
	if (mtd_erase_blocks(ctx, -1) < 0) {
                Dbug("error finishing mtd write of %s\n", name);
                mtd_write_close(ctx);
		goto error;
	}

	if (mtd_write_close(ctx)) {
                Dbug("error closing mtd write of %s\n", name);
                goto error;
	}
	return OK;
error:
	
	return ERROR;
}
//end for 函数打桩


/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SetCdromMarker
*
* 功能描述  : 设置设备形态标志
*
* 输入参数  : BSP_BOOL bCdromMarker  :
*             BSP_TRUE  :设置正常设备形态
*             BSP_FALSE :设置bootrom设备形态
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 当升级后台文件时，设置为上报正常设备形态，不恢复NV；
*             当升级前台文件时，设置为上报bootrom设备形态，恢复NV；
*
*****************************************************************************/
void dload_SetCdromMarker(bool bCdromMarker)
{
	/*TODO:*/
	//(void)setCdromFlag(bCdromMarker);//暂时打桩
}

/*****************************************************************************
* 函 数 名    : dload_align_buffer_write
*
* 功能描述  :  将接收到的镜像按文件系统和非文件系统进行区分，按各自方式写入flash
*
* 输入参数  : 无
* 输出参数  : 
*
* 返 回 值    : 
			  0:成功写入
			-1:写入失败
*
* 其它说明  : 
*
*****************************************************************************/

int dload_align_buffer_write(struct dload_buf_ctrl *write_buf)
{

	int ret=(int)ERROR;
	int data_type;

	if(( dcb->image_type > IMAGE_PART_START ) && ( dcb->image_type < IMAGE_PART_TOP ))
	{
        	/*超出地址范围*/        
        	#if defined (DLOAD_DEBUG_FLASH)
        	Dbug("dloadAlignBufferWrite:addr=0x%x,len=0x%x\n",
        	dcb->flash_current_offset, write_buf->used);
        	#endif
			
		data_type=get_image_property(dcb->image_type);
		if(data_type<0)
		{
			Dbug("Error:get image property fail!\n");
			return ERROR;
		}
		/*非文件系统的镜像*/
		if(DATA_YAFFS!=(data_type&PTABLE_IMAGE_TYPE_MASK))
		{
			ret = ptable_dload_write((unsigned int)write_buf->data,
	                            dcb->image_type,
	                            dcb->flash_current_offset,
	                            write_buf->used);
			Dbug("\ng_stDloadWriteBuffer.data:0x%x\nimage type:0x%x\ncurrent_loaded_length:%d;\ng_stDloadWriteBuffer.used:0x%x\n",write_buf->data,dcb->image_type,dcb->flash_current_offset,write_buf->used);
			printf("\ng_stDloadWriteBuffer.data:0x%x\nimage type:0x%x\ncurrent_loaded_length:%d;\ng_stDloadWriteBuffer.used:0x%x\n",write_buf->data,dcb->image_type,dcb->flash_current_offset,write_buf->used);

			dcb->flash_current_offset += write_buf->used;
		}
		else
		{/*文件系统的镜像*/
			ret = ptable_dload_write_fsimg((unsigned int)write_buf->data,
	                            dcb->image_type,
	                            dcb->flash_current_offset,
	                            write_buf->used);
			Dbug("\ng_stDloadWriteBuffer.data:0x%x\nimage type:0x%x\ncurrent_loaded_length:%d;\ng_stDloadWriteBuffer.used:0x%x\n",write_buf->data,dcb->image_type,dcb->flash_current_offset,write_buf->used);
			dcb->flash_current_offset+=write_buf->used-(DLOAD_NAND_SPARE_ALIGN_ADD-DLOAD_NAND_SPARE_ALIGN);
		}
        	return ret;   
    	}
    	else
    	{
    		Dbug("Error:image id is invalid!\n");
        	return ERROR; 
    	}

    return ret;
}

/*****************************************************************************
* 函 数 名    : image_dload_to_flash
*
* 功能描述  :  将接收到的数据放至向flash中写入的数据buf中，并调用写入函数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值    : 
			    0:操作成功
			  -1:操作失败
* 
* 其它说明  : 
*
*****************************************************************************/
int image_dload_to_flash(struct dload_buf_ctrl **write_buf,unsigned int align_size)
{
	unsigned int ulIndex = 0;    /*标记包中已填入g_pDloadWriteBuffer的数据偏移量*/      
	unsigned int remainLen = 0;  /*标记当前数据包剩余字节数*/
	int ret;
	/*入口参数检查*/
	if( NULL == (*write_buf)->data)
	{
		Dbug("ImageDloadToFlash: NULL == g_stDloadWriteBuffer data error.\n");
		return ERROR;
	}

	/*当前包的包长大于g_pDloadWriteBuffer剩余的可用空间*/
    	if( dcb->current_loaded_length >= (align_size - (*write_buf)->used) )
    	{
        	/*将g_pDloadWriteBuffer剩余的可用空间填满*/
        	ulIndex = align_size - (*write_buf)->used;
        
        	memcpy((unsigned char*)((unsigned char*)(*write_buf)->data + (*write_buf)->used),  (unsigned char*)dcb->ram_addr, ulIndex);
        	(*write_buf)->used = align_size;
		ret=dload_align_buffer_write(*write_buf);
        	if( OK != ret)      /*将g_pDloadWriteBuffer中数据写入flash*/
        	{
            		Dbug("ImageDloadToFlash: dloadAlignBufferWrite error.1\n");
            		return ERROR;
        	}
        
        	(*write_buf)->used = 0;    /*将g_stDloadWriteBuffer清空*/

        	/* 如果还有数据要缓存,继续写入缓冲区 */
        	remainLen = dcb->current_loaded_length - ulIndex;
        	if (0 != remainLen)
        	{
            		memcpy((unsigned char *)((*write_buf)->data + (*write_buf)->used), (unsigned char *)((unsigned char *)dcb->ram_addr + ulIndex),  remainLen);
           		 (*write_buf)->used += remainLen; 
       		 }
	}
	else
   	{
    	}

    	/*将剩余包数据填入g_pDloadWriteBuffer*/
    	memcpy((unsigned char*)((unsigned char*)(*write_buf)->data + (*write_buf)->used), 
                  (unsigned char *)((unsigned char*)dcb->ram_addr + ulIndex), dcb->current_loaded_length - ulIndex);
    	(*write_buf)->used += dcb->current_loaded_length - ulIndex;

    	dcb->last_loaded_total_length += dcb->current_loaded_length;

    	#if defined (DLOAD_DEBUG_FLASH)
	Dbug("unsec:F b/e/c:0x%x,0x%x,0x%x,0x%x\n",dcb->flash_current_offset,
    	dcb->current_loaded_length,dcb->last_loaded_total_length,dcb->last_loaded_packet);
    	#endif

    	/*当前包为部件最后一个升级包*/
    	if( dcb->image_size == dcb->last_loaded_total_length )    
    	{
    		ret=dload_align_buffer_write(*write_buf);
        	if(OK != ret)
        	{
        		Dbug("the last dload packge fail,ret is %d",ret);
            		return ERROR;
        	}        

        	/* 在部件最后一个升级包升级完成后，记录当前记录的升级部件类型,用于DATA_END时
        	  * 判断是否为CDROMISO/WEBUI/RECOVERY升级
        	 */
        	dcb->finish_image_type= dcb->image_type;		

        	/*本映像正常烧写完毕，将全局变量复位*/
        	Dbug("sec: the image 0x%x dload success!\n", dcb->image_type);

		/*清空g_stDloadWriteBuffer*/ 
        	(*write_buf)->used = 0;    
   	 }    

	return OK;

}

/*****************************************************************************
* 函 数 名    : image_dload_process
*
* 功能描述  :  升级包的烧写
*
* 输入参数  : 无
* 输出参数  : 
*
* 返 回 值    : 
			  0:操作成功
			-1:操作失败
*
* 其它说明  : 
*
*****************************************************************************/
int image_dload_process(struct dload_buf_ctrl *write_buf,unsigned int align_size)
{
	/*入口参数检查*/
	if((NULL == dcb) || (dcb->current_loaded_length > align_size) || (0 == dcb->current_loaded_length) )
	{
		Dbug("Error: g_pDCBStr->current_loaded_length=0x%x align_size=0x%x.\n", (int)dcb->current_loaded_length,align_size);
		return (int)ERROR;
    	}    

	/*当前包为部件第一个升级包，获取当前部件烧写的目的地址*/
	if( 0 == dcb->last_loaded_total_length )
	{
		Dbug("imageDloadProcess: g_pDCBStr->image_type = 0x%x.\n",dcb->image_type);
        
        	/* 数据写入分区地址置0 */
        	dcb->flash_current_offset = 0;

		if( 0 != write_buf->used )
        	{
			Dbug("imageDloadProcess: g_stDloadWriteBuffer.used=0x%x error.\n", (int)write_buf->used);
			return (int)ERROR;
        	}
    	}

	/*向flash中写入升级包*/
	if(OK != image_dload_to_flash(&write_buf,align_size))
        {
        	Dbug("dload image to flash failed!\n");
		return (int)ERROR;
        }

	return OK;

}

/*****************************************************************************
* 函 数 名  : dload_buf_init
*
* 功能描述  : 分配下载缓冲区空间
*
* 输入参数  : 
			type: 镜像id
			len  : 分配的buf长度
* 输出参数  : 
*
* 返 回 值  : 
*			  0:操作成功
			-1:操作失败

* 其它说明  : 
*
*****************************************************************************/
int dload_buf_init(enum IMAGE_TYPE type, unsigned int len,struct dload_buf_ctrl *write_buf)
{
	
	 /*释放需要申请的指针*/
	if(write_buf->data)
	{
		free(write_buf->data);    
		write_buf->data = NULL;
	}

	if( NULL == write_buf->data)
	{        
		if( NULL == (write_buf->data = (unsigned char *)malloc((unsigned int)len)) )
                {
                    Dbug("dloadInit: g_stDloadWriteBuffer data OSAL_Malloc error.\n");
                    return (unsigned int) ERROR;
                }
            }

            write_buf->size   = len;
            write_buf->used   = 0;
                
            memset((void*)write_buf->data, 0x0, len);
           
	return OK;
}

/*****************************************************************************
* 函 数 名    : dloadNeedMidwayRestart
*
* 功能描述  : TODO
*
* 输入参数  : TODO
			
* 输出参数  : TODO
*
* 返 回 值    : 

* 其它说明  : 
*
*****************************************************************************/

int dloadNeedMidwayRestart(void)
{

	//TODO:
	return 0;
}

/******************************************************************************
*  Function:  dloadGetNvImeiNumber
*  Description:
*      获取IMEI
*
*  Calls:
*
*  Called By:
*
*  Data Accessed: 
*
*  Data Updated: 
*
*  Input:
*        aucIMEI: 存放IMEI
*
*  Output:
*         NONE
*
*  Return:
*         0: 函数成功返回
*         -1: 函数失败
*
********************************************************************************/
int dloadGetNvImeiNumber(unsigned char aucIMEI[])
{
    /*TODO: */
    return OK;
}

/*****************************************************************************
 Function   :dloadIsDefaultImeiValue
 Description: 判断是否是默认IMEI
 Input      : 
 Return     :
 History    :
*****************************************************************************/
int dloadIsDefaultImeiValue(unsigned char aucIMEI[])
{
    unsigned char  aucDefaultIMEI[IMEI_LEN]  = {0x0};
    unsigned char  ucIndex;


    for(ucIndex = 0; ucIndex < IMEI_LEN; ucIndex++)
    {
        if (aucIMEI[ucIndex] != aucDefaultIMEI[ucIndex])
        {
            return ERROR;
        }
    }

    return OK;

}


/******************************************************************************
*  Function:  getAuthorityId
*  Description:
*      获取下载鉴权协议Id
*
*  Calls:
*
*  Called By:
*
*  Data Accessed: 
*
*  Data Updated: 
*
*  Input:
*        buf: 存放下载鉴权协议Id的缓存
*        len: 缓存长度
*
*  Output:
*         NONE
*
*  Return:
*         0: 函数成功返回
*         1: 函数失败
*
********************************************************************************/
int get_authorityId(unsigned char buf[], int len)
{
	unsigned char  aucBuffer[IMEI_LEN] = {0x0};
	unsigned char  ucIndex;

	Dbug("getAuthorityId\n");

	if((NULL == buf) || (len <= 0) || (len < (IMEI_LEN + 3)))
	{
		Dbug("param error!\n");
	        return ERROR;
	}

	memset(buf, 0x0, (unsigned int)len);

	if (ERROR == dloadGetNvImeiNumber(aucBuffer))
	{
	        return ERROR;
	}

	 /* 假如IMEI号为默认NV项,则表示此NV项未激活*/
	if (OK == dloadIsDefaultImeiValue(aucBuffer))
	{
	        return ERROR;
	}    

	for(ucIndex = 0; ucIndex < (IMEI_LEN -1); ucIndex++)//实际IMEI号为15位
	{
	        if(((unsigned char)aucBuffer[ucIndex] <= 0x09))
	        {
	            buf[ucIndex] = aucBuffer[ucIndex] + '0';
	        }
	        else
	        {
	            return ERROR;
	        }
	}

	buf[ucIndex] = ',';
	buf[ucIndex + 1] = '0';   

	return OK;
}

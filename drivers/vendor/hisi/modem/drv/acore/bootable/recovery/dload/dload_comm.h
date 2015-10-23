/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  dload_comm.h
*
*   作    者 :  wuzechun
*
*   描    述 :  dload_lib.c 的头文件
*
*   修改记录 :  2012年12月26日  v1.00  wuzechun  创建
*
*************************************************************************/
#include "at.h"
#include "dload.h"
#include "mtdutils/mtdutils.h"

#define u32  unsigned int
#define DLOAD_NAND_SPARE_ALIGN      (0x200000) 
#define DLOAD_NAND_SPARE_ALIGN_ADD (0x202000)


struct MtdPartition {
    int device_index;
    unsigned int size;
    unsigned int erase_size;
    char *name;
};

struct MtdWriteContext {
    const MtdPartition *partition;
    char *buffer;
    size_t stored;
    int fd;

    off_t* bad_block_offsets;
    int bad_block_alloc;
    int bad_block_count;
};

int dload_align_buffer_write(struct dload_buf_ctrl *write_buf);

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
int image_dload_to_flash(struct dload_buf_ctrl **write_buf,unsigned int align_size);
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
int image_dload_process(struct dload_buf_ctrl *write_buf,unsigned int align_size);

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
int dload_buf_init(enum IMAGE_TYPE type, unsigned int len,struct dload_buf_ctrl *write_buf);

int dloadNeedMidwayRestart(void);
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

int get_authorityId(unsigned char buf[], int len);

/*
*************************************************************

*	升级镜像时，修改分区表中对应分区的count 的值 	*

*************************************************************
*/
int update_image_count(enum IMAGE_TYPE image_idx);

/*
*************************************************************

*	升级ptable镜像时，修改分区表中的recovery 分区的count 的值 	*

*************************************************************
*/
int update_ptable_count(void);



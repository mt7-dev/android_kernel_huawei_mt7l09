/*************************************************************************
*   版权所有(C) 1987-2013, 深圳华为技术有限公司.
*
*   文 件 名 :  Drvinterface.h
*
*   作    者 :  wangxiandong
*
*   描    述 :  本文件命名为"Drvinterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2013年1月21日  v1.00  wangxiandong 创建
*************************************************************************/
#ifndef __BSP_CIPHER_H__
#define __BSP_CIPHER_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "osl_types.h"
#include "drv_cipher.h"


/*****************************************************************************
* 函 数 名  : bsp_acc_dma
*
* 功能描述  : 为指定通道配置描述符，用于单纯的DMA搬移
*
* 输入参数  : bd_fifo_addr: BDFIFO首地址
*             in_mem_mgr    : 输入buffer首地址
*             out_mem_mgr   : 输出buffer首地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_dma(u32 bd_fifo_addr, const void* in_mem_mgr, const void* out_mem_mgr, ACC_SINGLE_CFG_S *cfg);

/*****************************************************************************
* 函 数 名  : bsp_acc_cipher
*
* 功能描述  : 为指定通道配置描述符，用于需要Cipher进行加密的操作
*
* 输入参数  : bd_fifo_addr: BDFIFO首地址
*             in_mem_mgr    : 输入buffer首地址
*             out_mem_mgr   : 输出buffer首地址
*             cfg       : Cipher配置信息
* 输出参数   : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_cipher(u32 bd_fifo_addr, const void* in_mem_mgr, const void* out_mem_mgr,
               ACC_SINGLE_CFG_S *cfg);

/**************************************************************************
 函数声明
**************************************************************************/

/*****************************************************************************
* 函 数 名  : bsp_cipher_regist_notify_func
*
* 功能描述  : 注册通知回调函数
*
* 输入参数  : fun_notify_func: 通知回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*****************************************************************************/
s32 bsp_cipher_regist_notify_func(CIPHER_NOTIFY_CB_T fun_notify_func);

/*****************************************************************************
* 函 数 名  : bsp_cipher_regist_freemem_func
*
* 功能描述  : 注册Memory释放回调函数
*
* 输入参数  : chx: 通道号
*             b_src:   0:输入Buffer / 1:输出Bufffer
*             func_freemem_func: Memory释放回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
s32 bsp_cipher_regist_freemem_func(u32 chx, s32 b_src, CIPHER_FREEMEM_CB_T func_freemem_func);

/*****************************************************************************
* 函 数 名  : bsp_cipher_purge
*
* 功能描述  : 清除所有当前指定通道的CIPHER任务(阻塞接口,完成purge后返回)
*
* 输入参数  : chx: 要清除的通道号
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_TIME_OUT
*****************************************************************************/
s32 bsp_cipher_purge(u32 chx);

/*****************************************************************************
* 函 数 名  : bsp_cipher_single_submit_task
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于单独操作。
*
* 输入参数  : chx: 通道号
*             in_mem_mgr: 输入buffer内存管理结构指针
*             out_mem_mgr:输出buffer内存管理结构指针
*             cfg: Cipher操作配置属性
*             subm_attr:提交的属性
*             Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
s32 bsp_cipher_single_submit_task(u32 chx, const void* in_mem_mgr, const void* out_mem_mgr,
                   CIHPER_SINGLE_CFG_S *cfg, CIPHER_SUBMIT_TYPE_E subm_attr, u32 Private);

/*****************************************************************************
* 函 数 名  : bsp_cipher_rela_submit_task
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于关联操作。
*
* 输入参数  : chx: 通道号
*             in_mem_mgr: 输入buffer内存管理结构指针
*             out_mem_mgr:输出buffer内存管理结构指针
*             p_cfg: Cipher操作配置属性
*             subm_attr:提交的属性
*             Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
s32 bsp_cipher_rela_submit_task(u32 chx, const void* in_mem_mgr, const void* out_mem_mgr,
                   CIHPER_RELA_CFG_S *p_cfg, CIPHER_SUBMIT_TYPE_E subm_attr, u32 Private);

/*****************************************************************************
* 函 数 名  : bsp_cipher_setkey
*
* 功能描述  : 设置密钥值
*
* 输入参数  : key_addr   : 密钥地址
*             key_len   : 密钥长度
*             key_index: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_ENUM
*           :      CIPHER_INVALID_KEY
*****************************************************************************/
s32 bsp_cipher_setkey(const void* key_addr,CIPHER_KEY_LEN_E key_len,u32 key_index);

/*****************************************************************************
* 函 数 名  : bsp_cipher_getkey
*
* 功能描述  : 根据key_index获取Key的地址
*
* 输入参数  : key_index: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:根据KeyIndex得到的Key的地址
*           : 失败:获取Key值所在地址失败,返回0
*****************************************************************************/
s32 bsp_cipher_getkey(u32 key_index, KEY_GET_S *key_get);

/*****************************************************************************
* 函 数 名  : bsp_cipher_set_data_buf_para
*
* 功能描述  : 获取上层数据信息并设置信息
*
* 输入参数  : chx        : 通道号
*             b_src          : TRUE:源地址的属性配置 / FALSE:目的地址属性配置
*             buf_oft     : 数据buffer指针偏移
*             len_oft     : Buffer长度偏移
*             next_oft       : 下一节点偏移
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
s32 bsp_cipher_set_data_buf_para(u32 chx, s32 b_src, u32 buf_oft, u32 len_oft, u32 next_oft);

/*****************************************************************************
* 函 数 名  : bsp_cipher_get_cmpl_src_buff
*
* 功能描述  : 获取已经操作完成的源数据地址
*
* 输入参数  : chx     : 通道号
* 输出参数  : src_addr : 操作完成的源地址;
*             dest_addr : 操作完成的目的地址
*             status    : 操作完成的状态
*             Private  : 返回的私有数据
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_RD
*****************************************************************************/
s32 bsp_cipher_get_cmpl_src_buff(u32 chx, u32 *src_addr,u32 *dest_addr, CIPHER_NOTIFY_STAT_E *status, u32 *Private);

/*****************************************************************************
* 函 数 名  : bsp_acc_get_bdfifo_addr
*
* 功能描述  : 用于获取当前可用的BDFIFO首地址
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : BDFIFO首地址
*****************************************************************************/
u32 bsp_acc_get_bdfifo_addr(void);

/*****************************************************************************
* 函 数 名  : bsp_acc_get_status
*
* 功能描述  : 获取当前通道状态
*
* 输入参数  :  无
* 输出参数   : 无
* 返 回 值  : 通道忙/空闲
*****************************************************************************/
s32 bsp_acc_get_status(void);

/*****************************************************************************
* 函 数 名  : bsp_acc_enable
*
* 功能描述  : 使能组包加速
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 bsp_acc_enable(u32 bd_fifo_addr);

/*****************************************************************************
* 函 数 名  : bsp_kdf_key_ram_read()
*
* 功能描述  : 从Key Ram中读取数据
*
* 输入参数  : key_index: Key索引，从该索引处开始读取数据
*             read_length : 读取数据长度，以Byte为单位
* 输出参数  : dest_addr : 目的地址，将从KeyRam中读取的数据存储到该位置
* 返 回 值  : 成功: BSP_OK
*           : 失败: BSP_ERROR
*****************************************************************************/
s32 bsp_kdf_key_ram_read(void *dest_addr, u32 key_index ,u32 read_length);

/*****************************************************************************
* 函 数 名  : bsp_kdf_key_ram_write
*
* 功能描述  : 向KeyRam中写入数据
*
* 输入参数  : key_index: Key索引，从该索引处开始写入数据
*             src_addr : 源地址，写入到KeyRam中的数据地址
*             length : 数据长度
*
* 输出参数  : 无
* 返 回 值  : 成功: BSP_OK
*           : 失败: BSP_ERROR
*****************************************************************************/
s32 bsp_kdf_key_ram_write(u32 key_index, void *src_addr, u32 length);

/*****************************************************************************
* 函 数 名  : bsp_kdf_key_make
*
* 功能描述  : KDF计算，用于产生新的Key
*
* 输入参数  : key_cfg_info: KDF运算时参数sha_key属性配置
            : s_cfg_info: KDF运算时参数sha_s属性配置
            : dest_index : KDF运算产生的Key放置在KeyRam中的位置
* 输出参数  : 无
* 返 回 值  : 成功:BSP_OK
*           : 失败:BSP_ERROR
*****************************************************************************/
s32 bsp_kdf_key_make(KEY_CONFIG_INFO_S *key_cfg_info, S_CONFIG_INFO_S *s_cfg_info,
                        u32 dest_index, KEY_MAKE_S *key_make);

int cipher_close_clk(void);
int cipher_open_clk(void);

/*************************CIPHER END **********************************/

/*************************KDF HASH START **********************************/
/*****************************************************************************
* 函 数 名  : kdf_init
*
* 功能描述  : A核KDF队列空间申请初始化，用于计算HASH值，在C核启动之前使用
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0    : 正确
*             其他 : 错误
*****************************************************************************/
int kdf_init(void);

int kdf_hash_init(void);
int kdfdev_init(void);
void kdfdev_exit(void);
/*****************************************************************************
* 函 数 名  : kdf_reg_init
*
* 功能描述  : A核KDF寄存器配置，用于计算HASH值，在C核启动之前使用
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
void kdf_reg_init(void);
/*****************************************************************************
* 函 数 名  : kdf_hash_setkey
*
* 功能描述  : A核KDF操作设置key，用于计算HASH值，在C核启动之前使用
*
* 输入参数  : 传入key的首地址，默认key的长度固定为32字节
* 输出参数  : 无
* 返 回 值  : 0    : 正确
*             其他 : 错误
*****************************************************************************/
int kdf_hash_setkey(void * key_addr);
/*****************************************************************************
* 函 数 名  : kdf_result_hash_get
*
* 功能描述  : A核KDF操作获取结果HASH，在C核启动之前使用
*
* 输入参数  : 无
* 输出参数  : out_hash : 用于接收结果HASH的内存空间首地址，默认至少32字节
* 返 回 值  : 0    : 正确
*             其他 : 错误
*****************************************************************************/
int kdf_result_hash_get(void * out_hash);
/*****************************************************************************
* 函 数 名  : kdf_hash_make
*
* 功能描述  : A核KDF操作计算HASH，在C核启动之前使用
*
* 输入参数  : sha_s_addr : 传入数据的首地址，数据最大长度不能超过512字节
*             sha_s_len  : 传入数据的实际长度，以字节为单位
* 输出参数  : 无
* 返 回 值  : 0    : 正确
*             其他 : 错误
*****************************************************************************/
int kdf_hash_make(void * sha_s_addr, u32 sha_s_len);
/*************************KDF HASH END **********************************/

#endif /* end of __BSP_CIPHER_H__ */



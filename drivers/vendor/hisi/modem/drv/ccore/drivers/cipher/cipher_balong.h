
#ifndef	_CIPHER_BALONG_H
#define	_CIPHER_BALONG_H


#include "bsp_cipher.h"
#include "cipher_balong_common.h"
#include "kdf_balong.h"
#include "bsp_dpm.h"
#include "bsp_om.h"
#include <osl_spinlock.h>

/*=======================================宏定义================================================*/
#define CIPHER_MAX_CHN          0x4 /*通道1、2、3，0通道空置*/
#define CIPHER_DRB_CHN          0x3
#define CIPHER_MIN_CHN          0x1 /*从通道1开始*/

/* 输入buffer list节点个数最大保护值 ，每个BD允许拥有的最大CD数*/
#define CIPHER_CHECK_CDLIST_CNT   512
/*通道2和3优先级自动切换的流阈值*/
#define CIPHER_CHN_STRMTHRESHHLD  4096
/*通道RD完成中断判断*/
#define CIPHER_INTR_CHN_COMPL(chn, status) ((0x8U << ((chn) << 3)) & (status))
/* Buffer List 操作封装 主要用于1/2/3通道管理结构体 */
#define CIPHER_BUFLIST_GET_INBUF_PTR(p_node, chn) \
    ((u8*) (*((u32*) (((u32)(p_node)) + sg_module_mgr.chx_mgr[chn].in_buff_addr_oft))))

#define CIPHER_BUFLIST_GET_INBUF_LEN(p_node, chn) \
    ((u32) (*((u32*) (((u32)(p_node)) + sg_module_mgr.chx_mgr[chn].in_buff_len_oft))))

#define CIPHER_BUFLIST_GET_INBUFNEXT_PTR(p_node,chn) \
    ((u8*) (*((u32*) (((u32)(p_node)) + sg_module_mgr.chx_mgr[chn].in_buff_next_oft))))

#define CIPHER_BUFLIST_GET_OUTBUF_PTR(p_node, chn) \
    ((u8*) (*((u32*) (((u32)(p_node)) + sg_module_mgr.chx_mgr[chn].out_buff_addr_oft))))

#define CIPHER_BUFLIST_GET_OUTBUF_LEN(p_node, chn) \
    ((u32) (*((u32*) (((u32)(p_node)) + sg_module_mgr.chx_mgr[chn].out_buff_len_oft))))

#define CIPHER_BUFLIST_GET_OUTBUFNEXT_PTR(p_node,chn) \
    ((u8*) (*((u32*) (((u32)(p_node)) + sg_module_mgr.chx_mgr[chn].out_buff_next_oft))))
/*配置CD描述符*/
#define CIPHER_CFG_BUF_LIST(node, buf_ptr, buf_len, b_last) \
    do{\
        (node).pt_attr = (                           \
             ((u32)(buf_len) & 0xFFFF) |            \
             (((u32)(b_last) & 0x1)) << 0x10);      \
        (node).pt = (u32)(buf_ptr);                   \
    }while(0)

#define CIPHER_ERROR_PRINT(errno) \
do{\
    {\
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_CIPHER, "[CIPHER API ERROR]:\n\t\t\t <func>: %s;  <line>: %d;  <Errno>: %s (0x%08x)\n\n", \
               (int)__FUNCTION__, (int)__LINE__, (int)#errno, errno);\
    }\
}while(0)


/*=================结构体定义=============*/
/* CIPHER BD描述符结构 */
struct cipher_bd_s
{
	u32 cipher_cfg;           /*Cipher config*/
	u32 input_attr;           /*Input Attr*/
	u32 output_attr;          /*Output Attr*/
	u32 input_addr;           /*Input Addr*/
	u32 output_addr;          /*Output Addr*/
	u32 aph_attr;             /*Aph for IV*/
	u32 count;                /*count for IV*/
	u32 usr_field1;           /*Usr Field1*/
	u32 usr_field2;           /*Usr Field2*/
	u32 usr_field3;           /*Usr Field3*/
};
/* CIPHER RD描述符结构*/
struct cipher_rd_s
{
	u32 input_attr;           /*Input Attr*/
	u32 output_attr;          /*Output Attr*/
	u32 input_addr;           /*Input Addr*/
	u32 output_addr;          /*Output Addr*/
	u32 usr_field1;           /*Usr Field1*/
	u32 usr_field2;           /*Usr Field2*/
	u32 usr_field3;           /*Usr Field3*/
	u32 cipher_cfg;           /*Cipher Config*/
};
/*CD描述符结构*/
struct cipher_cd_s
{
	u32 pt;                  /*指针*/
	u32 pt_attr;             /*指针属性*/
};
/* 通道1/2/3管理全局结构体 */
struct cipher_chx_mgr_s
{
	u32	chx_bd_number;						  /*当前通道中BD和RD的个数[BD、RD个数相同]*/
	u32 cd_number_per_bd;				      /*RD中CD个数与BD相同*/

	struct cipher_bd_s *p_bd_base;            /* 当前通道BD描述符基地址 */
	struct cipher_rd_s *p_rd_base;            /* 当前通道RD描述符基地址 */
	struct cipher_cd_s *p_cdlist_base;        /* 当前通道CD List基地址 */

    CIPHER_FREEMEM_CB_T p_func_free_inmem;    /* 输入Buffer释放Callback函数*/
    CIPHER_FREEMEM_CB_T p_func_free_outmem;   /* 输出Buffer释放Callback函数*/

    u32 in_buff_addr_oft;                     /* 输入Buff地址在TTF结构中的偏移*/
    u32 in_buff_len_oft;                      /* 输入Buff长度在TTF结构中的偏移*/
    u32 in_buff_next_oft;                     /* 下一个TTF在TTF结构中的偏移*/
    u32 out_buff_addr_oft;                    /* 输出Buff地址在TTF结构中的偏移*/
    u32 out_buff_len_oft;                     /* 输出Buff长度在TTF结构中的偏移*/
    u32 out_buff_next_oft;                    /* 下一个TTF在TTF结构中的偏移*/

    s32 b_purging;                            /*是否正在清空通道标志*/
    CIPHER_MUTEX_T mtx_chn_opt;               /* Cipher操作的互斥量 */

	u32 cd_list_start_addr;                   /*本通道CD链表的第一个可用CD地址*/
	u32 cd_list_end_addr;                     /*本通道CD List的最后一个可用CD地址*/
};
/* CIPHER模块管理全局结构体 */
struct cipher_mdl_mgr_s
{
    u32 reg_base_addr;                                /* 模块寄存器基地址 */ 
    enum CIPHER_STATUS_E status;                      /* 初始化状态 */ 
    CIPHER_NOTIFY_CB_T p_func_notify;                 /* 用户注册的CallBack函数 */
    struct cipher_chx_mgr_s chx_mgr[CIPHER_MAX_CHN];  /* 通道管理结构 */
	struct kdf_chx_mgr_s    kdf_chx_mgr;              /* KDF通道管理结构 */
	
    CIPHER_KEY_LEN_E key_type_len[CIPHER_KEY_NUM];    /* 各个Key的长度*/
    s32 debug_level;                                  /* 调试级别设置*/
	
	u32 bd_full_count[CIPHER_MAX_CHN];	              /* BD FIFO空间满统计计数*/
	u32 single_count[CIPHER_MAX_CHN];	              /* 各通道调用单独操作的次数*/
	u32 rela_count[CIPHER_MAX_CHN];	                  /* 各通道调用关联操作的次数*/
	u32 invalid_rd_number[CIPHER_MAX_CHN];	          /* 无效RD发生的次数*/

	u32 time_out_bd1[CIPHER_MAX_CHN];	              /* BD处理超时发生的次数,读写指针不等*/
	u32 time_out_bd2[CIPHER_MAX_CHN];	              /* BD处理超时发生的次数，操作没有完成*/
	u32 time_out_kdf1;                                /* KDF通道处理超时发生的次数,读写指针不等*/
	u32 time_out_kdf2;                                /* KDF通道处理超时发生的次数，操作没有完成*/

	u32 key_length_table[CIPHER_KEY_LEN_BUTTOM];      /* {0x10, 0x18, 0x20}*/

	u32 sgl_key_len[CIPHER_KEY_LEN_BUTTOM];           /* {0x2, 0x3, 0x4}*/
	u8 security_op[CIPHER_SINGLE_OPT_BUTTOM];         /* {1,1,0,0,0,0,0,0};*/
	u8 integrity_op[CIPHER_SINGLE_OPT_BUTTOM];        /* {0,0,1,1,1,1,1,1};*/
	/* 单独操作*/
	u8 single_dirt_tbl[CIPHER_SINGLE_OPT_BUTTOM];     /* {0,1,0,0,1,1,1,1};*/
	u8 sig_op_security_tbl[CIPHER_SINGLE_OPT_BUTTOM]; /* {1,1,0,0,0,0,0,0};*/
	u8 sig_op_integrity_tbl[CIPHER_SINGLE_OPT_BUTTOM];/* {0,0,1,1,1,1,1,1};*/
	u8 mac_position_single[CIPHER_SINGLE_OPT_BUTTOM]; /* {0,0,0,0,1,0,1,0};*/
	u8 mac_length_single[CIPHER_SINGLE_OPT_BUTTOM];   /* {0,0,1,0,1,1,0,0};*/
	/* 关联操作*/
	u8 rela_dirt_tbl[CIPHER_RELA_OPT_BUTTOM];         /* {0,1,0,1};*/
	u8 rela_op_security_tbl[CIPHER_RELA_OPT_BUTTOM];  /* {1,1,1,1};*/
	u8 rela_op_integrity_tbl[CIPHER_RELA_OPT_BUTTOM]; /* {1,1,1,1};*/
	u8 rela_op_first[CIPHER_RELA_OPT_BUTTOM];         /* {1,0,0,1};*/
	u8 mac_position_rela[CIPHER_RELA_OPT_BUTTOM];     /* {0,0,1,1};*/
	/* 公用*/
	u8 inte_alg[CIPHER_ALG_BUTTOM];                   /* {0,1,2,3,4,5};*/
	u8 secu_alg[CIPHER_ALG_BUTTOM];                   /* {0,1,2,3,4,5};*/
	u8 int_enable_tbl[CIPHER_SUBM_BUTTOM];            /* {0,0,1};*/
	u8 herder_len_tbl[CIPHER_HDR_BIT_TYPE_BUTT];      /* {0,1,1,2,1,1,2};*/
	u8 aph_len_tbl[CIPHER_HDR_BIT_TYPE_BUTT];         /* {0,1,1,2,1,1,2};*/
	u8 aph_attr_tbl[CIPHER_HDR_BIT_TYPE_BUTT];        /* {0,1,1,1,0,0,0};*/
	u32 aph_mast_tbl[CIPHER_HDR_BIT_TYPE_BUTT];       /* {0x0,0x1f,0x7f,0xfff,0x1f,0x7f,0xfff};*/
	struct clk * cifer_clk;                           /* cipher时钟 */

	u32 key_ram_bak_buf[128];                         /*用于低功耗时保存key ram信息*/
	u32 suspend_enable;								  /* 指示是否需要 suspend/resume cipher*/
	s32 enabled;                                      /* 1:enable函数被调用过;0:未被调用 */
	struct spinlock spin_lock_irq;
	struct spinlock spin_lock_reg_conf;
};

/*=======================函数声明=======================*/
void cipher_set_debug_level(s32 level);
static void cipher_isr();
static void cipher_chn_mag_reset(u32 chx);
int cipher_open_clk(void);
int cipher_close_clk(void);
int cipher_reg_set(void);

static s32 cipher_init_kdf_chx_bdq_mem(u32 * org_kdf_bdq_addr);
static s32 cipher_init_kdf_chx_rdq_mem(u32 * org_kdf_rdq_addr);
s32 bsp_cipher_suspend(struct dpm_device *dev);
s32 bsp_cipher_resume(struct dpm_device *dev);

#endif

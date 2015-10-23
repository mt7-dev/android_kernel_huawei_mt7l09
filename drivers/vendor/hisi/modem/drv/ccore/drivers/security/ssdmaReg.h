

#ifndef _SSDMA_PRI_H_
#define _SSDMA_PRI_H_

#ifdef __cplusplus
extern "C" {
#endif


/***************************** ssdma base address *****************************/
#define SSDMA_BASE_REG  0x900b4000

/***************************** sha1 register *****************************/
#define SHA_BASE_REG    (SSDMA_BASE_REG+0x80) /* �ֲ�Ļ���ַ��4 */

#define SHA_DIN_0       (SHA_BASE_REG+0x0)
#define SHA_DIN_1       (SHA_BASE_REG+0x4)
#define SHA_DIN_2       (SHA_BASE_REG+0x8)
#define SHA_DIN_3       (SHA_BASE_REG+0xc)
#define SHA_DIN_4       (SHA_BASE_REG+0x10)
#define SHA_DIN_5       (SHA_BASE_REG+0x14)
#define SHA_DIN_6       (SHA_BASE_REG+0x18)
#define SHA_DIN_7       (SHA_BASE_REG+0x1c)
#define SHA_DIN_8       (SHA_BASE_REG+0x20)
#define SHA_DIN_9       (SHA_BASE_REG+0x24)
#define SHA_DIN_10      (SHA_BASE_REG+0x28)
#define SHA_DIN_11      (SHA_BASE_REG+0x2c)
#define SHA_DIN_12      (SHA_BASE_REG+0x30)
#define SHA_DIN_13      (SHA_BASE_REG+0x34)
#define SHA_DIN_14      (SHA_BASE_REG+0x38)
#define SHA_DIN_15      (SHA_BASE_REG+0x3c)

#define SHA_IVIN_0      (SHA_BASE_REG+0x40)
#define SHA_IVIN_1      (SHA_BASE_REG+0x44)
#define SHA_IVIN_2      (SHA_BASE_REG+0x48)
#define SHA_IVIN_3      (SHA_BASE_REG+0x4c)
#define SHA_IVIN_4      (SHA_BASE_REG+0x50)

#define SHA_IVOUT_0     (SHA_BASE_REG+0x54)
#define SHA_IVOUT_1     (SHA_BASE_REG+0x58)
#define SHA_IVOUT_2     (SHA_BASE_REG+0x5c)
#define SHA_IVOUT_3     (SHA_BASE_REG+0x60)
#define SHA_IVOUT_4     (SHA_BASE_REG+0x64)

#define SHA_BUSY        (SHA_BASE_REG+0x68)
#define SHA_BUSY_BITPOS     0
#define SHA_BUSY_BITWIDTH   1
#define SHA_BUSY_BITMASK    (((1<<SHA_BUSY_BITWIDTH)-1)<<SHA_BUSY_BITPOS)
#define SHA_IS_BUSY         (0x1<<SHA_BUSY_BITPOS)

#define SHA_START       (SHA_BASE_REG+0x6c)
#define SHA_AUTO_IVUPDATE_BITPOS    1
#define SHA_AUTO_IVUPDATE_BITWIDTH  1
#define SHA_AUTO_IVUPDATE_BITMASK   (((1<<SHA_AUTO_IVUPDATE_BITWIDTH)-1)<<SHA_AUTO_IVUPDATE_BITPOS)
#define SHA_IS_AUTO_IVUPDATE        (0x0<<SHA_AUTO_IVUPDATE_BITPOS)
#define SHA_IS_NOT_IVUPDATE         (0x1<<SHA_AUTO_IVUPDATE_BITPOS)

#define SHA_START_BITPOS            0
#define SHA_START_BITWIDTH          1
#define SHA_START_BITMASK           (((1<<SHA_START_BITWIDTH)-1)<<SHA_START_BITPOS)
#define SHA_IS_START                (0x1<<SHA_START_BITPOS)

/***************************** rsa register *****************************/
#define RSA_BASE_REG           (SSDMA_BASE_REG+0x200)

#define RSA_OPERAND_E_0        (RSA_BASE_REG+0x0)
#define RSA_OPERAND_E_1        (RSA_BASE_REG+0x4)
#define RSA_OPERAND_E_2        (RSA_BASE_REG+0x8)
#define RSA_OPERAND_E_3        (RSA_BASE_REG+0xc)
#define RSA_OPERAND_E_4        (RSA_BASE_REG+0x10)
#define RSA_OPERAND_E_5        (RSA_BASE_REG+0x14)
#define RSA_OPERAND_E_6        (RSA_BASE_REG+0x18)
#define RSA_OPERAND_E_7        (RSA_BASE_REG+0x1c)
#define RSA_OPERAND_E_8        (RSA_BASE_REG+0x20)
#define RSA_OPERAND_E_9        (RSA_BASE_REG+0x24)
#define RSA_OPERAND_E_10       (RSA_BASE_REG+0x28)
#define RSA_OPERAND_E_11       (RSA_BASE_REG+0x2c)
#define RSA_OPERAND_E_12       (RSA_BASE_REG+0x30)
#define RSA_OPERAND_E_13       (RSA_BASE_REG+0x34)
#define RSA_OPERAND_E_14       (RSA_BASE_REG+0x38)
#define RSA_OPERAND_E_15       (RSA_BASE_REG+0x3c)

#define RSA_OPERAND_N_0        (RSA_BASE_REG+0x40)
#define RSA_OPERAND_N_1        (RSA_BASE_REG+0x44)
#define RSA_OPERAND_N_2        (RSA_BASE_REG+0x48)
#define RSA_OPERAND_N_3        (RSA_BASE_REG+0x4c)
#define RSA_OPERAND_N_4        (RSA_BASE_REG+0x50)
#define RSA_OPERAND_N_5        (RSA_BASE_REG+0x54)
#define RSA_OPERAND_N_6        (RSA_BASE_REG+0x58)
#define RSA_OPERAND_N_7        (RSA_BASE_REG+0x5c)
#define RSA_OPERAND_N_8        (RSA_BASE_REG+0x60)
#define RSA_OPERAND_N_9        (RSA_BASE_REG+0x64)
#define RSA_OPERAND_N_10       (RSA_BASE_REG+0x68)
#define RSA_OPERAND_N_11       (RSA_BASE_REG+0x6c)
#define RSA_OPERAND_N_12       (RSA_BASE_REG+0x70)
#define RSA_OPERAND_N_13       (RSA_BASE_REG+0x74)
#define RSA_OPERAND_N_14       (RSA_BASE_REG+0x78)
#define RSA_OPERAND_N_15       (RSA_BASE_REG+0x7c)

#define RSA_OPERAND_C_0        (RSA_BASE_REG+0x80)
#define RSA_OPERAND_C_1        (RSA_BASE_REG+0x84)
#define RSA_OPERAND_C_2        (RSA_BASE_REG+0x88)
#define RSA_OPERAND_C_3        (RSA_BASE_REG+0x8c)
#define RSA_OPERAND_C_4        (RSA_BASE_REG+0x90)
#define RSA_OPERAND_C_5        (RSA_BASE_REG+0x94)
#define RSA_OPERAND_C_6        (RSA_BASE_REG+0x98)
#define RSA_OPERAND_C_7        (RSA_BASE_REG+0x9c)
#define RSA_OPERAND_C_8        (RSA_BASE_REG+0xa0)
#define RSA_OPERAND_C_9        (RSA_BASE_REG+0xa4)
#define RSA_OPERAND_C_10       (RSA_BASE_REG+0xa8)
#define RSA_OPERAND_C_11       (RSA_BASE_REG+0xac)
#define RSA_OPERAND_C_12       (RSA_BASE_REG+0xb0)
#define RSA_OPERAND_C_13       (RSA_BASE_REG+0xb4)
#define RSA_OPERAND_C_14       (RSA_BASE_REG+0xb8)
#define RSA_OPERAND_C_15       (RSA_BASE_REG+0xbc)
  
#define RSA_OPERAND_M_0        (RSA_BASE_REG+0xC0)
#define RSA_OPERAND_M_1        (RSA_BASE_REG+0xC4)
#define RSA_OPERAND_M_2        (RSA_BASE_REG+0xC8)
#define RSA_OPERAND_M_3        (RSA_BASE_REG+0xCC)
#define RSA_OPERAND_M_4        (RSA_BASE_REG+0xD0)
#define RSA_OPERAND_M_5        (RSA_BASE_REG+0xD4)
#define RSA_OPERAND_M_6        (RSA_BASE_REG+0xD8)
#define RSA_OPERAND_M_7        (RSA_BASE_REG+0xDC)
#define RSA_OPERAND_M_8        (RSA_BASE_REG+0xE0)
#define RSA_OPERAND_M_9        (RSA_BASE_REG+0xE4)
#define RSA_OPERAND_M_10       (RSA_BASE_REG+0xE8)
#define RSA_OPERAND_M_11       (RSA_BASE_REG+0xEC)
#define RSA_OPERAND_M_12       (RSA_BASE_REG+0xF0)
#define RSA_OPERAND_M_13       (RSA_BASE_REG+0xF4)
#define RSA_OPERAND_M_14       (RSA_BASE_REG+0xF8)
#define RSA_OPERAND_M_15       (RSA_BASE_REG+0xFC)

#define RSA_RESULT_R_0         (RSA_BASE_REG+0x100)
#define RSA_RESULT_R_1         (RSA_BASE_REG+0x104)
#define RSA_RESULT_R_2         (RSA_BASE_REG+0x108)
#define RSA_RESULT_R_3         (RSA_BASE_REG+0x10C)
#define RSA_RESULT_R_4         (RSA_BASE_REG+0x110)
#define RSA_RESULT_R_5         (RSA_BASE_REG+0x114)
#define RSA_RESULT_R_6         (RSA_BASE_REG+0x118)
#define RSA_RESULT_R_7         (RSA_BASE_REG+0x11C)
#define RSA_RESULT_R_8         (RSA_BASE_REG+0x120)
#define RSA_RESULT_R_9         (RSA_BASE_REG+0x124)
#define RSA_RESULT_R_10        (RSA_BASE_REG+0x128)
#define RSA_RESULT_R_11        (RSA_BASE_REG+0x12C)
#define RSA_RESULT_R_12        (RSA_BASE_REG+0x130)
#define RSA_RESULT_R_13        (RSA_BASE_REG+0x134)
#define RSA_RESULT_R_14        (RSA_BASE_REG+0x138)
#define RSA_RESULT_R_15        (RSA_BASE_REG+0x13C)

#define RSA_N_LENGTH           (RSA_BASE_REG+0x140)
#define RSA_N_LENGTH_BITPOS     0
#define RSA_N_LENGTH_BITWIDTH   5
#define RSA_N_LENGTH_BITMASK    (((1<<RSA_N_LENGTH_BITWIDTH)-1)<<RSA_N_LENGTH_BITPOS)

#define RSA_E_LENGTH           (RSA_BASE_REG+0x144)
#define RSA_E_LENGTH_BITPOS     0
#define RSA_E_LENGTH_BITWIDTH   4
#define RSA_E_LENGTH_BITMASK    (((1<<RSA_E_LENGTH_BITWIDTH)-1)<<RSA_E_LENGTH_BITPOS)

#define RSA_MULT_EXP_MODULE    (RSA_BASE_REG+0x148)
#define RSA_MULT_EXP_MOD_BITPOS     0
#define RSA_MULT_EXP_MOD_BITWIDTH   1
#define RSA_MULT_EXP_MOD_BITMASK    (((1<<RSA_MULT_EXP_MOD_BITWIDTH)-1)<<RSA_MULT_EXP_MOD_BITPOS)
#define RSA_IS_MULT_MOD             (0x1<<RSA_MULT_EXP_MOD_BITPOS)
#define RSA_IS_EXP_MOD              (0x0<<RSA_MULT_EXP_MOD_BITPOS)

#define RSA_FORCE_END          (RSA_BASE_REG+0x14C)
#define RSA_FORCE_END_BITPOS        0
#define RSA_FORCE_END_BITWIDTH      1
#define RSA_FORCE_END_BITMASK       (((1<<RSA_FORCE_END_BITWIDTH)-1)<<RSA_FORCE_END_BITPOS)
#define RSA_IS_FORCE_END            (0x1<<RSA_FORCE_END_BITPOS)

#define RSA_BUSY               (RSA_BASE_REG+0x150)
#define RSA_BUSY_BITPOS         0
#define RSA_BUSY_BITWIDTH       1
#define RSA_BUSY_BITMASK        (((1<<RSA_BUSY_BITWIDTH)-1)<<RSA_BUSY_BITPOS)
#define RSA_IS_BUSY             (0x1<<RSA_BUSY_BITPOS)

#define RSA_ERROR              (RSA_BASE_REG+0x154)
#define RSA_ERROR_BITPOS        0
#define RSA_ERROR_BITWIDTH      1
#define RSA_ERROR_BITMASK       (((1<<RSA_ERROR_BITWIDTH)-1)<<RSA_ERROR_BITPOS)
#define RSA_IS_ERROR            (0x1<<RSA_ERROR_BITPOS)

#define RSA_START              (RSA_BASE_REG+0x158)
#define RSA_START_BITPOS        0
#define RSA_START_BITWIDTH      1
#define RSA_START_BITMASK       (((1<<RSA_START_BITWIDTH)-1)<<RSA_START_BITPOS)
#define RSA_IS_START            (0x1<<RSA_START_BITPOS)

/***************************** mailbox register *****************************/
#define MBOX_BASE_REG           (SSDMA_BASE_REG+0x2000)

#define MBOX_DSP_LOCK           (MBOX_BASE_REG+0x0)
    #define MBOX_DSP_LOCK_BITPOS        0
    #define MBOX_DSP_LOCK_BITWIDTH      1
    #define MBOX_DSP_LOCK_BITMASK       (((1<<MBOX_DSP_LOCK_BITWIDTH)-1)<<MBOX_DSP_LOCK_BITPOS)
    #define MBOX_IS_DSP_LOCK            (0x1<<MBOX_DSP_LOCK_BITPOS)

#define MBOX_ARM_LOCK           (MBOX_BASE_REG+0x4)
    #define MBOX_ARM_LOCK_BITPOS        0
    #define MBOX_ARM_LOCK_BITWIDTH      1
    #define MBOX_ARM_LOCK_BITMASK       (((1<<MBOX_ARM_LOCK_BITWIDTH)-1)<<MBOX_ARM_LOCK_BITPOS)
    #define MBOX_IS_ARM_LOCK            (0x1<<MBOX_ARM_LOCK_BITPOS)

#define MBOX_DSP_DEBUGEN        (MBOX_BASE_REG+0x8)
    #define MBOX_DSP_DEBUGEN_BITPOS     0
    #define MBOX_DSP_DEBUGEN_BITWIDTH   1
    #define MBOX_DSP_DEBUGEN_BITMASK    (((1<<MBOX_DSP_DEBUGEN_BITWIDTH)-1)<<MBOX_DSP_DEBUGEN_BITPOS)
    #define MBOX_IS_DSP_DEBUGEN         (0x1<<MBOX_DSP_DEBUGEN_BITPOS)

#define MBOX_APPARM_DEBUGEN     (MBOX_BASE_REG+0xC)
    #define MBOX_APPARM_DEBUGEN_BITPOS      0
    #define MBOX_APPARM_DEBUGEN_BITWIDTH    1
    #define MBOX_APPARM_DEBUGEN_BITMASK     (((1<<MBOX_APPARM_DEBUGEN_BITWIDTH)-1)<<MBOX_APPARM_DEBUGEN_BITPOS)
    #define MBOX_IS_APPARM_DEBUGEN          (0x1<<MBOX_APPARM_DEBUGEN_BITPOS)
    
#define MBOX_COMARM_DEBUGEN     (MBOX_BASE_REG+0x10)
    #define MBOX_COMARM_DEBUGEN_BITPOS      0
    #define MBOX_COMARM_DEBUGEN_BITWIDTH    1
    #define MBOX_COMARM_DEBUGEN_BITMASK     (((1<<MBOX_COMARM_DEBUGEN_BITWIDTH)-1)<<MBOX_COMARM_DEBUGEN_BITPOS)
    #define MBOX_IS_COMARM_DEBUGEN          (0x1<<MBOX_COMARM_DEBUGEN_BITPOS)

#define MBOX_CLK_GATEN          (MBOX_BASE_REG+0x14)
    #define MBOX_AES_CLKEN_BITPOS       0
    #define MBOX_AES_CLKEN_BITWIDTH     1
    #define MBOX_AES_CLKEN_BITMASK      (((1<<MBOX_AES_CLKEN_BITWIDTH)-1)<<MBOX_AES_CLKEN_BITPOS)
    #define MBOX_IS_AES_CLKEN           (0x1<<MBOX_AES_CLKEN_BITPOS)

    #define MBOX_DES_CLKEN_BITPOS       1
    #define MBOX_DES_CLKEN_BITWIDTH     1
    #define MBOX_DES_CLKEN_BITMASK      (((1<<MBOX_DES_CLKEN_BITWIDTH)-1)<<MBOX_DES_CLKEN_BITPOS)
    #define MBOX_IS_DES_CLKEN           (0x1<<MBOX_DES_CLKEN_BITPOS)

    #define MBOX_SHA_CLKEN_BITPOS       2
    #define MBOX_SHA_CLKEN_BITWIDTH     1
    #define MBOX_SHA_CLKEN_BITMASK      (((1<<MBOX_SHA_CLKEN_BITWIDTH)-1)<<MBOX_SHA_CLKEN_BITPOS)
    #define MBOX_IS_SHA_CLKEN           (0x1<<MBOX_SHA_CLKEN_BITPOS)

    #define MBOX_MD5_CLKEN_BITPOS       3
    #define MBOX_MD5_CLKEN_BITWIDTH     1
    #define MBOX_MD5_CLKEN_BITMASK      (((1<<MBOX_MD5_CLKEN_BITWIDTH)-1)<<MBOX_MD5_CLKEN_BITPOS)
    #define MBOX_IS_MD5_CLKEN           (0x1<<MBOX_MD5_CLKEN_BITPOS)

    #define MBOX_RSA_CLKEN_BITPOS       4
    #define MBOX_RSA_CLKEN_BITWIDTH     1
    #define MBOX_RSA_CLKEN_BITMASK      (((1<<MBOX_RSA_CLKEN_BITWIDTH)-1)<<MBOX_RSA_CLKEN_BITPOS)
    #define MBOX_IS_RSA_CLKEN           (0x1<<MBOX_RSA_CLKEN_BITPOS)

    #define MBOX_TRNG_CLKEN_BITPOS      5
    #define MBOX_TRNG_CLKEN_BITWIDTH    1
    #define MBOX_TRNG_CLKEN_BITMASK     (((1<<MBOX_TRNG_CLKEN_BITWIDTH)-1)<<MBOX_TRNG_CLKEN_BITPOS)
    #define MBOX_IS_TRNG_CLKEN          (0x1<<MBOX_TRNG_CLKEN_BITPOS)

#define MBOX_INT_MASK           (MBOX_BASE_REG+0x18)
    #define MBOX_INTMASK_MBOX_BITPOS    0
    #define MBOX_INTMASK_MBOX_BITWIDTH  1
    #define MBOX_INTMASK_MBOX_BITMASK   (((1<<MBOX_INTMASK_MBOX_BITWIDTH)-1)<<MBOX_INTMASK_MBOX_BITPOS)
    #define MBOX_IS_INTMASK_MBOX        (0x1<<MBOX_INTMASK_MBOX_BITPOS)

    #define MBOX_INTMASK_DMA_BITPOS     1
    #define MBOX_INTMASK_DMA_BITWIDTH   1
    #define MBOX_INTMASK_DMA_BITMASK    (((1<<MBOX_INTMASK_DMA_BITWIDTH)-1)<<MBOX_INTMASK_DMA_BITPOS)
    #define MBOX_IS_INTMASK_DMA         (0x1<<MBOX_INTMASK_DMA_BITPOS)

    #define MBOX_INTMASK_AES_BITPOS     2
    #define MBOX_INTMASK_AES_BITWIDTH   1
    #define MBOX_INTMASK_AES_BITMASK    (((1<<MBOX_INTMASK_AES_BITWIDTH)-1)<<MBOX_INTMASK_AES_BITPOS)
    #define MBOX_IS_INTMASK_AES     (0x1<<MBOX_INTMASK_AES_BITPOS)

    #define MBOX_INTMASK_DES_BITPOS     3
    #define MBOX_INTMASK_DES_BITWIDTH   1
    #define MBOX_INTMASK_DES_BITMASK    (((1<<MBOX_INTMASK_DES_BITWIDTH)-1)<<MBOX_INTMASK_DES_BITPOS)
    #define MBOX_IS_INTMASK_DES         (0x1<<MBOX_INTMASK_DES_BITPOS)

    #define MBOX_INTMASK_SHA_BITPOS     4
    #define MBOX_INTMASK_SHA_BITWIDTH   1
    #define MBOX_INTMASK_SHA_BITMASK    (((1<<MBOX_INTMASK_SHA_BITWIDTH)-1)<<MBOX_INTMASK_SHA_BITPOS)
    #define MBOX_IS_INTMASK_SHA         (0x1<<MBOX_INTMASK_SHA_BITPOS)

    #define MBOX_INTMASK_MD5_BITPOS     5
    #define MBOX_INTMASK_MD5_BITWIDTH   1
    #define MBOX_INTMASK_MD5_BITMASK    (((1<<MBOX_INTMASK_MD5_BITWIDTH)-1)<<MBOX_INTMASK_MD5_BITPOS)
    #define MBOX_IS_INTMASK_MD5         (0x1<<MBOX_INTMASK_MD5_BITPOS)

    #define MBOX_INTMASK_RSA_BITPOS     6
    #define MBOX_INTMASK_RSA_BITWIDTH   1
    #define MBOX_INTMASK_RSA_BITMASK    (((1<<MBOX_INTMASK_RSA_BITWIDTH)-1)<<MBOX_INTMASK_RSA_BITPOS)
    #define MBOX_IS_INTMASK_RSA         (0x1<<MBOX_INTMASK_RSA_BITPOS)

    #define MBOX_INTMASK_TRNG_BITPOS    7
    #define MBOX_INTMASK_TRNG_BITWIDTH  1
    #define MBOX_INTMASK_TRNG_BITMASK   (((1<<MBOX_INTMASK_TRNG_BITWIDTH)-1)<<MBOX_INTMASK_TRNG_BITPOS)
    #define MBOX_IS_INTMASK_TRNG        (0x1<<MBOX_INTMASK_TRNG_BITPOS)

    #define MBOX_INTMASK_EXP_BITPOS     8
    #define MBOX_INTMASK_EXP_BITWIDTH   1
    #define MBOX_INTMASK_EXP_BITMASK    (((1<<MBOX_INTMASK_EXP_BITWIDTH)-1)<<MBOX_INTMASK_EXP_BITPOS)
    #define MBOX_IS_INTMASK_EXP         (0x1<<MBOX_INTMASK_EXP_BITPOS)

#define MBOX_INT_STATE          (MBOX_BASE_REG+0x1C)
    #define MBOX_INTSTATE_MBOX_BITPOS   0
    #define MBOX_INTSTATE_MBOX_BITWIDTH 1
    #define MBOX_INTSTATE_MBOX_BITMASK  (((1<<MBOX_INTSTATE_MBOX_BITWIDTH)-1)<<MBOX_INTSTATE_MBOX_BITPOS)
    #define MBOX_IS_INTSTATE_MBOX       (0x1<<MBOX_INTSTATE_MBOX_BITPOS)

    #define MBOX_INTSTATE_DMA_BITPOS    1
    #define MBOX_INTSTATE_DMA_BITWIDTH  1
    #define MBOX_INTSTATE_DMA_BITMASK   (((1<<MBOX_INTSTATE_DMA_BITWIDTH)-1)<<MBOX_INTSTATE_DMA_BITPOS)
    #define MBOX_IS_INTSTATE_DMA        (0x1<<MBOX_INTSTATE_DMA_BITPOS)

    #define MBOX_INTSTATE_AES_BITPOS    2
    #define MBOX_INTSTATE_AES_BITWIDTH  1
    #define MBOX_INTSTATE_AES_BITMASK   (((1<<MBOX_INTSTATE_AES_BITWIDTH)-1)<<MBOX_INTSTATE_AES_BITPOS)
    #define MBOX_IS_INTSTATE_AES        (0x1<<MBOX_INTSTATE_AES_BITPOS)

    #define MBOX_INTSTATE_DES_BITPOS    3
    #define MBOX_INTSTATE_DES_BITWIDTH  1
    #define MBOX_INTSTATE_DES_BITMASK   (((1<<MBOX_INTSTATE_DES_BITWIDTH)-1)<<MBOX_INTSTATE_DES_BITPOS)
    #define MBOX_IS_INTSTATE_DES        (0x1<<MBOX_INTSTATE_DES_BITPOS)

    #define MBOX_INTSTATE_SHA_BITPOS    4
    #define MBOX_INTSTATE_SHA_BITWIDTH  1
    #define MBOX_INTSTATE_SHA_BITMASK   (((1<<MBOX_INTSTATE_SHA_BITWIDTH)-1)<<MBOX_INTSTATE_SHA_BITPOS)
    #define MBOX_IS_INTSTATE_SHA        (0x1<<MBOX_INTSTATE_SHA_BITPOS)

    #define MBOX_INTSTATE_MD5_BITPOS    5
    #define MBOX_INTSTATE_MD5_BITWIDTH  1
    #define MBOX_INTSTATE_MD5_BITMASK   (((1<<MBOX_INTSTATE_MD5_BITWIDTH)-1)<<MBOX_INTSTATE_MD5_BITPOS)
    #define MBOX_IS_INTSTATE_MD5        (0x1<<MBOX_INTSTATE_MD5_BITPOS)

    #define MBOX_INTSTATE_RSA_BITPOS    6
    #define MBOX_INTSTATE_RSA_BITWIDTH  1
    #define MBOX_INTSTATE_RSA_BITMASK   (((1<<MBOX_INTSTATE_RSA_BITWIDTH)-1)<<MBOX_INTSTATE_RSA_BITPOS)
    #define MBOX_IS_INTSTATE_RSA        (0x1<<MBOX_INTSTATE_RSA_BITPOS)

    #define MBOX_INTSTATE_TRNG_BITPOS   7
    #define MBOX_INTSTATE_TRNG_BITWIDTH 1
    #define MBOX_INTSTATE_TRNG_BITMASK  (((1<<MBOX_INTSTATE_TRNG_BITWIDTH)-1)<<MBOX_INTSTATE_TRNG_BITPOS)
    #define MBOX_IS_INTSTATE_TRNG       (0x1<<MBOX_INTSTATE_TRNG_BITPOS)

    #define MBOX_INTSTATE_EXP_BITPOS    8
    #define MBOX_INTSTATE_EXP_BITWIDTH  1
    #define MBOX_INTSTATE_EXP_BITMASK   (((1<<MBOX_INTSTATE_EXP_BITWIDTH)-1)<<MBOX_INTSTATE_EXP_BITPOS)
    #define MBOX_IS_INTSTATE_EXP        (0x1<<MBOX_INTSTATE_EXP_BITPOS)

#define MBOX_KEYGEN_MOD         (MBOX_BASE_REG+0x20)
    #define MBOX_KEYGEN_MOD_BITPOS      0
    #define MBOX_KEYGEN_MOD_BITWIDTH    2
    #define MBOX_KEYGEN_MOD_BITMASK     (((1<<MBOX_KEYGEN_MOD_BITWIDTH)-1)<<MBOX_KEYGEN_MOD_BITPOS)
    #define MBOX_IS_PKEY_MOD            (0x0<<MBOX_KEYGEN_MOD_BITPOS)
    #define MBOX_IS_EKEY_MOD            (0x2<<MBOX_KEYGEN_MOD_BITPOS)
    #define MBOX_IS_INTKEY_MOD          (0x3<<MBOX_KEYGEN_MOD_BITPOS)

#define MBOX_KEY_TARGET         (MBOX_BASE_REG+0x24)
    #define MBOX_KEY_TARGET_BITPOS      0
    #define MBOX_KEY_TARGET_BITWIDTH    3
    #define MBOX_KEY_TARGET_BITMASK     (((1<<MBOX_KEY_TARGET_BITWIDTH)-1)<<MBOX_KEY_TARGET_BITPOS)
    #define MBOX_IS_AES_KEY             (0x0<<MBOX_KEY_TARGET_BITPOS)
    #define MBOX_IS_DES_KEY1            (0x1<<MBOX_KEY_TARGET_BITPOS)
    #define MBOX_IS_DES_KEY2            (0x2<<MBOX_KEY_TARGET_BITPOS)
    #define MBOX_IS_DES_KEY3            (0x3<<MBOX_KEY_TARGET_BITPOS)
    #define MBOX_IS_RSA_EKEY            (0x4<<MBOX_KEY_TARGET_BITPOS)
    #define MBOX_IS_RSA_NKEY            (0x5<<MBOX_KEY_TARGET_BITPOS)
    #define MBOX_IS_INT_KEY             (0x6<<MBOX_KEY_TARGET_BITPOS)

#define MBOX_KEYGEN_CTRL        (MBOX_BASE_REG+0x28)
    #define MBOX_ROOTKEY_SEL_BITPOS     0
    #define MBOX_ROOTKEY_SEL_BITWIDTH   3
    #define MBOX_ROOTKEY_SEL_BITMASK    (((1<<MBOX_ROOTKEY_SEL_BITWIDTH)-1)<<MBOX_ROOTKEY_SEL_BITPOS)
    #define MBOX_IS_USE_ROOTKEY0        (0x0<<MBOX_ROOTKEY_SEL_BITPOS)
    #define MBOX_IS_USE_ROOTKEY1        (0x1<<MBOX_ROOTKEY_SEL_BITPOS)
    #define MBOX_IS_USE_ROOTKEY2        (0x2<<MBOX_ROOTKEY_SEL_BITPOS)
    #define MBOX_IS_USE_ROOTKEY3        (0x3<<MBOX_ROOTKEY_SEL_BITPOS)
    #define MBOX_IS_USE_ROOTKEY4        (0x4<<MBOX_ROOTKEY_SEL_BITPOS)
    #define MBOX_IS_USE_ROOTKEY5        (0x5<<MBOX_ROOTKEY_SEL_BITPOS)
    #define MBOX_IS_USE_ROOTKEY6        (0x6<<MBOX_ROOTKEY_SEL_BITPOS)
    #define MBOX_IS_USE_ROOTKEY7        (0x7<<MBOX_ROOTKEY_SEL_BITPOS)

    #define MBOX_KEYWR_EN_BITPOS        3
    #define MBOX_KEYWR_EN_BITWIDTH      1
    #define MBOX_KEYWR_EN_BITMASK       (((1<<MBOX_KEYWR_EN_BITWIDTH)-1)<<MBOX_KEYWR_EN_BITPOS)
    #define MBOX_IS_KEYWR_EN            (0x1<<MBOX_KEYWR_EN_BITPOS)

    #define MBOX_KEYRD_EN_BITPOS        4
    #define MBOX_KEYRD_EN_BITWIDTH      1
    #define MBOX_KEYRD_EN_BITMASK       (((1<<MBOX_KEYRD_EN_BITWIDTH)-1)<<MBOX_KEYRD_EN_BITPOS)
    #define MBOX_IS_KEYRD_EN            (0x1<<MBOX_KEYRD_EN_BITPOS)

    #define MBOX_INTKEYGEN_EN_BITPOS    5
    #define MBOX_INTKEYGEN_EN_BITWIDTH  1
    #define MBOX_INTKEYGEN_EN_BITMASK   (((1<<MBOX_INTKEYGEN_EN_BITWIDTH)-1)<<MBOX_INTKEYGEN_EN_BITPOS)
    #define MBOX_IS_INTKEYGEN_EN        (0x1<<MBOX_INTKEYGEN_EN_BITPOS)

    #define MBOX_INTKEY2IP_EN_BITPOS    6
    #define MBOX_INTKEY2IP_EN_BITWIDTH  1
    #define MBOX_INTKEY2IP_EN_BITMASK   (((1<<MBOX_INTKEY2IP_EN_BITWIDTH)-1)<<MBOX_INTKEY2IP_EN_BITPOS)
    #define MBOX_IS_INTKEY2IP_EN        (0x1<<MBOX_INTKEY2IP_EN_BITPOS)

    #define MBOX_KEYCTRL_END_BITPOS     7
    #define MBOX_KEYCTRL_END_BITWIDTH   1
    #define MBOX_KEYCTRL_END_BITMASK    (((1<<MBOX_KEYCTRL_END_BITWIDTH)-1)<<MBOX_KEYCTRL_END_BITPOS)
    #define MBOX_IS_KEYCTRL_END         (0x1<<MBOX_KEYCTRL_END_BITPOS)
    
#define MBOX_KEYGEN_STATE       (MBOX_BASE_REG+0x2C)
    #define MBOX_INTSTATE_KEYWR_BITPOS          0
    #define MBOX_INTSTATE_KEYWR_BITWIDTH        1
    #define MBOX_INTSTATE_KEYWR_BITMASK         (((1<<MBOX_INTSTATE_KEYWR_BITWIDTH)-1)<<MBOX_INTSTATE_KEYWR_BITPOS)
    #define MBOX_IS_INTSTATE_KEYWR              (0x1<<MBOX_INTSTATE_KEYWR_BITPOS)

    #define MBOX_INTSTATE_KEYRD_BITPOS          1
    #define MBOX_INTSTATE_KEYRD_BITWIDTH        1
    #define MBOX_INTSTATE_KEYRD_BITMASK         (((1<<MBOX_INTSTATE_KEYRD_BITWIDTH)-1)<<MBOX_INTSTATE_KEYRD_BITPOS)
    #define MBOX_IS_INTSTATE_KEYRD              (0x1<<MBOX_INTSTATE_KEYRD_BITPOS)

    #define MBOX_INTSTATE_INTKEYGEN_BITPOS      2
    #define MBOX_INTSTATE_INTKEYGEN_BITWIDTH    1
    #define MBOX_INTSTATE_INTKEYGEN_BITMASK     (((1<<MBOX_INTSTATE_INTKEYGEN_BITWIDTH)-1)<<MBOX_INTSTATE_INTKEYGEN_BITPOS)
    #define MBOX_IS_INTSTATE_INTKEYGEN          (0x1<<MBOX_INTSTATE_INTKEYGEN_BITPOS)

    #define MBOX_INTSTATE_INTKEY2IP_BITPOS      3
    #define MBOX_INTSTATE_INTKEY2IP_BITWIDTH    1
    #define MBOX_INTSTATE_INTKEY2IP_BITMASK     (((1<<MBOX_INTSTATE_INTKEY2IP_BITWIDTH)-1)<<MBOX_INTSTATE_INTKEY2IP_BITPOS)
    #define MBOX_IS_INTSTATE_INTKEY2IP          (0x1<<MBOX_INTSTATE_INTKEY2IP_BITPOS)

#define MBOX_KEYGEN_MASK        (MBOX_BASE_REG+0x30)
    #define MBOX_INTMASK_KEYWR_BITPOS           0
    #define MBOX_INTMASK_KEYWR_BITWIDTH         1
    #define MBOX_INTMASK_KEYWR_BITMASK          (((1<<MBOX_INTMASK_KEYWR_BITWIDTH)-1)<<MBOX_INTMASK_KEYWR_BITPOS)
    #define MBOX_IS_INTMASK_KEYWR               (0x1<<MBOX_INTMASK_KEYWR_BITPOS)

    #define MBOX_INTMASK_KEYRD_BITPOS           1
    #define MBOX_INTMASK_KEYRD_BITWIDTH         1
    #define MBOX_INTMASK_KEYRD_BITMASK          (((1<<MBOX_INTMASK_KEYRD_BITWIDTH)-1)<<MBOX_INTMASK_KEYRD_BITPOS)
    #define MBOX_IS_INTMASK_KEYRD               (0x1<<MBOX_INTMASK_KEYRD_BITPOS)

    #define MBOX_INTMASK_INTKEYGEN_BITPOS       2
    #define MBOX_INTMASK_INTKEYGEN_BITWIDTH     1
    #define MBOX_INTMASK_INTKEYGEN_BITMASK      (((1<<MBOX_INTMASK_INTKEYGEN_BITWIDTH)-1)<<MBOX_INTMASK_INTKEYGEN_BITPOS)
    #define MBOX_IS_INTMASK_INTKEYGEN           (0x1<<MBOX_INTMASK_INTKEYGEN_BITPOS)

    #define MBOX_INTMASK_INTKEY2IP_BITPOS       3
    #define MBOX_INTMASK_INTKEY2IP_BITWIDTH     1
    #define MBOX_INTMASK_INTKEY2IP_BITMASK      (((1<<MBOX_INTMASK_INTKEY2IP_BITWIDTH)-1)<<MBOX_INTMASK_INTKEY2IP_BITPOS)
    #define MBOX_IS_INTMASK_INTKEY2IP           (0x1<<MBOX_INTMASK_INTKEY2IP_BITPOS)

#define MBOX_KEYI_0             (MBOX_BASE_REG+0x34)
#define MBOX_KEYI_1             (MBOX_BASE_REG+0x38)
#define MBOX_KEYI_2             (MBOX_BASE_REG+0x3C)
#define MBOX_KEYI_3             (MBOX_BASE_REG+0x40)

#define MBOX_KEYO_0             (MBOX_BASE_REG+0x44)
#define MBOX_KEYO_1             (MBOX_BASE_REG+0x48)
#define MBOX_KEYO_2             (MBOX_BASE_REG+0x4C)
#define MBOX_KEYO_3             (MBOX_BASE_REG+0x50)

#define MBOX_IV_0               (MBOX_BASE_REG+0x54)
#define MBOX_IV_1               (MBOX_BASE_REG+0x58)
#define MBOX_IV_2               (MBOX_BASE_REG+0x5C)
#define MBOX_IV_3               (MBOX_BASE_REG+0x60)

#define MBOX_INT_CLEAR          (MBOX_BASE_REG+0x6C)
    #define MBOX_INTCLR_MBOX_BITPOS         0
    #define MBOX_INTCLR_MBOX_BITWIDTH       1
    #define MBOX_INTCLR_MBOX_BITMASK        (((1<<MBOX_INTCLR_MBOX_BITWIDTH)-1)<<MBOX_INTCLR_MBOX_BITPOS)
    #define MBOX_IS_INTCLR_MBOX             (0x1<<MBOX_INTCLR_MBOX_BITPOS)

    #define MBOX_INTCLR_DMA_BITPOS          1
    #define MBOX_INTCLR_DMA_BITWIDTH        1
    #define MBOX_INTCLR_DMA_BITMASK         (((1<<MBOX_INTCLR_DMA_BITWIDTH)-1)<<MBOX_INTCLR_DMA_BITPOS)
    #define MBOX_IS_INTCLR_DMA              (0x1<<MBOX_INTCLR_DMA_BITPOS)

    #define MBOX_INTCLR_AES_BITPOS          2
    #define MBOX_INTCLR_AES_BITWIDTH        1
    #define MBOX_INTCLR_AES_BITMASK         (((1<<MBOX_INTCLR_AES_BITWIDTH)-1)<<MBOX_INTCLR_AES_BITPOS)
    #define MBOX_IS_INTCLR_AES              (0x1<<MBOX_INTCLR_AES_BITPOS)

    #define MBOX_INTCLR_DES_BITPOS          3
    #define MBOX_INTCLR_DES_BITWIDTH        1
    #define MBOX_INTCLR_DES_BITMASK         (((1<<MBOX_INTCLR_DES_BITWIDTH)-1)<<MBOX_INTCLR_DES_BITPOS)
    #define MBOX_IS_INTCLR_DES              (0x1<<MBOX_INTCLR_DES_BITPOS)

    #define MBOX_INTCLR_SHA_BITPOS          4
    #define MBOX_INTCLR_SHA_BITWIDTH        1
    #define MBOX_INTCLR_SHA_BITMASK         (((1<<MBOX_INTCLR_SHA_BITWIDTH)-1)<<MBOX_INTCLR_SHA_BITPOS)
    #define MBOX_IS_INTCLR_SHA              (0x1<<MBOX_INTCLR_SHA_BITPOS)

    #define MBOX_INTCLR_MD5_BITPOS          5
    #define MBOX_INTCLR_MD5_BITWIDTH        1
    #define MBOX_INTCLR_MD5_BITMASK         (((1<<MBOX_INTCLR_MD5_BITWIDTH)-1)<<MBOX_INTCLR_MD5_BITPOS)
    #define MBOX_IS_INTCLR_MD5              (0x1<<MBOX_INTCLR_MD5_BITPOS)

    #define MBOX_INTCLR_RSA_BITPOS          6
    #define MBOX_INTCLR_RSA_BITWIDTH        1
    #define MBOX_INTCLR_RSA_BITMASK         (((1<<MBOX_INTCLR_RSA_BITWIDTH)-1)<<MBOX_INTCLR_RSA_BITPOS)
    #define MBOX_IS_INTCLR_RSA              (0x1<<MBOX_INTCLR_RSA_BITPOS)

    #define MBOX_INTCLR_TRNG_BITPOS         7
    #define MBOX_INTCLR_TRNG_BITWIDTH       1
    #define MBOX_INTCLR_TRNG_BITMASK        (((1<<MBOX_INTCLR_TRNG_BITWIDTH)-1)<<MBOX_INTCLR_TRNG_BITPOS)
    #define MBOX_IS_INTCLR_TRNG             (0x1<<MBOX_INTCLR_TRNG_BITPOS)

    #define MBOX_INTCLR_EXP_BITPOS          8
    #define MBOX_INTCLR_EXP_BITWIDTH        1
    #define MBOX_INTCLR_EXP_BITMASK         (((1<<MBOX_INTCLR_EXP_BITWIDTH)-1)<<MBOX_INTCLR_EXP_BITPOS)
    #define MBOX_IS_INTCLR_EXP              (0x1<<MBOX_INTCLR_EXP_BITPOS)

    #define MBOX_INTCLR_KEYWR_BITPOS        9
    #define MBOX_INTCLR_KEYWR_BITWIDTH      1
    #define MBOX_INTCLR_KEYWR_BITMASK       (((1<<MBOX_INTCLR_KEYWR_BITWIDTH)-1)<<MBOX_INTCLR_KEYWR_BITPOS)
    #define MBOX_IS_INTCLR_KEYWR            (0x1<<MBOX_INTCLR_KEYWR_BITPOS)

    #define MBOX_INTCLR_KEYRD_BITPOS        10
    #define MBOX_INTCLR_KEYRD_BITWIDTH      1
    #define MBOX_INTCLR_KEYRD_BITMASK       (((1<<MBOX_INTCLR_KEYRD_BITWIDTH)-1)<<MBOX_INTCLR_KEYRD_BITPOS)
    #define MBOX_IS_INTCLR_KEYRD            (0x1<<MBOX_INTCLR_KEYRD_BITPOS)

    #define MBOX_INTCLR_INTKEYGEN_BITPOS    11
    #define MBOX_INTCLR_INTKEYGEN_BITWIDTH  1
    #define MBOX_INTCLR_INTKEYGEN_BITMASK   (((1<<MBOX_INTCLR_INTKEYGEN_BITWIDTH)-1)<<MBOX_INTCLR_INTKEYGEN_BITPOS)
    #define MBOX_IS_INTCLR_INTKEYGEN        (0x1<<MBOX_INTCLR_INTKEYGEN_BITPOS)

    #define MBOX_INTCLR_INTKEY2IP_BITPOS    12
    #define MBOX_INTCLR_INTKEY2IP_BITWIDTH  1
    #define MBOX_INTCLR_INTKEY2IP_BITMASK   (((1<<MBOX_INTCLR_INTKEY2IP_BITWIDTH)-1)<<MBOX_INTCLR_INTKEY2IP_BITPOS)
    #define MBOX_IS_INTCLR_INTKEY2IP        (0x1<<MBOX_INTCLR_INTKEY2IP_BITPOS)

/***************************** dma register *****************************/
#define DMA_BASE_REG            (SSDMA_BASE_REG+0x3000)
                                                                     
#define DMA_SADDR               (DMA_BASE_REG+0x0)
#define DMA_DADDR               (DMA_BASE_REG+0x4)                          
#define DMA_TOTALLEN            (DMA_BASE_REG+0x8)                       
#define DMA_SEGLEN              (DMA_BASE_REG+0xC)
    #define DMA_SEGLENGTH_BITPOS        0
    #define DMA_SEGLENGTH_BITWIDTH      5
    #define DMA_SEGLENGTH_BITMASK       (((1<<DMA_SEGLENGTH_BITWIDTH)-1)<<DMA_SEGLENGTH_BITPOS)

#define DMA_CTRL                (DMA_BASE_REG+0x10)
    #define DMA_CBC_MOD_BITPOS      0
    #define DMA_CBC_MOD_BITWIDTH    1
    #define DMA_CBC_MOD_BITMASK     (((1<<DMA_CBC_MOD_BITWIDTH)-1)<<DMA_CBC_MOD_BITPOS)
    #define DMA_IS_EVERY_CBC        (0x0<<DMA_CBC_MOD_BITPOS)
    #define DMA_IS_TOTAL_CBC        (0x1<<DMA_CBC_MOD_BITPOS)

    #define DMA_AES_MOD_BITPOS      1
    #define DMA_AES_MOD_BITWIDTH    1
    #define DMA_AES_MOD_BITMASK     (((1<<DMA_AES_MOD_BITWIDTH)-1)<<DMA_AES_MOD_BITPOS)
    #define DMA_IS_AES_MOD          (0x1<<DMA_AES_MOD_BITPOS)

    #define DMA_DES_MOD_BITPOS      2
    #define DMA_DES_MOD_BITWIDTH    1
    #define DMA_DES_MOD_BITMASK     (((1<<DMA_DES_MOD_BITWIDTH)-1)<<DMA_DES_MOD_BITPOS)
    #define DMA_IS_DES_MOD          (0x1<<DMA_DES_MOD_BITPOS)

    #define DMA_SHA_MOD_BITPOS      3
    #define DMA_SHA_MOD_BITWIDTH    1
    #define DMA_SHA_MOD_BITMASK     (((1<<DMA_SHA_MOD_BITWIDTH)-1)<<DMA_SHA_MOD_BITPOS)
    #define DMA_IS_SHA_MOD          (0x1<<DMA_SHA_MOD_BITPOS)

    #define DMA_MD5_MOD_BITPOS      4
    #define DMA_MD5_MOD_BITWIDTH    1
    #define DMA_MD5_MOD_BITMASK     (((1<<DMA_MD5_MOD_BITWIDTH)-1)<<DMA_MD5_MOD_BITPOS)
    #define DMA_IS_MD5_MOD          (0x1<<DMA_MD5_MOD_BITPOS)

    #define DMA_RSA_MOD_BITPOS      5
    #define DMA_RSA_MOD_BITWIDTH    1
    #define DMA_RSA_MOD_BITMASK     (((1<<DMA_RSA_MOD_BITWIDTH)-1)<<DMA_RSA_MOD_BITPOS)
    #define DMA_IS_RSA_MOD          (0x1<<DMA_RSA_MOD_BITPOS)

#define DMA_INT                 (DMA_BASE_REG+0x14)
    #define DMA_INTSTATUS_TRANSFINISHED_BITPOS      0
    #define DMA_INTSTATUS_TRANSFINISHED_BITWIDTH    1
    #define DMA_INTSTATUS_TRANSFINISHED_BITMASK     (((1<<DMA_INTSTATUS_TRANSFINISHED_BITWIDTH)-1)<<DMA_INTSTATUS_TRANSFINISHED_BITPOS)
    #define DMA_IS_INTSTATUS_TRANSFINISHED          (0x1<<DMA_INTSTATUS_TRANSFINISHED_BITPOS)

    #define DMA_INTSTATUS_TRANSERR_BITPOS           1
    #define DMA_INTSTATUS_TRANSERR_BITWIDTH         1
    #define DMA_INTSTATUS_TRANSERR_BITMASK          (((1<<DMA_INTSTATUS_TRANSERR_BITWIDTH)-1)<<DMA_INTSTATUS_TRANSERR_BITPOS)
    #define DMA_IS_INTSTATUS_TRANSERR               (0x0<<DMA_INTSTATUS_TRANSERR_BITPOS)
                
#define DMA_START               (DMA_BASE_REG+0x18)
    #define DMA_START_BITPOS        0
    #define DMA_START_BITWIDTH      1
    #define DMA_START_BITMASK       (((1<<DMA_START_BITWIDTH)-1)<<DMA_START_BITPOS)
    #define DMA_IS_START            (0x1<<DMA_START_BITPOS)
	

#ifdef __cplusplus
}
#endif

#endif  /*_SSDMA_PRI_H_*/


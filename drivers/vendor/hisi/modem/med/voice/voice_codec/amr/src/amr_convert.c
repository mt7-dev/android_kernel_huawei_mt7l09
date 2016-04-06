

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_lib.h"
#include "codec_op_vec.h"

#include "amr_interface.h"
#include "amr_convert.h"
#include "ucom_comm.h"

#include "voice_pcm.h"

/*
#include "med_com_vec_op.h"
*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* AMR 4.75kbit/s 模式比特顺序对应关系，详见协议TS26.101，Annex B */
const VOS_UINT16 g_auhwAmr475BitOrder[95] =
{
    0,      1,       2,      3,      4,      5,      6,      7,     8,      9,
    10,     11,     12,     13,     14,     15,     23,     24,     25,     26,
    27,     28,     48,     49,     61,     62,     82,     83,     47,     46,
    45,     44,     81,     80,     79,     78,     17,     18,     20,     22,
    77,     76,     75,     74,     29,     30,     43,     42,     41,     40,
    38,     39,     16,     19,     21,     50,     51,     59,     60,     63,
    64,     72,     73,     84,     85,     93,     94,     32,     33,     35,
    36,     53,     54,     56,     57,     66,     67,     69,     70,     87,
    88,     90,     91,     34,     55,     68,     89,     37,     58,     71,
    92,     31,     52,     65,     86
};

/* AMR 5.15kbit/s 模式比特顺序对应关系，详见协议TS26.101，Annex B */
const VOS_UINT16 g_auhwAmr515BitOrder[103] =
{
     7,      6,      5,      4,      3,      2,      1,      0,     15,     14,
    13,     12,     11,     10,      9,      8,     23,     24,     25,     26,
    27,     46,     65,     84,     45,     44,     43,     64,     63,     62,
    83,     82,     81,    102,    101,    100,     42,     61,     80,     99,
    28,     47,     66,     85,     18,     41,     60,     79,     98,     29,
    48,     67,     17,     20,     22,     40,     59,     78,     97,     21,
    30,     49,     68,     86,     19,     16,     87,     39,     38,     58,
    57,     77,     35,     54,     73,     92,     76,     96,     95,     36,
    55,     74,     93,     32,     51,     33,     52,     70,     71,     89,
    90,     31,     50,     69,     88,     37,     56,     75,     94,     34,
    53,     72,     91
};

/* AMR 5.9kbit/s 模式比特顺序对应关系，详见协议TS26.101，Annex B */
const VOS_UINT16 g_auhwAmr59BitOrder[118] =
{
    0,     1,      4,      5,      3,      6,      7,      2,     13,     15,
    8,     9,     11,     12,     14,     10,     16,     28,     74,     29,
    75,    27,    73,     26,     72,     30,     76,     51,     97,     50,
    71,    96,    117,    31,     77,     52,     98,     49,     70,     95,
    116,   53,    99,     32,     78,     33,     79,     48,     69,     94,
    115,   47,    68,     93,    114,     46,     67,     92,    113,     19,
    21,    23,    22,     18,     17,     20,     24,    111,     43,     89,
    110,   64,    65,     44,     90,     25,     45,     66,     91,    112,
    54,    100,   40,     61,     86,    107,     39,     60,     85,    106,
    36,    57,    82,    103,     35,     56,     81,    102,     34,     55,
    80,    101,   42,     63,     88,    109,     41,     62,     87,    108,
    38,    59,    84,    105,     37,     58,     83,    104
};

/* AMR 6.7kbit/s 模式比特顺序对应关系，详见协议TS26.101，Annex B */
const VOS_UINT16 g_auhwAmr67BitOrder[134] =
{
    0,      1,      4,      3,      5,      6,     13,      7,      2,      8,
    9,      11,     15,     12,     14,     10,     28,     82,     29,     83,
    27,     81,     26,     80,     30,     84,     16,     55,    109,     56,
    110,    31,     85,     57,    111,     48,     73,    102,    127,     32,
    86,     51,     76,    105,    130,     52,     77,    106,    131,     58,
    112,    33,     87,     19,     23,     53,     78,    107,    132,     21,
    22,     18,     17,     20,     24,     25,     50,     75,    104,    129,
    47,     72,    101,    126,     54,     79,    108,    133,     46,     71,
    100,   125,    128,    103,     74,     49,     45,     70,     99,    124,
    42,     67,     96,    121,     39,     64,     93,    118,     38,     63,
    92,    117,     35,     60,     89,    114,     34,     59,     88,    113,
    44,     69,     98,    123,     43,     68,     97,    122,     41,     66,
    95,    120,     40,     65,     94,    119,     37,     62,     91,    116,
    36,     61,     90,    115
};

/* AMR 7.4kbit/s 模式比特顺序对应关系，详见协议TS26.101，Annex B */
const VOS_UINT16 g_auhwAmr74BitOrder[148] =
{
    0,      1,      2,      3,      4,      5,      6,      7,      8,      9,
    10,     11,     12,     13,     14,     15,     16,     26,     87,     27,
    88,     28,     89,     29,     90,     30,     91,     51,     80,    112,
    141,    52,     81,    113,    142,     54,     83,    115,    144,     55,
    84,    116,    145,     58,    119,     59,    120,     21,     22,     23,
    17,     18,     19,     31,     60,     92,    121,     56,     85,    117,
    146,    20,     24,     25,     50,     79,    111,    140,     57,     86,
    118,   147,     49,     78,    110,    139,     48,     77,     53,     82,
    114,   143,    109,    138,     47,     76,    108,    137,     32,     33,
    61,     62,     93,     94,    122,    123,     41,     42,     43,     44,
    45,     46,     70,     71,     72,     73,     74,     75,    102,    103,
    104,   105,    106,    107,    131,    132,    133,    134,    135,    136,
    34,     63,     95,    124,     35,     64,     96,    125,     36,     65,
    97,    126,     37,     66,     98,    127,     38,     67,     99,    128,
    39,     68,    100,    129,     40,     69,    101,    130
};

/* AMR 7.95kbit/s 模式比特顺序对应关系，详见协议TS26.101，Annex B */
const VOS_UINT16 g_auhwAmr795BitOrder[159] =
{
     8,     7,       6,      5,      4,      3,      2,     14,     16,      9,
    10,     12,     13,     15,     11,     17,     20,     22,     24,     23,
    19,     18,     21,     56,     88,    122,    154,     57,     89,    123,
    155,    58,     90,    124,    156,     52,     84,    118,    150,     53,
    85,    119,    151,     27,     93,     28,     94,     29,     95,     30,
    96,     31,     97,     61,    127,     62,    128,     63,    129,     59,
    91,    125,    157,     32,     98,     64,    130,      1,      0,     25,
    26,     33,     99,     34,    100,     65,    131,     66,    132,     54,
    86,    120,    152,     60,     92,    126,    158,     55,     87,    121,
    153,   117,    116,    115,     46,     78,    112,    144,     43,     75,
    109,   141,     40,     72,    106,    138,     36,     68,    102,    134,
    114,   149,    148,    147,    146,     83,     82,     81,     80,     51,
    50,     49,     48,     47,     45,     44,     42,     39,     35,     79,
    77,     76,     74,     71,     67,    113,    111,    110,    108,    105,
    101,   145,    143,    142,    140,    137,    133,     41,     73,    107,
    139,    37,     69,    103,    135,     38,     70,    104,    136
};

/* AMR 10.2kbit/s 模式比特顺序对应关系，详见协议TS26.101，Annex B */
const VOS_UINT16 g_auhwAmr102BitOrder[204] =
{
     7,      6,      5,      4,      3,      2,      1,      0,     16,     15,
    14,     13,     12,     11,     10,      9,      8,     26,     27,     28,
    29,     30,     31,    115,    116,    117,    118,    119,    120,     72,
    73,    161,    162,     65,     68,     69,    108,    111,    112,    154,
    157,   158,    197,    200,    201,     32,     33,    121,    122,     74,
    75,    163,    164,     66,    109,    155,    198,     19,     23,     21,
    22,     18,     17,     20,     24,     25,     37,     36,     35,     34,
    80,     79,     78,     77,    126,    125,    124,    123,    169,    168,
    167,   166,     70,     67,     71,    113,    110,    114,    159,    156,
    160,   202,    199,    203,     76,    165,     81,     82,     92,     91,
    93,     83,     95,     85,     84,     94,    101,    102,     96,    104,
    86,    103,     87,     97,    127,    128,    138,    137,    139,    129,
    141,   131,    130,    140,    147,    148,    142,    150,    132,    149,
    133,   143,    170,    171,    181,    180,    182,    172,    184,    174,
    173,   183,    190,    191,    185,    193,    175,    192,    176,    186,
    38,     39,     49,     48,     50,     40,     52,     42,     41,     51,
    58,     59,     53,     61,     43,     60,     44,     54,    194,    179,
    189,   196,    177,    195,    178,    187,    188,    151,    136,    146,
    153,   134,    152,    135,    144,    145,    105,     90,    100,    107,
    88,    106,     89,     98,     99,     62,     47,     57,     64,     45,
    63,     46,     55,     56
};

/* AMR 12.2kbit/s 模式比特顺序对应关系，详见协议TS26.101，Annex B */
const VOS_UINT16 g_auhwAmr122BitOrder[244] =
{
    0,       1,      2,      3,      4,      5,      6,      7,      8,      9,
    10,     11,     12,     13,     14,     23,     15,     16,     17,     18,
    19,     20,     21,     22,     24,     25,     26,     27,     28,     38,
    141,    39,     142,     40,    143,     41,    144,     42,    145,     43,
    146,    44,     147,     45,    148,     46,    149,     47,     97,    150,
    200,    48,      98,    151,    201,     49,     99,    152,    202,     86,
    136,   189,     239,     87,    137,    190,    240,     88,    138,    191,
    241,    91,     194,     92,    195,     93,    196,     94,    197,     95,
    198,    29,      30,     31,     32,     33,     34,     35,     50,    100,
    153,   203,      89,    139,    192,    242,     51,    101,    154,    204,
    55,    105,     158,    208,     90,    140,    193,    243,     59,    109,
    162,   212,      63,    113,    166,    216,     67,    117,    170,    220,
    36,     37,      54,     53,     52,     58,     57,     56,     62,     61,
    60,     66,      65,     64,     70,     69,     68,    104,    103,    102,
    108,   107,      106,    112,    111,    110,    116,    115,    114,    120,
    119,   118,      157,    156,    155,    161,    160,    159,    165,    164,
    163,   169,      168,    167,    173,    172,    171,    207,    206,    205,
    211,   210,      209,    215,    214,    213,    219,    218,    217,    223,
    222,   221,       73,     72,     71,     76,     75,     74,     79,     78,
    77,     82,       81,     80,     85,     84,     83,    123,    122,    121,
    126,   125,      124,    129,    128,    127,    132,    131,    130,    135,
    134,   133,      176,    175,    174,    179,    178,    177,    182,    181,
    180,   185,      184,    183,    188,    187,    186,    226,    225,    224,
    229,   228,      227,    232,    231,    230,    235,    234,    233,    238,
    237,   236,       96,    199
};

/* 帧类型比特数数据对照表，AMR各种速率模式下A、B、C子流对应的长度，以bit为单位*/
const VOS_UINT16 g_ashwAmrFrmBitsLen[8][3] =
{
    { 42, 53, 0 },
    { 49, 54, 0 },
    { 55, 63, 0 },
    { 58, 76, 0 },
    { 61, 87, 0 },
    { 75, 84, 0 },
    { 65, 99, 40 },
    { 81, 103, 60 }
};

/* 模式比特顺序对应关系综合table，为AMR各种速率模式下比特顺序对应关系的综合表 */
const VOS_UINT16 *g_apAmrBitsOrderTab[8] =
{
    g_auhwAmr475BitOrder,
    g_auhwAmr515BitOrder,
    g_auhwAmr59BitOrder,
    g_auhwAmr67BitOrder,
    g_auhwAmr74BitOrder,
    g_auhwAmr795BitOrder,
    g_auhwAmr102BitOrder,
    g_auhwAmr122BitOrder
};

/* AMR各种速率模式对应的数据长度，以bit为单位 */
const VOS_UINT16 g_ashwAmrBitsOrderTabLen[8] =
{
    95,                                                                         /* 对应速率4.75kbit/s码流比特长度 */
    103,                                                                        /* 对应速率5.15kbit/s码流比特长度 */
    118,                                                                        /* 对应速率5.9kbit/s码流比特长度 */
    134,                                                                        /* 对应速率6.7kbit/s码流比特长度 */
    148,                                                                        /* 对应速率7.4kbit/s码流比特长度 */
    159,                                                                        /* 对应速率7.95kbit/s码流比特长度 */
    204,                                                                        /* 对应速率10.2kbit/s码流比特长度 */
    244                                                                         /* 对应速率12.2kbit/s码流比特长度 */
};

/* 指示一次通话过程中是否收到过SID_FIRST帧 */
VOS_UINT16 g_uhwAmrSidFirstFlag = 0;
/* AMR,ABC子流长度转换表
*/
MED_UINT16 g_auwAmrMap[AMR_FRAMETYPE_NUM][AMR_SUB_FRM_BUTT]
    = {{42,53,0},
       {49,54,0},
       {55,63,0},
       {58,76,0},
       {61,87,0},
       {75,84,0},
       {65,99,40},
       {81,103,60},
       {39,0,0},
       {43,0,0},
       {38,0,0},
       {37,0,0},
       {0,0,0},
       {0,0,0},
       {0,0,0},
       {0,0,0}};

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 AMR_UmtsUlConvertSpeech(
                VOS_UINT16                 uhwMode,
                VOS_INT16                 *pshwEncSerialBef,
                VOICE_WPHY_AMR_SERIAL_STRU  *pstFrameAfterConv)
{
    VOS_UINT16              uhwALen;
    VOS_UINT16              uhwBLen;
    VOS_UINT16              uhwCLen;

    VOS_INT16              *pshwEncSerialAfter;
    const VOS_UINT16       *puhwBitsOrder;

    /* 速率模式不正确 */
    if (uhwMode > CODEC_AMR_FRM_TYPE_122K)
    {
        return CODEC_RET_ERR;
    }

    uhwALen = pstFrameAfterConv->auhwLen[0];
    uhwBLen = pstFrameAfterConv->auhwLen[1];
    uhwCLen = pstFrameAfterConv->auhwLen[2];

    pshwEncSerialAfter = (VOS_INT16 *)pstFrameAfterConv->auwData;

    /*获取对应模式比特顺序表*/
    puhwBitsOrder      = AMR_GetBitsOrderTabPtr(uhwMode);

    /* 转换A子帧 */
    AMR_UmtsUlConvertSubFrame(uhwALen,
                              puhwBitsOrder,
                              pshwEncSerialBef,
                              pshwEncSerialAfter);

    puhwBitsOrder += uhwALen;
    pshwEncSerialAfter  += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwALen, 31), 5)); /*右移5位表示32比特对齐，与2相乘表示pshwSubFrame为16比特数组指针*/

    /* 转换B子帧 */
    AMR_UmtsUlConvertSubFrame(uhwBLen,
                              puhwBitsOrder,
                              pshwEncSerialBef,
                              pshwEncSerialAfter);

    puhwBitsOrder += uhwBLen;
    pshwEncSerialAfter  += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwBLen, 31), 5)); /*右移5位表示32比特对齐，与2相乘表示pshwSubFrame为16比特数组指针*/

    /* 转换C子帧 */
    AMR_UmtsUlConvertSubFrame(uhwCLen,
                              puhwBitsOrder,
                              pshwEncSerialBef,
                              pshwEncSerialAfter);

    return CODEC_RET_OK;
}
VOS_VOID AMR_UmtsUlConvertSubFrame(
                VOS_UINT16          uhwLen,
                const VOS_UINT16   *puhwBitsOrder,
                VOS_INT16          *pshwEncSerialBef,
                VOS_INT16          *pshwEncSerialAfter)
{
    VOS_UINT16  i;
    VOS_INT16   ashwBits[AMR_UMTS_MAX_SUB_FRAME_SIZE];                          /*12.2kbps模式下B子流的长度最大,为103 */

    /*检查转换长度为0，不作处理*/
    if (0 == uhwLen)
    {
        return;
    }

    /*先进行BIT重排*/
    for(i = 0; i < uhwLen ; i++)
    {
        ashwBits[i] = *(pshwEncSerialBef + puhwBitsOrder[i]);
    }

    CODEC_ComCodecBits2Bytes(ashwBits,
                           (VOS_INT16)uhwLen,
                           (VOS_INT8 *)pshwEncSerialAfter);
}
VOS_VOID AMR_UlConvertSidFirst(
                VOS_UINT16                  uhwMode,
                AMR_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter)
{
    /*置SID_FIRST帧全0码流*/
    pstSidEncSerialAfter->ashwSidData1[0]   = 0;
    pstSidEncSerialAfter->ashwSidData1[1]   = 0;
    pstSidEncSerialAfter->shwSidData2       = 0;
    pstSidEncSerialAfter->shwSti            = CODEC_AMR_STI_SID_FIRST;
    pstSidEncSerialAfter->shwModeIndication = uhwMode;

}
VOS_VOID AMR_UlConvertSidUpdate(
                VOS_UINT16                  uhwMode,
                VOS_INT16                  *pshwEncSerial,
                AMR_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter)
{
    /*转换35比特SID帧*/
    CODEC_ComCodecBits2Bytes(pshwEncSerial,
                           AMR_SID_UPDATE_LENGTH,
                           (VOS_INT8 *)pstSidEncSerialAfter);

    /* 1bit STI标志 */
    pstSidEncSerialAfter->shwSti = CODEC_AMR_STI_SID_UPDATE;

    /* 速率模式指示 */
    pstSidEncSerialAfter->shwModeIndication = uhwMode;
}
VOS_UINT32  AMR_UmtsUlConvert(
                AMR_ENCODED_SERIAL_STRU  *pstFrameBefConv,
                VOICE_WPHY_AMR_SERIAL_STRU *pstFrameAfterConv)
{

    VOS_UINT16                          uhwMode = 0;
    VOS_INT16                          *pshwEncSerialBef;
    VOS_INT16                          *pshwEncSerialAfter;
    CODEC_RET_ENUM_UINT32           enRetResult;

    if ((VOS_NULL == pstFrameBefConv) || (VOS_NULL == pstFrameAfterConv))
    {
        return CODEC_RET_ERR;
    }

    enRetResult = CODEC_RET_OK;

    /*对格式转换后输出的码流数据结构体清零*/
    UCOM_MemSet(pstFrameAfterConv, 0, sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /*设置转换后AMR码流的帧质量标志为为好帧*/
    pstFrameAfterConv->enQualityIdx = CODEC_AMR_FQI_QUALITY_GOOD;

    /*从输入参数中获取AMR速率模式及转换前码流指针*/
    uhwMode            = (VOS_UINT16)pstFrameBefConv->enAmrMode;
    pshwEncSerialBef   = pstFrameBefConv->ashwEncSerial;

    /*获取转换后码流指针*/
    pshwEncSerialAfter = (VOS_INT16 *)pstFrameAfterConv->auwData;

    /*判断编码器输出的帧类型*/
    switch (pstFrameBefConv->uhwFrameType)
    {
        case CODEC_AMR_TYPE_TX_SPEECH_GOOD:
        {
            if (uhwMode > CODEC_AMR_MODE_122)
            {
                return CODEC_RET_ERR;
            }

            /*写入SPEECH_GOOD帧的ABC三个子流的长度*/
            pstFrameAfterConv->auhwLen[0] = AMR_GetNumOfFrmBits(uhwMode, AMR_SUB_FRM_A);
            pstFrameAfterConv->auhwLen[1] = AMR_GetNumOfFrmBits(uhwMode, AMR_SUB_FRM_B);
            pstFrameAfterConv->auhwLen[2] = AMR_GetNumOfFrmBits(uhwMode, AMR_SUB_FRM_C);

            /*如果是语音帧进行比特转换*/
            enRetResult = AMR_UmtsUlConvertSpeech(uhwMode,
                                                  pshwEncSerialBef,
                                                  pstFrameAfterConv);

            break;
        }

        case CODEC_AMR_TYPE_TX_SID_FIRST:
        {
            if (uhwMode > CODEC_AMR_MODE_122)
            {
                return CODEC_RET_ERR;
            }

            /*写入SID_FIRST帧的ABC三个子流的长度*/
            pstFrameAfterConv->auhwLen[0] = AMR_SID_A_LENGTH;
            pstFrameAfterConv->auhwLen[1] = AMR_SID_B_LENGTH;
            pstFrameAfterConv->auhwLen[2] = AMR_SID_C_LENGTH;

            /*对SID_FIRST帧进行比特转换*/
            AMR_UlConvertSidFirst(uhwMode, (AMR_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);
            break;
        }

        case CODEC_AMR_TYPE_TX_SID_UPDATE:
        {
            if (uhwMode > CODEC_AMR_MODE_122)
            {
                return CODEC_RET_ERR;
            }

            /*写入SID_UPDATE帧的ABC三个子流的长度*/
            pstFrameAfterConv->auhwLen[0] = AMR_SID_A_LENGTH;
            pstFrameAfterConv->auhwLen[1] = AMR_SID_B_LENGTH;
            pstFrameAfterConv->auhwLen[2] = AMR_SID_C_LENGTH;

            /*对SID_UPDATE帧进行比特转换*/
            AMR_UlConvertSidUpdate(uhwMode,
                                   pshwEncSerialBef,
                                   (AMR_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);

            break;
        }

        default: /*包括NO_DATA的情况*/
        {
            break;
        }
    }

    return enRetResult;
}


VOS_VOID AMR_UmtsDlConvertSpeech(
                VOS_UINT16                  uhwMode,
                VOICE_WPHY_AMR_SERIAL_STRU   *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU    *pstDecSerialAfter)
{
    VOS_UINT16              uhwALen;
    VOS_UINT16              uhwBLen;
    VOS_UINT16              uhwCLen;

    VOS_INT16                   *pshwSerialBef;
    VOS_INT16                   *pshwSerialAfter;

    const VOS_UINT16            *puhwBitsOrder;

    pshwSerialBef   = (VOS_INT16 *)pstDecSerialBef->auwData;
    pshwSerialAfter = pstDecSerialAfter->ashwEncSerial;

    /*获取全局变量不同速率模式比特排序表指针*/
    puhwBitsOrder = AMR_GetBitsOrderTabPtr(uhwMode);

    /*设置帧类型为好的语音帧*/
    pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SPEECH_GOOD;

    /* 填充AMR速率模式 */
    pstDecSerialAfter->enAmrMode    = uhwMode;

    /*获取ABC子流长度*/
    uhwALen = pstDecSerialBef->auhwLen[0];
    uhwBLen = pstDecSerialBef->auhwLen[1];
    uhwCLen = pstDecSerialBef->auhwLen[2];

    /* 转换子帧A */
    AMR_UmtsDlConvertSubFrame(uhwALen,
                              puhwBitsOrder,
                              pshwSerialBef,
                              pshwSerialAfter);

    puhwBitsOrder   += uhwALen;
    //pshwSerialAfter += pstDecSerialBef->uhwALen;
    pshwSerialBef   += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwALen, 31), 5)); /*右移5位表示32比特对齐，与2相乘表示pshwSubFrame为16比特数组指针*/

    /* 转换子帧B */
    AMR_UmtsDlConvertSubFrame(uhwBLen,
                              puhwBitsOrder,
                              pshwSerialBef,
                              pshwSerialAfter);

    puhwBitsOrder   += uhwBLen;
    //pshwSerialAfter += pstDecSerialBef->uhwBLen;
    pshwSerialBef   += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwBLen, 31), 5)); /*右移5位表示32比特对齐，与2相乘表示pshwSubFrame为16比特数组指针*/

    /* 转换子帧C */
    AMR_UmtsDlConvertSubFrame(uhwCLen,
                              puhwBitsOrder,
                              pshwSerialBef,
                              pshwSerialAfter);

}
VOS_VOID AMR_UmtsDlConvertSubFrame(
                VOS_UINT16         uhwLen,
                const VOS_UINT16  *puhwBitsOrder,
                VOS_INT16         *pshwSerialBef,
                VOS_INT16         *pshwSerialAfter)
{
    VOS_UINT16    i;
    VOS_UINT16    uhwIndex;
    VOS_INT16     ashwBits[AMR_UMTS_MAX_SUB_FRAME_SIZE];                        /* 12.2kbps模式下B子流的长度最大,为103 */

    /*检查转换长度是否为0，为0时不作处理*/
    if (0 == uhwLen)
    {
        return;
    }

    /*将紧凑码流转换成非紧凑比特流*/
    CODEC_ComCodecBytes2Bits((VOS_INT8 *)pshwSerialBef,
                           (VOS_INT16)uhwLen,
                           ashwBits);

    /*根据比特排序表对接收的非紧凑比特流进行反重排*/
    for (i = 0; i < uhwLen ; i++)
    {
        uhwIndex                  = puhwBitsOrder[i];
        pshwSerialAfter[uhwIndex] = ashwBits[i];
    }

}


VOS_VOID AMR_UmtsDlConvertSid(
                AMR_ENCODED_SID_FRAME_STRU  *pstSidEncSerial,
                AMR_ENCODED_SERIAL_STRU     *pstDecSerialAfter)
{
    VOS_UINT16   uhwSidFirstFlag               = 0;
    VOS_UINT16  *puhwSidFirstFlag              = VOS_NULL;

    /*获取全局变量AMR一次通话过程中是否收到过SID_FIRST帧标志,并初始化为0*/
    puhwSidFirstFlag  = AMR_GetSidFirstFlagPtr();
    uhwSidFirstFlag   = *puhwSidFirstFlag;

    pstDecSerialAfter->enAmrMode = CODEC_AMR_MODE_MRDTX;

    /*STI_SID_Update帧码流解析*/
    if (CODEC_AMR_STI_SID_UPDATE == pstSidEncSerial->shwSti)
    {
        /* 如果本次通话中未出现SID_FIRST,则把本帧当NO_DATA处理 */
        if (uhwSidFirstFlag != 0)
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SID_UPDATE;
        }
        else
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SID_BAD;
            return;
        }

        /*转换35比特SID帧*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pstSidEncSerial,
                                AMR_SID_UPDATE_LENGTH,
                                pstDecSerialAfter->ashwEncSerial);
    }
    else  /* STI_SID_FIRST帧 */
    {
        pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SID_FIRST;
        *puhwSidFirstFlag               = 1;
    }
}
VOS_VOID AMR_UmtsDlConvertGoodFrame(
                VOICE_WPHY_AMR_SERIAL_STRU  *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
{
    VOS_UINT16                  uhwMode    = 0;
    VOS_UINT16                  uhwALength = 0;

    VOS_INT16                  *pshwEncSerial;

    /*从输入数据中获取A子流的长度和码流指针*/
    uhwALength    = pstDecSerialBef->auhwLen[0];
    pshwEncSerial = (VOS_INT16 *)pstDecSerialBef->auwData;


    if (AMR_SID_A_LENGTH == uhwALength)
    {
        /*SID帧码流解析*/
        AMR_UmtsDlConvertSid((AMR_ENCODED_SID_FRAME_STRU *)pshwEncSerial,
                             pstDecSerialAfter);
    }
    else
    {
        /*根据A子流长度查表搜索速率模式*/
        for (uhwMode = 0; uhwMode <= CODEC_AMR_MODE_122; uhwMode++)
        {
            if (uhwALength == AMR_GetNumOfFrmBits(uhwMode, 0))
            {
                break;
            }
        }

        /*若未查到对应速率模式，则判为NO DATA帧*/
        if (uhwMode > CODEC_AMR_MODE_122)
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_NO_DATA;


            pstDecSerialAfter->enAmrMode    = CODEC_AMR_MODE_MRDTX;
        }
        else /* 正常的AMR语音帧 */
        {
            *AMR_GetSidFirstFlagPtr() = 0;
            AMR_UmtsDlConvertSpeech(uhwMode, pstDecSerialBef, pstDecSerialAfter);
        }
    }

}
VOS_VOID AMR_UmtsDlConvertBadFrame(
                VOICE_WPHY_AMR_SERIAL_STRU   *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU    *pstDecSerialAfter)
{
    VOS_UINT16               uhwAmrLength;

    /* 从输入数据中获取A子流的长度 */
    uhwAmrLength  = pstDecSerialBef->auhwLen[0];

    if (uhwAmrLength >= AMR_MR475_A_LENGTH)
    {
        /*speech坏帧解析*/
        pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SPEECH_BAD;
    }
    else if (AMR_SID_A_LENGTH == uhwAmrLength)
    {
        pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_SID_BAD;

        /* 设置模式指示 */
        pstDecSerialAfter->enAmrMode    = CODEC_AMR_MODE_475;
    }
    else
    {
        /*NO DATA帧解析*/
        pstDecSerialAfter->uhwFrameType = CODEC_AMR_TYPE_RX_NO_DATA;

        /*设置模式指示*/
        pstDecSerialAfter->enAmrMode    = CODEC_AMR_MODE_475;
    }
}
VOS_UINT32  AMR_UmtsDlConvert(
                VOICE_WPHY_AMR_SERIAL_STRU  *pstDecSerialBef,
                AMR_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
{
    CODEC_AMR_FQI_QUALITY_ENUM_UINT16    enAmrQualityIndication;

    if (( VOS_NULL == pstDecSerialAfter ) || ( VOS_NULL == pstDecSerialBef ))
    {
        return CODEC_RET_ERR;
    }

    enAmrQualityIndication = pstDecSerialBef->enQualityIdx;

    /* 对比特转换及反重排输出的码流结构体进行清零 */
    UCOM_MemSet(pstDecSerialAfter, 0, sizeof(AMR_ENCODED_SERIAL_STRU));

    if (CODEC_AMR_FQI_QUALITY_GOOD == enAmrQualityIndication)
    {
        AMR_UmtsDlConvertGoodFrame(pstDecSerialBef, pstDecSerialAfter);
    }
    else
    {
        AMR_UmtsDlConvertBadFrame(pstDecSerialBef, pstDecSerialAfter);
    }

    return CODEC_RET_OK;
}
VOS_UINT32 AMR_GsmUlConvert(
                AMR_ENCODED_SERIAL_STRU *pstFrameBefConv,
                VOS_INT16               *pshwEncSerialAfter)
{
    VOS_UINT16                 i;
    VOS_UINT16                 uhwMode;
    VOS_UINT16                 uhwLen;
    VOS_UINT16                 uhwIndex;
    const VOS_UINT16          *puhwBitOrderTable;


    VOS_INT16                  ahswEncSerialSorted[AMR_MAX_SERIAL_SIZE];
    VOS_INT16                 *pshwEncSerial;

    /*输入参数非空检查*/
    if(( VOS_NULL == pstFrameBefConv ) ||( VOS_NULL == pshwEncSerialAfter ))
    {
        return CODEC_RET_ERR;
    }

    /*对比特重排后输出ahswEncSerialSorted[]数据清零*/
    CODEC_OpVecSet(ahswEncSerialSorted, AMR_MAX_SERIAL_SIZE, 0);

    /*对输出紧凑码流数组进行清零*/
    CODEC_OpVecSet(pshwEncSerialAfter, AMR_NUM_WORDS_OF_ENC_SERIAL, 0);

    /*获取编码模式和比特转换前码流指针*/
    uhwMode         = (VOS_UINT16)pstFrameBefConv->enAmrMode;
    pshwEncSerial   = pstFrameBefConv->ashwEncSerial;

    /* 根据编解码编码结果分类进行比特重排处理 */
    if (CODEC_AMR_TYPE_TX_SPEECH_GOOD == pstFrameBefConv->uhwFrameType)
    {
        /*获取对应模式比特排序表和比特长度*/
        puhwBitOrderTable = AMR_GetBitsOrderTabPtr(uhwMode);
        uhwLen            = AMR_GetBitsLenOfMode(uhwMode);

        /*根据比特顺序表对比特流进行比特重排*/
        for (i = 0; i < uhwLen; i++)
        {
            uhwIndex               = puhwBitOrderTable[i];
            ahswEncSerialSorted[i] = pshwEncSerial[uhwIndex];
        }

        /*将重排后比特流压缩为紧凑比特流*/
        CODEC_ComCodecBits2Bytes(ahswEncSerialSorted,
                               (VOS_INT16)uhwLen,
                               (VOS_INT8 *)pshwEncSerialAfter);
    }
    else if (CODEC_AMR_TYPE_TX_SID_UPDATE == pstFrameBefConv->uhwFrameType)
    {
        /*获取35比特SID_UPDATE帧码流*/
        AMR_UlConvertSidUpdate(uhwMode,
                               pshwEncSerial,
                               (AMR_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);
    }
    else
    {
        /* 包括SID_FIRST及NO_DATA等其它情况，需对pAmrFrame做清零处理 */
        /* 由于之前已经预先全部清零，此处无需再次进行 */
    }

    return CODEC_RET_OK;
}
VOS_UINT32 AMR_GsmDlConvert(
                VOS_INT16                         *pshwAmrFrame,
                CODEC_AMR_TYPE_RX_ENUM_UINT16  enRxType,
                CODEC_AMR_MODE_ENUM_UINT16     enMode,
                VOS_INT16                         *pshwDecSerial)
{
    VOS_UINT16           i;
    VOS_UINT16           uhwIndex;
    VOS_UINT16           uhwBitsLength;

    VOS_INT16            ashwEncSerial[AMR_MAX_SERIAL_SIZE];

    const VOS_UINT16    *puhwBitsOrder;

    /* 输入参数非空检查 */
    if (( VOS_NULL == pshwAmrFrame ) || ( VOS_NULL == pshwDecSerial ))
    {
        return CODEC_RET_ERR;
    }

    /*对非紧凑逐比特排序序列进行清零*/
    CODEC_OpVecSet(ashwEncSerial, AMR_MAX_SERIAL_SIZE, 0);

    /*对比特转换及反重排输出的数据进行清零*/
    CODEC_OpVecSet(pshwDecSerial, AMR_MAX_SERIAL_SIZE, 0);

    /* 根据外部输入的RX_FrameType判断是否是语音帧还是非语音帧 */
    if (   (CODEC_AMR_TYPE_RX_SPEECH_GOOD == enRxType)
        || (CODEC_AMR_TYPE_RX_SPEECH_DEGRADED == enRxType))
    {
        /*获取全局变量不同速率模式比特排序表指针和比特长度*/
        puhwBitsOrder = AMR_GetBitsOrderTabPtr(enMode);
        uhwBitsLength = AMR_GetBitsLenOfMode(enMode);

        /*将紧凑型码流转成非紧凑逐比特排列*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pshwAmrFrame,
                               (VOS_INT16)uhwBitsLength,
                               ashwEncSerial);

        /*对该比特流进行比特反重排，输出至pDecodeSpeech*/
        for (i = 0; i < uhwBitsLength; i++)
        {
            uhwIndex                = puhwBitsOrder[i];
            pshwDecSerial[uhwIndex] = ashwEncSerial[i];
        }
    }
    else if (CODEC_AMR_TYPE_RX_SID_UPDATE == enRxType)
    {
        /*将紧凑型码流转成非紧凑逐比特排列*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pshwAmrFrame,
                                AMR_SID_UPDATE_LENGTH,
                                ashwEncSerial);

        /*获取35比特的数据内容，无需进行反重排，输出至pDecodeSpeech*/
        CODEC_OpVecCpy(pshwDecSerial, ashwEncSerial, AMR_SID_UPDATE_LENGTH);
    }
    else
    {
        /* 其他情况下声码器不使用帧内内容数据，因此不做处理 */
    }

    return CODEC_RET_OK;
}


VOS_UINT32 AMR_ImsDlConvert(IMS_DL_DATA_OBJ_STRU *pstSrc,AMR_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
{
    VOICE_WPHY_AMR_SERIAL_STRU stImsDlData;
    VOS_UINT8                 *puchData     = NULL;
    VOS_UINT8                  uchRateMode  = 0;
    VOS_UINT8                  uchSti       = 0;
    VOS_UINT8                  uchBitLeft   = 0;
    VOS_UINT8                  uchTemp      = 0;

    /* 清空下行临时对象 */
    UCOM_MemSet(&stImsDlData,0,sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /* ErrorFlag检测、FrameType检测，不在当前FrameType范围内，填充nodata帧 */
    if ((IMSA_VOICE_ERROR == pstSrc->usErrorFlag) || (AMR_FRAMETYPE_NUM <= pstSrc->usFrameType))
    {
        /* 填充成nodata帧 */
        /* ABC子流长度 */
        stImsDlData.auhwLen[AMR_SUB_FRM_A] = 0;
        stImsDlData.auhwLen[AMR_SUB_FRM_B] = 0;
        stImsDlData.auhwLen[AMR_SUB_FRM_C] = 0;
        stImsDlData.enQualityIdx = pstSrc->usQualityIdx;
    }
    else
    {
        /*****************填充hifi侧amr帧结构*****************/
        /* 填充帧头 */
        /* ABC子流长度 */
        stImsDlData.auhwLen[AMR_SUB_FRM_A] = g_auwAmrMap[pstSrc->usFrameType][AMR_SUB_FRM_A];
        stImsDlData.auhwLen[AMR_SUB_FRM_B] = g_auwAmrMap[pstSrc->usFrameType][AMR_SUB_FRM_B];
        stImsDlData.auhwLen[AMR_SUB_FRM_C] = g_auwAmrMap[pstSrc->usFrameType][AMR_SUB_FRM_C];
        stImsDlData.enQualityIdx = pstSrc->usQualityIdx;

        /* 填充数据域 */
        AMR_FilledImsDlData((VOS_UINT8 *)pstSrc->ausData,&stImsDlData);

        /* 对SID进行处理 */
        if(AMR_SID_A_LENGTH == stImsDlData.auhwLen[AMR_SUB_FRM_A])
        {
            puchData    = (MED_UINT8*)&stImsDlData.auwData[0];
            uchRateMode = (puchData[4]&0x0e)<<3;      /*ModeIndication:3*/
            uchSti      = (puchData[4]&0x10)>>1;      /*Sti:1*/
            uchBitLeft  = (puchData[4]&0xe0)>>4;      /*SidData2:3*/
            uchTemp     = (uchRateMode|uchSti)|(uchBitLeft);
            puchData[4] = uchTemp;
        }
    }

    /*将紧凑排列的码流数据转换为非紧凑排列数据，并根据重排表进行反重排*/
    AMR_UmtsDlConvert(&stImsDlData,
                       pstDecSerialAfter);

    return CODEC_RET_OK;
}
VOS_VOID AMR_ImsUlConvert(VOS_UINT16 uhwAMRType,VOS_UINT16 uhwMode,VOS_INT16* pstEncSerial)
{
    IMS_UL_DATA_OBJ_STRU stUlDataObj;
    VOS_UINT16           uhwFrameType;
    VOS_UINT8           *puchData = NULL;

    UCOM_MemSet(&stUlDataObj,0,sizeof(IMS_UL_DATA_OBJ_STRU));

    puchData = (MED_UINT8*)stUlDataObj.ausData;

    /*****************填充消息*****************/
    /*****************填充IMSA侧amr帧结构*****************/
    /* 填充数据域 */
    if(CODEC_AMR_TYPE_TX_NO_DATA != uhwAMRType)
    {
        AMR_FilledImsUlData(&puchData[0],(VOICE_WPHY_AMR_SERIAL_STRU*)pstEncSerial);
    }

    /* 获取帧类型 */
    switch(uhwAMRType)
    {
        /* SID帧 */
        case CODEC_AMR_TYPE_TX_SID_FIRST:
        {
            uhwFrameType = AMR_FRAMETYPE_SID;
            puchData[4]  = (puchData[4]&0xe0)|((MED_UINT8)(uhwMode<<1));
        }break;
        case CODEC_AMR_TYPE_TX_SID_UPDATE:
        {
            uhwFrameType = AMR_FRAMETYPE_SID;
            puchData[4]  = (puchData[4]&0xe0)|(0x10|((MED_UINT8)(uhwMode<<1)));

        }break;
        /* NOData帧 */
        case CODEC_AMR_TYPE_TX_NO_DATA:
        {
            uhwFrameType = AMR_FRAMETYPE_NODATA;
        }break;
        /* speech帧 */
        default:
        {
            uhwFrameType = (MED_UINT8)(uhwMode);
        }break;
    }

    stUlDataObj.usStatus    = uhwAMRType;   /* 语音帧还是SID帧，好帧还是坏帧 */
    stUlDataObj.usFrameType = uhwFrameType; /* 帧类型 */

    /* 重置Encode Buffer数据，置成上行IMS数据结构 */
    UCOM_MemSet(pstEncSerial,0,sizeof(VOS_UINT16)*VOICE_CODED_FRAME_LENGTH);
    UCOM_MemCpy(&pstEncSerial[0],&stUlDataObj,sizeof(IMS_UL_DATA_OBJ_STRU));

}


VOS_VOID AMR_FilledImsDlData( VOS_UINT8 *puchData,VOICE_WPHY_AMR_SERIAL_STRU *pstDes)
{
    MED_UINT8*  puchSrc;
    MED_UINT32* puwADes;
    MED_UINT32* puwBDes;
    MED_UINT32* puwCDes;

    /* A B C码流长度 */
    MED_UINT32 uwALen = pstDes->auhwLen[AMR_SUB_FRM_A];
    MED_UINT32 uwBLen = pstDes->auhwLen[AMR_SUB_FRM_B];
    MED_UINT32 uwCLen = pstDes->auhwLen[AMR_SUB_FRM_C];

    MED_UINT32 uwCurBit = 0;   /* 当前bit计数 */

    puchSrc = puchData; /* 去掉帧头后的数据域 */

    /* 计算需要填充的A B C码流起始地址,32bit对齐 */
    puwADes = &(pstDes->auwData[0]);
    puwBDes = ((0==(uwALen%32))?(puwADes + (uwALen/32)):(puwADes + (uwALen/32) + 1));
    puwCDes = ((0==(uwBLen%32))?(puwBDes + (uwBLen/32)):(puwBDes + (uwBLen/32) + 1));

    /* 填充A码流 */
    AMR_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwADes,&uwCurBit,uwALen);

    /* 填充B码流 */
    AMR_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwBDes,&uwCurBit,uwBLen);

    /* 填充C码流 */
    AMR_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwCDes,&uwCurBit,uwCLen);

}
VOS_VOID AMR_FilledImsUlData( VOS_UINT8   *puchImsaData,
                                    VOICE_WPHY_AMR_SERIAL_STRU* pstHifiData)
{
    VOS_UINT8*  puchDes;
    VOS_UINT32* puwASrc;
    VOS_UINT32* puwBSrc;
    VOS_UINT32* puwCSrc;

    /* A B C码流长度 */
    VOS_UINT32 uwALen = (VOS_UINT32)pstHifiData->auhwLen[AMR_SUB_FRM_A];
    VOS_UINT32 uwBLen = (VOS_UINT32)pstHifiData->auhwLen[AMR_SUB_FRM_B];
    VOS_UINT32 uwCLen = (VOS_UINT32)pstHifiData->auhwLen[AMR_SUB_FRM_C];

    VOS_UINT32 uwCurBit = 0;   /* 当前bit计数 */

    /* 计算需要填充的A B C码流起始地址,32bit对齐 */
    puwASrc = &(pstHifiData->auwData[0]);
    puwBSrc = ((0 == (uwALen%32))?(puwASrc + (uwALen/32)):(puwASrc + (uwALen/32) + 1));
    puwCSrc = ((0 == (uwBLen%32))?(puwBSrc + (uwBLen/32)):(puwBSrc + (uwBLen/32) + 1));

    /* 填充的目的地址 */
    puchDes = puchImsaData;

    /* 填充A码流 */
    AMR_ImsUlBitFilled((VOS_UINT8*)puwASrc,puchDes,&uwCurBit,uwALen);

    /* 填充B码流 */
    AMR_ImsUlBitFilled((VOS_UINT8*)puwBSrc,puchDes,&uwCurBit,uwBLen);

    /* 填充C码流 */
    AMR_ImsUlBitFilled((VOS_UINT8*)puwCSrc,puchDes,&uwCurBit,uwCLen);
}
VOS_VOID AMR_ImsUlBitFilled(VOS_UINT8* puchSrc,VOS_UINT8* puchDes,
                                 VOS_UINT32* puwCurBit,VOS_UINT32 uwLen)
{
    VOS_UINT32 i = 0;
    VOS_UINT8  uchVal = 0;
    VOS_UINT8  auwBitShift[8]
    ={UCOM_BIT7,UCOM_BIT6,UCOM_BIT5,UCOM_BIT4,UCOM_BIT3,UCOM_BIT2,UCOM_BIT1,UCOM_BIT0};

    for(i = 0; i < uwLen; i++)
    {
        uchVal     = (puchSrc[i/8]>>auwBitShift[i%8])&0x01;
        uchVal     = (VOS_UINT8)(uchVal<<auwBitShift[(*puwCurBit)%8]);
        puchDes[(*puwCurBit)/8]  = (uchVal)|(puchDes[(*puwCurBit)/8]);
        (*puwCurBit) += 1;
    }
}


VOS_VOID AMR_ImsDlBitFilled(VOS_UINT8* puchSrc,VOS_UINT8* puchDes,
                                 VOS_UINT32* puwCurBit,VOS_UINT32 uwLen)
{
    VOS_UINT32 i = 0;
    VOS_UINT8  uchVal = 0;
    VOS_UINT8  auwBitShift[8]
    ={UCOM_BIT7,UCOM_BIT6,UCOM_BIT5,UCOM_BIT4,UCOM_BIT3,UCOM_BIT2,UCOM_BIT1,UCOM_BIT0};

    for(i = 0; i < uwLen; i++)
    {
        uchVal     = (puchSrc[(*puwCurBit)/8]>>auwBitShift[(*puwCurBit)%8])&0x01;
        uchVal     = (VOS_UINT8)(uchVal<<auwBitShift[i%8]);
        puchDes[i/8]  = (uchVal)|(puchDes[i/8]);
        (*puwCurBit) += 1;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



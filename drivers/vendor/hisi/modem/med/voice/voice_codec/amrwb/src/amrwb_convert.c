

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_lib.h"
#include "amrwb_interface.h"
#include "amrwb_convert.h"
#include "ucom_comm.h"
#include "voice_pcm.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*****************************************************************************
 变量名     : g_auhwAmrwb660BitOrder
 功能描述   : AMR-WB 6.60kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb660BitOrder[132] =
{
    0  , 5  , 6  , 7  , 61 , 84 , 107, 130, 62 , 85,
    8  , 4  , 37 , 38 , 39 , 40 , 58 , 81 , 104, 127,
    60 , 83 , 106, 129, 108, 131, 128, 41 , 42 , 80,
    126, 1  , 3  , 57 , 103, 82 , 105, 59 , 2  , 63,
    109, 110, 86 , 19 , 22 , 23 , 64 , 87 , 18 , 20,
    21 , 17 , 13 , 88 , 43 , 89 , 65 , 111, 14 , 24,
    25 , 26 , 27 , 28 , 15 , 16 , 44 , 90 , 66 , 112,
    9  , 11 , 10 , 12 , 67 , 113, 29 , 30 , 31 , 32,
    34 , 33 , 35 , 36 , 45 , 51 , 68 , 74 , 91 , 97,
    114, 120, 46 , 69 , 92 , 115, 52 , 75 , 98 , 121,
    47 , 70 , 93 , 116, 53 , 76 , 99 , 122, 48 , 71,
    94 , 117, 54 , 77 , 100, 123, 49 , 72 , 95 , 118,
    55 , 78 , 101, 124, 50 , 73 , 96 , 119, 56 , 79,
    102, 125
};

/*****************************************************************************
 变量名     : g_auhwAmrwb885BitOrder
 功能描述   : AMR-WB 8.85kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb885BitOrder[177] =
{
    0  , 4  , 6  , 7  , 5  , 3  , 47 , 48 , 49 , 112,
    113, 114, 75 , 106, 140, 171, 80 , 111, 145, 176,
    77 , 108, 142, 173, 78 , 109, 143, 174, 79 , 110,
    144, 175, 76 , 107, 141, 172, 50 , 115, 51 , 2  ,
    1  , 81 , 116, 146, 19 , 21 , 12 , 17 , 18 , 20 ,
    16 , 25 , 13 , 10 , 14 , 24 , 23 , 22 , 26 , 8  ,
    15 , 52 , 117, 31 , 82 , 147, 9  , 33 , 11 , 83 ,
    148, 53 , 118, 28 , 27 , 84 , 149, 34 , 35 , 29 ,
    46 , 32 , 30 , 54 , 119, 37 , 36 , 39 , 38 , 40 ,
    85 , 150, 41 , 42 , 43 , 44 , 45 , 55 , 60 , 65 ,
    70 , 86 , 91 , 96 , 101, 120, 125, 130, 135, 151,
    156, 161, 166, 56 , 87 , 121, 152, 61 , 92 , 126,
    157, 66 , 97 , 131, 162, 71 , 102, 136, 167, 57 ,
    88 , 122, 153, 62 , 93 , 127, 158, 67 , 98 , 132,
    163, 72 , 103, 137, 168, 58 , 89 , 123, 154, 63 ,
    94 , 128, 159, 68 , 99 , 133, 164, 73 , 104, 138,
    169, 59 , 90 , 124, 155, 64 , 95 , 129, 160, 69 ,
    100, 134, 165, 74 , 105, 139, 170
};

/*****************************************************************************
 变量名     : g_auhwAmrwb1265BitOrder
 功能描述   : AMR-WB 12.65kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb1265BitOrder[253] =
{
    0  , 4  , 6  , 93 , 143, 196, 246, 7  , 5  , 3  ,
    47 , 48 , 49 , 50 , 51 , 150, 151, 152, 153, 154,
    94 , 144, 197, 247, 99 , 149, 202, 252, 96 , 146,
    199, 249, 97 , 147, 200, 250, 100, 203, 98 , 148,
    201, 251, 95 , 145, 198, 248, 52 , 2  , 1  , 101,
    204, 155, 19 , 21 , 12 , 17 , 18 , 20 , 16 , 25 ,
    13 , 10 , 14 , 24 , 23 , 22 , 26 , 8  , 15 , 53 ,
    156, 31 , 102, 205, 9  , 33 , 11 , 103, 206, 54 ,
    157, 28 , 27 , 104, 207, 34 , 35 , 29 , 46 , 32 ,
    30 , 55 , 158, 37 , 36 , 39 , 38 , 40 , 105, 208,
    41 , 42 , 43 , 44 , 45 , 56 , 106, 159, 209, 57 ,
    66 , 75 , 84 , 107, 116, 125, 134, 160, 169, 178,
    187, 210, 219, 228, 237, 58 , 108, 161, 211, 62 ,
    112, 165, 215, 67 , 117, 170, 220, 71 , 121, 174,
    224, 76 , 126, 179, 229, 80 , 130, 183, 233, 85 ,
    135, 188, 238, 89 , 139, 192, 242, 59 , 109, 162,
    212, 63 , 113, 166, 216, 68 , 118, 171, 221, 72 ,
    122, 175, 225, 77 , 127, 180, 230, 81 , 131, 184,
    234, 86 , 136, 189, 239, 90 , 140, 193, 243, 60 ,
    110, 163, 213, 64 , 114, 167, 217, 69 , 119, 172,
    222, 73 , 123, 176, 226, 78 , 128, 181, 231, 82 ,
    132, 185, 235, 87 , 137, 190, 240, 91 , 141, 194,
    244, 61 , 111, 164, 214, 65 , 115, 168, 218, 70 ,
    120, 173, 223, 74 , 124, 177, 227, 79 , 129, 182,
    232, 83 , 133, 186, 236, 88 , 138, 191, 241, 92 ,
    142, 195, 245
};

/*****************************************************************************
 变量名     : g_auhwAmrwb1425BitOrder
 功能描述   : AMR 14.25kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb1425BitOrder[285] =
{
    0  , 4  , 6  , 101, 159, 220, 278, 7  , 5  , 3  ,
    47 , 48 , 49 , 50 , 51 , 166, 167, 168, 169, 170,
    102, 160, 221, 279, 107, 165, 226, 284, 104, 162,
    223, 281, 105, 163, 224, 282, 108, 227, 106, 164,
    225, 283, 103, 161, 222, 280, 52 , 2  , 1  , 109,
    228, 171, 19 , 21 , 12 , 17 , 18 , 20 , 16 , 25 ,
    13 , 10 , 14 , 24 , 23 , 22 , 26 , 8  , 15 , 53 ,
    172, 31 , 110, 229, 9  , 33 , 11 , 111, 230, 54 ,
    173, 28 , 27 , 112, 231, 34 , 35 , 29 , 46 , 32 ,
    30 , 55 , 174, 37 , 36 , 39 , 38 , 40 , 113, 232,
    41 , 42 , 43 , 44 , 45 , 56 , 114, 175, 233, 62 ,
    120, 181, 239, 75 , 133, 194, 252, 57 , 115, 176,
    234, 63 , 121, 182, 240, 70 , 128, 189, 247, 76 ,
    134, 195, 253, 83 , 141, 202, 260, 92 , 150, 211,
    269, 84 , 142, 203, 261, 93 , 151, 212, 270, 85 ,
    143, 204, 262, 94 , 152, 213, 271, 86 , 144, 205,
    263, 95 , 153, 214, 272, 64 , 122, 183, 241, 77 ,
    135, 196, 254, 65 , 123, 184, 242, 78 , 136, 197,
    255, 87 , 145, 206, 264, 96 , 154, 215, 273, 58 ,
    116, 177, 235, 66 , 124, 185, 243, 71 , 129, 190,
    248, 79 , 137, 198, 256, 88 , 146, 207, 265, 97 ,
    155, 216, 274, 59 , 117, 178, 236, 67 , 125, 186,
    244, 72 , 130, 191, 249, 80 , 138, 199, 257, 89 ,
    147, 208, 266, 98 , 156, 217, 275, 60 , 118, 179,
    237, 68 , 126, 187, 245, 73 , 131, 192, 250, 81 ,
    139, 200, 258, 90 , 148, 209, 267, 99 , 157, 218,
    276, 61 , 119, 180, 238, 69 , 127, 188, 246, 74 ,
    132, 193, 251, 82 , 140, 201, 259, 91 , 149, 210,
    268, 100, 158, 219, 277
};

/*****************************************************************************
 变量名     : g_auhwAmrwb1585BitOrder
 功能描述   : AMR-WB 15.85kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb1585BitOrder[317] =
{
    0  , 4  , 6  , 109, 175, 244, 310, 7  , 5  , 3  ,
    47 , 48 , 49 , 50 , 51 , 182, 183, 184, 185, 186,
    110, 176, 245, 311, 115, 181, 250, 316, 112, 178,
    247, 313, 113, 179, 248, 314, 116, 251, 114, 180,
    249, 315, 111, 177, 246, 312, 52 , 2  , 1  , 117,
    252, 187, 19 , 21 , 12 , 17 , 18 , 20 , 16 , 25 ,
    13 , 10 , 14 , 24 , 23 , 22 , 26 , 8  , 15 , 53 ,
    188, 31 , 118, 253, 9  , 33 , 11 , 119, 254, 54 ,
    189, 28 , 27 , 120, 255, 34 , 35 , 29 , 46 , 32 ,
    30 , 55 , 190, 37 , 36 , 39 , 38 , 40 , 121, 256,
    41 , 42 , 43 , 44 , 45 , 56 , 122, 191, 257, 63 ,
    129, 198, 264, 76 , 142, 211, 277, 89 , 155, 224,
    290, 102, 168, 237, 303, 57 , 123, 192, 258, 70 ,
    136, 205, 271, 83 , 149, 218, 284, 96 , 162, 231,
    297, 62 , 128, 197, 263, 75 , 141, 210, 276, 88 ,
    154, 223, 289, 101, 167, 236, 302, 58 , 124, 193,
    259, 71 , 137, 206, 272, 84 , 150, 219, 285, 97 ,
    163, 232, 298, 59 , 125, 194, 260, 64 , 130, 199,
    265, 67 , 133, 202, 268, 72 , 138, 207, 273, 77 ,
    143, 212, 278, 80 , 146, 215, 281, 85 , 151, 220,
    286, 90 , 156, 225, 291, 93 , 159, 228, 294, 98 ,
    164, 233, 299, 103, 169, 238, 304, 106, 172, 241,
    307, 60 , 126, 195, 261, 65 , 131, 200, 266, 68 ,
    134, 203, 269, 73 , 139, 208, 274, 78 , 144, 213,
    279, 81 , 147, 216, 282, 86 , 152, 221, 287, 91 ,
    157, 226, 292, 94 , 160, 229, 295, 99 , 165, 234,
    300, 104, 170, 239, 305, 107, 173, 242, 308, 61 ,
    127, 196, 262, 66 , 132, 201, 267, 69 , 135, 204,
    270, 74 , 140, 209, 275, 79 , 145, 214, 280, 82 ,
    148, 217, 283, 87 , 153, 222, 288, 92 , 158, 227,
    293, 95 , 161, 230, 296, 100, 166, 235, 301, 105,
    171, 240, 306, 108, 174, 243, 309
};

/*****************************************************************************
 变量名     : g_auhwAmrwb1825BitOrder
 功能描述   : AMR-WB 18.25kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb1825BitOrder[365] =
{
    0  , 4  , 6  , 121, 199, 280, 358, 7  , 5  , 3  ,
    47 , 48 , 49 , 50 , 51 , 206, 207, 208, 209, 210,
    122, 200, 281, 359, 127, 205, 286, 364, 124, 202,
    283, 361, 125, 203, 284, 362, 128, 287, 126, 204,
    285, 363, 123, 201, 282, 360, 52 , 2  , 1  , 129,
    288, 211, 19 , 21 , 12 , 17 , 18 , 20 , 16 , 25 ,
    13 , 10 , 14 , 24 , 23 , 22 , 26 , 8  , 15 , 53 ,
    212, 31 , 130, 289, 9  , 33 , 11 , 131, 290, 54 ,
    213, 28 , 27 , 132, 291, 34 , 35 , 29 , 46 , 32 ,
    30 , 55 , 214, 37 , 36 , 39 , 38 , 40 , 133, 292,
    41 , 42 , 43 , 44 , 45 , 56 , 134, 215, 293, 198,
    299, 136, 120, 138, 60 , 279, 58 , 62 , 357, 139,
    140, 295, 156, 57 , 219, 297, 63 , 217, 137, 170,
    300, 222, 64 , 106, 61 , 78 , 294, 92 , 142, 141,
    135, 221, 296, 301, 343, 59 , 298, 184, 329, 315,
    220, 216, 265, 251, 218, 237, 352, 223, 157, 86 ,
    171, 87 , 164, 351, 111, 302, 65 , 178, 115, 323,
    72 , 192, 101, 179, 93 , 73 , 193, 151, 337, 309,
    143, 274, 69 , 324, 165, 150, 97 , 338, 110, 310,
    330, 273, 68 , 107, 175, 245, 114, 79 , 113, 189,
    246, 259, 174, 71 , 185, 96 , 344, 100, 322, 83 ,
    334, 316, 333, 252, 161, 348, 147, 82 , 269, 232,
    260, 308, 353, 347, 163, 231, 306, 320, 188, 270,
    146, 177, 266, 350, 256, 85 , 149, 116, 191, 160,
    238, 258, 336, 305, 255, 88 , 224, 99 , 339, 230,
    228, 227, 272, 242, 241, 319, 233, 311, 102, 74 ,
    180, 275, 66 , 194, 152, 325, 172, 247, 244, 261,
    117, 158, 166, 354, 75 , 144, 108, 312, 94 , 186,
    303, 80 , 234, 89 , 195, 112, 340, 181, 345, 317,
    326, 276, 239, 167, 118, 313, 70 , 355, 327, 253,
    190, 176, 271, 104, 98 , 153, 103, 90 , 76 , 267,
    277, 248, 225, 262, 182, 84 , 154, 235, 335, 168,
    331, 196, 341, 249, 162, 307, 148, 349, 263, 321,
    257, 243, 229, 356, 159, 119, 67 , 187, 173, 145,
    240, 77 , 304, 332, 314, 342, 109, 254, 81 , 278,
    105, 91 , 346, 318, 183, 250, 197, 328, 95 , 155,
    169, 268, 226, 236, 264
};


/*****************************************************************************
 变量名     : g_auhwAmrwb1985BitOrder
 功能描述   : AMR-WB 19.85kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb1985BitOrder[397] =
{
    0  , 4  , 6  , 129, 215, 304, 390, 7  , 5  , 3  ,
    47 , 48 , 49 , 50 , 51 , 222, 223, 224, 225, 226,
    130, 216, 305, 391, 135, 221, 310, 396, 132, 218,
    307, 393, 133, 219, 308, 394, 136, 311, 134, 220,
    309, 395, 131, 217, 306, 392, 52 , 2  , 1  , 137,
    312, 227, 19 , 21 , 12 , 17 , 18 , 20 , 16 , 25 ,
    13 , 10 , 14 , 24 , 23 , 22 , 26 , 8  , 15 , 53 ,
    228, 31 , 138, 313, 9  , 33 , 11 , 139, 314, 54 ,
    229, 28 , 27 , 140, 315, 34 , 35 , 29 , 46 , 32 ,
    30 , 55 , 230, 37 , 36 , 39 , 38 , 40 , 141, 316,
    41 , 42 , 43 , 44 , 45 , 56 , 142, 231, 317, 63 ,
    73 , 92 , 340, 82 , 324, 149, 353, 159, 334, 165,
    338, 178, 163, 254, 77 , 168, 257, 153, 343, 57 ,
    248, 238, 79 , 252, 166, 67 , 80 , 201, 101, 267,
    143, 164, 341, 255, 339, 187, 376, 318, 78 , 328,
    362, 115, 232, 242, 253, 290, 276, 62 , 58 , 158,
    68 , 93 , 179, 319, 148, 169, 154, 72 , 385, 329,
    333, 344, 102, 83 , 144, 233, 323, 124, 243, 192,
    354, 237, 64 , 247, 202, 209, 150, 116, 335, 268,
    239, 299, 188, 196, 298, 94 , 195, 258, 123, 363,
    384, 109, 325, 371, 170, 370, 84 , 110, 295, 180,
    74 , 210, 191, 106, 291, 205, 367, 381, 377, 206,
    355, 122, 119, 120, 383, 160, 105, 108, 277, 380,
    294, 284, 285, 345, 208, 269, 249, 366, 386, 300,
    297, 259, 125, 369, 197, 97 , 194, 286, 211, 281,
    280, 183, 372, 87 , 155, 283, 59 , 348, 327, 184,
    76 , 111, 330, 203, 349, 69 , 98 , 152, 145, 189,
    66 , 320, 337, 173, 358, 251, 198, 174, 263, 262,
    126, 241, 193, 88 , 388, 117, 95 , 387, 112, 359,
    287, 244, 103, 272, 301, 171, 162, 234, 273, 127,
    373, 181, 292, 85 , 378, 302, 121, 107, 364, 346,
    356, 212, 278, 213, 65 , 382, 288, 207, 113, 175,
    99 , 296, 374, 368, 199, 260, 185, 336, 331, 161,
    270, 264, 250, 240, 75 , 350, 151, 60 , 89 , 321,
    156, 274, 360, 326, 70 , 282, 167, 146, 352, 81 ,
    91 , 389, 266, 245, 177, 235, 190, 256, 204, 342,
    128, 118, 303, 104, 379, 182, 114, 375, 200, 96 ,
    293, 172, 214, 365, 279, 86 , 289, 351, 347, 357,
    261, 186, 176, 271, 90 , 100, 147, 322, 275, 361,
    71 , 332, 61 , 265, 157, 246, 236
};

/*****************************************************************************
 变量名     : g_auhwAmrwb2305BitOrder
 功能描述   : AMR-WB 10.2kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb2305BitOrder[461] =
{
    0  , 4  , 6  , 145, 247, 352, 454, 7  , 5  , 3  ,
    47 , 48 , 49 , 50 , 51 , 254, 255, 256, 257, 258,
    146, 248, 353, 455, 151, 253, 358, 460, 148, 250,
    355, 457, 149, 251, 356, 458, 152, 359, 150, 252,
    357, 459, 147, 249, 354, 456, 52 , 2  , 1  , 153,
    360, 259, 19 , 21 , 12 , 17 , 18 , 20 , 16 , 25 ,
    13 , 10 , 14 , 24 , 23 , 22 , 26 , 8  , 15 , 53 ,
    260, 31 , 154, 361, 9  , 33 , 11 , 155, 362, 54 ,
    261, 28 , 27 , 156, 363, 34 , 35 , 29 , 46 , 32 ,
    30 , 55 , 262, 37 , 36 , 39 , 38 , 40 , 157, 364,
    41 , 42 , 43 , 44 , 45 , 56 , 158, 263, 365, 181,
    192, 170, 79 , 57 , 399, 90 , 159, 297, 377, 366,
    275, 68 , 183, 388, 286, 194, 299, 92 , 70 , 182,
    401, 172, 59 , 91 , 58 , 400, 368, 161, 81 , 160,
    264, 171, 80 , 389, 390, 378, 379, 193, 298, 69 ,
    266, 265, 367, 277, 288, 276, 287, 184, 60 , 195,
    82 , 93 , 71 , 369, 402, 173, 162, 444, 300, 391,
    98 , 76 , 278, 61 , 267, 374, 135, 411, 167, 102,
    380, 200, 87 , 178, 65 , 94 , 204, 124, 72 , 342,
    189, 305, 381, 396, 433, 301, 226, 407, 289, 237,
    113, 215, 185, 128, 309, 403, 116, 320, 196, 331,
    370, 422, 174, 64 , 392, 83 , 425, 219, 134, 188,
    432, 112, 427, 139, 279, 163, 436, 208, 447, 218,
    236, 229, 97 , 294, 385, 230, 166, 268, 177, 443,
    225, 426, 101, 272, 138, 127, 290, 117, 347, 199,
    414, 95 , 140, 240, 410, 395, 209, 129, 283, 346,
    105, 241, 437, 86 , 308, 448, 203, 345, 186, 107,
    220, 415, 334, 319, 106, 313, 118, 123, 73 , 207,
    421, 214, 384, 373, 438, 62 , 371, 341, 75 , 449,
    168, 323, 164, 242, 416, 324, 304, 197, 335, 404,
    271, 63 , 191, 325, 96 , 169, 231, 280, 312, 187,
    406, 84 , 201, 100, 67 , 382, 175, 336, 202, 330,
    269, 393, 376, 383, 293, 307, 409, 179, 285, 314,
    302, 372, 398, 190, 180, 89 , 99 , 103, 232, 78 ,
    88 , 77 , 136, 387, 165, 198, 394, 125, 176, 428,
    74 , 375, 238, 227, 66 , 273, 282, 141, 306, 412,
    114, 85 , 130, 348, 119, 291, 296, 386, 233, 397,
    303, 405, 284, 445, 423, 221, 210, 205, 450, 108,
    274, 434, 216, 343, 337, 142, 243, 321, 408, 451,
    310, 292, 120, 109, 281, 439, 270, 429, 332, 295,
    418, 211, 315, 222, 326, 131, 430, 244, 327, 349,
    417, 316, 143, 338, 440, 234, 110, 212, 452, 245,
    121, 419, 350, 223, 132, 441, 328, 413, 317, 339,
    126, 104, 137, 446, 344, 239, 435, 115, 333, 206,
    322, 217, 228, 424, 453, 311, 351, 111, 442, 224,
    213, 122, 431, 340, 235, 246, 133, 144, 420, 329,
    318
};

/*****************************************************************************
 变量名     : g_auhwAmrwb2385BitOrder
 功能描述   : AMR-WB 23.85kbit/s 模式比特顺序对应关系，详见协议TS26.201，Annex B
*****************************************************************************/
const VOS_UINT16 g_auhwAmrwb2385BitOrder[477] =
{
    0  , 4  , 6  , 145, 251, 360, 466, 7  , 5  , 3  ,
    47 , 48 , 49 , 50 , 51 , 262, 263, 264, 265, 266,
    146, 252, 361, 467, 151, 257, 366, 472, 148, 254,
    363, 469, 149, 255, 364, 470, 156, 371, 150, 256,
    365, 471, 147, 253, 362, 468, 52 , 2  , 1  , 157,
    372, 267, 19 , 21 , 12 , 17 , 18 , 20 , 16 , 25 ,
    13 , 10 , 14 , 24 , 23 , 22 , 26 , 8  , 15 , 53 ,
    268, 31 , 152, 153, 154, 155, 258, 259, 260, 261,
    367, 368, 369, 370, 473, 474, 475, 476, 158, 373,
    9  , 33 , 11 , 159, 374, 54 , 269, 28 , 27 , 160,
    375, 34 , 35 , 29 , 46 , 32 , 30 , 55 , 270, 37 ,
    36 , 39 , 38 , 40 , 161, 376, 41 , 42 , 43 , 44 ,
    45 , 56 , 162, 271, 377, 185, 196, 174, 79 , 57 ,
    411, 90 , 163, 305, 389, 378, 283, 68 , 187, 400,
    294, 198, 307, 92 , 70 , 186, 413, 176, 59 , 91 ,
    58 , 412, 380, 165, 81 , 164, 272, 175, 80 , 401,
    402, 390, 391, 197, 306, 69 , 274, 273, 379, 285,
    296, 284, 295, 188, 60 , 199, 82 , 93 , 71 , 381,
    414, 177, 166, 456, 308, 403, 98 , 76 , 286, 61 ,
    275, 386, 135, 423, 171, 102, 392, 204, 87 , 182,
    65 , 94 , 208, 124, 72 , 350, 193, 313, 393, 408,
    445, 309, 230, 419, 297, 241, 113, 219, 189, 128,
    317, 415, 116, 328, 200, 339, 382, 434, 178, 64 ,
    404, 83 , 437, 223, 134, 192, 444, 112, 439, 139,
    287, 167, 448, 212, 459, 222, 240, 233, 97 , 302,
    397, 234, 170, 276, 181, 455, 229, 438, 101, 280,
    138, 127, 298, 117, 355, 203, 426, 95 , 140, 244,
    422, 407, 213, 129, 291, 354, 105, 245, 449, 86 ,
    316, 460, 207, 353, 190, 107, 224, 427, 342, 327,
    106, 321, 118, 123, 73 , 211, 433, 218, 396, 385,
    450, 62 , 383, 349, 75 , 461, 172, 331, 168, 246,
    428, 332, 312, 201, 343, 416, 279, 63 , 195, 333,
    96 , 173, 235, 288, 320, 191, 418, 84 , 205, 100,
    67 , 394, 179, 344, 206, 338, 277, 405, 388, 395,
    301, 315, 421, 183, 293, 322, 310, 384, 410, 194,
    184, 89 , 99 , 103, 236, 78 , 88 , 77 , 136, 399,
    169, 202, 406, 125, 180, 440, 74 , 387, 242, 231,
    66 , 281, 290, 141, 314, 424, 114, 85 , 130, 356,
    119, 299, 304, 398, 237, 409, 311, 417, 292, 457,
    435, 225, 214, 209, 462, 108, 282, 446, 220, 351,
    345, 142, 247, 329, 420, 463, 318, 300, 120, 109,
    289, 451, 278, 441, 340, 303, 430, 215, 323, 226,
    334, 131, 442, 248, 335, 357, 429, 324, 143, 346,
    452, 238, 110, 216, 464, 249, 121, 431, 358, 227,
    132, 453, 336, 425, 325, 347, 126, 104, 137, 458,
    352, 243, 447, 115, 341, 210, 330, 221, 232, 436,
    465, 319, 359, 111, 454, 228, 217, 122, 443, 348,
    239, 250, 133, 144, 432, 337, 326
};

/*****************************************************************************
 变量名     : g_ashwAmrwbFrmBitsLen
 功能描述   : AMR-WB帧类型比特数数据对照表，AMR-WB各种外部速率模式下A、B、C子流对应的长度及ABC长度总和，以bit为单位
*****************************************************************************/
const VOS_UINT16 g_ashwAmrwbFrmBitsLen[11][4] =
{
    { 54, 78,  0, 132 },
    { 64, 113, 0, 177 },
    { 72, 181, 0, 253 },
    { 72, 213, 0, 285 },
    { 72, 245, 0, 317 },
    { 72, 293, 0, 365 },
    { 72, 325, 0, 397 },
    { 72, 389, 0, 461 },
    { 72, 405, 0, 477 },
    { 73, 244, 0, 317 },
    { 74, 403, 0, 477 }
};


/* 模式比特顺序对应关系综合table，为AMRWB各种速率模式下比特顺序对应关系的综合表 */
const VOS_UINT16 *g_apAmrwbBitsOrderTab[11] =
{
    g_auhwAmrwb660BitOrder,
    g_auhwAmrwb885BitOrder,
    g_auhwAmrwb1265BitOrder,
    g_auhwAmrwb1425BitOrder,
    g_auhwAmrwb1585BitOrder,
    g_auhwAmrwb1825BitOrder,
    g_auhwAmrwb1985BitOrder,
    g_auhwAmrwb2305BitOrder,
    g_auhwAmrwb2385BitOrder,
    g_auhwAmrwb1585BitOrder,
    g_auhwAmrwb2385BitOrder
};

/* AMRWB各种速率模式对应的数据长度，以bit为单位 */
const VOS_UINT16 g_ashwAmrwbBitsOrderTabLen[11] =
{
    132,                                                                        /* 对应速率6.60kbit/s码流比特长度 */
    177,                                                                        /* 对应速率8.85kbit/s码流比特长度 */
    253,                                                                        /* 对应速率12.65kbit/s码流比特长度 */
    285,                                                                        /* 对应速率14.25kbit/s码流比特长度 */
    317,                                                                        /* 对应速率15.85kbit/s码流比特长度 */
    365,                                                                        /* 对应速率18.25kbit/s码流比特长度 */
    397,                                                                        /* 对应速率19.85kbit/s码流比特长度 */
    461,                                                                        /* 对应速率23.05kbit/s码流比特长度 */
    477,                                                                         /* 对应速率23.85kbit/s码流比特长度 */
    317,                                                                        /* 对应速率15.85kbit/s码流比特长度 */
    477                                                                         /* 对应速率23.85kbit/s码流比特长度 */
};
/* AMRWB各种外部速率模式对应实际编码速率模式的映射表 */
const VOS_UINT16 g_ashwAmrwbModeMapTab[11] =
{
    CODEC_AMRWB_MODE_660,
    CODEC_AMRWB_MODE_885,
    CODEC_AMRWB_MODE_1265,
    CODEC_AMRWB_MODE_1425,
    CODEC_AMRWB_MODE_1585,
    CODEC_AMRWB_MODE_1825,
    CODEC_AMRWB_MODE_1985,
    CODEC_AMRWB_MODE_2305,
    CODEC_AMRWB_MODE_2385,
    CODEC_AMRWB_MODE_1585,
    CODEC_AMRWB_MODE_2385
};
/* 指示一次通话过程中是否收到过SID_FIRST帧 */
VOS_UINT16 g_uhwAmrwbSidFirstFlag = 0;
/* AMRWB,ABC子流长度转换表
*/
MED_UINT16 g_auwAmrWBMap[AMRWB_FRAMETYPE_NUM][AMRWB_SUB_FRM_BUTT]
    = {{54,78,0,132},
       {64,113,0,177},
       {72,181,0,253},
       {72,213,0,285},
       {72,245,0,317},
       {72,293,0,365},
       {72,325,0,397},
       {72,389,0,461},
       {72,405,0,477},
       {40,0,0,40},
       {0,0,0,0},
       {0,0,0,0},
       {0,0,0,0},
       {0,0,0,0},
       {0,0,0,0},
       {0,0,0,0}};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : AMRWB_UmtsUlConvertSpeech
 功能描述  : W上行AMR-WB比特重排及转换，将编码器输出的非紧凑语音码流分成ABC子流排列，
             同时将码流根据重排表进行重排并转换为紧凑排列码流，其中ABC紧凑子流以
             32比特进行对齐
             重排表参见协议TS 26.201附录B
 输入参数  : uhwMode             --AMR-WB速率模式
             pshwEncSerialBef    --AMR-WB编码器输出的AMRWB码流数组指针
 输出参数  : pstFrameAfterConv   --经ABC子流排列并完成反重排和紧凑压缩的码流数据结构体指针
 返 回 值  : 无
 调用函数  :
 被调函数  : AMRWB_UmtsUlConvert()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AMRWB_UmtsUlConvertSpeech(
                VOS_UINT16                  uhwMode,
                VOS_INT16                  *pshwEncSerialBef,
                VOICE_WPHY_AMR_SERIAL_STRU   *pstFrameAfterConv)
{
    VOS_UINT16              uhwALen;
    VOS_UINT16              uhwBLen;

    VOS_INT16              *pshwEncSerialAfter;
    const VOS_UINT16       *puhwBitsOrder;

    uhwALen = pstFrameAfterConv->auhwLen[0];
    uhwBLen = pstFrameAfterConv->auhwLen[1];

    pshwEncSerialAfter = (VOS_INT16 *)pstFrameAfterConv->auwData;

    /*获取对应模式比特顺序表*/
    puhwBitsOrder      = AMRWB_GetBitsOrderTabPtr(uhwMode);

    /* 转换A子帧 */
    AMRWB_UmtsUlConvertSubFrame(uhwALen,
                              puhwBitsOrder,
                              pshwEncSerialBef,
                              pshwEncSerialAfter);

    puhwBitsOrder += uhwALen;
    pshwEncSerialAfter  += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwALen, 31), 5)); /*右移5位表示32比特对齐，与2相乘表示pshwSubFrame为16比特数组指针*/

    /* 转换B子帧 */
    AMRWB_UmtsUlConvertSubFrame(uhwBLen,
                              puhwBitsOrder,
                              pshwEncSerialBef,
                              pshwEncSerialAfter);

    /* C子帧为0，不处理 */

    return CODEC_RET_OK;
}

/*****************************************************************************
 函 数 名  : AMRWB_UmtsUlConvertSubFrame
 功能描述  : UMTS网络上行AMRWB子帧根据输入的重排表进行比特重排，并将非紧凑排列的码流
             转换为紧凑排列的码流数组
 输入参数  : uhwLen               --子帧的长度
             puhwBitsOrder        --AMRWB比特排序表
             pshwEncSerialBef     --输入非紧凑排列码流数组指针
 输出参数  : pshwEncSerialAfter   --输出经重排和紧凑排列的码流数组指针
 返 回 值  : 无
 调用函数  : CODEC_ComCodecBits2Bytes()
 被调函数  : AMRWB_UmtsUlConvertSpeech()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AMRWB_UmtsUlConvertSubFrame(
                VOS_UINT16          uhwLen,
                const VOS_UINT16   *puhwBitsOrder,
                VOS_INT16          *pshwEncSerialBef,
                VOS_INT16          *pshwEncSerialAfter)
{
    VOS_UINT16  uhwCnt;
    VOS_INT16   ashwBits[AMRWB_UMTS_MAX_ABC_SUM_LENTH];                         /*AMR-WB ABC子流总和最大比特数*/

    /*检查转换长度为0，不作处理*/
    if (0 == uhwLen)
    {
        return;
    }

    /*先进行BIT重排*/
    for(uhwCnt = 0; uhwCnt < uhwLen ; uhwCnt++)
    {
        ashwBits[uhwCnt] = *(pshwEncSerialBef + puhwBitsOrder[uhwCnt]);
    }

    CODEC_ComCodecBits2Bytes(ashwBits,
                           (VOS_INT16)uhwLen,
                           (VOS_INT8 *)pshwEncSerialAfter);
}

/*****************************************************************************
 函 数 名  : AMRWB_UlConvertSidFirst
 功能描述  : SID_FIRST帧比特转换,SID FIRST帧置零，同时STI标志置为SID FIRST帧标志
 输入参数  : pstSidEncSerialAfter   --SID FIRST帧码流结构体指针
 输出参数  : pstSidEncSerialAfter   --SID FIRST帧码流结构体指针
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AMRWB_UlConvertSidFirst(
                VOS_UINT16                    uhwMode,
                AMRWB_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter)
{
    /*置SID_FIRST帧全0码流*/
    pstSidEncSerialAfter->ashwSidData1[0]   = 0;
    pstSidEncSerialAfter->ashwSidData1[1]   = 0;
    pstSidEncSerialAfter->shwSidData2       = 0;
    pstSidEncSerialAfter->shwSti            = CODEC_AMR_STI_SID_FIRST;
    pstSidEncSerialAfter->shwModeIndication = uhwMode;

}
/*****************************************************************************
 函 数 名  : AMRWB_UmtsUlConvertSid
 功能描述  : 进行SID帧数据格式的转换
 输入参数  : VOS_INT16                  *pshwEncSerial        --比特紧凑排列前码流数据
 输出参数  : AMRWB_ENCODED_SID_FRAME_STRU *pstSidEncSerialAfter --比特紧凑排列后SID UPDATE帧结构体指针
 返 回 值  : 无
 调用函数  :
 被调函数  : AMRWB_UmtsUlConvert()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AMRWB_UlConvertSidUpdate(
                VOS_UINT16                        uhwMode,
                VOS_INT16                        *pshwEncSerial,
                AMRWB_ENCODED_SID_FRAME_STRU     *pstSidEncSerialAfter)
{
    /*转换SID帧*/
    CODEC_ComCodecBits2Bytes(pshwEncSerial,
                           AMRWB_SID_UPDATE_LENGTH,
                           (VOS_INT8 *)pstSidEncSerialAfter);

    /* 1bit STI标志 */
    pstSidEncSerialAfter->shwSti = CODEC_AMR_STI_SID_UPDATE;

    /* 速率模式指示 */
    pstSidEncSerialAfter->shwModeIndication = uhwMode;
}

/*****************************************************************************
 函 数 名  : AMRWB_UmtsConvert
 功能描述  : UMTS上行AMRWB格式转换，即根据不同模式将编码后非紧凑排列的比特流按照重排表
             进行重排，并压缩为紧凑排列的码流。
             重排表重排表参见协议TS 26.201附录B
 输入参数  : pstFrameBefConv    --AMRWB编码器输出的编码数据结构体指针
 输出参数  : pstFrameAfterConv  --经比特重排及压缩后比特紧凑排列的AMRWB码流结构体指针，
                                  格式固定按如下格式排列:
                                  质量标志(占16bit)
                                  ABC子流长度(各占16bit),单位比特
                                  ABC子流数据(紧凑排列，32bit对齐，大端格式)
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  AMRWB_UmtsUlConvert(
                AMRWB_ENCODED_SERIAL_STRU  *pstFrameBefConv,
                VOICE_WPHY_AMR_SERIAL_STRU   *pstFrameAfterConv)
{

    VOS_UINT16                          uhwMode = 0;
    VOS_INT16                          *pshwEncSerialBef;
    VOS_INT16                          *pshwEncSerialAfter;
    CODEC_RET_ENUM_UINT32           enRetResult = CODEC_RET_OK;

    /*对格式转换后输出的码流数据结构体清零*/
    UCOM_MemSet(pstFrameAfterConv, 0, sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /*设置转换后AMRWB码流的帧质量标志为为好帧*/
    pstFrameAfterConv->enQualityIdx = CODEC_AMR_FQI_QUALITY_GOOD;

    /*从输入参数中获取AMRWB速率模式及转换前码流指针*/
    uhwMode            = (VOS_UINT16)pstFrameBefConv->enAmrMode;
    pshwEncSerialBef   = pstFrameBefConv->ashwEncSerial;

    /*获取转换后码流指针*/
    pshwEncSerialAfter = (VOS_INT16 *)pstFrameAfterConv->auwData;

    /*判断编码器输出的帧类型*/
    switch (pstFrameBefConv->uhwFrameType)
    {
        case CODEC_AMRWB_TYPE_TX_SPEECH_GOOD:
        {
            /*写入SPEECH_GOOD帧的ABC三个子流的长度*/
            pstFrameAfterConv->auhwLen[0] = AMRWB_GetNumOfFrmBits(uhwMode, AMRWB_SUB_FRM_A);
            pstFrameAfterConv->auhwLen[1] = AMRWB_GetNumOfFrmBits(uhwMode, AMRWB_SUB_FRM_B);
            pstFrameAfterConv->auhwLen[2] = AMRWB_GetNumOfFrmBits(uhwMode, AMRWB_SUB_FRM_C);

            /*如果是语音帧进行比特转换*/
            enRetResult = AMRWB_UmtsUlConvertSpeech(uhwMode,
                                                    pshwEncSerialBef,
                                                    pstFrameAfterConv);

            break;
        }

        case CODEC_AMRWB_TYPE_TX_SID_FIRST:
        {
            /*写入SID_FIRST帧的ABC三个子流的长度*/
            pstFrameAfterConv->auhwLen[0] = AMRWB_SID_A_LENGTH;
            pstFrameAfterConv->auhwLen[1] = AMRWB_SID_B_LENGTH;
            pstFrameAfterConv->auhwLen[2] = AMRWB_SID_C_LENGTH;

            /*对SID_FIRST帧进行比特转换*/
            AMRWB_UlConvertSidFirst(uhwMode,
                                    (AMRWB_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);
            break;
        }

        case CODEC_AMRWB_TYPE_TX_SID_UPDATE:
        {
            /*写入SID_UPDATE帧的ABC三个子流的长度*/
            pstFrameAfterConv->auhwLen[0] = AMRWB_SID_A_LENGTH;
            pstFrameAfterConv->auhwLen[1] = AMRWB_SID_B_LENGTH;
            pstFrameAfterConv->auhwLen[2] = AMRWB_SID_C_LENGTH;

            /*对SID_UPDATE帧进行比特转换*/
            AMRWB_UlConvertSidUpdate(uhwMode,
                                     pshwEncSerialBef,
                                     (AMRWB_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);

            break;
        }

        default: /*包括NO_DATA的情况*/
        {
            break;
        }
    }

    return enRetResult;
}

/*****************************************************************************
 函 数 名  : AMRWB_UmtsDlConvertSpeech
 功能描述  : UMTS下行AMRWB比特反重排
 输入参数  : uhwMode             --AMRWB速率模式
             pstDecSerialBef     --比特转换及反重排前WPHY发送过来的紧凑码流数据结构体指针
                                 格式固定按如下格式排列:
                                 质量标志(占16bit)
                                 ABC子流长度(各占16bit),单位比特
                                 ABC子流数据(紧凑排列，32bit对齐)
 输出参数  : pstDecSerialAfter   --比特转换及反重排非紧凑码流数据结构体指针
 返 回 值  : 无
 调用函数  : AMRWB_UmtsDlConvertSubFrame()
 被调函数  : AMRWB_UmtsDlConvertGoodFrame()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AMRWB_UmtsDlConvertSpeech(
                VOS_UINT16                    uhwMode,
                VOICE_WPHY_AMR_SERIAL_STRU     *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU    *pstDecSerialAfter)
{
    VOS_UINT16              uhwALen;
    VOS_UINT16              uhwBLen;

    VOS_INT16                   *pshwSerialBef;
    VOS_INT16                   *pshwSerialAfter;

    const VOS_UINT16            *puhwBitsOrder;

    pshwSerialBef   = (VOS_INT16 *)pstDecSerialBef->auwData;
    pshwSerialAfter = pstDecSerialAfter->ashwEncSerial;

    /*获取全局变量不同速率模式比特排序表指针*/
    puhwBitsOrder = AMRWB_GetBitsOrderTabPtr(uhwMode);

    /*设置帧类型为好的语音帧*/
    pstDecSerialAfter->uhwFrameType = CODEC_AMRWB_TYPE_RX_SPEECH_GOOD;

    /* 填充AMRWB速率模式 */
    pstDecSerialAfter->enAmrMode    = uhwMode;

    /*获取ABC子流长度*/
    uhwALen = pstDecSerialBef->auhwLen[0];
    uhwBLen = pstDecSerialBef->auhwLen[1];

    /* 转换子帧A */
    AMRWB_UmtsDlConvertSubFrame(uhwALen,
                                puhwBitsOrder,
                                pshwSerialBef,
                                pshwSerialAfter);

    puhwBitsOrder   += uhwALen;
    pshwSerialBef   += (2 * CODEC_OpShr(CODEC_OpAdd((VOS_INT16)uhwALen, 31), 5)); /*右移5位表示32比特对齐，与2相乘表示pshwSubFrame为16比特数组指针*/

    /* 转换子帧B */
    AMRWB_UmtsDlConvertSubFrame(uhwBLen,
                                puhwBitsOrder,
                                pshwSerialBef,
                                pshwSerialAfter);

    /* 子帧C长度0 */

}

/*****************************************************************************
 函 数 名  : AMRWB_UmtsDlConvertSubFrame
 功能描述  : 将UMTS网络下行AMRWB子帧紧凑码流转换为一个比特占一个字的非紧凑型码流，
             并根据协议TS 26.201附录B重排表进行重排
 输入参数  : uhwLen             --子帧的长度
             puhwBitsOrder      --AMRWB比特排序表
             pshwSerialBef      --比特转换及反重排前数据
 输出参数  : pshwSerialAfter    --比特转换及反重排后数据
 返 回 值  : 无
 调用函数  :
 被调函数  : AMRWB_UmtsDlConvertSpeech()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AMRWB_UmtsDlConvertSubFrame(
                VOS_UINT16              uhwLen,
                const VOS_UINT16       *puhwBitsOrder,
                VOS_INT16              *pshwSerialBef,
                VOS_INT16              *pshwSerialAfter)
{
    VOS_UINT16    uhwCnt;
    VOS_UINT16    uhwIndex;
    VOS_INT16     ashwBits[AMRWB_UMTS_MAX_ABC_SUM_LENTH];                       /*AMR-WB ABC子流总和最大比特数*/

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
    for (uhwCnt = 0; uhwCnt < uhwLen ; uhwCnt++)
    {
        uhwIndex                  = puhwBitsOrder[uhwCnt];
        pshwSerialAfter[uhwIndex] = ashwBits[uhwCnt];
    }

}

/*****************************************************************************
 函 数 名  : AMRWB_UmtsDlConvertSid
 功能描述  : UMTS网络下行AMRWB SID帧比特反转
 输入参数  : pstSidEncSerial     --SID帧比特转换及反重排前紧凑排列码流数据结构体指针
 输出参数  : pstDecSerialAfter   --比特转换及反重排非紧凑码流数据结构体指针
 返 回 值  : 无
 调用函数  :
 被调函数  : AMRWB_UmtsDlConvertGoodFrame()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AMRWB_UmtsDlConvertSid(
                AMRWB_ENCODED_SID_FRAME_STRU  *pstSidEncSerial,
                AMRWB_ENCODED_SERIAL_STRU     *pstDecSerialAfter)
{
    VOS_UINT16   uhwSidFirstFlag               = 0;
    VOS_UINT16  *puhwSidFirstFlag              = VOS_NULL;

    /*获取全局变量AMRWB一次通话过程中是否收到过SID_FIRST帧标志,并初始化为0*/
    puhwSidFirstFlag  = AMRWB_GetSidFirstFlagPtr();
    uhwSidFirstFlag   = *puhwSidFirstFlag;

    pstDecSerialAfter->enAmrMode = CODEC_AMRWB_MODE_MRDTX;

    /*STI_SID_Update帧码流解析*/
    if (CODEC_AMR_STI_SID_UPDATE == pstSidEncSerial->shwSti)
    {
        /* 如果本次通话中未出现SID_FIRST,则把本帧当NO_DATA处理 */
        if (uhwSidFirstFlag != 0)
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMRWB_TYPE_RX_SID_UPDATE;
        }
        else
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMRWB_TYPE_RX_SID_BAD;
            return;
        }

        /*转换SID帧*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pstSidEncSerial,
                                AMRWB_SID_UPDATE_LENGTH,
                                pstDecSerialAfter->ashwEncSerial);
    }
    else  /* STI_SID_FIRST帧 */
    {
        pstDecSerialAfter->uhwFrameType = CODEC_AMRWB_TYPE_RX_SID_FIRST;
        *puhwSidFirstFlag               = 1;
    }
}

/*****************************************************************************
 函 数 名  : AMRWB_UmtsDlConvertGoodFrame
 功能描述  : AMRWB下行GOOD帧转换处理，即根据A子流长度判断当前帧类型，
             并针对不同帧类型调用对应帧类型码流转换函数
 输入参数  : pstDecSerialBef   --比特转换及反重排前WPHY发送过来的紧凑码流数据结构体指针
                                 格式固定按如下格式排列:
                                 质量标志(占16bit)
                                 ABC子流长度(各占16bit),单位比特
                                 ABC子流数据(紧凑排列，32bit对齐)
 输出参数  : pstDecSerialAfter --比特转换及反重排非紧凑码流数据结构体指针
 返 回 值  : void
 调用函数  : AMRWB_UmtsDlConvertSid()
             AMRWB_UmtsDlConvertSpeech()
 被调函数  : AMRWB_UmtsDlConvert()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AMRWB_UmtsDlConvertGoodFrame(
                VOICE_WPHY_AMR_SERIAL_STRU    *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
{
    VOS_UINT16                  uhwMode    = 0;
    VOS_UINT16                  uhwALength = 0;
    VOS_UINT16                  uhwBLength = 0;
    VOS_UINT16                  uhwCLength   = 0;
    VOS_UINT16                  uhwABCLength = 0;

    VOS_INT16                  *pshwEncSerial;

    /*从输入数据中获取A/B子流的长度和码流指针*/
    uhwALength    = pstDecSerialBef->auhwLen[0];
    uhwBLength    = pstDecSerialBef->auhwLen[1];
    uhwCLength    = pstDecSerialBef->auhwLen[2];
    pshwEncSerial = (VOS_INT16 *)pstDecSerialBef->auwData;

    /*计算ABC子流长度总和*/
    uhwABCLength = uhwALength + uhwBLength + uhwCLength;


    if (AMRWB_SID_A_LENGTH == uhwALength)
    {
        /*SID帧码流解析*/
        AMRWB_UmtsDlConvertSid((AMRWB_ENCODED_SID_FRAME_STRU *)pshwEncSerial,
                             pstDecSerialAfter);
    }
    else
    {
        /*根据ABC子流长度总和查表搜索速率模式*/
        for (uhwMode = 0; uhwMode <= CODEC_AMRWB_MODE_2385; uhwMode++)
        {
            if (uhwABCLength == AMRWB_GetNumOfFrmBits(uhwMode, AMRWB_SUB_FRM_ABC))
            {
                break;
            }
        }

        /*若未查到对应速率模式，则判为NO DATA帧*/
        if (uhwMode > CODEC_AMRWB_MODE_2385)
        {
            pstDecSerialAfter->uhwFrameType = CODEC_AMRWB_TYPE_RX_NO_DATA;


            pstDecSerialAfter->enAmrMode    = CODEC_AMRWB_MODE_MRDTX;
        }
        else /* 正常的AMRWB语音帧 */
        {
            *AMRWB_GetSidFirstFlagPtr() = 0;
            AMRWB_UmtsDlConvertSpeech(uhwMode, pstDecSerialBef, pstDecSerialAfter);
        }
    }

}

/*****************************************************************************
 函 数 名  : AMRWB_UmtsDlConvertBadFrame
 功能描述  : W下行AMRWB解码坏帧格式转换
 输入参数  : pstDecSerialBef      --比特转换及反重排前WPHY发送过来的紧凑码流数据结构体指针
                                 格式固定按如下格式排列:
                                 质量标志(占16bit)
                                 ABC子流长度(各占16bit),单位比特
                                 ABC子流数据(紧凑排列，32bit对齐)
 输出参数  : pstDecSerialAfter    --比特转换及反重排非紧凑码流数据结构体指针
 返 回 值  : 无
 调用函数  :
 被调函数  : AMRWB_UmtsDlConvert()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AMRWB_UmtsDlConvertBadFrame(
                VOICE_WPHY_AMR_SERIAL_STRU     *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU    *pstDecSerialAfter)
{
    VOS_UINT16               uhwAmrLength;


    /* 从输入数据中获取A子流的长度 */
    uhwAmrLength  = pstDecSerialBef->auhwLen[0];

    if (uhwAmrLength >= AMRWB_MR660_A_LENGTH)
    {
        /*speech坏帧解析*/
        pstDecSerialAfter->uhwFrameType = CODEC_AMRWB_TYPE_RX_SPEECH_BAD;
    }
    else if (AMRWB_SID_A_LENGTH == uhwAmrLength)
    {
        pstDecSerialAfter->uhwFrameType = CODEC_AMRWB_TYPE_RX_SID_BAD;

        /* 设置模式指示 */
        pstDecSerialAfter->enAmrMode    = CODEC_AMRWB_MODE_660;
    }
    else
    {
        /*NO DATA帧解析*/
        pstDecSerialAfter->uhwFrameType = CODEC_AMRWB_TYPE_RX_NO_DATA;

        /*设置模式指示*/
        pstDecSerialAfter->enAmrMode    = CODEC_AMRWB_MODE_660;
    }
}


/*****************************************************************************
 函 数 名  : AMRWB_UmtsDlConvert
 功能描述  : 根据解码质量标志对W下行AMRWB解码格式转换；
             将紧凑排列的码流数据转换为非紧凑排列数据，并根据重排表进行反重排；
             重排表重排表参见协议TS 26.201附录B
 输入参数  : pstDecSerialBef   --比特转换及反重排前WPHY发送过来的紧凑码流数据结构体指针
                                 格式固定按如下格式排列:
                                 质量标志(占16bit)
                                 ABC子流长度(各占16bit),单位比特
                                 ABC子流数据(紧凑排列，32bit对齐)
 输出参数  : pstDecSerialAfter  --比特转换及反重排非紧凑码流数据结构体指针
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
 调用函数  : AMRWB_UmtsDlConvertGoodFrame()
             AMRWB_UmtsDlConvertBadFrame()
 被调函数  : AMRWB_Decode()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  AMRWB_UmtsDlConvert(
                VOICE_WPHY_AMR_SERIAL_STRU    *pstDecSerialBef,
                AMRWB_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
{
    CODEC_AMR_FQI_QUALITY_ENUM_UINT16    enAmrQualityIndication;

    enAmrQualityIndication = pstDecSerialBef->enQualityIdx;

    /* 对比特转换及反重排输出的码流结构体进行清零 */
    UCOM_MemSet(pstDecSerialAfter, 0, sizeof(AMRWB_ENCODED_SERIAL_STRU));

    if (CODEC_AMR_FQI_QUALITY_GOOD == enAmrQualityIndication)
    {
        AMRWB_UmtsDlConvertGoodFrame(pstDecSerialBef, pstDecSerialAfter);
    }
    else
    {
        AMRWB_UmtsDlConvertBadFrame(pstDecSerialBef, pstDecSerialAfter);
    }

    return CODEC_RET_OK;
}

/*****************************************************************************
 函 数 名  : AMRWB_GsmUlConvert
 功能描述  : GSM下AMRWB上行比特转换，即根据不同模式将编码后非紧凑排列的比特流按照重排表
             进行重排，并压缩为紧凑排列的码流。
             重排表重排表参见协议TS 26.201附录B
 输入参数  : pstFrameBefConv       --AMRWB编码器输出的编码数据结构体指针
 输出参数  : pshwEncSerialAfter    --经比特重排及压缩后比特紧凑排列的AMRWB码流数组指针
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
 调用函数  :
 被调函数  : AMRWB_Encode()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AMRWB_GsmUlConvert(
                AMRWB_ENCODED_SERIAL_STRU *pstFrameBefConv,
                VOS_INT16               *pshwEncSerialAfter)
{
    VOS_UINT16                 uhwCnt;
    VOS_UINT16                 uhwMode;
    VOS_UINT16                 uhwLen;
    VOS_UINT16                 uhwIndex;
    const VOS_UINT16          *puhwBitOrderTable;


    VOS_INT16                  ahswEncSerialSorted[AMRWB_MAX_SERIAL_SIZE];
    VOS_INT16                 *pshwEncSerial;

    /*对比特重排后输出ahswEncSerialSorted[]数据清零*/
    CODEC_OpVecSet(ahswEncSerialSorted, AMRWB_MAX_SERIAL_SIZE, 0);

    /*对输出紧凑码流数组进行清零*/
    CODEC_OpVecSet(pshwEncSerialAfter, AMRWB_NUM_WORDS_OF_ENC_SERIAL, 0);

    /*获取编码模式和比特转换前码流指针*/
    uhwMode         = (VOS_UINT16)pstFrameBefConv->enAmrMode;
    pshwEncSerial   = pstFrameBefConv->ashwEncSerial;

    /* 根据编解码编码结果分类进行比特重排处理 */
    if (CODEC_AMRWB_TYPE_TX_SPEECH_GOOD == pstFrameBefConv->uhwFrameType)
    {
        /*获取对应模式比特排序表和比特长度*/
        puhwBitOrderTable = AMRWB_GetBitsOrderTabPtr(uhwMode);
        uhwLen            = AMRWB_GetBitsLenOfMode(uhwMode);

        /*根据比特顺序表对比特流进行比特重排*/
        for (uhwCnt = 0; uhwCnt < uhwLen; uhwCnt++)
        {
            uhwIndex                    = puhwBitOrderTable[uhwCnt];
            ahswEncSerialSorted[uhwCnt] = pshwEncSerial[uhwIndex];
        }

        /*将重排后比特流压缩为紧凑比特流*/
        CODEC_ComCodecBits2Bytes(ahswEncSerialSorted,
                               (VOS_INT16)uhwLen,
                               (VOS_INT8 *)pshwEncSerialAfter);
    }
    else if (CODEC_AMRWB_TYPE_TX_SID_UPDATE == pstFrameBefConv->uhwFrameType)
    {
        /*获取SID_UPDATE帧码流*/
        AMRWB_UlConvertSidUpdate(uhwMode,
                                 pshwEncSerial,
                                 (AMRWB_ENCODED_SID_FRAME_STRU *)pshwEncSerialAfter);
    }
    else
    {
        /* 包括SID_FIRST及NO_DATA等其它情况，需对pAmrFrame做清零处理 */
        /* 由于之前已经预先全部清零，此处无需再次进行 */
    }

    return CODEC_RET_OK;
}


/*****************************************************************************
 函 数 名  : AMRWB_GsmDlConvert
 功能描述  : GSM下AMRWB下行比特反重排
 输入参数  : pshwAmrFrame     --比特转换及反重排前数据
             enRxType         --AMRWB语音帧类型
             enMode           --AMRWB速率模式
 输出参数  : pshwDecodeSpeech --比特转换及反重排后数据
 返 回 值  : 成功返回CODEC_RET_OK
             失败返回CODEC_RET_ERR
 调用函数  :
 被调函数  : AMRWB_Decode()

 修改历史      :
  1.日    期   : 2011年11月15日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AMRWB_GsmDlConvert(
                VOS_INT16                           *pshwAmrFrame,
                CODEC_AMRWB_TYPE_RX_ENUM_UINT16  enRxType,
                CODEC_AMRWB_MODE_ENUM_UINT16     enMode,
                VOS_INT16                           *pshwDecSerial)
{
    VOS_UINT16           uhwCnt;
    VOS_UINT16           uhwIndex;
    VOS_UINT16           uhwBitsLength;

    VOS_INT16            ashwEncSerial[AMRWB_MAX_SERIAL_SIZE];

    const VOS_UINT16    *puhwBitsOrder;

    /*对非紧凑逐比特排序序列进行清零*/
    CODEC_OpVecSet(ashwEncSerial, AMRWB_MAX_SERIAL_SIZE, 0);

    /*对比特转换及反重排输出的数据进行清零*/
    CODEC_OpVecSet(pshwDecSerial, AMRWB_MAX_SERIAL_SIZE, 0);

    /* 根据外部输入的RX_FrameType判断是否是语音帧还是非语音帧 */
    if (   (CODEC_AMRWB_TYPE_RX_SPEECH_GOOD == enRxType)
        || (CODEC_AMRWB_TYPE_RX_SPEECH_DEGRADED == enRxType))
    {
        if (enMode >= CODEC_AMRWB_MODE_MRDTX)
        {
            return CODEC_RET_ERR;
        }

        /*获取全局变量不同速率模式比特排序表指针和比特长度*/
        puhwBitsOrder = AMRWB_GetBitsOrderTabPtr(enMode);
        uhwBitsLength = AMRWB_GetBitsLenOfMode(enMode);

        /*将紧凑型码流转成非紧凑逐比特排列*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pshwAmrFrame,
                               (VOS_INT16)uhwBitsLength,
                               ashwEncSerial);

        /*对该比特流进行比特反重排，输出至pshwDecSerial*/
        for (uhwCnt = 0; uhwCnt < uhwBitsLength; uhwCnt++)
        {
            uhwIndex                = puhwBitsOrder[uhwCnt];
            pshwDecSerial[uhwIndex] = ashwEncSerial[uhwCnt];
        }
    }
    else if (CODEC_AMRWB_TYPE_RX_SID_UPDATE == enRxType)
    {
        /*将紧凑型码流转成非紧凑逐比特排列*/
        CODEC_ComCodecBytes2Bits((VOS_INT8 *)pshwAmrFrame,
                                AMRWB_SID_UPDATE_LENGTH,
                                ashwEncSerial);

        /*获取35比特的数据内容，无需进行反重排，输出至pDecodeSpeech*/
        CODEC_OpVecCpy(pshwDecSerial, ashwEncSerial, AMRWB_SID_UPDATE_LENGTH);
    }
    else
    {
        /* 其他情况下声码器不使用帧内内容数据，因此不做处理 */
    }

    return CODEC_RET_OK;
}
VOS_UINT32 AMRWB_ImsDlConvert(IMS_DL_DATA_OBJ_STRU *pstSrc,AMRWB_ENCODED_SERIAL_STRU   *pstDecSerialAfter)
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
    if ((IMSA_VOICE_ERROR == pstSrc->usErrorFlag) || (AMRWB_FRAMETYPE_NUM <= pstSrc->usFrameType))
    {
        /* 填充成nodata帧 */
        /* ABC子流长度 */
        stImsDlData.auhwLen[AMRWB_SUB_FRM_A] = 0;
        stImsDlData.auhwLen[AMRWB_SUB_FRM_B] = 0;
        stImsDlData.auhwLen[AMRWB_SUB_FRM_C] = 0;
        stImsDlData.enQualityIdx = pstSrc->usQualityIdx;
    }
    else
    {
        /*****************填充hifi侧amr帧结构*****************/
        /* 填充帧头 */
        /* ABC子流长度 */
        stImsDlData.auhwLen[AMRWB_SUB_FRM_A] = g_auwAmrWBMap[pstSrc->usFrameType][AMRWB_SUB_FRM_A];
        stImsDlData.auhwLen[AMRWB_SUB_FRM_B] = g_auwAmrWBMap[pstSrc->usFrameType][AMRWB_SUB_FRM_B];
        stImsDlData.auhwLen[AMRWB_SUB_FRM_C] = g_auwAmrWBMap[pstSrc->usFrameType][AMRWB_SUB_FRM_C];
        stImsDlData.enQualityIdx = pstSrc->usQualityIdx;

        /* 填充数据域 */
        AMRWB_FilledImsDlData((VOS_UINT8 *)pstSrc->ausData,&stImsDlData);

        /* 对SID进行处理 */
        if(AMRWB_SID_A_LENGTH == stImsDlData.auhwLen[AMRWB_SUB_FRM_A])
        {
            puchData    = (MED_UINT8*)&stImsDlData.auwData[0];
            uchRateMode = (puchData[4]&0x0f)<<4;      /*ModeIndication:4*/
            uchSti      = (puchData[4]&0x10)>>1;      /*Sti:1*/
            uchBitLeft  = (puchData[4]&0xe0)>>5;      /*SidData2:3*/
            uchTemp     = (uchRateMode|uchSti)|(uchBitLeft);
            puchData[4] = uchTemp;
        }
    }

    /*将紧凑排列的码流数据转换为非紧凑排列数据，并根据重排表进行反重排*/
    AMRWB_UmtsDlConvert(&stImsDlData,
                        pstDecSerialAfter);

    return CODEC_RET_OK;
}
VOS_VOID AMRWB_ImsUlConvert(VOS_UINT16 uhwAMRType,VOS_UINT16 uhwMode,VOS_INT16* pstEncSerial)
{
    IMS_UL_DATA_OBJ_STRU stUlDataObj;
    VOS_UINT16           uhwFrameType;
    VOS_UINT8           *puchData = NULL;

    UCOM_MemSet(&stUlDataObj,0,sizeof(IMS_UL_DATA_OBJ_STRU));

    puchData = (VOS_UINT8*)stUlDataObj.ausData;

    /*****************填充消息*****************/
    /*****************填充IMSA侧amr帧结构*****************/
    /* 填充数据域 */
    if(CODEC_AMR_TYPE_TX_NO_DATA != uhwAMRType)
    {
        AMRWB_FilledImsUlData(&puchData[0],(VOICE_WPHY_AMR_SERIAL_STRU*)pstEncSerial);
    }

    /* 获取帧类型 */
    switch(uhwAMRType)//pstEncodedData->enAmrFrameType)
    {
        /* SID帧 */
        case CODEC_AMR_TYPE_TX_SID_FIRST:
        {
            uhwFrameType = AMRWB_FRAMETYPE_SID;
            puchData[4]  = (puchData[4]&0xe0)|((VOS_UINT8)uhwMode);
        }break;
        case CODEC_AMR_TYPE_TX_SID_UPDATE:
        {
            uhwFrameType = AMRWB_FRAMETYPE_SID;
            puchData[4]  = (puchData[4]&0xe0)|(0x10|((VOS_UINT8)uhwMode));

        }break;
        /* NOData帧 */
        case CODEC_AMR_TYPE_TX_NO_DATA:
        {
            uhwFrameType = AMRWB_FRAMETYPE_NODATA;
        }break;
        /* speech帧 */
        default:
        {
            uhwFrameType = (VOS_UINT8)(uhwMode);
        }break;
    }

    stUlDataObj.usStatus    = uhwAMRType;   /* 语音帧还是SID帧，好帧还是坏帧 */
    stUlDataObj.usFrameType = uhwFrameType; /* 帧类型 */

    /* 重置Encode Buffer数据，置成上行IMS数据结构 */
    UCOM_MemSet(pstEncSerial,0,sizeof(VOS_UINT16)*VOICE_CODED_FRAME_LENGTH);
    UCOM_MemCpy(&pstEncSerial[0],&stUlDataObj,sizeof(IMS_UL_DATA_OBJ_STRU));

}


VOS_VOID AMRWB_FilledImsDlData( VOS_UINT8 *puchData,VOICE_WPHY_AMR_SERIAL_STRU *pstDes)
{
    VOS_UINT8*  puchSrc;
    VOS_UINT32* puwADes;
    VOS_UINT32* puwBDes;
    VOS_UINT32* puwCDes;

    /* A B C码流长度 */
    VOS_UINT32 uwALen = pstDes->auhwLen[AMRWB_SUB_FRM_A];
    VOS_UINT32 uwBLen = pstDes->auhwLen[AMRWB_SUB_FRM_B];
    VOS_UINT32 uwCLen = pstDes->auhwLen[AMRWB_SUB_FRM_C];

    VOS_UINT32 uwCurBit = 0;   /* 当前bit计数 */

    puchSrc = puchData; /* 去掉帧头后的数据域 */

    /* 计算需要填充的A B C码流起始地址,32bit对齐 */
    puwADes = &(pstDes->auwData[0]);
    puwBDes = ((0==(uwALen%32))?(puwADes + (uwALen/32)):(puwADes + (uwALen/32) + 1));
    puwCDes = ((0==(uwBLen%32))?(puwBDes + (uwBLen/32)):(puwBDes + (uwBLen/32) + 1));

    /* 填充A码流 */
    AMRWB_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwADes,&uwCurBit,uwALen);

    /* 填充B码流 */
    AMRWB_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwBDes,&uwCurBit,uwBLen);

    /* 填充C码流 */
    AMRWB_ImsDlBitFilled(puchSrc,(VOS_UINT8*)puwCDes,&uwCurBit,uwCLen);

}
VOS_VOID AMRWB_FilledImsUlData( VOS_UINT8   *puchImsaData,
                                        VOICE_WPHY_AMR_SERIAL_STRU* pstHifiData)
{
    VOS_UINT8*  puchDes;
    VOS_UINT32* puwASrc;
    VOS_UINT32* puwBSrc;
    VOS_UINT32* puwCSrc;

    /* A B C码流长度 */
    VOS_UINT32 uwALen = (VOS_UINT32)pstHifiData->auhwLen[AMRWB_SUB_FRM_A];
    VOS_UINT32 uwBLen = (VOS_UINT32)pstHifiData->auhwLen[AMRWB_SUB_FRM_B];
    VOS_UINT32 uwCLen = (VOS_UINT32)pstHifiData->auhwLen[AMRWB_SUB_FRM_C];

    VOS_UINT32 uwCurBit = 0;   /* 当前bit计数 */

    /* 计算需要填充的A B C码流起始地址,32bit对齐 */
    puwASrc = &(pstHifiData->auwData[0]);
    puwBSrc = ((0 == (uwALen%32))?(puwASrc + (uwALen/32)):(puwASrc + (uwALen/32) + 1));
    puwCSrc = ((0 == (uwBLen%32))?(puwBSrc + (uwBLen/32)):(puwBSrc + (uwBLen/32) + 1));

    /* 填充的目的地址 */
    puchDes = puchImsaData;

    /* 填充A码流 */
    AMRWB_ImsUlBitFilled((VOS_UINT8*)puwASrc,puchDes,&uwCurBit,uwALen);

    /* 填充B码流 */
    AMRWB_ImsUlBitFilled((VOS_UINT8*)puwBSrc,puchDes,&uwCurBit,uwBLen);

    /* 填充C码流 */
    AMRWB_ImsUlBitFilled((VOS_UINT8*)puwCSrc,puchDes,&uwCurBit,uwCLen);
}
VOS_VOID AMRWB_ImsUlBitFilled(VOS_UINT8* puchSrc,VOS_UINT8* puchDes,
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
        puchDes[(*puwCurBit)/8] = (uchVal)|(puchDes[(*puwCurBit)/8]);
        (*puwCurBit) += 1;
    }
}


VOS_VOID AMRWB_ImsDlBitFilled(VOS_UINT8* puchSrc,VOS_UINT8* puchDes,
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



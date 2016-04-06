

/*****************************************************************************
   1 头文件包含
******************************************************************************/
#include "codec_op_lib.h"


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
   2 全局变量定义
******************************************************************************/
/* 求2的幂次方查找表 */
const VOS_INT16 g_ashwOpLExp2Table[33] =
    {16384, 16743, 17109, 17484, 17867, 18258, 18658, 19066, 19484, 19911, 20347,
     20792, 21247, 21713, 22188, 22674, 23170, 23678, 24196, 24726, 25268, 25821,
     26386, 26964, 27554, 28158, 28774, 29405, 30048, 30706, 31379, 32066, 32767};

/* 计算log2的查找表*/
const VOS_INT16 g_ashwOpLog2Tab0[10] =
    {-32768,-28262,-24149,-20365,-16862,-13600,-10549,-7683,-4981,-2425};

/* 计算log2的查找表 */
const VOS_INT16 g_ashwOpLog2Tab1[10] =
    {22529,20567,18920,17517,16308,15255,14330,13511,12780,12124};

/* 计算log2的查找表 */
const VOS_INT16 g_ashwOpLog2Tab2[10] =
    {16384,18022,19660,21299,22938,24576,26214,27853,29491,31130};

/* 计算积分的查找表 */
const MED_INT32 g_aswIntegralTableY1[50] =
        {6563315, 4522600, 3146528, 2206410, 1557338, 1105315, 788235, 564444, 405657, 292476,
         211477 , 153303 , 111389 , 81104  , 59167  , 43239  , 31650 , 23201 , 17031 , 12518 ,
         9212   , 6786   , 5005   , 3694   , 2729   , 2018   , 1494  , 1106  , 820   , 608   ,
         451    , 335    , 249    , 185    , 138    , 103    , 76    , 57    , 42    , 32    ,
         24     , 18     , 13     , 10     , 7      , 5      , 4     , 3     , 2     , 2};

const MED_INT32 g_aswIntegralTableY2[50] =
        {164101378, 146227921, 131440079, 118951501, 108238180, 98933517, 90771118, 83551410, 77121101, 71359962,
         66172019 , 61479485 , 57218485 , 53335960 , 49787387 , 46535069, 43546832, 40795019, 38255696, 35908033,
         33733804 , 31716980 , 29843402 , 28100514 , 26477135 , 24963277, 23549983, 22229205, 20993681, 19836851,
         18752765 , 17736023 , 16781706 , 15885328 , 15042790 , 14250338, 13504529, 12802202, 12140447, 11516583,
         10928138 , 10372826 , 9848536  , 9353311  , 8885336  , 8442928 , 8024523 , 7628668 , 7254008 , 6899282};

const MED_INT32 g_aswIntegralTableY3[50] =
        {331235728, 323807468, 316809348, 310196390, 303930316, 297978277, 292311871, 286906380, 281740154, 276794126,
         272051416, 267497010, 263117494, 258900832, 254836189, 250913770, 247124698, 243460895, 239914995, 236480261,
         233150514, 229920073, 226783707, 223736583, 220774229, 217892501, 215087549, 212355790, 209693886, 207098719,
         204567376, 202097127, 199685413, 197329832, 195028125, 192778166, 190577950, 188425587, 186319291, 184257373,
         182238234, 180260360, 178322314, 176422733, 174560321, 172733847, 170942137, 169184075, 167458598, 165764688};

const MED_INT32 g_aswIntegralTableY4[100] =
        {945951990, 852049528, 797384697, 758684010, 728707625, 704241276, 683573653, 665684551, 649916514, 635820943,
         623078040, 611451780, 600762986, 590872422, 581669738, 573065996, 564988461, 557376895, 550180843, 543357628,
         536870825, 530689102, 524785308, 519135763, 513719689, 508518758, 503516715, 498699084, 494052910, 489566561,
         485229546, 481032377, 476966442, 473023898, 469197588, 465480959, 461867996, 458353167, 454931369, 451597891,
         448348366, 445178746, 442085269, 439064432, 436112967, 433227824, 430406147, 427645263, 424942663, 422295991,
         419703030, 417161694, 414670013, 412226131, 409828292, 407474837, 405164192, 402894868, 400665451, 398474598,
         396321035, 394203547, 392120979, 390072230, 388056252, 386072042, 384118645, 382195146, 380300673, 378434389,
         376595494, 374783221, 372996835, 371235631, 369498933, 367786091, 366096480, 364429501, 362784576, 361161150,
         359558688, 357976676, 356414617, 354872033, 353348462, 351843459, 350356595, 348887454, 347435636, 346000752,
         344582429, 343180303, 341794025, 340423255, 339067664, 337726933, 336400756, 335088832, 333790872, 332506595};

/*****************************************************************************
   3 函数实现
*****************************************************************************/

VOS_INT32 CODEC_OpL_Exp2(
                VOS_INT16               shwExponent,
                VOS_INT16               shwFraction,
                VOS_INT16               shwQn)
{
    /* 将小数部分[0.0, 1.0)均匀划分为32等分, 每一段的起始值的幂次方作为该段输出。*/
    const VOS_INT16 *pshwTable = g_ashwOpLExp2Table;

    /* 临时变量 */
    VOS_INT32       swX;
    VOS_INT16       shwIndex;
    VOS_INT16       shwA;
    VOS_INT16       shwTemp;
    VOS_INT16       shwExp;

    /* 返回值 */
    VOS_INT32       swExp2Out;

    /* 根据小数幂值计算表的地址:(shwFraction*32)>>15*/
    swX      = CODEC_OpL_shl((VOS_INT32)shwFraction, 6);
    shwIndex = CODEC_OpExtract_h(swX);

    /* 取出幂的小数部分的bit0-bit9 */
    swX = CODEC_OpL_shr(swX, 1);
    shwA = CODEC_OpExtract_l(swX);
    shwA = shwA & 0x7FFF;

    swX = CODEC_OpL_deposit_h( pshwTable[shwIndex] );
    shwTemp = CODEC_OpSub( pshwTable[shwIndex], pshwTable[shwIndex + 1] );

    /* swX = swX - shwTemp*shwA*2 */
    swX = CODEC_OpL_msu(swX, shwTemp, shwA);

    shwExp = 30 - shwExponent;

    /* 归一化到Qn */
    swExp2Out = CODEC_OpL_shl(swX, shwQn - shwExp);

    return(swExp2Out);
}
VOS_INT16 MED_PRECISE_OpLog2(
                VOS_INT16               shwAmIn,
                VOS_INT16               shwQn)
{
    /* 存储每段起点对数值的表。10点对数表，对应输入为[0.5, 1.0],每一段的对数值都取第1点的对数值 */
    VOS_INT16 *pshwLogTab0 = (VOS_INT16 *)g_ashwOpLog2Tab0;

    /* 存储每段dy/dx值的表 */
    VOS_INT16 *pshwLogTab1 = (VOS_INT16 *)g_ashwOpLog2Tab1;

    /* 存储每段输入起始值x0的表 */
    VOS_INT16 *pshwLogTab2 = (VOS_INT16 *)g_ashwOpLog2Tab2;

    /* 临时变量 */
    VOS_INT16       shwTemp = 0;
    VOS_INT16       shwNormshift;
    VOS_INT16       shwPoint;
    VOS_INT16       shwPoint1;
    VOS_INT16       shwIndex;
    VOS_INT16       shwDx;
    VOS_INT16       shwDy;

    /* 返回值 */
    VOS_INT16       shwLog2Out;

    /* 小于0.5, 归一化到[0.5, 1) */
    while (shwAmIn < CODEC_OpShl(1, shwQn-1))
    {
        shwTemp = shwTemp + 1;
        shwAmIn = CODEC_OpShl(shwAmIn, 1);
    }

    /* 对输入进行归一化 */
    shwNormshift = CODEC_OpNorm_s(shwAmIn);
    shwAmIn = CODEC_OpShl(shwAmIn, shwNormshift);

    /* 输入shwAmIn的定标是Qn， 输出的定标是Q10 */
    shwPoint = shwNormshift + shwTemp + shwQn;
    shwPoint1 = CODEC_OpShl((15 - shwPoint), 10);

    /* 根据输入值计算表的地址:((shwAmIn-16384)*20)>>15 */
    shwIndex = CODEC_OpMult( (shwAmIn - 16384), 20 );                             /* 16384由2^14得来 */

    shwDx = shwAmIn - pshwLogTab2[shwIndex];
    shwDy = CODEC_OpSaturate( CODEC_OpL_shr(CODEC_OpL_mult0(shwDx, pshwLogTab1[shwIndex]),
                                13) );

    shwLog2Out = CODEC_OpShr(shwDy + pshwLogTab0[shwIndex], 5);
    shwLog2Out = shwLog2Out + shwPoint1;

    return(shwLog2Out);
}


VOS_INT32 CODEC_OpL_Power(
                VOS_INT16               shwBottom,
                VOS_INT16               shwQn1,
                VOS_INT16               shwPower,
                VOS_INT16               shwQn2,
                VOS_INT16               shwQnOut)
{
    /* 返回值 */
    VOS_INT32       swPowerOut;

    /* 临时变量 */
    VOS_INT16       shwLog2;
    VOS_INT32       swExp;
    VOS_INT32       swTemp;
    VOS_INT16       shwQn;
    VOS_INT32       swExponent;
    VOS_INT16       shwFraction;
    VOS_INT16       shwExponent;

    if ( 0 == shwBottom )
    {
        return(0);
    }


    /* log2(x)，精度为[16bit Q10]*/
    shwLog2 = MED_PRECISE_OpLog2(shwBottom, shwQn1);

    /* 以2为底的幂次方 */
    swExp = CODEC_OpL_mult0(shwPower, shwLog2);
    shwQn = 10 + shwQn2;

    /* 取幂的整数部分 */
    swExponent = CODEC_OpL_shr(swExp, shwQn);

    /* 取幂的小数部分(幂数减去幂的整数部分) */
    swTemp = CODEC_OpL_sub(swExp, CODEC_OpL_shl(swExponent, shwQn));
    shwFraction = CODEC_OpSaturate(CODEC_OpL_shr(swTemp, shwQn - 15));

    /* x的y次方转化为求以2为底的幂次方 */
    shwExponent = CODEC_OpSaturate(swExponent);
    swPowerOut  = CODEC_OpL_Exp2(shwExponent, shwFraction, shwQnOut);

    return(swPowerOut);
}
VOS_INT32 CODEC_OpL_Exp(
                VOS_INT16               shwExponent,
                VOS_INT16               shwQn1,
                VOS_INT16               shwQnOut )
{
    /* e的值定标为Q13.e = int16(2.718281828459545 * 2^13)*/
    VOS_INT16       shwE         = 22268;
    VOS_INT16       shwNormshift = 13;

    /* 返回值 */
    VOS_INT32       swExpOut;

    swExpOut = CODEC_OpL_Power(shwE, shwNormshift, shwExponent, shwQn1, shwQnOut);

    return(swExpOut);
}


MED_INT32 CODEC_OpL_Integral(MED_INT16 shwX)
{
    /* 对输入数据分4段进行查表，0.0到0.05之间查表4(让小于1/2048的数等于1/2048)，0.05到0.2之间查表3,0.2到2.0之间查表2,2.0到15.657之间查表1，输入大于15.657的输出直接等于1/2^27*/
    MED_INT32 *pswTableY1 = (MED_INT32*)g_aswIntegralTableY1;
    MED_INT32 *pswTableY2 = (MED_INT32*)g_aswIntegralTableY2;
    MED_INT32 *pswTableY3 = (MED_INT32*)g_aswIntegralTableY3;
    MED_INT32 *pswTableY4 = (MED_INT32*)g_aswIntegralTableY4;

    MED_INT32               swX;
    MED_INT32               swXMax    = 2101446967;                              /* int32(15.657*(2^27)) */
    MED_INT32               swXMin1   = 268435456;                               /* int32(2.0*(2^27)) */
    MED_INT32               swXMin2   = 26843546;                                /* int32(0.2*(2^27))   */
    MED_INT32               swXMin3   = 6710886;                                 /* int32(0.05*(2^27)) */
    MED_INT32               swXMin4   = 65536;                                   /* int32(1.0/2048.0*(2^27)) */

    MED_INT16               shwXStep1 = 29992;                                   /* int16((2^13)*3.6611) */
    MED_INT16               shwXStep2 = 28444;                                   /* int16((2^10)*27.7778) */
    MED_INT16               shwXStep3 = 21333;                                   /* int16((2^6)*333.3333) */
    MED_INT16               shwXStep4 = 32315;                                   /* int16((2^4)*2019.7) */

    MED_INT16               shwIndex  = 0;
    MED_INT16               shwIndexTmp = 0;


    /* 返回值 */
    MED_INT32               swSumOut = 0;

    swX = CODEC_OpL_deposit_h(shwX);

    if (swX < swXMin3)
    {
        /* 计算查表地址 */
        swX         = CODEC_OpL_max(swX, swXMin4);
        shwIndexTmp = (MED_INT16)CODEC_OpL_shr_r(CODEC_OpL_mpy_32_16(CODEC_OpL_sub(swX, swXMin4),shwXStep4), 16);
        shwIndex    = CODEC_OpMin(shwIndexTmp, 99);

        swSumOut    = pswTableY4[shwIndex];
    }
    else if (swX < swXMin2)
    {
        /* 计算查表地址 */
        shwIndexTmp = (MED_INT16)CODEC_OpL_shr_r(CODEC_OpL_mpy_32_16(CODEC_OpL_sub(swX, swXMin3),shwXStep3),18);
        shwIndex    = CODEC_OpMin(shwIndexTmp, 49);

        swSumOut    = pswTableY3[shwIndex];
    }
    else if (swX < swXMin1)
    {
        /* 计算查表地址 */
        shwIndexTmp = (MED_INT16)CODEC_OpL_shr_r(CODEC_OpL_mpy_32_16(CODEC_OpL_sub(swX, swXMin2),shwXStep2), 22);
        shwIndex    = CODEC_OpMin(shwIndexTmp, 49);

        swSumOut    = pswTableY2[shwIndex];
    }
    else if (swX <= swXMax)
    {
        /* 计算查表地址 */
        shwIndexTmp = (MED_INT16)CODEC_OpL_shr_r(CODEC_OpL_mpy_32_16(CODEC_OpL_sub(swX, swXMin1),shwXStep1), 25);
        shwIndex    = CODEC_OpMin(shwIndexTmp, 49);

        swSumOut    = pswTableY1[shwIndex];
    }
    else
    {
        swSumOut = 1;
    }

    return (swSumOut);

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


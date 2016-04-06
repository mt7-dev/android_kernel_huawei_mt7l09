

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "med_aec_hf_nlp.h"
#include "ucom_comm.h"
#include <stdlib.h>

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/
/* 窄带下NLP时域汉宁窗 */
/*
shwFrameLen = 160;
window    = [0 , sqrt( hanning(2*shwFrameLen - 1)' )];
window_fi = round(window * 32768);
window_fi(window_fi==32768)=32767;
*/
VOS_INT16 g_ashwMedAecHfNlpHannWinNb[MED_AEC_HF_NLP_FRM_LEN_NB*2] =
{
        0,   322,   643,   965,  1286,  1608,  1929,  2250,  2571,  2892,
     3212,  3532,  3851,  4171,  4490,  4808,  5126,  5444,  5760,  6077,
     6393,  6708,  7022,  7336,  7650,  7962,  8274,  8585,  8895,  9204,
     9512,  9819, 10126, 10431, 10736, 11039, 11342, 11643, 11943, 12242,
    12540, 12836, 13132, 13426, 13719, 14010, 14300, 14589, 14876, 15162,
    15447, 15730, 16011, 16291, 16569, 16846, 17121, 17395, 17666, 17937,
    18205, 18472, 18736, 18999, 19261, 19520, 19777, 20033, 20286, 20538,
    20788, 21035, 21281, 21525, 21766, 22006, 22243, 22478, 22711, 22942,
    23170, 23397, 23621, 23843, 24062, 24279, 24494, 24707, 24917, 25125,
    25330, 25533, 25733, 25931, 26127, 26320, 26510, 26698, 26883, 27066,
    27246, 27423, 27598, 27770, 27939, 28106, 28270, 28431, 28590, 28746,
    28899, 29049, 29197, 29341, 29483, 29622, 29758, 29891, 30022, 30149,
    30274, 30395, 30514, 30630, 30743, 30853, 30959, 31063, 31164, 31262,
    31357, 31449, 31538, 31624, 31706, 31786, 31863, 31936, 32007, 32074,
    32138, 32200, 32258, 32313, 32365, 32413, 32459, 32501, 32541, 32577,
    32610, 32640, 32667, 32691, 32711, 32729, 32743, 32754, 32762, 32766,
    32767, 32766, 32762, 32754, 32743, 32729, 32711, 32691, 32667, 32640,
    32610, 32577, 32541, 32501, 32459, 32413, 32365, 32313, 32258, 32200,
    32138, 32074, 32007, 31936, 31863, 31786, 31706, 31624, 31538, 31449,
    31357, 31262, 31164, 31063, 30959, 30853, 30743, 30630, 30514, 30395,
    30274, 30149, 30022, 29891, 29758, 29622, 29483, 29341, 29197, 29049,
    28899, 28746, 28590, 28431, 28270, 28106, 27939, 27770, 27598, 27423,
    27246, 27066, 26883, 26698, 26510, 26320, 26127, 25931, 25733, 25533,
    25330, 25125, 24917, 24707, 24494, 24279, 24062, 23843, 23621, 23397,
    23170, 22942, 22711, 22478, 22243, 22006, 21766, 21525, 21281, 21035,
    20788, 20538, 20286, 20033, 19777, 19520, 19261, 18999, 18736, 18472,
    18205, 17937, 17666, 17395, 17121, 16846, 16569, 16291, 16011, 15730,
    15447, 15162, 14876, 14589, 14300, 14010, 13719, 13426, 13132, 12836,
    12540, 12242, 11943, 11643, 11342, 11039, 10736, 10431, 10126,  9819,
     9512,  9204,  8895,  8585,  8274,  7962,  7650,  7336,  7022,  6708,
     6393,  6077,  5760,  5444,  5126,  4808,  4490,  4171,  3851,  3532,
     3212,  2892,  2571,  2250,  1929,  1608,  1286,   965,   643,   322
};    /* Q15 */

VOS_INT16 g_ashwMedAecHfNlpHannWinWb[MED_AEC_HF_NLP_FRM_LEN_WB*2] =
{
        0,   161,   322,   483,   643,   804,   965,  1126,  1286,  1447,
     1608,  1768,  1929,  2090,  2250,  2411,  2571,  2731,  2892,  3052,
     3212,  3372,  3532,  3692,  3851,  4011,  4171,  4330,  4490,  4649,
     4808,  4967,  5126,  5285,  5444,  5602,  5760,  5919,  6077,  6235,
     6393,  6550,  6708,  6865,  7022,  7180,  7336,  7493,  7650,  7806,
     7962,  8118,  8274,  8429,  8585,  8740,  8895,  9049,  9204,  9358,
     9512,  9666,  9819,  9973, 10126, 10279, 10431, 10584, 10736, 10888,
    11039, 11191, 11342, 11492, 11643, 11793, 11943, 12093, 12242, 12391,
    12540, 12688, 12836, 12984, 13132, 13279, 13426, 13572, 13719, 13865,
    14010, 14155, 14300, 14445, 14589, 14733, 14876, 15019, 15162, 15305,
    15447, 15588, 15730, 15871, 16011, 16151, 16291, 16430, 16569, 16708,
    16846, 16984, 17121, 17258, 17395, 17531, 17666, 17802, 17937, 18071,
    18205, 18338, 18472, 18604, 18736, 18868, 18999, 19130, 19261, 19390,
    19520, 19649, 19777, 19905, 20033, 20160, 20286, 20413, 20538, 20663,
    20788, 20912, 21035, 21159, 21281, 21403, 21525, 21646, 21766, 21886,
    22006, 22125, 22243, 22361, 22478, 22595, 22711, 22827, 22942, 23056,
    23170, 23284, 23397, 23509, 23621, 23732, 23843, 23953, 24062, 24171,
    24279, 24387, 24494, 24601, 24707, 24812, 24917, 25021, 25125, 25228,
    25330, 25432, 25533, 25633, 25733, 25833, 25931, 26029, 26127, 26223,
    26320, 26415, 26510, 26604, 26698, 26791, 26883, 26975, 27066, 27156,
    27246, 27335, 27423, 27511, 27598, 27684, 27770, 27855, 27939, 28023,
    28106, 28188, 28270, 28351, 28431, 28511, 28590, 28668, 28746, 28823,
    28899, 28974, 29049, 29123, 29197, 29269, 29341, 29412, 29483, 29553,
    29622, 29690, 29758, 29825, 29891, 29957, 30022, 30086, 30149, 30212,
    30274, 30335, 30395, 30455, 30514, 30572, 30630, 30687, 30743, 30798,
    30853, 30906, 30959, 31012, 31063, 31114, 31164, 31214, 31262, 31310,
    31357, 31403, 31449, 31494, 31538, 31581, 31624, 31665, 31706, 31747,
    31786, 31825, 31863, 31900, 31936, 31972, 32007, 32041, 32074, 32107,
    32138, 32169, 32200, 32229, 32258, 32286, 32313, 32339, 32365, 32389,
    32413, 32437, 32459, 32481, 32501, 32522, 32541, 32559, 32577, 32594,
    32610, 32626, 32640, 32654, 32667, 32679, 32691, 32701, 32711, 32720,
    32729, 32736, 32743, 32749, 32754, 32758, 32762, 32764, 32766, 32767,
    32767, 32767, 32766, 32764, 32762, 32758, 32754, 32749, 32743, 32736,
    32729, 32720, 32711, 32701, 32691, 32679, 32667, 32654, 32640, 32626,
    32610, 32594, 32577, 32559, 32541, 32522, 32501, 32481, 32459, 32437,
    32413, 32389, 32365, 32339, 32313, 32286, 32258, 32229, 32200, 32169,
    32138, 32107, 32074, 32041, 32007, 31972, 31936, 31900, 31863, 31825,
    31786, 31747, 31706, 31665, 31624, 31581, 31538, 31494, 31449, 31403,
    31357, 31310, 31262, 31214, 31164, 31114, 31063, 31012, 30959, 30906,
    30853, 30798, 30743, 30687, 30630, 30572, 30514, 30455, 30395, 30335,
    30274, 30212, 30149, 30086, 30022, 29957, 29891, 29825, 29758, 29690,
    29622, 29553, 29483, 29412, 29341, 29269, 29197, 29123, 29049, 28974,
    28899, 28823, 28746, 28668, 28590, 28511, 28431, 28351, 28270, 28188,
    28106, 28023, 27939, 27855, 27770, 27684, 27598, 27511, 27423, 27335,
    27246, 27156, 27066, 26975, 26883, 26791, 26698, 26604, 26510, 26415,
    26320, 26223, 26127, 26029, 25931, 25833, 25733, 25633, 25533, 25432,
    25330, 25228, 25125, 25021, 24917, 24812, 24707, 24601, 24494, 24387,
    24279, 24171, 24062, 23953, 23843, 23732, 23621, 23509, 23397, 23284,
    23170, 23056, 22942, 22827, 22711, 22595, 22478, 22361, 22243, 22125,
    22006, 21886, 21766, 21646, 21525, 21403, 21281, 21159, 21035, 20912,
    20788, 20663, 20538, 20413, 20286, 20160, 20033, 19905, 19777, 19649,
    19520, 19390, 19261, 19130, 18999, 18868, 18736, 18604, 18472, 18338,
    18205, 18071, 17937, 17802, 17666, 17531, 17395, 17258, 17121, 16984,
    16846, 16708, 16569, 16430, 16291, 16151, 16011, 15871, 15730, 15588,
    15447, 15305, 15162, 15019, 14876, 14733, 14589, 14445, 14300, 14155,
    14010, 13865, 13719, 13572, 13426, 13279, 13132, 12984, 12836, 12688,
    12540, 12391, 12242, 12093, 11943, 11793, 11643, 11492, 11342, 11191,
    11039, 10888, 10736, 10584, 10431, 10279, 10126,  9973,  9819,  9666,
     9512,  9358,  9204,  9049,  8895,  8740,  8585,  8429,  8274,  8118,
     7962,  7806,  7650,  7493,  7336,  7180,  7022,  6865,  6708,  6550,
     6393,  6235,  6077,  5919,  5760,  5602,  5444,  5285,  5126,  4967,
     4808,  4649,  4490,  4330,  4171,  4011,  3851,  3692,  3532,  3372,
     3212,  3052,  2892,  2731,  2571,  2411,  2250,  2090,  1929,  1768,
     1608,  1447,  1286,  1126,   965,   804,   643,   483,   322,   161
};    /* Q15 */


VOS_INT16 g_ashwWeightNB[] =                                                    /*Q15, 1*161*/
{0, 29491, 3277, 5356, 6217, 6878, 7435, 7925, 8369, 8777, 9157, 9514, 9851, 10172,
10478, 10773, 11055, 11328, 11593, 11848, 12097, 12339, 12574, 12804, 13028,
13247, 13461, 13671, 13877, 14079, 14278, 14472, 14664, 14852, 15037, 15219,
15399, 15576, 15750, 15922, 16092, 16260, 16425, 16588, 16750, 16909, 17067,
17223, 17377, 17529, 17680, 17829, 17977, 18123, 18268, 18412, 18554, 18695,
18834, 18972, 19110, 19245, 19380, 19514, 19646, 19778, 19908, 20038, 20166,
20294, 20420, 20546, 20670, 20794, 20917, 21039, 21160, 21281, 21401, 21519,
21637, 21755, 21871, 21987, 22102, 22217, 22331, 22444, 22556, 22668, 22779,
22889, 22999, 23109, 23217, 23325, 23433, 23540, 23646, 23752, 23857, 23962,
24066, 24170, 24273, 24376, 24478, 24580, 24681, 24781, 24882, 24982, 25081,
25180, 25278, 25376, 25474, 25571, 25668, 25764, 25860, 25955, 26050, 26145,
26239, 26333, 26427, 26520, 26613, 26705, 26797, 26889, 26980, 27071, 27162,
27252, 27342, 27432, 27521, 27610, 27699, 27787, 27875, 27963, 28050, 28137,
28224, 28311, 28397, 28483, 28568, 28653, 28738, 28823, 28908, 28992, 29076,
29159, 29243, 29326, 29409};

VOS_INT16 g_ashwWeightWB[] =                                                    /*Q15, 1*321*/
{0, 29491, 3277, 4745, 5352, 5819, 6212, 6559, 6872, 7160, 7428, 7680, 7918, 8145,
8361, 8569, 8769, 8961, 9148, 9328, 9504, 9674, 9841, 10003, 10161, 10316,
10467, 10615, 10761, 10903, 11043, 11181, 11316, 11449, 11580, 11708, 11835,
11960, 12083, 12205, 12324, 12443, 12560, 12675, 12789, 12901, 13013, 13123,
13231, 13339, 13445, 13551, 13655, 13758, 13861, 13962, 14062, 14162, 14260,
14358, 14455, 14551, 14646, 14740, 14834, 14927, 15019, 15110, 15201, 15291,
15380, 15469, 15557, 15644, 15731, 15817, 15903, 15988, 16072, 16156, 16239,
16322, 16405, 16486, 16568, 16648, 16729, 16809, 16888, 16967, 17045, 17123,
17201, 17278, 17355, 17431, 17507, 17582, 17658, 17732, 17807, 17880, 17954,
18027, 18100, 18173, 18245, 18317, 18388, 18459, 18530, 18600, 18670, 18740,
18810, 18879, 18948, 19016, 19085, 19153, 19220, 19288, 19355, 19422, 19488,
19555, 19621, 19686, 19752, 19817, 19882, 19947, 20011, 20076, 20140, 20203,
20267, 20330, 20393, 20456, 20519, 20581, 20643, 20705, 20767, 20828, 20889,
20951, 21011, 21072, 21132, 21193, 21253, 21313, 21372, 21432, 21491, 21550,
21609, 21667, 21726, 21784, 21842, 21900, 21958, 22015, 22073, 22130, 22187,
22244, 22301, 22357, 22414, 22470, 22526, 22582, 22637, 22693, 22748, 22804,
22859, 22914, 22968, 23023, 23077, 23132, 23186, 23240, 23294, 23348, 23401,
23455, 23508, 23561, 23614, 23667, 23720, 23772, 23825, 23877, 23930, 23982,
24034, 24085, 24137, 24189, 24240, 24291, 24343, 24394, 24445, 24495, 24546,
24597, 24647, 24698, 24748, 24798, 24848, 24898, 24947, 24997, 25047, 25096,
25145, 25195, 25244, 25293, 25342, 25390, 25439, 25488, 25536, 25584, 25633,
25681, 25729, 25777, 25824, 25872, 25920, 25967, 26015, 26062, 26109, 26156,
26203, 26250, 26297, 26344, 26391, 26437, 26484, 26530, 26576, 26622, 26668,
26714, 26760, 26806, 26852, 26898, 26943, 26989, 27034, 27079, 27125, 27170,
27215, 27260, 27304, 27349, 27394, 27439, 27483, 27528, 27572, 27616, 27660,
27705, 27749, 27793, 27837, 27880, 27924, 27968, 28011, 28055, 28098, 28142,
28185, 28228, 28271, 28314, 28357, 28400, 28443, 28486, 28528, 28571, 28614,
28656, 28699, 28741, 28783, 28825, 28867, 28910, 28951, 28993, 29035, 29077,
29119, 29160, 29202, 29243, 29285, 29326, 29368, 29409, 29450};

VOS_INT16 g_ashwFreqNB[] =                                                      /*Q14*/
{16384, 32767, 17679, 18216, 18627, 18975, 19280, 19557, 19811, 20048, 20270, 20480,
20680, 20871, 21054, 21230, 21401, 21565, 21725, 21879, 22030, 22177, 22320,
22459, 22596, 22729, 22860, 22989, 23114, 23238, 23359, 23478, 23596, 23711,
23825, 23937, 24047, 24156, 24263, 24369, 24473, 24576, 24678, 24778, 24878,
24976, 25073, 25169, 25264, 25358, 25451, 25543, 25634, 25724, 25814, 25902,
25990, 26077, 26163, 26248, 26333, 26417, 26500, 26583, 26665, 26746, 26827,
26907, 26986, 27065, 27143, 27221, 27298, 27375, 27451, 27526, 27601, 27676,
27750, 27824, 27897, 27969, 28041, 28113, 28184, 28255, 28326, 28396, 28465,
28535, 28604, 28672, 28740, 28808, 28875, 28942, 29009, 29075, 29141, 29207,
29272, 29337, 29401, 29466, 29530, 29593, 29657, 29720, 29782, 29845, 29907,
29969, 30031, 30092, 30153, 30214, 30274, 30334, 30394, 30454, 30514, 30573,
30632, 30691, 30749, 30808, 30866, 30923, 30981, 31038, 31095, 31152, 31209,
31266, 31322, 31378, 31434, 31489, 31545, 31600, 31655, 31710, 31764, 31819,
31873, 31927, 31981, 32035, 32088, 32142, 32195, 32248, 32301, 32353, 32406,
32458, 32510, 32562, 32614, 32665, 32717}; /* 1*161 */

VOS_INT16 g_ashwFreqWB[] =                                                      /*Q14*/
{16384, 32767, 17300, 17679, 17970, 18216, 18432, 18627, 18807, 18975, 19132, 19280,
19422, 19557, 19686, 19811, 19931, 20048, 20160, 20270, 20376, 20480, 20581,
20680, 20776, 20871, 20963, 21054, 21143, 21230, 21316, 21401, 21483, 21565,
21645, 21725, 21802, 21879, 21955, 22030, 22104, 22177, 22249, 22320, 22390,
22459, 22528, 22596, 22663, 22729, 22795, 22860, 22925, 22989, 23052, 23114,
23176, 23238, 23299, 23359, 23419, 23478, 23537, 23596, 23654, 23711, 23768,
23825, 23881, 23937, 23992, 24047, 24101, 24156, 24209, 24263, 24316, 24369,
24421, 24473, 24525, 24576, 24627, 24678, 24728, 24778, 24828, 24878, 24927,
24976, 25025, 25073, 25121, 25169, 25217, 25264, 25311, 25358, 25405, 25451,
25497, 25543, 25589, 25634, 25679, 25724, 25769, 25814, 25858, 25902, 25946,
25990, 26034, 26077, 26120, 26163, 26206, 26248, 26291, 26333, 26375, 26417,
26459, 26500, 26542, 26583, 26624, 26665, 26706, 26746, 26787, 26827, 26867,
26907, 26947, 26986, 27026, 27065, 27104, 27143, 27182, 27221, 27260, 27298,
27336, 27375, 27413, 27451, 27489, 27526, 27564, 27601, 27639, 27676, 27713,
27750, 27787, 27824, 27860, 27897, 27933, 27969, 28005, 28041, 28077, 28113,
28149, 28184, 28220, 28255, 28291, 28326, 28361, 28396, 28431, 28465, 28500,
28535, 28569, 28604, 28638, 28672, 28706, 28740, 28774, 28808, 28841, 28875,
28909, 28942, 28975, 29009, 29042, 29075, 29108, 29141, 29174, 29207, 29239,
29272, 29304, 29337, 29369, 29401, 29433, 29466, 29498, 29530, 29561, 29593,
29625, 29657, 29688, 29720, 29751, 29782, 29814, 29845, 29876, 29907, 29938,
29969, 30000, 30031, 30061, 30092, 30122, 30153, 30183, 30214, 30244, 30274,
30304, 30334, 30365, 30394, 30424, 30454, 30484, 30514, 30543, 30573, 30602,
30632, 30661, 30691, 30720, 30749, 30778, 30808, 30837, 30866, 30894, 30923,
30952, 30981, 31010, 31038, 31067, 31095, 31124, 31152, 31181, 31209, 31237,
31266, 31294, 31322, 31350, 31378, 31406, 31434, 31462, 31489, 31517, 31545,
31572, 31600, 31628, 31655, 31682, 31710, 31737, 31764, 31792, 31819, 31846,
31873, 31900, 31927, 31954, 31981, 32008, 32035, 32062, 32088, 32115, 32142,
32168, 32195, 32221, 32248, 32274, 32301, 32327, 32353, 32379, 32406, 32432,
32458, 32484, 32510, 32536, 32562, 32588, 32614, 32639, 32665, 32691, 32717,
32742}; /* 1*321 */

VOS_INT16 g_ashwCoeff[] = {18432, 19456, 20480, 19456, 18432, 17408, 16384};    /*Q12*/

/*****************************************************************************
   3 函数实现
*****************************************************************************/

VOS_VOID MED_AEC_HF_NlpInit(
                MED_AEC_NV_STRU                    *pstAecParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate,
                MED_AEC_HF_NLP_STRU                *pstNlp)
{
    VOS_INT16               shwIdx;

    UCOM_MemSet((VOS_VOID*)pstNlp, 0, sizeof(MED_AEC_HF_NLP_STRU));

    /* NV配置 */
    pstNlp->stSmoothGain.shwRefCntThd         = pstAecParam->shwNlpRefCnt;      /* 远端信号计数门限 */
    pstNlp->stSmoothGain.shwRefAmp1           = pstAecParam->shwNlpRefAmp1;     /* 远端信号能量门限1，用于抑制第一声回声 */
    pstNlp->stSmoothGain.shwExOverdrive       = pstAecParam->shwNlpExOverdrive; /* 加强的Overdrive */
    pstNlp->stSTSuppress.shwResdPowAlph       = pstAecParam->shwNlpResdPowAlph; /* NLP处理后计算残差信号能量的滤波系数 */
    pstNlp->stSTSuppress.shwResdPowThd        = pstAecParam->shwNlpResdPowThd;  /* NLP处理后残差信号能量小于此门限且参考信号较大时进一步抑制残差信号 */
    pstNlp->stSmoothGain.shwOvrdod            = pstAecParam->shwNlpSmoothGainDod;/* NLPSmoothGain增益因子的幂指数 */
    pstNlp->stBandSort.shwHnlIdx              = pstAecParam->shwNlpBandSortIdx;  /* 回声不相干系数adhnled升序排序后结果的一般分段点Idx*/
    pstNlp->stBandSort.shwHnlIdxLow           = pstAecParam->shwNlpBandSortIdxLow;/*回声不相干系数adhnled升序排序后结果的低分段点Idx*/


    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        pstNlp->stTrans.pshwWin               = &g_ashwMedAecHfNlpHannWinNb[0];
        pstNlp->stTrans.shwFrmLen             =  MED_AEC_HF_NLP_FRM_LEN_NB;
        pstNlp->stTrans.shwFftLen             =  MED_AEC_HF_NLP_FFT_LEN_NB;
        pstNlp->stTrans.enFftNumIndex         =  MED_FFT_NUM_320;
        pstNlp->stTrans.enIfftNumIndex        =  MED_IFFT_NUM_320;

        pstNlp->stCn.shwBinLen                =  (MED_AEC_HF_NLP_FFT_LEN_NB >> 1) + 1;
        pstNlp->stSmoothPSD.shwFFTLenth       =  MED_AEC_HF_NLP_FFT_LEN_NB;

        pstNlp->stOverdrive.shwMult           =  CODEC_SAMPLE_RATE_MODE_8000;

        pstNlp->stBandSort.shwFFTLenth        =  MED_AEC_HF_NLP_FFT_LEN_NB;
        pstNlp->stBandSort.shwEchoRange       =  MED_AEC_HF_ECHO_BIN_RANGE_NB;
        pstNlp->stBandSort.shwEchoLeng        =  (MED_AEC_HF_NLP_FFT_LEN_NB / 2) + 1;
        pstNlp->stBandSort.shwBinStart        =  MED_AEC_HF_ECHO_BIN_START;

        pstNlp->stSmoothGain.pashwWeight      =  g_ashwWeightNB;
        pstNlp->stSmoothGain.pashwFreq        =  g_ashwFreqNB;

        pstNlp->stSTSuppress.shwResdPowBandLow=  MED_AEC_HF_NLP_RESDPOW_LOWBAND_8k;
        pstNlp->stSTSuppress.shwResdPowBandHi =  MED_AEC_HF_NLP_RESDPOW_HIGHBAND_8k;

        /* 舒适噪声结构体初始化 */
        pstNlp->stCn.shwNormalize             =  MED_AEC_HF_NLP_INSERTCN_NORMALIZE_NB;
        for (shwIdx = 0; shwIdx < (1 + (MED_AEC_MAX_FFT_LEN / 2)); shwIdx++)
        {
            pstNlp->stCn.aswNearPsdBin[shwIdx] = MED_AEC_HF_NLP_CN_NEARPSD_INIT;
            pstNlp->stCn.aswPsdBin[shwIdx] = MED_AEC_HF_NLP_CN_PSD_INIT_NB;
        }
    }
    else if (CODEC_SAMPLE_RATE_MODE_16000 == enSampleRate)
    {
        pstNlp->stTrans.pshwWin               =  &g_ashwMedAecHfNlpHannWinWb[0];
        pstNlp->stTrans.shwFrmLen             =  MED_AEC_HF_NLP_FRM_LEN_WB;
        pstNlp->stTrans.shwFftLen             =  MED_AEC_MAX_FFT_LEN;
        pstNlp->stTrans.enFftNumIndex         =  MED_FFT_NUM_640;
        pstNlp->stTrans.enIfftNumIndex        =  MED_IFFT_NUM_640;

        pstNlp->stCn.shwBinLen                =  (MED_AEC_MAX_FFT_LEN >> 1) + 1;
        pstNlp->stSmoothPSD.shwFFTLenth       =  MED_AEC_MAX_FFT_LEN;

        pstNlp->stOverdrive.shwMult           =  CODEC_SAMPLE_RATE_MODE_16000;

        pstNlp->stBandSort.shwFFTLenth        =  MED_AEC_MAX_FFT_LEN;
        pstNlp->stBandSort.shwEchoRange       =  MED_AEC_HF_ECHO_BIN_RANGE_WB;
        pstNlp->stBandSort.shwEchoLeng        =  (MED_AEC_MAX_FFT_LEN / 2) + 1;
        pstNlp->stBandSort.shwBinStart        =  ((MED_AEC_HF_ECHO_BIN_START + 1) << 1) - 1;

        pstNlp->stSmoothGain.pashwWeight      =  g_ashwWeightWB;
        pstNlp->stSmoothGain.pashwFreq        =  g_ashwFreqWB;

        pstNlp->stSTSuppress.shwResdPowBandLow=  MED_AEC_HF_NLP_RESDPOW_LOWBAND_16k;
        pstNlp->stSTSuppress.shwResdPowBandHi =  MED_AEC_HF_NLP_RESDPOW_HIGHBAND_16k;

        /* 舒适噪声结构体初始化 */
        pstNlp->stCn.shwNormalize             =  MED_AEC_HF_NLP_INSERTCN_NORMALIZE_WB;
        for (shwIdx = 0; shwIdx < (1 + (MED_AEC_MAX_FFT_LEN / 2)); shwIdx++)
        {
            pstNlp->stCn.aswNearPsdBin[shwIdx] = MED_AEC_HF_NLP_CN_NEARPSD_INIT;
            pstNlp->stCn.aswPsdBin[shwIdx] = MED_AEC_HF_NLP_CN_PSD_INIT_WB;
        }
    }
    else
    {
        /* UNDEFINED */
    }

    pstNlp->stCn.enCnFlag                     =  CODEC_SWITCH_OFF;
    pstNlp->stSmoothPSD.shwGammaT             =  MED_AEC_HF_NLP_SMOOTH_PARA_SLOW;
    pstNlp->stOverdrive.swHnlMin              =  32767;
    pstNlp->stOverdrive.shwHnlLocalMin        =  32767;
    pstNlp->stOverdrive.shwCohxdLocalMin      =  32767;

    pstNlp->stOverdrive.shwOvrd               =  4094;
    pstNlp->stOverdrive.shwOvrdSm             =  4094;
    pstNlp->stOverdrive.shwSuppState          =  1;

}



VOS_VOID MED_AEC_HF_NlpMain(
                MED_AEC_HF_NLP_STRU           *pstNlp,
                VOS_INT32                     *pswWeight,
                VOS_INT16                     *pshwNearFrm,
                VOS_INT16                     *pshwRefrFrm,
                VOS_INT16                     *pshwResdFrm,
                VOS_INT16                     *pshwOut)
{

    MED_AEC_HF_NLP_TRANS_STRU         *pstTime2Freq     = &pstNlp->stTrans;
    MED_AEC_HF_NLP_TRANS_STRU         *pstFreq2Time     = &pstNlp->stTrans;
    MED_AEC_HF_NLP_SMOOTH_PSD_STRU    *pstSmoothPSD     = &pstNlp->stSmoothPSD;
    MED_AEC_HF_NLP_BAND_SORT_STRU     *pstBandSort      = &pstNlp->stBandSort;
    MED_AEC_HF_NLP_OVER_DRIVE_STRU    *pstOverdrive     = &pstNlp->stOverdrive;
    MED_AEC_HF_NLP_CN_STRU            *pstCn            = &pstNlp->stCn;
    MED_AEC_HF_NLP_SMOOTH_GAIN_STRU   *pstSmoothGain    = &pstNlp->stSmoothGain;
    MED_AEC_HF_NLP_STSUPPRE_STRU      *pstSTSuppress    = &pstNlp->stSTSuppress;

    VOS_INT16                          shwFftLen        = pstNlp->stTrans.shwFftLen;

    VOS_INT16                         *pshwNearFreq     = &pstTime2Freq->ashwNearFreq[0];
    VOS_INT16                         *pshwResdFreq     = &pstTime2Freq->ashwResdFreq[0];

    VOS_INT32                         *pswSee           = &pstSmoothPSD->aswSee[0];
    VOS_INT32                         *pswSdd           = &pstSmoothPSD->aswSdd[0];
    VOS_INT32                         *pswSxx           = &pstSmoothPSD->aswSxx[0];
    VOS_INT16                         *pshwhnled        = &pstSmoothPSD->ashwhnled[0];
    VOS_INT16                         *pshwcohed        = &pstSmoothPSD->ashwcohed[0];
    VOS_INT16                         *pshwcohxd        = &pstSmoothPSD->ashwcohxd[0];

    VOS_INT16                         *pshwDivergeState = &pstOverdrive->shwDivergeState;

    VOS_INT16                          shwMaxFar;
    VOS_INT16                          shwPosMax;

    /* NLP模块从时域信号变换到频域 */
    MED_AEC_HF_NlpTime2Freq(pstTime2Freq, pshwNearFrm, pshwRefrFrm, pshwResdFrm);

    /* 求参考信号最大值 */
    shwMaxFar   = CODEC_OpVecMaxAbs(pshwRefrFrm + (shwFftLen >> 1), (shwFftLen >> 1), &shwPosMax);

    /* 计算功率谱平滑系数 */
    MED_AEC_HF_NlpGamma(pstSmoothPSD, pswSxx);
    MED_AEC_HF_NlpSmoothPSD(pstSmoothPSD, pstTime2Freq, shwMaxFar);

    /* 频点排序 */
    MED_AEC_HF_NlpBandSort(pshwcohed, pshwcohxd, pshwhnled, pstBandSort);

    /* 计算NLP残差增益过冲量 */
    MED_AEC_HF_NlpOverdrive(pshwcohxd, pshwcohed, pstBandSort, pstOverdrive, pshwhnled);

    /* 残差发散处理 */
    MED_AEC_HF_NlpDivergeProc(pswSee, pswSdd, pshwNearFreq, pshwResdFreq, pshwDivergeState, pswWeight, shwFftLen);

    /* 施加残差增益 */
    /* swOvrdSm在NlpSmoothGain和Overdrive的位宽不同，需要统一 */
    /* 参数ashwErrFreq是数组，应该传递指针，实际传递了数组最后元素的下一个元素 */
    MED_AEC_HF_NlpSmoothGain(pstSmoothGain,
                             pstBandSort->shwHnlPrefAvg,
                             pshwhnled,
                             pstOverdrive->shwOvrdSm,
                             shwFftLen,
                             pstSmoothPSD->shwGammaT,
                             pshwResdFreq,
                             shwMaxFar);

    /* 单讲进一步抑制 */
    MED_AEC_HF_STSuppress(pstSTSuppress, shwFftLen, pshwRefrFrm, pshwResdFreq);

    /* 估计舒适噪声 */
    MED_AEC_HF_NlpNoisePower(pstCn, pstSmoothPSD->aswSddOrg);

    /* 插入舒适噪声 */
    /* 建议将结构体指针作为函数形参，而不是结构体成员 */
    /* pashwSym在NlpInsertCN和NoisePower中的位宽不统一 */
    /* ashwErrFreq[MED_AEC_MAX_FFT_LEN]是元素，建议传递指针 */
    MED_AEC_HF_NlpInsertCN(pstCn, shwFftLen, pshwResdFreq, pshwhnled);

    /* NLP模块从频域信号变换到时域 */
    /* 函数参数需要修改，pshwResdFreq是pstFreq2Time指向结构体成员，不需要单独传递 */
    MED_AEC_HF_NlpFreq2Time(pstFreq2Time, pshwResdFreq, pshwOut);

}
VOS_VOID MED_AEC_HF_NlpTime2Freq(
                    MED_AEC_HF_NLP_TRANS_STRU       *pstTrans,
                    VOS_INT16                       *pshwNearFrm,
                    VOS_INT16                       *pshwRefrFrm,
                    VOS_INT16                       *pshwResdFrm)
{
    VOS_INT16                 shwIdxI;
    VOS_INT16                 shwFrameLen         =  pstTrans->shwFrmLen;
    VOS_INT16                 shwFftLen           =  pstTrans->shwFftLen;
    VOS_INT16                *pshwLastResdFrm     = &pstTrans->ashwLastResdFrm[0];
    VOS_INT16                *pshwNearFreq        = &pstTrans->ashwNearFreq[0];
    VOS_INT16                *pshwResdFreq        = &pstTrans->ashwResdFreq[0];
    VOS_INT16                *pshwRefrFreq        = &pstTrans->ashwRefrFreq[0];
    MED_FFT_NUM_ENUM_INT16    enFftNumIndex       =  pstTrans->enFftNumIndex;
    VOS_INT16                *pshwWin             =  pstTrans->pshwWin;
    VOS_INT16                 shwInMax;
    VOS_INT16                 shwNormShift;
    VOS_INT16                 swMaxPos;
    VOS_INT16                *pshwBuf             =  MED_NULL;

    /* 内存申请 */
    pshwBuf = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT16));

    /* 近端信号时频变换 */

    /* 加汉宁窗 */
    for (shwIdxI = 0; shwIdxI < shwFftLen; shwIdxI++)
    {
        pshwBuf[shwIdxI] = CODEC_OpMult_r(pshwNearFrm[shwIdxI],
                                          pshwWin[shwIdxI]);
    }

    /* 归一化 */
    shwInMax             = CODEC_OpVecMaxAbs(pshwBuf, shwFftLen, &swMaxPos);
    shwNormShift         = CODEC_OpNorm_s(shwInMax);
    CODEC_OpVecShl(pshwBuf, shwFftLen, shwNormShift, pshwBuf);

    /* 时频变换 */
    MED_FFT_Fft(enFftNumIndex, pshwBuf, pshwNearFreq);

    /* 反归一化 */
    CODEC_OpVecShr_r(pshwNearFreq, shwFftLen, shwNormShift, pshwNearFreq);

    /* 残差信号时频变换 */
    /* 上一帧和当前帧残差时域信号拼接 */
    CODEC_OpVecCpy(pshwBuf, pshwLastResdFrm, shwFrameLen);
    CODEC_OpVecCpy(pshwBuf + shwFrameLen, pshwResdFrm, shwFrameLen);

    /* 当前帧残差时域信号作为历史值 */
    CODEC_OpVecCpy(pshwLastResdFrm, pshwResdFrm, shwFrameLen);

    /* 加汉宁窗 */
    for (shwIdxI = 0; shwIdxI < shwFftLen; shwIdxI++)
    {
        pshwBuf[shwIdxI] = CODEC_OpMult_r(pshwBuf[shwIdxI], pshwWin[shwIdxI]);
    }

    /* 归一化 */
    shwInMax             = CODEC_OpVecMaxAbs(pshwBuf, shwFftLen, &swMaxPos);

    shwNormShift         = CODEC_OpNorm_s(shwInMax);

    CODEC_OpVecShl(pshwBuf, shwFftLen, shwNormShift, pshwBuf);

    /* 时频变换 */
    MED_FFT_Fft(enFftNumIndex, pshwBuf, pshwResdFreq);

    /* 反归一化 */
    CODEC_OpVecShr_r(pshwResdFreq, shwFftLen, shwNormShift, pshwResdFreq);

    /* 远端信号时频变换 */
    /* 加汉宁窗 */
    for (shwIdxI = 0; shwIdxI < shwFftLen; shwIdxI++)
    {
        pshwBuf[shwIdxI] = CODEC_OpMult_r(pshwRefrFrm[shwIdxI],
                                          pshwWin[shwIdxI]);
    }

    /* 归一化 */
    shwInMax             = CODEC_OpVecMaxAbs(pshwBuf, shwFftLen, &swMaxPos);

    shwNormShift         = CODEC_OpNorm_s(shwInMax);

    CODEC_OpVecShl(pshwBuf, shwFftLen, shwNormShift, pshwBuf);

    /* 时频变换 */
    MED_FFT_Fft(enFftNumIndex, pshwBuf, pshwRefrFreq);

    /* 反归一化 */
    CODEC_OpVecShr_r(pshwRefrFreq, shwFftLen, shwNormShift, pshwRefrFreq);

    /* 内存释放 */
    UCOM_MemFree(pshwBuf);

}
VOS_VOID MED_AEC_HF_NlpGamma(
                MED_AEC_HF_NLP_SMOOTH_PSD_STRU *pstSmoothPSD,
                MED_INT32                      *paswSx)
{
    MED_INT32   swSum  = 0;
    MED_INT16  *pshwGammaCount;
    MED_INT16  *pshwGammaT;

    pshwGammaCount = &pstSmoothPSD->shwGammaCount;
    pshwGammaT     = &pstSmoothPSD->shwGammaT;

    swSum = CODEC_OpVecSum32(&paswSx[29], 131);

    /* 根据近端能量计数 */
    if (swSum < MED_AEC_HF_NLP_SMOOTH_PARA_POW_THD)
    {
        (*pshwGammaCount)++;
    }
    else
    {
        *pshwGammaCount = MED_AEC_HF_NLP_SMOOTH_PARA_GAMMA_CNT_MIN;
    }

    /* 快速滤波 */
    if (*pshwGammaCount <= MED_AEC_HF_NLP_SMOOTH_PARA_GAMMA_CNT_MIN)
    {
        *pshwGammaT     = MED_AEC_HF_NLP_SMOOTH_PARA_FAST;
        *pshwGammaCount = MED_AEC_HF_NLP_SMOOTH_PARA_GAMMA_CNT_MIN;
    }

    /* 慢速滤波 */
    if (*pshwGammaCount >= MED_AEC_HF_NLP_SMOOTH_PARA_GAMMA_CNT_MAX)
    {
        *pshwGammaT     = MED_AEC_HF_NLP_SMOOTH_PARA_SLOW;
        *pshwGammaCount = MED_AEC_HF_NLP_SMOOTH_PARA_GAMMA_CNT_MAX;
    }

}
VOS_VOID MED_AEC_HF_NlpSmoothPSD(
                MED_AEC_HF_NLP_SMOOTH_PSD_STRU  *pstSmoothPSD,
                MED_AEC_HF_NLP_TRANS_STRU       *pstTime2Freq,
                MED_INT16                        shwMaxFar)
{
    MED_INT32      swCntI;
    MED_INT32      swCntTmp;
    MED_INT16      shwBandLen;
    MED_INT16      shwFFTLenth;
    VOS_INT32      swSumPara1;
    VOS_INT32      swSumPara2;
    VOS_INT32      swSumPara3;
    VOS_INT32      swSumPara4;

    VOS_INT32     *pswSee       = pstSmoothPSD->aswSee;
    VOS_INT32     *pswSdd       = pstSmoothPSD->aswSdd;
    VOS_INT32     *pswSddOrg    = pstSmoothPSD->aswSddOrg;
    VOS_INT32     *pswSxx       = pstSmoothPSD->aswSxx;
    VOS_INT32     *pswSxd       = pstSmoothPSD->aswSxd;
    VOS_INT32     *pswSed       = pstSmoothPSD->aswSed;

    VOS_INT16     *pshwResdFreq = pstTime2Freq->ashwResdFreq;
    VOS_INT16     *pshwNearFreq = pstTime2Freq->ashwNearFreq;
    VOS_INT16     *pshwRefrFreq = pstTime2Freq->ashwRefrFreq;

    VOS_INT16     *pshwcohed    = pstSmoothPSD->ashwcohed;
    VOS_INT16     *pshwcohxd    = pstSmoothPSD->ashwcohxd;
    VOS_INT16     *pshwhnled    = pstSmoothPSD->ashwhnled;

    VOS_INT16      shwGammaT    = pstSmoothPSD->shwGammaT;
    VOS_INT16      shwGammaTTmp;
    VOS_INT16      shwGammaTSubTmp;
    MED_INT16      shwGammaTSub;

    VOS_INT16      shwTmp;

    CODEC_OP_FLOAT_STRU *pstCoSed;
    CODEC_OP_FLOAT_STRU *pstCoSxd;

    CODEC_OP_FLOAT_STRU  stFloatTmp1;
    CODEC_OP_FLOAT_STRU  stFloatTmp2;


    /* 滤波系数GammaT和1的差 */
    shwGammaTSub = 0x7FFF - shwGammaT;

    shwGammaTTmp     = shwGammaT;
    shwGammaTSubTmp  = shwGammaTSub;

    shwFFTLenth  = pstSmoothPSD->shwFFTLenth;
    shwBandLen   = (shwFFTLenth >> 1) + 1;

    /* 内存申请 */
    pstCoSed   = (CODEC_OP_FLOAT_STRU *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(CODEC_OP_FLOAT_STRU));
    pstCoSxd   = (CODEC_OP_FLOAT_STRU *)UCOM_MemAlloc((VOS_UINT16)shwBandLen * sizeof(CODEC_OP_FLOAT_STRU));

    /* 残差自功率谱，帧间平滑 */
    /* pswSee[0] = shwGammaT * pswSee[0] + shwGammaTSub * pshwResdFreq[0]*pshwResdFreq[0]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSee[0], MED_AEC_HF_NLP_SMOOTH_PARA_SLOW);
    swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[0], pshwResdFreq[0]);
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, MED_AEC_HF_NLP_SMOOTH_PARA_SLOW_SUB);
    pswSee[0]        = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* pswSee[max] = shwGammaT * pswSee[max] + shwGammaTSub * pshwResdFreq[1]*pshwResdFreq[1]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSee[shwBandLen - 1], shwGammaT);
    swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[1], pshwResdFreq[1]);
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSub);
    pswSee[shwBandLen - 1] = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* 近端自功率谱，帧间平滑 */
    /* pswSdd[0] = shwGammaT * pswSdd[0] + shwGammaTSub * pshwNearFreq[0]*pshwNearFreq[0]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSdd[0], MED_AEC_HF_NLP_SMOOTH_PARA_SLOW);
    swSumPara2       = CODEC_OpL_mult0(pshwNearFreq[0], pshwNearFreq[0]);
    pswSddOrg[0]     = swSumPara2;
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, MED_AEC_HF_NLP_SMOOTH_PARA_SLOW_SUB);
    pswSdd[0]        = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* pswSdd[max] = shwGammaT * pswSdd[max] + shwGammaTSub * pshwNearFreq[1]*pshwNearFreq[1]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSdd[shwBandLen - 1], shwGammaT);
    swSumPara2       = CODEC_OpL_mult0(pshwNearFreq[1], pshwNearFreq[1]);
    pswSddOrg[1]     = swSumPara2;
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSub);
    pswSdd[shwBandLen - 1] = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* 远端自功率谱，帧间平滑 */
    /* pswSxx[0] = shwGammaT * pswSxx[0] + shwGammaTSub * pshwRefrFreq[0]*pshwRefrFreq[0]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSxx[0], MED_AEC_HF_NLP_SMOOTH_PARA_SLOW);
    swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[0], pshwRefrFreq[0]);
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, MED_AEC_HF_NLP_SMOOTH_PARA_SLOW_SUB);
    pswSxx[0]        = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* pswSxx[max] = shwGammaT * pswSdd[max] + shwGammaTSub * pshwRefrFreq[1]*pshwNearFreq[1]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSxx[shwBandLen - 1], shwGammaT);
    swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[1], pshwRefrFreq[1]);
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSub);
    pswSxx[shwBandLen - 1] = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* [1...max-1] */
    for (swCntI = 1; swCntI < (shwBandLen - 1); swCntI++)
    {
        /* 残差自功率谱，帧间平滑pswSee[1...max-1] */
        swCntTmp         = 2 * swCntI;
        swSumPara1       = CODEC_OpL_mpy_32_16(pswSee[swCntI], shwGammaTTmp);
        swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[swCntTmp], pshwResdFreq[swCntTmp]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSee[swCntI]   = CODEC_OpL_add(swSumPara1, swSumPara2);
        swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[swCntTmp + 1], pshwResdFreq[swCntTmp + 1]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSee[swCntI]   = CODEC_OpL_add(pswSee[swCntI], swSumPara2);

        /* 近端自功率谱，帧间平滑pswSdd[1...max-1] */
        swSumPara1       = CODEC_OpL_mpy_32_16(pswSdd[swCntI], shwGammaTTmp);
        swSumPara2       = CODEC_OpL_mult0(pshwNearFreq[swCntTmp], pshwNearFreq[swCntTmp]);
        swSumPara3       = swSumPara2;
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSdd[swCntI]   = CODEC_OpL_add(swSumPara1, swSumPara2);
        swSumPara2       = CODEC_OpL_mult0(pshwNearFreq[swCntTmp + 1], pshwNearFreq[swCntTmp + 1]);
        swSumPara4       = swSumPara2;
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSdd[swCntI]   = CODEC_OpL_add(pswSdd[swCntI], swSumPara2);
        pswSddOrg[swCntI + 1]
                         = CODEC_OpL_add(swSumPara3, swSumPara4);

        /* 远端自功率谱，帧间平滑 pswSxx[1...max-1] */
        swSumPara1       = CODEC_OpL_mpy_32_16(pswSxx[swCntI], shwGammaTTmp);
        swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[swCntTmp], pshwRefrFreq[swCntTmp]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSxx[swCntI]   = CODEC_OpL_add(swSumPara1, swSumPara2);
        swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[swCntTmp + 1], pshwRefrFreq[swCntTmp + 1]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSxx[swCntI]   = CODEC_OpL_add(pswSxx[swCntI], swSumPara2);
    }


    /* 远端信号和近端信号互功率谱，帧间平滑 */
    /* pswSxd[0] = shwGammaT * pswSxd[0] + shwGammaTSub * pshwRefrFreq[0]*ashwNearFreq[0]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSxd[0], MED_AEC_HF_NLP_SMOOTH_PARA_SLOW);
    swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[0], pshwNearFreq[0]);
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, MED_AEC_HF_NLP_SMOOTH_PARA_SLOW_SUB);
    pswSxd[0]        = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* pswSxd[1] = shwGammaT * pswSxd[1] + shwGammaTSub * pshwRefrFreq[1]*ashwNearFreq[1]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSxd[1], shwGammaT);
    swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[1], pshwNearFreq[1]);
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSub);
    pswSxd[1]        = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* pswSxd[2...max] */
    for (swCntI = 2; swCntI < shwFFTLenth; swCntI = swCntI + 2)
    {
        swSumPara1       = CODEC_OpL_mpy_32_16(pswSxd[swCntI], shwGammaTTmp);
        swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[swCntI], pshwNearFreq[swCntI]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSxd[swCntI]   = CODEC_OpL_add(swSumPara1, swSumPara2);
        swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[swCntI + 1], pshwNearFreq[swCntI + 1]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSxd[swCntI]   = CODEC_OpL_add(pswSxd[swCntI], swSumPara2);

        swSumPara1       = CODEC_OpL_mpy_32_16(pswSxd[swCntI + 1], shwGammaTTmp);
        swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[swCntI], pshwNearFreq[swCntI + 1]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSxd[swCntI + 1]= CODEC_OpL_sub(swSumPara1, swSumPara2);
        swSumPara2       = CODEC_OpL_mult0(pshwRefrFreq[swCntI + 1], pshwNearFreq[swCntI]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSxd[swCntI + 1]= CODEC_OpL_add(pswSxd[swCntI + 1], swSumPara2);
    }

    /* 残差信号和近端信号互功率谱，帧间平滑 */
    /* pswSed[0] = shwGammaT * pswSed[0] + shwGammaTSub * pshwResdFreq[0]*ashwNearFreq[0]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSed[0], MED_AEC_HF_NLP_SMOOTH_PARA_SLOW);
    swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[0], pshwNearFreq[0]);
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, MED_AEC_HF_NLP_SMOOTH_PARA_SLOW_SUB);
    pswSed[0]        = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* pswSed[1] = shwGammaT * pswSed[1] + shwGammaTSub * pshwResdFreq[1]*ashwNearFreq[1]; */
    swSumPara1       = CODEC_OpL_mpy_32_16(pswSed[1], shwGammaT);
    swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[1], pshwNearFreq[1]);
    swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSub);
    pswSed[1]        = CODEC_OpL_add(swSumPara1, swSumPara2);

    /* pswSed[2...max] */
    for (swCntI = 2; swCntI < shwFFTLenth; swCntI = swCntI + 2)
    {
        swSumPara1       = CODEC_OpL_mpy_32_16(pswSed[swCntI], shwGammaTTmp);
        swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[swCntI], pshwNearFreq[swCntI]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSed[swCntI]   = CODEC_OpL_add(swSumPara1, swSumPara2);
        swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[swCntI + 1], pshwNearFreq[swCntI + 1]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSed[swCntI]   = CODEC_OpL_add(pswSed[swCntI], swSumPara2);

        swSumPara1       = CODEC_OpL_mpy_32_16(pswSed[swCntI + 1], shwGammaTTmp);
        swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[swCntI], pshwNearFreq[swCntI + 1]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSed[swCntI + 1]= CODEC_OpL_sub(swSumPara1, swSumPara2);
        swSumPara2       = CODEC_OpL_mult0(pshwResdFreq[swCntI + 1], pshwNearFreq[swCntI]);
        swSumPara2       = CODEC_OpL_mpy_32_16(swSumPara2, shwGammaTSubTmp);
        pswSed[swCntI + 1]= CODEC_OpL_add(pswSed[swCntI + 1], swSumPara2);
    }

    /* 计算pstCoSed = Sed.*conj(Sed) */
    pstCoSed[0]                  = CODEC_OpFloatAMul32U(pswSed[0], pswSed[0]);
    pstCoSed[shwBandLen - 1]     = CODEC_OpFloatAMul32U(pswSed[1], pswSed[1]);

    /* 计算pswCoSxd = Sxd.*conj(Sxd) */
    pstCoSxd[0]               = CODEC_OpFloatAMul32U(pswSxd[0], pswSxd[0]);
    pstCoSxd[shwBandLen - 1]  = CODEC_OpFloatAMul32U(pswSxd[1], pswSxd[1]);

    /* [1...max-1] */
    for (swCntI = 1; swCntI < (shwBandLen - 1); swCntI++)
    {
        /* pswCoSed[1...max-1] */
        swCntTmp         = 2 * swCntI;
        stFloatTmp1      = CODEC_OpFloatAMul32U(pswSed[swCntTmp], pswSed[swCntTmp]);
        stFloatTmp2      = CODEC_OpFloatAMul32U(pswSed[swCntTmp + 1], pswSed[swCntTmp + 1]);
        pstCoSed[swCntI] = CODEC_OpFloatAdd(stFloatTmp1, stFloatTmp2);

        /* pswCoSxd[1...max-1] */
        stFloatTmp1      = CODEC_OpFloatAMul32U(pswSxd[swCntTmp], pswSxd[swCntTmp]);
        stFloatTmp2      = CODEC_OpFloatAMul32U(pswSxd[swCntTmp + 1], pswSxd[swCntTmp + 1]);
        pstCoSxd[swCntI] = CODEC_OpFloatAdd(stFloatTmp1, stFloatTmp2);
    }

    /* 残差和近端信号相干系数
       远端和近端信号相干系数*/
    for (swCntI = 0; swCntI < shwBandLen; swCntI++)
    {
        stFloatTmp1       = CODEC_OpFloatAMul32U(pswSee[swCntI], pswSdd[swCntI]);
        stFloatTmp1.shwE  = stFloatTmp1.shwE - 15;
        stFloatTmp1.shwM  = (0 == stFloatTmp1.shwM) ? 512 : stFloatTmp1.shwM;
        pshwcohed[swCntI] = CODEC_OpFloatTo16(CODEC_OpFloatDivU(pstCoSed[swCntI], stFloatTmp1));

        pswSxx[swCntI]    = (0 == pswSxx[swCntI]) ? 1 : pswSxx[swCntI];
        stFloatTmp1       = CODEC_OpFloatAMul32U(pswSxx[swCntI], pswSdd[swCntI]);
        stFloatTmp1.shwE  = stFloatTmp1.shwE - 15;
        stFloatTmp1.shwM  = (0 == stFloatTmp1.shwM) ? 2 : stFloatTmp1.shwM;
        pshwcohxd[swCntI] = CODEC_OpFloatTo16(CODEC_OpFloatDivU(pstCoSxd[swCntI], stFloatTmp1));
    }

    pstSmoothPSD->shwMaxFarSmooth  = CODEC_OpAdd(shwMaxFar >> 1, pstSmoothPSD->shwMaxFarSmooth >> 1);

    if (pstSmoothPSD->shwMaxFarSmooth > MED_AEC_HF_SMOOTH_GAIN_TH)
    {
        CODEC_OpVecCpy(&pshwhnled[0], &pshwcohed[0], MED_AEC_HF_SMOOTH_HIGH_FREQ_TH);

        for (swCntI = MED_AEC_HF_SMOOTH_HIGH_FREQ_TH; swCntI < shwBandLen; swCntI++)
        {
            shwTmp            = CODEC_OpSub(0x7fff, pshwcohxd[swCntI]);
            pshwhnled[swCntI] = CODEC_OpMin(shwTmp, pshwcohed[swCntI]);
        }

    }
    else
    {
        CODEC_OpVecCpy(&pshwhnled[0], &pshwcohed[0], shwBandLen);

        for (swCntI = MED_AEC_HF_SMOOTH_HI_GAIN_FREQ_NUM; swCntI < shwFFTLenth; swCntI++)
        {
            if (CODEC_OpAbs_s(pshwRefrFreq[swCntI])> MED_AEC_HF_SMOOTH_HI_GAIN_FREQ_TH)
            {
                shwTmp    = CODEC_OpSub(0x7fff, pshwcohxd[(swCntI >> 1) - 1]);
                 pshwhnled[(swCntI >> 1) - 1]
                          = CODEC_OpMin(shwTmp, pshwcohed[(swCntI >> 1) - 1]);

                shwTmp    = CODEC_OpSub(0x7fff, pshwcohxd[swCntI >> 1]);
                pshwhnled[swCntI >> 1]
                          = CODEC_OpMin(shwTmp, pshwcohed[swCntI >> 1]);

                shwTmp    = CODEC_OpSub(0x7fff, pshwcohxd[(swCntI >> 1) + 1]);
                pshwhnled[(swCntI >> 1) + 1]
                          = CODEC_OpMin(shwTmp, pshwcohed[(swCntI >> 1) + 1]);
            }
        }
    }

    UCOM_MemFree(pstCoSxd);
    UCOM_MemFree(pstCoSed);

}


VOS_VOID MED_AEC_HF_NlpBandSort(
                VOS_INT16                       *pshwCohed,
                VOS_INT16                       *pshwCohxd,
                VOS_INT16                       *pshwHnled,
                MED_AEC_HF_NLP_BAND_SORT_STRU   *pstBandSort)
{
    VOS_INT16           shwCntI;
    VOS_INT16           shwTemp;
    VOS_INT16           ashwHnled[MED_AEC_HF_ECHO_BIN_RANGE_WB];
    VOS_INT32           swTemp;

    CODEC_OpVecCpy(ashwHnled,
                   &pshwHnled[pstBandSort->shwBinStart],
                   pstBandSort->shwEchoRange);

    /*Cohed的均值*/
    swTemp = 0;
    for ( shwCntI = 0 ; shwCntI < pstBandSort->shwEchoRange ; shwCntI++ )
    {
        shwTemp  = pstBandSort->shwBinStart + shwCntI;
        swTemp  += pshwCohed[shwTemp];
    }
    pstBandSort->shwCohedMean  = (VOS_INT16)(swTemp / pstBandSort->shwEchoRange);

    /*Cohxd的均值*/
    swTemp = 0;
    for ( shwCntI = 0 ; shwCntI < pstBandSort->shwEchoRange ; shwCntI++ )
    {
        shwTemp  = pstBandSort->shwBinStart + shwCntI;
        swTemp  += ( MED_AEC_HF_SHW_MAX - pshwCohxd[shwTemp]);
    }

    pstBandSort->shwHnlSortQ    = (VOS_INT16)(swTemp / pstBandSort->shwEchoRange);

    /*Hnled排序*/
    MED_AEC_HF_NlpShortSort(ashwHnled, &ashwHnled[pstBandSort->shwEchoRange-1]);

    pstBandSort->shwHnlPrefAvg                 = ashwHnled[pstBandSort->shwHnlIdx];
    pstBandSort->shwHnlPrefAvgLow              = ashwHnled[pstBandSort->shwHnlIdxLow];

}
 VOS_VOID MED_AEC_HF_NlpSwap(VOS_INT16 *pshwTempa, VOS_INT16 *pshwTempb)
{
    VOS_INT16                shwTemp;

    if ( pshwTempa != pshwTempb )
    {
        shwTemp     = *pshwTempa;
       *pshwTempa   = *pshwTempb;
       *pshwTempb   =  shwTemp;
    }
}


VOS_VOID MED_AEC_HF_NlpShortSort(
                VOS_INT16           *pshwLow,
                VOS_INT16           *pshwHigh)
{
    VOS_INT16                               *pshwCntI;
    VOS_INT16                               *pshwMax;
    VOS_INT16                                shwCntI;
    VOS_INT16                                shwTempNum;

    /*短序列的长度*/
    shwTempNum  = (VOS_INT16)( (pshwHigh - pshwLow) + 1 );

    for ( shwCntI = 0 ; shwCntI < shwTempNum; shwCntI++ )
    {
        /*找最大值*/
        pshwMax = pshwLow;

        for ( pshwCntI = pshwLow ; pshwCntI <= pshwHigh; pshwCntI++ )
        {
            pshwMax = (*pshwCntI > *pshwMax) ? pshwCntI : pshwMax;
        }

        /*最大值与最高位互换, 最高位地址递减*/
        MED_AEC_HF_NlpSwap(pshwMax, pshwHigh);
        pshwHigh--;
    }
}
VOS_VOID  MED_AEC_HF_NlpOverdrive(
                VOS_INT16                      *pshwCohxd,
                VOS_INT16                      *pshwCohed,
                MED_AEC_HF_NLP_BAND_SORT_STRU  *pstBandSort,
                MED_AEC_HF_NLP_OVER_DRIVE_STRU *pstOverDrive,
                VOS_INT16                      *pshwHnled)
{
    VOS_INT16                           shwCohedMean;
    VOS_INT16                           shwHnlSortQ;
    VOS_INT16                           shwHnlPrefAvg;
    VOS_INT16                           shwHnlPrefAvgLow;
    VOS_INT16                           shwCntI;
    VOS_INT16                           shwTemp;
    VOS_INT16                           shwTemp2;
    VOS_INT32                           swTemp;
    VOS_UINT32                          uwCount;

    shwCohedMean     = pstBandSort->shwCohedMean;
    shwHnlSortQ      = pstBandSort->shwHnlSortQ;
    shwHnlPrefAvg    = pstBandSort->shwHnlPrefAvg;
    shwHnlPrefAvgLow = pstBandSort->shwHnlPrefAvgLow;

    shwTemp = (shwHnlSortQ < pstOverDrive->shwCohxdLocalMin)
              && (shwHnlSortQ < MED_AEC_HF_SORT_MIN);

    if ( shwTemp )
    {
        pstOverDrive->shwCohxdLocalMin = shwHnlSortQ;
    }

    /* always undone*/
    if ( MED_AEC_HF_SHW_MAX == pstOverDrive->shwCohxdLocalMin)
    {
        pstOverDrive->shwOvrd       = MED_AEC_HF_OVRD_MIN;
        shwHnlPrefAvg               = shwHnlSortQ;
        shwHnlPrefAvgLow            = shwHnlSortQ;
        for ( shwCntI = 0 ; shwCntI < pstBandSort->shwEchoLeng ; shwCntI++ )
        {
            pshwHnled[shwCntI]      = CODEC_OpSub( MED_AEC_HF_SHW_MAX, pshwCohxd[shwCntI]);
        }

    }

    shwTemp = (shwCohedMean > MED_AEC_HF_COHED_MEAN_HIGH)
              && (shwHnlSortQ  > MED_AEC_HF_SORT_HIGH);
    if ( shwTemp )
    {
        pstOverDrive->shwSuppState = 0;
    }
    shwTemp = (shwCohedMean < MED_AEC_HF_COHED_MEAN_LOW)
              || (shwHnlSortQ  < MED_AEC_HF_SORT_LOW);
    if ( shwTemp )
    {
        pstOverDrive->shwSuppState = 1;
    }

    if ( 0 == pstOverDrive->shwSuppState )
    {
        shwHnlPrefAvg     = shwCohedMean;
        shwHnlPrefAvgLow  = shwCohedMean;
        uwCount           = ((VOS_UINT16)(pstBandSort->shwEchoLeng))*sizeof(VOS_INT16);
        UCOM_MemCpy(pshwHnled, pshwCohed, uwCount);
    }

    shwTemp = (shwHnlPrefAvgLow < pstOverDrive->shwHnlLocalMin)
              && (shwHnlPrefAvgLow < MED_AEC_HF_PREAVG_LOW);
    if ( shwTemp )
    {
        pstOverDrive->shwHnlLocalMin = shwHnlPrefAvgLow;
        pstOverDrive->swHnlMin       = (VOS_INT32)shwHnlPrefAvgLow;
        pstOverDrive->shwHnlNewMin   = MED_AEC_HF_SHW_MAX;
        pstOverDrive->swHnlMinCtr    = 0;
    }

    if ( MED_AEC_HF_SHW_MAX == pstOverDrive->shwHnlNewMin )
    {
        pstOverDrive->swHnlMinCtr   = CODEC_OpL_add(pstOverDrive->swHnlMinCtr, MED_AEC_HF_SHW_MAX);
    }

    if ( MED_AEC_HF_OVRD_MINCTR_MAX == pstOverDrive->swHnlMinCtr )
    {
        pstOverDrive->shwHnlNewMin   = 0;
        pstOverDrive->swHnlMinCtr    = 0;

        /*
          swTemp  = max(1024*log(0.00001)/(1024*log(stOverdrive.dhnlMin) ),3)
          下列语句实现如上功能
        */
        shwTemp = (pstOverDrive->swHnlMin >= MED_AEC_HF_SHW_MAX)
                  || (pstOverDrive->swHnlMin <= MED_AEC_HF_OVRD_LOG_CONST);
        if ( shwTemp )
        {
            pstOverDrive->shwOvrd = MED_AEC_HF_OVRD_MIN;
        }
        else
        {
            shwTemp  = -CODEC_OpLog10( MED_PP_SHIFT_BY_15, pstOverDrive->swHnlMin);
            swTemp   = MED_AEC_HF_OVRD_LOG_DIVED << MED_PP_SHIFT_BY_11;
            shwTemp2 = CODEC_OpSaturate(swTemp / shwTemp);

            pstOverDrive->shwOvrd = CODEC_OpMax(shwTemp2, MED_AEC_HF_OVRD_MIN);
        }

    }

    swTemp = ( pstOverDrive->shwHnlLocalMin + ( MED_AEC_HF_OVRD_MULT / pstOverDrive->shwMult ) );
    pstOverDrive->shwHnlLocalMin     = (VOS_INT16)CODEC_OpL_min(swTemp, MED_AEC_HF_SHW_MAX);
    swTemp = ( pstOverDrive->shwCohxdLocalMin + ( MED_AEC_HF_OVRD_HALF_MULT / pstOverDrive->shwMult )) ;
    pstOverDrive->shwCohxdLocalMin   = (VOS_INT16)CODEC_OpL_min(swTemp, MED_AEC_HF_SHW_MAX);

    if ( pstOverDrive->shwOvrd < pstOverDrive->shwOvrdSm )
    {
        shwTemp                  = CODEC_OpMult_r(pstOverDrive->shwOvrdSm, MED_AEC_HF_OVRD_HIGH);
        shwTemp2                 = CODEC_OpMult_r(pstOverDrive->shwOvrd, (MED_AEC_HF_SHW_MAX - MED_AEC_HF_OVRD_HIGH));
        pstOverDrive->shwOvrdSm  = CODEC_OpAdd(shwTemp, shwTemp2);
    }
    else
    {
        shwTemp                  = CODEC_OpMult_r(pstOverDrive->shwOvrdSm, MED_AEC_HF_OVRD_LOW) ;
        shwTemp2                 = CODEC_OpMult_r(pstOverDrive->shwOvrd, (MED_AEC_HF_SHW_MAX - MED_AEC_HF_OVRD_LOW));
        pstOverDrive->shwOvrdSm  = CODEC_OpAdd(shwTemp, shwTemp2);
    }

    pstBandSort->shwHnlPrefAvg    = shwHnlPrefAvg;
    pstBandSort->shwHnlPrefAvgLow = shwHnlPrefAvgLow;
}



VOS_VOID MED_AEC_HF_NlpDivergeProc(
                VOS_INT32 *pswErrPSD,
                VOS_INT32 *pswNearPSD,
                VOS_INT16 *pshwNearFreq,
                VOS_INT16 *pshwErrFreq,
                VOS_INT16 *shwDivergeState,
                VOS_INT32 *pswWeight,
                VOS_INT16  shwFftLen )
{
    VOS_INT32               shwIdx;
    VOS_INT32               swErrPow      = 0;
    VOS_INT32               swNearPow     = 0;
    VOS_INT32               swErrPowTemp;
    VOS_INT32               swNearPowTemp;
    VOS_INT16               shwBinLen     = (shwFftLen >> 1) + 1;
    VOS_INT16               shwDynmcShift = 0;

    /* 计算残差和近场信号互谱能量 */
    for ( shwIdx = 0; shwIdx < shwBinLen; shwIdx++ )
    {
        /* swErrPow  = CODEC_OpL_add( swErrPow, pswErrPSD[shwIdx]   >> 7 );
        swNearPow = CODEC_OpL_add( swNearPow, pswNearPSD[shwIdx] >> 7 );*/
        swErrPow  = CODEC_OpL_add( swErrPow,
                                   pswErrPSD[shwIdx] >> shwDynmcShift );
        swNearPow = CODEC_OpL_add( swNearPow,
                                   pswNearPSD[shwIdx] >> shwDynmcShift );

        /* 动态右移方案 */
        if ((swErrPow > MED_AEC_HF_NLP_DYNAMICSHIFT_TH)
            || (swNearPow > MED_AEC_HF_NLP_DYNAMICSHIFT_TH) )
        {
            swErrPow     >>= 1;
            swNearPow    >>= 1;
            shwDynmcShift += 1;
        }
    }

    /* 发散状态判断 */
    if ( 0 == *shwDivergeState )
    {
        /* 若上帧为发散且本帧残差能量大于近场能量，则本帧判为发散 */
        /* 残差信号即为近场信号 */
        if ( swErrPow > swNearPow )
        {
            CODEC_OpVecCpy( pshwErrFreq,
                            pshwNearFreq,
                            (VOS_INT32) MED_AEC_MAX_FFT_LEN );
            *shwDivergeState = 1;
         }
    }
    else
    {
        /* 残差能量乘以系数1.05 */
        swErrPowTemp = CODEC_OpL_mpy_32_16( swErrPow,
                                            MED_AEC_HF_NLP_ERRPOW_FAC );
        swErrPowTemp = CODEC_OpL_shl( swErrPowTemp, MED_PP_SHIFT_BY_1 );
        if ( swErrPowTemp < swNearPow )
        {
            /* 若上帧为收敛且本帧残差能量(乘以1.05)小于近场能量，则本帧判为收敛 */
            *shwDivergeState = 0;
        }
        else
        {
            /* 若上帧为收敛且本帧残差能量(乘以1.05)大于近场能量，则本帧判为发散 */
            /* 残差信号即为近场信号 */
            CODEC_OpVecCpy( pshwErrFreq,
                            pshwNearFreq,
                            (VOS_INT32) MED_AEC_MAX_FFT_LEN );
        }
    }

    /* 近场能量乘以系数19.95 */
    swNearPowTemp = CODEC_OpL_mpy_32_16( swNearPow,
                                         MED_AEC_HF_NLP_NEARPOW_FAC );
    swNearPowTemp = CODEC_OpL_shl( swNearPowTemp, MED_PP_SHIFT_BY_5 );

    /* 若本帧残差能量远大于近场能量(乘以19.95) */
    if ( swErrPow > swNearPowTemp )
    {
        /* 滤波器系数置零 */
        UCOM_MemSet( &pswWeight[0], 0, (MED_AEC_HF_AF_M * MED_AEC_MAX_FFT_LEN) * sizeof(VOS_INT32));
    }
}
VOS_VOID MED_AEC_HF_NlpSmoothGain(
                MED_AEC_HF_NLP_SMOOTH_GAIN_STRU *pstSmoothGain,
                VOS_INT16                        shwPrefAvg,
                VOS_INT16                       *pashwAdhnled,
                VOS_INT16                        shwDovrdSm,
                VOS_INT16                        shwFftLen,
                VOS_INT16                        shwGamma,
                VOS_INT16                       *pshwErrFreq,
                VOS_INT16                        shwMaxFar)
{
    VOS_INT16               shwBinLen = (shwFftLen >> 1) + 1;
    VOS_INT16               shwIdx;
    VOS_INT16               shwA;
    VOS_INT16               shwB;
    VOS_INT16               shwAdodTmp;
    VOS_INT16               shwRefCntThd;                                       /* 远端信号计数 */
    VOS_INT16               shwRefAmp1;                                         /* 远端信号能量门限1，用于抑制第一声回声 */
    VOS_INT16              *pashwWeight;
    VOS_INT16              *pashwFreq;

    shwRefCntThd  = pstSmoothGain->shwRefCntThd;
    shwRefAmp1    = pstSmoothGain->shwRefAmp1;
    pashwWeight   = pstSmoothGain->pashwWeight;
    pashwFreq     = pstSmoothGain->pashwFreq;

    if ((shwMaxFar > shwRefAmp1) && (pstSmoothGain->shwRefCnt < MED_AEC_MAX_NLP_OVERDRIVE_FAR_CNT))
    {
        pstSmoothGain->shwRefCnt++;
    }
    /* 计算各频点增益 */
    for ( shwIdx = 0; shwIdx < shwBinLen; shwIdx++ )
    {

        if ( shwPrefAvg < pashwAdhnled[shwIdx] )
        {
            shwA = CODEC_OpMult(pashwWeight[shwIdx], shwPrefAvg);
            shwB = CODEC_OpMult(CODEC_OpSub(MED_AEC_HF_SHW_MAX,
                                            pashwWeight[shwIdx]),
                                pashwAdhnled[shwIdx]);
            pashwAdhnled[shwIdx] = CODEC_OpAdd( shwA, shwB );
        }

        /* 计算各频点增益:如果shwNvdod是负数，则计算幂指数；否则，幂指数由shwNvdod指定*/
        if  ( pstSmoothGain->shwOvrdod < 0 )
        {
            shwAdodTmp           = CODEC_OpMult(shwDovrdSm, pashwFreq[shwIdx]); /* Q10 */
            shwAdodTmp           = shwAdodTmp - (MED_AEC_HF_OVRD_SUB >> 1);
            shwAdodTmp           = CODEC_OpMax(shwAdodTmp,
                                               (MED_AEC_HF_OVRD_MIN >> 1));

            if (  (pstSmoothGain->shwRefCnt < shwRefCntThd)
                &&(pstSmoothGain->shwRefCnt > 0))
            {
                shwAdodTmp       = pstSmoothGain->shwExOverdrive;
            }
        }
        else
        {
              shwAdodTmp         = pstSmoothGain->shwOvrdod;
        }

        {
              pashwAdhnled[shwIdx] = CODEC_OpSaturate(CODEC_OpL_Power(
                                pashwAdhnled[shwIdx],
                                MED_PP_SHIFT_BY_15,
                                shwAdodTmp,
                                MED_PP_SHIFT_BY_9,
                                MED_PP_SHIFT_BY_15));

        }
    }

    /* 各频点乘以增益 */
    pshwErrFreq[0] = CODEC_OpMult(pashwAdhnled[0], pshwErrFreq[0]);
    pshwErrFreq[1] = CODEC_OpMult(pashwAdhnled[1], pshwErrFreq[1]);

    for ( shwIdx = 2; shwIdx < shwFftLen; shwIdx =  shwIdx + 2)
    {
        pshwErrFreq[shwIdx]   = CODEC_OpMult(pashwAdhnled[(shwIdx>>1)+1],
                                         pshwErrFreq[shwIdx]);
        pshwErrFreq[shwIdx+1] = CODEC_OpMult(pashwAdhnled[(shwIdx>>1)+1],
                                         pshwErrFreq[shwIdx+1]);
    }

}


VOS_VOID MED_AEC_HF_NlpInsertCN(
                    MED_AEC_HF_NLP_CN_STRU          *pstCn,
                    VOS_INT16                        shwFftLen,
                    VOS_INT16                       *pshwErrFreq,
                    VOS_INT16                       *pashwAdhnled)
{
    VOS_INT32                           *pashwSym       = &pstCn->aswPsdBin[0];
    CODEC_SWITCH_ENUM_UINT16             enCnFlag       =  pstCn->enCnFlag;
    VOS_INT16                            shwIdx;
    VOS_INT16                            swSymSqrt;
    VOS_INT16                            shwBinLen      = (shwFftLen >> 1) + 1;
    VOS_INT32                            ashwAdhnledSqr;
    VOS_INT32                            ashwAdhnledSub;
    VOS_INT16                            ashwGain;
    VOS_INT16                            shwUnReal;
    VOS_INT16                            shwUnImag;
    VOS_INT16                           *pshwRand;
    VOS_INT32                            swNoisePow;
    VOS_INT16                            shwDynmcShift;
    VOS_INT32                            shwRandSquared;
    VOS_INT16                            shwNoisePowDenom;
    VOS_INT16                            shwNoiseLen    = shwFftLen + 2;
    VOS_INT16                            shwNoiseEng;
    VOS_INT16                            shwNormalize   = pstCn->shwNormalize;

    if ( CODEC_SWITCH_ON == enCnFlag )
    {
        pshwRand = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwNoiseLen
                                                          * sizeof(VOS_INT16));
        swNoisePow       = CODEC_OpL_mult0( MED_AEC_HF_SHW_MAX,
                                            MED_AEC_HF_SHW_MAX );               /* 奈奎斯特频点的噪声为实数1，则初始值为32767*32767 */
        shwNoisePowDenom = 0;
        shwDynmcShift    = 0;

        /* 计算一帧随机噪声的能量 */
        for ( shwIdx = 4; shwIdx < shwNoiseLen; shwIdx++ )
        {
            pshwRand[shwIdx] = (VOS_INT16) (rand() & 0xffff);/* [false alarm]:经确认此处使用rand无问题  */
            shwRandSquared   = CODEC_OpL_mult0( pshwRand[shwIdx],
                                              pshwRand[shwIdx] );               /*Q30*/
            swNoisePow       = CODEC_OpL_add( swNoisePow,
                               CODEC_OpL_shr( shwRandSquared, shwDynmcShift ));
            /* 动态右移 */
            if ( swNoisePow > MED_AEC_HF_NLP_INSERTCN_DYNAMICSHIFT_TH )
            {
                swNoisePow    >>= 1;
                shwDynmcShift  += 1;
            }
        }

        /* 为了简化2的n次方开方，需要保证n为偶数: sqrt(2^n)=2^(n/2) */
        if ( (shwDynmcShift % 2) !=0 )
        {
            swNoisePow    >>= 1;
            shwDynmcShift  += 1;
        }

        if ( swNoisePow > 0 )
        {
            /* 求噪声平均能量 */
            swNoisePow  = CODEC_OpL_mpy_32_16( swNoisePow, shwNormalize );
            /* 对平均能量开方 */
            shwNoiseEng = CODEC_OpSqrt( swNoisePow );
            /* 由于移位数shwDynmcShift为偶数，所以开方后只左移shwDynmcShift/2 */
            shwNoiseEng = CODEC_OpShl( shwNoiseEng, (shwDynmcShift >> 1) );     /*Q15*/
            /* 对该帧噪声平均能量取倒数 */
            shwNoisePowDenom = CODEC_OpDiv_s( MED_AEC_HF_NLP_INSERTCN_Q13_MAX,
                                              shwNoiseEng );                    /*Q13*/
        }

        /* 在奈奎斯特频点插入噪声 */
        /* 对该频点信号能量开方 */
        swSymSqrt = 0;
        if ( pashwSym[1] >= 0 )
        {
            swSymSqrt = CODEC_OpSqrt( pashwSym[1] );
        }
        /* 计算滤波系数sqrt(1-adhnled.^2) */
        ashwAdhnledSqr = CODEC_OpL_mult0( pashwAdhnled[1],
                                          pashwAdhnled[1] );                    /*Q30*/
        ashwAdhnledSub = CODEC_OpL_sub( MED_AEC_HF_NLP_INSERTCN_Q30_MAX,
                                        ashwAdhnledSqr );
        ashwGain       = CODEC_OpSqrt( ashwAdhnledSub );                        /*Q15*/
        /* 噪声能量归一化 */
        ashwGain       = CODEC_OpMult( ashwGain, shwNoisePowDenom );            /*Q13*/
        /* 乘以信号能量 */
        ashwGain       = CODEC_OpMult( ashwGain, swSymSqrt );                   /*Q-2*/
        ashwGain       = CODEC_OpShl( ashwGain,
                                      MED_PP_SHIFT_BY_2 );                      /*Q0*/
        /* 噪声与该频点信号相加 */
        pshwErrFreq[1] = CODEC_OpAdd(pshwErrFreq[1], ashwGain );


        /* 在其他频点插入噪声 */
        for ( shwIdx = 2; shwIdx < shwBinLen; shwIdx++ )
        {
            /* 对各频点信号能量开方 */
            swSymSqrt = 0;
            if ( pashwSym[shwIdx] >= 0 )
            {
                swSymSqrt = CODEC_OpSqrt( pashwSym[shwIdx] );
            }

            /* 计算滤波系数sqrt(1-adhnled.^2) */
            ashwAdhnledSqr = CODEC_OpL_mult0( pashwAdhnled[shwIdx],
                                              pashwAdhnled[shwIdx] );           /*Q30*/
            ashwAdhnledSub = CODEC_OpL_sub( MED_AEC_HF_NLP_INSERTCN_Q30_MAX,
                                            ashwAdhnledSqr );
            ashwGain       = CODEC_OpSqrt( ashwAdhnledSub );                    /*Q15*/

            /* 噪声能量归一化 */
            ashwGain       = CODEC_OpMult( ashwGain, shwNoisePowDenom );        /*Q13*/

            /* 乘以信号能量 */
            ashwGain       = CODEC_OpMult( ashwGain, swSymSqrt );               /*Q-2*/
            ashwGain       = CODEC_OpShl( ashwGain,
                                          MED_PP_SHIFT_BY_2 );                  /*Q0*/

            shwUnReal      = CODEC_OpMult( ashwGain, pshwRand[shwIdx<<1] );     /*Q0*/
            shwUnImag      = CODEC_OpMult( ashwGain, pshwRand[(shwIdx<<1)+1] ); /*Q0*/

            /* 噪声与各频点信号相加 */
            pshwErrFreq[(shwIdx<<1)-2] = CODEC_OpAdd(pshwErrFreq[(shwIdx<<1)-2],
                                                     shwUnReal );
            pshwErrFreq[(shwIdx<<1)-1] = CODEC_OpAdd(pshwErrFreq[(shwIdx<<1)-1],
                                                     shwUnImag );

        }

        UCOM_MemFree(pshwRand);

    }
}



VOS_VOID MED_AEC_HF_NlpFreq2Time(
                    MED_AEC_HF_NLP_TRANS_STRU       *pstTrans,
                    VOS_INT16                       *pshwResdFreq,
                    VOS_INT16                       *pshwOut)
{
    VOS_INT16                *pshwWin             =  pstTrans->pshwWin;
    VOS_INT16                *pshwResdFrm         =  pstTrans->ashwResdFrm;
    VOS_INT16                 shwFrameLen         =  pstTrans->shwFrmLen;
    VOS_INT16                 shwFftLen           =  pstTrans->shwFftLen;
    MED_FFT_NUM_ENUM_INT16    enIfftNumIndex      =  pstTrans->enIfftNumIndex;
    VOS_INT16                *pshwBuf             =  MED_NULL;

    /* 内存申请 */
    pshwBuf = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFftLen * sizeof(VOS_INT16));

    /* 频时变换 */
    MED_FFT_Ifft(enIfftNumIndex, pshwResdFreq, pshwBuf);

    /* 加汉宁窗 */
    CODEC_OpVvMult(pshwBuf, pshwWin, shwFftLen, pshwBuf);

    /* 当前帧和上一帧加窗残差时域重叠部分信号相加 */
    CODEC_OpVvAdd(pshwBuf, pshwResdFrm + shwFrameLen, shwFrameLen, pshwOut);

    /* 加窗残差时域信号作为历史值 */
    CODEC_OpVecCpy(pshwResdFrm, pshwBuf, shwFftLen);

    /* 内存释放 */
    UCOM_MemFree(pshwBuf);
}




VOS_VOID MED_AEC_HF_NlpNoisePower(
                    MED_AEC_HF_NLP_CN_STRU          *pstCn,
                    VOS_INT32                       *pswSddOrg)
{
    VOS_INT16                            shwIdxI;
    VOS_INT16                            shwBinLen      =  pstCn->shwBinLen;
    CODEC_SWITCH_ENUM_UINT16             enCnFlag       =  pstCn->enCnFlag;
    VOS_INT32                           *pswNearPsdBin  = &pstCn->aswNearPsdBin[0];
    VOS_INT32                           *pswPsdBin      = &pstCn->aswPsdBin[0];
    VOS_INT32                           *pswBuf         =  MED_NULL;
    VOS_INT32                           *pswTmp         =  MED_NULL;
    VOS_INT32                           *pswSdd         =  pswSddOrg;

    if (CODEC_SWITCH_ON == enCnFlag)
    {
        /* 内存申请 */
        pswBuf = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBinLen * sizeof(VOS_INT32));
        pswTmp = (VOS_INT32 *)UCOM_MemAlloc((VOS_UINT16)shwBinLen * sizeof(VOS_INT32));


        /* 平滑功率谱 */
        for (shwIdxI = 0; shwIdxI < shwBinLen; shwIdxI++)
        {
            pswBuf[shwIdxI]        = CODEC_OpL_mpy_32_16(pswSdd[shwIdxI],        MED_AEC_HF_NLP_CN_ALPHA);
            pswTmp[shwIdxI]        = CODEC_OpL_mpy_32_16(pswNearPsdBin[shwIdxI], MED_AEC_HF_NLP_CN_ONE_SUB_ALPHA);
            pswNearPsdBin[shwIdxI] = CODEC_OpL_add(pswBuf[shwIdxI], pswTmp[shwIdxI]);
        }

        /* 比较功率谱，取较小值 */
        for (shwIdxI = 0; shwIdxI < shwBinLen; shwIdxI++)
        {
            if (pswNearPsdBin[shwIdxI] < pswPsdBin[shwIdxI])
            {
                pswBuf[shwIdxI] = pswNearPsdBin[shwIdxI];
            }
            else
            {
                pswBuf[shwIdxI] = pswPsdBin[shwIdxI];
            }
        }

        /* 修正功率谱 */
        pstCn->swCancelCount++;
        if (pstCn->swCancelCount > MED_AEC_HF_CANCEL_COUNT_THD)
        {
            for (shwIdxI = 0; shwIdxI < shwBinLen; shwIdxI++)
            {
                pswTmp[shwIdxI]    = CODEC_OpL_sub      (pswPsdBin[shwIdxI], pswBuf[shwIdxI]);
                pswTmp[shwIdxI]    = CODEC_OpL_mpy_32_16(pswTmp[shwIdxI],    MED_AEC_HF_NLP_CN_STEP);
                pswBuf[shwIdxI]    = CODEC_OpL_add      (pswBuf[shwIdxI],    pswTmp[shwIdxI]);
                pswTmp[shwIdxI]    = CODEC_OpL_mpy_32_16(pswBuf[shwIdxI],    MED_AEC_HF_NLP_CN_RAMP);
                pswPsdBin[shwIdxI] = CODEC_OpL_add      (pswBuf[shwIdxI],    pswTmp[shwIdxI]);
            }
        }

        /* 内存释放 */
        UCOM_MemFree(pswBuf);
        UCOM_MemFree(pswTmp);
    }
}

/*****************************************************************************
 函 数 名  : MED_AEC_HF_STSuppress
 功能描述  : 判断单讲再进一步抑制
 输入参数  : pstSTSuppress                      - 单讲结构体指针
             shwFftLen                          - FFT长度
             pshwRefrFrm                        - 频域参考信号指针
             pshwResdFreq                       - NLP处理后的频域残差信号指针
 输出参数  : pshwResdFreq                       - 频域残差信号指针
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年02月13日
    作    者   : g184435
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID MED_AEC_HF_STSuppress(
                MED_AEC_HF_NLP_STSUPPRE_STRU  *pstSTSuppress,
                VOS_INT16                      shwFftLen,
                VOS_INT16                     *pshwRefrFrm,
                VOS_INT16                     *pshwResdFreq)
{
    VOS_INT16                          shwIdx;
    VOS_INT16                          shwBandLo;
    VOS_INT16                          shwBandHi;
    VOS_INT32                          swTemp;
    VOS_INT32                          swSumRefrLast;

    swSumRefrLast                   = pstSTSuppress->swSumRefr;
    pstSTSuppress->swSumRefr        = 0;
    for (shwIdx = (shwFftLen >> 1); shwIdx < shwFftLen; shwIdx++)
    {
        pstSTSuppress->swSumRefr    = CODEC_OpL_add(pstSTSuppress->swSumRefr,
                                                    (VOS_INT32)CODEC_OpAbs_s(*(pshwRefrFrm + shwIdx)));
    }

    swTemp                          = 0;
    shwBandLo                       = pstSTSuppress->shwResdPowBandLow;
    shwBandHi                       = pstSTSuppress->shwResdPowBandHi;
    for (shwIdx = shwBandLo; shwIdx < shwBandHi; shwIdx++)
    {
        swTemp = CODEC_OpL_add(swTemp, (VOS_INT32)CODEC_OpAbs_s(*(pshwResdFreq + shwIdx)));
    }
    pstSTSuppress->swSumResd  = CODEC_OpL_sub(pstSTSuppress->swSumResd,
                                              CODEC_OpL_mpy_32_16(CODEC_OpL_sub(pstSTSuppress->swSumResd, swTemp),
                                                                  pstSTSuppress->shwResdPowAlph));

    if (((pstSTSuppress->swSumRefr >= MED_AEC_HF_NLP_REFRPOW_THD)
        || (swSumRefrLast >= MED_AEC_HF_NLP_REFRPOW_THD))
        && (pstSTSuppress->swSumResd < pstSTSuppress->shwResdPowThd))
    {
        CODEC_OpVecShr(pshwResdFreq, shwFftLen, MED_PP_SHIFT_BY_5, pshwResdFreq);
    }

    /* 将频域-1的信号置零，避免有效的噪声不能消除，移位无法将-1移为0 */
    for (shwIdx = 0; shwIdx < shwFftLen; shwIdx++)
    {
        pshwResdFreq[shwIdx] = (pshwResdFreq[shwIdx] == -1) ? 0 : pshwResdFreq[shwIdx];
    }

}

#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif

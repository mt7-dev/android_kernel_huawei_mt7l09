/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  GPIO.h
*
*   作    者 :  liuwenhui
*
*   描    述 :  GPIO模块接口文件，从V7R1 BSP_GPIO.h移植
*
*   修改记录 :  2012年11月27日
*************************************************************************/
#ifndef _GPIO_BALONG_H_
#define _GPIO_BALONG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <drv_nv_def.h>
#include <hi_gpio.h>
#include <osl_types.h>
#include <bsp_memmap.h>

/* GPIO状态*/
#define GPIO_OK                     0
#define GPIO_ERROR                  -1

#ifndef HI_K3_GPIO

/* GPIO最大管脚个数*/
#define GPIO_MAX_PINS              (32)

#ifdef HI_GPIO4_REGBASE_ADDR
#define GPIO_MAX_BANK_NUM          (6)
#else
#define GPIO_MAX_BANK_NUM          (4)
#endif

#define GPIO_TOTAL_PINS_NUM        (GPIO_MAX_BANK_NUM * GPIO_MAX_PINS)

/* GPIO是否初始化标志*/
#define GPIO_DEF_NOT_RUNNING        0
#define GPIO_DEF_RUNNING            1

/*gpio number*/
#define GPIO_0_0	                  (0)
#define GPIO_0_1	                  (1)
#define GPIO_0_2	                  (2)
#define GPIO_0_3	                  (3)
#define GPIO_0_4	                  (4)
#define GPIO_0_5	                  (5)
#define GPIO_0_6	                  (6)
#define GPIO_0_7	                  (7)
#define GPIO_0_8	                  (8)
#define GPIO_0_9	                  (9)
#define GPIO_0_10	                  (10)
#define GPIO_0_11	                  (11)
#define GPIO_0_12					  (12)
#define GPIO_0_13	                  (13)
#define GPIO_0_14	                  (14)
#define GPIO_0_15	                  (15)
#define GPIO_0_16	                  (16)
#define GPIO_0_17	                  (17)
#define GPIO_0_18	                  (18)
#define GPIO_0_19	                  (19)
#define GPIO_0_20	                  (20)
#define GPIO_0_21	                  (21)
#define GPIO_0_22	                  (22)
#define GPIO_0_23	                  (23)
#define GPIO_0_24	                  (24)
#define GPIO_0_25	                  (25)
#define GPIO_0_26	                  (26)
#define GPIO_0_27	                  (27)
#define GPIO_0_28	                  (28)
#define GPIO_0_29					  (29)
#define GPIO_0_30	                  (30)
#define GPIO_0_31	                  (31)
  
#define GPIO_1_0	                  (32)
#define GPIO_1_1	                  (33)
#define GPIO_1_2	                  (34)
#define GPIO_1_3	                  (35)
#define GPIO_1_4	                  (36)
#define GPIO_1_5	                  (37)
#define GPIO_1_6	                  (38)
#define GPIO_1_7	                  (39)
#define GPIO_1_8	                  (40)
#define GPIO_1_9	                  (41)
#define GPIO_1_10	                  (42)
#define GPIO_1_11	                  (43)
#define GPIO_1_12					  (44)
#define GPIO_1_13	                  (45)
#define GPIO_1_14	                  (46)
#define GPIO_1_15	                  (47)
#define GPIO_1_16	                  (48)
#define GPIO_1_17	                  (49)
#define GPIO_1_18	                  (50)
#define GPIO_1_19	                  (51)
#define GPIO_1_20	                  (52)
#define GPIO_1_21	                  (53)
#define GPIO_1_22	                  (54)
#define GPIO_1_23	                  (55)
#define GPIO_1_24	                  (56)
#define GPIO_1_25	                  (57)
#define GPIO_1_26	                  (58)
#define GPIO_1_27	                  (59)
#define GPIO_1_28	                  (60)
#define GPIO_1_29					  (61)
#define GPIO_1_30	                  (62)
#define GPIO_1_31	                  (63)
  
#define GPIO_2_0	                  (64)
#define GPIO_2_1	                  (65)
#define GPIO_2_2	                  (66)
#define GPIO_2_3	                  (67)
#define GPIO_2_4	                  (68)
#define GPIO_2_5	                  (69)
#define GPIO_2_6	                  (70)
#define GPIO_2_7	                  (71)
#define GPIO_2_8	                  (72)
#define GPIO_2_9	                  (73)
#define GPIO_2_10	                  (74)
#define GPIO_2_11	                  (75)
#define GPIO_2_12					  (76)
#define GPIO_2_13	                  (77)
#define GPIO_2_14	                  (78)
#define GPIO_2_15	                  (79)
#define GPIO_2_16	                  (80)
#define GPIO_2_17	                  (81)
#define GPIO_2_18	                  (82)
#define GPIO_2_19	                  (83)
#define GPIO_2_20	                  (84)
#define GPIO_2_21	                  (85)
#define GPIO_2_22	                  (86)
#define GPIO_2_23	                  (87)
#define GPIO_2_24	                  (88)
#define GPIO_2_25	                  (89)
#define GPIO_2_26	                  (90)
#define GPIO_2_27	                  (91)
#define GPIO_2_28	                  (92)
#define GPIO_2_29					  (93)
#define GPIO_2_30	                  (94)
#define GPIO_2_31	                  (95) 

#define GPIO_3_0	                  (96)
#define GPIO_3_1	                  (97)
#define GPIO_3_2	                  (98)
#define GPIO_3_3	                  (99)
#define GPIO_3_4	                  (100)
#define GPIO_3_5	                  (101)
#define GPIO_3_6	                  (102)
#define GPIO_3_7	                  (103)
#define GPIO_3_8	                  (104)
#define GPIO_3_9	                  (105)
#define GPIO_3_10	                  (106)
#define GPIO_3_11	                  (107)
#define GPIO_3_12					  (108)
#define GPIO_3_13	                  (109)
#define GPIO_3_14	                  (110)
#define GPIO_3_15	                  (111)
#define GPIO_3_16	                  (112)
#define GPIO_3_17	                  (113)
#define GPIO_3_18	                  (114)
#define GPIO_3_19	                  (115)
#define GPIO_3_20	                  (116)
#define GPIO_3_21	                  (117)
#define GPIO_3_22	                  (118)
#define GPIO_3_23	                  (119)
#define GPIO_3_24	                  (120)
#define GPIO_3_25	                  (121)
#define GPIO_3_26	                  (122)
#define GPIO_3_27	                  (123)
#define GPIO_3_28	                  (124)
#define GPIO_3_29					  (125)
#define GPIO_3_30	                  (126)
#define GPIO_3_31	                  (127)

#define GPIO_4_0	                  (128)
#define GPIO_4_1	                  (129)
#define GPIO_4_2	                  (130)
#define GPIO_4_3	                  (131)
#define GPIO_4_4	                  (132)
#define GPIO_4_5	                  (133)
#define GPIO_4_6	                  (134)
#define GPIO_4_7	                  (135)
#define GPIO_4_8	                  (136)
#define GPIO_4_9	                  (137)
#define GPIO_4_10	                  (138)
#define GPIO_4_11	                  (139)
#define GPIO_4_12					  (140)
#define GPIO_4_13	                  (141)
#define GPIO_4_14	                  (142)
#define GPIO_4_15	                  (143)
#define GPIO_4_16	                  (144)
#define GPIO_4_17	                  (145)
#define GPIO_4_18	                  (146)
#define GPIO_4_19	                  (147)
#define GPIO_4_20	                  (148)
#define GPIO_4_21	                  (159)
#define GPIO_4_22	                  (150)
#define GPIO_4_23	                  (151)
#define GPIO_4_24	                  (152)
#define GPIO_4_25	                  (153)
#define GPIO_4_26	                  (154)
#define GPIO_4_27	                  (155)
#define GPIO_4_28	                  (156)
#define GPIO_4_29					  (157)
#define GPIO_4_30	                  (158)
#define GPIO_4_31	                  (159)

#define GPIO_5_0	                  (160)
#define GPIO_5_1	                  (161)
#define GPIO_5_2	                  (162)
#define GPIO_5_3	                  (163)
#define GPIO_5_4	                  (164)
#define GPIO_5_5	                  (165)
#define GPIO_5_6	                  (166)
#define GPIO_5_7	                  (167)
#define GPIO_5_8	                  (168)
#define GPIO_5_9	                  (169)
#define GPIO_5_10	                  (170)
#define GPIO_5_11	                  (171)
#define GPIO_5_12					  (172)
#define GPIO_5_13	                  (173)
#define GPIO_5_14	                  (174)
#define GPIO_5_15	                  (175)
#define GPIO_5_16	                  (176)
#define GPIO_5_17	                  (177)
#define GPIO_5_18	                  (178)
#define GPIO_5_19	                  (179)
#define GPIO_5_20	                  (180)
#define GPIO_5_21	                  (181)
#define GPIO_5_22	                  (182)
#define GPIO_5_23	                  (183)
#define GPIO_5_24	                  (184)
#define GPIO_5_25	                  (185)
#define GPIO_5_26	                  (186)
#define GPIO_5_27	                  (187)
#define GPIO_5_28	                  (188)
#define GPIO_5_29					  (189)
#define GPIO_5_30	                  (190)
#define GPIO_5_31	                  (191)

#else

#define GPIO_0_0	0
#define GPIO_0_1	1
#define GPIO_0_2	2
#define GPIO_0_3	3
#define GPIO_0_4	4
#define GPIO_0_5	5
#define GPIO_0_6	6
#define GPIO_0_7	7

#define GPIO_1_0	8
#define GPIO_1_1	9
#define GPIO_1_2	10
#define GPIO_1_3	11
#define GPIO_1_4	12
#define GPIO_1_5	13
#define GPIO_1_6	14
#define GPIO_1_7	15

#define GPIO_2_0	16
#define GPIO_2_1	17
#define GPIO_2_2	18
#define GPIO_2_3	19
#define GPIO_2_4	20
#define GPIO_2_5	21
#define GPIO_2_6	22
#define GPIO_2_7	23

#define GPIO_3_0	24
#define GPIO_3_1	25
#define GPIO_3_2	26
#define GPIO_3_3	27
#define GPIO_3_4	28
#define GPIO_3_5	29
#define GPIO_3_6	30
#define GPIO_3_7	31

#define GPIO_4_0	32
#define GPIO_4_1	33
#define GPIO_4_2	34
#define GPIO_4_3	35
#define GPIO_4_4	36
#define GPIO_4_5	37
#define GPIO_4_6	38
#define GPIO_4_7	39

#define GPIO_5_0	40
#define GPIO_5_1	41
#define GPIO_5_2	42
#define GPIO_5_3	43
#define GPIO_5_4	44
#define GPIO_5_5	45
#define GPIO_5_6	46
#define GPIO_5_7	47

#define GPIO_6_0	48
#define GPIO_6_1	49
#define GPIO_6_2	50
#define GPIO_6_3	51
#define GPIO_6_4	52
#define GPIO_6_5	53
#define GPIO_6_6	54
#define GPIO_6_7	55

#define GPIO_7_0	56
#define GPIO_7_1	57
#define GPIO_7_2	58
#define GPIO_7_3	59
#define GPIO_7_4	60
#define GPIO_7_5	61
#define GPIO_7_6	62
#define GPIO_7_7	63

#define GPIO_8_0	64
#define GPIO_8_1	65
#define GPIO_8_2	66
#define GPIO_8_3	67
#define GPIO_8_4	68
#define GPIO_8_5	69
#define GPIO_8_6	70
#define GPIO_8_7	71

#define GPIO_9_0	72
#define GPIO_9_1	73
#define GPIO_9_2	74
#define GPIO_9_3	75
#define GPIO_9_4	76
#define GPIO_9_5	77
#define GPIO_9_6	78
#define GPIO_9_7	79

#define GPIO_10_0	80
#define GPIO_10_1	81
#define GPIO_10_2	82
#define GPIO_10_3	83
#define GPIO_10_4	84
#define GPIO_10_5	85
#define GPIO_10_6	86
#define GPIO_10_7	87

#define GPIO_11_0	88
#define GPIO_11_1	89
#define GPIO_11_2	90
#define GPIO_11_3	91
#define GPIO_11_4	92
#define GPIO_11_5	93
#define GPIO_11_6	94
#define GPIO_11_7	95

#define GPIO_12_0	96
#define GPIO_12_1	97
#define GPIO_12_2	98
#define GPIO_12_3	99
#define GPIO_12_4	100
#define GPIO_12_5	101
#define GPIO_12_6	102
#define GPIO_12_7	103

#define GPIO_13_0	104
#define GPIO_13_1	105
#define GPIO_13_2	106
#define GPIO_13_3	107
#define GPIO_13_4	108
#define GPIO_13_5	109
#define GPIO_13_6	110
#define GPIO_13_7	111

#define GPIO_14_0	112
#define GPIO_14_1	113
#define GPIO_14_2	114
#define GPIO_14_3	115
#define GPIO_14_4	116
#define GPIO_14_5	117
#define GPIO_14_6	118
#define GPIO_14_7	119

#define GPIO_15_0	120
#define GPIO_15_1	121
#define GPIO_15_2	122
#define GPIO_15_3	123
#define GPIO_15_4	124
#define GPIO_15_5	125
#define GPIO_15_6	126
#define GPIO_15_7	127

#define GPIO_16_0	128
#define GPIO_16_1	129
#define GPIO_16_2	130
#define GPIO_16_3	131
#define GPIO_16_4	132
#define GPIO_16_5	133
#define GPIO_16_6	134
#define GPIO_16_7	135

#define GPIO_17_0	136
#define GPIO_17_1	137
#define GPIO_17_2	138
#define GPIO_17_3	139
#define GPIO_17_4	140
#define GPIO_17_5	141
#define GPIO_17_6	142
#define GPIO_17_7	143

#define GPIO_18_0	144
#define GPIO_18_1	145
#define GPIO_18_2	146
#define GPIO_18_3	147
#define GPIO_18_4	148
#define GPIO_18_5	149
#define GPIO_18_6	150
#define GPIO_18_7	151

#define GPIO_19_0	152
#define GPIO_19_1	153
#define GPIO_19_2	154
#define GPIO_19_3	155
#define GPIO_19_4	156
#define GPIO_19_5	157
#define GPIO_19_6	158
#define GPIO_19_7	159

#define GPIO_20_0	160
#define GPIO_20_1	161
#define GPIO_20_2	162
#define GPIO_20_3	163
#define GPIO_20_4	164
#define GPIO_20_5	165
#define GPIO_20_6	166
#define GPIO_20_7	167

#define GPIO_21_0	168
#define GPIO_21_1	169
#define GPIO_21_2	170
#define GPIO_21_3	171
#define GPIO_21_4	172
#define GPIO_21_5	173
#define GPIO_21_6	174
#define GPIO_21_7	175

#define GPIO_22_0	176
#define GPIO_22_1	177
#define GPIO_22_2	178
#define GPIO_22_3	179
#define GPIO_22_4	180
#define GPIO_22_5	181
#define GPIO_22_6	182
#define GPIO_22_7	183

#define GPIO_23_0	184
#define GPIO_23_1	185
#define GPIO_23_2	186
#define GPIO_23_3	187
#define GPIO_23_4	188
#define GPIO_23_5	189
#define GPIO_23_6	190
#define GPIO_23_7	191

#define GPIO_24_0	192
#define GPIO_24_1	193
#define GPIO_24_2	194
#define GPIO_24_3	195
#define GPIO_24_4	196
#define GPIO_24_5	197
#define GPIO_24_6	198
#define GPIO_24_7	199

#define GPIO_25_0	200
#define GPIO_25_1	201
#define GPIO_25_2	202
#define GPIO_25_3	203
#define GPIO_25_4	204
#define GPIO_25_5	205
#define GPIO_25_6	206
#define GPIO_25_7	207

#define GPIO_26_0	208
#define GPIO_26_1	209
#define GPIO_26_2	210
#define GPIO_26_3	211
#define GPIO_26_4	212
#define GPIO_26_5	213

/*define GPIO 0 ~ GPIO 7*/
#define   GPIO_000     GPIO_0_0
#define   GPIO_001     GPIO_0_1
#define   GPIO_002     GPIO_0_2
#define   GPIO_003     GPIO_0_3
#define   GPIO_004     GPIO_0_4
#define   GPIO_005     GPIO_0_5
#define   GPIO_006     GPIO_0_6
#define   GPIO_007     GPIO_0_7

/*define GPIO 8 ~ GPIO 15*/
#define   GPIO_008     GPIO_1_0
#define   GPIO_009     GPIO_1_1
#define   GPIO_010     GPIO_1_2
#define   GPIO_011     GPIO_1_3
#define   GPIO_012     GPIO_1_4
#define   GPIO_013     GPIO_1_5
#define   GPIO_014     GPIO_1_6
#define   GPIO_015     GPIO_1_7

/*define GPIO 16 ~ GPIO 23*/
#define   GPIO_016     GPIO_2_0
#define   GPIO_017     GPIO_2_1
#define   GPIO_018     GPIO_2_2
#define   GPIO_019     GPIO_2_3
#define   GPIO_020     GPIO_2_4
#define   GPIO_021     GPIO_2_5
#define   GPIO_022     GPIO_2_6
#define   GPIO_023     GPIO_2_7

/*define GPIO 24 ~ GPIO 31*/
#define   GPIO_024     GPIO_3_0
#define   GPIO_025     GPIO_3_1
#define   GPIO_026     GPIO_3_2
#define   GPIO_027     GPIO_3_3
#define   GPIO_028     GPIO_3_4
#define   GPIO_029     GPIO_3_5
#define   GPIO_030     GPIO_3_6
#define   GPIO_031     GPIO_3_7

/*define GPIO 32 ~ GPIO 39*/
#define   GPIO_032     GPIO_4_0
#define   GPIO_033     GPIO_4_1
#define   GPIO_034     GPIO_4_2
#define   GPIO_035     GPIO_4_3
#define   GPIO_036     GPIO_4_4
#define   GPIO_037     GPIO_4_5
#define   GPIO_038     GPIO_4_6
#define   GPIO_039     GPIO_4_7

/*define GPIO 40 ~ GPIO 47*/
#define   GPIO_040     GPIO_5_0
#define   GPIO_041     GPIO_5_1
#define   GPIO_042     GPIO_5_2
#define   GPIO_043     GPIO_5_3
#define   GPIO_044     GPIO_5_4
#define   GPIO_045     GPIO_5_5
#define   GPIO_046     GPIO_5_6
#define   GPIO_047     GPIO_5_7

/*define GPIO 48 ~ GPIO 55*/
#define   GPIO_048     GPIO_6_0
#define   GPIO_049     GPIO_6_1
#define   GPIO_050     GPIO_6_2
#define   GPIO_051     GPIO_6_3
#define   GPIO_052     GPIO_6_4
#define   GPIO_053     GPIO_6_5
#define   GPIO_054     GPIO_6_6
#define   GPIO_055     GPIO_6_7

/*define GPIO 56 ~ GPIO 63*/
#define   GPIO_056     GPIO_7_0
#define   GPIO_057     GPIO_7_1
#define   GPIO_058     GPIO_7_2
#define   GPIO_059     GPIO_7_3
#define   GPIO_060     GPIO_7_4
#define   GPIO_061     GPIO_7_5
#define   GPIO_062     GPIO_7_6
#define   GPIO_063     GPIO_7_7

/*define GPIO 64 ~ GPIO 71*/
#define   GPIO_064     GPIO_8_0
#define   GPIO_065     GPIO_8_1
#define   GPIO_066     GPIO_8_2
#define   GPIO_067     GPIO_8_3
#define   GPIO_068     GPIO_8_4
#define   GPIO_069     GPIO_8_5
#define   GPIO_070     GPIO_8_6
#define   GPIO_071     GPIO_8_7

/*define GPIO 72 ~ GPIO 79*/
#define   GPIO_072     GPIO_9_0
#define   GPIO_073     GPIO_9_1
#define   GPIO_074     GPIO_9_2
#define   GPIO_075     GPIO_9_3
#define   GPIO_076     GPIO_9_4
#define   GPIO_077     GPIO_9_5
#define   GPIO_078     GPIO_9_6
#define   GPIO_079     GPIO_9_7

/*define GPIO 80 ~ GPIO 87*/
#define   GPIO_080     GPIO_10_0
#define   GPIO_081     GPIO_10_1
#define   GPIO_082     GPIO_10_2
#define   GPIO_083     GPIO_10_3
#define   GPIO_084     GPIO_10_4
#define   GPIO_085     GPIO_10_5
#define   GPIO_086     GPIO_10_6
#define   GPIO_087     GPIO_10_7

/*define GPIO 88 ~ GPIO 95*/
#define   GPIO_088     GPIO_11_0
#define   GPIO_089     GPIO_11_1
#define   GPIO_090     GPIO_11_2
#define   GPIO_091     GPIO_11_3
#define   GPIO_092     GPIO_11_4
#define   GPIO_093     GPIO_11_5
#define   GPIO_094     GPIO_11_6
#define   GPIO_095     GPIO_11_7

/*define GPIO 96 ~ GPIO 103*/
#define   GPIO_096     GPIO_12_0
#define   GPIO_097     GPIO_12_1
#define   GPIO_098     GPIO_12_2
#define   GPIO_099     GPIO_12_3
#define   GPIO_100     GPIO_12_4
#define   GPIO_101     GPIO_12_5
#define   GPIO_102     GPIO_12_6
#define   GPIO_103     GPIO_12_7

/*define GPIO 104 ~ GPIO 111*/
#define   GPIO_104     GPIO_13_0
#define   GPIO_105     GPIO_13_1
#define   GPIO_106     GPIO_13_2
#define   GPIO_107     GPIO_13_3
#define   GPIO_108     GPIO_13_4
#define   GPIO_109     GPIO_13_5
#define   GPIO_110     GPIO_13_6
#define   GPIO_111     GPIO_13_7

/*define GPIO 112 ~ GPIO 119*/
#define   GPIO_112     GPIO_14_0
#define   GPIO_113     GPIO_14_1
#define   GPIO_114     GPIO_14_2
#define   GPIO_115     GPIO_14_3
#define   GPIO_116     GPIO_14_4
#define   GPIO_117     GPIO_14_5
#define   GPIO_118     GPIO_14_6
#define   GPIO_119     GPIO_14_7

/*define GPIO 120 ~ GPIO 127*/
#define   GPIO_120     GPIO_15_0
#define   GPIO_121     GPIO_15_1
#define   GPIO_122     GPIO_15_2
#define   GPIO_123     GPIO_15_3
#define   GPIO_124     GPIO_15_4
#define   GPIO_125     GPIO_15_5
#define   GPIO_126     GPIO_15_6
#define   GPIO_127     GPIO_15_7

/*define GPIO 128 ~ GPIO 135*/
#define   GPIO_128     GPIO_16_0
#define   GPIO_129     GPIO_16_1
#define   GPIO_130     GPIO_16_2
#define   GPIO_131     GPIO_16_3
#define   GPIO_132     GPIO_16_4
#define   GPIO_133     GPIO_16_5
#define   GPIO_134     GPIO_16_6
#define   GPIO_135     GPIO_16_7

/*define GPIO 136 ~ GPIO 143*/
#define   GPIO_136     GPIO_17_0
#define   GPIO_137     GPIO_17_1
#define   GPIO_138     GPIO_17_2
#define   GPIO_139     GPIO_17_3
#define   GPIO_140     GPIO_17_4
#define   GPIO_141     GPIO_17_5
#define   GPIO_142     GPIO_17_6
#define   GPIO_143     GPIO_17_7

/*define GPIO 144 ~ GPIO 151*/
#define   GPIO_144     GPIO_18_0
#define   GPIO_145     GPIO_18_1
#define   GPIO_146     GPIO_18_2
#define   GPIO_147     GPIO_18_3
#define   GPIO_148     GPIO_18_4
#define   GPIO_149     GPIO_18_5
#define   GPIO_150     GPIO_18_6
#define   GPIO_151     GPIO_18_7

/*define GPIO 152 ~ GPIO 159*/
#define   GPIO_152     GPIO_19_0
#define   GPIO_153     GPIO_19_1
#define   GPIO_154     GPIO_19_2
#define   GPIO_155     GPIO_19_3
#define   GPIO_156     GPIO_19_4
#define   GPIO_157     GPIO_19_5
#define   GPIO_158     GPIO_19_6
#define   GPIO_159     GPIO_19_7

/*define GPIO 160 ~ GPIO 167*/
#define   GPIO_160     GPIO_20_0
#define   GPIO_161     GPIO_20_1
#define   GPIO_162     GPIO_20_2
#define   GPIO_163     GPIO_20_3
#define   GPIO_164     GPIO_20_4
#define   GPIO_165     GPIO_20_5
#define   GPIO_166     GPIO_20_6
#define   GPIO_167     GPIO_20_7

/*define GPIO 168 ~ GPIO 175*/
#define   GPIO_168     GPIO_21_0
#define   GPIO_169     GPIO_21_1
#define   GPIO_170     GPIO_21_2
#define   GPIO_171     GPIO_21_3
#define   GPIO_172     GPIO_21_4
#define   GPIO_173     GPIO_21_5
#define   GPIO_174     GPIO_21_6
#define   GPIO_175     GPIO_21_7

/*define GPIO 176 ~ GPIO 183*/
#define   GPIO_176     GPIO_22_0
#define   GPIO_177     GPIO_22_1
#define   GPIO_178     GPIO_22_2
#define   GPIO_179     GPIO_22_3
#define   GPIO_180     GPIO_22_4
#define   GPIO_181     GPIO_22_5
#define   GPIO_182     GPIO_22_6
#define   GPIO_183     GPIO_22_7

/*define GPIO 184 ~ GPIO 191*/
#define   GPIO_184     GPIO_23_0
#define   GPIO_185     GPIO_23_1
#define   GPIO_186     GPIO_23_2
#define   GPIO_187     GPIO_23_3
#define   GPIO_188     GPIO_23_4
#define   GPIO_189     GPIO_23_5
#define   GPIO_190     GPIO_23_6
#define   GPIO_191     GPIO_23_7

/*define GPIO 192 ~ GPIO 199*/
#define   GPIO_192     GPIO_24_0
#define   GPIO_193     GPIO_24_1
#define   GPIO_194     GPIO_24_2
#define   GPIO_195     GPIO_24_3
#define   GPIO_196     GPIO_24_4
#define   GPIO_197     GPIO_24_5
#define   GPIO_198     GPIO_24_6
#define   GPIO_199     GPIO_24_7

/*define GPIO 200 ~ GPIO 207*/
#define   GPIO_200     GPIO_25_0
#define   GPIO_201     GPIO_25_1
#define   GPIO_202     GPIO_25_2
#define   GPIO_203     GPIO_25_3
#define   GPIO_204     GPIO_25_4
#define   GPIO_205     GPIO_25_5
#define   GPIO_206     GPIO_25_6
#define   GPIO_207     GPIO_25_7

/*define GPIO 208 ~ GPIO 213*/
#define   GPIO_208     GPIO_26_0
#define   GPIO_209     GPIO_26_1
#define   GPIO_210     GPIO_26_2
#define   GPIO_211     GPIO_26_3
#define   GPIO_212     GPIO_26_4
#define   GPIO_213     GPIO_26_5

#define   iomg_015     0x03C
#define   iomg_016     0x040
#define   iomg_017     0x044
#define   iomg_018     0x048
#define   iomg_096     0x188
#define   iomg_097     0x18c
#define   iomg_098     0x190
#define   iomg_099     0x194
#define   iomg_100     0x198
#define   iomg_101     0x19c
#define   iomg_102     0x1a0
#define   iomg_103     0x1a4
#define   iomg_104     0x1a8
#define   iomg_105     0x1ac
#define   iomg_106     0x1b0
#define   iomg_107     0x1b4
#define   iomg_108     0x1b8
#define   iomg_109     0x1bc
#define   iomg_110     0x1c0
#define   iomg_111     0x1c4
#define   iomg_112     0x1c8
#define   iomg_113     0x1cc
#define   iomg_114     0x1d0
#define   iomg_115     0x1d4
#define   iomg_116     0x1d8
#define   iomg_117     0x1dc
#define   iomg_118     0x1e0
#define   iomg_119     0x1e4
#define   iomg_120     0x1e8
#define   iomg_121     0x1ec
#define   iomg_122     0x1f0

#define   iocg_015     0x83C
#define   iocg_016     0x840
#define   iocg_017     0x844
#define   iocg_018     0x848
#define   iocg_130     0xA10
#define   iocg_131     0xA14
#define   iocg_132     0xA18
#define   iocg_133     0xA1C
#define   iocg_134     0xA20
#define   iocg_135     0xA24
#define   iocg_136     0xA28
#define   iocg_137     0xA2C
#define   iocg_138     0xA30
#define   iocg_139     0xA34

#define   iocg_140     0xA38
#define   iocg_141     0xA3C
#define   iocg_142     0xA40
#define   iocg_143     0xA44
#define   iocg_144     0xA48
#define   iocg_145     0xA4C
#define   iocg_146     0xA50
#define   iocg_147     0xA54
#define   iocg_148     0xA58
#define   iocg_149     0xA5C
                     
#define   iocg_150     0xA60
#define   iocg_151     0xA64
#define   iocg_152     0xA68
#define   iocg_153     0xA6C
#define   iocg_154     0xA70
#define   iocg_155     0xA74
#define   iocg_156     0xA78


/*define GPIO maximum group*/
#define    GPIO_MAX_GROUP    26

/*define  maximum number in a group*/
#define    GPIO_MAX_NUMBER   8
#define    GPIO_GROUP_BIT    3
#define    GPIO_TOTAL	     213
#define    GPIODIR           0x400

#endif

typedef enum{
	MASTER_0 = 0,
	MASTER_1,
	NAGTIVE,
	GR_BUTT
}ANTN_SW_GROUP_E;
/*****************************************************************************
* 函 数 名  : bsp_gpio_init
*
* 功能描述  : GPIO初始化接口
*
* 输入参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
s32 bsp_gpio_init(void);

/*****************************************************************************
* 函 数 名  : bsp_gpio_direction_input
*
* 功能描述  : 设置GPIO引脚方向为输入
*
* 输入参数  : UINT32 gpio    GPIO引脚编号
*
* 返 回 值  : 无
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
void bsp_gpio_direction_input(u32 gpio);

/*****************************************************************************
* 函 数 名  : bsp_gpio_direction_output
*
* 功能描述  : 设置GPIO引脚的方向为输出，并设置该引脚默认电平值
*
* 输入参数  : UINT32 gpio       GPIO引脚编号
*             UINT32 value      待设置电平值,即0为低电平，1为高电平
*
* 返 回 值  : 无
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
void bsp_gpio_direction_output(u32 gpio, u32 value);

/*****************************************************************************
* 函 数 名  : bsp_gpio_direction_get
*
* 功能描述  : 查询GPIO 引脚的方向
*
* 输入参数  : UINT32 gpio        GPIO引脚编号
*
* 返 回 值  : 返回gpio引脚方向,即0为输入，1为输出
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
s32 bsp_gpio_direction_get(u32 gpio);

/*****************************************************************************
* 函 数 名  : bsp_gpio_set_value
*
* 功能描述  : 设置GPIO 引脚的电平值
*
* 输入参数  : u32 gpio        GPIO引脚编号
*			  u32 value      管脚高/低电平
*
* 返 回 值  : 无
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
void bsp_gpio_set_value(u32 gpio,u32 value);

/*****************************************************************************
* 函 数 名  : bsp_gpio_get_value
*
* 功能描述  : 查询GPIO引脚的电平值
*
* 输入参数  : UINT32 gpio        GPIO引脚编号
*
* 返 回 值  : 返回GPIO引脚的电平值
*
* 修改记录  : 2012年11月27日
*****************************************************************************/
s32 bsp_gpio_get_value(u32 gpio);

void gpio_power_set_high(GPIO_POWER_E mid);

void gpio_power_set_low(GPIO_POWER_E mid);

int gpio_power_get_status(GPIO_POWER_E mid);

int bsp_rf_switch_init(void);

int bsp_ldo_gpio_init(void);

int bsp_rse_gpio_set(unsigned int flag, unsigned int mask, unsigned int value);

int bsp_antn_sw_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define _GPIO_BALONG_H_ */

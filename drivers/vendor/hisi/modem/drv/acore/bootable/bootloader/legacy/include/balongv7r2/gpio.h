/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  GPIO.h
*
*   作    者 :  liuwenhui
*
*   描    述 :  GPIO模块接口文件
*
*   修改记录 :  2012年11月27日
*************************************************************************/
#ifndef _GPIO_H_
#define _GPIO_H_
#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */


/* GPIO标志*/
#define GPIO_OK                (0)
#define GPIO_ERROR             (-1)

/*gpio 编号*/
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
/*****************************************************************************
* 函 数 名  : gpio_direction_input
*
* 功能描述  : 设置GPIO引脚方向为输入
*
* 输入参数  : unsigned gpio    GPIO引脚编号
*
* 返 回 值  : int
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
int gpio_direction_input(unsigned gpio);

/*****************************************************************************
* 函 数 名  : gpio_direction_output
*
* 功能描述  : 设置GPIO引脚的方向为输出，并设置该引脚默认电平值
*
* 输入参数  : unsigned gpio       GPIO引脚编号
*             int      value      待设置电平值,即0为低电平，1为高电平
*
* 返 回 值  : void
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
int gpio_direction_output(unsigned gpio, int value);

/*****************************************************************************
* 函 数 名  : gpio_direction_get
*
* 功能描述  : 查询GPIO 引脚的方向
*
* 输入参数  : unsigned gpio        GPIO引脚编号
*
* 返 回 值  : 返回gpio引脚方向,即0为输入，1为输出
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
int gpio_direction_get(unsigned gpio);

/*****************************************************************************
* 函 数 名  : gpio_set_value
*
* 功能描述  : 设置GPIO 引脚的电平值
*
* 输入参数  : unsigned gpio        GPIO引脚编号
*             int      value       引脚电平
*
* 输出参数  : 无
*
* 返 回 值  : void
*
* 修改记录  : 2012年11月27日   
*****************************************************************************/
void gpio_set_value(unsigned gpio,int value);

/*****************************************************************************
* 函 数 名  : gpio_get_value
*
* 功能描述  : 查询GPIO引脚的电平值
*
* 输入参数  : unsigned gpio        GPIO引脚编号
*
* 返 回 值  : 返回GPIO引脚的电平值
*
* 修改记录  : 2012年11月27日
*****************************************************************************/
int gpio_get_value(unsigned gpio);
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End #define _GPIO_H_ */

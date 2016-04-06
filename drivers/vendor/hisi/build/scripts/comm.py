#!/usr/bin/python
#-*- coding: UTF-8 -*-
#**************************************************************
#
#          版权所有 (C), 2001-2011,华为技术有限公司
#
#**************************************************************
#文件名    ：comm.py
#版本号    ：初稿
#生成日期  ：2013年09月09日
#功能描述  ：公共接口
#使用方法  : 
#修改历史  ：
#1.日期    ：2013年09月09日
#  修改内容：创建文件

def write_string_to_file(filepath,filename,string):
	"""
	功能描述：生成文件，并写入指定字符串
	参数：filepath 文件路径
	      filename 文件名
		  string   字符串
	返回值：
	异常描述：
	修改记录：1.日期    : 2013年09月09日
				作者    : l00167020
				修改内容: 创建函数
	"""	
	filetowrite = filepath + "/" + filename
	f = file(filetowrite, "w")
	f.write(string)
	f.close()	

def GenerateBuildInfo(params_dict):
	"""
	功能描述：生成编译构建的相关信息文件，供冒烟平台使用
	参数：params_dict入参字典
	返回值：
	异常描述：
	修改记录：1.日期    : 2013年09月09日
				作者    : l00167020
				修改内容: 创建函数
	"""	
	buildinfo=""
		
	for key in params_dict.keys():
#		if key != 'buildgroup' and\
		if "relversion" != key and \
		"transfer" != key and \
		"addname" != key and \
		"" != params_dict[key]:
			buildinfo += key + ":" + params_dict[key] + " "

	return buildinfo
	
def GenerateCmdArgFromDict(params_dict):
	"""
	功能描述：将字典转换为命令行参数
	参数：params_dict入参字典
	返回值：
	异常描述：
	修改记录：1.日期    : 2013年09月09日
				作者    : l00167020
				修改内容: 创建函数
	"""	
	CmdArgString=""
		
	for key in params_dict.keys():
		if "" != params_dict[key]:
			CmdArgString += key + "=" + params_dict[key] + " "
			
	return CmdArgString

def GetTargetFromArg(arg):
	"""
	功能描述： 获取coverity,klocwork,fortify携带的编译目标
	参数：arg coverity,klocwork,fortify携带的编译目标
	返回值：
	异常描述：
	修改记录：1.日期    : 2013年10月31日
				作者    : l00167020
				修改内容: 创建函数
	"""	
	if not "true" == arg:
		return arg

	return ""
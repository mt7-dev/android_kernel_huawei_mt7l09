#!/usr/bin/python
#-*- coding: UTF-8 -*-
#**************************************************************
#
#          版权所有 (C), 2001-2012,华为技术有限公司
#
#**************************************************************
#文件名    transfer_sh.py
#版本号    ：初稿
#作者      ：l00167020
#生成日期  ：2013年08月05日
#功能描述  ：将编译结果压缩并传输到大网
#使用方法  : 
#输入参数  : 
#输出参数  ：
#返回值    ：
#修改历史  ：
#1.日期    ：2013年08月05日
#  作者    ：l00167020
#  修改内容：创建文件

import os
import sys
import platform
import string

def compress_and_transfer(top_dir, params_dict, compile_time, addname):
    # 获取操作系统
    os_type = platform.system()
    top_dir = os.path.normpath(top_dir)
    
    # process input parameters       
    product_name = params_dict['product']
    trans_target_path = os.path.join(top_dir, 'build', 'delivery', product_name)
    
    # create product directory in transer target directory
    if "Linux" == os_type:
        trans_target_name = product_name
    else:
        trans_target_name = "tool"  

    if "" != addname :
        trans_target_name = trans_target_name + '_' + addname
        
    trans_target_name += '.rar'    
    trans_target = os.path.join(trans_target_path, trans_target_name)

    # compress tool
    if "Linux" == os_type:
        compress_tool = 'zip' + ' -r1q '
    else:
        compress_tool = os.path.join(top_dir, 'build', 'tools', 'utility', 'tools', '7za.exe') + ' a -r -y -bd'

    # stuff to compress
    if params_dict.has_key('trans_targets') and params_dict["trans_targets"] != "":
        transtarget_list = params_dict["trans_targets"].split(",")                        
    elif params_dict.has_key('COVERITY'):
        transtarget_list = ['coverity/']
    elif params_dict.has_key('KLOCWORK'):
        transtarget_list = ['klocwork/'] 
    else:
        transtarget_list = ['lib/','log/','img/','image/','tool/']

    transtarget_list.append("buildinfo.txt")
    stuff_to_compress = ' '.join(transtarget_list)
    
    # delete it if trans target left last time
    if os.path.exists(trans_target):
        os.system('rm -f ' + trans_target)

    compress_cmd = ' '.join(['cd',trans_target_path, '&&', compress_tool, trans_target, stuff_to_compress])    
    errcode = os.system(compress_cmd)
    if errcode != 0:
        print "[ERROR]:compress_and_transfer :  compress product failed!"
        return errcode

    # transfer 
    if os.environ.has_key('TRANSFER_TOOL_PATH'):
        trans_tool_path = os.environ['TRANSFER_TOOL_PATH']
    elif "Linux" == os_type:
        trans_tool_path = "/opt/deployment/scripts"
    else:
        trans_tool_path = "c:/Development"    

    trans_tool_path = os.path.normpath(trans_tool_path)
    trans_tool = os.path.join(trans_tool_path,"hibuild_transfer.py")
    if not os.path.exists(trans_tool):
        print "[ERROR]:compress_and_transfer, transfer failed for transfer tool not existed!"
        return -1
    
    transfer_cmd = " ".join(["python", trans_tool, trans_target])
    
    for i in range(0,3):
        errcode = os.system(transfer_cmd)
        if errcode == 0:
            break

    # delete rar after transfer complete 
    os.system('rm -f '+ trans_target)
    
    if not errcode == 0:
        print "[ERROR]: compress_and_transfer, transfer failed!"        
    
    return errcode
    

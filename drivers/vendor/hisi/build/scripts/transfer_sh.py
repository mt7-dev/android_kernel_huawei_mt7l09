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

def compress_and_transfer(top_dir, branch_name, product_name, buildowner, compiletime,additional_pkg_name):
	# transfer target ,transer target path
	trans_target_name = branch_name+'_'+product_name+'_'+ buildowner+'_'+compiletime 
	trans_target_path = top_dir +'/build/delivery' + '/' + product_name

	# create product directory in transer target directory
	trans_product_name = product_name
	if additional_pkg_name != '':
		trans_product_name = product_name + '_' + additional_pkg_name
	
	# first compress
	compress_target_layer_1 = 'lib/' + ' ' + 'log/' + ' ' + 'img/' +' ' + 'tool/' + ' ' + 'image/'+ ' ' + 'buildinfo.txt' 		
	compress_product_name = trans_product_name + '.rar'
	if os.path.exists(trans_target_path + '/' + compress_product_name):
		os.system('rm -f' + ' ' + trans_target_path + '/'+ compress_product_name)

	cmd = 'cd ' + trans_target_path + '&& zip -r1q ' + trans_target_path + '/'  + compress_product_name + ' ' + compress_target_layer_1
	errcode = os.system(cmd)
	if errcode != 0:
		print "compress_and_transfer :  compress product failed!"
		return errcode
		
	# second compress
	compress_target_layer_2 = compress_product_name + ' ' + 'timestamp.log' + ' ' + 'buildowner.txt'	
	compress_target_name = trans_target_name + '.rar'	
	if os.path.exists(trans_target_path + '/' + compress_target_name):
		os.system('rm -f' + ' ' + trans_target_path + '/'+ compress_target_name)
	
	cmd = 'cd ' + trans_target_path + '&& zip -r1q ' + trans_target_path + '/' + compress_target_name + ' ' + compress_target_layer_2
	errcode = os.system(cmd)
	if errcode != 0:
		print "compress_and_transfer :  compress failed!"
		return errcode
		
	# transfer 
	trans_scripts_path = top_dir + '/build'
	errcode	= os.system('cd ' + trans_scripts_path+ ' && bash hibuild_upload.sh '+ trans_target_path + ' '+ compress_target_name)
	if errcode != 0:
		print "compress_and_transfer :  transfer failed!"
		return errcode
	
	# delete rar after transfer complete 
	os.system('rm -f' + ' ' + trans_target_path + '/' +compress_product_name)
	os.system('rm -f' + ' ' + trans_target_path + '/'+ compress_target_name)
	
	return 0	
	
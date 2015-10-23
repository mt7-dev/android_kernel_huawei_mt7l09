#!/usr/bin/env python
#coding=utf-8
import os
import sys
import os.path


def get_cur_dir():
	path = sys.path[0]
	if os.path.isdir(path):
		return path
	elif os.path.isfile(path):
		return os.path.dirname(path)

def rm_file_path(rm_target):
	if os.path.isdir(rm_target):
		if os.path.exists(rm_target):
			shutil.rmtree(rm_target)
	elif os.path.isfile(rm_target):
		os.remove(rm_target)


start_dir = get_cur_dir()

for dirpath, dirnames, filenames in os.walk(start_dir):
	for full_file_name in filenames:
		if os.path.splitext(full_file_name)[1] == '.o':
			file_name = os.path.join(dirpath, full_file_name)
			dump_file = file_name + '.txt'
#			print file_name
			rm_file_path(dump_file)
			os.system("/opt/4.5.1/bin/arm-none-linux-gnueabi-objdump -D "+file_name +" > " + dump_file)
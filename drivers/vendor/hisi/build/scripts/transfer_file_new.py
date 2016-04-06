#!/usr/bin/python

import os
import sys
import getpass
import time
import shutil
import datetime

def write_info_2file(w_file, info):
	local_file = open(w_file, 'a')
	local_file.write(info)
	local_file.close()

def usage():
	print '''Usage:
	python transfer_file_new.py product_name balong_topdir hibuild_transfer.py
	transfer_file_new.py hibuild_transfer.py must include path infomation.'''

def main():
	if len(sys.argv) < 4 :
		usage()
	else:
		pro_name = sys.argv[1]
		pro_balong_dir = sys.argv[2]
		trans_agent = sys.argv[3]
		trans_file_cmd = r'python ' + trans_agent + r' ' + pro_balong_dir + r'/build/delivery/' + pro_name + r'.tar.gz'

		build_info_file = pro_balong_dir + r'/build/delivery/' + pro_name + r'/buildinfo.txt'
		#add compile_end_time to buildinfo.txt - zhangqiushui
		end_time = datetime.datetime.now()
		compile_end_time = end_time.strftime('%Y%m%d_%H%M%S')
		fp = open(build_info_file, 'a')
		fp.write("compile_end_time:" + compile_end_time + '\n')
		fp.close()
		#end
		if not os.path.isfile(build_info_file):
			buildinfo = 'product:' + pro_name + '\n'
			buildinfo += 'buildowner:' + getpass.getuser() + '\n'
			buildinfo += 'timestamp:' + time.strftime("%H'%M'%S", time.localtime(time.time())) + '\n'
			write_info_2file(build_info_file, buildinfo)

		os.system(trans_file_cmd)
		dstdir = pro_balong_dir + "/build/delivery/" + pro_name + "/log"
		build_info_file_n = pro_balong_dir + "/build/delivery/buildinfo_" + pro_name + ".txt"
		os.rename(build_info_file, build_info_file_n)
		shutil.copy(build_info_file_n, dstdir)
		os.remove(build_info_file_n)

if __name__ == "__main__" :
	main()

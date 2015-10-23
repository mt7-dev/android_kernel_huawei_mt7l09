
import os
import sys
import shutil
import socket
import getpass
import time

def rm_file_path(rm_target):
	if os.path.isdir(rm_target):
		if os.path.exists(rm_target):
			os.system('attrib -r ' + rm_target + r'/*.* /s')
			shutil.rmtree(rm_target)
	elif os.path.isfile(rm_target):
		os.remove(rm_target)

def write_info_2file(w_file, info):
	local_file = open(w_file, 'w')
	local_file.write(info)
	local_file.close()

def usage():
	print '''Usage:
	python transfer_file.py trans_file_dir BuildAgentEx.exe product_version product_name
	transfer_file.py BuildAgentEx.exe must include path infomation.'''

def main():
	if len(sys.argv) < 4 :
		usage()
	else:
		pro_ver             = 'Balong ' + sys.argv[1]
		pro_name            = sys.argv[2]
		host_ip             = sys.argv[3]
		pro_balong_dir      = sys.argv[4]
		trans_agent         = sys.argv[5]
		trans_dir           = sys.argv[6]
		trans_match_file    = sys.argv[7]
		trans_package_name  = sys.argv[8]
		trans_file_cmd      = trans_agent + r' "/path ' + trans_dir + r'" "' + trans_match_file + r'" "' + trans_package_name + r'" '
		ver_file            = os.path.join(trans_dir, 'version.txt')
		upload_dir          = trans_dir + r'/UPLOAD'
		gerrit_info_file    =pro_balong_dir + r'/change_info.log'
		
		if os.environ.has_key('jenkins_transfer_flag') and os.getenv('jenkins_transfer_flag') == "true":
			#llt_end_time=time.strftime('%Y%m%d_%H%M%S',time.localtime(time.time()))
			build_info_file = trans_dir + r'/buildinfo.txt'
			str  = 'product:' + pro_name +'\n'
			str += 'target_product:\n'
			str += 'schedule_time:' + os.getenv('schedule_time') +'\n'
			str += 'llt_end_time:' + os.getenv('llt_end_time') +'\n'
			str += 'buildowner:' + getpass.getuser() +'\n'
			str += 'tag:' + os.getenv('gerrit_tag_env') + '\n'
			str += 'owners:' + os.getenv('AUTHOR') +'\n'
			str += 'changeid:' + os.getenv('gerrit_change_id_env') +'\n'
			str += 'version:' + pro_ver + '\n'
			str += 'branch:' + os.getenv('gerrit_branch_env') +'\n'
			str += 'timestamp:' + os.getenv('timestamp') +'\n'
			str += 'compile_start_time:\n'
			str += 'compile_end_time:\n'
			
			write_info_2file(build_info_file, str)
		
		if len(sys.argv) == 10:
			release_path = sys.argv[9]
			trans_file_cmd = trans_file_cmd + r' "' + release_path + r'" '
			
		rm_file_path(ver_file)
		write_info_2file(ver_file, pro_ver)
		str  = 'owners         : ' + getpass.getuser() +'\n'
		str += 'timestamp      : ' + time.strftime('%Y-%m-%d-%H:%M:%S',time.localtime(time.time())) + '\n'
		str += 'tag            : \n'
		str += 'version        : BalongV7R2\n'
		str += 'product        : ' + pro_name +'\n'
		if os.path.exists(gerrit_info_file):
			gerrit_status = file(gerrit_info_file, 'r').readlines()
			for line in gerrit_status:
				str += line
		str += 'Host IP        : ' + host_ip +'\n'
		str += 'Project path   : ' + pro_balong_dir + '\n'
		str += 'Product Version:' + pro_ver + '\n'
		
		os.system(trans_file_cmd)

if __name__ == "__main__" :
	main()
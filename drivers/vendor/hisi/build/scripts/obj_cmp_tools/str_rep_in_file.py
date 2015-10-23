#!/usr/bin/env python
#coding=utf-8
import os
import os.path
import sys
import string
import stat
from shutil import copy


start_dir = os.getcwd()
print start_dir
vector = ['__FILE__','__DATE__','__TIME__','__FUNCTION__','__func__']
rep_vector = ['\"\"','\"\"','\"\"','\"\"','\"\"']
def_vector = ['#define ZEROLINEZERO      0 ','#define ZEROFILEZERO      "" ','#define ZERODATEZERO      "" ','#define ZEROTIMEZERO       "" ','#define ZEROFUNCTIONZERO       "" ','#define ZEROfuncZERO      "" ']
my_define_macro_tag='//___STR_REP_IN_FILE_PY_TAG\n'



def change_keywords():
	logfile = open("str_rep_change.log",'w')
	for dirpath, dirnames, filenames in os.walk(start_dir):
		for full_file_name in filenames:
			if os.path.splitext(full_file_name)[1] == '.h' or os.path.splitext(full_file_name)[1] == '.c':
				file_name = os.path.join(dirpath, full_file_name)
				
				input_file = open(file_name)
				src_content = input_file.read()
				input_file.close() 
				
				my_define_macro =""
				i=0
				for word in vector:
					if src_content.find(word) != -1 :
						my_define_macro+="#undef " + word + " //" + my_define_macro_tag  
						my_define_macro+="#define "+ word +" " + rep_vector[i] + " //" + my_define_macro_tag  
					i=i+1
				if my_define_macro != "" :
					os.chmod(file_name,stat.S_IWRITE)
					output_file = open(file_name, 'w')
					output_file.write(my_define_macro)
					output_file.write(src_content)
					output_file.close( )
					
					log_line = 'changed file :'+file_name + '. ' + str(my_define_macro.count('\n')) + '\n'
					print log_line
					logfile.write(log_line)
	logfile.close()
def restore_keywords():
	logfile = open("str_rep_restore.log",'w')
	for dirpath, dirnames, filenames in os.walk(start_dir):
		for full_file_name in filenames:
			if os.path.splitext(full_file_name)[1] == '.h' or os.path.splitext(full_file_name)[1] == '.c':
				file_name = os.path.join(dirpath, full_file_name)
				
				input_lines = open(file_name, 'r').readlines()
				if my_define_macro_tag in input_lines :
					os.chmod(file_name,stat.S_IWRITE)
					output_file = open(file_name, 'w')
					count=0
					for line in input_lines:
						if my_define_macro_tag in line:
							count=count+1
							continue
						output_file.write(line)
					output_file.close()
					
					log_line = 'changed file :'+file_name + '. ' + str(count) + '\n'
					print log_line
					logfile.write(log_line)
	logfile.close()
	
if __name__ == "__main__" :
	if len(sys.argv) <= 1:
		print "Usage: str_rep_in_file.py change|restore"
	elif sys.argv[1] == "change" :
		change_keywords()
	elif sys.argv[1] == "restore" :
		restore_keywords()
	else:
		print "Usage: str_rep_in_file.py change|restore"


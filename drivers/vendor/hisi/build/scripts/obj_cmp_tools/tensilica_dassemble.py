import os
import sys
import string

system_flag = "--xtensa-system=C:/usr/xtensa/XtDevTools/install/builds/RD-2011.2-win32/Balong_V7R1_BBE16_prod/config "
core_flag = "--xtensa-core=Balong_V7R1_BBE16_prod  "
params_flag = "--xtensa-params=  "
filenames = os.listdir(os.getcwd())
for file in filenames:
	if os.path.splitext(file)[1] == ".o" :
		print "objdumparm.exe -D "+file 
		os.system("C:/usr/xtensa/XtDevTools/install/tools/RD-2011.2-win32/XtensaTools/bin/xt-objdump.exe " + system_flag + core_flag + params_flag + " -D "+file +" > " +file + ".txt")
os.system("pause")

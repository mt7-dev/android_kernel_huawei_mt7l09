import os
import sys
import string

filenames = os.listdir(os.getcwd())
for file in filenames:
	if os.path.splitext(file)[1] == ".o" :
		print "objdumparm.exe -D "+file 
		os.system("C:/ZSP/ZView3.2.0/zspg2/bin/zdobjdump.exe -D "+file +" > " +file + ".txt")
os.system("pause")

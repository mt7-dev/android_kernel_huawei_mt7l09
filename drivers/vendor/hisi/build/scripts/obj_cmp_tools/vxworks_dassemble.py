import os
import sys
import string

filenames = os.listdir(os.getcwd())
for file in filenames:
	if os.path.splitext(file)[1] == ".o" or os.path.splitext(file)[1] == ".elf" :
		print "objdumparm.exe -D "+file 
		os.system("C:/WindRiver/gnu/4.1.2-vxworks-6.8/x86-win32/bin/objdumparm.exe -D "+file +" > " +file + ".txt")
os.system("pause")

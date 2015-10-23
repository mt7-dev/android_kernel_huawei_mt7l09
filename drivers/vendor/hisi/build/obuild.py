#!/usr/bin/python
import os
import string
import sys
import datetime
import time
import stat
import shutil

def read_argv_from_file(file_obuild_cmd):
	my_argv = []
	if os.path.exists(file_obuild_cmd):
		fil = open(file_obuild_cmd, 'r')
		for v in fil.readlines() :
			my_argv.append(string.rstrip(v,'\n'))
		fil.close()
	return my_argv

def save_argv_to_file(file_obuild_cmd, my_argv):
	fil = open(file_obuild_cmd, 'w')
	for v in my_argv:
		fil.write(v + '\n')
	fil.close()
				
def main():
	top_dir = string.replace(os.path.abspath('..'), '//', '/')
	file_obuild_cmd = 'obuild.cmd'
	start_time = datetime.datetime.now()
	
	# import lib
	sys.path.append(top_dir + "/build/scripts")
	import set_sys_env_var
	import make_product_config_file
	import transfer_sh

	# try get argv from obuild.cmd
	my_argv = sys.argv[1:]
	if len(my_argv) == 0 :
		print my_argv
		my_argv = read_argv_from_file(file_obuild_cmd)
		
	# scan parameters to dict
	params_dict = {}
	params_string = ""
	for p in my_argv:
		params_string += " " + p
		if string.find(p, '=') != -1 :
			subs = string.split(p,'=')
			params_dict[subs[0]]=subs[1]
		else:
			params_dict[p]=""
	
	if params_dict.has_key('ci'):
		if not params_dict.has_key('changeid'):
			print"You need to input change id if you want to trigger jenkins to build."
			print "Example:"
			ci_usage()
			sys.exit(1)
		change_id=params_dict['changeid']
		branch=""
		if params_dict.has_key('branch'):
			branch=params_dict['branch']
		ci_cmd="./obuild_ci_new.sh trigger " + change_id + " " + branch
		os.system(ci_cmd)
	else:
		# print compiletime
		compiletime = time.strftime('%Y%m%d_%H%M%S',time.localtime(time.time()))
		params_string += " compiletime=" +compiletime
		
		print ">> PARAMS:" + params_string
		
		# 'HUTAF_HLT_COV'
		if params_dict.has_key('HUTAF_HLT_COV'):
			if params_dict['HUTAF_HLT_COV']=='true':
				os.environ['HUTAF_HLT_COV'] = 'true'
				os.environ['HUTAF_HLT_WRAPPER'] = 'lltwrapper' 
				os.environ['HUTAF_HLT_LTCOV'] = 'y'

		# 'product' is required.
		if not params_dict.has_key('product') :
			print "\'product\' is required."
			usage()
			sys.exit(1)
		
		# config ccache
		cfg_cmd=top_dir + "/build/tools/ccache/ccache -M 30G"
		os.system(cfg_cmd)
		
		#Define whether is strip version	
		build_dir      = top_dir + r'/build'
		ps_dir         = top_dir + r'/modem/ps'
		ps_as_dir      = ps_dir + r'/as'
		lib_obj_dir    = top_dir + r'/modem/libs'
		product_lib_dir = lib_obj_dir + r'/' + params_dict['product']
		delivery_dir   = build_dir + r'/delivery'
		delivery_lib   = delivery_dir + r'/' + params_dict['product'] + r'/lib'
		if not params_dict.has_key('distclean') and not params_dict.has_key('clean'):
			if params_dict.has_key('publish'):
				if not os.path.exists(delivery_lib):
					cmd = 'mkdir  -p ' + delivery_lib
					os.system(cmd)
				if not os.path.exists(product_lib_dir):
					print "No such directory " + product_lib_dir
					sys.exit(1)
				cmd_copy_libs='cp  -rf '+ product_lib_dir +'/* ' + delivery_lib
				os.system(cmd_copy_libs)
				if os.path.exists(ps_dir + r'/comm/tl/lte/comm/HPA'):
					shutil.rmtree(ps_dir + r'/comm/tl/lte/comm/HPA')
					shutil.rmtree(ps_dir + r'/comm/tl/lte/comm/Stub')
					shutil.rmtree(ps_dir + r'/comm/tl/lte/comm/PSOM/Src')
		
		build_branch_name =''
		if params_dict.has_key('branch'):
			build_branch_name =params_dict['branch']
			
		additional_pkg_name =''
		if params_dict.has_key('addname'):
			additional_pkg_name =params_dict['addname']
			
		buildowner = 'jenkins_ci'
		if params_dict.has_key('buildowner'):
			buildowner =params_dict['buildowner']
		
		#generate buildowner.txt and timestamp.log
		delivery_path=top_dir+'/build/delivery/'+params_dict['product']
		if not os.path.exists(delivery_path):
			os.system('mkdir -p '+delivery_path)
			
		os.system('echo '+compiletime+'>'+delivery_path +'/timestamp.log')
		os.system('echo '+buildowner+'>'+delivery_path +'/buildowner.txt')
		
		# guess ROOT_WIND_PATH_W
		if not os.environ.has_key('ROOT_WIND_PATH_W') :
			if os.environ.has_key('WIND_HOME') :
				os.environ['ROOT_WIND_PATH_W'] = os.environ['WIND_HOME']
			elif os.path.exists('/opt/WindRiver6.8.3'):
				os.environ['ROOT_WIND_PATH_W'] = '/opt/WindRiver6.8.3'
			elif os.path.exists('/opt/windriver'):
				os.environ['ROOT_WIND_PATH_W'] = '/opt/windriver'
			elif os.path.exists('/opt/WindRiver'):
				os.environ['ROOT_WIND_PATH_W'] = '/opt/WindRiver'
			elif os.path.exists('C:/WindRiver'):
				os.environ['ROOT_WIND_PATH_W'] = 'C:/WindRiver'
			elif os.path.exists('D:/WindRiver'):
				os.environ['ROOT_WIND_PATH_W'] = 'D:/WindRiver'
			else:
				os.environ['ROOT_WIND_PATH_W'] = ''
				print 'Failed to guess WindRiver Home path. please set env ROOT_WIND_PATH_W or WIND_HOME'
				#usage()
				#sys.exit(-1)
				
		#xtensa license server
		if not os.environ.has_key('LM_LICENSE_FILE') :
			os.environ['LM_LICENSE_FILE'] = '27000@10.97.85.185:27000@10.72.133.121'	

		# setup environment vars: ROOT_BALONG_HOME, HOME and main.env,  ROOT_WIND_PATH_W
		os.environ['ROOT_BALONG_HOME'] = top_dir
		if os.environ['ROOT_WIND_PATH_W'] == 'C:/WindRiver' or os.environ['ROOT_WIND_PATH_W'] == 'D:/WindRiver':

			set_sys_env_var.config_system_env_var(top_dir + "/build/scripts/main_windows.env", r'')
		else:
			set_sys_env_var.config_system_env_var(top_dir + "/build/scripts/main.env", r'')
		print '>> The ROOT_BALONG_HOME=', os.environ['ROOT_BALONG_HOME'] 

		# generate product_config.h & product_config.mk
		product_name = params_dict['product']
		product_define_path = os.path.join(top_dir, "config/product/" + product_name +"/define/product_config.define")
		product_config_path = os.path.join(top_dir, "config/product/" + product_name +"/config/")
	#	if os.path.exists(product_config_path + "product_config.h") :
	#		os.chmod(product_config_path + "product_config.h", stat.S_IWRITE)
	#	if os.path.exists(product_config_path + "balong_product_config.mk") :
	#		os.chmod(product_config_path + "balong_product_config.mk", stat.S_IWRITE)
		make_product_config_file.gen_product_config_file(product_define_path, product_config_path + "product_config.h")
		make_product_config_file.gen_product_config_file(product_define_path, product_config_path + "balong_product_config.mk")

		# logfile
		logfile = top_dir + "/build/delivery/" + product_name + "/log/obuild.log"
		if params_dict.has_key('logfile')  :
			logfile = params_dict['logfile'] 
		if not os.path.exists(os.path.dirname(logfile)) :
			os.makedirs(os.path.dirname(logfile))
		print ">> Output log to " +logfile + " ..."

		# jobs
		if not params_dict.has_key('-j'):
			params_string += " -j 20 "
			
		cmd = "make " + params_string 
		
		# distcc
		if params_dict.has_key('distcc'):
			if params_dict['distcc']=='true':
				os.environ['distcc'] = 'true'
				os.environ['DISTCC'] = 'distcc' 


		# emake
		os.environ['EMAKE_ROOT'] = 'C:\Python27;' + top_dir + ';'+os.environ['ROOT_WIND_PATH_W'] 
		#os.environ['EMAKE_ROOT'] = os.environ['EMAKE_ROOT'] +  
		os.environ['EMAKE_ROOT'] = os.environ['EMAKE_ROOT'] + ';C:\ZSP'  
		os.environ['EMAKE_ROOT'] = os.environ['EMAKE_ROOT'] + ';C:\usr' 	
		os.environ['EMAKE_CM'] = '10.97.84.198' 
		os.environ['EMAKEFLAGS'] = '--emake-class=V7R1C10_BUGFIX_C'
		if params_dict.has_key('EA') and params_dict['EA']=='true' :
			cmd = "emake " + params_string 
			
		cmd = cmd + " 1> " + logfile + " 2>&1 "
		print ">> EXEC: " + cmd
		
		if params_dict.has_key('FORTIFY') and params_dict['FORTIFY']=='true' :
			os.environ['FORTIFY'] = 'true'
			os.environ['SOURCEANALYZER'] = top_dir + "/build/tools/fortify/bin/sourceanalyzer -b " + product_name
			fortify_path = top_dir + "/build/delivery/" + product_name + "/fortify"
			os.environ['FORTIFY_PATH'] = fortify_path
			cmd_create_project = top_dir + "/build/tools/fortify/bin/sourceanalyzer -b " + product_name + " -clean"
			os.system(cmd_create_project)
		
		if params_dict.has_key('TQE') and params_dict['TQE']=='true' :
			os.environ['COVERITY_UNSUPPORTED'] = '1'
			tqe_tmp_dir = top_dir + "/build/delivery/" + product_name + "/tqe_tmp"
			os.mkdir(tqe_tmp_dir)
			cmd = top_dir + "/build/tools/coverity/bin/cov-build --dir "  + tqe_tmp_dir + " --encoding UTF-8 " + cmd
		
		save_argv_to_file(file_obuild_cmd, my_argv)
		errcode =os.system(cmd)

		if params_dict.has_key('FORTIFY') and params_dict['FORTIFY']=='true' :
			cmd_generat_fpr = top_dir + "/build/tools/fortify/bin/sourceanalyzer -b " + product_name + " -scan -f " + product_name + ".fpr -64 -Xss100M -Xmx80000M"
			cmd_generat_report = top_dir + "/build/tools/fortify/bin/ReportGenerator -format xml -f " + product_name + ".xml -source " + product_name + ".fpr -template OWASP2010.xml -Xmx40000M"
			os.system(cmd_generat_fpr)
			os.system(cmd_generat_report)
			os.system('rm -r -f ' + top_dir + '/build/' + product_name + '.fpr')
			os.system('rm -r -f ' + fortify_path)
			os.mkdir(fortify_path)
			os.system('mv ' + top_dir + '/build/' + product_name + '.xml ' + fortify_path)
		
		if params_dict.has_key('TQE') and params_dict['TQE']=='true' :
			cmd_tqe_zip = top_dir + "/build/tools/coverity/bin/7za a -tzip coverity.zip " + tqe_tmp_dir + "/* -r"
			tqe_mid_dir = top_dir + "/build/delivery/" + product_name + "/tqe"
			os.system('rm -r -f ' + tqe_mid_dir)
			os.mkdir(tqe_mid_dir)
			os.system(cmd_tqe_zip)
			os.system('mv coverity.zip ' + tqe_mid_dir)
			os.system('rm -r -f ' + tqe_tmp_dir)
		
		# tranfer for shanghai
		if params_dict.has_key('hs_sh') and "true" == params_dict['hs_sh']:
			if not errcode == 0 :
				additional_pkg_name+="fail"
				
			transfer_request = "true"
			# if clean, no need to transfer build output
			if string.find(params_string, 'clean') != -1 :
				transfer_request = 'false'
					
			if params_dict.has_key('transfer'):
				transfer_request =params_dict['transfer']
				
			transfer_rlt = 0
			if "false" != transfer_request:
				transfer_rlt = transfer_sh.compress_and_transfer(top_dir,build_branch_name,product_name,buildowner,compiletime,additional_pkg_name)	
			if transfer_rlt:
				print "transfer failed!"
				errcode += transfer_rlt	
		
		if errcode == 0 :
			print ">> Everything is Done! Save command line to " + file_obuild_cmd
		else :
			print ">> Error: " + str(errcode)
			
		# summry
		end_time = datetime.datetime.now()
		duration_s = (end_time - start_time).seconds
		hour = duration_s /3600
		min = duration_s /60 - (duration_s /3600) * 60
		sec = duration_s - (duration_s /60) * 60
		print ">> Duration  : %d:%d:%d" %(hour, min, sec)
		try:
			logf = open(logfile, 'a')
			print >> logf, "summry"
			print >> logf, "---------------------------------------------"
			print >> logf, "start time:" + str(start_time)
			print >> logf, "end time  :" + str(end_time)
			print >> logf, "duration  : %d:%d:%d" %(hour, min, sec)
			print >> logf, "---------------------------------------------"
			logf.close()
		except:
			print sys.exc_info()[0]
			os.system('tskill be.exe')
			os.system('tskill ctoa.exe')
			os.system('tskill wgen.exe')
			
		#count error warning and info in log,added by WX161502 2013.06.17
		if params_dict.has_key('count'):
			if params_dict['count']=='true':
				countfile = top_dir + "/build/delivery/" + product_name + "/log/count.log"
				logf = open(logfile, 'r')
				res = logf.read()
				countf= open(countfile, 'w')
				print >> countf, "warning:" , res.count("warning:")
				print >> countf, "info:" , res.count("info:")
				print >> countf, "error:" , res.count("error:")
				print >> countf, "Error:" , res.count("Error ")
				logf.close()
				countf.close()
				
		if len(sys.argv) < 2 :
			os.system("pause")
		if errcode != 0 :
			sys.exit(-1)
		

def usage():
	print "\n\nUsage:\n"
	print "	obuild.py product=hi6930_p531_fpga "
	print "	obuild.py product=hi6930_p531_fpga balong"
	print "	obuild.py product=hi6930_p531_fpga pkg"
	print "	obuild.py product=hi6930_p531_fpga mcore mcore_libs verbose=true distcc=true -j 40 jobs=40"
	print "	obuild.py product=hi6930_p531_fpga drv lphy lmsp lps gudsp gumsp gups verbose=true  distcc=true -j 40 jobs=40"
	print "	obuild.py product=hi6930_p531_fpga acore acore-module=lps/lmsp/gups/gumsp/drv"
	print "	obuild.py product=hi6930_p531_fpga clean"
	print "\n"
	os.system("pause")
	
def ci_usage():
	print "pytohn obuild.py CHANGE_ID1,CHANGE_ID2,..."
	print "pytohn obuild.py CHANGE_ID BRANCH_NAME"

if __name__ == "__main__" :
	main()

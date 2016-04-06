#!/usr/bin/python
#-*- coding: UTF-8 -*-

import commands
import string 

def get_check_info(params_dict,top_dir):
	if (params_dict.has_key("CPPCHECK")):
		check_type = "cppcheck"
		get_cppcheck_version_cmd = top_dir + "/llt/ci/tools/cppcheck/cppcheck --version"
		check_tool_version = commands.getstatusoutput(get_cppcheck_version_cmd)[1]
	elif (params_dict.has_key("RISKYAPI")):
		check_type = "riskyapi"
		check_tool_version = "riskyApiTool430"
	elif (params_dict.has_key("PCLINT")):
		check_type = "pclint"
		get_pclint_version_cmd = "wine " + top_dir + "/llt/ci/tools/pc-lint/LINT-NT.EXE -v"
		check_tool_version = commands.getstatusoutput(get_pclint_version_cmd)[1].split("\n")[0].split(",")[0]
	elif (params_dict.has_key("warn_check")):
		check_type = "warncheck"
		check_tool_version = "None"
	elif (params_dict.has_key("COVERITY")):
		check_type = "coverity"
		get_coverity_version_cmd = top_dir + "/build/tools/coverity/bin/cov-build -ident"
		check_tool_version = commands.getstatusoutput(get_coverity_version_cmd)[1].split("\n")[0]
	elif (params_dict.has_key("FORTIFY")):
		check_type = "fortify"
		get_fortify_version_cmd = top_dir + "/build/tools/fortify/bin/sourceanalyzer -version"
		check_tool_version = commands.getstatusoutput(get_fortify_version_cmd)[1]
	else:
		check_type = None
		check_tool_version = None
	return (check_type, check_tool_version)

def get_vsc_info(params_dict, additional_pkg_name, top_dir):
	CODE_CHECK_REPORT_DEST = '/code_check_report'
	vsc_info = []
	(check_type, check_tool_version) = get_check_info(params_dict,top_dir)
	if (check_type != None and params_dict.has_key("version") and params_dict.has_key("branch") and params_dict.has_key("tag") and ("phisik3" == params_dict["buildowner"] or "jenkins_ci" == params_dict["buildowner"])):
		dest_path_list=[CODE_CHECK_REPORT_DEST]
		dest_path_list.append(params_dict["version"])
		dest_path_list.append(params_dict["branch"])
		dest_path_list.append(params_dict["tag"][-64:-37])
		dest_path_list.append(check_type)
		dest_path_list.append(params_dict["product"]+additional_pkg_name+".rar")
		output_file_path = "/".join(dest_path_list)
		hdfs_url = "url:" + output_file_path

		tool_version = "tool_version:" + check_tool_version
		tool_version = tool_version.replace(' ', '*')
		tool_version = string.strip(tool_version, '\n')
		tool_version = string.strip(tool_version, '\r')

		top_dir_split = top_dir.split("/")
		for dir_item in top_dir_split:
			if dir_item.find("VSC_") == 0:
				job = "job:" + dir_item
				job = job + " "

		if (not params_dict.has_key("COVERITY") and not params_dict.has_key("FORTIFY")):
			vsc_info.append(hdfs_url)
		vsc_info.append(tool_version)
		vsc_info.append(job)
		return " ".join(vsc_info)
	else:
		return None

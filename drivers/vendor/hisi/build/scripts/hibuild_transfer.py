#!/usr/bin/python
#-*- coding: UTF-8 -*-
import os
import time
from ftplib import FTP

def parse_buildinfo(buildinfo_file):
    buildinfo_dict = {}
    f = open(buildinfo_file, "r")
    buildinfos = f.readlines()
    f.close()
    lines_count = len(buildinfos)
    for i in range(lines_count):
        if buildinfos[i].strip() == "":
            continue
        key = buildinfos[i].split(':')[0].rstrip()
        value = buildinfos[i].split(':')[1].rstrip()
        buildinfo_dict[key] = value
    return buildinfo_dict

def write_str_2_file(file_name, content):
    try:
        parent_dir = os.path.dirname(file_name)
        if not os.path.exists(parent_dir):
            os.makedirs(parent_dir)

        file_handle = open(file_name, 'a')
        file_handle.write(content)
        file_handle.close()

        return True
    except:
        print "[ERROR]: hibuild_transfer.py, write_str_2_file, failed to write"

    return False

def ftp_transfer(src_file, dest_list, ftp_dict, filename=""):
    if filename == "":
        file_name = os.path.basename(src_file)
    else:
        file_name = filename
    try:
        ftp = FTP()
        ftp.connect(ftp_dict["ftp_srv_ip"], ftp_dict["ftp_port"])
        ftp.login(ftp_dict["ftp_user"], ftp_dict["ftp_passwd"])
        for each in dest_list:
            if not each in ftp.nlst():
                ftp.mkd(each)
            ftp.cwd(each)

        file = open(src_file, "rb")
        ftp.storbinary("STOR " + file_name, file)
        file.close()
        ftp.quit()
        return True
    except:
        print "[ERROR]: ftp_transfer, failed to tranfer by ftp !"
        print dest_list

    return False

def check_dict_key(dict, key):
    if not dict.has_key(key):
        print "[ERROR]:key %s not exist!" % key
        return False
    return True

def generate_versioninfo_for_smoke(buildinfo_dict, rarFile, path, content):
    if not buildinfo_dict.has_key("smoke"):
        return ""
    rarFileName = os.path.basename(rarFile)
    versionInfo_file_name = buildinfo_dict["timestamp"] + "@VersionInfo" + "_" + rarFileName[0:-7] + ".txt"
    verisonInfo_file = os.path.join(path, versionInfo_file_name)

    if write_str_2_file(verisonInfo_file, content):
        return verisonInfo_file
    return ""

def transfer_versioninfo_for_smoke(buildinfo_dict, versionInfo_file, ftp_dict):

    smoke_target_list = buildinfo_dict["smoke"].split(",")
    dst_path_list = ["ci_llt", ""]
    for each_target in smoke_target_list:
        dst_path_list[1] = each_target
        if buildinfo_dict.has_key("version") and buildinfo_dict["version"] == "BalongV8R1":
            dst_path_list[1] += "_v8r1"
        if not ftp_transfer(versionInfo_file, dst_path_list, ftp_dict):
            print "[ERROR]:hibuild_transfer.py, transfer_versioninfo_for_smoke, transfer version info for %s failed" % dst_path_list[1]

def main():
    if 1 > len(os.sys.argv):
        print "Need 1 parameters, actual:%d" % (len(os.sys.argv) - 1)
        exit(-1)

    src_file = os.sys.argv[1]
    src_path = os.path.dirname(src_file)
    src_file_name = os.path.basename(src_file)

    ftp_dict = {}
    ftp_dict["ftp_srv_ip"] = "10.106.40.214"
    ftp_dict["ftp_port"] = "21"
    ftp_dict["ftp_user"] = "l00227173"
    ftp_dict["ftp_passwd"] = "123456"

    trans_dst = "\\\\10.107.154.225\\compilepackage"

    # parse buildinfo.txt
    #buildinfo_file = os.path.join(src_path, "buildinfo.txt")
    buildinfo_file = src_path + r'/' + src_file_name[:-7] + r'/' + "buildinfo.txt"
    buildinfo_dict = {}
    if not os.path.exists(buildinfo_file):
        print "[ERROR]:hibuild_transfer.py,buildinfo.txt not exist in %s!" % src_path
        exit(-1)

    buildinfo_dict = parse_buildinfo(buildinfo_file)

    # check buildinfo
    if not check_dict_key(buildinfo_dict, "buildowner") \
       or not check_dict_key(buildinfo_dict, "product") \
       or not check_dict_key(buildinfo_dict, "timestamp"):
        exit(-1)

    if "pjenkinsbj" == buildinfo_dict["buildowner"]:
        if not check_dict_key(buildinfo_dict, "version") \
        or not check_dict_key(buildinfo_dict, "branch") \
        or not check_dict_key(buildinfo_dict, "changeid"):
            exit(-1)
    
    # if compile failing, append a suffix "_fail" to the package - zhangqiushui
    if buildinfo_dict.has_key("compile_result") and buildinfo_dict["compile_result"] == "fail":
        src_file_with_fail_mark = ""
        src_file_with_fail_mark = src_file.split(r'.tar.gz')[0] + r'_fail' + r'.tar.gz'
        os.rename(src_file, src_file_with_fail_mark)
        src_file = src_file_with_fail_mark
        src_file_name = os.path.basename(src_file)
    # end - zhangqiushui
    
    dest_path_list = []
    if "patch_maker" == buildinfo_dict["buildowner"]:
        dest_path_list.append("release_patch")
        dest_path_list.append(buildinfo_dict["patch_name"])
    elif "pjenkinsbj" == buildinfo_dict["buildowner"]:
        if not buildinfo_dict.has_key("COVERITY") and not buildinfo_dict.has_key("KLOCWORK"):
            dest_path_list.append("CI_Version")
        else:
            dest_path_list.append("TQE_Version")
        dest_path_list.append(buildinfo_dict["version"])
        dest_path_list.append(buildinfo_dict["branch"])
        #dest_path_list.append(buildinfo_dict["timestamp"])
        dest_path_list.append(buildinfo_dict["timestamp"] + "_" + buildinfo_dict["changeid"][:8])

    else:
        if not buildinfo_dict.has_key("COVERITY") and not buildinfo_dict.has_key("KLOCWORK"):
            dest_path_list.append("ReleaseFolder")
        else:
            dest_path_list.append("TQE_ReleaseFolder")
        date = time.strftime("%Y-%m-%d", time.localtime())
        dest_path_list.append(date)
        dest_path_list.append(buildinfo_dict["buildowner"])
        dest_path_list.append(buildinfo_dict["timestamp"])

    t_start_time = 'trans_start_time:' + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())) + '\n'
    # start tranfer
    if not ftp_transfer(src_file, dest_path_list, ftp_dict):
        exit(-1)

    # transfer buildinfo.txt for smoke
    buildinfo_for_smoke_name = ("buildinfo_" + src_file_name).replace(".tar.gz", ".txt")
    write_str_2_file(buildinfo_file, t_start_time)
    t_end_time = 'trans_end_time:' + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())) + '\n'
    write_str_2_file(buildinfo_file, t_end_time)
    g_end_time = 'gate_end_time:' + time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())) + '\n'
    write_str_2_file(buildinfo_file, g_end_time)
    #add result to buildinfo.txt according to compile result - zhangqiushui
    if buildinfo_dict.has_key("compile_result") and buildinfo_dict["compile_result"] == "fail":
        write_str_2_file(buildinfo_file, "result:fail\n")
    else:
        write_str_2_file(buildinfo_file, "result:success\n")
    #end

    ftp_transfer(buildinfo_file, dest_path_list, ftp_dict, buildinfo_for_smoke_name)

    dest_path_list.insert(0, trans_dst)
    output_path = "\\".join(dest_path_list)
    print "\n>> Transfer success, output path : %s\n" % output_path

    # transfer version_info.txt if specify pre smoke
    if buildinfo_dict.has_key("smoke") and "" != buildinfo_dict["smoke"]:
        versionInfo_file = generate_versioninfo_for_smoke(buildinfo_dict, src_file, src_path, output_path)
        transfer_versioninfo_for_smoke(buildinfo_dict, versionInfo_file, ftp_dict)
        os.system("rm -f " + versionInfo_file)

    exit(0)

if __name__ == "__main__" :
    main()

#set_sys_env_var.py  --- set system environment variable used for compilation through reading *.env file

import os
import string
import sys

# Description: 
def remove_invalid_ch(str):
    result =''
    str = string.strip(str)
    str = string.lstrip(str)
    str = string.rstrip(str)
    str = string.strip(str, ' ')
    str = string.strip(str, '\n')
    str = string.strip(str, '\r')
    result = str
    return result

# Description: split string with separater
# Input : 'A = B'(string);separater
# Output: C['A'', B'](array)
def split_str_with_separator(str, sep):
    str = remove_invalid_ch(str)
    fields =''
    
    if string.find(str, sep) > 0:
        fields = string.split(str, sep)
        fields[0] = remove_invalid_ch(fields[0])
        fields[1] = remove_invalid_ch(fields[1])
    return fields
    #print fields
    result = (fields[0], fields[1])
    #print result
    return result

# Description: determine weather string should be replaced. conditon 'ROOT_'
# Input : str_org, str_condtion
# Output: True/False
def is_string_found(str_org, str_condtion):
    if string.find(str_org, str_condtion) > -1:
        return True
    else:
        return False

# Description: 
# Input : str_org, str_condtion
# Output: True/False
def replace_string(str_org, str1, str2):
    result =''
    str_org = str_org.replace('$(', '')
    str_org = str_org.replace(')', '')
    str_org = str_org.replace(str1, str2)
    
    result = str_org
    return result

# Description: 
# Input : argv[1] = env_file; argv[2] = tool_dir
# Output: 
def config_system_env_var(env_file, tool_dir):
    arr_split = ('','')
    compilation_dir_head = '' 
    compilation_dir =''
    all_compilation_dir = ''
    env_path = ''
    
    tmp_file = file(env_file, 'r')
    while True:
        line = tmp_file.readline()
        if len(line) == 0: # Zero length indicates EOF
            break
        
        line = remove_invalid_ch(line)
        if line.split() and not line.startswith('#'):    #remove blank line
            arr_split = split_str_with_separator(line, '=')
            compilation_dir_head = remove_invalid_ch(arr_split[0])
            compilation_dir  = remove_invalid_ch(arr_split[1])
            
            #add equal sign's right string of line to os directory
            if is_string_found(compilation_dir, '$'):
                idx1 = string.find(compilation_dir, '(') + 1
                idx2 = string.find(compilation_dir, ')')
                root_path = compilation_dir[idx1:idx2]
                #print root_path
                path_val = os.getenv(root_path)
                compilation_dir = replace_string(compilation_dir, root_path, str(path_val))
                #if os.getenv(compilation_dir_head) == None :
                os.environ[compilation_dir_head] = compilation_dir
                    #print 'os.environ['+compilation_dir_head+'] = '+os.environ[compilation_dir_head]
            else :
                os.environ[compilation_dir_head] = compilation_dir
                #print 'os.environ['+compilation_dir_head+'] = '+os.environ[compilation_dir_head]
            
            #add string starts with 'PATH_' to Windows System Path
            if compilation_dir_head.startswith('PATH_'):
                all_compilation_dir = all_compilation_dir + compilation_dir +';'
            #print compilation_dir_head
    
    #curr_dir = os.getcwd()
    #tool_dir = curr_dir + tool_path
    #print tool_dir
    os.environ['PATH'] = ''
    os.environ['PATH'] = os.environ['PATH'] + all_compilation_dir + tool_dir + ';'
    
    if os.environ['ROOT_WIND_PATH_W'] != 'C:/WindRiver' and os.environ['ROOT_WIND_PATH_W'] != 'D:/WindRiver':

		os.environ['PATH'] = string.replace(os.environ['PATH'], ';', ':')
    
    tmp_file.close()
    
    #os.system('prjmake /?')

#config_system_env_var(sys.argv[1], sys.argv[2])
#print sys.argv[1]

#os.system('xcp.exe a.txt b.txt')
#os.system('dcc --help')
#os.system('xt-make -v')
#os.system('prjmake /?')
#os.system('xmake -v')


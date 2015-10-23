#!/bin/bash
set -ex
#set -n


##########################################
#variable state
##########################################
CMD=$1
PROJECT_TOPDIR=$2
PRODUCT_NAME=$3
BALONG_TOPDIR=${PROJECT_TOPDIR}/vendor/hisi
REPO_DIR=${PROJECT_TOPDIR}/.repo
LIBS_DIR=${PROJECT_TOPDIR}/libs
TIME_DIR=${gerrit_branch_env}/`date +%Y%m%d`
TAG_PATH=${TIME_DIR}/${gerrit_tag_env}
CONF_OBJ_LIST="ascomm_ccore.o gas_ccore.o gudsp_comm_RT.o gudsp_mcore.o gudsp_mcore_RT.o pscomm_ccore.o tds_dsp_ddr_data.bin ten_lte_dtcm.bin  ten_lte_itcm.bin ten_pub_dtcm.bin ten_pub_itcm.bin tlas_ccore.o ttf_ccore.o was_ccore.o"
CONF_IMAGE_LIST="lphy.bin"

#############################
# FUNCTION AREA
#############################

# FUNTION     : generate_tag()
# DESCRIPTION : generate tag
# PARAMETERS  : none
function generate_tag()
{
	MANIFEST_DIR=${PROJECT_TOPDIR}/.repo/manifests
	
	mkdir -p ${MANIFEST_DIR}/${TIME_DIR}
	cd ${MANIFEST_DIR}/${TIME_DIR}
	
	# variable timestamp from environment
	repo manifest -r -o tag_${gerrit_branch_env}_${timestamp}_${gerrit_change_id_env}.xml
	git add tag_${gerrit_branch_env}_${timestamp}_${gerrit_change_id_env}.xml
	git commit -m "tag_${gerrit_branch_env}_${timestamp}_${gerrit_change_id_env}.xml"
	git push origin default:${gerrit_branch_env}
}


# FUNTION     : download_tag_code()
# DESCRIPTION : download tag code
# PARAMETERS  : none
function download_tag_code()
{
	cd ${PROJECT_TOPDIR}
	date
	# download tag code
	rm -rf ${PROJECT_TOPDIR}/.repo/manifest*
	rm -rf ${LIBS_DIR}
	repo init -u ssh://${gerrit_server_env}/balong/manifest.git -b ${gerrit_branch_env} -m ${TAG_PATH} --no-repo-verify --repo-branch=stable
	repo sync -c
	repo forall -c "git checkout -f HEAD"
	repo forall -c "git clean -df"
	repo start jenkins_ci --all
	repo abandon jenkins_ci
	repo start jenkins_ci --all
	
	# clone libs
	git clone  ssh://${gerrit_server_env}/balong/libs.git -b ${gerrit_branch_env}
	date
}


# FUNTION     : upload_confidential_lib()
# DESCRIPTION : upload generated binary files after building.
# PARAMETERS  : none
function upload_confidential_lib()
{
	# copy binary files
	rm -rf ${LIBS_DIR}/*
	for product in ${PRODUCT_NAME}
	do
		mkdir -p ${LIBS_DIR}/${product}
		for file in ${CONF_OBJ_LIST}
		do
			delivery_product_path=${BALONG_TOPDIR}/build/delivery/${product}
			file_path=${delivery_product_path}/lib/${file}
			if [ -s ${file_path} ]
			then
				cp -rf ${file_path} ${LIBS_DIR}/${product}
			fi
		done
		
		for image in ${CONF_IMAGE_LIST}
		do
			delivery_product_path=${BALONG_TOPDIR}/build/delivery/${product}
			file_path=${delivery_product_path}/image/${image}
			if [ -s ${file_path} ]
			then
				cp -rf ${file_path} ${LIBS_DIR}/${product}
			fi
		done
	done
	date
	
	# upload files to libs
	cd ${LIBS_DIR}
	git add -f .
	git commit -m "upload libs"
	git push -f
	lib_commit_id=`git log |head -1|cut -f2 -d' '`
	date
	
	# replenish tag
	cd ${REPO_DIR}/manifests
	libs_info="\<project name=\"balong\/libs\" path=\"vendor\/hisi\/modem\/libs\" revision=\"${lib_commit_id}\" upstream=\"${gerrit_branch_env}\"\/\>"
	balong_build_line_num=`grep balong/build -n ${TIME_DIR}/${gerrit_tag_env} | awk -F : '{print $1}'`
	balong_build_line_num=`echo ${balong_build_line_num}`
	sed -i "${balong_build_line_num}a ${libs_info}" ${TIME_DIR}/${gerrit_tag_env}
	
	git add ${TIME_DIR}/${gerrit_tag_env}
	git commit -m "add libs to ${gerrit_tag_env}"
	git push -f origin default:${gerrit_branch_env}
	date
}


##########################################
# main control
##########################################
case $CMD in
	maketag) generate_tag;;
	init) download_tag_code;;
	build) clean_product; build_product;;
	uploadtag) upload_confidential_lib;;
	help) help;;
	*) echo "unknown command", $CMD;;
esac


#!/bin/bash
set -x
#set -n

##########################################
#variable state
##########################################
# The following variables come from env .
#
# gerrit_account_env
# gerrit_server_env
# gerrit_change_id_env
# gerrit_patchset_revision_env
# gerrit_branch_env
# gerrit_tag_env
# gerrit_apitoken_env(no used)
# jenkins_url_env(no used)
# param_extra_env
# WORKSPACE
# #########################################

CMD=$1
if [ ${CMD} = "trigger" ]
then
	CHANGE_ID=$2
	BRANCH=$3
elif [ ${CMD} = "review" ]
then
	ACTION=$2
	VALUE=$3
	MSGINFO="$4"
else
	PROJECT_TOPDIR=$2
	PRODUCT_NAME=$3
#	CHANGESET_FILE=$4
	BALONG_TOPDIR=${PROJECT_TOPDIR}/vendor/hisi
fi

###########################
# FUNCTION AREA
###### function lists #####

# FUNTION     : gerrit_cmd()
# DESCRIPTION : gerrit command
# PARAMETERS  :
# $1: command
# $2: options
# $3: commit id/change id
# $4: operator
# $5: value
function gerrit_cmd()
{
	cmdstr="$1"
	
	if [ -z "${gerrit_account_env}" ]
	then
		gerrit_account_env=${LOGNAME}
		gerrit_server_env=10.106.40.162
	fi
	# command format
	ssh -p 29418 ${gerrit_account_env}@${gerrit_server_env} gerrit ${cmdstr}
}


# FUNTION     : check_cid_is_empty()
# DESCRIPTION : Check if there is left change id which has not been disposed.
# PARAMETERS  : 
# $1: commit id
# $2: msg
function check_cid_is_empty()
{
	cid="$1"
	msg="$2"
	
	if [ -z "${cid}" ]
	then
		print_error_info "WARN" ${gerrit_change_id_env} "" "" "" "${msg}"
		if [ ${count_env} -lt ${cid_num_env} ]
		then
			echo "Dispose next change id."
			exit 0
		else
			exit 1
		fi
	fi
}

# FUNTION     : init_id()
# DESCRIPTION : init change id
# PARAMETERS  : none
revision_id=""
revision_prj=""
function init_id()
{
	if [ -n ${gerrit_change_id_env} ]
	then
		# obtain all commits
		branch_id=""
		all_revision_id=`gerrit_cmd "query --current-patch-set ${gerrit_change_id_env}" | grep "revision:" | awk '{print $2}'`
		
		# filter commit id
		for filter_id in ${all_revision_id}
		do
			branch=`gerrit_cmd "query --current-patch-set ${filter_id}" | grep "branch:" | awk '{print $2}'`
			branch=`echo ${branch}`
			
			if [ ${branch} = ${gerrit_branch_env} ]
			then
				branch_id="${branch_id} ${filter_id}"
				branch_id=`echo ${branch_id}`
			fi
		done
		
		check_cid_is_empty "${branch_id}" "This change id does not match branch ${gerrit_branch_env}, please check. From function init_id()."
		
		for filter_id in ${branch_id}
		do
			status_id=`gerrit_cmd "query --current-patch-set ${filter_id}" | grep "status:" | awk '{print $2}'`
			project=`gerrit_cmd "query --current-patch-set ${filter_id}" | grep "project:" | awk '{print $2}'`
			status_id=`echo ${status_id}`
			project=`echo ${project}`
			
			if [ ${status_id} != "MERGED" -a ${status_id} != "ABANDONED" ]
			then
				revision_id="${revision_id} ${filter_id}"
				revision_prj="${revision_prj} ${project}"
				revision_id=`echo ${revision_id}`
				revision_prj=`echo ${revision_prj}`
			fi
		done
		
		check_cid_is_empty "${revision_id}" "This change id has always been merge, please check. From function init_id()."
		
		revision_id=`echo ${revision_id}`
		revision_prj=`echo ${revision_prj}`
	else
		print_error_info "ERROR" "" "" "" "" "change id is empty, please check."
		exit 1
	fi
}


# FUNTION     : check_commit()
# DESCRIPTION : check commit
# PARAMETERS  : none
function check_commit()
{
#	revision_id=`gerrit_cmd query --current-patch-set ${gerrit_change_id_env} | grep "revision" | awk '{print $2}'`
	for id in ${revision_id}
	do
		project=`gerrit_cmd "query --current-patch-set ${id}" | grep "project:" | awk '{print $2}'`
		branch=`gerrit_cmd "query --current-patch-set ${id}" | grep "branch:" | awk '{print $2}'`
		review_type=`gerrit_cmd "query --current-patch-set ${id}" | grep "type:"| awk '{print $2}'`
		value_array=(`gerrit_cmd "query --current-patch-set ${id}" | grep "value:" | awk '{print $2}'` 0)
		review_type=`echo ${review_type}`
		num="0"
		r_flag="0"
		v_flag="0"
		
		# find key word "CRVW" and "VRIF"
		for tp in ${review_type}
		do
			tp=`echo ${tp}`
			if [ ${tp} = "CRVW" ]
			then
				if [ ${value_array[num]} -le 0 ]
				then
					print_error_info "ERROR" ${gerrit_change_id_env} ${id} ${project} ${branch} "Should review+1, current is ${value_array[num]}.This information from function check_commit()."
					exit 1
				fi
				# if the commit has been reviewed ,then var flag would add 1.
				let r_flag=${r_flag}+1
			elif [ ${tp} = "VRIF" ]
			then
				echo ""
				#if [ ${value_array[num]} -le 0 ]
				#then
				#	print_error_info "ERROR" ${gerrit_change_id_env} ${id} ${project} ${branch} "Should verify+1, current is ${value_array[num]}.This information from function check_commit()."
				#	exit 1
				#fi
				# if the commit has been reviewed ,then var flag would add 1.
				#let v_flag=${v_flag}+1
			fi
			let num=${num}+1
		done
		
		# if flag != 0,indicate the commit has been reviewed.
		if [ ${r_flag} -eq 0 ]
		then
			print_error_info "ERROR" ${gerrit_change_id_env} ${id} ${project} ${branch} "Not review+1,please check. From function check_commit()."
			exit 1
		fi
		
		#if [ ${v_flag} -eq 0 ]
		#then
		#	print_error_info "ERROR" ${gerrit_change_id_env} ${id} ${project} ${branch} "Not verify, the commits need verify+1 and code review+1,please check. This information from function check_commit()."
		#	exit 1
		#fi
	done
	
	print_error_info "SUCCESS" ${gerrit_change_id_env} ${revision_id} ${revision_prj} ${gerrit_branch_env} "All commit has been review+1 !"
}

# FUNTION     : sync_code()
# DESCRIPTION : sync source code
# PARAMETERS  : none
function sync_code()
{
	cd ${BALONG_TOPDIR}
	repo forall -c "git checkout -f HEAD"
	repo forall -c "git clean -df"
	repo sync -c
}

# FUNTION     : download_patchset()
# DESCRIPTION : download patchset
# PARAMETERS  : none
function download_patchset()
{
	date
	MANIFEST_XML_PATH=${PROJECT_TOPDIR}/.repo/manifest.xml
	cd ${BALONG_TOPDIR}
	
	for rvid in ${revision_id}
	do
		gerrit_project=`gerrit_cmd "query --current-patch-set ${rvid}" | grep "project:" | awk '{print $2}'`
		gerrit_refspec=`gerrit_cmd "query --current-patch-set ${rvid}" | grep "ref:" | awk '{print $2}'`
		
		# clear empty sepatator
		gerrit_project=`echo ${gerrit_project}`
		gerrit_refspec=`echo ${gerrit_refspec}`
		
		# local repository path
		local_repository_path=`cat ${MANIFEST_XML_PATH} | grep "${gerrit_project}" | awk -F "\"" '{print $2}'`
		local_repository_path=`echo ${local_repository_path}`
		
		#echo ${gerrit_project} ${gerrit_change_number}/${gerrit_patchset_number}
		echo ${gerrit_project} ${local_repository_path} ${gerrit_refspec}
		
		# download commit
		cd ${PROJECT_TOPDIR}/${local_repository_path}
		#repo download --cherry-pick ${gerrit_project} ${gerrit_change_number}/${gerrit_patchset_number}
		git pull -s octopus ssh://${gerrit_account_env}@${gerrit_server_env}:29418/${gerrit_project} ${gerrit_refspec}
		if [ $? -eq 0 ]
		then
			print_error_info "SUCCESS" ${gerrit_change_id_env} ${rvid} ${gerrit_project} ${gerrit_branch_env} "download code success"
		else
			message_info="This\ commit\ ${rvid}\ may\ be\ conflict,\ please\ check\ ${JOB_URL}\/${BUILD_NUMBER}"
			gerrit_cmd "review ${rvid} --verified -1 --message ${message_info}"
			print_error_info "ERROR" ${gerrit_change_id_env} ${rvid} ${gerrit_project} ${gerrit_branch_env} "This commit maybe conflict, please check. This information from function download_patchset()."
			exit 1
		fi
	done
}


# FUNTION     : clean_product()
# DESCRIPTION : clean product
# PARAMETERS  : none
function clean_product()
{
	date
	cd ${BALONG_TOPDIR}/build
	python obuild.py product=${PRODUCT_NAME} distclean
	rm -rf ${WORKSPACE}/*.log
	cd ${BALONG_TOPDIR}
	repo forall -c "git checkout -f HEAD"
	repo forall -c "git clean -df"
	echo "clean done"
}


# FUNTION     : build_product()
# DESCRIPTION : build product
# PARAMETERS  : none
function build_product()
{
	echo "call build product success" && date
	logfile_path=${BALONG_TOPDIR}/build/delivery/${PRODUCT_NAME}/log
	
	cd ${BALONG_TOPDIR}/build
	# record commit info
	mkdir -p ${logfile_path}
	
	if [ -z ${gerrit_tag_env} ]
	then
		echo "GERRIT_CHANGE_ID=${gerrit_change_id_env}" > ${logfile_path}/change_info.log
		echo "GERRIT_CHANGE_REVISION_ID=${revision_id}" >> ${logfile_path}/change_info.log
		echo "GERRIT_CHANGE_PROJECT=${revision_prj}" >> ${logfile_path}/change_info.log
		echo "GERRIT_CHANGE_BRANCH=${gerrit_branch_env}" >> ${logfile_path}/change_info.log
		echo "BALONG_PRODUCT_NAME=${PRODUCT_NAME}" >> ${logfile_path}/change_info.log
	else
		echo "TAG_NAME=${gerrit_tag_env}" > ${logfile_path}/tag.log
		echo "GERRIT_CHANGE_ID=${gerrit_change_id_env}" >> ${logfile_path}/tag.log
		echo "GERRIT_CHANGE_BRANCH=${gerrit_branch_env}" >> ${logfile_path}/tag.log
		echo "BALONG_PRODUCT_NAME=${PRODUCT_NAME}" >> ${logfile_path}/tag.log
	fi
	
	# build product
	echo "building product ${PRODUCT_NAME}" && date
	python obuild.py product=${PRODUCT_NAME} logfile=${WORKSPACE}/obuild_${PRODUCT_NAME}.log ${param_extra_env}
	if [ $? -eq 0 ]
	then
		print_error_info "SUCCESS" ${gerrit_change_id_env} "" "" "${gerrit_branch_env}" "Congratulation ! Everything has been builded success!"
	else
		if [ -n "${revision_id}" ]
		then
			message_info="product\ ${PRODUCT_NAME}\ build\ fail.Please\ check\ ${JOB_URL}${BUILD_NUMBER}"
			review_commit verify -1 "${message_info}"
		fi
		print_error_info "ERROR" ${gerrit_change_id_env} "" "" "${gerrit_branch_env}" "build fail,please check log. From function build_product()."
		exit 1
	fi
	#review_commit verify 1
	echo "build product ${PRODUCT_NAME} finish." && date
}

# FUNTION     : review_commit()
# DESCRIPTION : review commit
# PARAMETERS  : 
# $1: verify or codereview
# $2: value,-2,-1,0,1,2
function review_commit()
{
	cmd=$1
	val=$2
	msg="$3"
	if [ -z "${msg}" ]
	then
		msg="${cmd}\ ${val}\ by\ code\ gate"
	fi
	
	for id in ${revision_id}
	do
		if [ ${cmd} = "verify" ]
		then
			gerrit_cmd "review ${id} --verified ${val} --message ${msg}"
		elif [ ${cmd} = "codereview" ]
		then
			gerrit_cmd "review ${id} --code-review ${val} --message ${msg}"
		fi
	done
}

# FUNTION     : merge_commit()
# DESCRIPTION : merge commit
# PARAMETERS  : none
function merge_commit()
{
	for id in ${revision_id}
	do
		status=`gerrit_cmd "query --current-patch-set ${id}" | grep "status:" | awk '{print $2}'`
		if [ ${status} != "MERGED" ]
		then
			gerrit_cmd "review ${id} --verified 1 --code-review 2 --submit"
			status_now=`gerrit_cmd "query --current-patch-set ${id}" | grep "status:" | awk '{print $2}'`
			project=`gerrit_cmd "query --current-patch-set ${id}" | grep "project:" | awk '{print $2}'`
			if [ ${status_now} = "MERGED" ]
			then
				print_error_info "SUCCESS" ${gerrit_change_id} ${id} ${project} ${gerrit_branch_env} "" "Merge success"
				repo sync ${project}
			else
				print_error_info "ERROR" ${gerrit_change_id} ${id} ${project} ${gerrit_branch_env} "Maybe merge conflict, please login gerrit to check. This information from function merge_commit()."
				exit 1
			fi
		fi
	done
}


# FUNTION     : trigger_jenkins()
# DESCRIPTION : use it to trigger jenkins project
# PARAMETERS  : none
function trigger_jenkins()
{
	if [ -z ${BRANCH} ]
	then
		BRANCH=br_balongv7r2_bugfix
	fi
	
	if [ -z ${jenkins_project_env} ]
	then
	{
		jenkins_url_env=http://10.106.40.215:8082/
		jenkins_project_env=BALONGV7R2_BUILD_CODE_GATE
	}
	fi
	
	if [ -n "${CHANGE_ID}" ]
	then
		# partition change id with separator ","
		num=`echo ${CHANGE_ID} | awk  '{print index($0,",")}'`
		if [ ${num} -eq 0 ]
		then
			change_id=${CHANGE_ID}
		else
			change_id=`echo ${CHANGE_ID} | awk 'gsub(","," ") {print $0}'`
		fi
		
		# trigger build
		jenkins_cli=./tools/utility/tools/jenkins-cli.jar
		for id in ${change_id}
		do
			gerrit_author=`gerrit_cmd "query ${id}" | grep "name" | awk -F ":" '{print $2}'`
			gerrit_author=`echo ${gerrit_author}`
		done
		java -jar ${jenkins_cli} -s ${jenkins_url_env} build -v -s ${jenkins_project_env} -p CHANGEID="${change_id}" -p BRANCH=${BRANCH} -p AUTHOR="${gerrit_author}"
	else
		print_error_info "ERROR" ${CHANGE_ID} "" "" "" "Change id is empty . Please check . This information from function trigger_jenkins()."
		exit 1
	fi
}


# FUNTION     : print_error_info()
# DESCRIPTION : print error info
# PARAMETERS  : 
# $1: Error or Success
# $2: change id
# $3: commit id or change id
# $4: project
# $5: branch
# $6: print string
function print_error_info()
{
	echo "***********************************************************"
	echo "Status:      : $1"
	echo "change id    : $2"
	echo "commit id    : $3"
	echo "project      : $4"
	echo "branch       : $5"
	echo "jenkins info : $6"
	echo "***********************************************************"
}


# FUNTION     : help()
# DESCRIPTION : help
# PARAMETERS  : none
function help()
{
	echo "***********************************************************"
	echo "Usage:"
	echo "Manually trigger jenkins to build:"
	echo "./obuild_ci.sh prebuild CHANGE_ID"
	echo ""
	echo "Attention: The default branch is br_balongv7r2_bugfix,and you can type the command like this if you want to triggger another branch ,such as  br_feature_v7r2_gate_test:"
	echo "./obuild_ci.sh prebuild CHANGE_ID BRANCH_NAME"
	echo "***********************************************************"
}

# FUNTION     : jenkins_project_config()
# DESCRIPTION : jenkins project config
# PARAMETERS  : none
function jenkins_project_config()
{
	# configure obuild system
	export USE_CCACHE=1
	export PATH=$HOME/bin:$PATH
	export param_extra=""
	export jenkins_url_env=http://10.106.40.215:8082/
	export jenkins_project_env=BALONGV7R2_BUILD_CODE_GATE

	# configure code gate
	export gerrit_account_env=z00221552
	export gerrit_server_env=10.106.40.162
	export gerrit_branch_env=${BRANCH}
	export gerrit_url="ssh://10.106.40.162/balong/manifest.git"
	export gerrit_manifest="balongv7r2_full.xml"
	if [ -z "${gerrit_branch_env}" ]
	then
		#export gerrit_branch_env=br_feature_v7r2_gate_test
		export gerrit_branch_env=br_balongv7r2_bugfix
	fi

	# configure transfer path
	date_dir=`date +%F`
	time_dir=`date +%H-%M-%S`
	export param_extra="target_ci=${JOB_NAME}/${gerrit_branch_env}/${date_dir}/${time_dir}"

	# configure project path
	CI_ROOTDIR=/home/${gerrit_account_env}/${JOB_NAME}
	BALONG_TOPDIR=${CI_ROOTDIR}/${gerrit_branch_env}

	# init workspace
	if [ -d ${BALONG_TOPDIR} ]
	then
		echo ${BALONG_TOPDIR}
	else
		mkdir -p ${BALONG_TOPDIR}
		cd ${BALONG_TOPDIR}
		repo init -u ${gerrit_url} -b ${gerrit_branch_env} -m ${gerrit_manifest} --no-repo-verify --repo-branch=stable
		repo sync -c
	fi

	echo ${CHANGEID}
	change_id=`echo ${CHANGEID}`

	# main control
	for id in ${change_id}
	do
		export gerrit_change_id_env=${id}
		date
		cd ${BALONG_TOPDIR}
		repo forall -c "git checkout -f HEAD"
		repo forall -c "git clean -df"
		repo start jenkins_ci --all
		repo abandon jenkins_ci
		repo start jenkins_ci --all
		repo sync -c
		date

		cd ${BALONG_TOPDIR}/vendor/hisi/build
		echo "Download code"
		./obuild_ci_new.sh download ${BALONG_TOPDIR}
		date

		echo "Build code"
		./obuild_ci_new.sh build ${BALONG_TOPDIR} hi6930_v7r2_udp
		date
		./obuild_ci_new.sh build ${BALONG_TOPDIR} hi6930_v7r2_e5_dcm
		date
		
		echo "Verify commit"
		./obuild_ci_new.sh review verify 1

		echo "Trigger project merge"
		#java -jar jenkins-cli.jar -s http://10.106.40.227:8082/ build BALONGV7R2_MERGE_CODE_GATE -p CHANGEID=${id} BRANCH=${BRANCH} AUTHOR=${AUTHOR}
	done
	echo "***********************************************************************************************"
	echo "BALONGV7R2_BUILD_CODE_GATING build success!"
	echo "The project BALONGV7R2_MERGE_CODE_GATING will be triggered until the commit has been review+1."
	echo "Please check !"
	echo "***********************************************************************************************"
}


##########################################
# main control
##########################################
case $CMD in
	trigger) trigger_jenkins;;
	download) init_id; download_patchset;;
	build) init_id; clean_product; build_product;;
	build_tag) clean_product; build_product;;
	check) init_id; check_commit;;
	merge) init_id; merge_commit;;
	review) init_id; review_commit ${ACTION} ${VALUE} "${MSGINFO}";;
	help) help;;
	*) echo "unknown command", $CMD;;
esac

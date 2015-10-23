#!/bin/bash
set -ex
#set -n

CMD=$1
PROJECT_TOPDIR=$2
PRODUCT_NAME=$3
CHANGESET_FILE=$4
PARAM_EX=$5
BALONG_TOPDIR=${PROJECT_TOPDIR}/vendor/hisi

#variable state
# The following variables come from env .
#
# gerrit_account
# gerrit_server
# gerrit_change_id
# gerrit_patchset_revision
# gerrit_branch
# 

# set revision_id
revision_id=
all_revision_id=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${gerrit_change_id} | grep "revision" | awk '{print $2}'`
for filter_id in ${all_revision_id}
do
	gerrit_branch_name=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${filter_id} | grep "branch" | awk '{print $2}'`
	if [ ${gerrit_branch_name} = ${gerrit_branch} ]
	then
		revision_id="${revision_id} ${filter_id}"
	fi
done
echo ${revision_id}

# function area
#

# review commit
function detect_commit()
{
	echo ${gerrit_change_id}
	FLAG="0"
	
	ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit review ${gerrit_patchset_revision} --code-review 2
	
	if [ -n ${gerrit_change_id} ]
	then
#		revision_id=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${gerrit_change_id} | grep "revision" | awk '{print $2}'`
		echo "revision_id : ${revision_id}"
		for id in ${revision_id}
		do
			review_type=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${id} | grep "type" | grep "CRVW" | awk '{print $2}'`
			rowCount=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${id} | grep "rowCount" | awk '{print $2}'`
			project=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${id} | grep "project" | awk '{print $2}'`
			echo "review_type : ${review_type}"
			echo "project : ${project}"
			
			# if current project has been revewed
			for tp in ${review_type}
			do
				if [ ${tp} = "CRVW" ]
				then
					let FLAG=${FLAG}+1
				else
					echo "Project: ${project} should reveiw+1"
					exit 1
				fi
			done
		done
	fi
}

# sync source 
function sync_source()
{
	date
	echo ok > ${WORKSPACE}/dummy.txt
	rm -rf  ${WORKSPACE}/*
	cd ${BALONG_TOPDIR}
	#repo forall -c "git checkout -f HEAD^1"
	#repo forall -c "git clean -df"
	#repo sync -j 1

	for rvid in ${revision_id}
	do
		gerrit_project=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${rvid} | grep "project" | awk '{print $2}'`
		gerrit_change_number=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${rvid} | grep "ref" | awk -F "/" '{print $4}'`
		gerrit_patchset_number=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${rvid} | grep "ref" | awk -F "/" '{print $5}'`
		
		# clear empty sepatator
		gerrit_change_number=`echo ${gerrit_change_number}`
		gerrit_patchset_number=`echo ${gerrit_patchset_number}`
		
		echo ${gerrit_project}
		echo ${gerrit_change_number}
		echo ${gerrit_patchset_number}
		echo ${gerrit_project} ${gerrit_change_number}/${gerrit_patchset_number}
		
#		repo download ${gerrit_project} ${gerrit_change_number}/${gerrit_patchset_number}
		repo download --cherry-pick ${gerrit_project} ${gerrit_change_number}/${gerrit_patchset_number}
		echo "download code success"
	done
}

# clean product
function clean_product()
{
	date
	cd ${BALONG_TOPDIR}
	cd build
	python obuild.py product=${PRODUCT_NAME} clean logfile=${WORKSPACE}/obuild_${PRODUCT_NAME}.log  ${PARAM_EX}
	cd ${BALONG_TOPDIR}
	repo forall -c "git checkout -f HEAD"
	repo forall -c "git clean -df"
}

# build product
function build_product()
{
	date
	echo ok > ${WORKSPACE}/dummy.txt
	rm -rf  ${WORKSPACE}/*
	export USE_CCACHE="1"
	cd ${BALONG_TOPDIR}
	mkdir -p ./build/delivery/${PRODUCT_NAME}/log
	echo "GERRIT_CHANGE_ID=${GERRIT_CHANGE_ID}" > ./build/delivery/${PRODUCT_NAME}/log/change_info.log
	echo "GERRIT_CHANGE_SUBJECT=${GERRIT_CHANGE_SUBJECT}" >> ./build/delivery/${PRODUCT_NAME}/log/change_info.log
	echo "GERRIT_CHANGE_OWNER_EMAIL=${GERRIT_CHANGE_OWNER_EMAIL}" >> ./build/delivery/${PRODUCT_NAME}/log/change_info.log
	echo "BALONG_PRODUCT_NAME=${PRODUCT_NAME}" >> ./build/delivery/${PRODUCT_NAME}/log/change_info.log

	if [ -f ${WORKSPACE}/changesets.log ]; then
		cp -f ${CHANGESET_FILE} build/delivery/${PRODUCT_NAME}/log/changesets.log
	fi
	cd build
	python obuild.py product=${PRODUCT_NAME} logfile=${WORKSPACE}/obuild_${PRODUCT_NAME}.log ${PARAM_EX} ${param_extra}
	date
}

# merge commit
function submit_commit()
{
	for id in ${revision_id}
	do
		status=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${id} | grep "status" | awk '{print $2}'`
		if [ ${status} = "MERGED" ]
		then
			echo ""
		else
			ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit review ${id} --verified 1 --code-review 2
			ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit review ${id} --submit
			status_now=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${id} | grep "status" | awk '{print $2}'`
			if [ ${status_now} = "MERGED" ]
			then
				echo "Merge success"
			else
				echo "Merge fail"
				echo "Maybe merge conflict, please login gerrit to check."
				exit 1
			fi
		fi
	done
	
}

function reset_code()
{
	project=`ssh -p 29418 ${gerrit_account}@${gerrit_server} gerrit query --current-patch-set ${gerrit_change_id} | grep "project" | awk '{print $2}'`
	for prj in ${project}
	do
		prj_local_path=`cat ${PROJECT_TOPDIR}/.repo/manifest.xml | grep "${prj}" |  awk '{print $2}' | awk -F "\"" '{print $2}'`
		echo "add by zhangteng ${PROJECT_TOPDIR}/${prj_local_path}"
		cd ${PROJECT_TOPDIR}/${prj_local_path}
		git reset HEAD^1
		git checkout  -f .
	done
}

# main control
case $CMD in
	sync) sync_source;;
	build) clean_product; build_product;;
	review) detect_commit;;
	submit) submit_commit;;
	reset) reset_code;;
	*) echo "unknown command", $CMD;;
esac

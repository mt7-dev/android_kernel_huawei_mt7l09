#!/bin/bash
transfer_path=$1
transfer_file=$2

transfer_time=`date -d  now +%Y%m%d_%H%M%S`
echo . > $transfer_path/TransferFinish.flg

echo prompt off >upload.ftp
echo open 10.97.84.32 >>upload.ftp
echo user jenkins_ci transfer >>upload.ftp
echo lcd $transfer_path>>upload.ftp
echo bin >>upload.ftp
echo mkdir /home/jenkins_ci/ftphome/$transfer_time >>upload.ftp
echo mkdir /home/jenkins_ci/ftphome/$transfer_time/$transfer_file >>upload.ftp
echo cd /home/jenkins_ci/ftphome/$transfer_time/$transfer_file >>upload.ftp
echo put $transfer_file >>upload.ftp
echo put TransferFinish.flg >>upload.ftp
echo bye >>upload.ftp
echo quit >>upload.ftp
echo exit >>upload.ftp

# ftp......
ftp -i -n <upload.ftp

rm -f $transfer_path/TransferFinish.flg
rm -f upload.ftp

echo Finish 


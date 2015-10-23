#!/bin/bash

#1 compile
hibuild_script_path=`pwd`

if [ -f /usr/local/bin/hi_build ] && [ $LOGNAME != "jenkins_ci" ]
then
 change_role=hi_build
fi

#compileTime=`date -d  now +%Y%m%d_%H%M%S`

# compile
cd  $hibuild_script_path

$change_role python -B obuild.py $* buildowner=$LOGNAME hs_sh=true

exit "$?"

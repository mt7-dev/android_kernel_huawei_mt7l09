#!/bin/bash
set -x


if [ ${LOGNAME} == "jenkins_ci" ] || [ ${LOGNAME} == "phisik3" ] || [ ${LOGNAME} == "jenkins" ]
then
    python -B obuild.py $* buildowner=${LOGNAME} hs_sh=true 
elif [ -f /usr/local/bin/hi_build ]
then
    change_role=hi_build
    ${change_role} python -B obuild.py $* buildowner=${LOGNAME} hs_sh=true
else
    #if no hi_build there, regard it as beijing env.
    python -B obuild.py $* buildowner=${LOGNAME}
fi

exit "$?"


#!/bin/bash
set -e
echo "****************************************************************"
echo "ATTENTION: All uncommit changes will be deleted!!! sure(yes|no)?"
echo "****************************************************************"

read sure_ok
if [ "$sure_ok" != "yes" ]; then
	exit 1
fi
cd ../../../
repo forall -c git checkout -f HEAD
repo forall -c git clean -dxf
repo sync -j 1
exit 0

#!/bin/bash 
set -e

ANDROID_ROOT_DIR=$1
ANDROID_PRODUCT_NAME=$2
ANDROID_PRODUCT_MODE=$3

cd ${ANDROID_ROOT_DIR}

. build/envsetup.sh

choosecombo release ${ANDROID_PRODUCT_NAME} eng normal

./prebuilts/misc/linux-x86/ccache/ccache -M 30G
android_pkgs="lpm3image"
echo "make  -j 48 " ${android_pkgs}
make  -j 48 ${android_pkgs}

exit $?

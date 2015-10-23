#!/system/bin/busybox sh
cdrom=$(cat /proc/mtd|grep cdromiso|cut -d: -f1|sed 's/mtd/mtdblock/')
sdpath=/dev/block/platform/hi_mci.0
gadget=/sys/devices/platform/dwc3/gadget
mntpath=/mnt/sd
for i in  $(ls $gadget|grep lun)
do 
	if [ "$(cat $gadget/$i/ro)" = "1" ];then
        cdromlun=$gadget/$i/file
    elif 
        [ "$(cat $gadget/$i/mode)" = "0" ];then
		sdlun=$gadget/$i/file
    fi

done

echo /dev/block/$cdrom > $cdromlun

if [ $(ls $sdpath|grep mmcblk[0-9]p1) ];then
    echo $sdpath/$(ls $sdpath|grep mmcblk[0-9]p1$) > $sdlun
    mount -t vfat $sdpath/$(ls $sdpath|grep mmcblk[0-9]p1$) $mntpath

elif 
    [ $(ls $sdpath|grep mmcblk[0-9]$) ];then
   echo $sdpath/$(ls $sdpath|grep mmcblk[0-9]$) > $sdlun 
   mount -t vfat $sdpath/$(ls $sdpath|grep mmcblk[0-9]$) $mntpath

fi

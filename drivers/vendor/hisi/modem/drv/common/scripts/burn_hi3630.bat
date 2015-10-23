fastboot -i 0x12d1 flash xloader            sec_xloader.img
fastboot -i 0x12d1 flash ptable             ptable.img
fastboot -i 0x12d1 flash fastboot           fastboot.img
fastboot -i 0x12d1 reboot-bootloader
ping -n 6 127.0.0.1>nul
fastboot -i 0x12d1 flash dts                dt.img
fastboot -i 0x12d1 flash fw_lpm3            lpm3.img
fastboot -i 0x12d1 flash sensorhub          sensorhub.img
fastboot -i 0x12d1 flash fw_hifi            hifi_k3v3.img
fastboot -i 0x12d1 flash teeos              trustedcore.img
fastboot -i 0x12d1 flash boot               boot.img
fastboot -i 0x12d1 flash nvme               nvme.img
fastboot -i 0x12d1 flash recovery           recovery.img
fastboot -i 0x12d1 flash cache              cache.img
fastboot -i 0x12d1 flash userdata           userdata.img
fastboot -i 0x12d1 flash system             system.img
fastboot -i 0x12d1 flash modemnvm_update    nv.bin
fastboot -i 0x12d1 flash modem              balong_modem.bin 
fastboot -i 0x12d1 flash modem_dsp          lphy.bin
pause

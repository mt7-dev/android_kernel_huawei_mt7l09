fastboot -i 0x12d1 flash ptable ptable.bin
fastboot -i 0x12d1 flash m3boot m3boot.bin
fastboot -i 0x12d1 flash m3image bsp_mcore.bin
fastboot -i 0x12d1 flash fastboot fastboot.img
fastboot -i 0x12d1 flash kernel boot.img
fastboot -i 0x12d1 flash vxworks balong_modem.bin
fastboot -i 0x12d1 erase system
fastboot -i 0x12d1 flash system system.img
fastboot -i 0x12d1 flash dsp lphy.bin
fastboot -i 0x12d1 flash nvdload nv.bin

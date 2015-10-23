目录结构：
1、include目录包含公共头文件；
2、source目录为源码目录，每个驱动模块放在对应的文件夹中，均包含Makefile文件，可以单独编译；
3、bin为最终结果目录。


编译说明：
1、本地必须安装Wind River Workbench 3.2。
2、将本文件夹内所有文件拷贝到本地。
3、在“VxWorks Development Shell”中，运行make，可以编译所有驱动，在bin文件夹下生成OnChipRom.elf、OnChipRom.bin等文件。
4、在“VxWorks Development Shell”中，运行“make 各驱动模块”，可以单独编译该驱动。

================================================================
修改记录：

2014-03-06 移植v7r2 onchiprom代码，并针对p532差异点修改适配。
2014-03-10 串口配置帧格式data修改为8bit，a9 pll和分频比修改。
2014-03-17 修改USB解复位，value |= (uint32_t)(0x1F << 21);
2014-03-21 比对v711合入nandc自适应修改。
2014-03-27 比对v711合入nandc自适应修改，以及数据类型定义。
2014-03-28 将USB地址原为魔鬼数字，修改为宏定义。
2014-03-29 USB注释掉软件配置fifo部分，crc计算优化。
2014-04-03 USB使用2.0配置USB3_SPEED_HS_PHY_30MHZ_OR_60MHZ

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

2011-4-11 吴择淳
搭建OnChipRom编译框架，支持单独编译各驱动模块。
代码大多从BalongV3R1拿过来，编译链接不通过，后续需根据V7R1的IP和安全启动流程做修改。

2011-7-18 吴择淳
优化Makefile结构,实现lib方式增量编译.

2011-11-7 吴择淳
MPW回片验证后,修改加入:
1.eMMC IO管脚复用配置;
2.NANDC4新IP支持,增大Flash脉宽;
3.HS UART加入波特率配置延时;
4.外部PMU打开;
5.USB控制器升级适配;

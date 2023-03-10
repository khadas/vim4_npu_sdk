一、配置cmake编译环境：
    1、验证是否存在cmake 环境：
        cmake --version 显示cmake 版本即为成功。如果不存在cmake，执行后面几步配置cmake环境
    2、 安装cmake
        pip3 install cmake==3.16.3

    3、 验证是否安装成功
        cmake --version
        显示cmake 版本即为成功

二、配置编译工具链
    1、 下载工具链
        32位：https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads/ 链接下
            gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz

        64位：当前暂时没有64bit版本，暂时不添加。如果有客户使用对应版本且未及时添加toolchain路径，请反馈。

    2、 arm-none-linux-gnueabihf安装
        a、解压，并放置在自己需要的文件夹内
            tar -xvJf ***.tar.xz
        b、编辑bash.bashrc文件
            sudo vi ~/.bashrc
        c、添加环境变量
            export PATH=path_to/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin:$PATH
        d、更新环境变量
            source ~/.bashrc
        e、检查环境是否加入成功
            执行：arm-none-linux-gnueabihf-gcc -v，查看gcc 32bit版本

    注：详细过程可参考：https://www.cnblogs.com/flyinggod/p/9468612.html

三、编译可执行文件
    a.修改编译脚本文件CMakeLists.txt
        1.line6: 设置ADLA_TOOL_PATH的路径
            SET(ADLA_TOOL_PATH  ../../../../) --> SET(ADLA_TOOL_PATH  /home/xxx/adla-toolkit-binary-1.6.10.3)
        2.设置编译32位、64位可执行文件：
        line5:
            SET(LINUX_32 linux)   -->编译32位可执行文件
            改成 SET(LINUX_64 linux)   -->编译64位可执行文件
    b.编译可执行文件：
        mkdir build
        cd build
        cmake ..
        make
    结果：生成可执行文件 adla_nnsdk_test_32

四、运行可执行文件
    1、将当前整个目录拷贝到/data目录下
    2、执行命令：
        ./adla_nnsdk_test_32 xxxxxxx.adla xxxxxx.jpeg
        2: 18.807236
        125: 12.043229
        120: 11.053375
        121: 11.053375
        141: 11.053375
1、 Configure the cmake compilation environment:
    1. Verify whether there is a cmake environment:
       cmake --version shows the cmake version is successful. If cmake does not exist, perform the following steps to configure the cmake environment
    2. Install cmake
        pip3 install cmake==3.16.3
    3. Verify that the installation is successful
        cmake --version
        Displaying the cmake version is successful

2. Configure the compilation toolchain
    1. Download the toolchain
        32位：https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads/ 链接下
            gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf.tar.xz

        64-bit: There is currently no 64-bit version, and it will not be added for the time being. If some customers use the corresponding version and did not add the toolchain path in time, please give feedback.

    2、 arm-none-linux-gnueabihf install
        a. Unzip it and place it in the folder you need
            tar -xvJf ***.tar.xz
        b. Edit the bash.bashrc file
            sudo vi ~/.bashrc
        c. Add environment variables
            export PATH=path_to/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin:$PATH
        d. update environment variables
            source ~/.bashrc
        e. Check if the environment is successfully joined
           Execue：arm-none-linux-gnueabihf-gcc -v，View gcc 32bit version

    Note: The detailed process can refer to: https://www.cnblogs.com/flyinggod/p/9468612.html

三、Compile the executable
    a.Modify the compilation script file CMakeLists.txt
        1.line6: Set the path of ADLA_TOOL_PATH
            SET(ADLA_TOOL_PATH  ../../../../) --> SET(ADLA_TOOL_PATH  /home/xxx/adla-toolkit-binary-1.6.10.3)
        2.Set to compile 32-bit and 64-bit executable files:
        line5:
            SET(LINUX_32 linux)   -->Compile a 32-bit executable
            SET(LINUX_64 linux)   -->Compile a 64-bit executable
    b.Compile：
        mkdir build
        cd build
        cmake ..
        make
    Result: build executable adla_nnsdk_test_32

四、run the executable file
    1、Copy the entire current directory to the /data directory
    2、Execute the command:
        ./adla_nnsdk_test_32 xxxxxxx.adla xxxxxx.jpeg
        2: 18.807236
        125: 12.043229
        120: 11.053375
        121: 11.053375
        141: 11.053375
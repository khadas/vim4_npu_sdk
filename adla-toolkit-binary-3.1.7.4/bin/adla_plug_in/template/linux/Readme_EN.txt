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
            SET(YOCTO_64 linux)   -->Compile a YOCTO 64-bit executable
    b.Compile：
        mkdir build
        cd build
        cmake ..
        make
    Result: build executable adla_nnsdk_test_32

四、run the executable file
    1、Copy the entire current directory to the /data directory
    2、Generate the input bin file
         Because the current code only supports JPEG/JPG/bin inputs, non-image inputs need to be in binary bin format. If you want to quickly run the compiled executable file, you can use the script that comes with the tool to generate a bin file corresponding to the random input, refer to /xxx/adla-toolkit-binary-x.x.x.x /bin/adla_plug_in/tools/input_npy_txt_bin_generate.py
    3、Execute the command:               
         ./adla_nnsdk_test_64 xxxxxxx.adla input_1 input_2 ... input_n 0 1 0 0
         Usage: ./adla_nnsdk_test_64 [adlafilepath] [input 1 filepath] [input 2 filepath] ... [input n filepath]  [outputtype] [looptimes] [performance_test] [use_dma]

Note :
        [adlafilepath] ------------------> Path to the adla model file
        [input 1 filepath] [input 2 filepath] ... [input n filepath] -----------------> The individual inputs to the adla model only support JPEG/JPG/bin.
        [outputtype] ------------------>Set the type of the model output buffer, where 0 (default) indicates the output buffer is float, and 1 indicates the output buffer is of the same data type as the model output.
        [looptimes] ------------------->Set the number of model inference iterations.  looptimes=100 (default).
        [performance_test]------------>Set whether to enable model performance testing, which will output the time taken for the set_buf, inference, and quantize to float processes during model inference. 0 (default) indicates disabled, while 1 indicates enabled.
        [use_dma]--------------------->Set whether to use DMA input mode. 0 (default) indicates not using DMA, while 1 indicates using DMA.
一、配置ndk编译环境：
    1、 验证是否存在ndk-build编译环境：
        执行：ndk-build -v，有版本信息提示即为成功。如果没有，参照后面几步配置ndk-build编译环境
    2、下载android-ndk-r21e
        https://github.com/android/ndk/wiki/Unsupported-Downloads
    3、 解压，获得路径/xxxx/android-ndk-r21e
    4、配置环境变量
        a.编辑bash.bashrc文件
            sudo vi ~/.bashrc
            在文件末尾添加： export PATH=/xxxx/android-ndk-r21e:$PATH
        b.更新环境变量
            source ~/.bashrc
        c.检查环境是否加入成功
            ndk-build --version  有版本信息显示

二、编译可执行文件
    1.修改jni/Android.mk 文件line7
        ADLA_TOOL_PATH=/xxx/xxx/adla-toolkit-binary-1.6.10.3
    2.设置编译32位、64位可执行文件：
        在jni目录下执行：
            cp Application_64.mk Application.mk 配置编译64位可执行文件
            cp Application_32.mk Application.mk 配置编译32位可执行文件
    3.编译：
        在jni同级目录下执行，ndk-build
        生成可执行文件：
            libs/armeabi-v7a/adla_nnsdk_test_32
            libs/arm64-v8a/adla_nnsdk_test_64

三、运行可执行文件
    1、将当前整个目录拷贝到/data目录下
    2、生成输入的bin文件
        因为当前代码中输入仅支持JPEG/JPG/bin，因此对于非图像类型的输入需要使用二进制bin文件。如果要快速跑起编译的可执行文件，可以使用工具中自带的脚本生成随机输入对应的bin文件，参考/xxx/adla-toolkit-binary-x.x.x.x/bin/adla_plug_in/tools/input_npy_txt_bin_generate.py
    3、执行命令：
         ./adla_nnsdk_test_64 xxxxxxx.adla input_1 input_2 ... input_n 0 1 0 0
        Usage: ./adla_nnsdk_test_64 [adlafilepath] [input 1 filepath] [input 2 filepath] ... [input n filepath]  [outputtype] [looptimes] [performance_test] [use_dma]

    参数详解：
    [adlafilepath] ------------------> adla模型文件的路径
    [input 1 filepath] [input 2 filepath] ... [input n filepath] -----------------> adla模型的各个输入，目前支持JPEG/JPG/bin。
    [outputtype] ------------------>设置模型输出buf的类型，0（default）表示输出buf为float类型，1表示输出buf为模型输出对应的数据类型。
    [looptimes] ------------------->设置模型推理次数，looptimes=100(default).
    [performance_test]------------>设置是否开启模型性能测试，将分别输出模型运行过程中set_buf、inference、quantize to float的时间。0（default）表示不开启，1表示开启。
    [use_dma]--------------------->设置是否使用DMA输入模式。0（default）表示不使用，1表示使用。
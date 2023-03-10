1.configure the ndk compilation environment：
    2.1 Downloadandroid-ndk-r21e
        https://github.com/android/ndk/wiki/Unsupported-Downloads
    2.2 unzip
    2.3 Set environment variables
        export PATH=/home/siqiyang/work/ndk/android-ndk-r21e:$PATH

二、Compile the executable file
    1.Modify jni/Android.mk file line7
        ADLA_TOOL_PATH=/xxx/xxx/adla-toolkit-binary-1.6.10.3
    2.Set to compile 32-bit and 64-bit executable files:
        Execute in the jni directory:
            cp Application_64.mk Application.mk -->Configure to compile 64-bit executables
            cp Application_32.mk Application.mk -->Configure to compile 32-bit executables

    3.compile：
        Execute in the same level directory as jni, ndk-build
        generate executable：
            libs/armeabi-v7a/adla_nnsdk_test_32
            libs/arm64-v8a/adla_nnsdk_test_64

三、Run the executable file
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
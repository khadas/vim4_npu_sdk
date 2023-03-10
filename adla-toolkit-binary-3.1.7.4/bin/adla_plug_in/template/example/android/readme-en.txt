1. image burning
  The S5&T7C platform must use the USB burning tool V3.2.4, otherwise the burning may be unsuccessful. Follow the steps to refer to "V3 Tool Installation Steps and Notes.doc"

2. configure the ndk compilation environment：
    2.1 Downloadandroid-ndk-r21e
        https://github.com/android/ndk/wiki/Unsupported-Downloads
    2.2 unzip
    2.3 Set environment variables
        export NDKROOT=/home/siqiyang/work/ndk/android-ndk-r21e
        export PATH=$NDKROOT:$PATH

3、Compile the executable file
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

4. the board environment preparation
    4.1 Set root permissions
        Open power shell and enter adb root; adb remount

    4.2 Push the nnsdk and library files to the board
        adb push /example/lib/lib64/mobilenet_v2_int8.so /vendor/lib

5. executable file running
    5.1 Copy the entire current directory to the /vendor directory
    5.2 Execute the command：
        ./package/adla_nnsdk_testxx ./package/image_classify_xxx.adla ./package/fish_i8.bin
        2: 18.642260
        125: 11.713278
        120: 10.888399
        141: 10.888399
        121: 10.723424

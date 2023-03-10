一、版本烧录
  S5,T7C平台必须使用USB烧录工具V3.2.4，否则可能导致烧录不成功，按照步骤请参考《V3工具安装步骤及注意.doc》

二、配置ndk编译环境：
	1、下载android-ndk-r21e
		https://github.com/android/ndk/wiki/Unsupported-Downloads
	2、 解压
	3、 设置环境变量
        export NDKROOT=/home/siqiyang/work/ndk/android-ndk-r21e
        export PATH=$NDKROOT:$PATH

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

四、板端环境准备
    1、设置root权限
        打开power shell，依次输入adb root; adb remount

    2、将nnsdk与jpeg_t库文件push到板端
        adb push /example/lib/lib64/libnnsdk.so /vendor/lib

五、可执行文件运行
    1、将当前整个目录拷贝到/vendor目录下
    2、执行命令：
        ./package/adla_nnsdk_testxx ./package/image_classify_xxx.adla ./package/fish_i8.bin
        2: 18.642260
        125: 11.713278
        120: 10.888399
        141: 10.888399
        121: 10.723424

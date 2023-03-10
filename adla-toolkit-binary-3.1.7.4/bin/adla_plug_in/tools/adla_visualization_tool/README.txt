### 应用背景

    ADLA Netron 是基于开源软件Netron开发的可视化神经网络模型工具，它提供了直观的界面，能够帮助用户更好地理解和分析ADLA神经网络模型的结构、层次以及硬件版本等参数。

### Netron Version 1.4

    OS : x64 / linux

### 功能介绍

    1. ADLA Netron / Compiler / Aml HW 等Version信息、cpu op type 及 adla model system memory size

    2. 模型Inputs / Outputs，包括name、type(uint8/int8/int32...)、quantization、location

    3. 中间Nodes的参数信息，包括node type、op_type(Hardware/Software)、Node inputs / Outputs ：[name、type(uint8/int8/int32...)、quantization、location]

### 使用方法

    1. 模型转换时, 设置模型转换工具的环境变量: export ADLA_DUMP_MODEL_INFO=True
       同时确保dev.json文件有如下配置
        · 设置 "disable_fusion": 1
        · 如果DDK Version高于2.1.1.6, 还需要设置 "include_debug_info": 1

    2. adla 可视化
        · x64: 在netron.exe中选择打开adla文件即可
        · linux: ./netron ***.adla 会自动弹出可视化界面

### adla可视化显示请见"demo"文件夹(以模型inceptionv1_uint8.adla为例)

    1. 模型可视化结构图: [inceptionv1_uint8_v0.7.3.png]

    2. ADLA Netron / Compiler / Aml HW 等Version信息: [version_details.png]

    3. 中间层node包含的参数信息: [node_details.png]
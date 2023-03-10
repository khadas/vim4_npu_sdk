### Application Background

    ADLA Netron is a tool for visualizing neural network models based on the open-source software Netron, which provides an intuitive interface that helps users better understand and analyze parameters such as the structure, hierarchy, and hardware version of ADLA neural network models.

### Netron Version 1.4

    OS : x64 / linux

### Features

    1. ADLA Netron / Compiler / Aml HW etc. Version info, cpu op type and adla model system memory size

    2. model Inputs / Outputs, including name, type (uint8/int8/int32...), quantization, location, etc. Model Inputs / Outputs, including name, type (uint8/int8/int32...), quantization, location

    3. Parameter information of intermediate Nodes, including node type, op_type(Hardware/Software), Node inputs / Outputs:[name, type(uint8/int8/int32...), quantization, location name, type(uint8/int8/int32...), quantization, location]

### Usage

    1. When converting models, set the environment variable of the model conversion tool: export ADLA_DUMP_MODEL_INFO=True
       Also make sure the dev.json file has the following configurations
        - Set "disable_fusion": 1
        - If the DDK Version is higher than 2.1.1.6, you also need to set "include_debug_info": 1.

    2. adla visualization
        - x64: Just select open adla file in netron.exe.
        - linux: . /netron ***.adla will automatically pop up the visualization interface.

### adla visualization see "demo" folder (take model inceptionv1_uint8.adla as an example)

    1. model visualization structure: [inceptionv1_uint8_v0.7.3.png]

    2. Version information of ADLA Netron / Compiler / Aml HW etc.: [version_details.png].

    3. information about parameters included in the middle-layer node: [nodes_details.png]

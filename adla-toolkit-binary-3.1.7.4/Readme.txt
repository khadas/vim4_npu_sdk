Model conversion reference demo/convert_adla.sh and model conversion guidance document

adla-toolkit-binary-x.x.xx.x: Amlogic model conversion tool and sdk development kit

Directory Introduction:
    demo/:
        Some demo models and conversion scripts provided to demonstrate model conversion
    bin/:
        ├── adla_convert                   -->bin file for model conversion
        ├── adlalib                        -->Some dependent files of adla_convert
        └── adla_plug_in
            ├── common                     -->Configuration files for some model conversions
            ├── template
            │      ├── android             -->Template code for Android environment
            │      ├── linux               -->Template code for Linux environment
            │      ├── example             -->The demo code of nnsdk will be readjusted in the future
            │      │      ├── android      -->Android demo code
            │      │      └── linux        -->Linux demo code
            │      └── nnsdk               -->nnsdk development kit
            └── tools
                ├── adla_info_tool                 -->adlainfo tool,parse adla file information
                ├── adla_visualization_tool        -->adla netron tool, adla file visualization tool
                ├── NNIDE_tool                     -->adla NNIDE tool, Neural Network Integrated Development Environment
                ├── input_npy_txt_bin_generate.py  -->Generate random input npy/txt/bin file script
                ├── tflite_refactorer_tool         -->tflite refactorer tool, Applicable to use NNAPI to integrate apk scenarios
                └── Quantize_optimization_tool     -->Optimize quantization accuracy tools

Note:
    1、NNSDK development kit path: adla-toolkit-binary-x.x.xx.x/bin/adla_plug_in/template/nnsdk
    2、NNSDK demo code: adla-toolkit-binary-x.x.xx.x/bin/adla_plug_in/template/example
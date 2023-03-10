##This documentation mainly introduces the catalog price structure of the NNIDE tool.


1、The directory structure is as follows
├── demo
│   ├── amlnn                               #Compiled file of nnide_tool	
│   ├── dataset                             #Dataset directory
│   │   ├── aml_dataset_crop_inception          #Classification model test data set(490 pictures)
│   │   └── input                               #Single frame test data
│   ├── demo_adla_compare_buf.py            #Comparison script between the accuracy of the model file saved on the PC and the accuracy of the board end
│   ├── demo_adla_jupyter.ipynb				
│   ├── demo_adla_run_datasets.py           #Classification model top1/top5 accuracy test script
│   ├── demo_adla_run_multi_io.py           #Multi-input model inference script
│   ├── demo_adla_run_single_io.py          #Single input model inference script
│   ├── model_file                          #Test related model files
│   │   ├── adla                                #Converted adla file
│   │   └── original                            #Original model files, and conversion scripts
│   ├── NNIDE_package                       #Board end IDE executable file					
│   │   ├── android_32                      #Android 32-bit system IDE execution file
│   │   │   └── NNIDE
│   │   ├── android_64                      #Android 64-bit system IDE execution file
│   │   │   └── NNIDE
│   │   ├── linux_32                        #Linux 64-bit system IDE execution file
│   │   │   └── NNIDE
│   │   └── yocto_64                        #Yocto 64-bit system IDE execution file
│   │   └── NNIDE
│   └── requirements.txt                    #PC environment dependency list
└── Readme.txt

Note:
	1. Unzip command: tar -zxf demo.tar.gz
	2. For detailed execution steps, please refer to the document: NNIDE_1.x.x.docx
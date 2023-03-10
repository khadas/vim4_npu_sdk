/****************************************************************************
*
*    Copyright (c) 2019  by amlogic Corp.  All rights reserved.
*
*    The material in this file is confidential and contains trade secrets
*    of amlogic Corporation. No part of this work may be disclosed,
*    reproduced, copied, transmitted, or used in any way for any purpose,
*    without the express written permission of amlogic Corporation.
*
***************************************************************************/
/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "nn_sdk.h"
#include "nn_util.h"
#include <pthread.h>
#include "jpeglib.h"

static int input_width = 0, input_high = 0;

///////////////////////////////////////////////////////////
nn_input inData;

int destroy_network(void *qcontext)
{
    int ret = aml_module_destroy(qcontext);
    return ret;
}

int run_network(int qnetType, void *qcontext, char *input_path)
{
    int ret = 0;
    unsigned char *rawdata = NULL;
    nn_output *outdata = NULL;
    aml_output_config_t outconfig;

    FILE *File = NULL;
    File = fopen(input_path, "rb");
    if (File == NULL)
    {
        printf("File null returned\n");
        return -1;
    }
    rawdata = (unsigned char *)malloc(input_width * input_high * 3 * sizeof(char));
    fread(rawdata, sizeof(char), input_width * input_high * 3  ,File);
    fclose(File);
    if (rawdata == NULL)
    {
        printf("face_compare_data null returned\n");
        return -1;
    }
    inData.input = rawdata;

    ret = aml_module_input_set(qcontext, &inData);
    memset(&outconfig,0,sizeof(aml_output_config_t));

    outconfig.format = AML_OUTDATA_FLOAT32;
    outconfig.typeSize = sizeof(aml_output_config_t);
    outdata = (nn_output*)aml_module_output_get(qcontext, outconfig);

    if (outdata == NULL)
    {
        printf("aml_module_output_get error\n");
        return -1;
    }
    process_top5((float*)outdata->out[0].buf,outdata->out[0].size/sizeof(float),NULL);

    return ret;
}

void* init_network_file(int argc,char **argv)
{
    int size = 0;
    void *qcontext = NULL;
    aml_config config;
    memset(&config, 0, sizeof(aml_config));
    input_width = 224;
    input_high = 224;

    config.typeSize = sizeof(aml_config);
    config.nbgType = NN_ADLA_FILE;
    config.path = (const char *)argv[1];
    config.modelType = ADLA_LOADABLE;

    inData.input_index = 0;
    inData.size = input_width * input_high * 3;
    inData.input_type = BINARY_RAW_DATA;

    qcontext = aml_module_create(&config);
    if (qcontext == NULL)
    {
        printf("amlnn_init is fail\n");
        return NULL;
    }

    if (config.nbgType == NN_ADLA_MEMORY && config.pdata != NULL)
    {
        free((void*)config.pdata);
    }
    return qcontext;
}

int main(int argc,char **argv)
{
    if (argc != 3)
	{
		printf("Usage: ./adla_nnsdk_test_32 [xxx.adla] [input]\n");
		return -1;
	}

    int ret = 0;
    char* jpath = (char *)argv[2];
    void *context = NULL;
    int netType = 0;

    context = init_network_file(argc, argv);
    if (context == NULL)
    {
        printf("init_network fail.\n");
        return -1;
    }

    ret = run_network(netType, context, jpath);
    if (ret != 0)
    {
        printf("run_network fail.\n");
        return -1;
    }

    ret = destroy_network(context);
    if (ret != 0)
    {
        printf("destroy_network fail.\n");
        return -1;
    }
    return ret;
}
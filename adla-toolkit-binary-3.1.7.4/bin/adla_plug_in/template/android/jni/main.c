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
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "jpeglib.h"
#include <time.h>
#include <sys/types.h>
#include <ctype.h>

#include "nn_sdk.h"
#include "nn_util.h"
#include "config.h"

///////////////////////////////////////////////////////////
#define _CHECK_MODULE_CREATE_(ptr, lbl) do {\
    if (NULL == ptr) {\
        printf("In ***init_network*** function, an error occurred when calling the ***aml_module_create*** interface %d\n", __LINE__); \
        goto lbl; \
    } \
}while(0)

#define _CHECK_GET_INPUT_(size1, size2, lbl) do {\
    if (size1 != size2) {\
        printf("In ***run_network*** function, the read input size does not match the actual size of the model %d\n", __LINE__); \
        goto lbl; \
    } \
}while(0)

#define _CHECK_SET_INPUT_(ret, lbl) do {\
    if (0 != ret) {\
        printf("In ***run_network*** function, an error occurred when calling the ***aml_module_input_set*** interface %d\n", __LINE__); \
        goto lbl; \
    } \
}while(0)

#define _CHECK_INFERENCE_(ptr, lbl) do {\
    if (NULL == ptr) {\
        printf("In ***run_network*** function, an error occurred when calling the ***aml_module_output_get*** interface %d\n", __LINE__); \
        goto lbl; \
    } \
}while(0)

#define _CHECK_PTR_(ptr, lbl) do {\
    if (NULL == ptr) {\
        goto lbl; \
    } \
}while(0)

/*-------------------------------------------
        Macros and Variables
-------------------------------------------*/
static int output_type = 0;
static int looptimes = 100;
static int performance_test = 0;
static int use_dma = 0;
/*-------------------------------------------
                  Functions
-------------------------------------------*/

static int _jpeg_to_bmp
    (
    FILE * inputFile,
    unsigned char* bmpData,
    unsigned int bmpWidth,
    unsigned int bmpHeight,
    unsigned int channel
    )
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    unsigned char *point = NULL;
    unsigned long width, height;
    unsigned short depth = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo,inputFile);
    jpeg_read_header(&cinfo,TRUE);

    cinfo.dct_method = JDCT_IFAST;

    if (bmpData == NULL)
    {
        return -1;
    }
    else
    {
        jpeg_start_decompress(&cinfo);

        width  = cinfo.output_width;
        height = cinfo.output_height;
        depth  = cinfo.output_components;
        if (width * height * depth != bmpWidth * bmpHeight * channel)
        {
           printf("wrong jpg file , the jpg file size should be %u %u %u\n",
               bmpWidth, bmpHeight, channel);
           return -1;
        }

        buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr)&cinfo, JPOOL_IMAGE, width*depth, 1);

        point = bmpData;

        while (cinfo.output_scanline < height)
        {
            jpeg_read_scanlines(&cinfo, buffer, 1);
            memcpy(point, *buffer, width * depth);
            point += width * depth;
        }

        jpeg_finish_decompress(&cinfo);
    }

    jpeg_destroy_decompress(&cinfo);

    return 0;
}

unsigned char *get_jpeg_rawData(const char *name,unsigned int width,unsigned int height)
{
    FILE *bmpFile;
    unsigned char *bmpData;
    unsigned int sz,w,h,c;
    int status;

    bmpFile = NULL;
    bmpData = NULL;
    w = width;
    h = height;
    c = 3;
    sz = w*h*3;

    bmpFile = fopen( name, "rb" );
    if (bmpFile == NULL)
    {
        printf("null returned\n");
        goto final;
    }

    bmpData = (unsigned char *)malloc(sz * sizeof(char));
    if (bmpData == NULL)
    {
        printf("null returned\n");
        goto final;
    }
    memset(bmpData, 0, sz * sizeof(char));

    status = _jpeg_to_bmp( bmpFile, bmpData, w, h, c);
    if (status == -1)
    {
        free(bmpData);
        fclose(bmpFile);
        return NULL;
    }

final:
    if (bmpFile) fclose(bmpFile);
    return bmpData;
}

static void save_txt(const char *savename, float *buf, int num)
{
    FILE *file = fopen(savename, "w");
    if (file != NULL) {
        for (int i = 0; i < num; i++) {
            fprintf(file, "%8.6f\n", buf[i]);
        }
        fclose(file);
        printf("Float array saved to '%s'.\n", savename);
    } else {
        printf("Unable to open '%s' for writing.", savename);
    }
}

static void save_txt_i8(const char *savename, int8_t *buf, int num)
{
    FILE *file = fopen(savename, "w");
    if (file != NULL) {
        for (int j = 0; j < num; j++) {
            fprintf(file, "%3d\n", buf[j]);
        }
        fclose(file);
        printf("int8 array saved to '%s'.\n", savename);
    } else {
        printf("Unable to open '%s' for writing.", savename);
    }
}

static void save_txt_i16(const char *savename, int16_t *buf, int num)
{
    FILE *file = fopen(savename, "w");
    if (file != NULL) {
        for (int j = 0; j < num; j++) {
            fprintf(file, "%3d\n", buf[j]);
        }

        fclose(file);
        printf("int16 array saved to '%s'.\n", savename);
    } else {
        printf("Unable to open '%s' for writing.", savename);
    }
}

static void save_txt_u8(const char *savename, uint8_t *buf, int num)
{
    FILE *file = fopen(savename, "w");
    if (file != NULL) {
        for (int j = 0; j < num; j++) {
            fprintf(file, "%3d\n", buf[j]);
        }

        fclose(file);
        printf("uint8 array saved to '%s'.\n", savename);
    } else {
        printf("Unable to open '%s' for writing.", savename);
    }

}

int stringToInteger(const char *str) {
    int result = 0;
    int sign = 1;
    int i = 0;

    if (str[0] == '-') {
        sign = -1;
        i = 1;
    }

    for (; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        } else {
            return -1;
        }
    }
    return result * sign;
}

void process_top5(float *buf, unsigned int num)
{
    int j = 0;
    unsigned int MaxClass[5]={-1,-1,-1,-1,-1};
    float fMaxProb[5]={buf[0]};

    float *pfMaxProb = fMaxProb;
    unsigned int  *pMaxClass = MaxClass,i = 0;

    for (j = 0; j < 5 && j < num; j++)
    {
        i=0;
        while ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
        {
            i++;
        }
        if (i<num) {
            *(pfMaxProb+j) = buf[i];
            *(pMaxClass+j) = i;
        }

        for (i; i<num; i++)
        {
            if ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
            {
                continue;
            }

            if (buf[i] > *(pfMaxProb+j))
            {
                *(pfMaxProb+j) = buf[i];
                *(pMaxClass+j) = i;
            }
        }
    }
    for (i=0; i<5; i++)
    {
        printf("top %d:score--%f,class--%d\n", i, fMaxProb[i], MaxClass[i]);
    }
}
void process_top5_i16(int16_t *buf,int num, int zeroPoint, float scale)
{
    int j = 0;
    unsigned int MaxClass[5]={0};
    int16_t fMaxProb[5]={0};

    int16_t *pfMaxProb = fMaxProb;
    unsigned int  *pMaxClass = MaxClass,i = 0;

    for (j = 0; j < 5 && j < num; j++)
    {
        i=0;
        while ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
        {
            i++;
        }
        if (i<num) {
            *(pfMaxProb+j) = buf[i];
            *(pMaxClass+j) = i;
        }

        for (i; i<num; i++)
        {
            if ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
            {
                continue;
            }

            if (buf[i] > *(pfMaxProb+j))
            {
                *(pfMaxProb+j) = buf[i];
                *(pMaxClass+j) = i;
            }
        }
    }
    printf("zeroPoint %d  scale %f\n",zeroPoint, scale);
    for (i = 0; i < 5; i++)
    {
        printf("top %d:score--%f,class--%d\n", i, (float)(fMaxProb[i] - zeroPoint) * scale, MaxClass[i]);
    }
}

void process_top5_i8(int8_t *buf,int num, int zeroPoint, float scale)
{
    int j = 0;
    int i = 0;
    unsigned int MaxClass[5]={0};
    int8_t fMaxProb[5]={0};

    int8_t *pfMaxProb = fMaxProb;
    unsigned int *pMaxClass = MaxClass;

    for (j = 0; j < 5 && j < num; j++)
    {
        i=0;
        while ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
        {
            i++;
        }
        if (i<num) {
            *(pfMaxProb+j) = buf[i];
            *(pMaxClass+j) = i;
        }

        for (i; i<num; i++)
        {
            if ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
            {
                continue;
            }

            if (buf[i] > *(pfMaxProb+j))
            {
                *(pfMaxProb+j) = buf[i];
                *(pMaxClass+j) = i;
            }
        }
    }
    printf("zeroPoint %d  scale %f\n",zeroPoint, scale);
    for (i = 0; i < 5; i++)
    {
        printf("top %d:score--%f,class--%d\n", i, (float)(fMaxProb[i] - zeroPoint) * scale, MaxClass[i]);
    }
}

void process_top5_u8(uint8_t *buf,int num, int zeroPoint, float scale)
{
    int j = 0;
    int i = 0;
    unsigned int MaxClass[5]={0};
    uint8_t fMaxProb[5]={0};

    uint8_t *pfMaxProb = fMaxProb;
    unsigned int *pMaxClass = MaxClass;

    for (j = 0; j < 5 && j < num; j++)
    {
        i=0;
        while ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
        {
            i++;
        }
        if (i<num) {
            *(pfMaxProb+j) = buf[i];
            *(pMaxClass+j) = i;
        }

        for (i; i<num; i++)
        {
            if ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
            {
                continue;
            }

            if (buf[i] > *(pfMaxProb+j))
            {
                *(pfMaxProb+j) = buf[i];
                *(pMaxClass+j) = i;
            }
        }
    }
    printf("zeroPoint %d  scale %f\n",zeroPoint, scale);
    for (i = 0; i < 5; i++)
    {
        printf("top %d:score--%f,class--%d\n", i, (float)(fMaxProb[i] - zeroPoint) * scale, MaxClass[i]);
    }
}

static void post_process(void *out,int out_type)
{
    nn_output *out_data = (nn_output *)out;
    data_type dt = 0;
    for (int i = 0;i<OUTPUT_COUNT; i++) {
        printf("\n--------------------------\n");
        dt = model_output_info[i].type;
        if (out_type == 0) {
            dt = FLOAT_T;
        }
        switch (dt)
        {
            case INT8_T:
                printf("output %d Int8 top5 result:\n",i);
                printf("--------------------------\n");
                process_top5_i8((int8_t*)out_data->out[i].buf, out_data->out[i].size, out_data->out[i].param[0].quant_data.affine.zeroPoint, out_data->out[i].param[0].quant_data.affine.scale);
                break;
            case UINT8_T:
                printf("output %d Uint8 top5 result:\n",i);
                printf("--------------------------\n");
                process_top5_u8((uint8_t*)out_data->out[i].buf, out_data->out[i].size, out_data->out[i].param[0].quant_data.affine.zeroPoint, out_data->out[i].param[0].quant_data.affine.scale);
                break;
            case INT16_T:
                printf("output %d Int16 top5 result:\n",i);
                printf("--------------------------\n");
                process_top5_i16((int16_t*)out_data->out[i].buf, out_data->out[i].size, out_data->out[i].param[0].quant_data.affine.zeroPoint, out_data->out[i].param[0].quant_data.affine.scale);
                break;
            case FLOAT_T:
                printf("output %d Float32 top5 result:\n",i);
                printf("--------------------------\n");
                process_top5((float*)out_data->out[i].buf,out_data->out[i].size/sizeof(float));
                break;
            default:
                break;
        }
        printf("--------------------------\n");
    }
}

void generator_output_filename(char* save_result_path,int indexnum){

    switch (model_output_info[indexnum].dim_num) {
        case 1:
            snprintf(save_result_path, 128, "output_%d_%d.txt", indexnum,model_output_info[indexnum].size[0]);
            break;
        case 2:
            snprintf(save_result_path, 128, "output_%d_%d_%d.txt", indexnum,model_output_info[indexnum].size[0],model_output_info[indexnum].size[1]);
            break;
        case 3:
            snprintf(save_result_path, 128, "output_%d_%d_%d_%d.txt", indexnum,model_output_info[indexnum].size[0],model_output_info[indexnum].size[1],model_output_info[indexnum].size[2]);
            break;
        case 4:
            snprintf(save_result_path, 128, "output_%d_%d_%d_%d_%d.txt", indexnum,model_output_info[indexnum].size[0],model_output_info[indexnum].size[1],model_output_info[indexnum].size[2],model_output_info[indexnum].size[3]);
            break;
        case 5:
            snprintf(save_result_path, 128, "output_%d_%d_%d_%d_%d_%d.txt", indexnum,model_output_info[indexnum].size[0],model_output_info[indexnum].size[1],model_output_info[indexnum].size[2],model_output_info[indexnum].size[3],model_output_info[indexnum].size[4]);
            break;
        default:
            snprintf(save_result_path, 128, "output_%d_more_than_5.txt", indexnum);
            break;
    }

}

static void save_outputtotxt(void *out,int out_type)
{
    nn_output *out_data = (nn_output *)out;
    int num = 0;
    int indexnum = 0;
    data_type dt =0;
    for (int i = 0 ; i < OUTPUT_COUNT ; i++) {
        if (out_type) {
            dt = model_output_info[i].type;
        } else {
            dt = FLOAT_T;
        }
        char save_result_path[128] = {0};
        switch (dt)
        {
            case INT8_T:
            {
                int8_t *outbuf = (int8_t*)out_data->out[i].buf;
                num = out_data->out[i].size/sizeof(int8_t);
                indexnum = i;
                generator_output_filename(save_result_path,indexnum);
                save_txt_i8(save_result_path,outbuf,num);
                break;
            }
            case UINT8_T:
            {
                uint8_t *outbuf = (uint8_t*)out_data->out[i].buf;
                num = out_data->out[i].size/sizeof(uint8_t);
                indexnum = i;
                generator_output_filename(save_result_path,indexnum);
                save_txt_u8(save_result_path,outbuf,num);
                break;
            }
            case INT16_T:
            {
                int16_t *outbuf = (int16_t*)out_data->out[i].buf;
                num = out_data->out[i].size/sizeof(int16_t);
                indexnum = i;
                generator_output_filename(save_result_path,indexnum);
                save_txt_i16(save_result_path,outbuf,num);
                break;
            }
            case FLOAT_T:
            {
                float *outbuf = (float*)out_data->out[i].buf;
                num = out_data->out[i].size/sizeof(float);
                indexnum = i;
                generator_output_filename(save_result_path,indexnum);
                save_txt(save_result_path,outbuf,num);
                break;
            }
            default:
                break;
        }
    }
}

static void *read_file(const char *file_path, int *file_size)
{
    FILE *fp = NULL;
    int size = 0;
    void *buf = NULL;
    fp = fopen(file_path, "rb");
    if (NULL == fp)
    {
        printf("open file fail!\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    buf = malloc(sizeof(unsigned char) * size);

    fread(buf, 1, size, fp);

    fclose(fp);

    *file_size = size;
    return buf;
}

static int destroy_network(void *qcontext)
{
    int ret = aml_module_destroy(qcontext);
    return ret;
}

static void* run_network(void *qcontext, int argc, char **argv)
{
    int ret = 0;
    int model_size = 0;
    int input_size = 0;
    unsigned char *rawdata = NULL;
    nn_input inData;
    nn_output *qoutdata = NULL;
    aml_output_config_t outconfig;
    aml_memory_config_t mem_config;
    aml_memory_data_t mem_data;
    //multi input module
    int input_num = INPUT_COUNT;
    if ( argc - 6 != input_num ) {
        printf("the input data num should be %d\n", input_num);
        printf("Usage: %s [adlafilepath] [input 1 filepath] [input 2 filepath] ... [input n filepath] [outputtype] [looptimes] [performance_test] [use_dma]\n", argv[0]);
        return NULL;
    }
    for (int i = 0; i < INPUT_COUNT; i++)
    {
        inData.input_index = i;
        //Set the input buf size
        int datatype_size = 1;
        if (model_input_info[i].type == 3 ) { //INT16
            datatype_size = 2;
        } else if (model_input_info[i].type == 4) {//FLOAT32
            datatype_size = 4;
        } else if (model_input_info[i].type == 5) {//INT32
            datatype_size = 4;
        } else if (model_input_info[i].type == 6) {//INT64
            datatype_size = 8;
        }
        char* input_path = (char *)argv[2 + i];
        char *last_dot = strrchr(input_path, '.');
        if (last_dot != NULL) {
            last_dot = last_dot + 1;
            for (int j = 0; last_dot[j]; j++) {
                last_dot[j] = tolower(last_dot[j]);
            }
        } else {
            printf("input format error,only support jpeg/jpg bin !\n");
            return NULL;
        }
        if (strcmp(last_dot, "jpeg") == 0 ||strcmp(last_dot, "jpg") == 0) { //only support I8/U8 Model.
            inData.input_type = RGB24_RAW_DATA;
            rawdata = get_jpeg_rawData(input_path,model_input_info[i].size[1], model_input_info[i].size[2]);
            model_size = model_input_info[i].size[1] * model_input_info[i].size[2] * model_input_info[i].size[3];
            // _CHECK_GET_INPUT_(model_size, input_size, run_final);
            inData.size = model_size;
        }
        else if (strcmp(last_dot, "bin") == 0) {
            // If bin mode is used, the data needs to be pre-processed properly
            inData.input_type = BINARY_RAW_DATA;
            input_size = 0;
            rawdata = (unsigned char *)read_file(input_path, &input_size);
            model_size = datatype_size;
            for ( int j = 0; j < model_input_info[i].dim_num; j++) {
                model_size *= model_input_info[i].size[j];
            }
            _CHECK_GET_INPUT_(model_size, input_size, run_final);
            inData.size =input_size;
        }
        if (use_dma) {
            // If dma mode is used, the data needs to be pre-processed properly
            // for (int i = 0; i < inData.size; i++)
            // {
            //     rawdata[i] = rawdata[i] - 128;
            // }
            inData.input_type = INPUT_DMA_DATA;
            mem_config.cache_type = AML_WITH_CACHE;
            mem_config.memory_type = AML_VIRTUAL_ADDR;
            mem_config.direction = AML_MEM_DIRECTION_READ_WRITE;
            mem_config.index = i;
            mem_config.mem_size = inData.size;
            aml_util_mallocBuffer(qcontext, &mem_config, &mem_data);
            memcpy(mem_data.viraddr, rawdata, inData.size);
            aml_util_swapExternalInputBuffer(qcontext, &mem_config, &mem_data);
            inData.input = (uint8_t*)mem_data.memory;
        } else {
            inData.input = rawdata;
        }
        ret = aml_module_input_set(qcontext, &inData);
        _CHECK_SET_INPUT_(ret, run_final);
        free(rawdata);
    }

    memset(&outconfig,0,sizeof(aml_output_config_t));
    if (model_output_info[0].type == FLOAT_T) //if float output,it does not need to be converted to float repeatedly. Only support bin output,
    {
        outconfig.format = AML_OUTDATA_RAW;
    }
    else{
        if (output_type) {
            outconfig.format = AML_OUTDATA_RAW;
        } else {
            outconfig.format = AML_OUTDATA_FLOAT32;
        }
    }


    outconfig.typeSize = sizeof(aml_output_config_t);
    struct timespec start, end;
    double elapsed_time;
    if (performance_test == 1) {
        printf("---------------------------------------------\n");
        printf("\tBegin Performance Test ......\n");
        printf("---------------------------------------------\n");
        outconfig.perfMode = AML_PERF_OUTPUT_SET;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int loop = 0; loop < looptimes; loop++)
            qoutdata = (nn_output*)aml_module_output_get(qcontext, outconfig);
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed_time = ((end.tv_sec - start.tv_sec) *1000 + (end.tv_nsec - start.tv_nsec) / 1.0e6)/looptimes;
        printf("\n\t--------------------------\n");
        printf("\tOutput Set buf Performance\n");
        printf("\t--------------------------\n");
        printf("\tSet buf Times = %.2f ms\n",elapsed_time);
        printf("\t--------------------------\n");
        outconfig.perfMode = AML_PERF_INFERENCE;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int loop = 0; loop < looptimes; loop++)
            qoutdata = (nn_output*)aml_module_output_get(qcontext, outconfig);
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed_time = ((end.tv_sec - start.tv_sec) *1000 + (end.tv_nsec - start.tv_nsec) / 1.0e6)/looptimes;
        printf("\n\t--------------------------\n");
        printf("\tInference Performance\n");
        printf("\t--------------------------\n");
        printf("\tInference Times = %.2f ms\n",elapsed_time);
        printf("\tFPS = %.2f \n",1000.0/elapsed_time);
        printf("\t--------------------------\n");
        outconfig.perfMode = AML_PERF_OUTPUT_GET;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int loop = 0; loop < looptimes; loop++)
            qoutdata = (nn_output*)aml_module_output_get(qcontext, outconfig);
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed_time = ((end.tv_sec - start.tv_sec) *1000 + (end.tv_nsec - start.tv_nsec) / 1.0e6)/looptimes;
        printf("\n\t--------------------------\n");
        printf("\tOutput type to float32 Performance\n");
        printf("\t--------------------------\n");
        printf("\tOutput type to float32 Times= %.2f ms\n",elapsed_time);
        printf("\t--------------------------\n");
        printf("\n---------------------------------------------\n");
        printf("\tEnd Performance Test ......\n");
        printf("---------------------------------------------\n");
    }
    outconfig.perfMode = AML_NO_PERF;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int loop = 0; loop < looptimes; loop++)
        qoutdata = (nn_output*)aml_module_output_get(qcontext, outconfig);
    clock_gettime(CLOCK_MONOTONIC, &end);
    _CHECK_INFERENCE_(qoutdata, run_final);
    elapsed_time = (end.tv_sec - start.tv_sec) *1000 + (end.tv_nsec - start.tv_nsec) / 1.0e6;
    printf("\n--------------------------\n");
    printf("Inference Performance looptimes = %d\n",looptimes);
    printf("Total times %.2f ms\n",elapsed_time);
    elapsed_time /= looptimes;
    printf("--------------------------\n");
    printf("Inference Times = %.2f ms\n",elapsed_time);
    printf("FPS = %.2f \n",1000.0/elapsed_time);
    printf("--------------------------\n");

run_final:
    if (use_dma)
        aml_util_freeBuffer(qcontext, &mem_config, &mem_data);
    return (void *)qoutdata;
}

static void* init_network(int argc,char **argv)
{
    int size = 0;
    void *qcontext = NULL;
    aml_config config;
    memset(&config, 0, sizeof(aml_config));

    config.nbgType = NN_ADLA_FILE;
    config.path = (const char *)argv[1];
    config.modelType = ADLA_LOADABLE;
    config.typeSize = sizeof(aml_config);

    qcontext = aml_module_create(&config);
    _CHECK_MODULE_CREATE_(qcontext, init_final);

init_final:
    return qcontext;
}

int main(int argc,char **argv)
{
    if (argc < 7)
    {
        printf("Usage: %s [adlafilepath] [input 1 filepath] [input 2 filepath] ... [input n filepath] [outputtype] [loop_times] [performance_test] [use_dma]\n", argv[0]);
        return -1;
    }
    output_type = (int)(*(char*)argv[argc-4] - '0');
    performance_test = (int)(*(char*)argv[argc-2] - '0');
    use_dma = (int)(*(char*)argv[argc-1] - '0');
    looptimes = stringToInteger((char*)argv[argc-3]);
    if (looptimes == -1) {
        printf("looptimes is an invalid value\n");
        return -1;
    }
    int ret = 0;
    void *context = NULL;
    void *outdata = NULL;

    context = init_network(argc, argv);
    _CHECK_PTR_(context, final);

    outdata = run_network(context,argc,argv);
    _CHECK_PTR_(outdata, final);

    post_process(outdata,output_type);
    save_outputtotxt(outdata,output_type);
final:
    ret = destroy_network(context);
    fflush(stdout);
    fflush(stderr);
    return ret;
}
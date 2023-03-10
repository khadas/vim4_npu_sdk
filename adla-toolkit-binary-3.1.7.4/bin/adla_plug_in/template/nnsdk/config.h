#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef enum {
    INT8_T=1,
    UINT8_T,
    INT16_T,
    FLOAT_T,
    INT32_T,
    INT64_T
} data_type;

typedef struct Info
{
    int dim_num;
    data_type type;
    int size[6];
} _tensor_info;

#define ADLA_NAME name

#define INPUT_COUNT number
_tensor_info model_input_info[INPUT_COUNT] = info_message;

#define OUTPUT_COUNT number
_tensor_info model_output_info[OUTPUT_COUNT] = info_message;

#ifdef __cplusplus
}
#endif

#endif
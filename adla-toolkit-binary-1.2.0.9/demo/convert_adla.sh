#!/bin/bash
  
ACUITY_PATH=../bin/
#ACUITY_PATH=../python/tvm/
adla_convert=${ACUITY_PATH}adla_convert


if [ ! -e "$adla_convert" ]; then
    adla_convert=${ACUITY_PATH}adla_convert.py
fi

$adla_convert --model-type caffe \
        --model ./model_source/caffe_model/resnet-18.prototxt  \
        --weights ./model_source/caffe_model/resnet-18.caffemodel \
        --quantize-dtype int8 --outdir caffe_output \
        --source-file dataset.txt --channel-mean-value "128,128,128,128" \
        --target-platform PRODUCT_PID0XA001
        #--batch-size 4 (default 1)

$adla_convert --model-type darknet \
        --model ./model_source/darknet_model/vgg-conv.cfg  \
        --weights ./model_source/darknet_model/vgg-conv.weights \
        --quantize-dtype uint8 --outdir darknet_output \
        --mean 127.5 --std-dev 127.5 \
        --default-ranges-min 0  --default-ranges-max 1 \
        --batch-size 4

$adla_convert --model-type onnx \
        --model ./model_source/onnx_model/model_UnetBased_0620v8-ep44-seg3ch.onnx \
        --inputs "0 1 2" \
        --input-shapes  "3,288,288#3,288,288#3,288,288"  \
        --dtypes "float32#float32#float32" \
        --quantize-dtype int8 --outdir onnx_output  \
        --batch-size 4 --target-platform PRODUCT_PID0XA001

$adla_convert --model-type pytorch \
        --model ./model_source/pytorch_model/squeezenet1_0.pt \
        --inputs "input" --input-shape  "3,224,224" \
        --quantize-dtype int8 --outdir pytorch_output \
        --channel-mean-value "0,0,0,256" \
        --source-file dataset.txt --batch-size 2

$adla_convert --model-type mxnet \
        --model ./model_source/mxnet_model/mobilenet0.25-symbol.json \
        --weights ./model_source/mxnet_model/mobilenet0.25-0000.params \
        --inputs "data" --input-shape "3,224,224" \
        --quantize-dtype int16 --outdir mxnet_output \
        --channel-mean-value "0,0,0,256" \
        --source-file dataset.txt \
        --target-platform PRODUCT_PID0XA001
        #--batch-size 4 (default 1)

$adla_convert --model-type tensorflow \
        --model ./model_source/tensorflow_model/resnet50.pb \
        --inputs input --input-shapes 224,224,3 \
        --outputs resnet_v1_50/block4/unit_3/bottleneck_v1/Relu \
        --quantize-dtype int8 --outdir tensorflow_output \
        --channel-mean-value "0,0,0,256" \
        --source-file dataset.txt \
        --target-platform PRODUCT_PID0XA001

$adla_convert --model-type tflite \
        --model ./model_source/tflite_model/mobilenetv2.tflite \
        --inputs input --input-shapes 224,224,3 \
        --quantize-dtype int8 --outdir tflite_output \
        --channel-mean-value "0,0,0,256" \
        --source-file dataset.txt \
        --target-platform PRODUCT_PID0XA001

$adla_convert --model-type paddle \
        --model ./model_source/paddle_model/mobilenet_v1 \
        --inputs x --input-shapes 3,224,224 \
        --quantize-dtype int8 --outdir paddle_output \
        --channel-mean-value "0,0,0,256" \
        --source-file dataset.txt \
        --target-platform PRODUCT_PID0XA001

$adla_convert --model-type quantized_tflite \
        --model ./model_source/quantized_tflite_model/resnet18_int8.tflite \
		--outdir quantized_tflite_output \
        --target-platform PRODUCT_PID0XA001

$adla_convert --model-type keras \
        --model ./model_source/keras_model/resnet34-v2-7.h5 \
		--inputs input_1 --input-shapes 224,224,3 \
		--source-file dataset.txt --channel-mean-value 127.5,127.5,127.5,127.5 \
        --target-platform PRODUCT_PID0XA001  --outdir keras_output

# yolov7_tiny
$adla_convert --model-type onnx \
        --model ./model_source/yolov7_tiny/yolov7_tiny.onnx \
        --inputs "images" \
        --input-shapes  "3,640,640"  \
        --dtypes "float32" \
        --quantize-dtype int8 --outdir onnx_output  \
        --channel-mean-value "0,0,0,255"  \
        --source-file dataset.txt  \
        --batch-size 1 --target-platform PRODUCT_PID0XA003

# yolov8n
$adla_convert --model-type onnx \
        --model ./model_source/yolov8n/yolov8n.onnx \
        --inputs "images" \
        --input-shapes  "3,640,640"  \
        --dtypes "float32" \
        --quantize-dtype int8 --outdir onnx_output  \
        --channel-mean-value "0,0,0,255"  \
        --source-file dataset.txt  \
        --batch-size 1 --target-platform PRODUCT_PID0XA003

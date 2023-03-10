import sys
import os
import numpy as np
import tensorflow as tf
from scipy import spatial

class aml_nn_model:
    def __init__(self):
        '''Model Conversion Toolpath'''
        self.adla_tool_path="/xxxx/adla-toolkit-binary-1.6.10.3"

        '''Quantify model information '''
        self.mdoel_type='tflite'
        self.model_path='./mnv1_fuse3_256p_w256xh144.tflite'
        self.inputs ="input_1"
        self.input_shapes='144,256,3'
        self.quant_type='int8' #'int16'
        self.source_file='/xxxx/pose_detection/FLIC/dataset.txt'
        self.channel_mean_value='123.68,116.779,103.939,127.5'
        self.itreation=200
        self.outdir='./'
        self.disable_per_channel=False
        '''Quantify model information'''
        '''The input and output of the original model'''
        self.input_npy_path=['./array_test_0.npy']
        self.output_npy_path=['./output_0.npy', './output_1.npy']
        '''The input and output of the original model'''
        '''Quantization parameter information'''
        self.quantize_algos=["min-max","percentile","hist_percentile","hist_asymm_percentile"]
        self.percentiles = [0.99991,0.99992,0.99993,0.99994,0.99995,0.99996,0.99997,0.99998,0.99999] #for percentiles algo
        self.num_histogram_bins = [2048,4096,8192,12288,16384]
        self.iterations = [100,10]
        self.weights_optimize_algos = ['cos-dis'] #for wegiths mle algo
        self.weights_quantize_algos=['min-max','mle']
        self.weights_thres_sizes = [2048]
        self.weights_neg_scales = [0.9991,0.9992,0.9993,0.9994,0.9995,0.9996,0.9997,0.9998,0.9999,0.99999]
        '''Quantization parameter information'''
        os.environ['ADLA_EXPORT_MIDDLE_TO_TFLITE']="True"
        self.adla_convert_path=self.adla_tool_path+"/bin/adla_convert"
        self.middle_tflite = "{}_{}.tflite".format(self.model_path.split("/")[-1].split(".")[0],self.quant_type)
        self.cmd_line_base = "{} --model-type {} --model {} --inputs {} --input-shapes {} \
            --quantize-dtype {} --disable-per-channel {} --source-file {} \
            --channel-mean-value {} --outdir {} --inference-input-type 'float32' \
            --inference-output-type 'float32' ".format(self.adla_convert_path, self.mdoel_type, self.model_path, self.inputs,\
            self.input_shapes, self.quant_type,self.disable_per_channel, self.source_file, self.channel_mean_value, self.outdir)
        self.result="test_result.txt"
        self.output=[]
        self.euclidean_dis=[]
        self.cos_sim=[]

    def run_tflite(self):
        print("Load Model:{}".format(self.middle_tflite))
        interpreter = tf.lite.Interpreter(model_path=self.middle_tflite,num_threads=8)
        interpreter.allocate_tensors()

        for i in range(len(interpreter.get_input_details())):
            index = interpreter.get_input_details()[i]['index']
            name = interpreter.get_input_details()[i]["name"]
            shape = interpreter.get_input_details()[i]["shape"]
            dtype = interpreter.get_input_details()[i]["dtype"]
            quantization = interpreter.get_input_details()[i]["quantization"]
            print("\nInput[{}]: index:{},name:{} shape:{}, dtype:{}, quantization:{}".format(i, index, name, shape, dtype, quantization))

            data = np.load(self.input_npy_path[i])
            #data=np.expand_dims(data, axis=0)
            interpreter.set_tensor(index, data)

        interpreter.invoke()

        for i in range(len(interpreter.get_output_details())):
            index = interpreter.get_output_details()[i]['index']
            name = interpreter.get_output_details()[i]["name"]
            shape = interpreter.get_output_details()[i]["shape"]
            dtype = interpreter.get_output_details()[i]["dtype"]
            quantization = interpreter.get_output_details()[i]["quantization"]
            print("\nOutput[{}]: index:{}, shape:{}, dtype:{}, quantization:{}".format(i, index, name, shape, dtype, quantization))
            out = interpreter.get_tensor(index)
            self.output.append(out)
        os.system("rm -rf {} ".format(self.middle_tflite))

    def compute_euclidean_dis_and_cos(self):
        if len(self.output) != len(self.output_npy_path):
            print("Error: The number of outputs does not match the number of outputs provided")
            exit()

        for i in range(len(self.output)):
            temp1=self.output[i]
            temp2=np.load(self.output_npy_path[i])

            squart_dis = tf.square(tf.subtract(temp1,temp2))
            dis_sum = tf.reduce_sum(squart_dis)
            euclidean = tf.sqrt(dis_sum)

            cos_sim_temp = 1 - spatial.distance.cosine(temp1.reshape(1, -1), temp2.reshape(1, -1))

            self.euclidean_dis.append(float(euclidean.numpy()))
            self.cos_sim.append(cos_sim_temp)
            print("\n Result cos_sim:{},euclidean_dis:{}\n".format(self.cos_sim[-1], self.euclidean_dis[-1]))

    def record_result(self,cmd_line):
        with open(self.result, 'a') as record:
             record.write(cmd_line+" \n")
             for i in range(len(self.output)):
                 record.write("output[{}]:cosine similarity:{:-<8f},Euclidean distance:{:-<8f} \n".format(i, self.cos_sim[i], self.euclidean_dis[i]))
             record.write("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ \n")
        self.euclidean_dis=[]
        self.cos_sim=[]
        self.output=[]

    def test_result(self,cmd_line):
        self.run_tflite()

        self.compute_euclidean_dis_and_cos()

        self.record_result(cmd_line)

def main(argv):
    aml_model=aml_nn_model()
    for quantize_algo in aml_model.quantize_algos:
        if quantize_algo=="hist_percentile" or quantize_algo=="hist_asymm_percentile":
            for percentile in aml_model.percentiles:
                for num_histogram_bin in aml_model.num_histogram_bins:
                    for iterations in aml_model.iterations:
                        for weights_quantize_algo in aml_model.weights_quantize_algos:
                            if weights_quantize_algo=="mle":
                                for weights_thres_sizes in aml_model.weights_thres_sizes:
                                    for weights_neg_scales in aml_model.weights_neg_scales:
                                        for weights_optimize_algos in aml_model.weights_optimize_algos:
                                            cmd_line=aml_model.cmd_line_base + " --weights-quantize-algo mle \
                                            --weights-threshold-size {} --iterations {} \
                                            --quantize-algo {} --percentile {} --divergence-nbins {}\
                                            --weights-neg-scale {} --weights-optimize-algo {}".format(weights_thres_sizes,iterations,
                                                                                                        quantize_algo,percentile,num_histogram_bin,
                                                                                                        weights_neg_scales,weights_optimize_algos)
                                            print(cmd_line)
                                            ret=os.system(cmd_line)
                                            if ret:
                                                print("Model conversion command failed to execute,\nCommand：{}".format(cmd_line))
                                                exit()
                                            aml_model.test_result(cmd_line)
                            else:
                                cmd_line=aml_model.cmd_line_base + " --quantize-algo {} --percentile {} --divergence-nbins {} --iterations {}".format(quantize_algo,percentile,num_histogram_bin,iterations)
                                print(cmd_line)
                                ret=os.system(cmd_line)
                                if ret:
                                    print("Model conversion command failed to execute,\nCommand：{}".format(cmd_line))
                                    exit()
                                aml_model.test_result(cmd_line)
        elif quantize_algo=="percentile":
            for percentile in aml_model.percentiles:
                for weights_quantize_algo in aml_model.weights_quantize_algos:
                    for iterations in aml_model.iterations:
                        if weights_quantize_algo=="mle":
                            for weights_thres_sizes in aml_model.weights_thres_sizes:
                                for weights_neg_scales in aml_model.weights_neg_scales:
                                    for weights_optimize_algos in aml_model.weights_optimize_algos:
                                        cmd_line=aml_model.cmd_line_base + " --weights-quantize-algo mle \
                                        --weights-threshold-size {} --iterations {} \
                                        --quantize-algo {} --percentile {} \
                                        --weights-neg-scale {} --weights-optimize-algo {}".format(weights_thres_sizes,iterations,
                                                                                                quantize_algo,percentile,
                                                                                                weights_neg_scales,weights_optimize_algos)
                                        print(cmd_line)
                                        ret=os.system(cmd_line)
                                        if ret:
                                            print("Model conversion command failed to execute,\nCommand：{}".format(cmd_line))
                                            exit()
                                        aml_model.test_result(cmd_line)
                        else:
                            cmd_line=aml_model.cmd_line_base + " --quantize-algo {} --percentile {} --iterations {}".format(quantize_algo,percentile,iterations)
                            print(cmd_line)
                            ret=os.system(cmd_line)
                            if ret:
                                print("Model conversion command failed to execute,\nCommand：{}".format(cmd_line))
                                exit()
                            aml_model.test_result(cmd_line)
        else:
            for iterations in aml_model.iterations:
                for weights_quantize_algo in aml_model.weights_quantize_algos:
                        if weights_quantize_algo=="mle":
                            for weights_thres_sizes in aml_model.weights_thres_sizes:
                                for weights_neg_scales in aml_model.weights_neg_scales:
                                    for weights_optimize_algos in aml_model.weights_optimize_algos:
                                        cmd_line=aml_model.cmd_line_base + " --weights-quantize-algo mle \
                                        --weights-threshold-size {} --iterations {} \
                                        --weights-neg-scale {} --weights-optimize-algo {}".format(weights_thres_sizes,iterations,
                                                                                                weights_neg_scales,weights_optimize_algos)
                                        print(cmd_line)
                                        ret=os.system(cmd_line)
                                        if ret:
                                            print("Model conversion command failed to execute,\nCommand：{}".format(cmd_line))
                                            exit()
                                        aml_model.test_result(cmd_line)
                        else:
                            for iterations in aml_model.iterations:
                                cmd_line=aml_model.cmd_line_base + "--iterations {}".format(iterations)
                                print(cmd_line)
                                ret=os.system(cmd_line)
                                if ret:
                                    print("Model conversion command failed to execute,\nCommand：{}".format(cmd_line))
                                    exit()
                                aml_model.test_result(cmd_line)
if __name__ == '__main__':
    main(sys.argv)

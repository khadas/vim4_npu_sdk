import sys
import numpy as np

def generate_data(argv):
    shape = [int(d) for d in argv[1].split(',')]
    dtype_map = {'np.int64': np.int64, 'np.float32': np.float32, 'np.float16': np.float16, 'np.int32': np.int32, 'np.int16': np.int16, 'np.int8': np.int8, 'np.uint8': np.uint8}
    file_type=argv[2]
    target_dtype = argv[3]
    if len(argv) == 6:
        low = int(argv[4])
        high = int(argv[5])
    else:
        low = 0
        high = 255


    if target_dtype == 'np.int64' or target_dtype == 'np.int32' or target_dtype == 'np.int16' or target_dtype == 'np.int8' or target_dtype == 'np.uint8':
        data = np.random.randint(low, high, size = (shape)).astype(dtype_map[target_dtype])
    else:
        if (len(shape) == 4):
            data = np.random.rand(shape[0], shape[1], shape[2], shape[3]).astype(dtype_map[target_dtype])
        elif (len(shape) == 3):
            data = np.random.rand(shape[0], shape[1], shape[2]).astype(dtype_map[target_dtype])
        elif (len(shape) == 2):
            data = np.random.rand(shape[0], shape[1]).astype(dtype_map[target_dtype])
        elif (len(shape) == 1):
            data = np.random.rand(shape[0],).astype(dtype_map[target_dtype])

    if file_type=='npy':
        dump_name="input_" + argv[1].replace(',', '_') + ".npy"
        np.save(dump_name, data)
        print("\ngenerate_npy_numpy inputshape:{}, dump_name:{} \n".format(shape, dump_name))
    elif file_type=='txt':
        dump_name="input_" + argv[1].replace(',', '_') + ".txt"
        data=data.reshape((-1))
        np.savetxt(dump_name, data)
        print("\ngenerate_numpy_txt inputshape:{}, dump_name:{} \n".format(shape, dump_name))
    elif file_type=='bin':
        dump_name="input_" + argv[1].replace(',', '_') + ".bin"
        data.tofile(dump_name)
        print("\ngenerate_numpy_binfile inputshape:{}, dump_name:{} \n".format(shape, dump_name))
    else:
        pass

#python3 input_npy_txt_bin_generate.py 1,3,224,224 bin np.int64 0 255
#python3 input_npy_txt_bin_generate.py 1,3,224,224 npy np.int64 0 255
#python3 input_npy_txt_bin_generate.py 1,3,224,224 txt np.int64 0 255
def main(argv):
    if len(argv) < 4:
        print("Error: input parameter error")
        print('Usage: python3 {} shape file_type'.format(argv[0]))
        print('Case1: python3 {} 1,3,224,224 bin np.float32 0 255'.format(argv[0]))
        print('Case2: python3 {} 1,3,224,224 txt np.float32 0 255'.format(argv[0]))
        print('Case3: python3 {} 1,3,224,224 npy np.float32 0 255'.format(argv[0]))
        exit()

    generate_data(argv)


if __name__ == '__main__':
    main(sys.argv)

cmake_minimum_required(VERSION 3.5.1)
SET(CMAKE_SYSTEM_NAME Linux)
PROJECT(AML_NNSDK)

SET(LINUX_32 linux)
SET(ADLA_TOOL_PATH  ../../../../) #/home/adla-toolkit-binary-1.6.10.3
SET(NNSDK_PATH  ${ADLA_TOOL_PATH}/bin/adla_plug_in/template/nnsdk)

include_directories(${NNSDK_PATH}/include .)

if(DEFINED LINUX_32)
	SET(CMAKE_C_COMPILER  arm-none-linux-gnueabihf-gcc)
	SET(CMAKE_CXX_COMPILER  arm-none-linux-gnueabihf-g++)
	SET(LINK_DIR ${NNSDK_PATH}/libraries/linux/lib32  ./)

	link_directories(${LINK_DIR})
	aux_source_directory(. SRC_LIST)
	link_libraries(nnsdk jpeg)

	ADD_EXECUTABLE(adla_nnsdk_test_32 ${SRC_LIST})
	target_link_libraries(adla_nnsdk_test_32 -lpthread -lm -ldl)

elseif(DEFINED LINUX_64)
	SET(CMAKE_C_COMPILER  aarch64-none-linux-gnu-gcc)
	SET(CMAKE_CXX_COMPILER  aarch64-none-linux-gnu-g++)
	SET(LINK_DIR ${NNSDK_PATH}/libraries/linux/lib64  ./)

	link_directories(${LINK_DIR})
	aux_source_directory(. SRC_LIST)
	link_libraries(nnsdk jpeg)

	ADD_EXECUTABLE(adla_nnsdk_test_64 ${SRC_LIST})
	target_link_libraries(adla_nnsdk_test_64 -lpthread -lm -ldl)

elseif(DEFINED YOCTO_64)
	SET(CMAKE_C_COMPILER  aarch64-none-linux-gnu-gcc)
	SET(CMAKE_CXX_COMPILER  aarch64-none-linux-gnu-g++)
	SET(LINK_DIR ${NNSDK_PATH}/libraries/yocto/lib64  ./)

	link_directories(${LINK_DIR})
	aux_source_directory(. SRC_LIST)
	link_libraries(nnsdk jpeg)

	add_executable(nnsdk_yocto_64 ${SRC_LIST})
	target_link_libraries(nnsdk_yocto_64 -lpthread -lm -ldl)

endif()

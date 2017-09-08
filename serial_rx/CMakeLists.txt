PROJECT(serial_rx)
cmake_minimum_required(VERSION 3.5)

#add_compile_options(-std=c++11)
add_compile_options(-std=c11)

#set include and libs
SET(MY_SYS_ROOT "/home/syp/bin/0.arm_gcc_env/arm-cortexa9-linux-gnueabihf/sys-root")
SET(MY_GCC_ROOT "/home/syp/bin/0.arm_gcc_env/")
SET(CMAKE_FIND_ROOT_PATH  ${MY_SYS_ROOT})
SET(CMAKE_C_COMPILER      ${MY_GCC_ROOT}/bin/arm-cortexa9-linux-gnueabihf-cc)
SET(CMAKE_CXX_COMPILER    ${MY_GCC_ROOT}/bin/arm-cortexa9-linux-gnueabihf-c++)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET (SYS_INC ${MY_SYS_ROOT}/include ${MY_SYS_ROOT}/usr/include ${MY_GCC_ROOT}/include)
SET (SYS_LIB ${MY_SYS_ROOT}/lib ${MY_GCC_ROOT}/lib)

INCLUDE_DIRECTORIES(${SYS_INC} . src)
LINK_DIRECTORIES(${SYS_LIB} )

AUX_SOURCE_DIRECTORY( ./src SRC_LIST)

ADD_EXECUTABLE(serial_rx ${SRC_LIST} )
TARGET_LINK_LIBRARIES(serial_rx pthread)


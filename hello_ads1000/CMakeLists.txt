PROJECT (HELLO_ADS)
#set sysroot path
cmake_minimum_required(VERSION 3.5)

add_compile_options(-std=c++11)
#add_compile_options(-std=c11)

#set include and libs
SET(MY_ARM_ROOT "/home/syp/bin/0.arm_gcc_env")
SET(CMAKE_FIND_ROOT_PATH  ${MY_ARM_ROOT})
SET(CMAKE_C_COMPILER      ${MY_ARM_ROOT}/bin/arm-cortexa9-linux-gnueabihf-cc)
SET(CMAKE_CXX_COMPILER    ${MY_ARM_ROOT}/bin/arm-cortexa9-linux-gnueabihf-c++)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET (SYS_INC ${MY_SYS_ROOT}/include ${MY_SYS_ROOT}/usr/include ${MY_GCC_ROOT}/include)
SET (SYS_LIB ${MY_SYS_ROOT}/lib ${MY_GCC_ROOT}/lib)

#set source code dir
#############################################################
######WARINING###############################################
#BIN NAME MUST BE THE SAME AS YOUR build.sh and .bb file!!!!#
#############################################################
SET (BIN_TARGET "hello_ads")
SET (SRC_LIST "src/main.cpp"
              "src/i2c.c")

#set compile dir
ADD_EXECUTABLE(${BIN_TARGET} ${SRC_LIST} )

#set include and libs
#mraa
INCLUDE_DIRECTORIES(${MRAA_INC} ${SYS_INC} "src")
LINK_DIRECTORIES(${MRAA_LIB} ${SYS_LIB})

#compile with libs
TARGET_LINK_LIBRARIES(${BIN_TARGET})

PROJECT(serial_tx)
cmake_minimum_required(VERSION 3.5)

#add_compile_options(-std=c++11)
add_compile_options(-std=c11)

#set include and libs
SET(MY_ARM_ROOT "../../0.arm_gcc_env")
SET(CMAKE_FIND_ROOT_PATH  ${MY_ARM_ROOT})
SET(CMAKE_C_COMPILER      ${MY_ARM_ROOT}/bin/arm-cortexa9-linux-gnueabihf-cc)
SET(CMAKE_CXX_COMPILER    ${MY_ARM_ROOT}/bin/arm-cortexa9-linux-gnueabihf-c++)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET (SYS_INC ${MY_ARM_ROOT}/include)
SET (SYS_LIB ${MY_ARM_ROOT}/lib )

INCLUDE_DIRECTORIES(${SYS_INC} . src)
LINK_DIRECTORIES(${SYS_LIB} )

AUX_SOURCE_DIRECTORY( ./src SRC_LIST)

ADD_EXECUTABLE(serial_tx ${SRC_LIST} )
TARGET_LINK_LIBRARIES(serial_tx pthread)


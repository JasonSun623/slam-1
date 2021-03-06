set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(GCC_PREFIX arm-xilinx-linux-gnueabi-)

find_program(CMAKE_C_COMPILER NAMES ${GCC_PREFIX}gcc)
find_program(CMAKE_CXX_COMPILER NAMES ${GCC_PREFIX}g++)
find_program(CMAKE_LINKER NAMES ${GCC_PREFIX}ld)
find_program(CMAKE_AR NAMES ${GCC_PREFIX}ar)

SET(CMAKE_FIND_ROOT_PATH ${ROOTFS})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_PREFIX_PATH ${CMAKE_FIND_ROOT_PATH})
set(CMAKE_LIBRARY_PATH ${CMAKE_FIND_ROOT_PATH}/lib:${CMAKE_FIND_ROOT_PATH}/usr/lib)

include_directories(${CMAKE_FIND_ROOT_PATH}/include)

set(TARGET "zynq")
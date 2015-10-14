# Based on: https://developer.mbed.org/forum/team-63-Bluetooth-Low-Energy-community/topic/5257/
cmake_minimum_required(VERSION 2.8)

project(PROJECT)

set(ARMGCC_PREFIX
  ${PROJECT_SOURCE_DIR}/../../tools/arm/gcc-arm-none-eabi-4_9-2015q2)
set(MBED_SRC_PATH
  ${PROJECT_SOURCE_DIR}/../../lib/mbed/libraries/mbed)
set(BLE_API_SRC_PATH
  ${PROJECT_SOURCE_DIR}/../../lib/ble)
set(NRF51822_SRC_PATH
  ${PROJECT_SOURCE_DIR}/../../lib/ble-nrf51822/source)

# Source: http://stackoverflow.com/questions/7046956/populating-srcs-from-cmakelists-txt-in-subdirectories
macro(add_sources)
  file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
  foreach(_src ${ARGN})
    if(_relPath)
      list(APPEND SRCS "${_relPath}/${_src}")
    else()
      list(APPEND SRCS "${_src}")
    endif()
  endforeach()
  if(_relPath)
    set(SRCS ${SRCS} PARENT_SCOPE)
  endif()
endmacro()

# Source: http://www.vtk.org/Wiki/CMake/Examples
macro (header_directories return_list dir)
  file(GLOB_RECURSE new_list ${dir}/*.h)
  set(dir_list "")
  foreach(file_path ${new_list})
    get_filename_component(dir_path ${file_path} PATH)
    set(dir_list ${dir_list} ${dir_path})
  endforeach()
  list(REMOVE_DUPLICATES dir_list)
  set(${return_list} ${dir_list})
endmacro()

set(TOOLCHAIN armgcc)

set(CMAKE_CXX_COMPILER ${ARMGCC_PREFIX}/bin/arm-none-eabi-g++)
set(CMAKE_C_COMPILER   ${ARMGCC_PREFIX}/bin/arm-none-eabi-gcc)
set(SIZE_COMMAND       ${ARMGCC_PREFIX}/bin/arm-none-eabi-size)
set(OBJCOPY_COMMAND    ${ARMGCC_PREFIX}/bin/arm-none-eabi-objcopy)

set(MAIN_TARGET        ${PROJECT_NAME}.elf)
enable_language(ASM)

header_directories(NORDIC_SDK_INCLUDES
  ${MBED_SRC_PATH}/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/Lib/nordic_sdk)
header_directories(NRF51822_INCLUDES ${NRF51822_SRC_PATH})
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${MBED_SRC_PATH}/
  ${MBED_SRC_PATH}/api
  ${MBED_SRC_PATH}/common
  ${MBED_SRC_PATH}/hal
  ${MBED_SRC_PATH}/targets
  ${MBED_SRC_PATH}/targets/cmsis
  ${MBED_SRC_PATH}/targets/cmsis/TARGET_NORDIC
  ${MBED_SRC_PATH}/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822
  ${MBED_SRC_PATH}/targets/hal
  ${MBED_SRC_PATH}/targets/hal/TARGET_NORDIC
  ${MBED_SRC_PATH}/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822
  ${MBED_SRC_PATH}/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/Lib
  ${MBED_SRC_PATH}/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/TARGET_RBLAB_BLENANO
  ${MBED_SRC_PATH}/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822/TOOLCHAIN_GCC_ARM
  ${BLE_API_SRC_PATH}
  ${NORDIC_SDK_INCLUDES}
  ${NRF51822_INCLUDES})

add_definitions(
  -O2
  -DTARGET_NRF51822
  -DTARGET_M0
  -DTARGET_NORDIC
  -D__CORTEX_M0
  -DARM_MATH_CM0
  -D__MBED__=1
  -DNRF51
  -mcpu=cortex-m0
  -mthumb
  -Wall
  -Wextra
  -Wno-unused-parameter
  -Wno-missing-field-initializers
  -Wno-error=switch
  -Wno-switch
  -Wa,-adhlns=$@.lst
  -fmessage-length=0
  -fno-builtin
  -ffunction-sections
  -fdata-sections
  -fno-delete-null-pointer-checks
  -fomit-frame-pointer
  -fno-common
  -funsigned-bitfields
  -DTOOLCHAIN_GCC_ARM
  -DTOOLCHAIN_GCC
  -DTARGET_NRF51822_MKIT
  -DTARGET_MCU_NRF51822
  -DTARGET_MCU_NORDIC_32K)

set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -std=gnu++98 -fno-rtti -fno-exceptions -fno-threadsafe-statics")
set(CMAKE_CXX_LINK_FLAGS "")
set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} -std=gnu99 -Wno-pointer-sign -Wno-pointer-to-int-cast")
set(CMAKE_ASM_FLAGS
  "${COMMON_COMPILE_FLAGS} -x assembler-with-cpp")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS
  -T${MBED_SRC_PATH}/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822/TOOLCHAIN_GCC_ARM/TARGET_MCU_NORDIC_32K/NRF51822.ld)
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS
  "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -Wl,--gc-sections -Wl,--wrap,main -Wl,-Map=${PROJECT_NAME}.map -mcpu=cortex-m0 -mthumb --specs=nano.specs -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys")

file(GLOB MBED_SRC_SOURCES
  ${MBED_SRC_PATH}/common/*.c
  ${MBED_SRC_PATH}/common/*.cpp
  ${MBED_SRC_PATH}/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822/*.c
  ${MBED_SRC_PATH}/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/*.c
  ${MBED_SRC_PATH}/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/Lib/app_common/*.c)
file(GLOB_RECURSE BLE_API_SOURCES
  ${BLE_API_SRC_PATH}/source/*.cpp
  ${BLE_API_SRC_PATH}/source/services/*.cpp)
file(GLOB_RECURSE NRF51822_SOURCES
  ${NRF51822_SRC_PATH}/*.c
  ${NRF51822_SRC_PATH}/*.cpp)
add_sources(
  main.cpp
  ${MBED_SRC_SOURCES}
  ${BLE_API_SOURCES}
  ${NRF51822_SOURCES}
  ${MBED_SRC_PATH}/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822/TOOLCHAIN_GCC_ARM/startup_NRF51822.s)

add_executable(${MAIN_TARGET} ${SRCS})

add_custom_command(
  TARGET ${MAIN_TARGET}
  COMMAND ${SIZE_COMMAND} ${MAIN_TARGET}
  COMMAND ${OBJCOPY_COMMAND} -O ihex ${MAIN_TARGET} ${PROJECT_NAME}.hex        COMMAND srec_cat ${MBED_SRC_PATH}/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/Lib/s130_nrf51822_1_0_0/s130_nrf51_1.0.0_softdevice.hex -intel ${PROJECT_NAME}.hex -intel -o combined.hex -intel --line-length=44)
add_custom_target(upload)
add_custom_command(TARGET upload COMMAND cp combined.hex /Volumes/MBED)


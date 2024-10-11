cmake_minimum_required(VERSION 3.13)

set (PROJECT_NAME MultiIO)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(PICO_BOARD pico)

include(pico_sdk_import.cmake)
include(../../../external/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(${PROJECT_NAME} C CXX ASM)

pico_sdk_init()

add_subdirectory(../../../external/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src no-OS-FatFS-SD-SDIO-SPI-RPi-Pico)

set(TARGET_NAME ${PROJECT_NAME})

add_executable(${TARGET_NAME}
    main.cpp
    MultiIO.cpp
    MultiIODevice.cpp
    DiskCtrlDevice.cpp
    SDCard.cpp
)

pico_generate_pio_header(${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/bus_ctrl.pio)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    pico_enable_stdio_uart(${TARGET_NAME} 1)
endif()

target_compile_definitions(${TARGET_NAME} PRIVATE
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${TARGET_NAME} PRIVATE
    pico_stdlib
    pico_multicore
    hardware_gpio
    hardware_pio
    hardware_dma
    FreeRTOS-Kernel
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

pico_add_extra_outputs(${TARGET_NAME})

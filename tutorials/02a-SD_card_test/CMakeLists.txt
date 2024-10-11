cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME SDCardTest)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

include(pico_sdk_import.cmake)

pico_sdk_init()

add_subdirectory(../../external/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src no-OS-FatFS-SD-SDIO-SPI-RPi-Pico)

project(${PROJECT_NAME} C CXX ASM)

set(TARGET_NAME ${PROJECT_NAME})

add_executable(${TARGET_NAME}
	main.c
)

pico_enable_stdio_uart(${TARGET_NAME} 1)

target_compile_definitions(${TARGET_NAME} PRIVATE
)

target_include_directories(${TARGET_NAME} PRIVATE
)

target_link_libraries(${TARGET_NAME} PRIVATE
    pico_stdlib
    hardware_gpio
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
)

pico_add_extra_outputs(${TARGET_NAME})

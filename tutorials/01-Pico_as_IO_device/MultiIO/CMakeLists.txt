cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)

set (PROJECT_NAME MultiIO)
project(${PROJECT_NAME} C CXX ASM)

#set(CMAKE_EXE_LINKER_FLAGS "-specs=nano.specs -Wl,--no-warn-rwx-segment")

pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.c
    cpu_bus.c
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/bus_ctrl.pio)

#pico_enable_stdio_uart(${PROJECT_NAME} 1)
#pico_enable_stdio_usb(${PROJECT_NAME} 1)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib
    hardware_pio
    hardware_dma
)

pico_add_extra_outputs(${PROJECT_NAME})

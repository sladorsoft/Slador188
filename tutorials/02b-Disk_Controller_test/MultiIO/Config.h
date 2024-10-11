#pragma once

#include <hardware/pio.h>
#include <hardware/spi.h>

#include <FreeRTOS.h>
#include <task.h>

#include <cstdint>

#ifdef NDEBUG
#define DBG_PRINT(...)
#else
#include <stdio.h>
#include <pico/stdlib.h>
#define DBG_PRINT(...) printf(__VA_ARGS__)
#endif

namespace Config
{
    const UBaseType_t TASK_PRIORITY_IDLE   = (tskIDLE_PRIORITY + 0U);
    const UBaseType_t TASK_PRIORITY_LOW    = (tskIDLE_PRIORITY + 1U);
    const UBaseType_t TASK_PRIORITY_NORMAL = (tskIDLE_PRIORITY + 2U);
    const UBaseType_t TASK_PRIORITY_HIGH   = (tskIDLE_PRIORITY + 3U);

    const PIO BusCtrlPIO = pio0;
    const uint BusCtrlSM = 0;

    const uint BusCtrlWriteIRQ = PIO0_IRQ_0;
    const uint BusCtrlReadIRQ = PIO0_IRQ_1;
    const uint BusCtrlINTPin = 21;

    const uint LEDPin = 22;

    spi_inst_t* const SDCardSPI = spi0;
};
